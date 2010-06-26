/**
 * @file	usbtrc_api.c
 *
 * Interface of the USBTRC SWE.
 *
 * @author
 * @version 0.1
 */

#include "usbtrc/usbtrc_i.h"
#include "usbtrc/usbtrc_api.h"

#include "sys_types.h"
#include "uart/uart.h"

#include "string.h"


/**
 * Constants definition
 */
#define STX	0x02
#define DLE	0x10


/**
 * Structures definition
 */
typedef struct s_uart {

	/* Callback used to signal received bytes to USBT user */
	void (*callback) (void);

    /*
     * Framing flags.
     */
    SYS_BOOL dle_detected;
    SYS_BOOL inframe;
    SYS_BOOL encapsulation_flag;
    unsigned char frame_length;

} t_uart;


/**
 * Local and globale variables
 */
static t_uart uart;


void (*USBT_callback_function)(void);


/**
 * Functions
 */

/*******************************************************************************
 *
 *                           USBT_get_callback
 * 
 * Purpose  : Returns callback function to be called each time characters are
 *			  received.
 *            USBT_get_callback should be called by USBTRC SWE.
 *
 * Arguments: none
 *
 * Returns: the callback function
 *
 ******************************************************************************/

void *USBT_get_callback (void)
{
	return (void*)uart.callback;
}

/*******************************************************************************
 *
 *                           USBT_initialize_uart_sleep
 * 
 * Purpose  : dummy function, there is nothing to do to enter sleep mode on USB.
 *
 * Arguments: In : uart_id : UART id.
 *            Out: none
 *
 * Returns: none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void USBT_initialize_uart_sleep (T_tr_UartId uart_id)
{
	if(!usbtrc_ready()) return;

	USBTRC_SEND_TRACE("USBTRC: USBT_initialize_uart_sleep", RV_TRACE_LEVEL_DEBUG_LOW);
}

/*******************************************************************************
 *
 *                               USBT_Init
 * 
 * Purpose  : Initializes the module and the USB driver.
 *
 * Arguments: In : uart_id : unused.
 *                 baudrate: unused.
 *                 callback: user's function called characters are received.
 *            Out: none
 *
 * Returns: none
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

void USBT_Init (T_tr_UartId uart_id,
				T_tr_Baudrate baudrate,
				void (callback_function (void)))
{	
	/* initialize uart structure */
	uart.callback = callback_function;
	uart.dle_detected = 0;
    uart.inframe = 0;
    uart.encapsulation_flag = 0;
    uart.frame_length = 0;

	USBT_callback_function = uart.callback;

	/* report DSR hight to host */
	/*usbtrc_report_dsr_hight();*/
}

