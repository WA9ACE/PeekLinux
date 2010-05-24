#ifndef _HWDUART_H_
#define _HWDUART_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

typedef enum {
   Uart0Jitter_153K,
   Uart0Jitter_230K,
   Uart1Jitter_153K,
   Uart1Jitter_230K
}HwdUartBaudRateT;

/*----------------------------------------------------------------------------
 Global Data
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/
/*****************************************************************************
 
  FUNCTION NAME:   HwdUartBaudRateInit

  DESCRIPTION:     Configures the onboard Uart dividers to get input freq = 3.6864MHz

  PARAMETERS:      SysAmpsCdmaSelectT AmpsCdmaSel

  RETURNED VALUES: None

*****************************************************************************/
extern void HwdUartBaudRateInit(SysAmpsCdmaSelectT AmpsCdmaSel);

/*****************************************************************************
 
  FUNCTION NAME:   HwdUartSetBaudRate

  DESCRIPTION:     Configures the onboard Uart dividers

  PARAMETERS:      uint8             UartNumber
                   UartBaseBaudRateT BaseBaudRate

  RETURNED VALUES: None

*****************************************************************************/
extern void HwdUartSetBaudRate( uint8 UartNumber,
                                HwdUartBaudRateT BaseBaudRate);

/*****************************************************************************
 
  FUNCTION NAME:   HwdUartSetBaudRateFrom32K

  DESCRIPTION:     Configures the onboard Uart dividers with 32K source clock;
                   the only Uart clock we can get from 32K is 9600 Hz, so
                   the maximum baud rate is 600 baud

  PARAMETERS:      uint8   UartNumber
  
  RETURNED VALUES: None
  
*****************************************************************************/
void HwdUartSetBaudRateFrom32K( uint8 UartNumber);

/*****************************************************************************
 
  FUNCTION NAME:   HwdUartJitterClkCtrl

  DESCRIPTION:     Enables or disables the particular Uart clock

  PARAMETERS:      uint8    UartNumber
                   bool     State

  RETURNED VALUES: None

*****************************************************************************/
extern void HwdUartJitterClkCtrl( uint8 UartNumber, bool State );




#endif
