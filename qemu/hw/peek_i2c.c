#include "hw.h"
#include "i2c.h"
#include "peek.h"

#define OMAP2_INTR_REV    0x34
#define OMAP2_GC_REV      0x34

typedef struct locosto_i2c_s {
    i2c_slave slave;    /* this must be the first struct member */
    i2c_bus *bus;
    qemu_irq irq;
    qemu_irq drq[2];

    uint8_t revision;
    uint8_t mask;
    uint16_t stat;
    uint16_t dma;
    uint16_t count;
    int count_cur;
    uint32_t fifo;
    int rxlen;
    int txlen;
    uint16_t control;
    uint16_t addr[2];
    uint8_t divider;
    uint8_t times[2];
    uint16_t test;
} locosto_i2c_s;

/* prototypes */
i2c_bus *locosto_i2c_bus(struct locosto_i2c_s *s);


static void locosto_i2c_interrupts_update(struct locosto_i2c_s *s)
{
    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: stat 0x%02x mask 0x%02x dma 0x%02x\n", __FUNCTION__, s->stat, s->mask, s->dma);
    qemu_set_irq(s->irq, s->stat & s->mask);
    
   // if ((s->dma >> 15) & 1)					/* RDMA_EN */
    //    qemu_set_irq(s->drq[0], (s->stat >> 3) & 1);		/* RRDY */
    //if ((s->dma >> 7) & 1)					/* XDMA_EN */
    //    qemu_set_irq(s->drq[1], (s->stat >> 4) & 1);		/* XRDY */
}

/* These are only stubs now.  */
static void locosto_i2c_event(i2c_slave *i2c, enum i2c_event event)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *)i2c;

    if ((~s->control >> 15) & 1)				/* I2C_EN */
        return;
    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: Got event 0x%02x\n", __FUNCTION__, event);
    switch (event) {
    case I2C_START_SEND:
    case I2C_START_RECV:
        s->stat |= 1 << 9;					/* AAS */
        break;
    case I2C_FINISH:
        s->stat |= 1 << 2;					/* ARDY */
        break;
    case I2C_NACK:
        s->stat |= 1 << 1;					/* NACK */
        break;
    }

    locosto_i2c_interrupts_update(s);
}

static int locosto_i2c_rx(i2c_slave *i2c)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *)i2c;
    uint8_t ret = 0;

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: Got RX event\n", __FUNCTION__);

    if ((~s->control >> 15) & 1)				/* I2C_EN */
        return -1;

    if (s->txlen)
        ret = s->fifo >> ((-- s->txlen) << 3) & 0xff;
    else
        s->stat |= 1 << 10;					/* XUDF */
    s->stat |= 1 << 4;						/* XRDY */

    locosto_i2c_interrupts_update(s);
    return ret;
}

static int locosto_i2c_tx(i2c_slave *i2c, uint8_t data)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *)i2c;

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: Got TX event\n", __FUNCTION__);

    if ((~s->control >> 15) & 1)				/* I2C_EN */
        return 1;

    if (s->rxlen < 4)
        s->fifo |= data << ((s->rxlen ++) << 3);
    else
        s->stat |= 1 << 11;					/* ROVR */
    s->stat |= 1 << 3;						/* RRDY */

    locosto_i2c_interrupts_update(s);
    return 1;
}