/*******************************************************************************
 *
 *                           USBT_ReadNChars
 * 
 * Purpose  : Reads N characters from the RX buffer.
 *
 * Arguments: In : uart_id      : unused.
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

SYS_UWORD32 USBT_ReadNChars (T_tr_UartId uart_id,
							 char *buffer,
							 SYS_UWORD32 chars_to_read)
{
	if(!usbtrc_ready()) return 0;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_ReadNChars", chars_to_read);

    return (SYS_UWORD32)usbtrc_q_rx_get(buffer, chars_to_read);
}

/*******************************************************************************
 *
 *                           USBT_ReadNBytes
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

SYS_UWORD32 USBT_ReadNBytes (T_tr_UartId uart_id,
							 char *buffer_p,
							 SYS_UWORD32 bytes_to_read,
							 SYS_BOOL *eof_detected_p)
{
    SYS_UWORD32 bytes_written;
    SYS_UWORD32 bytes_in_rx_buffer;
    SYS_UWORD32 bytes_to_process;
	char		byte;


	if(!usbtrc_ready()) return 0;

    bytes_written = 0;


	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_ReadNBytes", bytes_to_read);

	bytes_in_rx_buffer = usbtrc_q_rx_info();

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

		usbtrc_q_rx_get(&byte, 1);
        switch (byte) {

            /*
             * Current byte is DLE.
             */

            case DLE:
                                                        
                if (!uart.dle_detected) {

                    /*
                     * No DLE previously detected => 
                     * Skip the current byte and set the flag.
                     */

                    uart.dle_detected = 1;
                }

                else { /* if (uart.dle_detected) */

                    if (uart.inframe) {

                        /*
                         * DLE previously detected AND currently inside of a frame =>
                         * Copy the current byte in the output buffer, reset the flag
                         * and increase the frame length.
                         */

                        uart.dle_detected = 0;
                        uart.frame_length++;
                        *(buffer_p++) = byte;
                        bytes_written++;
                    }

                    else { /* if (!uart.inframe) */

                        /*
                         * DLE previously detected AND currently outside of a frame =>
                         * Skip the current byte.
                         */
                    }
                }

            break; /* case DLE */

            /*
             * Current byte is STX.
             */

            case STX:
                                                      
                if ((!uart.dle_detected) && (uart.inframe)) {

                    /*
                     * No DLE previously detected AND currently inside of a frame.
                     */

                    if (uart.frame_length) {

                        /*
                         * Frame length is not zero (End of Frame) => 
                         * Skip the current byte and set the flags (EOF).
                         */

                        uart.inframe = 0;
                        uart.frame_length = 0;

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

                    else { /* if (!uart.frame_length) */

                        /*
                         * Frame length is zero (STX followed by another STX =
                         * Synchro lost but start of a new frame) =>
                         * Skip the current byte and keep the flag set.
                         */
                    }
                }

                else if ((!uart.dle_detected) && (!uart.inframe)) {

                    /*
                     * No DLE previously detected AND currently outside of a
                     * frame (Start of Frame) =>
                     * Skip the current byte and set the flag.
                     */

                    uart.inframe = 1;
                }

                else if ((uart.dle_detected) && (uart.inframe)) {

                    /*
                     * DLE previously detected AND currently inside of a frame =>
                     * Copy the current byte in the output buffer, reset the flag
                     * and increase the frame length.
                     */

                    uart.dle_detected = 0;
                    uart.frame_length++;
                    *(buffer_p++) = byte;
                    bytes_written++;
                }

                else if ((uart.dle_detected) && (!uart.inframe)) {

                    /*
                     * DLE previously detected AND currently outside of a frame =>
                     * Skip the current byte and reset the flag.
                     */

                    uart.dle_detected = 0;
                }

            break; /* case STX */

            /*
             * Current byte is neither DLE nor STX.
             */

            default:

                if (uart.inframe) {

                    /*
                     * Currently inside of a frame =>
                     * Copy the current byte in the output buffer and increase
                     * the frame length.
                     */

                    uart.frame_length++;
                    *(buffer_p++) = byte;
                    bytes_written++;
                }

                else { /* if (!uart.inframe) */

                    /*
                     * Currently outside of a frame =>
                     * Skip the current byte.
                     */
                }

            break; /* default */
        }

        bytes_to_process--;
    }

	/* if receiving was suspended, re-enable incomming data by giving
		a buffer to USB LLD */
	if(usbtrc_rx_suspended() == TRUE)
	{		
		usbtrc_set_rx_buffer_bulk();
	}

    return (bytes_written);
}

/*******************************************************************************
 *
 *                           USBT_WriteNChars
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


extern T_RVF_MUTEX usb_tx_mutex;
SYS_UWORD32 USBT_WriteNChars (T_tr_UartId uart_id,
							  char *buffer,
							  SYS_UWORD32 chars_to_write)
{
	char *buf;
	UINT32 i;
	//UINT16 put_size =0;
	T_USBTRC_USB_DEVICE *usbdev = usbtrc_env_ctrl_blk_p->usb_device;

//--> Modified for PM
	//if(!usbtrc_ready()) return 0;
	if(!usbtrc_ready()) return chars_to_write;
//<-- Modified for PM
	
	if(chars_to_write == 0) return 0;

#if( WCP_PROF == 1)
	if(chars_to_write > BUFFER_TX_SIZE)
		chars_to_write = BUFFER_TX_SIZE;
#else
	if(chars_to_write > USBTRC_MAX_PACKET_SIZE)
		chars_to_write = USBTRC_MAX_PACKET_SIZE;
#endif

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_WriteNChars", chars_to_write);

       rvf_lock_mutex (&usb_tx_mutex);
	/* memcpy data to queue */
