/*******************************************************************************
 *
 * UART.C
 *
 * This module allows to use the UARTs of chipset 1.5 in interrupt mode for
 * the Receive side and in polling mode for the Transmit side.
 * The driver calls a user's function when characters are received.
 *
 * (C) Texas Instruments 1999
 *
 ******************************************************************************/

#include "chipset.cfg"
#include "board.cfg"
#include "l1sw.cfg"

#include "sys_types.h"

#include "uart/traceswitch.h"
#include "uart/uart.h"

#include <string.h>

#include "memif/mem.h"

#if (L1_POWER_MGT != 0x00)
#include "uart/uart_pwr.h"
#endif
#include "sys_inth.h"

#ifdef  BTS
#include "../../drv_app/dma/dma_api.h"
#include "../../drv_app/dma/dma_message.h"
#endif

#ifdef  BTS
#define UART_HW_FLOW_CONTROL 1
#endif
#ifndef UART_HW_FLOW_CONTROL
#define UART_HW_FLOW_CONTROL 0
#endif

/*
 * Needed to reset and restart the sleep timer in case of incoming characters.
 */
#if (OP_L1_STANDALONE == 1)
  #include "uart/serialswitch_core.h"
#else
  #include "uart/serialswitch.h"
#endif

extern SYS_BOOL uart_sleep_timer_enabled;

#define BUFFER_SIZE (512) /* In bytes. */
#define FIFO_SIZE    (64) /* In bytes. */

#define STX                  0x02
#define DLE                  0x10

/*
 * TLR is used to program the RX FIFO trigger levels. FCR[7:4] are  not used.
 */

#define RX_FIFO_TRIGGER_LEVEL (12 << 4)

#if (CHIPSET == 15)
/*
  * Static Switch configuration (between MCU and DSP) register for UART
  */
  #define UART1_SSW_MCU_CONF  0xFFFF7280
  #define UART1_MCU_SWITCH       (1<<0)
  #define UART1_DSP_SWITCH        (1<<1)

/*
  * Module version register
  */

  #define MVR (0x14)

/*
  * System configuration register
  */
  #define SYSC (0x15) /* offset */
  #define SYSC_AUTOIDLE (1<<0)
  #define SYSC_SOFTRESET (1<<1)
  #define SYSC_ENAWAKEUP (1<<2)
  #define SYSC_NOIDLEMODE (1<<3)
  #define SYSC_SMARTIDLEMODE (1<<4)



/*
  * System status register
  */
  #define SYSS (0x16)
  #define SYSS_RESETDONE (1<<0)

/*
  * Wake up enable register
  */
  #define WER (0x17)
  #define WER_CTS (1<<0)
  #define WER_DSR (1<<1)
  #define WER_RI (1<<2)
  #define WER_DCD (1<<3)
  #define WER_RX (1<<4)
  #define WER_RHR (1<<5)
  #define WER_RLS (1<<6)
#endif

/*
 * 16750 addresses. Registers accessed when LCR[7] = 0.
 */

#define RHR (0x00) /* Rx buffer register - Read access   */
#define THR (0x00) /* Tx holding register - Write access */
#define IER (0x01) /* Interrupt enable register          */

/*
 * 16750 addresses. Registers accessed when LCR[7] = 1.
 */

#define DLL (0x00) /* Divisor latch (LSB) */
#define DLM (0x01) /* Divisor latch (MSB) */


/*
 * EFR is accessed when LCR[7:0] = 0xBF.
 */

#define EFR (0x02) /* Enhanced feature register */


/*
 * 16750 addresses. Bit 5 of the FCR register is accessed when LCR[7] = 1.
 */

#define IIR  (0x02) /* Interrupt ident. register - Read only */
#define FCR  (0x02) /* FIFO control register - Write only    */
#define LCR  (0x03) /* Line control register                 */
#define MCR  (0x04) /* Modem control register                */
#define LSR  (0x05) /* Line status register                  */
#define MSR  (0x06) /* Modem status register                 */
#define TCR  (0x06) /* Transmission control register         */
#define TLR  (0x07) /* Trigger level register                */
#define MDR1 (0x08) /* Mode definition register 1            */
#define SCR  (0x10) /* Supplementary Control register        */
#define SSR  (0x11) /* Supplementary Status register         */


/*
 * Supplementary control register.
 */

#define TX_EMPTY_CTL_IT (0x08)
#define RX_CTS_WAKE_UP_ENABLE_BIT (4) /* Use RESET_BIT and SET_BIT macros. */
#if (CHIPSET == 15)
  #define TX_TRIG_GRANU1 (1<<6)
  #define RX_TRIG_GRANU1 (1<<7)
#endif
/*
 * Enhanced feature register.
 */

#define ENHANCED_FEATURE_BIT (4) /* Use RESET_BIT and SET_BIT macros. */

#if (UART_HW_FLOW_CONTROL==1)
#define AUTO_CTS_ENABLED     (7) /* Use RESET_BIT and SET_BIT macros. */
#define AUTO_RTS_ENABLED     (6) /* Use RESET_BIT and SET_BIT macros. */
#endif


/*
 * Mode definition register 1.
 */
#if (CHIPSET == 15)
  #define UART_MODE_16x             (0x00)
#else
  #define UART_MODE             (0x00)
#endif
#define SIR_MODE              (0x01)
#if (CHIPSET == 15)
  #define UART_MODE_16xAUTOBAUDING (0x02) /* Reserved in UART/IrDA. */
  #define UART_MODE_13x            (0x03)
#else
  #define UART_MODE_AUTOBAUDING (0x02) /* Reserved in UART/IrDA. */
#endif
#define RESET_DEFAULT_STATE   (0x07)
#define IR_SLEEP_DISABLED     (0x00)
#define IR_SLEEP_ENABLED      (0x08)
#define SIR_TX_WITHOUT_ACREG2 (0x00) /* Reserved in UART/modem. */
#define SIR_TX_WITH_ACREG2    (0x20) /* Reserved in UART/modem. */
#define FRAME_LENGTH_METHOD   (0x00) /* Reserved in UART/modem. */
#define EOT_BIT_METHOD        (0x80) /* Reserved in UART/modem. */

/*
 * Supplementary Status Register
 */

#define TX_FIFO_FULL (0x01)


/*
 * Interrupt enable register.
 */

#define ERBI  (0x01) /* Enable received data available interrupt            */
#define ETBEI (0x02) /* Enable transmitter holding register empty interrupt */
#define ELSI  (0x04) /* Enable receiver line status interrupt               */
#define EDSSI (0x08) /* Enable modem status interrupt                       */
#define IER_SLEEP (0x10)  /* Enable sleep mode                              */

/*
 * Modem control register.
 */

#define MDTR (0x01) /* Data terminal ready. */
#define MRTS (0x02) /* Request to send.     */
#define TCR_TLR_BIT (6)

/*
 * Line status register.
 */

#define DR   (0x01) /* Data ready                                  */
#define OE   (0x02) /* Overrun error                               */
#define PE   (0x04) /* Parity error                                */
#define FE   (0x08) /* Framing error                               */
#define BI   (0x10) /* Break interrupt                             */
#define THRE (0x20) /* Transmitter holding register (FIFO empty)   */
#define TEMT (0x40) /* Transmitter empty (FIFO and TSR both empty) */

/*
 * Interrupt identification register.
 * Bit 0 is set to 0 if an IT is pending.
 * Bits 1 and 2 are used to identify the IT.
 */

#define IIR_BITS_USED  (0x07)
#define IT_NOT_PENDING (0x01)
#define RX_DATA        (0x04)
#define TX_EMPTY       (0x02)
#define MODEM_STATUS   (0x00)

/*
 * Line control register.
 */

#define WLS_5         (0x00) /* Word length: 5 bits                    */
#define WLS_6         (0x01) /* Word length: 6 bits                    */
#define WLS_7         (0x02) /* Word length: 7 bits                    */
#define WLS_8         (0x03) /* Word length: 8 bits                    */
#define STB           (0x04) /* Number of stop bits: 0: 1, 1: 1,5 or 2 */
#define PEN           (0x08) /* Parity enable                          */
#define EPS           (0x10) /* Even parity select                     */
#define BREAK_CONTROL (0x40) /* Enable a break condition               */
#define DLAB          (0x80) /* Divisor latch access bit               */
#define DIV_EN_BIT    (7)

/*
 * FIFO control register.
 */

#define FIFO_ENABLE   (0x01)
#define RX_FIFO_RESET (0x02)
#define TX_FIFO_RESET (0x04)

#ifdef  BTS
/*
 * Limited by DMA register CFN 16-bits
 */
#define TX_DMA_THRESHOLD (65535)

#define TX_FIFO_TRIGGER_CLEAR (0x0)
#define TX_FIFO_TRIGGER_LEVEL (0x1 << 4)