static void locosto_i2c_fifo_run(struct locosto_i2c_s *s)
{
    int ack = 1;

    if (!i2c_bus_busy(s->bus))
        return;

    if ((s->control >> 2) & 1) {				/* RM */
        if ((s->control >> 1) & 1) {				/* STP */
            i2c_end_transfer(s->bus);
            s->control &= ~(1 << 1);				/* STP */
            s->count_cur = s->count;
            s->txlen = 0;
        } else if ((s->control >> 9) & 1) {			/* TRX */
            while (ack && s->txlen)
                ack = (i2c_send(s->bus,
                                        (s->fifo >> ((-- s->txlen) << 3)) &
                                        0xff) >= 0);
            s->stat |= 1 << 4;					/* XRDY */
        } else {
            while (s->rxlen < 4)
                s->fifo |= i2c_recv(s->bus) << ((s->rxlen ++) << 3);
            s->stat |= 1 << 3;					/* RRDY */
        }
    } else {
        if ((s->control >> 9) & 1) {				/* TRX */
            while (ack && s->count_cur && s->txlen) {
                ack = (i2c_send(s->bus,
                                        (s->fifo >> ((-- s->txlen) << 3)) &
                                        0xff) >= 0);
                s->count_cur --;
            }
            if (ack && s->count_cur)
                s->stat |= 1 << 4;				/* XRDY */
            else
                s->stat &= ~(1 << 4);				/* XRDY */
            if (!s->count_cur) {
                s->stat |= 1 << 2;				/* ARDY */
                s->control &= ~(1 << 10);			/* MST */
            }
        } else {
            while (s->count_cur && s->rxlen < 4) {
                s->fifo |= i2c_recv(s->bus) << ((s->rxlen ++) << 3);
                s->count_cur --;
            }
            if (s->rxlen)
                s->stat |= 1 << 3;				/* RRDY */
            else
                s->stat &= ~(1 << 3);				/* RRDY */
        }
        if (!s->count_cur) {
            if ((s->control >> 1) & 1) {			/* STP */
                i2c_end_transfer(s->bus);
                s->control &= ~(1 << 1);			/* STP */
                s->count_cur = s->count;
                s->txlen = 0;
            } else {
                s->stat |= 1 << 2;				/* ARDY */
                s->control &= ~(1 << 10);			/* MST */
            }
        }
    }

    s->stat |= (!ack) << 1;					/* NACK */
    if (!ack)
        s->control &= ~(1 << 1);				/* STP */
}

void locosto_i2c_reset(struct locosto_i2c_s *s)
{
    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN, "%s: zip\n", __FUNCTION__);
    s->mask = 0;
    s->stat = 0;
    s->dma = 0;
    s->count = 0;
    s->count_cur = 0;
    s->fifo = 0;
    s->rxlen = 0;
    s->txlen = 0;
    s->control = 0;
    s->addr[0] = 0;
    s->addr[1] = 0;
    s->divider = 0;
    s->times[0] = 0;
    s->times[1] = 0;
    s->test = 0;
}

static void locosto_i2c_status_reset(struct locosto_i2c_s *s)
{
    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN, "%s: boing\n", __FUNCTION__);
    s->stat = 0;
    s->count = 0;
    s->fifo = 0;
    s->rxlen = 0;
    s->txlen = 0;
    s->test = 0;
}

static uint32_t locosto_i2c_read(void *opaque, target_phys_addr_t addr)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *) opaque;
    int offset = addr;
    uint16_t ret;

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: offset=0x%02x\n", __FUNCTION__, offset);

    switch (offset) {
    case 0x00:	/* I2C_REV */
        return s->revision;					/* REV */

    case 0x02:	/* I2C_IE */
        return s->mask;

    case 0x04:	/* I2C_STAT */
        return 0x14; //s->stat | (i2c_bus_busy(s->bus) << 12);

    case 0x08:  /* I2C_SYSS */
        return (s->control >> 15) & 1;                          /* I2C_EN */

    case 0x0A:  /* I2C_BUF */
        return s->dma;

    case 0x0C:	/* I2C_CNT */
        return s->count_cur;					/* DCOUNT */

    case 0x0E:	/* I2C_DATA */
        ret = 0;
        if (s->control & (1 << 14)) {				/* BE */
            ret |= ((s->fifo >> 0) & 0xff) << 8;
            ret |= ((s->fifo >> 8) & 0xff) << 0;
        } else {
            ret |= ((s->fifo >> 8) & 0xff) << 8;
            ret |= ((s->fifo >> 0) & 0xff) << 0;
        }
        if (s->rxlen == 1) {
            s->stat |= 1 << 15;					/* SBD */
            s->rxlen = 0;
        } else if (s->rxlen > 1) {
            if (s->rxlen > 2)
                s->fifo >>= 16;
            s->rxlen -= 2;
        } else
            /* XXX: remote access (qualifier) error - what's that?  */;
        if (!s->rxlen) {
            s->stat &= ~(1 << 3);				/* RRDY */
            if (((s->control >> 10) & 1) &&			/* MST */
                            ((~s->control >> 9) & 1)) {		/* TRX */
                s->stat |= 1 << 2;				/* ARDY */
                s->control &= ~(1 << 10);			/* MST */
            }
        }
        s->stat &= ~(1 << 11);					/* ROVR */
        locosto_i2c_fifo_run(s);
        locosto_i2c_interrupts_update(s);
        return ret;

    case 0x10:	/* I2C_SYSC */
        return 0;

    case 0x12:	/* I2C_CON */
        return s->control;

    case 0x14:	/* I2C_OA */
        return s->addr[0];

    case 0x16:	/* I2C_SA */
        return s->addr[1];

    case 0x18:	/* I2C_PSC */
        return s->divider;

    case 0x1A:	/* I2C_SCLL */
        return s->times[0];

    case 0x1C:	/* I2C_SCLH */
        return s->times[1];

    case 0x1E:	/* I2C_SYSTEST */
        if (s->test & (1 << 15)) {				/* ST_EN */
            s->test ^= 0xa;
            return s->test;
        } else
            return s->test & ~0x300f;
    }

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: offset=0x%02X\n", __FUNCTION__, offset);

    return 0;
}