//	put_size = usbtrc_q_tx_put(buffer, chars_to_write);
	buf = usbdev->buffer_tx_temp;
	for(i=0; i<chars_to_write; i++)
	{
		*buf++ = buffer[i];
	}
	usbdev->tx_size = chars_to_write;


//	if((usbdev->write_in_progress == FALSE)&&(put_size != 0))
	if(chars_to_write != 0)
	{
	
		/* extract some data from queue and send on USB */
		if(usbtrc_process_tx_context() == RVM_INTERNAL_ERR)
		{
			USBTRC_SEND_TRACE("USBTRC: USBT_WriteNChars failed to write on USB", RV_TRACE_LEVEL_DEBUG_LOW);
			/*we infor the caller that something went wrong*/
			/*but ther is still some data left in the internal buffers*/
//			//put_size = 0;
                    rvf_unlock_mutex (&usb_tx_mutex);
			return 0;
		}
	}

    return chars_to_write;
}

/*******************************************************************************
 *
 *                           USBT_EncapsulateNChars
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

SYS_UWORD32 USBT_EncapsulateNChars (T_tr_UartId uart_id,
									char *buffer,
									SYS_UWORD32 chars_to_write)
{
	char *buf;
	UINT16 size, i;
	char stx = STX;
	T_USBTRC_USB_DEVICE *usbdev = usbtrc_env_ctrl_blk_p->usb_device;

//--> Modified for PM
	//if(!usbtrc_ready()) return 0;
	if(!usbtrc_ready()) return chars_to_write;
//<-- Modified for PM

#if( WCP_PROF == 1)
	if((chars_to_write + 2) > BUFFER_TX_SIZE)
		chars_to_write = BUFFER_TX_SIZE - 2;
#else
	if((chars_to_write + 2) > USBTRC_MAX_PACKET_SIZE)
		chars_to_write = USBTRC_MAX_PACKET_SIZE - 2;
#endif

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_EncapsulateNChars", chars_to_write);

	/* check if queue can store requested chars + 2 STX chars */

	if(usbtrc_q_tx_try(chars_to_write + 2) == FALSE)
	{
		return 0;
	}

       rvf_lock_mutex (&usb_tx_mutex);
	   
	/* queue head STX */
//	usbtrc_q_tx_put(&stx, 1);
	buf = usbdev->buffer_tx_temp;
       *buf++ = stx;

	/* queue data */
//	usbtrc_q_tx_put(buffer, (UINT16)chars_to_write);
	for(i=0; i<chars_to_write; i++)
	{
		*buf++ = buffer[i];
	}

	/* queue tail STX */
//	usbtrc_q_tx_put(&stx, 1);
       *buf++ = stx;	
	usbdev->tx_size = chars_to_write+2;

//	if(usbdev->write_in_progress == FALSE)
	//{
		
		/* extract some data from queue and send */
		if(usbtrc_process_tx_context() == RVM_INTERNAL_ERR)
		{
			USBTRC_SEND_TRACE("USBTRC: USBT_EncapsulateNChars failed to write on USB", RV_TRACE_LEVEL_DEBUG_LOW);
			/*we infor the caller that something went wrong*/
			/*but ther is still some data left in the internal buffers*/
			chars_to_write = 0;
                     rvf_unlock_mutex (&usb_tx_mutex);			

		}

//	}

    return chars_to_write;
}

/*******************************************************************************
 *
 *                           USBT_WriteNBytes
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

SYS_UWORD32 USBT_WriteNBytes (T_tr_UartId uart_id,
							  SYS_UWORD8 *buffer,
							  SYS_UWORD32 bytes_to_write)
{
	char *buf;
	UINT32 i, size, n = 0; /* n is the number of stuffing bytes */
    static UINT32 split_packet;
	char stx = STX;
	char dle = DLE;

#if( WCP_PROF == 1)
	SYS_UWORD32 max_buf_size = BUFFER_TX_SIZE;