/*
 * SCR register.
 * Enable control of DMA configuration
 * with settings in SCR register
 */

#define SCR_DMA_CTRL        (0x01)
#define SCR_DMA_CTRL_TXDMA  (0x06)
#define SCR_TX_GRAN_CTRL    6
#endif

/*
 * These macros allow to read and write a UART register.
 */

#if (CHIPSET == 15)
#define SEPERATOR 2
#else
#define SEPERATOR 1
#endif

#define READ_UART_REGISTER(UART,REG) \
            *((volatile SYS_UWORD8 *) ((UART)->base_address + ((REG)*SEPERATOR)))

#define WRITE_UART_REGISTER(UART,REG,VALUE) \
            *((volatile SYS_UWORD8 *) ((UART)->base_address + ((REG)*SEPERATOR))) = (VALUE)

#define RESET_BIT(UART,REG,BIT)    \
            (WRITE_UART_REGISTER ( \
                 UART, REG, READ_UART_REGISTER (UART, REG) & ~(1 << (BIT))))

#define SET_BIT(UART,REG,BIT)      \
            (WRITE_UART_REGISTER ( \
                 UART, REG, READ_UART_REGISTER (UART, REG) | (1 << (BIT))))

/*
 * These macros allow to enable or disable the wake-up interrupt.
 */

#define ENABLE_WAKEUP_INTERRUPT(UART)   \
    SET_BIT(UART, SCR, RX_CTS_WAKE_UP_ENABLE_BIT);

#define DISABLE_WAKEUP_INTERRUPT(UART)   \
    RESET_BIT(UART, SCR, RX_CTS_WAKE_UP_ENABLE_BIT);


/*
 * This macro allows to know if the RX buffer is full. It must be called only
 * from the RX interrupt handler. If it is called from the application, the
 * rx_in pointer may be updated if a RX interrupt occurs.
 */

#define RX_BUFFER_FULL(UART)                                      \
            (((UART)->rx_in == (UART)->rx_out - 1) ||             \
             ((UART)->rx_in == (UART)->rx_out + BUFFER_SIZE - 1))


/*
 * This allows monitor the last 32 inbound buffers gotten from the RX FIFO.
 */

//#define UART_RX_BUFFER_DUMP

#ifdef UART_RX_BUFFER_DUMP
struct {
    char rx_buffer[(BUFFER_SIZE + 1) << 5];
    char *rx_in;
    int  errors_count;
    int  wrong_interrupt_status;
} uart_rx_buffer_dump = {0};
#endif


typedef struct s_uart {

    SYS_UWORD32 base_address;

    /*
     * Buffers management.
     */

    char rx_buffer[BUFFER_SIZE + 1];
    char *rx_in;
    char *rx_out;

    /*
     * Errors counters.
     */

    SYS_UWORD32 framing_error;
    SYS_UWORD32 parity_error;
    SYS_UWORD32 overrun_error;

    /*
     * Framing flags.
     */

    SYS_BOOL dle_detected;
    SYS_BOOL inframe;
    SYS_BOOL encapsulation_flag;
    unsigned char frame_length;

/*-----------------------IMPORTANT-----------------------------------------------
    Separate callback function is not needed, as it is mapped in BT as xLisrFunc in
    file Uart_bts.c. The xLisrFunc internally calls TxHisr or RxHisr depending on the
    IIR.
    For that we need to define BTS_TX_INT.
---------------------------------------------------------------------------------*/
#ifdef BTS_TX_INT
    void (*callback_function) (SYS_UWORD8);
#else
    void (*callback_function) (void);
#endif
#ifdef  BTS
    /*
     * Tx Buffers management.
     */
    char        *tx_buffer;        // Pointer to base adress of user buffer
    UINT16      tx_cur_pos;       // Current position in the buffer
    UINT16      tx_size;           // buffer size to be transmitted
    SYS_BOOL    is_transmitting;

    /*
     * Will be used for TX-DMA transfer
     */
    T_RV_RETURN              uart_dma_return_path;
    T_DMA_CHANNEL            uart_dma_channel;
    T_DMA_CHANNEL_PARAMETERS uart_dma_channel_info;
    UINT8                    uart_dma_peripheral;
#endif
} t_uart;

static t_uart uart_parameter[NUMBER_OF_TR_UART];

static const SYS_UWORD32 base_address[NUMBER_OF_TR_UART] =
{
#if (CHIPSET == 15)
  MEM_UART_MODEM_IRDA
#else
#if (((BOARD != 35) && (BOARD != 46)) || (GSM_IDLE_RAM == 0))
  MEM_UART_IRDA,
#endif
  MEM_UART_MODEM
#if (CHIPSET == 12)
  , MEM_UART_MODEM2
#endif
#endif
};


#if (CHIPSET == 15)

/*
 * DLL (LSB) and DLH (MSB) registers values using the 48 MHz clock.
 */

static const SYS_UWORD8 dll[] =
{
      0x01, /* 3.6864 Mb/s baud. */
      0x02, /* 1.8342 Mb/s baud. */
      0x04, /*   921.6 Kb/s baud. */
      0x08, /*   460.8 Kb/s baud. */
      0x0D, /*   230.4 Kb/s baud. */
      0x1A, /*   115.2 Kb/s baud. */
      0x34, /*     57.6 Kb/s baud. */
      0x4E, /*     38.4 Kb/s baud. */
      0x68, /*     28.8 Kb/s baud. */
      0x9C, /*     19.2 Kb/s baud. */
      0xD0, /*     14.4 Kb/s baud. */
      0x39, /*       9.6 Kb/s baud. */
      0x71, /*       4.8 Kb/s baud. */
      0xE2, /*       2.4 Kb/s baud. */
      0xC4, /*       1.2 Kb/s baud. */
      0x88, /*       0.6 Kb/s baud. */
      0x10  /*       0.3 Kb/s baud. */
};

static const SYS_UWORD8 dlh[] =
{
      0x00, /* 3.6864 Mb/s baud. */
      0x00, /* 1.8342 Mb/s baud. */
      0x00, /*   921.6 Kb/s baud. */
      0x00, /*   460.8 Kb/s baud. */
      0x00, /*   230.4 Kb/s baud. */
      0x00, /*   115.2 Kb/s baud. */
      0x00, /*     57.6 Kb/s baud. */
      0x00, /*     38.4 Kb/s baud. */
      0x00, /*     28.8 Kb/s baud. */
      0x00, /*     19.2 Kb/s baud. */
      0x00, /*     14.4 Kb/s baud. */
      0x01, /*       9.6 Kb/s baud. */
      0x02, /*       4.8 Kb/s baud. */
      0x04, /*       2.4 Kb/s baud. */
      0x09, /*       1.2 Kb/s baud. */
      0x13, /*       0.6 Kb/s baud. */
      0x27  /*       0.3 Kb/s baud. */
};

#else

/*
 * DLL (LSB) and DLH (MSB) registers values using the 13 MHz clock.
 */

static const SYS_UWORD8 dll[] =
{
      2, /* 406250 baud. */
      7, /* 115200 baud. */
     14, /*  57600 baud. */
     21, /*  38400 baud. */
     24, /*  33900 baud. */
     28, /*  28800 baud. */
     42, /*  19200 baud. */
     56, /*  14400 baud. */
     84, /*   9600 baud. */
    169, /*   4800 baud. */
     83, /*   2400 baud. */
    165, /*   1200 baud. */
     74, /*    600 baud. */
    148, /*    300 baud. */
     40, /*    150 baud. */
     81  /*     75 baud. */
};

static const SYS_UWORD8 dlh[] =
{
     0, /* 406250 baud. */
     0, /* 115200 baud. */
     0, /*  57600 baud. */
     0, /*  38400 baud. */
     0, /*  33900 baud. */
     0, /*  28800 baud. */
     0, /*  19200 baud. */
     0, /*  14400 baud. */
     0, /*   9600 baud. */
     0, /*   4800 baud. */
     1, /*   2400 baud. */
     2, /*   1200 baud. */
     5, /*    600 baud. */
    10, /*    300 baud. */
    21, /*    150 baud. */
    42  /*     75 baud. */
};

#endif

