
/* 
 * Peek system Emulation
 *
 * Copyright (c) 2010 cmw
 *
 */

#include "hw.h"
#include "peek.h"
#include "flash.h"
#include "console.h"
#include "devices.h"
#include "sysemu.h"
#include "loader.h"
#include "boards.h"
#include "arm-misc.h"
#include "qemu-char.h"
#include "qemu-timer.h"
#include "framebuffer.h"
#include "i2c.h"
#include "pc.h"

#define TRACE do { fprintf(stderr, "%s:%d:%s()\n", __FILE__, __LINE__, __FUNCTION__); } while (0)

static struct keymap map[0xE0] = {
    [0 ... 0xDF] = { -1, -1 },
    [0x1e] = {0,0}, /* a */
    [0x30] = {0,1}, /* b */
    [0x2e] = {0,2}, /* c */
    [0x20] = {0,3}, /* d */
    [0x12] = {0,4}, /* e */
    [0x21] = {0,5}, /* f */
    [0x22] = {1,0}, /* g */
    [0x23] = {1,1}, /* h */
    [0x17] = {1,2}, /* i */
    [0x24] = {1,3}, /* j */
    [0x25] = {1,4}, /* k */
    [0x26] = {1,5}, /* l */
    [0x32] = {2,0}, /* m */
    [0x31] = {2,1}, /* n */
    [0x18] = {2,2}, /* o */
    [0x19] = {2,3}, /* p */
    [0x10] = {2,4}, /* q */
    [0x13] = {2,5}, /* r */
    [0x1f] = {3,0}, /* s */
    [0x14] = {3,1}, /* t */
    [0x16] = {3,2}, /* u */
    [0x2f] = {3,3}, /* v */
    [0x11] = {3,4}, /* w */
    [0x2d] = {3,5}, /* x */
    [0x15] = {4,2}, /* y */
    [0x2c] = {4,3}, /* z */
    [0xc7] = {5,0}, /* Home */
    [0x2a] = {5,1}, /* shift */
    [0x39] = {5,2}, /* space */
    [0x39] = {5,3}, /* space */
    [0x1c] = {5,5}, /*  enter */
    [0xc8] = {6,0}, /* up */
    [0xd0] = {6,1}, /* down */
    [0xcb] = {6,2}, /* left */
    [0xcd] = {6,3}, /* right */
};

/* globals */
uint32_t g_debug = (//LOCO_DEBUG_INTH |
                    //LOCO_DEBUG_ULPD |
                    //LOCO_DEBUG_CLKM |
                    //LOCO_DEBUG_I2C |
                    //LOCO_DEBUG_MPU |
                    //LOCO_DEBUG_UART |
		    //LOCO_DEBUG_SOCKET |
                    //0xffffffff |
                    0);
uint32_t g_dlevel = 0;//LOCO_DLVL_WARN;
FILE *g_debug_fp = NULL;
struct locosto_intr_handler_s *g_inth; // debug only

/* prototypes */
void locosto_uart_reset(struct locosto_uart_s *s);
struct locosto_uart_s *locosto_uart_init(target_phys_addr_t base, qemu_irq irq, CharDriverState *chr);
static locosto_tpu_s *locosto_tpu_init(target_phys_addr_t base, target_phys_addr_t rambase, 
                                       qemu_irq irqf, qemu_irq irqp);
struct locosto_mpu_timer_s *locosto_mpu_timer_init(target_phys_addr_t base, qemu_irq irq, uint8 iswdt);
void locosto_inth_reset(struct locosto_intr_handler_s *s);
struct locosto_intr_handler_s *locosto_inth_init(target_phys_addr_t base,
						 unsigned long size, qemu_irq **pins,
						 qemu_irq parent_irq, qemu_irq parent_fiq);


uint32_t locosto_badwidth_read16(void *opaque, target_phys_addr_t addr)
{
    uint16_t ret;

    cpu_physical_memory_read(addr, (void *) &ret, 2);

    fprintf(stderr, "%s: 16-bit register %#08x (%p) = 0x%08x\n",
            __FUNCTION__, (unsigned int)addr, opaque, ret);

    return ret;
}

void locosto_badwidth_write16(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    uint16_t val16 = value;

    fprintf(stderr, "%s: 16-bit register %#08x (%p) = 0x%08x\n",
            __FUNCTION__, (unsigned int)addr, opaque, value);

    cpu_physical_memory_write(addr, (void *) &val16, 2);
}


/**************************************************************************/
/* MPU OS timers */
/**************************************************************************/

typedef struct clk {
    const char *name;
    const char *alias;
    struct clk *parent;
    struct clk *child1;
    struct clk *sibling;
    uint32_t flags;
    int id;

    int running;                /* Is currently ticking */
    int enabled;                /* Is enabled, regardless of its input clk */
    unsigned long rate;         /* Current rate (if .running) */
    unsigned int divisor;       /* Rate relative to input (if .enabled) */
    unsigned int multiplier;    /* Rate relative to input (if .enabled) */
    qemu_irq users[16];         /* Who to notify on change */
    int usecount;               /* Automatically idle when unused */
} locosto_clk;

struct locosto_mpu_timer_s {
    qemu_irq irq;
    locosto_clk clk;
    uint32_t val;
    int64_t time;
    QEMUTimer *timer;
    int64_t rate; // 
    int it_ena; // intr ena
    uint8 is_wdt;        // is this a watchdog timer?
    uint8 wdt_mode;      // 0 = watchdog timer, 1 = disable wd state 1 (got 0xf5), 2 = disable state 2 (disabled)

    int enable;	// ext timer clock en
    int ptv;	// prescaler
    int ar;	// auto-reload
    int st;	// start
    uint32_t reset_val;
};

static inline uint32_t locosto_timer_read(struct locosto_mpu_timer_s *timer)
{
    uint64_t distance = qemu_get_clock(vm_clock) - timer->time;

    if (timer->st && timer->enable && timer->rate)
        return timer->val - muldiv64(distance >> (timer->ptv + 1),
                                     timer->rate, get_ticks_per_sec());
    else
        return timer->val;
}

static inline void locosto_timer_sync(struct locosto_mpu_timer_s *timer)
{
    timer->val = locosto_timer_read(timer);
    timer->time = qemu_get_clock(vm_clock);
    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer sync called\n");
}

static inline void locosto_timer_update(struct locosto_mpu_timer_s *timer)
{
    int64_t expires;

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer update called() enable - 0x%02x timer ar - 0x%02x rate - %lld\n",
               timer->enable ,timer->ar , (long long int)timer->rate);
    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer reset val 0x%02x time %ld\n", timer->reset_val, timer->time);

    if (timer->enable && timer->st && timer->rate) {
        timer->val = timer->reset_val;  /* Should skip this on clk enable */
        expires = muldiv64((uint64_t) timer->val << (timer->ptv + 1),
                        get_ticks_per_sec(), timer->rate);
        qemu_mod_timer(timer->timer, timer->time + expires);
        LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR,
                   "timer expires time %ld\n", timer->time + expires);
    } else {
        qemu_del_timer(timer->timer);
    }
}

static void locosto_timer_fire(void *opaque)
{
    struct locosto_mpu_timer_s *timer = opaque;

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer fired\n");

    if (!timer->ar) {
        timer->val = 0;
        timer->st = 0;
    }

    if (timer->it_ena)
        /* Edge-triggered irq */
        qemu_irq_pulse(timer->irq);
}

static void locosto_timer_tick(void *opaque)
{
    struct locosto_mpu_timer_s *timer = (struct locosto_mpu_timer_s *) opaque;
    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer tick\n");

    locosto_timer_sync(timer);
    locosto_timer_fire(timer);
    locosto_timer_update(timer);
}

static uint32_t locosto_mpu_timer_read(void *opaque, target_phys_addr_t addr)
{
    struct locosto_mpu_timer_s *s = (struct locosto_mpu_timer_s *) opaque;

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "%s: read at %0x\n", __FUNCTION__, (int)addr);

    switch (addr) {
    case 0x00:  /* CNTL_TIMER */
        return (s->enable << 5) | (s->ptv << 2) | (s->ar << 1) | s->st;

    case 0x02:  /* LOAD_TIM */
        return s->reset_val;
        break;

    case 0x04:  /* READ_TIM */
        return locosto_timer_read(s);
    }

    LOCOSTO_BAD_REG(addr);
    return 0;
}

static void locosto_mpu_timer_write(void *opaque, target_phys_addr_t addr,
                                    uint32_t value)
{
    struct locosto_mpu_timer_s *s = (struct locosto_mpu_timer_s *) opaque;

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "%s: value %08x written at %0x\n", __FUNCTION__, value, (int)addr);

    switch (addr) {
    case 0x00:  /* CNTL_TIMER */
	LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "timer_write st = 0x%02x\n", (int)value & 1); 
        locosto_timer_sync(s);
        s->enable = (value >> 5) & 1;
        s->ptv = (value >> 2) & 7;
        s->ar = (value >> 1) & 1;
        s->st = value & 1;
	if (s->enable) LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_INFO, "timer enabled\n");
        locosto_timer_update(s);
        return;

    case 0x02:  /* LOAD_TIM */
        s->reset_val = value;
        return;

    case 0x04:  /* READ_TIM */
        LOCOSTO_RO_REG(addr);
        break;

    default:
        LOCOSTO_BAD_REG(addr);
    }
}