static void locosto_i2c_write(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *) opaque;
    int offset = addr;
    int nack;

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: offset=0x%02x value=0x%02x\n", __FUNCTION__, offset, value);
    switch (offset) {
    case 0x00:	/* I2C_REV */
	return; // RO
    case 0x02:	/* I2C_IE */
        s->mask = value & 0x1f;
        break;

    case 0x04:	/* I2C_STAT */
        return; // RO

    case 0x08:  /* I2C_SYSS */
        return; // RO

    case 0x0A:	/* I2C_BUF */
        s->dma = value & 0x8080;
        if (value & (1 << 15))					/* RDMA_EN */
            s->mask &= ~(1 << 3);				/* RRDY_IE */
        if (value & (1 << 7))					/* XDMA_EN */
            s->mask &= ~(1 << 4);				/* XRDY_IE */
        break;

    case 0x0C:	/* I2C_CNT */
        s->count = value;					/* DCOUNT */
        break;

    case 0x0E:	/* I2C_DATA */
        if (s->txlen > 2) {
            /* XXX: remote access (qualifier) error - what's that?  */
            break;
        }
        s->fifo <<= 16;
        s->txlen += 2;
        if (s->control & (1 << 14)) {				/* BE */
            s->fifo |= ((value >> 8) & 0xff) << 8;
            s->fifo |= ((value >> 0) & 0xff) << 0;
        } else {
            s->fifo |= ((value >> 0) & 0xff) << 8;
            s->fifo |= ((value >> 8) & 0xff) << 0;
        }
        s->stat &= ~(1 << 10);					/* XUDF */
        if (s->txlen > 2)
            s->stat &= ~(1 << 4);				/* XRDY */
        locosto_i2c_fifo_run(s);
        locosto_i2c_interrupts_update(s);
        break;

    case 0x10:	/* I2C_SYSC */
        if (value & 2)
            locosto_i2c_reset(s);
        break;

    case 0x12:	/* I2C_CON */
        s->control = value & 0xcf87;

        if (~value & (1 << 15)) {                               /* I2C_EN */
            // just reset status bits, not control bits
            locosto_i2c_status_reset(s);
            break;
        }

        if ((value & (1 << 15)) && !(value & (1 << 10))) {	/* MST */
            LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: I^2C slave mode not supported\n", __FUNCTION__);
            break;
        }
        if ((value & (1 << 15)) && value & (1 << 8)) {		/* XA */
            LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: 10-bit addressing mode not supported\n", __FUNCTION__);
            break;
        }
        if ((value & (1 << 15)) && value & (1 << 0)) {		/* STT */
	    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: Starting transfer to 0x%02x\n", __FUNCTION__, s->addr[1]);
            nack = !!i2c_start_transfer(s->bus, s->addr[1],	/* SA */
                            (~value >> 9) & 1);			/* TRX */
            s->stat |= nack << 1;				/* NACK */
            s->control &= ~(1 << 0);				/* STT */
            s->fifo = 0;
            if (nack) {
                s->control &= ~(1 << 1);			/* STP */
	         LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: Sending stop bit\n", __FUNCTION__);
	    }
            else {
		LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: count_cur - 0x%02x\n", __FUNCTION__, s->count);
                s->count_cur = s->count;
                locosto_i2c_fifo_run(s);
            }
            locosto_i2c_interrupts_update(s);
        }
        break;

    case 0x14:	/* I2C_OA */
        s->addr[0] = value & 0x3ff;
        //i2c_set_slave_address(&s->slave, value & 0x7f);
        break;

    case 0x16:	/* I2C_SA */
        LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN, "%s: slave addr %x\n",
                   __FUNCTION__, value);
        s->addr[1] = value & 0x3ff;
        break;

    case 0x18:	/* I2C_PSC */
        s->divider = value;
        break;

    case 0x1A:	/* I2C_SCLL */
        s->times[0] = value;
        break;

    case 0x1C:	/* I2C_SCLH */
        s->times[1] = value;
        break;

    case 0x1E:	/* I2C_SYSTEST */
        s->test = value & 0xf80f;
        if (value & (1 << 11)){					/* SBB */
                s->stat |= 0x3f;
                locosto_i2c_interrupts_update(s);
	}
        if (value & (1 << 15))					/* ST_EN */
            LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: System Test not supported\n", __FUNCTION__);
        break;

    default:
	LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_WARN,
               "%s: offset=0x%02X value=0x%02X\n", __FUNCTION__, offset, value);
        return;
    }
}