#ifdef BTS_TX_INT
/*******************************************************************************
 *
 *                       uart_subscribe_dma_channel
 *
 * Purpose:     Acquired DMA channel for current transfer
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#if CHIPSET == 15
T_RV_RET
uart_subscribe_dma_channel(T_tr_UartId uart_id, t_uart *uart);
#endif

/*******************************************************************************
 *
 *                       uart_unsubscribe_dma_channel
 *
 * Purpose:     Release acquired DMA channel
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#if CHIPSET == 15
T_RV_RET
uart_unsubscribe_dma_channel(t_uart *uart);
#endif

/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback1
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-1.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/

void uart_tx_dma_status_callback1(void *response);
#if (CHIPSET!=15)
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback2
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-2.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/

void uart_tx_dma_status_callback2(void *response);
#if (CHIPSET == 12)
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback3
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-3.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/

void uart_tx_dma_status_callback3(void *response);
 #endif
 #endif
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback
 *
 * Purpose:     Handle DMA status messages.
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *                   void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/

void uart_tx_dma_status_callback(t_uart* uart, void *response);

/*******************************************************************************
 *
 *                       uart_tx_dma
 *
 * Purpose:     Initiate process to start a DMA transfer
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

T_RV_RET
uart_tx_dma(T_tr_UartId uart_id, t_uart *uart);

/*******************************************************************************
 *
 *                           fill_tx_fifo
 *
 * Purpose:     If more amount of data is available to be written, write it to
 *              TX fifo, else flag completion to the higher layer.
 *
 * Arguments:   In : t_uart*       : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     Number of bytes written.
 *
 *
 * Warning:     Parameters are not verified.
 ******************************************************************************/

void
fill_tx_fifo(t_uart *uart);

/*******************************************************************************
 *
 *                       uart_start_tx_dma_transfer
 *
 * Purpose:     Sets DMA parameters for the current dma request
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

T_RV_RET
uart_start_tx_dma_transfer(t_uart* uart);

#endif //BTS_TX_INT

/*******************************************************************************
 *
 *                              read_rx_fifo
 *
 * Purpose  : Check the bytes written into the RX FIFO. Characters are not
 *            written in the RX buffer if it is full. The HISR is called if
 *            enough characters are received.
 *
 * Arguments: In : uart: pointer on UART structure.
 *            Out: none
 *
 * Returns  : none
 *
 ******************************************************************************/

static void
read_rx_fifo (t_uart *uart)
{
    volatile SYS_UWORD8 status;
    int                 error_detected;
    SYS_UWORD8          char_received=0;

    /*
     * Since new characters have been received, the sleep timer is reset then
     * restarted preventing the system to enter deep-sleep for a new period of
     * time.
     */

    /* DAR_ER */
    if(uart->callback_function)
    {
    SER_activate_timer_hisr ();
    uart_sleep_timer_enabled = 1;
    }

    status = READ_UART_REGISTER (uart, LSR);

    while (status & DR) { /* While RX FIFO is not empty... */

        error_detected = 0;

		#ifdef UART_HW_FLOW_CONTROL
		if(!RX_BUFFER_FULL (uart))
		{
    	#endif
        char_received = READ_UART_REGISTER (uart, RHR);
		#ifdef UART_HW_FLOW_CONTROL
		}
		else
		{
			SYS_UWORD8 temp;
			//MASK RX Interrupt
			if(uart->callback_function)
            {
			temp = READ_UART_REGISTER(uart,IER);
			temp &= ~(ERBI);
			WRITE_UART_REGISTER (uart, IER, temp);
			break;
		}
		}
		#endif

        /*
         * Check if an error (overrun, parity, framing or break) is associated with the
         * received data. If there is an error the byte is not copied into the
         * RX buffer.
         */

        if (status & (OE | PE | FE | BI)) {

            if (status & PE)
                uart->parity_error++;

            if (status & FE)
                uart->framing_error++;

            if (status & OE)
                uart->overrun_error++;

            error_detected = 1;
        }

        /*
         * If there is no error the byte is copied into the RX
         * buffer if it is not full.
         */

        if (!error_detected && !RX_BUFFER_FULL (uart)) {

            *(uart->rx_in++) = char_received;

            if (uart->rx_in == &(uart->rx_buffer[0]) + BUFFER_SIZE + 1)
                uart->rx_in = &(uart->rx_buffer[0]);

#ifdef UART_RX_BUFFER_DUMP
            *(uart_rx_buffer_dump.rx_in)++ = char_received;

            if (uart_rx_buffer_dump.rx_in == uart_rx_buffer_dump.rx_buffer + sizeof (uart_rx_buffer_dump.rx_buffer))
                uart_rx_buffer_dump.rx_in = uart_rx_buffer_dump.rx_buffer;
        }
        else {
            uart_rx_buffer_dump.errors_count++;
#endif
        }

        status = READ_UART_REGISTER (uart, LSR);
    }

    /*
     * Call the user's function.
     */
#ifndef BTS_TX_INT
    if (uart->callback_function != NULL)
        (*(uart->callback_function)) ();
#else
    if (uart->callback_function != NULL)
        (*(uart->callback_function)) (RX_DATA);
#endif
}

/*******************************************************************************
 *
 *                           initialize_uart_sleep
 *
 * Purpose  : Performs basic UART hardware initialization including sleep mode.
 *
 * Arguments: In : uart_id : UART id.
 *            Out: none
 *
 * Returns: none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void
initialize_uart_sleep (T_tr_UartId uart_id)
{
    t_uart     *uart;
    int        index;
    SYS_UWORD8 dummy;

    for (index = 0; index < NUMBER_OF_TR_UART; index++)
        uart_parameter[index].base_address = base_address[index];

    uart = &(uart_parameter[uart_id]);

    /*
     * Mask all interrupts causes and disable sleep mode.
     */

    WRITE_UART_REGISTER (uart, IER, 0x00);

    /*
     * Reset UART mode configuration.
     */

    WRITE_UART_REGISTER (uart, MDR1, RESET_DEFAULT_STATE);

    /*
     * LCR[7:0] = 0xBF to allow to access EFR
     * EFR[4] = 1 to allow to program IER[4].
     */

    WRITE_UART_REGISTER (uart, LCR, 0xBF);
    SET_BIT (uart, EFR, ENHANCED_FEATURE_BIT);
    WRITE_UART_REGISTER (uart, LCR, 0x83);

    /*
     * Enable FIFO and reset them.
     */

    WRITE_UART_REGISTER (uart, FCR, FIFO_ENABLE   |
                                    RX_FIFO_RESET |
                                    TX_FIFO_RESET);

    /*
     * Program the baud generator (dummy 115200).
     */

    WRITE_UART_REGISTER (uart, DLL, 0x07);
    WRITE_UART_REGISTER (uart, DLM, 0x00);

    /*
     * LCR[7] = 0 to allow to access IER and RHR - normal mode.
     */

    RESET_BIT (uart, LCR, DIV_EN_BIT);

    /*
     * Select UART mode.
     */
#if (CHIPSET == 15)
    WRITE_UART_REGISTER (uart, MDR1, UART_MODE_16x);
#else
    WRITE_UART_REGISTER (uart, MDR1, UART_MODE);
#endif

    /*
     * Clear Interrupt and check that Rx FIFO is empty.
     */

    dummy = READ_UART_REGISTER (uart, IIR);

    while (READ_UART_REGISTER (uart, LSR) & DR)
        dummy = READ_UART_REGISTER (uart, RHR);

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Enable sleep mode.
     */

    WRITE_UART_REGISTER (uart, IER, IER_SLEEP);
#endif
}


/*******************************************************************************
 *
 *                               UA_Init
 *
 * Purpose  : Initializes the module and the UART.
 *
 * Arguments: In : uart_id : UART id.
 *                 baudrate: baud rate selected.
 *                 callback: user's function called characters are received.
 *            Out: none
 *
 * Returns: none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

#ifndef BTS_TX_INT
void
UA_Init (T_tr_UartId uart_id, T_tr_Baudrate baudrate,
         void (callback_function (void)))
#else
/*
 * UA_Init function takes function pointer to void callback_function(void) whereas
 * the xLisrFunc used when BTS_TX_INT flag is defined has  the following function
 * signature void xLisrFunc(SYS_UWORD8 iirStatus).
 *In that case we need to modify the UA_Init as
 */
void
UA_Init (T_tr_UartId uart_id, T_tr_Baudrate baudrate,
              void (callback_function (SYS_UWORD8)))