static CPUReadMemoryFunc *locosto_mpu_timer_readfn[] = {
    locosto_mpu_timer_read,
    locosto_mpu_timer_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *locosto_mpu_timer_writefn[] = {
    locosto_mpu_timer_write,
    locosto_mpu_timer_write,
    locosto_badwidth_write16,
};

static void locosto_mpu_timer_reset(struct locosto_mpu_timer_s *s)
{
    qemu_del_timer(s->timer); // unqueue any pending intrs

    s->enable = 0;
    s->reset_val = 0xffff;
    s->rate = 1300000;
    s->val = 0;
    s->ptv = 0;
    s->ar = 0;
    s->st = 0;
    s->it_ena = 1;
    s->wdt_mode = 0;
}

struct locosto_mpu_timer_s *locosto_mpu_timer_init(target_phys_addr_t base, qemu_irq irq, uint8 is_wdt)
{
    int iomemtype;
    struct locosto_mpu_timer_s *s = (struct locosto_mpu_timer_s *)
            qemu_mallocz(sizeof(struct locosto_mpu_timer_s));

    s->irq = irq;
    s->timer = qemu_new_timer(vm_clock, locosto_timer_tick, s);
    s->is_wdt = is_wdt;

    locosto_mpu_timer_reset(s);

    LOCOSTO_OPAQUE("timer", s);
    iomemtype = cpu_register_io_memory(locosto_mpu_timer_readfn, locosto_mpu_timer_writefn, s);
    cpu_register_physical_memory(base, 0x7FF, iomemtype);

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "Timer init done\n");

    return s;
}

/**************************************************************************/
/* UART Module */
/**************************************************************************/

void locosto_uart_reset(struct locosto_uart_s *s)
{
    s->syscontrol = 0;
}

static uint32_t locosto_uart_read(void *opaque, target_phys_addr_t addr)
{
    //struct locosto_uart_s *s = (struct locosto_uart_s *) opaque;
    //fprintf(stderr, "%s\n", __FUNCTION__);

    switch (addr) {
    /*
    case 0x10:  // MDR1 
        return s->mdr[0];
    case 0x12:  // MDR2 IRDA Only 
	return 0;
    case 0x40:  // SCR 
        return s->scr;
    case 0x44:  // SSR 
        return 0x0;
    case 0x4C:  // OSC_12M_SEL (LOCOSTO1) 
        //return s->clksel;
    case 0x14:  // TXFLL IRDA Only 
    case 0x16:  // TXFLH IRDA Only 
    case 0x18:  // SFREGL IRDA Only 
    case 0x1A:  // SFREGH IRDA Only 
    case 0x1C:  // BLR IRDA Only  
    case 0x1E:  // ACREG IRDA Only 
    case 0x24:  // EBLR IRDA Only 
	return 0;
    case 0x28:  // MVR 
        return 0x30;
    case 0x2A:  // SYSC
        return s->syscontrol;  
    */
    case 0x16*2:  // SYSS 
        LOCO_DEBUG(LOCO_DEBUG_UART, LOCO_DLVL_INFO, "%s: SYSS read\n", __FUNCTION__);
        return 1;
    case 0x17*2:  // WER 
	break;
        //return s->wkup;
    }
    LOCO_DEBUG(LOCO_DEBUG_UART, LOCO_DLVL_INFO, "%s: value 0 read @ %08x(%x)\n",
               __FUNCTION__, (int)addr, (int)addr/2);
    return 0;
}

static void locosto_uart_write(void *opaque, target_phys_addr_t addr,
                               uint32_t value)
{
    struct locosto_uart_s *s = (struct locosto_uart_s *) opaque;

    LOCO_DEBUG(LOCO_DEBUG_UART, LOCO_DLVL_INFO, "%s: value %08x written at %0x(%x)\n",
               __FUNCTION__, value, (int)addr, (int)addr/2);
return;

    switch (addr) {
    case 0x8*2:  // MDR1
        s->mdr[0] = value & 0x7f;
        break;
    case 0x9*2:  // MDR2
        //s->mdr[1] = value & 0xff;
        break;
    case 0x10*2:  // SCR
        s->scr = value & 0xff;
        break;
    case 0x12*2:  // EBLR
        //s->eblr = value & 0xff;
        break;
    case 0x15*2:  // SYSC 
        s->syscontrol = value & 0x1d;
        if (value & 2)
            locosto_uart_reset(s);
        break;
    case 0x16*2:  // SYSS
        LOCOSTO_RO_REG(addr);
        break;
    default:
       LOCO_DEBUG(LOCO_DEBUG_UART, LOCO_DLVL_INFO, "%s: value %08x written at %0x\n",
                  __FUNCTION__, value, (int)addr);
       LOCO_BREAK;
       return;
    }
}
static CPUReadMemoryFunc *uart_readfn[] = {
    locosto_uart_read,
    locosto_uart_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *uart_writefn[] = {
    locosto_uart_write,
    locosto_uart_write,
    locosto_badwidth_write16,
};

struct locosto_uart_s *locosto_uart_init(target_phys_addr_t base, qemu_irq irq, CharDriverState *chr)
{
    struct locosto_uart_s *s = (struct locosto_uart_s *) qemu_mallocz(sizeof(struct locosto_uart_s));

    int io = cpu_register_io_memory(uart_readfn, uart_writefn, s);
    LOCOSTO_OPAQUE("uart", s);

    cpu_register_physical_memory(base, 0x100, io);

    s->irq = irq;
    s->serial = serial_mm_init(base, 1, irq, 1300000, chr ?: qemu_chr_open("null", "null", NULL), 1);

    return s;
}


/**************************************************************************/
/* LCD Module */
/**************************************************************************/

#include "pixel_ops.h"
#define draw_line_func drawfn

#define DEPTH 8
#include "omap_lcd_template.h"
#define DEPTH 15
#include "omap_lcd_template.h"
#define DEPTH 16
#include "omap_lcd_template.h"
#define DEPTH 32
#include "omap_lcd_template.h"

static draw_line_func draw_line_table2[33] = {
    [0 ... 32]	= 0,
    [8]		= draw_line2_8,
    [15]	= draw_line2_15,
    [16]	= draw_line2_16,
    [32]	= draw_line2_32,
}, draw_line_table4[33] = {
    [0 ... 32]	= 0,
    [8]		= draw_line4_8,
    [15]	= draw_line4_15,
    [16]	= draw_line4_16,
    [32]	= draw_line4_32,
}, draw_line_table8[33] = {
    [0 ... 32]	= 0,
    [8]		= draw_line8_8,
    [15]	= draw_line8_15,
    [16]	= draw_line8_16,
    [32]	= draw_line8_32,
}, draw_line_table12[33] = {
    [0 ... 32]	= 0,
    [8]		= draw_line12_8,
    [15]	= draw_line12_15,
    [16]	= draw_line12_16,
    [32]	= draw_line12_32,
}, draw_line_table16[33] = {
    [0 ... 32]	= 0,
    [8]		= draw_line16_8,
    [15]	= draw_line16_15,
    [16]	= draw_line16_16,
    [32]	= draw_line16_32,
};

static void locosto_lcd_update_display(void *opaque)
{
    locosto_lcd_s *lcd = (locosto_lcd_s *) opaque;
    draw_line_func draw_line;
    int src_width, dest_width;
    int height, first, last;
    int width, linesize, bpp;
    target_phys_addr_t frame_base;

    if (!lcd || !ds_get_bits_per_pixel(lcd->ds))
        return;

    switch (ds_get_bits_per_pixel(lcd->ds)) {
    case 8:
        dest_width = 1;
        break;
    case 15:
        dest_width = 2;
        break;
    case 16:
        dest_width = 2;
        break;
    case 24:
        dest_width = 3;
        break;
    case 32:
        dest_width = 4;
        break;
    default:
        fprintf(stderr, "%s: Bad color depth\n", __FUNCTION__);
        exit(1);
    }

    //LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: i:%d\n", __FUNCTION__, lcd->invalidate);

    /* Colour depth */
    switch (4) {
    case 1:
        draw_line = draw_line_table2[ds_get_bits_per_pixel(lcd->ds)];
        bpp = 2;
        break;

    case 2:
        draw_line = draw_line_table4[ds_get_bits_per_pixel(lcd->ds)];
        bpp = 4;
        break;

    case 3:
        draw_line = draw_line_table8[ds_get_bits_per_pixel(lcd->ds)];
        bpp = 8;
        break;

    case 4 ... 7:
        draw_line = draw_line_table16[ds_get_bits_per_pixel(lcd->ds)];
        bpp = 16;
        break;

    default:
        LOCO_BREAK;
        /* Unsupported at the moment.  */
        return;
    }

    /* Resolution */
    first = last = 0;
    width = 320;
    height = 240;
    lcd->invalidate = 1;

    /* Content */
    //frame_base = 0x007c3750;
    //frame_base = 0x007c4250;
    frame_base =0x007288f0;

    if (!ds_get_bits_per_pixel(lcd->ds))
        return;

    src_width = width * bpp >> 3;
    linesize = ds_get_linesize(lcd->ds);
#if 1

    framebuffer_update_display(lcd->ds, frame_base,
                               width, height,
                               src_width,       /* Length of source line, in bytes.  */
                               linesize,        /* Bytes between adjacent horizontal output pixels.  */
                               dest_width,      /* Bytes between adjacent vertical output pixels.  */
                               lcd->invalidate,
                               draw_line, lcd->palette,
                               &first, &last);
    if (first >= 0) {
        //fprintf(stderr, "dpy_update()\n");
        dpy_update(lcd->ds, 0, 0, width, height);
    }
#endif
    lcd->invalidate = 0;
}

static void locosto_lcd_invalidate_display(void *opaque) {
    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s:\n", __FUNCTION__);
    locosto_lcd_s *locosto_lcd = opaque;
    locosto_lcd->invalidate = 1;
}

static void locosto_lcd_screen_dump(void *opaque, const char *filename) {
    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s:\n", __FUNCTION__);

    locosto_lcd_update_display(opaque);
    //if (omap_lcd && ds_get_data(omap_lcd->state))
    //    ppm_save(filename, ds_get_data(omap_lcd->state),
    //            omap_lcd->width, omap_lcd->height,
    //            ds_get_linesize(omap_lcd->state));
}

static void locosto_lcd_reset(locosto_lcd_s *lcd)
{
    int i;
    lcd->lcd_ctrl =
        (3<<12) |       // min frame size
        (1<<11) |       // suspend_en
        1;              // soft_nreset
    lcd->invalidate = 1;
    for (i = 0; i < 256; i++)
        lcd->palette[i] = i << 8 | i;
}

static uint32_t lcd_read(void *opaque, target_phys_addr_t offset)
{
    locosto_lcd_s *s = (locosto_lcd_s *)opaque;
    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: offset=0x%02X %x\n", __FUNCTION__, (int)offset,
               s->lcd_ctrl);

    switch (offset) {
    case 0x0:
        return s->lcd_ctrl;
    default:
        
    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: UNMAPPED_OFFSET = 0x%02X\n", __FUNCTION__, (int)offset);
    }
    return 0;
}

