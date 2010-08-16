#ifndef PEEK_H
#define PEEK_H

#include "i2c.h"
#include "qemu-timer.h"

#define LOCO_DEBUG_I2C	(0x00000001)
#define LOCO_DEBUG_INTH	(0x00000002)
#define LOCO_DEBUG_MPU	(0x00000004)
#define LOCO_DEBUG_UART	(0x00000008)
#define LOCO_DEBUG_CLKM	(0x00000010)
#define LOCO_DEBUG_ULPD	(0x00000020)
#define LOCO_DEBUG_TPU	(0x00000040)
#define LOCO_DEBUG_LCD	(0x00000080)
#define LOCO_DEBUG_EMIF	(0x00000100)
#define LOCO_DEBUG_GPIO	(0x00000200)
#define LOCO_DEBUG_KPD  (0x00000400)
#define LOCO_DEBUG_SOCKET (0x00000800)

#define LOCO_DLVL_ERR	(0)
#define LOCO_DLVL_WARN	(1)
#define LOCO_DLVL_INFO	(2)
#define LOCO_DLVL_INFO2	(3)

extern uint32_t g_debug;
extern uint32_t g_dlevel;
extern FILE *g_debug_fp;

//#define LOCO_DEBUG
#define LOCO_DEBUG(module, level, msg ...)                              \
    do {                                                                \
	if ((module) & g_debug || (level) <= g_dlevel) {                \
	    fprintf(g_debug_fp, "%ld : ", qemu_get_clock(vm_clock));    \
            fprintf(g_debug_fp, msg);                                   \
        }                                                               \
    } while (0)

// gdb should catch this :)
#define LOCO_BREAK  (*(int *)0 = 1)

/* ******************************************
 * mem map
 */
#define EXT_RAM_BASE    0x00400000
#define CS3_BASE        0x06000000
#define CS3_SIZE 	((8*1024*1024) - 1)
#define SRAM_BASE       0x08000000
#define NOR_SECTOR	0x10000
#define NOR_BASE	0x06800000
#define SROM_BASE       0x08050000
#define ULPD_BASE       0xFFFE2000
#define TIMER1_BASE     0xFFFE3800
#define GPIO0_BASE      0xFFFE4800
#define GPIO1_BASE      0xFFFE5000
#define GPIO2_BASE      0xFFFE5800
#define TIMER2_BASE     0xFFFE6800
#define UART_BASE       0xFFFF7000
#define TPURAM_BASE     0xFFFF9000
#define DPLL_BASE       0xFFFF9800
#define LCD_BASE        0xFFFFA000 /* 0x7ff */
#define EMIF_BASE       0xFFFFFB00
#define KEYBOARD_BASE   0xFFFEB800
#define TPU_BASE        0xFFFFF000
#define WDTIMER_BASE    0xFFFFF800
#define INTH_BASE       0xFFFFFA00
#define CLKM_BASE       0xFFFFFD00

/* KeyBoard */

struct  keymap {
    int column;
    int row;
};

#define KBR_CTRL_REG              (0x00) /* KBR control reg */
#define KBR_DEBOUNCING_TIME       (0x02) /* KBR debouncing time reg */
#define KBR_LONG_KEY		  (0x04) /* KBR Long Key Time Reg */
#define KBR_TOUT_REG		  (0x06) /* KBR Time out Reg */
#define KBR_ISR_REG		  (0x08) /* KBR Interrupt Status reg */
#define KBR_CLR_ISR_REG		  (0x0a) /* KBR Clear Interrupt reg */
#define KBR_IE_REG 		  (0x0C) /* KBR Interrupt enable */
#define KBR_STATE_MACHINE_STATUS  (0x0E) /* KBR state machine status reg */
#define KBR_IN                    (0x10) /* KBR inputs (rows) */
#define KBR_OUT                   (0x12) /* KBR outputs (columns) */
#define KBR_FULL_CODE		  (0x14) /* Full code reg */

/* DPLL */
#define DPLL_CTRL_LOCK          (1<<0)
#define DPLL_CTRL_BYPASSDIV(x)  ((((x) & (3<<2)) >> 2)+1) // read val
#define DPLL_CTRL_PLLEN         (1<<4)
#define DPLL_CTRL_PLLDIV(x)     ((((x) & (3<<5)) >> 5)+1) // read val
#define DPLL_CTRL_PLLMULT(x)    (((x) & (31<<7)) >> 7) // read val