#endif
{
    t_uart *uart;
    int    index;
    int    debug;

#if (CHIPSET == 15)
    /* Enable MCU access to UART1 */

    /* Set the MCU_SWITCH bit in the SSW_MCU_CONF register (MCU software : write 1)	*/
    *(volatile SYS_UWORD8 *)(UART1_SSW_MCU_CONF) = *(volatile SYS_UWORD8 *)(UART1_SSW_MCU_CONF) | UART1_MCU_SWITCH;
#endif

#ifdef UART_RX_BUFFER_DUMP
    uart_rx_buffer_dump.rx_in = uart_rx_buffer_dump.rx_buffer;
#endif

    for (index = 0; index < NUMBER_OF_TR_UART; index++)
        uart_parameter[index].base_address = base_address[index];

    uart = &(uart_parameter[uart_id]);

    uart->rx_in  = &(uart->rx_buffer[0]);
    uart->rx_out = &(uart->rx_buffer[0]);

    uart->callback_function = callback_function;

    uart->framing_error = 0;
    uart->parity_error  = 0;
    uart->overrun_error = 0;

    uart->dle_detected = 0;
    uart->inframe = 0;
    uart->encapsulation_flag = 0;
    uart->frame_length = 0;

#if (CHIPSET == 15)
    /*
      * SYSC configuration :
      * AutoIdle:  0, Clock is running
      * SoftReset:  1, Reset the module
      * EnaWakeup: 0, wake up feature is disabled
      * IdleMode: 2, smart  idle mode is enabled
      */
    WRITE_UART_REGISTER(uart,SYSC,SYSC_SOFTRESET | SYSC_SMARTIDLEMODE|SYSC_AUTOIDLE );

    /*
      * Wait until reset is over
      */
    while((READ_UART_REGISTER(uart, SYSS) & SYSS_RESETDONE) == 0x00);

    /*
      * configure WER register : None of the events are allowed to wake up the system
      */
    WRITE_UART_REGISTER(uart, WER, 0x00);

#endif

    /*
     * Mask all interrupts causes and disable sleep mode.
     */

    WRITE_UART_REGISTER (uart, IER, 0x00);

    /*
     * Reset UART mode configuration.
     */

    WRITE_UART_REGISTER (uart, MDR1, RESET_DEFAULT_STATE   |
                                     IR_SLEEP_DISABLED     |
                                     SIR_TX_WITHOUT_ACREG2 |
                                     FRAME_LENGTH_METHOD);

    /*
     * FIFO configuration.
     * EFR[4] = 1 to allow to program FCR[5:4] and MCR[7:5].
     */

    WRITE_UART_REGISTER (uart, LCR, 0xBF);

    SET_BIT (uart, EFR, ENHANCED_FEATURE_BIT);

	/* Enable Hardware Flow Control As compile time option
	*/
#if (UART_HW_FLOW_CONTROL==1)
  if(uart->callback_function)
  {
	SET_BIT (uart, EFR, AUTO_CTS_ENABLED);
	SET_BIT (uart, EFR, AUTO_RTS_ENABLED);
  }
  else
  {
    RESET_BIT (uart, EFR, AUTO_CTS_ENABLED);
    RESET_BIT (uart, EFR, AUTO_RTS_ENABLED);
  }
#endif

    /*
     * Select the word length, the number of stop bits , the parity and set
     * LCR[7] (DLAB) to allow to program FCR, DLL and DLM.
     */
    WRITE_UART_REGISTER (uart, LCR, 0x00);


#ifdef BTS_TX_INT
/*---->> Sidhu For DMA set the TX GRAN to 1 */
    SET_BIT (uart, SCR, SCR_TX_GRAN_CTRL/*CTRL Tx Gran-1*/);
/*---->> Sidhu For DMA set the TX GRAN to 1 */
    WRITE_UART_REGISTER (uart, FCR, FIFO_ENABLE   |
                                    RX_FIFO_RESET | /* self cleared */
                                    TX_FIFO_RESET | 0x10); /* self cleared */
#else
    /*
     * Program the FIFO control register. Bit 0 must be set when other FCR bits
     * are written to or they are not programmed.
     * FCR is a write-only register. It will not be modified.
     */
    WRITE_UART_REGISTER (uart, FCR, FIFO_ENABLE   |
                                    RX_FIFO_RESET | /* self cleared */
                                    TX_FIFO_RESET | 0x10); /* self cleared */
#endif

    WRITE_UART_REGISTER (uart, LCR, 0xBF);

    RESET_BIT (uart, EFR, ENHANCED_FEATURE_BIT);

    WRITE_UART_REGISTER (uart, LCR, WLS_8 | DLAB);

    /*
     * Program the trigger levels.
     * MCR[6] must be set to 1.
     */

    SET_BIT (uart, MCR, TCR_TLR_BIT);
    /* Setting TCR Halt trigger Level to 60
       And Resume level to Half the RX_FIFO_TRIGGER  */
    WRITE_UART_REGISTER (uart, TCR,((RX_FIFO_TRIGGER_LEVEL>>2)&0xF0)|0x0F);
    WRITE_UART_REGISTER (uart, TLR, RX_FIFO_TRIGGER_LEVEL);

    /*
     * Program the baud generator.
     */

    WRITE_UART_REGISTER (uart, DLL, dll[baudrate]);
    WRITE_UART_REGISTER (uart, DLM, dlh[baudrate]);


    /*
     * Reset LCR[7] (DLAB) to have access to the RBR, THR and IER registers.
     */

    WRITE_UART_REGISTER (uart, LCR, READ_UART_REGISTER (uart, LCR) & ~DLAB);


    /*
     * Select UART mode.
     */
#if (CHIPSET == 15)
    if(baudrate <= TR_BAUD_460800)
    {
     /* 13x mode */
      WRITE_UART_REGISTER (uart, MDR1, UART_MODE_13x |
                                     IR_SLEEP_DISABLED     |
                                     SIR_TX_WITHOUT_ACREG2 |
                                     FRAME_LENGTH_METHOD);
    }
    else
    {
      /* 16x mode */
      WRITE_UART_REGISTER (uart, MDR1, UART_MODE_16x |
                                     IR_SLEEP_DISABLED     |
                                     SIR_TX_WITHOUT_ACREG2 |
                                     FRAME_LENGTH_METHOD);
    }
#else
    WRITE_UART_REGISTER (uart, MDR1, UART_MODE             |
                                     IR_SLEEP_DISABLED     |
                                     SIR_TX_WITHOUT_ACREG2 |
                                     FRAME_LENGTH_METHOD);
#endif

#if ((CHIPSET == 5) || (CHIPSET == 6))
    /*
     * Unmask RX interrupt
     */

    WRITE_UART_REGISTER (uart, IER, ERBI);
#else
    /*
     * Unmask RX interrupt and allow sleep mode.
     */

    /* DAR_ER */
    if (uart->callback_function)
    {
    WRITE_UART_REGISTER (uart, IER, ERBI | IER_SLEEP);
    }
    else
    {
        /* No Callback No Read interrupt */
        WRITE_UART_REGISTER (uart, IER,IER_SLEEP);
    }
#endif
}

/*******************************************************************************
 *
 *                           UA_ReadNChars
 *
 * Purpose  : Reads N characters from the RX buffer.
 *
 * Arguments: In : uart_id      : UART id.
 *                 buffer       : buffer address where the characters are
 *                                copied.
 *                 chars_to_read: number of characters to read.
 *            Out: none
 *
 * Returns  : The number of characters read.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#if (OP_L1_STANDALONE == 0)
#pragma CODE_SECTION(UA_ReadNChars, ".uartintram")
#endif
SYS_UWORD32
UA_ReadNChars (T_tr_UartId uart_id,
               char *buffer,
               SYS_UWORD32 chars_to_read)
{
    SYS_UWORD32 chars_in_rx_buffer;
    SYS_UWORD32 chars_to_copy;
    SYS_UWORD32 chars_written;
    char        *rx_in;
    t_uart      *uart;

    uart = &(uart_parameter[uart_id]);

	#if (L1_POWER_MGT!=0)
		#if (OP_L1_STANDALONE == 0)
			Uart_Vote_DeepSleepStatus();
		#endif
	#endif
    /* DAR_ER */
    if(!(uart->callback_function))
    {
        read_rx_fifo(uart);
    }
    /*
     * A copy of the rx_in pointer is used because it may be updated by
     * the interrupt handler.
     * Get the number of bytes available in the RX buffer.
     */

    rx_in = uart->rx_in;

    if (uart->rx_out <= rx_in)
        chars_in_rx_buffer = (SYS_UWORD32) (rx_in - uart->rx_out);
    else
        chars_in_rx_buffer = (SYS_UWORD32) (rx_in - uart->rx_out + BUFFER_SIZE + 1);

    /*
     * No more bytes than those received may be written in the output buffer.
     */

    if (chars_in_rx_buffer >= chars_to_read)
        chars_to_copy = chars_to_read;
    else
        chars_to_copy = chars_in_rx_buffer;

    chars_written = chars_to_copy;

    /*
     * Write the received bytes in the output buffer.
     */

    while (chars_to_copy) {

        *(buffer++) = *(uart->rx_out++);
        chars_to_copy--;

        if (uart->rx_out == &(uart->rx_buffer[0]) + BUFFER_SIZE + 1)
            uart->rx_out = &(uart->rx_buffer[0]);
    }
	#ifdef UART_HW_FLOW_CONTROL
	// Just in case RX interrupts got disabled in HW flow control
	//WRITE_UART_REGISTER (uart, IER, ERBI);
	if((uart->callback_function))
    {
	WRITE_UART_REGISTER (uart, IER,READ_UART_REGISTER (uart, IER) | ERBI);
    }
	#endif

    return (chars_written);
}