static void lcd_write(void *opaque, target_phys_addr_t offset, uint32_t value)
{
    locosto_lcd_s *s = (locosto_lcd_s *)opaque;
    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: offset=0x%02X value=0x%02X\n",
               __FUNCTION__, (int)offset, value);

    switch (offset) {
    case 0x0:
        s->lcd_ctrl = value | 1;
        if (!(value & 1))
            locosto_lcd_reset(s);
        break;
    default:

    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: UNMAPPED_OFFSET = 0x%02X\n", __FUNCTION__, (int)offset);
    }
}

static CPUReadMemoryFunc *lcd_readfn[] = {
    locosto_badwidth_read16,
    lcd_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *lcd_writefn[] = {
    locosto_badwidth_write16,
    lcd_write,
    locosto_badwidth_write16,
};

static locosto_lcd_s * locosto_lcd_init(target_phys_addr_t base, qemu_irq irq)
{
    locosto_lcd_s *lcd = qemu_mallocz(sizeof(locosto_lcd_s));
    int io;

    LOCO_DEBUG(LOCO_DEBUG_LCD, LOCO_DLVL_ERR, "%s: \n", __FUNCTION__);

    io = cpu_register_io_memory(lcd_readfn, lcd_writefn, lcd);
    LOCOSTO_OPAQUE("lcd", lcd);
    cpu_register_physical_memory(base, 0x7FF, io);

    locosto_lcd_reset(lcd);

    lcd->ds = graphic_console_init(locosto_lcd_update_display,
                                   locosto_lcd_invalidate_display,
                                   locosto_lcd_screen_dump, NULL, lcd);
    qemu_console_resize(lcd->ds, 320, 240);

    return lcd;
}

/**************************************************************************/
/* EMIF Config */
/**************************************************************************/

static void emif_reset(peek_state_s *s)
{
}

static uint32_t emif_read(void *opaque, target_phys_addr_t offset)
{
    //peek_state_s *s = (peek_state_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_INFO, "%s: offset %08x\n", __FUNCTION__, (int)offset);

    switch (offset) {
    case 0x0: // 
        return 0;
    default:
        LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X\n", __FUNCTION__, (int)offset);
    }
    return 0;
}

static void emif_write(void *opaque, target_phys_addr_t offset,
                       uint32_t value)
{
    //peek_state_s *s = (peek_state_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_INFO, "%s: offset=0x%02X value=0x%02X\n", __FUNCTION__, (int)offset, value);

    switch (offset) {
    case 0x04: // 
    case 0x08: // 
    case 0x0C: // 
    case 0x10: // 
        LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_INFO, "%s: CSCONF1 (%d) value=0x%04X\n",
                   __FUNCTION__, ((int)offset >> 2) & 0x7, value);
        return;
    case 0x14: // 
    case 0x16: // 
    case 0x18: // 
    case 0x1a: // 
        LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_INFO, "%s: CSCONF2 (%d) value=0x%04X\n",
                   __FUNCTION__, ((int)offset >> 3) & 0x7, value);
        return;
    default:
        LOCO_DEBUG(LOCO_DEBUG_EMIF, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X and value=0x%08X\n",
                   __FUNCTION__, (int)offset, value);
    }

}

static CPUReadMemoryFunc *emif_readfn[] = {
    locosto_badwidth_read16,
    emif_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *emif_writefn[] = {
    locosto_badwidth_write16,
    emif_write,
    locosto_badwidth_write16,
};

static void locosto_emif_init(peek_state_s *s)
{
    int io;
    LOCOSTO_OPAQUE("emif", s);
    io = cpu_register_io_memory(emif_readfn, emif_writefn, s);
    cpu_register_physical_memory(EMIF_BASE, 0x7FF, io);
    emif_reset(s);
}

/**************************************************************************/
/* GPIO Config */
/**************************************************************************/
/* General-Purpose I/O */

static void locosto_gpio_set(void *opaque, int line, int level)
{
    locosto_gpio_s *s = (locosto_gpio_s *) opaque;
    uint16_t prev = s->inputs;

    if (level)
        s->inputs |= 1 << line;
    else
        s->inputs &= ~(1 << line);

    if (((s->edge & s->inputs & ~prev) | (~s->edge & ~s->inputs & prev)) &
                    (1 << line) & s->dir & ~s->mask) {
        s->ints |= 1 << line;
        qemu_irq_raise(s->irq);
    }
}

static uint32_t locosto_gpio_read(void *opaque, target_phys_addr_t addr)
{
    locosto_gpio_s *s = (locosto_gpio_s *) opaque;
    int offset = addr & 0xff;
    uint16_t rval = 0;

    switch (offset) {
    case 0x00:	/* DATA_INPUT */
        rval = s->inputs & s->pins;
        break;

    case 0x02:	/* DATA_OUTPUT */
        rval = s->outputs;
        break;

    case 0x04:	/* DIRECTION_CONTROL */
        rval = s->dir;
        break;

    case 0x06:	/* CNTL */
        rval = s->cntl;
        break;

    case 0x0e:	/* BUZZER_LIGHT */
        rval = 0;
        break;

    case 0x16:	/* INTERRUPT_CONTROL */
        rval = s->edge;
        break;

    case 0x18:	/* INTERRUPT_MASK */
        rval = s->mask;
        break;

    case 0x1e:	/* INTERRUPT_STATUS */
        rval = s->ints;
        break;

    case 0x20:	/* PULL_DIR */
        rval = s->pull;
        break;

    case 0x22: /* INTR SOFT CLEAR */
        LOCOSTO_BAD_REG(addr);
        break;

    default:
        LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                   "%s: %d UNMAPPED_OFFSET = 0x%08X\n",
                   __FUNCTION__, s->idx, (int)offset);
        LOCOSTO_BAD_REG(addr);
        break;
    }

    LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_INFO, "%s: %d addr %08x = 0x%04x\n",
               __FUNCTION__, s->idx, (int)addr, rval);

    return rval;
}

static void locosto_gpio_write(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    locosto_gpio_s *s = (locosto_gpio_s *) opaque;
    int offset = addr & 0xff;
    uint16_t diff;
    int ln;

    LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_INFO, "%s: %d addr 0x%02x = 0x%04x\n",
               __FUNCTION__, s->idx, (int)addr, value);

    switch (offset) {
    case 0x00:	/* DATA_INPUT */
        LOCOSTO_RO_REG(addr);
        return;

    case 0x02:	/* DATA_OUTPUT */
        diff = (s->outputs ^ value) & ~s->dir;
        s->outputs = value;
        while ((ln = ffs(diff))) {
            ln --;
            if (s->handler[ln])
                qemu_set_irq(s->handler[ln], (value >> ln) & 1);
            diff &= ~(1 << ln);
        }
        break;

    case 0x04:	/* DIRECTION_CONTROL */
        diff = s->outputs & (s->dir ^ value);
        s->dir = value;

        value = s->outputs & ~s->dir;
        while ((ln = ffs(diff))) {
            ln --;
            if (s->handler[ln])
                qemu_set_irq(s->handler[ln], (value >> ln) & 1);
            diff &= ~(1 << ln);
        }
        break;

    case 0x6:	/* GPIO_CNTL */
        LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                   "%s: %d GPIO CLOCK %s\n", __FUNCTION__, s->idx,
                   (value & (1<<5)) ? "enabled" : "disabled");
        s->cntl = value;
        break;

    case 0x8:	/* LOAD_TIME_REG */
        LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                   "%s: %d buzzzer time 0x%04x\n", __FUNCTION__, s->idx, value);
        break;

    case 0xa:	/* SOFT_SET */
        diff = ~s->outputs & value & ~s->dir;
        s->outputs |= value;

        while ((ln = ffs(diff))) {
            ln --;
            if (s->handler[ln])
                qemu_set_irq(s->handler[ln], (s->outputs >> ln) & 1);
            diff &= ~(1 << ln);
        }
        break;

    case 0xc:	/* SOFT_CLEAR */
        diff = s->outputs & value & ~s->dir;
        s->outputs &= ~value;

        while ((ln = ffs(diff))) {
            ln --;
            if (s->handler[ln])
                qemu_set_irq(s->handler[ln], (s->outputs >> ln) & 1);
            diff &= ~(1 << ln);
        }
        break;

    case 0xe:	/* BUZZER_LIGHT */
        if (value & 1)
            LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                       "%s: %d buzzzer started\n", __FUNCTION__, s->idx);
        else
            LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                       "%s: %d buzzzer stopped\n", __FUNCTION__, s->idx);
        if (value & 2)
            LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                       "%s: %d light started\n", __FUNCTION__, s->idx);
        else
            LOCO_DEBUG(LOCO_DEBUG_GPIO, LOCO_DLVL_ERR,
                       "%s: %d light stopped\n", __FUNCTION__, s->idx);
        break;

    case 0x10:	/* LIGHT_LEVEL */
        LOCO_BREAK;
        break;

    case 0x12:	/* BUZZER_LEVEL - power 0-63*/
        s->buzzer_power = value;
        break;

    case 0x14:	/* JOGDIAL_MODE */
        LOCO_BREAK;
        break;

    case 0x16:	/* INTERRUPT_CONTROL */
        s->edge = value;
        break;

    case 0x18:	/* INTERRUPT_MASK */
        s->mask = value;
        break;

    case 0x1a:	/* JOGDIAL_DEBOUNCE */
        LOCO_BREAK;
        break;

    case 0x1c:	/* JOGDIAL_DIR */
        LOCO_BREAK;
        break;

    case 0x1e:	/* INTERRUPT_STATUS */
        s->ints &= ~value;
        if (!s->ints)
            qemu_irq_lower(s->irq);
        break;

    case 0x20:	/* PULL_DIR */
        s->pull = value;
        break;

    case 0x22:	/* SOFT CLEAR INTR STATUS */
        s->ints &= ~value;
        break;

    default:
        LOCOSTO_BAD_REG(addr);
        return;
    }
}