/* CLKM */

/* TPU */

/* ULPD */

/* Interrupts */
#define LOCOSTO_INT_WDTIMER     0
#define LOCOSTO_INT_TIMER1      1
#define LOCOSTO_INT_TIMER2      2
#define LOCOSTO_INT_MSCI        3
#define LOCOSTO_INT_TPU_FRAME   4
#define LOCOSTO_INT_TPU_PAGE    5
#define LOCOSTO_INT_DRP_S       6
#define LOCOSTO_INT_UART1       7
#define LOCOSTO_INT_KEYBOARD    8
#define LOCOSTO_INT_DRP_RX      9
#define LOCOSTO_INT_CAM         10
#define LOCOSTO_INT_END_GAUGING 11      // ULPD
#define LOCOSTO_INT_ABB         12
#define LOCOSTO_INT_MSSPI       13
#define LOCOSTO_INT_DMA         14
#define LOCOSTO_INT_API         15
#define LOCOSTO_INT_GPIO_0      16
#define LOCOSTO_INT_X1          17
#define LOCOSTO_INT_DRP_TX      18
#define LOCOSTO_INT_GSM_TIMER   19      // ULPD
#define LOCOSTO_INT_GEA         20
#define LOCOSTO_INT_GPIO_1      21
#define LOCOSTO_INT_GPIO_2      22
#define LOCOSTO_INT_CPORT       23
#define LOCOSTO_INT_USIM        24
#define LOCOSTO_INT_LCD         25
#define LOCOSTO_INT_USB         26
#define LOCOSTO_INT_X2          27
#define LOCOSTO_INT_I2C1        28
#define LOCOSTO_INT_SEC1        29
#define LOCOSTO_INT_I2C2        30
#define LOCOSTO_INT_NAND        31

/* Interrupt Handler */

# define LOCOSTO_BAD_REG(paddr)            \
        fprintf(stderr, "%s: Bad register %#08x \n",  \
                        __FUNCTION__, (unsigned int)paddr)
# define LOCOSTO_RO_REG(paddr)             \
        fprintf(stderr, "%s: Read-only register %#08x \n",    \
                        __FUNCTION__, (unsigned int)paddr)
# define LOCOSTO_16B_REG(paddr, opaque)                              \
        fprintf(stderr, "%s: 16-bit register %#08x (%p)\n",       \
		__FUNCTION__, (unsigned int)paddr, opaque)
#define LOCOSTO_OPAQUE(name, opaque) fprintf(stderr, name " is at %p\n", opaque)

uint32_t locosto_badwidth_read16(void *opaque, target_phys_addr_t addr);
void locosto_badwidth_write16(void *opaque, target_phys_addr_t addr, uint32_t value);

/* I2C */
#define I2C1_BASE       0xFFFFB800
#define I2C2_BASE       0xFFFFC800
#define LOCOSTO_DMA_I2C1_RX 16
#define LOCOSTO_DMA_I2C2_RX 25
#define PEEK_INTR_REV   0x37

typedef struct peek_socket_s {
    int sock;
    int type;
    int domain;
    int protocol;
    int key;
    /* Connect vars */
    int cRet;
    int wRet;
    int rRet;
    struct sockaddr_in *conn_in;

    /* Connect Sock */
    uint32_t cTransferSize;
    uint32_t cTransferCount;
    char *cTransferbuffer;

    /* Write Sock */
    uint32_t wTransferSize;
    uint32_t wTransferCount;
    char *wTransferbuffer;

    /* Read Sock */
    uint32_t rTransferSize;
    char *rTransferbuffer;
    uint32_t rTransferCount;
} peek_socket_s;

typedef struct locosto_clkm {
    uint32_t cntl_arm_clk;
    uint32_t cntl_clk;
    uint32_t cntl_rst;
    uint32_t apll_div_clk;
    uint32_t clk_prog_free;
} locosto_clkm;