/*******************************************************************************
 *
 *                           UA_ReadNBytes
 *
 * Purpose  : Reads and destuff N bytes from the RX buffer.
 *
 * Arguments: In : uart_id      : UART id.
 *                 buffer       : buffer address where the bytes are copied.
 *                 chars_to_read: number of bytes to read.
 *            Out: eof_detected : indicates if an EOF has been detected. Possible
 *                                values are:
 *                                 - 0: EOF not detected,
 *                                 - 1: EOF detected and no more bytes left,
 *                                 - 2: EOF not detected and more bytes left.
 *                                      Users must invoke this function one more
 *                                      time in order to get those remaining
 *                                      bytes,
 *                                 - 3: EOF detected and more bytes left. Users
 *                                      must invoke this function one more time
 *                                      in order to get those remaining bytes.
 *
 * Returns  : The number of bytes read.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

SYS_UWORD32
UA_ReadNBytes (T_tr_UartId uart_id,
               char *buffer_p,
               SYS_UWORD32 bytes_to_read,
               SYS_BOOL *eof_detected_p)
{
    SYS_UWORD32 bytes_written;
    SYS_UWORD32 bytes_in_rx_buffer;
    SYS_UWORD32 bytes_to_process;
    t_uart      *uart_p;
    char        *rx_in_p;

    bytes_written = 0;
    uart_p = &(uart_parameter[uart_id]);

	#if (L1_POWER_MGT!=0)
		#if (OP_L1_STANDALONE == 0)
			Uart_Vote_DeepSleepStatus();
		#endif
	#endif

    /*
     * A copy of the rx_in pointer is used because it may be updated by
     * the interrupt handler.
     * Get the number of bytes available in the RX buffer.
    */

    rx_in_p = uart_p->rx_in;

    if (uart_p->rx_out <= rx_in_p)
        bytes_in_rx_buffer = (SYS_UWORD32) (rx_in_p - uart_p->rx_out);
    else
        bytes_in_rx_buffer = (SYS_UWORD32) (rx_in_p - uart_p->rx_out + BUFFER_SIZE + 1);

    /*
     * No more bytes than those received may be processed and then written
     * in the output buffer.
     */

    if (bytes_in_rx_buffer > bytes_to_read) {
        bytes_to_process = bytes_to_read;

        /*
         * More bytes left. Users must invoke this function one more time
         * in order to get those remaining bytes.
         */

        *eof_detected_p  = 2;
    }
    else {
        bytes_to_process = bytes_in_rx_buffer;

        /*
         * No more bytes left.
         */

        *eof_detected_p  = 0;
    }

    /*
     * Perform the byte destuffing and then write the "valid" received bytes in
     * the output buffer.
     */

    while ((bytes_to_process) && !(*eof_detected_p & 0x01)) {

        switch (*(uart_p->rx_out)) {

            /*
             * Current byte is DLE.
             */

            case DLE:

                if (!uart_p->dle_detected) {

                    /*
                     * No DLE previously detected =>
                     * Skip the current byte and set the flag.
                     */

                    uart_p->dle_detected = 1;
                    uart_p->rx_out++;
                }

                else { /* if (uart_p->dle_detected) */

                    if (uart_p->inframe) {

                        /*
                         * DLE previously detected AND currently inside of a frame =>
                         * Copy the current byte in the output buffer, reset the flag
                         * and increase the frame length.
                         */

                        uart_p->dle_detected = 0;
                        uart_p->frame_length++;
                        *(buffer_p++) = *(uart_p->rx_out++);
                        bytes_written++;
                    }

                    else { /* if (!uart_p->inframe) */

                        /*
                         * DLE previously detected AND currently outside of a frame =>
                         * Skip the current byte.
                         */

                        uart_p->rx_out++;
                    }
                }

            break; /* case DLE */

            /*
             * Current byte is STX.
             */

            case STX:

                if ((!uart_p->dle_detected) && (uart_p->inframe)) {

                    /*
                     * No DLE previously detected AND currently inside of a frame.
                     */

                    if (uart_p->frame_length) {

                        /*
                         * Frame length is not zero (End of Frame) =>
                         * Skip the current byte and set the flags (EOF).
                         */

                        uart_p->inframe = 0;
                        uart_p->frame_length = 0;
                        uart_p->rx_out++;

                        /*
                         * More bytes left.
                         */

                        if ((*eof_detected_p == 0) && (bytes_to_process))
                            *eof_detected_p = 2;

                        /*
                         * EOF detected.
                         */

                        (*eof_detected_p)++;
                    }

                    else { /* if (!uart_p->frame_length) */

                        /*
                         * Frame length is zero (STX followed by another STX =
                         * Synchro lost but start of a new frame) =>
                         * Skip the current byte and keep the flag set.
                         */

                        uart_p->rx_out++;
                    }
                }

                else if ((!uart_p->dle_detected) && (!uart_p->inframe)) {

                    /*
                     * No DLE previously detected AND currently outside of a
                     * frame (Start of Frame) =>
                     * Skip the current byte and set the flag.
                     */

                    uart_p->inframe = 1;
                    uart_p->rx_out++;
                }

                else if ((uart_p->dle_detected) && (uart_p->inframe)) {

                    /*
                     * DLE previously detected AND currently inside of a frame =>
                     * Copy the current byte in the output buffer, reset the flag
                     * and increase the frame length.
                     */

                    uart_p->dle_detected = 0;
                    uart_p->frame_length++;
                    *(buffer_p++) = *(uart_p->rx_out++);
                    bytes_written++;
                }

                else if ((uart_p->dle_detected) && (!uart_p->inframe)) {

                    /*
                     * DLE previously detected AND currently outside of a frame =>
                     * Skip the current byte and reset the flag.
                     */

                    uart_p->dle_detected = 0;
                    uart_p->rx_out++;
                }

            break; /* case STX */

            /*
             * Current byte is neither DLE nor STX.
             */

            default:

                if (uart_p->inframe) {

                    /*
                     * Currently inside of a frame =>
                     * Copy the current byte in the output buffer and increase
                     * the frame length.
                     */

                    uart_p->frame_length++;
                    *(buffer_p++) = *(uart_p->rx_out++);
                    bytes_written++;
                }

                else { /* if (!uart_p->inframe) */

                    /*
                     * Currently outside of a frame =>
                     * Skip the current byte.
                     */

                    uart_p->rx_out++;
                }

            break; /* default */
        }

        if (uart_p->rx_out == &(uart_p->rx_buffer[0]) + BUFFER_SIZE + 1)
            uart_p->rx_out = &(uart_p->rx_buffer[0]);

        bytes_to_process--;
    }
	#ifdef UART_HW_FLOW_CONTROL
	// Just in case RX interrupts got disabled in HW flow control
	WRITE_UART_REGISTER (uart_p, IER, ERBI);
	#endif
    return (bytes_written);
}


/*******************************************************************************
 *
 *                           UA_WriteNChars
 *
 * Purpose  : Writes N characters in the TX FIFO.
 *
 * Arguments: In : uart_id       : UART id.
 *                 buffer        : buffer address from which characters are
 *                                 written.
 *                 bytes_to_write: number of bytes to write.
 *            Out: none
 *
 * Returns  : Number of bytes written.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#ifndef BTS_TX_INT

SYS_UWORD32
UA_WriteNChars (T_tr_UartId uart_id,
                char *buffer,
                SYS_UWORD32 chars_to_write)
{
    SYS_UWORD32 chars_in_tx_fifo=0;
    SYS_UWORD32 chars_written=0;
    t_uart      *uart;

    chars_written = 0;
    uart = &(uart_parameter[uart_id]);

	#if (L1_POWER_MGT!=0)
		#if (OP_L1_STANDALONE == 0)
			Uart_Vote_DeepSleepStatus();
		#endif
	#endif


#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Disable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
#endif

    /*
     * Copy the input buffer to the TX FIFO.
     * Ulyssse Bug #44: TX FIFO full status bit (SSR[1]) is corrupted during
     * one period of Bclock => Workaround S/W.
     * Write in TX FIFO only if FIFO is empty instead of writing in TX FIFO
     * while FIFO is not full.
     */

    if (READ_UART_REGISTER (uart, LSR) & THRE) {

        chars_in_tx_fifo = 0;

        while ((chars_written < chars_to_write) &&
               (chars_in_tx_fifo < FIFO_SIZE)) {

            WRITE_UART_REGISTER (uart, THR, *(buffer++));
            chars_written++;
            chars_in_tx_fifo++;
        }
    }

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Re-enable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) | IER_SLEEP);
#endif

    return (chars_written);
}
#else