static void locosto_gpio_reset(locosto_gpio_s *s)
{
    s->inputs = 0;
    s->outputs = ~0;
    s->dir = ~0;
    s->edge = ~0;
    s->mask = ~0;
    s->ints = 0;
    s->pins = ~0;
    s->cntl = 0;
    s->buzzer_power = 63;
}

/* *Some* sources say the memory region is 32-bit.  */
static CPUReadMemoryFunc * const locosto_gpio_readfn[] = {
    locosto_badwidth_read16,
    locosto_gpio_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc * const locosto_gpio_writefn[] = {
    locosto_badwidth_write16,
    locosto_gpio_write,
    locosto_badwidth_write16,
};

static locosto_gpio_s *locosto_gpio_init(target_phys_addr_t base,
                                         qemu_irq irq, uint8 idx)
{
    int iomemtype;
    locosto_gpio_s *s = (locosto_gpio_s *)
        qemu_mallocz(sizeof(locosto_gpio_s));

    s->irq = irq;
    s->idx = idx;
    s->in = qemu_allocate_irqs(locosto_gpio_set, s, 16);
    locosto_gpio_reset(s);

    iomemtype = cpu_register_io_memory(locosto_gpio_readfn,
                    locosto_gpio_writefn, s);
    cpu_register_physical_memory(base, 0x7ff, iomemtype);
    LOCOSTO_OPAQUE("gpio", s);

    return s;
}

qemu_irq *locosto_gpio_in_get(locosto_gpio_s *s);
qemu_irq *locosto_gpio_in_get(locosto_gpio_s *s)
{
    return s->in;
}

void locosto_gpio_out_set(locosto_gpio_s *s, int line, qemu_irq handler);
void locosto_gpio_out_set(locosto_gpio_s *s, int line, qemu_irq handler)
{
    if (line >= 16 || line < 0)
        hw_error("%s: %d No GPIO line %i\n", __FUNCTION__, s->idx, line);
    s->handler[line] = handler;
}


/**************************************************************************/
/* CLOCK Module */
/**************************************************************************/

static void clk_reset(peek_state_s *s)
{
    s->dpll_clkout = 1300000;
    s->dpll_ctrl = 1<<13;
    s->clkm.cntl_arm_clk = 1 | (1<<12);
    s->clkm.cntl_clk = 1 | (1<<4) | (1<<14);
    s->clkm.cntl_rst = 6;
}

static uint32_t clkm_read(void *opaque, target_phys_addr_t offset)
{
    peek_state_s *s = (peek_state_s *)opaque;
    offset &= 0xff;
    LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_INFO, "%s: offset %08x\n", __FUNCTION__, (int)offset);
    switch (offset) {
    case 0x0: // CNTL_ARM_CLK
        return s->clkm.cntl_arm_clk;
    case 0x2: // CNTL_CLK
        return s->clkm.cntl_clk;
    case 0x4: // CNTL_RST
        return s->clkm.cntl_rst;
    case 0xc: // CNTL_CLK_USB
        return 0x2; 
    case 0x10: // CNTL_APLL_DIV_CLK
        return s->clkm.apll_div_clk;
    case 0xe: // CNTL_CLK_PROG_FREE
        return s->clkm.clk_prog_free;
    default:
        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X\n",
                   __FUNCTION__, (int)offset);
        //LOCO_BREAK;
    }
    return 0;
}

static void clkm_write(void *opaque, target_phys_addr_t offset,
                       uint32_t value)
{
    peek_state_s *s = (peek_state_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_INFO, "%s: offset=0x%02X value=0x%08X\n", __FUNCTION__, (int)offset, value);

    offset &= 0xff;

    switch (offset) {
    case 0x0: // CNTL_ARM_CLK
        s->clkm.cntl_arm_clk = value;
        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_INFO,
                   "%s: MCU clock src is %s\n", __FUNCTION__,
                   value & 0x2 ? (value & 4 ? "clkin" : "dxco") : "dpll");
        if (!(s->clkm.cntl_arm_clk & 1)) {
            LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_WARN, 
                       "%s: arm requests to go to BIG sleep through CLKM!! (%04x)\n",
                       __FUNCTION__, g_inth->bank.mask);
            /* Suspend */
            cpu_interrupt(cpu_single_env, CPU_INTERRUPT_HALT);
        }
        if (!(s->clkm.cntl_arm_clk & (1<<12))) {
            LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_WARN, 
                       "%s: arm requests to go to DEEP sleep through CLKM!!\n", __FUNCTION__);
            /* Suspend */
            cpu_interrupt(cpu_single_env, CPU_INTERRUPT_HALT);
        }
        return;
    case 0x2: // CNTL_CLK
        s->clkm.cntl_clk = value;
        return;
    case 0x4: // CNTL_RST
        {
            uint8 changed = s->clkm.cntl_rst ^ value;
            if (changed & 0x2)
                if (value & 0x2)
                    LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                               "%s: request to take DSP out of reset\n",
                               __FUNCTION__);
            if (changed & 4)
                LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                           "%s: external reset set to %d\n",
                           __FUNCTION__, 0 != (value & 0x4));
            if (changed & 8 && value & 8) {
                LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                           "%s: arm requests wd reset!\n",
                           __FUNCTION__);
                //LOCO_BREAK;
            }
        }
        return;
    case 0xc: // CNTL_CLK_USB
        return;
    case 0x10: // CNTL_APLL_DIV_CLK
        s->clkm.apll_div_clk = value;
        return;
    case 0xe: // CNTL_CLK_PROG_FREE
        s->clkm.clk_prog_free = value;
        return;
    default:
        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X and value=0x%08X\n",
                   __FUNCTION__, (int)offset, value);
        //LOCO_BREAK;
    }

}

static CPUReadMemoryFunc *clkm_readfn[] = {
    clkm_read,
    clkm_read,
    locosto_badwidth_read16, // xxx - this still causes a badwidth read at 0x104
};

static CPUWriteMemoryFunc *clkm_writefn[] = {
    clkm_write,
    clkm_write,
    locosto_badwidth_write16,
};

/**************************************************************************/
/* DPLL Module */
/**************************************************************************/

static uint32_t dpll_read(void *opaque, target_phys_addr_t offset)
{
    peek_state_s *s = (peek_state_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_INFO,
               "%s: offset=0x%02X value=0x%02X\n",
               __FUNCTION__, (int)offset, s->dpll_ctrl);

    switch (offset) {   
    case 0x0: // CTRL
        return s->dpll_ctrl;
    default:
        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR, 
                   "%s: UNMAPPED_OFFSET = 0x%08X\n", __FUNCTION__, (int)offset);
    }
    return 0;
}

static void dpll_write(void *opaque, target_phys_addr_t offset,
                       uint32_t value)
{
    peek_state_s *s = (peek_state_s *)opaque;
    uint32_t freq = 1300000; // ref freq - should be CKM output

    LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_INFO,
               "%s: offset=0x%02X value=0x%02x->0x%02X\n",
               __FUNCTION__, (int)offset, s->dpll_ctrl, value);

    switch (offset) {
    case 0x0: // CTRL

        if (value & DPLL_CTRL_PLLEN) {
            // pll in lock mode
            freq = (freq * DPLL_CTRL_PLLMULT(value)) / DPLL_CTRL_PLLDIV(value);
            s->dpll_ctrl = value | DPLL_CTRL_LOCK;
        } else {
            // pll in bypass mode
            freq /= DPLL_CTRL_BYPASSDIV(value);
            s->dpll_ctrl = value & ~DPLL_CTRL_LOCK;
        }

        s->dpll_clkout = freq;

        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_WARN,
                   "%s: DPLL_CLKOUT set to %d MHz\n", __FUNCTION__, freq);
        break;
    default:
        LOCO_DEBUG(LOCO_DEBUG_CLKM, LOCO_DLVL_ERR,
                   "%s: UNMAPPED_OFFSET = 0x%08X and value=0x%08X\n",
                   __FUNCTION__, (int)offset, value);
    }
}