static void locosto_i2c_writeb(void *opaque, target_phys_addr_t addr,
                uint32_t value)
{
    struct locosto_i2c_s *s = (struct locosto_i2c_s *) opaque;
    int offset = addr;// & OMAP_MPUI_REG_MASK;

    LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_INFO,
               "%s: offset=0x%02X value=0x%02X\n", __FUNCTION__, offset, value);

    switch (offset) {
    case 0x0e:	/* I2C_DATA */
        if (s->txlen > 2) {
            /* XXX: remote access (qualifier) error - what's that?  */
            break;
        }
        s->fifo <<= 8;
        s->txlen += 1;
        s->fifo |= value & 0xff;
        s->stat &= ~(1 << 10);					/* XUDF */
        if (s->txlen > 2)
            s->stat &= ~(1 << 4);				/* XRDY */
        locosto_i2c_fifo_run(s);
        locosto_i2c_interrupts_update(s);
        break;

    default:
        LOCO_DEBUG(LOCO_DEBUG_I2C, LOCO_DLVL_ERR,
               "%s: offset=0x%02X value=0x%02X\n", __FUNCTION__, offset, value);
        return;
    }
}

static CPUReadMemoryFunc *locosto_i2c_readfn[] = {
    locosto_badwidth_read16,
    locosto_i2c_read,
    locosto_badwidth_read16,
};

static CPUWriteMemoryFunc *locosto_i2c_writefn[] = {
    locosto_i2c_writeb,	/* Only the last fifo write can be 8 bit.  */
    locosto_i2c_write,
    locosto_badwidth_write16,
};

static int locosto_i2c_int(i2c_slave *i2c)
{
    return 0;
}

static I2CSlaveInfo locosto_i2c_info = {
    .qdev.name = "locosto_i2c",
    .qdev.size = sizeof(locosto_i2c_s),
    .init = locosto_i2c_int,
    .event = locosto_i2c_event,
    .recv = locosto_i2c_rx,
    .send = locosto_i2c_tx 
};

i2c_bus *locosto_i2c_bus(struct locosto_i2c_s *s)
{
    return s->bus;
}

static void locosto_register_devices(void)
{
    i2c_register_slave(&locosto_i2c_info);
}

struct locosto_i2c_s *locosto_i2c_init(target_phys_addr_t base,
                                       qemu_irq irq, qemu_irq *dma)
{
    int iomemtype;
    struct locosto_i2c_s *s = qemu_mallocz(sizeof(locosto_i2c_s));
    i2c_bus *bus = i2c_init_bus(NULL, "i2c");

    /* TODO: set a value greater or equal to real hardware */
    s->revision = 0x37;
    s->irq = irq;
    //s->drq[1] = dma[1];
    s->bus = bus;

//    i2c_create_slave(bus, "locosto_i2c", 0x0);

    locosto_i2c_reset(s);

    iomemtype = cpu_register_io_memory(locosto_i2c_readfn,
                    locosto_i2c_writefn, s);
    cpu_register_physical_memory(base, 0x7FF, iomemtype);

    return s;
}

device_init(locosto_register_devices)