SYS_UWORD32
UA_WriteNChars (T_tr_UartId uart_id,
                char *buffer,
                SYS_UWORD32 chars_to_write)
{
    SYS_UWORD32 chars_in_tx_fifo=0;
    SYS_UWORD32 chars_written=0;
    t_uart      *uart;
    UINT8       scr_value;

    uart = &(uart_parameter[uart_id]);

    /* DAR ER*/
    if(uart->callback_function == NULL)
    {
      WRITE_UART_REGISTER (
          uart, IER, READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
      if (READ_UART_REGISTER (uart, LSR) & THRE) 
      {
        while ((chars_written < chars_to_write) &&
            (chars_in_tx_fifo < FIFO_SIZE)) 
        {

          WRITE_UART_REGISTER (uart, THR, *(buffer++));
          chars_written++;
          chars_in_tx_fifo++;
        }
      }
      WRITE_UART_REGISTER (
          uart, IER, READ_UART_REGISTER (uart, IER) | IER_SLEEP);
      return (chars_written);
    }

    /*
     * Check if already doing one transfer, new request
     * cannot be entertained.
     * is_transmitting will be reset when transmit is completed.
     */
    if((uart->is_transmitting)|| (chars_to_write == 0))
        return 0;

    uart->is_transmitting = 1;

    /*
     * TX_DMA_THRESHOLD is the maximum value that can be programmed in
     * DMA controller - i.e cen*cfn (1*2n16)
     */
    if(chars_to_write > TX_DMA_THRESHOLD)
        chars_to_write = TX_DMA_THRESHOLD;

    #if (L1_POWER_MGT!=0)
        #if (OP_L1_STANDALONE == 0)
            Uart_Vote_DeepSleepStatus();
        #endif
    #endif
#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Disable sleep mode.
     */

    WRITE_UART_REGISTER (uart, IER,
                         READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
#endif

    /*
     * Update buffer pointer and initialse cur buffer pointer
     * size of transfer
     */
    uart->tx_buffer     = buffer;
    uart->tx_size       = chars_to_write;
    uart->tx_cur_pos    = 0;

/*Comment -- Sidhu we will go to Tx INTR mode by default*/
    //if(chars_to_write < (2 * FIFO_SIZE))
    if(chars_to_write)
    {
        /*
         * Program SCR for Tx-Intr mode of operation
         * Force the generation of THR_IT on TX FIFO empty: SCR[3] = 1.
         */
        scr_value  = READ_UART_REGISTER (uart, SCR);
        scr_value &= (~SCR_DMA_CTRL_TXDMA);

        WRITE_UART_REGISTER (uart, SCR, scr_value | SCR_DMA_CTRL | TX_EMPTY_CTL_IT);

        fill_tx_fifo(uart);
    }
    else
/*Comment -- Sidhu we will go to Tx DMA mode by default*/
    {

        scr_value  = READ_UART_REGISTER (uart, SCR);
        scr_value &= (~TX_EMPTY_CTL_IT);
        /*
         * Initiate transfer of data thru DMA
         */
        uart_tx_dma(uart_id, uart);
    }

/*------>> Sidhu ----  Don't Enable sleep here we will enable
  ------>> if we have nothing to be sent more */
#if 0
#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Re-enable sleep mode.
     */
    WRITE_UART_REGISTER (uart, IER,
                         READ_UART_REGISTER (uart, IER) | IER_SLEEP);
#endif
#endif
/*------<< Sidhu ----  Don't Enable sleep here we will enable
  ------<< if we have nothing to be sent more */

    return (chars_to_write);
}
#endif

/*******************************************************************************
 *
 *                           UA_EncapsulateNChars
 *
 * Purpose  : Writes N characters in the TX FIFO in encapsulating them with 2
 *            STX bytes (one at the beginning and one at the end).
 *
 * Arguments: In : uart_id       : UART id.
 *                 buffer        : buffer address from which characters are
 *                                 written.
 *                 chars_to_write: number of chars to write.
 *            Out: none
 *
 * Returns  : Number of chars written.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

SYS_UWORD32
UA_EncapsulateNChars (T_tr_UartId uart_id,
                      char *buffer,
                      SYS_UWORD32 chars_to_write)
{
    SYS_UWORD32 chars_written;
    SYS_UWORD32 chars_in_tx_fifo;
    t_uart      *uart;

    chars_written = 0;
    uart = &(uart_parameter[uart_id]);

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Disable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
#endif

    /*
     * Copy the input buffer to the TX FIFO.
     * Ulyssse Bug #44: TX FIFO full status bit (SSR[1]) is corrupted during
     * one period of Bclock => Workaround S/W.
     * Write in TX FIFO only if FIFO is empty instead of writing in TX FIFO
     * while FIFO is not full.
     */

    if (READ_UART_REGISTER (uart, LSR) & THRE) {

        chars_in_tx_fifo = 0;

        /*
         * Check if the message has been already encapsulated.
         */

        if (!uart->encapsulation_flag) {
            /*
             * Write STX in the TX FIFO and set the flag.
             */

            WRITE_UART_REGISTER (uart, THR, STX);
            chars_in_tx_fifo++;
            uart->encapsulation_flag = 1;
        }

        /*
         * Keep one char margin in the TX FIFO for the last STX.
         */

        while ((chars_written < chars_to_write) &&
               (chars_in_tx_fifo < (FIFO_SIZE-1))) {

            WRITE_UART_REGISTER (uart, THR, *(buffer++));
            chars_written++;
            chars_in_tx_fifo++;
        }

        /*
         * Append STX byte at the end if the frame is complete.
         */

        if (chars_written == chars_to_write) {

            /*
             * Write STX in the TX FIFO and reset the flag.
             */

            WRITE_UART_REGISTER (uart, THR, STX);
            uart->encapsulation_flag = 0;
        }
    }

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Re-enable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) | IER_SLEEP);
#endif

    return (chars_written);
}


/*******************************************************************************
 *
 *                           UA_WriteNBytes
 *
 * Purpose  : Writes N bytes in the TX FIFO in encapsulating with 2 STX bytes
 *            at the beginning and the end of the frame, and in making byte
 *            stuffing.
 *
 * Arguments: In : uart_id       : UART id.
 *                 buffer        : buffer address from which bytes are
 *                                 written.
 *                 bytes_to_write: number of bytes to write.
 *            Out: none
 *
 * Returns  : Number of bytes written.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

SYS_UWORD32
UA_WriteNBytes (T_tr_UartId uart_id,
                SYS_UWORD8 *buffer,
                SYS_UWORD32 bytes_to_write)
{
    SYS_UWORD32 bytes_written;
    SYS_UWORD32 bytes_in_tx_fifo;
    t_uart      *uart;

    bytes_written = 0;
    uart = &(uart_parameter[uart_id]);

	#if (L1_POWER_MGT!=0)
		#if (OP_L1_STANDALONE == 0)
			Uart_Vote_DeepSleepStatus();
		#endif
	#endif

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Disable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
#endif

    /*
     * Copy the input buffer to the TX FIFO.
     * Ulyssse Bug #44: TX FIFO full status bit (SSR[1]) is corrupted during
     * one period of Bclock => Workaround S/W.
     * Write in TX FIFO only if FIFO is empty instead of writing in TX FIFO
     * while FIFO is not full.
     */

    if (READ_UART_REGISTER (uart, LSR) & THRE) {

        bytes_in_tx_fifo = 0;

        /*
         * Check if the message has been already encapsulated.
         */

        if (!uart->encapsulation_flag) {

            /*
             * Write STX in the TX FIFO and set the flag.
             */

            WRITE_UART_REGISTER (uart, THR, STX);
            bytes_in_tx_fifo++;
            uart->encapsulation_flag = 1;
        }

        /*
         * Keep 2 chars margin in the FIFO, one for the stuffing (if necessary)
         * and one for the last STX.
         */

        while ((bytes_written < bytes_to_write) &&
               (bytes_in_tx_fifo < (FIFO_SIZE-2))) {

            /*
             * Check for STX or DLE in order to perform the stuffing.
             */

            if ((*(buffer) == STX) || (*(buffer) == DLE)) {

                /*
                 * Write DLE in the TX FIFO.
                 */

                WRITE_UART_REGISTER (uart, THR, DLE);
                bytes_in_tx_fifo++;
            }

            WRITE_UART_REGISTER (uart, THR, *(buffer++));
            bytes_written++;
            bytes_in_tx_fifo++;
        }

        /*
         * Append STX byte at the end if the frame is complete.
         */

        if (bytes_written == bytes_to_write) {

            /*
             * Write STX in the TX FIFO and reset the flag.
             */

            WRITE_UART_REGISTER (uart, THR, STX);
            uart->encapsulation_flag = 0;
        }
    }