static CPUReadMemoryFunc *dpll_readfn[] = {
    locosto_badwidth_read16,
    dpll_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *dpll_writefn[] = {
    locosto_badwidth_write16,
    dpll_write,
    locosto_badwidth_write16,
};

static void locosto_dpll_init(peek_state_s *s)
{
    int io;
    io = cpu_register_io_memory(dpll_readfn, dpll_writefn, s);
    LOCOSTO_OPAQUE("dpll", s);
    cpu_register_physical_memory(DPLL_BASE, 0x7FF, io);
    s->dpll_ctrl = 1 << 13;
}

/**************************************************************************/
/* TPU Module */
/**************************************************************************/
/* from osmocom */
enum tpu_reg_arm {
	TPU_CTRL	= 0x0,	/* Control & Status Register */
	INT_CTRL	= 0x2,	/* Interrupt Control Register */
	INT_STAT	= 0x4,	/* Interrupt Status Register */
	TPU_OFFSET	= 0xC,	/* Offset operand value register */
	TPU_SYNCHRO	= 0xE,	/* synchro operand value register */
	IT_DSP_PG	= 0x20,
};

enum tpu_ctrl_bits {
	TPU_CTRL_RESET		= (1 << 0),
	TPU_CTRL_PAGE		= (1 << 1),
	TPU_CTRL_EN		= (1 << 2),
	/* unused */
	TPU_CTRL_DSP_EN		= (1 << 4),
	/* unused */
	TPU_CTRL_MCU_RAM_ACC	= (1 << 6),
	TPU_CTRL_TSP_RESET	= (1 << 7),
	TPU_CTRL_IDLE		= (1 << 8),
	TPU_CTRL_WAIT		= (1 << 9),
	TPU_CTRL_CK_ENABLE	= (1 << 10),
	TPU_CTRL_FULL_WRITE	= (1 << 11),
};

static void tpu_update(locosto_tpu_s *tpu)
{
}

static void locosto_tpu_tick(void *opaque)
{
    locosto_tpu_s *tpu = (locosto_tpu_s *)opaque;
    uint64_t nexttime;

    qemu_irq_pulse(tpu->irq_frame);

    //fprintf(stderr, "TPU_TICK\n");

    nexttime = qemu_get_clock(vm_clock) + get_ticks_per_sec() / 100;

    if ((tpu->tpu_ctrl & TPU_CTRL_EN) &&
        (tpu->tpu_ctrl & TPU_CTRL_CK_ENABLE)) {
        qemu_mod_timer(tpu->timer, nexttime);

        LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_WARN, "%s: tpu next intr @ %ld\n",
                   __FUNCTION__, qemu_get_clock(vm_clock) + get_ticks_per_sec() / 100);
    } else {
        // //qemu_del_timer(tpu->timer);
    }
}

static void tpu_reset(locosto_tpu_s *tpu)
{
    tpu->tpu_ctrl = (1<<7) | 1;
    tpu_update(tpu);
    //qemu_del_timer(tpu->timer);

    LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "%s: \n", __FUNCTION__);
}

static void tsp_reset(locosto_tpu_s *tpu)
{
    LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "%s: \n", __FUNCTION__);
}

static uint32_t tpu_read(void *opaque, target_phys_addr_t offset)
{
    locosto_tpu_s *tpu = (locosto_tpu_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "%s: OFFSET = 0x%08X  %04x\n",
               __FUNCTION__, (int)offset, tpu->tpu_ctrl);

    switch (offset) {
    case 0x0:
        return tpu->tpu_ctrl;
    default:
        LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_ERR, "%s: UNMAPPED_OFFSET = 0x%08X\n", __FUNCTION__, (int)offset);
        //LOCO_BREAK;
	return 0;
    }
    
}

static void tpu_write(void *opaque, target_phys_addr_t offset, uint32_t value)
{
    locosto_tpu_s *tpu = (locosto_tpu_s *)opaque;

    LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "%s: offset=0x%02X value=0x%02X\n", __FUNCTION__, (int)offset, value);
    switch (offset) {
    case 0x00: // TPU_CTRL
        if (value & TPU_CTRL_RESET) {
            tpu_reset(tpu);
        } else {
            if (value & TPU_CTRL_PAGE)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "page ");
            if (value & TPU_CTRL_EN)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "en ");
            if (value & TPU_CTRL_MCU_RAM_ACC)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "ram ");
            if (value & TPU_CTRL_IDLE)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "idle ");
            if (value & TPU_CTRL_WAIT)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "wait ");
            if (value & TPU_CTRL_CK_ENABLE)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "cken ");
            if (value & TPU_CTRL_FULL_WRITE)
                LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "full ");
            LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_INFO, "\n");

            if ((value & TPU_CTRL_EN) && (value & TPU_CTRL_CK_ENABLE)) {
                qemu_mod_timer(tpu->timer, qemu_get_clock(vm_clock) + get_ticks_per_sec() / 1); // 10ms                
            }
        }
        if (value & TPU_CTRL_TSP_RESET) {
            tsp_reset(tpu);
        }
        tpu->tpu_ctrl = value;
        break;
    default:
        LOCO_DEBUG(LOCO_DEBUG_TPU, LOCO_DLVL_ERR, "%s: UNMAPPED_OFFSET = 0x%08X\n", __FUNCTION__, (int)offset);
        //LOCO_BREAK;
    }
}

static CPUReadMemoryFunc *tpu_readfn[] = {
    locosto_badwidth_read16,
    tpu_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *tpu_writefn[] = {
    locosto_badwidth_write16,
    tpu_write,
    locosto_badwidth_write16,
};

static locosto_tpu_s *locosto_tpu_init(target_phys_addr_t tpu_base,
                                       target_phys_addr_t tpuram_base,
                                       qemu_irq irq_frame,
                                       qemu_irq irq_page)
{
    int io;

    locosto_tpu_s *tpu = qemu_mallocz(sizeof(locosto_tpu_s));

    /* Save for later */
    tpu->irq_frame = irq_frame;
    tpu->irq_page = irq_page;

    /* Map the control registers */
    io = cpu_register_io_memory(tpu_readfn, tpu_writefn, tpu);
    LOCOSTO_OPAQUE("tpu", tpu);
    cpu_register_physical_memory(tpu_base, 0x7FF, io);

    /* Map 0x800 of internal TPU RAM */
    cpu_register_physical_memory(tpuram_base, 0x7ff, qemu_ram_alloc(0x7ff));

    tpu->timer = qemu_new_timer(vm_clock, locosto_tpu_tick, tpu);

    tpu_reset(tpu);
    tsp_reset(tpu);

    return tpu;
}

/**************************************************************************/
/* INTH Module */
/**************************************************************************/

static void locosto_inth_sir_update(struct locosto_intr_handler_s *s, int is_fiq)
{
    int i, sir_intr, p_intr, p, f;
    struct locosto_intr_handler_bank_s *bank = &s->bank;
    uint32_t level;
    sir_intr = 0;
    p_intr = 255;

    /* Find the interrupt line with the highest dynamic priority.
     * Note: 0 denotes the hightest priority.
     * If all interrupts have the same priority, the default order is IRQ_N,
     * IRQ_N-1,...,IRQ_0. */
    level = bank->irqs & ~bank->mask & (is_fiq ? bank->fiq : ~bank->fiq);
    for (f = ffs(level), i = f - 1, level >>= f - 1; f; i += f,
             level >>= f) {
        p = bank->priority[i];
        if (p <= p_intr) {
            p_intr = p;
            sir_intr = i;
        }
        f = ffs(level >> 1);
    }
    s->sir_intr[is_fiq] = sir_intr;
}

static inline void locosto_inth_update(struct locosto_intr_handler_s *s, int is_fiq)
{
    struct locosto_intr_handler_bank_s *bank = &s->bank;
    uint32_t has_intr = 0;

    LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO,
               "locosto_inth_update is_fiq 0x%02x bank->irqs 0x%08x ~bank->mask 0x%08x\n",
               is_fiq, bank->irqs, ~bank->mask);

    has_intr |= bank->irqs & ~bank->mask & (is_fiq ? bank->fiq : ~bank->fiq);

    if (s->new_agr[is_fiq] & has_intr) {
        s->new_agr[is_fiq] = 0;
        locosto_inth_sir_update(s, is_fiq);
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_WARN, 
                   "locosto_inth_update qemu_set 0x%08x %d\n",
                   has_intr, s->sir_intr[is_fiq]);
        qemu_set_irq(s->parent_intr[is_fiq], 1);
    }
}

#define INT_FALLING_EDGE        (0)
#define INT_IS_EDGE(x)          (((bank->sens_edge >> (x)) & 1) == INT_FALLING_EDGE)

static void locosto_inth_set_intr(void *opaque, int irq, int req)
{
    struct locosto_intr_handler_s *ih = (struct locosto_intr_handler_s *) opaque;
    uint32_t rise;

    struct locosto_intr_handler_bank_s *bank = &ih->bank;
    int n = irq & 31;

    LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO,
               "*** Set Interrupt called irq - %d req - 0x%08x ***\n", n, req);

    if (req) {
        // set
        rise = ~bank->irqs & (1 << n);
        if (INT_IS_EDGE(n))
            rise &= ~bank->inputs;

        bank->inputs |= (1 << n);
        if (rise) {
            bank->irqs |= rise;
            locosto_inth_update(ih, 0);
            locosto_inth_update(ih, 1);
        }
    } else {
        // clear
        rise = bank->irqs & (1 << n) & (INT_FALLING_EDGE ? ~bank->sens_edge : bank->sens_edge);
        bank->irqs &= ~rise;
        bank->inputs &= ~(1 << n);
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO,
                   "*** rise - 0x%08x irqs - 0x%08x inputs - 0x%08x\n",
                   rise, bank->irqs, bank->inputs);
    }
}