struct locosto_intr_handler_bank_s {
    uint32_t irqs;
    uint32_t inputs;
    uint32_t mask;
    uint32_t fiq;
    uint32_t sens_edge;
    //uint32_t swi;
    unsigned char priority[32];
};

struct locosto_intr_handler_s {
    qemu_irq *pins;
    qemu_irq parent_intr[2];
    int level_only;

    /* state */
    uint32_t new_agr[2];
    int sir_intr[2];
    int autoidle;
    uint32_t mask;
    struct locosto_intr_handler_bank_s bank;
};

typedef struct locosto_tpu_ {
    uint32_t tpu_ctrl;
    QEMUTimer *timer;

    qemu_irq irq_frame;
    qemu_irq irq_page;
} locosto_tpu_s;

typedef struct locosto_lcd_ {
    DisplayState *ds;
    uint16_t palette[256];
    int invalidate;
    uint32_t lcd_ctrl;
    qemu_irq irq;
} locosto_lcd_s;

typedef struct locosto_ulpd_s {
    // gsm
    uint32_t gtimer_init;
    uint32_t gtimer_val;
    uint32_t gtimer_ctrl;
    uint32_t gtimer_it;
    uint32_t gsm_rate;

    // setup
    uint32_t setup_frame;
    uint32_t setup_rf;
    uint32_t setup_vtcxo;
    uint32_t setup_slicer;
    uint32_t setup_clk13;

    // gauging
    uint32_t gauge_ctrl;
    uint32_t gauge_status;
    uint32_t gauge_counter_hi;
    uint32_t gauge_counter_32;
    uint32_t gauge_counter_gsm;

    uint32_t inc_frac;
    uint32_t inc_sixteenth;
    uint32_t sixteenth_stop;

    uint32_t setup_dcxo_sysclken;
    uint32_t setup_dcxo_sleepen;

    int64_t ulpd_gauge_start;
    qemu_irq irq_gsm;
    qemu_irq irq_eog;
} locosto_ulpd_s;

typedef struct {
    qemu_irq irq;
    int8_t idx;
    qemu_irq *in;
    qemu_irq handler[16];

    uint16_t cntl;
    uint8_t buzzer_power;

    uint16_t inputs;
    uint16_t outputs;
    uint16_t dir;
    uint16_t edge;
    uint16_t mask;
    uint16_t ints;
    uint16_t pins;
    uint16_t pull;
} locosto_gpio_s;

typedef struct emif_state {
   uint32_t lru;
   uint32_t conf;
   uint32_t scs0;
   uint32_t scs1;
   uint32_t scs2;
   uint32_t scs3;

   uint32_t ecs0;
   uint32_t ecs1;
   uint32_t ecs2;
   uint32_t ecs3;
   uint32_t dynwait;
} locosto_emif_s;

typedef struct peek_state {
    CPUState *env;
    qemu_irq *irq[2];
    qemu_irq *drq;
    uint32_t dpll_ctrl;
    uint32_t dpll_clkout;
    int mouse_buttons;
    int mouse_dx;
    int mouse_dy;
    int mouse_dz;
    locosto_tpu_s *tpu;
    locosto_lcd_s *lcd;
    locosto_gpio_s *gpio[3];
    struct locosto_i2c_s *i2c[2];
    struct locosto_uart_s *uart;
    struct locosto_intr_handler_s *ih;
    locosto_clkm clkm;
    locosto_emif_s emif;
} peek_state_s;

/* UARTs */
typedef struct locosto_uart_s {
    SerialState *serial; /* TODO */
    qemu_irq irq;

    //uint8_t eblr;
    uint8_t syscontrol;
    //uint8_t wkup;
    //uint8_t cfps;
    uint8_t mdr[2];
    uint8_t scr;
    //uint8_t clksel;
} locosto_uart_s;

/* I2C */

#define I2C_TWL3029_ADDR 0x2D
#define I2C_DEVICE1_ADDR 0x15
#define I2C_DEVICE2_ADDR 0x16
#define I2C_KPD_ADDR     0x42

struct locosto_i2c_s *locosto_i2c_init(target_phys_addr_t base,
                qemu_irq irq, qemu_irq *dma);
void locosto_i2c_reset(struct locosto_i2c_s *s);
void peek_socket_init(peek_socket_s *s);

#endif