#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Re-enable sleep mode.
     */

    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) | IER_SLEEP);
#endif

    return (bytes_written);
}


/*******************************************************************************
 *
 *                            UA_WriteChar
 *
 * Purpose  : Writes a character in the TX FIFO.
 *
 * Arguments: In : uart: UART id.
 *                 character
 *            Out: none
 *
 * Returns  : none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void
UA_WriteChar (T_tr_UartId uart_id,
              char character)
{
    (void) UA_WriteNChars (uart_id, &character, 1);
}

/*******************************************************************************
 *
 *                          UA_WriteString
 *
 * Purpose  : Writes a null terminated string in the TX FIFO.
 *
 * Arguments: In : uart_id: UART id.
 *                 buffer : buffer address from which characters are written.
 *            Out: none
 *
 * Returns  : none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void
UA_WriteString (T_tr_UartId uart_id,
                char *buffer)
{
    (void) UA_WriteNChars (uart_id, buffer, strlen (buffer));
}

/*******************************************************************************
 *
 *                             UA_EnterSleep
 *
 * Purpose  : Checks if UART is ready to enter Deep Sleep. If ready, enables
 *            wake-up interrupt.
 *
 * Arguments: In : uart_id : UART id.
 *            Out: none
 *
 * Returns: 0    : Deep Sleep is not possible.
 *          >= 1 : Deep Sleep is possible.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

SYS_BOOL
UA_EnterSleep (T_tr_UartId uart_id)
{
    t_uart              *uart;
    SYS_BOOL            deep_sleep;
    volatile SYS_UWORD8 status;

    uart = &(uart_parameter[uart_id]);
    deep_sleep = 0;
#ifdef  BTS
    /*Check if DMA-TX is going on*/

    if(uart->is_transmitting)
        return deep_sleep;
#endif
    /*
     * Check if RX & TX FIFOs are both empty
     */

    status = READ_UART_REGISTER (uart, LSR);

    if (!(status & DR) &&
        (status & TEMT)) {

#if ((CHIPSET != 5) && (CHIPSET != 6))
        /*
         * Disable sleep mode.
         */

        WRITE_UART_REGISTER (
            uart, IER, READ_UART_REGISTER (uart, IER) & ~IER_SLEEP);
#endif

        /*
         * Mask RX interrupt.
         */

      /*  WRITE_UART_REGISTER (
            uart, IER, READ_UART_REGISTER (uart, IER) & ~ERBI);
	*/
        /*
         * Enable the wake-up interrupt.
         */

        ENABLE_WAKEUP_INTERRUPT (uart);

        WRITE_UART_REGISTER (
        uart, SYSC, READ_UART_REGISTER (uart, SYSC) | SYSC_ENAWAKEUP);

        WRITE_UART_REGISTER(uart, WER, 0x70);

	#if (CHIPSET == 15) //enable uart wake up interrupt
		F_INTH_ENABLE_ONE_IT(C_INTH_UART_WAKEUP_IT);
	#endif


        deep_sleep = 1;
    }

    return (deep_sleep);
}

/*******************************************************************************
 *
 *                              UA_WakeUp
 *
 * Purpose  : Wakes up UART after Deep Sleep.
 *
 * Arguments: In : uart_id : UART id.
 *            Out: none
 *
 * Returns: none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void
UA_WakeUp (T_tr_UartId uart_id)
{
    t_uart *uart;

    uart = &(uart_parameter[uart_id]);

    /*
     * Disable the wake-up interrupt.
     */

    //DISABLE_WAKEUP_INTERRUPT (uart);

    /*
     * Unmask RX interrupts.
     */

   /* WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) | ERBI);
  */
#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Allow sleep mode.
     */
    WRITE_UART_REGISTER (
        uart, IER, READ_UART_REGISTER (uart, IER) | IER_SLEEP);
    //WRITE_UART_REGISTER (
       // uart, SYSC, READ_UART_REGISTER (uart, SYSC) & ~SYSC_ENAWAKEUP);
 #if (CHIPSET == 15) //disable  uart wake up interrupt
		F_INTH_DISABLE_ONE_IT(C_INTH_UART_WAKEUP_IT);
 #endif

#endif
}

/*******************************************************************************
 *
 *                       UA_InterruptHandler
 *
 * Purpose  : Interrupt handler.
 *
 * Arguments: In : uart_id         : origin of interrupt
 *                 interrupt_status: source of interrupt
 *            Out: none
 *
 * Returns  : none
 *
 ******************************************************************************/
#ifndef BTS_TX_INT
void
UA_InterruptHandler (T_tr_UartId uart_id,
                     SYS_UWORD8 interrupt_status)
{
    t_uart *uart;

    uart = &(uart_parameter[uart_id]);

    switch (interrupt_status) {

        case RX_DATA:

            read_rx_fifo (uart);

        break;

        default:

#ifdef UART_RX_BUFFER_DUMP
            uart_rx_buffer_dump.wrong_interrupt_status++;
#endif

            /* No Processing */

        break;
    }
}
#else
void
UA_InterruptHandler (T_tr_UartId uart_id,
                     SYS_UWORD8 interrupt_status)
{
    t_uart *uart;

    uart = &(uart_parameter[uart_id]);
    switch (interrupt_status)
    {
    case RX_DATA:
        read_rx_fifo (uart);
        break;
    case TX_EMPTY:
        fill_tx_fifo (uart);
        break;
    default:
#ifdef UART_RX_BUFFER_DUMP
   uart_rx_buffer_dump.wrong_interrupt_status++;
#endif
            /* No Processing */
    break;
    }
}
#endif




#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif /* min */

/*******************************************************************************
 *
 *                           fill_tx_fifo
 *
 * Purpose:     If more amount of data is available to be written, write it to
 *              TX fifo, else flag completion to the higher layer.
 *
 * Arguments:   In : t_uart*       : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     Number of bytes written.
 *
 *
 * Warning:     Parameters are not verified.
 ******************************************************************************/
#ifdef BTS_TX_INT
#if (OP_L1_STANDALONE == 0)
#pragma CODE_SECTION(fill_tx_fifo, ".uartintram")
#endif
void
fill_tx_fifo(t_uart *uart)
{
    UINT8 chars_in_tx_fifo = 0;
    char  data = 0;
    char* temp = uart->tx_buffer;

    if (uart->tx_cur_pos < (uart->tx_size))
    {
        /*
         * Write data from Tx-Buffer to Tx FIFO.
         * Update buffer pointers.
         * Enable Tx Interrupt
         */
/*
        if (READ_UART_REGISTER (uart, LSR) & THRE)
        {
            chars_in_tx_fifo = 0;

            while ((uart->tx_cur_pos < (uart->tx_size)) &&
                   (chars_in_tx_fifo < FIFO_SIZE))
            {
                data = *(temp + uart->tx_cur_pos);

                WRITE_UART_REGISTER (uart, THR, data);

                uart->tx_cur_pos++;
                chars_in_tx_fifo++;
            }
            WRITE_UART_REGISTER (uart, IER,
                                 READ_UART_REGISTER (uart, IER) | ETBEI);
        }
*/

        if (READ_UART_REGISTER (uart, LSR) & THRE)
        {
            int bytes_to_write=min((uart->tx_size)-(uart->tx_cur_pos), FIFO_SIZE);
	     char* buff_to_write=(uart->tx_buffer)+(uart->tx_cur_pos);
	     uart->tx_cur_pos = uart->tx_cur_pos + bytes_to_write;

	     while (bytes_to_write & 0xF8)	// if we have 8 or more bytes
	     	{
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[0]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[1]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[2]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[3]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[4]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[5]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[6]);
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[7]);
			buff_to_write+=8;
			bytes_to_write-=8;
	     	}

	     while (bytes_to_write)	// if we have bytes left
	     	{
	     		WRITE_UART_REGISTER (uart, THR, buff_to_write[0]);
			buff_to_write++;
			bytes_to_write--;
	     	}

            WRITE_UART_REGISTER (uart, IER,
                                 READ_UART_REGISTER (uart, IER) | ETBEI);
        }

    }
    else /*TxBufferEmpty*/
    {

        /*
         * Disable Tx Interrupt
         * Will be re-enabled when fresh data transfer occurs from the
         * Higher layers
         */
        WRITE_UART_REGISTER (uart, IER,
                             (READ_UART_REGISTER (uart, IER) & (~ETBEI)));

/*------>> Sidhu ----  Enable sleep here we have nothing to be sent more */
#if 1
#if ((CHIPSET != 5) && (CHIPSET != 6))
    /*
     * Re-enable sleep mode.
     */
    WRITE_UART_REGISTER (uart, IER,
                         READ_UART_REGISTER (uart, IER) | IER_SLEEP);
#endif
#endif
/*------<< Sidhu ----  Enable sleep here we have nothing to be sent more */


        uart->is_transmitting = 0;

        /*Notify upper layer*/
        (*(uart->callback_function))(TX_EMPTY);

    }
    return;
}