static uint32_t locosto_inth_read(void *opaque, target_phys_addr_t addr)
{
    struct locosto_intr_handler_s *s = (struct locosto_intr_handler_s *) opaque;
    int i, offset = addr;
    int line_no;
    uint16_t rval = 0;
    struct locosto_intr_handler_bank_s *bank = &s->bank;
    offset &= 0xff;

    switch (offset) {
    case 0x00:  /* ITR */
	rval = (bank->irqs & 0xffff);
        break;

    case 0x02:
        rval =  (bank->irqs >> 16);
        break;

    case 0x08:  /* MIR */
	rval = (bank->mask & 0xffff);
        break;

    case 0x0A:
        rval = (bank->mask >> 16);
        break;

    case 0x10:  /* SIR_IRQ_CODE */
    case 0x12:  /* SIR_FIQ_CODE */
        line_no = s->sir_intr[(offset - 0x10) >> 1]; 
        i = line_no & 31;
        if (INT_IS_EDGE(i)) {
            if (bank->irqs & (1 << i)) LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_WARN,
                                                  "%s: clearning edge %d\n", __FUNCTION__, i);
            bank->irqs &= ~(1 << i);
        }
        rval = line_no;
        break;

    case 0x14:  /* CONTROL_REG */
        rval = 0;
        break;

    case 0x20:  /* ILR0 */
    case 0x22:  /* ILR1 */
    case 0x24:  /* ILR2 */
    case 0x26:  /* ILR3 */
    case 0x28:  /* ILR4 */
    case 0x2A:  /* ILR5 */
    case 0x2C:  /* ILR6 */
    case 0x2E:  /* ILR7 */
    case 0x30:  /* ILR8 */
    case 0x32:  /* ILR9 */
    case 0x34:  /* ILR10 */
    case 0x36:  /* ILR11 */
    case 0x38:  /* ILR12 */
    case 0x3A:  /* ILR13 */
    case 0x3C:  /* ILR14 */
    case 0x3E:  /* ILR15 */
    case 0x40:  /* ILR16 */
    case 0x42:  /* ILR17 */
    case 0x44:  /* ILR18 */
    case 0x46:  /* ILR19 */
    case 0x48:  /* ILR20 */
    case 0x4A:  /* ILR21 */
    case 0x4C:  /* ILR22 */
    case 0x4E:  /* ILR23 */
    case 0x50:  /* ILR24 */
    case 0x52:  /* ILR25 */
    case 0x54:  /* ILR26 */
    case 0x56:  /* ILR27 */
    case 0x58:  /* ILR28 */
    case 0x5A:  /* ILR29 */
    case 0x5C:  /* ILR30 */
    case 0x5E:  /* ILR31 */
        i = (offset - 0x20) >> 1;
        rval = (bank->priority[i] << 2) |
                (((bank->sens_edge >> i) & 1) << 1) |
                ((bank->fiq >> i) & 1);
    default:
        LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_ERR, "%s: INVALID READ: offset=0x%02X \n",
                   __FUNCTION__, offset);
    }

    LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "%s: offset=0x%02X val=0x%04x\n",
               __FUNCTION__, offset, rval);

    return rval;
}

static void locosto_inth_write(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    struct locosto_intr_handler_s *s = (struct locosto_intr_handler_s *) opaque;
    int i, offset = addr;
    struct locosto_intr_handler_bank_s *bank = &s->bank;
    offset &= 0xff;

    LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "%s: offset=0x%02X value=0x%04X\n", __FUNCTION__, offset, value);

    switch (offset) {
    case 0x00:  /* ITR */
        /* Important: ignore the clearing if the IRQ is level-triggered and
           the input bit is 1 */
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "ITR: before bank->irq 0x%08x value 0x%04x\n", bank->irqs, value);
        bank->irqs &= 0xffff0000 | (value & 0xffff) | (bank->inputs & (INT_FALLING_EDGE ? ~bank->sens_edge : bank->sens_edge));
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "ITR: after  bank->irq 0x%08x value 0x%04x\n", bank->irqs, value);
        return;
    case 0x02:
        LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "ITR: before bank->irq 0x%08x value 0x%04x\n", bank->irqs, value);
	bank->irqs &= 0xffff | (value<<16) | (bank->inputs & (INT_FALLING_EDGE ? ~bank->sens_edge : bank->sens_edge));
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "ITR: after  bank->irq 0x%08x value 0x%04x\n", bank->irqs, value);
	return;

    case 0x08:  /* MIR */
	bank->mask &= ~(0xffff); 
	bank->mask |= (value & 0xffff);
        locosto_inth_update(s, 0);
        locosto_inth_update(s, 1);
        return;

    case 0x0A:
	bank->mask &= ~(0xffff <<16);
	bank->mask |= (value << 16);
        locosto_inth_update(s, 0);
        locosto_inth_update(s, 1);
	return;

    case 0x10:  /* SIR_IRQ_CODE */
    case 0x12:  /* SIR_FIQ_CODE */
	LOCOSTO_RO_REG(addr);
        break;

    case 0x14:  /* CONTROL_REG */
        if (value & 2) { // FIQ
            qemu_set_irq(s->parent_intr[1], 0);
            s->new_agr[1] = ~0;
            locosto_inth_update(s, 1);
        }
        if (value & 1) { // IRQ
            qemu_set_irq(s->parent_intr[0], 0);
            s->new_agr[0] = ~0;
            locosto_inth_update(s, 0);
        }
        return;
		/* Interrupt Level Registers */
    case 0x20:  /* ILR0 */
    case 0x22:  /* ILR1 */
    case 0x24:  /* ILR2 */
    case 0x26:  /* ILR3 */
    case 0x28:  /* ILR4 */
    case 0x2A:  /* ILR5 */
    case 0x2C:  /* ILR6 */
    case 0x2E:  /* ILR7 */
    case 0x30:  /* ILR8 */
    case 0x32:  /* ILR9 */
    case 0x34:  /* ILR10 */
    case 0x36:  /* ILR11 */
    case 0x38:  /* ILR12 */
    case 0x3A:  /* ILR13 */
    case 0x3C:  /* ILR14 */
    case 0x3E:  /* ILR15 */
    case 0x40:  /* ILR16 */
    case 0x42:  /* ILR17 */
    case 0x44:  /* ILR18 */
    case 0x46:  /* ILR19 */
    case 0x48:  /* ILR20 */
    case 0x4A:  /* ILR21 */
    case 0x4C:  /* ILR22 */
    case 0x4E:  /* ILR23 */
    case 0x50:  /* ILR24 */
    case 0x52:  /* ILR25 */
    case 0x54:  /* ILR26 */
    case 0x56:  /* ILR27 */
    case 0x58:  /* ILR28 */
    case 0x5A:  /* ILR29 */
    case 0x5C:  /* ILR30 */
    case 0x5E:  /* ILR31 */
        i = (offset - 0x20) >> 1; // get Int index
	LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_INFO2, "Set interrupt %d \n - Priority: %d\n - EdgeSense: %d\n - FIQ: %d\n",
		   i, (value >> 2) & 0x1f, (value >> 1) & 1, (value & 1));
        bank->priority[i] = (value >> 2) & 0x1f; // Get bits 2-6 Priority
        bank->sens_edge &= ~(1 << i);                   // clear edge sense bit
        bank->sens_edge |= ((value >> 1) & 1) << i;     // set edge sense bit
        bank->fiq &= ~(1 << i);         // clear FIQ bit 
        bank->fiq |= (value & 1) << i;  // set FIQ bit
        return;

    }
    LOCO_DEBUG(LOCO_DEBUG_INTH, LOCO_DLVL_ERR, "%s: INVALID WRITE offset=0x%02X value=0x%04X\n", __FUNCTION__, offset, value);
}

static CPUReadMemoryFunc *locosto_inth_readfn[] = {
    locosto_badwidth_read16,
    locosto_inth_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *locosto_inth_writefn[] = {
    locosto_badwidth_write16,
    locosto_inth_write, 
    locosto_badwidth_write16,
};

void locosto_inth_reset(struct locosto_intr_handler_s *s)
{
    struct locosto_intr_handler_bank_s *bank = &s->bank;

    bank->irqs = 0x00000000;
    bank->mask = 0xffffffff;
    bank->sens_edge = 0x00000000;
    bank->fiq = 0x00000000;
    bank->inputs = 0x00000000;
    //bank->swi = 0x00000000;
    memset(bank->priority, 0, sizeof(bank->priority));

    s->new_agr[0] = ~0;
    s->new_agr[1] = ~0;
    s->sir_intr[0] = 0;
    s->sir_intr[1] = 0;
    s->autoidle = 0;
    s->mask = ~0;

    qemu_set_irq(s->parent_intr[0], 0);
    qemu_set_irq(s->parent_intr[1], 0);
}

struct locosto_intr_handler_s *locosto_inth_init(target_phys_addr_t base,
                unsigned long size, qemu_irq **pins,
                qemu_irq parent_irq, qemu_irq parent_fiq)
{
    int iomemtype;
    struct locosto_intr_handler_s *s = (struct locosto_intr_handler_s *)
        qemu_mallocz(sizeof(struct locosto_intr_handler_s) + 
                     sizeof(struct locosto_intr_handler_bank_s) /* <-- huh? */);

    s->parent_intr[0] = parent_irq;
    s->parent_intr[1] = parent_fiq;
    s->pins = qemu_allocate_irqs(locosto_inth_set_intr, s, 32);
    if (pins)
        *pins = s->pins;

    locosto_inth_reset(s);

    iomemtype = cpu_register_io_memory(locosto_inth_readfn, locosto_inth_writefn, s);
    LOCOSTO_OPAQUE("inth", s);
    cpu_register_physical_memory(base, size, iomemtype);

    return s;
}

/**************************************************************************/
/* Ultra Low-Power Device Module */
/**************************************************************************/

static uint32_t locosto_ulpd_pm_read(void *opaque, target_phys_addr_t addr)
{
    locosto_ulpd_s *s = (struct locosto_ulpd_s *) opaque;

    LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: offset = 0x%02x\n", __FUNCTION__, (int)addr);

    switch (addr) {
    case 0x16: // GSM_TIMER_INIT
	return s->gtimer_init;
    case 0x18: // GSM_TIMER_VALUE
	LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR,
                   "%s: GSM_TIMER_VALUE = 0x%02x value = 0x%02x\n",
                   __FUNCTION__, (int)addr, s->gtimer_val);
	return s->gtimer_val;
    case 0x14: // GSM_TIMER_CTRL
	return s->gtimer_ctrl;
    case 0x10: // GAUGING_CTRL
	return s->gauge_ctrl;
    case 0x1a: // GSM_TIMER_IT
	// bit0: was there interrupt?  clear-on-read
        break;
    default:
	LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: UNMAPPED offset = 0x%02x\n", __FUNCTION__, (int)addr);
	//LOCO_BREAK;
    }
    return 0;
}

