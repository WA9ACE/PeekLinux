#include "sys_types.h"
#include "uart/uart.h"


/**
 * Functions
 */
extern void (*USBT_callback_function)(void);

extern void USBT_initialize_uart_sleep (T_tr_UartId uart_id);

extern void USBT_Init (T_tr_UartId uart_id,
					   T_tr_Baudrate baudrate,
					   void (callback_function (void)));

extern SYS_UWORD32 USBT_ReadNChars (T_tr_UartId uart_id,
									char *buffer,
									SYS_UWORD32 chars_to_read);

extern SYS_UWORD32 USBT_ReadNBytes (T_tr_UartId uart_id,
									char *buffer_p,
									SYS_UWORD32 bytes_to_read,
									SYS_BOOL *eof_detected_p);

extern SYS_UWORD32 USBT_WriteNChars (T_tr_UartId uart_id,
									 char *buffer,
									 SYS_UWORD32 chars_to_write);

extern SYS_UWORD32 USBT_EncapsulateNChars (T_tr_UartId uart_id,
										   char *buffer,
										   SYS_UWORD32 chars_to_write);

extern SYS_UWORD32 USBT_WriteNBytes (T_tr_UartId uart_id,
									 SYS_UWORD8 *buffer,
									 SYS_UWORD32 bytes_to_write);

extern void USBT_WriteChar (T_tr_UartId uart_id, char character);

extern void USBT_WriteString (T_tr_UartId uart_id, char *buffer);

extern SYS_BOOL USBT_EnterSleep (T_tr_UartId uart_id);

extern void USBT_WakeUp (T_tr_UartId uart_id);

extern void USBT_InterruptHandler (T_tr_UartId uart_id, SYS_UWORD8 interrupt_status);