/*******************************************************************************
 *
 *                       uart_tx_dma
 *
 * Purpose:     Initiate process to start a DMA transfer
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

T_RV_RET
uart_tx_dma(T_tr_UartId uart_id, t_uart *uart)
{
    return uart_subscribe_dma_channel(uart_id, uart);
}

/*******************************************************************************
 *
 *                       uart_subscribe_dma_channel
 *
 * Purpose:     Acquired DMA channel for current transfer
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#if CHIPSET == 15
T_RV_RET
uart_subscribe_dma_channel(T_tr_UartId uart_id, t_uart *uart)
{
    T_RV_RET ret = RV_OK;

    if(uart_id == 0)
        uart->uart_dma_return_path.callback_func = uart_tx_dma_status_callback1;
    #if (CHIPSET!=15)
    else if(uart_id == 1)
        uart->uart_dma_return_path.callback_func = uart_tx_dma_status_callback2;
    #if (CHIPSET == 12)
    else
        uart->uart_dma_return_path.callback_func = uart_tx_dma_status_callback3;

    #endif
    #endif
    ret = dma_reserve_channel (DMA_CHAN_ANY, 0,DMA_QUEUE_DISABLE, 0,
                               uart->uart_dma_return_path);

    return (ret);
}
#endif

/*******************************************************************************
 *
 *                       uart_unsubscribe_dma_channel
 *
 * Purpose:     Release acquired DMA channel
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/
#if CHIPSET == 15
T_RV_RET
uart_unsubscribe_dma_channel (t_uart *uart)
{
    T_RV_RET ret = RV_OK;

    if(uart->uart_dma_channel != 0xFF)
        ret= dma_release_channel (uart->uart_dma_channel);
    else
        ret = RV_INTERNAL_ERR;

    return (ret);
}

#endif

/*******************************************************************************
 *
 *                       uart_start_tx_dma_transfer
 *
 * Purpose:     Sets DMA parameters for the current dma request
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *              Out: none
 *
 * Returns:     T_RV_RET    RV_OK if success
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

T_RV_RET
uart_start_tx_dma_transfer(t_uart* uart)
{
    T_DMA_CHANNEL_PARAMETERS    dma_channel_param;

    T_RV_RET ret = RV_OK;

    static UINT8 mmc_afl = 1;/*0x20;*/

    /*Set DMA channel parameters*/
    dma_channel_param.data_width            = DMA_DATA_S8;// CHB
    dma_channel_param.sync                  = C_DMA_CHANNEL_UART_IRDA_MODEM_TX;//4; // uart->uart_dma_peripheral;
    dma_channel_param.hw_priority           = DMA_HW_PRIORITY_LOW;
    dma_channel_param.dma_mode              = DMA_SINGLE;
    dma_channel_param.flush                 = DMA_FLUSH_DISABLED;

    // CHB
    dma_channel_param.nmb_elements          = 1;
    dma_channel_param.nmb_frames            = uart->tx_size;
    dma_channel_param.dma_end_notification  = DMA_NOTIFICATION;
    dma_channel_param.secure                = DMA_NOT_SECURED;

    dma_channel_param.source_address        = (UINT32)uart->tx_buffer;
    dma_channel_param.source_address_mode   = DMA_ADDR_MODE_POST_INC;
    dma_channel_param.source_packet         = DMA_NOT_PACKED;
    dma_channel_param.source_burst          = DMA_NO_BURST;

    dma_channel_param.destination_address   = (UINT32)((uart)->base_address + THR);
    dma_channel_param.destination_address_mode  = DMA_ADDR_MODE_CONSTANT;
    dma_channel_param.destination_packet    = DMA_NOT_PACKED;
    dma_channel_param.destination_burst     = DMA_NO_BURST;

    dma_channel_param.transfer              = DMA_MODE_TRANSFER_ENABLE;

    /*Set DMA channel parameters*/
    if( dma_set_channel_parameters(uart->uart_dma_channel, &dma_channel_param) != DMA_OK )
    {
        ret = RV_INTERNAL_ERR;
    }
    return (ret);
}

/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback1
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-1.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/
void
uart_tx_dma_status_callback1(void *response)
{
    t_uart *uart= &(uart_parameter[0]);

    uart->uart_dma_peripheral = 4;//DMA_SYNC_DEVICE_UART1_TX;

    uart_tx_dma_status_callback(uart,response);
}
#if (CHIPSET!=15)
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback2
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-2.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/
void
uart_tx_dma_status_callback2(void *response)
{

    t_uart *uart= &(uart_parameter[1]);

    uart->uart_dma_peripheral = 6;//DMA_SYNC_DEVICE_UART2_TX;

    uart_tx_dma_status_callback(uart,response);
}
#if (CHIPSET == 12)
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback3
 *
 * Purpose:     Called by DMA framework to report status of current DMA request
 *              for uart-3.
 *
 * Arguments:   In : void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/
void
uart_tx_dma_status_callback3(void *response)
{
    t_uart *uart= &(uart_parameter[2]);

    uart->uart_dma_peripheral = 8;//DMA_SYNC_DEVICE_UART_IRDA_TX;

    uart_tx_dma_status_callback(uart,response);
}
#endif
#endif
/*******************************************************************************
 *
 *                       uart_tx_dma_status_callback
 *
 * Purpose:     Handle DMA status messages.
 *
 * Arguments:   In : t_uart* uart     : Pointer to t_uart structure.
 *                   void* response   : status reponse from DMA framework.
 *              Out: none
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/
void
uart_tx_dma_status_callback(t_uart* uart, void *response)
{
    T_DMA_STATUS_RSP_MSG *dma_status_rsp_msg = (T_DMA_STATUS_RSP_MSG *)response;
    UINT8 scr_value;

    switch(dma_status_rsp_msg->result.status)
    {
        case DMA_RESERVE_OK:
            /*
             * Update channel number
             */
            uart->uart_dma_channel = dma_status_rsp_msg->result.channel;
            uart_start_tx_dma_transfer(uart);
        break;

        case DMA_PARAM_SET:

            scr_value = READ_UART_REGISTER (uart, SCR);

            WRITE_UART_REGISTER (uart, SCR, scr_value | SCR_DMA_CTRL |
                                            SCR_DMA_CTRL_TXDMA );
        break;

        case DMA_COMPLETED:
            /*
             * Notify higher layer Tx Completed
             * Release dma channel
             */
            if(uart->uart_dma_channel > 0)
            {
                f_dma_channel_disable (uart->uart_dma_channel);
                f_dma_channel_soft_reset(uart->uart_dma_channel);
            }
            /*
             * Program UART in Intr mode of operation
             */
            scr_value  = READ_UART_REGISTER (uart, SCR);
            scr_value &= ~(SCR_DMA_CTRL_TXDMA);
            WRITE_UART_REGISTER (uart, SCR, scr_value | SCR_DMA_CTRL);
            /*
             * Release DMA channel
             */
            uart_unsubscribe_dma_channel(uart);
            uart->is_transmitting = 0;
            (*(uart->callback_function))(TX_EMPTY);
        break;

        case DMA_NO_CHANNEL:
        case DMA_TOO_MANY_REQUESTS:
        case DMA_INVALID_PARAMETER:
        case DMA_NOT_READY:
        case DMA_MEMORY_ERROR:
        case DMA_ACTION_NOT_ALLOWED:
        case DMA_CHANNEL_BUSY:
        case DMA_QUEUED:
        case DMA_TIMEOUT_SOURCE:
        case DMA_TIMEOUT_DESTINATION:
        case DMA_MISS_EVENT:
            /*
             * got an DMA error remaining transfer will be
             * completed thru Intr mode
             * Program SCR for Tx-Intr mode of operation
             */
            scr_value  = READ_UART_REGISTER (uart, SCR);
            scr_value &= (~SCR_DMA_CTRL_TXDMA);

            WRITE_UART_REGISTER (uart, SCR, scr_value | SCR_DMA_CTRL | TX_EMPTY_CTL_IT);

            fill_tx_fifo(uart);
        break;

            default:
            break;
    } /* switch dma_status_rsp_msg->result.status */

    rvf_free_buf(response);
    return;
}

#endif
/*******************************************************************************
 *
 *                       uart_set_mode
 *
 * Purpose:     This is called to change the UART mode of operation.
 *
 * Arguments:   uart id , mode
 *
 * Returns:     none
 *
 * Warning:     Parameters are not verified.
 *
 ******************************************************************************/
void uart_set_mode(int uart_id, int mode)
{
    t_uart      *uart;
    uart = &(uart_parameter[uart_id]);
    // uart->uart_mode = mode;
}