static void locosto_ulpd_pm_write(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    locosto_ulpd_s *s = (struct locosto_ulpd_s *) opaque;
    //int64_t now, ticks;

    LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: offset = 0x%02x\n", __FUNCTION__, (int)addr);

    switch(addr) {
    case 0x16: // GSM_TIMER_INIT
	s->gtimer_init = value;
	break;
    case 0x18: // GSM_TIMER_VALUE
	LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: writing RO reg GSM_TIMER_VALUE\n", __FUNCTION__);
        LOCO_BREAK;
	break;
    case 0x14: // GSM_TIMER_CTRL
	s->gtimer_ctrl = value;
	break;
    case 0x1a: // GSM_TIMER_IT
	// bit0: was there interrupt?  clear-on-read
	LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: UNMAPPED offset = 0x%02x value = 0x%02x\n", __FUNCTION__, (int)addr, value);
	LOCO_BREAK;
	return;
	break;

    case 0x22: // SETUP_FRAME
        s->setup_frame = value;
        break;
    case 0x20: // SETUP_VTCXO
        s->setup_vtcxo = value;
        break;
    case 0x1e: // SETUP_SLICER
        s->setup_vtcxo = value;
        break;
    case 0x1c: // SETUP_CLOCK_13MHZ
        s->setup_clk13 = value;
        break;
    case 0x24: // SETUP_RF
        s->setup_rf = value;
        break;

    case 0x10:  /* GAUGING_CTRL */
        //LOCO_BREAK;
	s->gauge_ctrl = value;
	break;
    case 0x02: // INC_SIXTEENTH
        s->inc_sixteenth = value;
        break;
    case 0x00: // INC_FRAC
        s->inc_frac = value;
        break;
    case 0x28: // DCXO_SETUP_SYSCLKEN
        s->setup_dcxo_sysclken = value;
        break;
    case 0x26: // DCXO_SETUP_SLEEPEN
        s->setup_dcxo_sleepen = value;
        break;
    case 0x12: // GAUGING_STATUS
        LOCO_BREAK;
        break;
    case 0x0e: // HIGH_FREQ_MSB
	// bits 5:0
    case 0x0c: // HIGH_FREQ_LSB
	// bits 15:0
    case 0x0a: // COUNTER_32_MSB
    case 0x08: // COUNTER_32_LSB
    case 0x04: // SIXTEENTH_START
    case 0x06: // SIXTEENTH_STOP
        //case 0x28: // DCXO_SETUP_SYSCLKEN
        //case 0x26: // DCXO_SETUP_SLEEPEN
    default:
	LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: UNMAPPED offset = 0x%02x value = 0x%02x\n", __FUNCTION__, (int)addr, value);
	LOCO_BREAK;
	return;
    }
    LOCO_DEBUG(LOCO_DEBUG_ULPD, LOCO_DLVL_ERR, "%s: offset = 0x%02x value = 0x%04x\n", __FUNCTION__, (int)addr, value);
}

static CPUReadMemoryFunc *locosto_ulpd_pm_readfn[] = {
    locosto_badwidth_read16,
    locosto_ulpd_pm_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *locosto_ulpd_pm_writefn[] = {
    locosto_badwidth_write16,
    locosto_ulpd_pm_write,
    locosto_badwidth_write16,
};

static void locosto_ulpd_pm_init(target_phys_addr_t base, qemu_irq irq_gsm, qemu_irq irq_eog)
{
    struct locosto_ulpd_s *ulpd = (struct locosto_ulpd_s *) qemu_mallocz(sizeof(struct locosto_ulpd_s));

    int iomemtype = cpu_register_io_memory(locosto_ulpd_pm_readfn,
                                           locosto_ulpd_pm_writefn, ulpd);
    LOCOSTO_OPAQUE("ulpd", ulpd);

    cpu_register_physical_memory(base, 0x7FF, iomemtype);

    // gsm
    ulpd->gtimer_init = 0;
    ulpd->gtimer_val = 0x1;
    ulpd->gtimer_ctrl = 0x2;
    ulpd->gtimer_it = 0;

    // setup
    ulpd->setup_frame = 0;
    ulpd->setup_rf = 0;
    ulpd->setup_vtcxo = 0xfff;
    ulpd->setup_slicer = 0xfff;
    ulpd->setup_clk13 = 0x3f;

    // gauging
    ulpd->gauge_ctrl = 0;
    ulpd->gauge_status = 0;
    ulpd->gauge_counter_hi = 0;
    ulpd->gauge_counter_32 = 0;
    ulpd->gauge_counter_gsm = 0;

    ulpd->inc_frac = 0;
    ulpd->inc_sixteenth = 0;
    ulpd->sixteenth_stop = 0;

    ulpd->setup_dcxo_sysclken = 0;
    ulpd->setup_dcxo_sleepen = 0;

    ulpd->irq_gsm = irq_gsm;
    ulpd->irq_eog = irq_eog;
}
/* KeyBoard */

typedef struct LocostoKeyPadState {
    qemu_irq    irq;
    struct  keymap *map;
} LocostoKeyPadState;

typedef struct locosto_keyboard_s {
    uint32_t kbr_ctrl;
    uint32_t kbr_debounce;
    uint32_t kbr_state_status;
    uint32_t kbr_in;
    uint32_t kbr_isr;
    uint32_t kbr_fullcode;
    uint32_t kbr_ie;
    uint32_t kbr_clr_isr;
    uint32_t kbr_out;
    qemu_irq irq;
    LocostoKeyPadState *kp;
} locosto_keyboard_s;

/* KeyBoard */
static uint32_t locosto_keyboard_read(void *opaque, target_phys_addr_t addr)
{
    locosto_keyboard_s *s = (struct locosto_keyboard_s *) opaque;

    //LOCO_DEBUG(LOCO_DEBUG_KPD, LOCO_DLVL_ERR, "%s: offset = 0x%02x\n", __FUNCTION__, (int)addr);

    switch(addr) {
	case KBR_CTRL_REG:
	    return s->kbr_ctrl;
	case KBR_DEBOUNCING_TIME:
	    return s->kbr_debounce;
	case KBR_ISR_REG:
	    return s->kbr_isr;
	case KBR_CLR_ISR_REG:
	    return s->kbr_clr_isr;
	case KBR_IE_REG:
	    return s->kbr_ie;
	case KBR_STATE_MACHINE_STATUS:
	    return s->kbr_state_status;
	case KBR_IN:
	    return s->kbr_in;
	case KBR_OUT:
	    return s->kbr_out;
	case KBR_FULL_CODE:
	    return s->kbr_fullcode;
      default:
         // LOCO_DEBUG(LOCO_DEBUG_KPD, LOCO_DLVL_ERR, "%s: UNMAPPED offset = 0x%02x\n", __FUNCTION__, (int)addr);
	  break;
    }

    return 0;
}

static void locosto_keyboard_write(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    locosto_keyboard_s *s = (struct locosto_keyboard_s *) opaque;

    //LOCO_DEBUG(LOCO_DEBUG_KPD, LOCO_DLVL_ERR, "%s: offset = 0x%02x value = 0x%02x\n", __FUNCTION__, (int)addr, value);

    switch(addr) {
        case KBR_CTRL_REG:
      	    s->kbr_ctrl = value;
	    break;
	case KBR_DEBOUNCING_TIME:
	    s->kbr_debounce = value;
	    break;
	case KBR_ISR_REG:
	    s->kbr_isr = value;
	    break;
	case KBR_CLR_ISR_REG:
	    s->kbr_clr_isr = value;
	    break;
	case KBR_IE_REG:
	    s->kbr_ie = value;
	    break;
	case KBR_STATE_MACHINE_STATUS:
	    s->kbr_state_status = value;
	    break;
	case KBR_IN:
	    s->kbr_in = value;
	    break;
	case KBR_OUT:
	    s->kbr_out = value;	
	    break;
	case KBR_FULL_CODE:
	    s->kbr_fullcode = value;
	    break;
      default:
	//LOCO_DEBUG(LOCO_DEBUG_KPD, LOCO_DLVL_ERR, "%s: UNMAPPED offset = 0x%02x value = 0x%02x\n", __FUNCTION__, (int)addr, value);
	break;
    }

}
static CPUReadMemoryFunc *locosto_keyboard_readfn[] = {
    locosto_keyboard_read,
    locosto_keyboard_read,
    locosto_keyboard_read,
};

static CPUWriteMemoryFunc *locosto_keyboard_writefn[] = {
    locosto_keyboard_write,
    locosto_keyboard_write,
    locosto_keyboard_write,
};
/*
static void locosto_keyboard_event (LocostoKeyPadState *kp, int keycode)
{
    //int row, col,rel;

    fprintf(stderr, "Got keypad event 0x%02x\n", keycode);

}

static void locosto_register_keypad(LocostoKeyPadState *kp, struct keymap *map, int size)
{
    kp->map = map;
    qemu_add_kbd_event_handler((QEMUPutKBDEvent *) locosto_keyboard_event, kp);
}
*/
static void locosto_keyboard_init(target_phys_addr_t base, qemu_irq irq)
{
    locosto_keyboard_s *s = (struct locosto_keyboard_s *) qemu_mallocz(sizeof(struct locosto_keyboard_s));
    int io;
    LocostoKeyPadState *kp;

    kp = (LocostoKeyPadState *) qemu_mallocz(sizeof(LocostoKeyPadState));

    s->irq = irq;
    kp->irq = irq;

    io = cpu_register_io_memory(locosto_keyboard_readfn, locosto_keyboard_writefn, s);
    cpu_register_physical_memory(base, 0x7FF, io);

    //locosto_register_keypad(kp, map, 0xE0);
}

/**************************************************************************/
/* Handle QEMU IO presses */
/**************************************************************************/
static void peek_mouse_event(void *opaque,
                             int dx, int dy, int dz, int buttons_state)
{
    peek_state_s *s = opaque;

    /* XXX: SDL sometimes generates nul events: we delete them */
    if (dx == 0 && dy == 0 && dz == 0 && s->mouse_buttons == buttons_state)
	return;

    if (dz == 1) {
        // scroll down
        qemu_set_irq(s->gpio[0]->in[0], 1);
        printf("down\n");
    } else if (dz == -1) {
        // scroll up
        qemu_set_irq(s->gpio[0]->in[0], 0);
    } else {
        qemu_set_irq(s->gpio[0]->in[0], 0);
    }

    if (s->mouse_buttons != buttons_state) {
        //int diff = s->mouse_buttons ^ buttons_state;
        int i;

        printf("buttons: 0x%08x -> 0x%08x\n", s->mouse_buttons, buttons_state);

        for (i = 0; i < 3; i++) {
            int ln, b;

            b = ~s->gpio[i]->mask & s->gpio[i]->dir;
            printf("GPIO%d mask 0x%04x dir 0x%04x inputs 0x%04x\n", i,
                   s->gpio[i]->mask, s->gpio[i]->dir, s->gpio[i]->inputs);

            while ((ln = ffs(b))) {
                ln --;
                printf("GPIO %d active irq\n", i*16 + ln);
                b &= ~(1 << ln);
            }
        }
    }

    s->mouse_buttons = buttons_state;
}

static struct {
    int row;
    int column;
} peek_keymap[0x80] = {
    [0 ... 0x7f] = { -1, -1 },
    [0x3b] = { 0, 0 },	/* F1	-> Calendar */
    [0x3c] = { 1, 0 },	/* F2	-> Contacts */
    [0x3d] = { 2, 0 },	/* F3	-> Tasks List */
    [0x3e] = { 3, 0 },	/* F4	-> Note Pad */
    [0x01] = { 4, 0 },	/* Esc	-> Power */
    [0x4b] = { 0, 1 },	/* 	   Left */
    [0x50] = { 1, 1 },	/* 	   Down */
    [0x48] = { 2, 1 },	/*	   Up */
    [0x4d] = { 3, 1 },	/*	   Right */
    [0x4c] = { 4, 1 },	/* 	   Centre */
    [0x39] = { 4, 1 },	/* Spc	-> Centre */
};

static void peek_button_event(void *opaque, int keycode)
{
    //peek_state_s *s = (peek_state_s *) opaque;

    if (peek_keymap[keycode & 0x7f].row != -1) {
        fprintf(stderr, "KEY 0x%x\n", keycode);
        printf("KEY 0x%x\n", keycode);
    }
}


/**************************************************************************/
/* PEEK INIT */
/**************************************************************************/

/* actually in the peek's on-chip ROM, just used to redirect exceptions */
static void peek_bootrom_setup(void)
{
    int32_t intvecs[] = {
        0xea0016d0,     /*      b	0x5b48 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe28ff302,     /*      add	pc, pc, #134217728	; 0x8000000 */
	0xe3a0f084,     /*      mov	pc, #132	; 0x84          */
	0xe3a0f088,     /*      mov	pc, #136	; 0x88          */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe3a0f084,     /*      mov	pc, #132	; 0x84          */
	0xe3a0f088,     /*      mov	pc, #136	; 0x88          */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
	0xe12fff1e,     /*      bx	lr                              */
    };

    /* Map internal RAM */ 
    cpu_register_physical_memory(0, sizeof(intvecs), qemu_ram_alloc(sizeof(intvecs)));

    /* Load the instructions */
    cpu_physical_memory_write(0x00000000, (uint8*)intvecs, sizeof(intvecs));
}

static void peek_init(ram_addr_t ram_size,
                     const char *boot_device,
                     const char *kernel_filename, const char *kernel_cmdline,
                     const char *initrd_filename, const char *cpu_model)
{
    int io;
    qemu_irq *cpu_irq;
    ram_addr_t phys_flash;
    int srom_size,flash_size;

    peek_state_s *s = (peek_state_s *)qemu_mallocz(sizeof(peek_state_s));

    if (!cpu_model)
        cpu_model = "arm7tdmi";

    /* setup debug */
#if 0
    g_debug_fp = fopen("peeklog", "w+");
    if (!g_debug_fp) {
	LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "couldn't open peeklog\n");
	exit(1);
    }
#else
    g_debug_fp = stderr;
#endif

    /* */
    s->env = cpu_init(cpu_model);
    if (!s->env) {
        LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "Unable to find CPU definition\n");
        exit(1);
    }

    if (nb_option_roms == 2) {
	flash_size = get_image_size(option_rom[0]);
	srom_size = get_image_size(option_rom[1]);
        if (flash_size <= 0 || srom_size <= 0) {
		LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "Invalid rom size\n");
		exit(1);
	}
    } else {
	LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "You must supply firmware and srom and bootrom images\n");
	exit(1);
    }

    LOCO_DEBUG(LOCO_DEBUG_MPU, LOCO_DLVL_ERR, "Loading flash %s of size 0x%x\n Loading srom %s of size 0x%x\n",
	    option_rom[0], flash_size, option_rom[1], srom_size);

    /* Map first 8M of Nor attached to CS3 to rom file */