#else
	SYS_UWORD32 max_buf_size = USBTRC_MAX_PACKET_SIZE;
#endif
	

	T_USBTRC_USB_DEVICE *usbdev = usbtrc_env_ctrl_blk_p->usb_device;
	
//--> Modified for PM
	//if(!usbtrc_ready()) return 0;
	if(!usbtrc_ready()) return bytes_to_write;
//<-- Modified for PM	

    USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_WriteNBytes", bytes_to_write);

    rvf_lock_mutex (&usb_tx_mutex);
	   
	/* queue head STX */
    buf = usbdev->buffer_tx_temp;

    if(split_packet == 0)
    {
        *buf++ = stx;
        n++;
    }

	for(i=0; (i<bytes_to_write) && ( (i+n+1) < max_buf_size ); i++)
	{
   	    if( (buffer[i] == STX) || (buffer[i] == DLE) )
	    {
	        *buf++ = dle;
	        n++;
        }
	    *buf++ = buffer[i];
	}

    if((bytes_to_write+n+1)> max_buf_size)
    {
        split_packet++;
    }
    else
    {
        *buf++ = stx;
        n++;
        split_packet = 0;
    }


	bytes_to_write = i;

	/* check if queue can store requested chars + n stuffing chars + 2 STX chars */

#if 0
	if(usbtrc_q_tx_try(bytes_to_write + n + 2) == FALSE)
	{
		return 0;
	}
#endif

    usbdev->tx_size = bytes_to_write + n;
	
	/* extract some data from queue and*/
	/* then send them on usb in bulk endpoint */
	if(usbtrc_process_tx_context() == RVM_INTERNAL_ERR)
	{
		USBTRC_SEND_TRACE("USBTRC: USBT_WriteNBytes failed to write on USB", RV_TRACE_LEVEL_DEBUG_LOW);
		/*we infor the caller that something went wrong*/
		/*but ther is still some data left in the internal buffers*/
		bytes_to_write = 0;
        rvf_unlock_mutex (&usb_tx_mutex);
	}
    return bytes_to_write;
}

/*******************************************************************************
 *
 *                            USBT_WriteChar
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

void USBT_WriteChar (T_tr_UartId uart_id,
					 char character)
{
	if(!usbtrc_ready()) return;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_WriteChar", character);

    (void) USBT_WriteNChars (uart_id, &character, 1);
}

/*******************************************************************************
 *
 *                          USBT_WriteString
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

void USBT_WriteString (T_tr_UartId uart_id,
					   char *buffer)
{
	if(!usbtrc_ready()) return;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_WriteString", strlen (buffer));

    (void) USBT_WriteNChars (uart_id, buffer, strlen (buffer));
}

/*******************************************************************************
 *
 *                             USBT_EnterSleep
 * 
 * Purpose  : Checks if UART is ready to enter Deep Sleep. If ready, enables
 *            wake-up interrupt.
 *
 * Arguments: In : uart_id : UART id.
 *            Out: none
 *
 * Returns: 0	 : Deep Sleep is not possible.
 *          >= 1 : Deep Sleep is possible.
 *
 * Warning: Parameters are not verified.
 *
 ******************************************************************************/

SYS_BOOL USBT_EnterSleep (T_tr_UartId uart_id)
{
//--> Modified for PM
	//if(!usbtrc_ready()) return 0;
	if(!usbtrc_ready()) return TRUE;
//<-- Modified for PM	

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_EnterSleep", 0);

	/* if there is data left in TX buffer, can't enter sleep mode */
	if(usbtrc_q_tx_try(0) == NULL)
	{
		return 0;
	}

	return 1;
}

/*******************************************************************************
 *
 *                              USBT_WakeUp
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

void USBT_WakeUp (T_tr_UartId uart_id)
{
	if(!usbtrc_ready()) return;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_WakeUp", 0);
}

/*******************************************************************************
 *
 *                       USBT_InterruptHandler
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

void USBT_InterruptHandler (T_tr_UartId uart_id,
							SYS_UWORD8 interrupt_status)
{
	if(!usbtrc_ready()) return;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: USBT_InterruptHandler", interrupt_status);
}