#define CS3_SIZE (8*1024*1024)
    cpu_register_physical_memory(CS3_BASE, CS3_SIZE, (phys_flash = qemu_ram_alloc(CS3_SIZE)) | IO_MEM_ROM);
    load_image_targphys(option_rom[0], CS3_BASE, flash_size);

    /* Map SROM to supplied SROM image */
    cpu_register_physical_memory(SROM_BASE, srom_size,(phys_flash = qemu_ram_alloc(srom_size)) | IO_MEM_ROM);
    load_image_targphys(option_rom[1], SROM_BASE, srom_size);

    /* Setup Interrupt Handler */
    cpu_irq = arm_pic_init_cpu(s->env);
    s->ih = locosto_inth_init(INTH_BASE, 0x7F, &s->irq[0],
                    cpu_irq[ARM_PIC_CPU_IRQ], cpu_irq[ARM_PIC_CPU_FIQ]);
    g_inth = s->ih; // for debug

    /* Map 4MB of external of PSRAM  */
    cpu_register_physical_memory(EXT_RAM_BASE, 0x00400000, qemu_ram_alloc(0x00400000));

    /* Map (2.5Mb) 320KB of internal SRAM */ 
    cpu_register_physical_memory(SRAM_BASE, 0x50000, qemu_ram_alloc(0x50000));

    /* setup Peek's boot rom */
    peek_bootrom_setup();

    /* CLKM */
    clk_reset(s);
    io = cpu_register_io_memory(clkm_readfn, clkm_writefn, s);
    LOCOSTO_OPAQUE("clkm", s);
    cpu_register_physical_memory(CLKM_BASE, 0xFF, io);

    /* DPLL */
    locosto_dpll_init(s);

    /* EMIF */
    locosto_emif_init(s);

    /* GPIO */
    s->gpio[0] = locosto_gpio_init(GPIO0_BASE, s->irq[0][LOCOSTO_INT_GPIO_0], 0);
    s->gpio[1] = locosto_gpio_init(GPIO1_BASE, s->irq[0][LOCOSTO_INT_GPIO_1], 1);
    s->gpio[2] = locosto_gpio_init(GPIO2_BASE, s->irq[0][LOCOSTO_INT_GPIO_2], 2);

    /* TPU */
    s->tpu = locosto_tpu_init(TPU_BASE, TPURAM_BASE,
                             s->irq[0][LOCOSTO_INT_TPU_FRAME],
                             s->irq[0][LOCOSTO_INT_TPU_PAGE]);

    /* UART */
    s->uart = locosto_uart_init(UART_BASE, s->irq[0][LOCOSTO_INT_UART1], serial_hds[0]);

    /* LCD */
    s->lcd = locosto_lcd_init(LCD_BASE, s->irq[0][LOCOSTO_INT_LCD]);

    /* Timer init */
    locosto_mpu_timer_init(TIMER1_BASE, s->irq[0][LOCOSTO_INT_TIMER1], 0);
    locosto_mpu_timer_init(TIMER2_BASE, s->irq[0][LOCOSTO_INT_TIMER2], 0);

    /* ULPD init */
    locosto_ulpd_pm_init(ULPD_BASE, 
                         s->irq[0][LOCOSTO_INT_GSM_TIMER],
                         s->irq[0][LOCOSTO_INT_END_GAUGING]);

    /* KeyBoard */
    locosto_keyboard_init(KEYBOARD_BASE, s->irq[0][LOCOSTO_INT_KEYBOARD]);

    /* i2c */
    locosto_i2c_init(I2C1_BASE,s->irq[0][LOCOSTO_INT_I2C1], &s->drq[LOCOSTO_DMA_I2C1_RX]);
    locosto_i2c_init(I2C2_BASE,s->irq[0][LOCOSTO_INT_I2C2], &s->drq[LOCOSTO_DMA_I2C2_RX]);

    /* peek buttons */
    qemu_add_kbd_event_handler(peek_button_event, s);
    qemu_add_mouse_event_handler(peek_mouse_event, s, 1, "Peek Scroll Wheel");

    /* Set PC to start addr */
    s->env->regs[15] = CS3_BASE;

}

QEMUMachine peek_machine = {
    .name = "peek",
    .desc = "Peek (ARM7TDMI)",
    .init = peek_init,
    .is_default = 1,
};

static void peek_machine_init(void)
{
    qemu_register_machine(&peek_machine);
}

machine_init(peek_machine_init);
