/* ============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
*============================================================================
*
* @file armio.c
*
* This File contains functions that implement GPIO
* functionality.
*
* @path  $csw-system\drv_core\armio
*
* @rev  0.1
*
* ----------------------------------------------------------------------------
*!
*! Revision History
*! ==========
*! 14-Sep-2006 mf:  Modified by Bipin Johnson. Change required
*! to  comply with TI coding standards
*!
*============================================================================*/

/****************************************************************
*  INCLUDE FILES
****************************************************************/
#include "l1sw.cfg"
#include "swconfig.cfg"
#include "pin_config.h"
#include "sys_inth.h"
#include "nucleus.h"

#if (OP_L1_STANDALONE == 0)
	#ifdef BLUETOOTH_INCLUDED
	#include "btemobile.cfg"
	#endif
#endif

#if (OP_L1_STANDALONE == 1)
  #include "l1_macro.h"
  #include "l1_confg.h"
#endif
#include "board.cfg"
#include "chipset.cfg"

#include "sys_types.h"

#include "memif/mem.h"

#include "inth/iq.h"
#include "armio/Armio.h"


#if (REMU==1)
#define GPIO_HISR_STACK_SIZE			(1024)
#else
#define GPIO_HISR_STACK_SIZE			(512)
#endif

/****************************************************************
*  Variable declaration
****************************************************************/
static NU_HISR GPIO_Hisr;
#ifndef HISR_STACK_SHARING
static char GPIO_HisrStack[GPIO_HISR_STACK_SIZE];
#endif

void (*func_ptr_Arr[47])(void) = {NULL};
void (*call_back_fun)(void)=NULL;

typedef enum
{
    ARMIO_OK,
    ARMIO_INT_NOT_ENABLED,
    ARMIO_ERR,
    ARMIO_PIN_NO_ERR
} status;



/* ============================================================
*
*  AI_SetBit()
*  Description:This function Switch-on one bit.
*
*  @param  :    pin number
*
*  @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
*
* =============================================================
*/

UINT8 AI_SetBit(int bit)
{
#if (CHIPSET == 15)
    SYS_UWORD32 armio_out;

    if(bit>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

  if (bit >= 0 && bit <= 15)
    armio_out = ARMIO_OUT;
  else if (bit >= 16 && bit <= 31)
  {
    armio_out = ARMIO1_OUT;
	bit -= 16;
  }
  else if (bit >= 32 && bit <= 47)
  {
    armio_out = ARMIO2_OUT;
	bit -=32;
  }
  
   *((volatile SYS_UWORD16 *) armio_out) |= (1<<bit); 
 #endif

 return ARMIO_OK;
}

/* ============================================================
*
* AI_ResetBit()
* Description: This function Switch-off one bit
*
*  @param  :    pin number
*
*  @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
*
* =============================================================
*/

UINT8 AI_ResetBit(int bit)
{
#if (CHIPSET == 15)
volatile SYS_UWORD32 armio_out;

    if(bit>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

  if (bit >= 0 && bit <= 15)
      armio_out = ARMIO_OUT;
  else if (bit >= 16 && bit <= 31)
  {
    armio_out = ARMIO1_OUT;
	bit -= 16;
  }
  else if (bit >= 32 && bit <= 47)
  {
    armio_out = ARMIO2_OUT;
	bit -= 32;
  }
  
   *((volatile SYS_UWORD16 *) armio_out) &= ~(1<<bit); 

 #endif
    return ARMIO_OK;
}

/* ============================================================
*
* AI_ConfigBitAsOutput()
* Description : This function Set a bit as output
*
*  @param  :    pin number
*
*  @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
*
* =============================================================
*/

UINT8 AI_ConfigBitAsOutput(int bit)
{
#if (CHIPSET == 15)
    SYS_UWORD32 armio_io_cntl;

    if(bit>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

  if (bit >= 0 && bit <= 15)
    armio_io_cntl = ARMIO_IO_CNTL;
  else if (bit >= 16 && bit <= 31)
  {
    armio_io_cntl = ARMIO1_IO_CNTL;
	bit -= 16;
  }
  else if (bit >= 32 && bit <= 47)
  {
    armio_io_cntl = ARMIO2_IO_CNTL;
	bit -= 32;
  }
  
   *((volatile SYS_UWORD16 *) armio_io_cntl) &= ~(1<<bit); 

 #endif
    return ARMIO_OK;
}

/* ============================================================
*
* AI_ConfigBitAsInput()
* Description:This function Set a bit as input
*
*  @param    : pin number
*
*  @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
* =============================================================
*/

UINT8 AI_ConfigBitAsInput(int bit)
{
#if (CHIPSET == 15)
SYS_UWORD32 armio_io_cntl;

    if(bit>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

  if (bit >= 0 && bit <= 15)
    armio_io_cntl = ARMIO_IO_CNTL;
  else if (bit >= 16 && bit <= 31)
  {
    armio_io_cntl = ARMIO1_IO_CNTL;
	bit -= 16;
  }
  else if (bit >= 32 && bit <= 47)
  {
    armio_io_cntl = ARMIO2_IO_CNTL;
	bit -= 32;
  }
  
   *((volatile SYS_UWORD16 *) armio_io_cntl) |= (1<<bit); 

 #endif
    return ARMIO_OK;
}

/* ============================================================
*
* AI_ReadBit()
* Description:This function reads the value from a register
*
*  @param         pin number
*
*  @return          read value (1 or 0)
* =============================================================
*/

SYS_BOOL AI_ReadBit(int bit)
{
#if (CHIPSET == 15)
  SYS_UWORD32 armio_in;

  if (bit >= 0 && bit <= 15)
    armio_in = ARMIO_IN;
  else if (bit >= 16 && bit <= 31)
  {
    armio_in = ARMIO1_IN;
	bit -= 16;
  }
  else if (bit >= 32 && bit <= 47)
  {
    armio_in = ARMIO2_IN;
	bit -= 32;
  }
   if ((*((volatile SYS_UWORD16 *) armio_in)) & (1<<bit))
      return (1);
   else
      return (0);

#endif
}

#if (OP_L1_STANDALONE == 0)
/* ============================================================
 *
* AI_Power() description for void AI_Power(). This function Switch-on or off the board
 *
*  @param   : ON or OFF
*
*  @return   : None
* =============================================================
*/

void AI_Power(SYS_UWORD8 power)
{
    SYS_UWORD32 status_value;

  if (power == 0) 
  {
		/* If this function returns then charging in progress can't switch-Off */
		Switch_Off();
  }
}
#endif

/* ============================================================
*
* AI_ResetIoConfig()
* Description: This function reset all IO
* configurations and makes all bit as input
*
*  @param   :  none
*
*  @return    : none
*
* =============================================================
*/

void AI_ResetIoConfig(void)
{
    *((volatile SYS_UWORD16 *) ARMIO_IO_CNTL) = CONFIG_ALL_AS_INPUT; // all bits are inputs
#if (CHIPSET == 15)   
    *((volatile SYS_UWORD16 *) ARMIO1_IO_CNTL) = CONFIG_ALL_AS_INPUT; // all bits are inputs
    *((volatile SYS_UWORD16 *) ARMIO2_IO_CNTL) = CONFIG_ALL_AS_INPUT; // all bits are inputs

    GPIO_INTERRUPT_MASK_REG = MASK_ALL_INTERRUPT; // Mask All Interrupts 0
    GPIO1_INTERRUPT_MASK_REG = MASK_ALL_INTERRUPT; // Mask All Interrupts 1
    GPIO2_INTERRUPT_MASK_REG = MASK_ALL_INTERRUPT; // Mask All Interrupts 2
#endif

}

/* ============================================================
*
* AI_ClockEnable()
* Description:This function enable GPIO clock module
*
*  @param   :  none
*
*  @return    : none
*
* =============================================================
*/

void AI_ClockEnable(void)
{
   *((volatile SYS_UWORD16 *) ARMIO_CNTL_REG) |= ARMIO_CLOCKEN;    // set to 1 bit 5
#if (CHIPSET == 15)
   *((volatile SYS_UWORD16 *) ARMIO1_CNTL_REG) |= ARMIO_CLOCKEN;    // set to 1 bit 5
   *((volatile SYS_UWORD16 *) ARMIO2_CNTL_REG) |= ARMIO_CLOCKEN;    // set to 1 bit 5
#endif
}


/* ============================================================
 *
* AI_InitIOConfig()
* Description: This function Configure all GPIOs at
* initialization in order to optimize the power consumption of the C-Sample :
 *  - select IOs 8,9,10,11,12 and 13 on the pins instead of MCSI and MCUEN signals.
 *  - configure these IOs in output high.
 *  - configure the IOs 0 (Vibrator LED) and 1 (LCD_A0) in output low.
*
*  @param   : None
*
*   @return   : none
* =============================================================
*/

void AI_InitIOConfig(void)
{
  // reset the IOs config
  AI_ResetIoConfig();

  // CLKM_IO_CNTL register configuration :
  // select IOs 6,8,9,10,11,12 and 13 on the pins instead of MCSI and MCUEN signals.

  /* Bits 5,6,7,8 are used to output I/O 9,10,11,12 or MCSI pins */
  /* If Bluetooth, IO should be disabled, outputting MCSI used for Bluetooth voice */


  // ARMIO_OUT register configuration :
  // set IOs 8,9,10,11,12 and 13 as high
  // set IOs 0 to 7 as low


    /*enabling the interrupts for GPIOs*/
    AI_int_init();

}

/* ============================================================
*
* AI_SelectIOForIT()
* Description:This function Select which IO will be used to generate an interrupt and selects
* the edge at which the interrrupt should get detected
*
*  @param   : pin number
*  @param   : Edge at which interrupt need to be detected
*
*  @return   : None
* =============================================================
 */
 
void AI_SelectIOForIT (SYS_UWORD16 Pin, SYS_UWORD16 Edge)
{

  if (Pin <= 15)
  {
	//Update INTERRUPT_LEVEL_REG with Edge configuration on Pin selection
	if ((Edge & 0x0001))
	{
    	GPIO_INTERRUPT_LEVEL_REG |= (0x0001 << Pin);
	}
	else
	{
		GPIO_INTERRUPT_LEVEL_REG &= ~(0x0001 << Pin);
	}
	//Update INTERRUPT_MASK_REG to enable interrupt generation on Pin selection
	GPIO_INTERRUPT_MASK_REG &= ~(1 << Pin);

  }
  else if (Pin >= 16 && Pin <= 31)
  {
    Pin -= 16;
	//Update INTERRUPT_LEVEL_REG with Edge configuration on Pin selection
    if ((Edge & 0x0001))
	{
            GPIO1_INTERRUPT_LEVEL_REG |= (0x0001 << Pin);
	}
	else
	{
            GPIO1_INTERRUPT_LEVEL_REG &= ~(0x0001 << Pin);
	}
	//Update INTERRUPT_MASK_REG to enable interrupt generation on Pin selection
	GPIO1_INTERRUPT_MASK_REG &= ~(1 << Pin);

  }
  else if (Pin >= 32 && Pin <= 47)
  {
    Pin -= 32;
	//Update INTERRUPT_LEVEL_REG with Edge configuration on Pin selection
    if ((Edge & 0x0001))
	{
            GPIO2_INTERRUPT_LEVEL_REG |= (0x0001 << Pin);
	}
	else
	{
            GPIO2_INTERRUPT_LEVEL_REG &= ~(0x0001 << Pin);
	}
	//Update INTERRUPT_MASK_REG to enable interrupt generation on Pin selection
	GPIO2_INTERRUPT_MASK_REG &= ~(1 << Pin);
  }


}


/* ============================================================
*
* AI_MaskIT()
* Description:Mask the Iinterrupt specified by d_io_number.
* This function is for Locosto(I-Sample)
*
*  @param  :    pin number
*
*  @return   :    ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
*
* =============================================================
*/
UINT8 AI_MaskIT(SYS_UWORD16 d_io_number) {

    if(d_io_number>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

	if (d_io_number <= 15)
	{
		GPIO_INTERRUPT_MASK_REG |= (1 << d_io_number);
    }
    else if (d_io_number >=16 && d_io_number <= 31)
	{
		d_io_number -= 16;
		GPIO1_INTERRUPT_MASK_REG |= (1 << d_io_number);
    }
    else if (d_io_number >=32 && d_io_number <=47)
	{
		d_io_number -= 32;
		GPIO2_INTERRUPT_MASK_REG |= (1 << d_io_number);
	}
    return ARMIO_OK;
}
  
/* ============================================================
*
* AI_UnmaskIT()
* Description:Unmask the Interrupt specified by 'd_io_number'
* this function is for Calypso(E-Sample)
*
*  @param  :    pin number
*
*  @return   :   ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
=============================================================
*/

UINT8 AI_UnmaskIT(SYS_UWORD16 d_io_number) {

    if(d_io_number>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

	if ( d_io_number <= 15)
	{
		GPIO_INTERRUPT_MASK_REG &= ~(1 << d_io_number);
    }
    else if (d_io_number >=16 && d_io_number <= 31)
	{
		d_io_number -= 16;
		GPIO1_INTERRUPT_MASK_REG &= ~(1 << d_io_number);
    }
    else if (d_io_number >=32 && d_io_number <=47)
	{
		d_io_number -= 32;
		GPIO2_INTERRUPT_MASK_REG &= ~(1 << d_io_number);
	}
    return ARMIO_OK;
}


/* ============================================================
*
* gpio_sleep()
* Description:This function will be called by the L1 when going to deep sleep
* These configuration are required to obtain maximum power saving.
*
*  @param  :    none
*
*  @return   :   none
*
* =============================================================
*/

void gpio_sleep(void)
{
	pin_configuration_camera_irda(0); //Configure to IrDA
	GPIO_DIRECTION_OUT(2);	GPIO_CLEAR_OUTPUT(2);	// GPIO_2 set to Output LOW
	GPIO_DIRECTION_OUT(4);	GPIO_SET_OUTPUT(4);	// GPIO_4 is set to Output High
    /* GPIO_DIRECTION_OUT(11);	GPIO_SET_OUTPUT(11); SIM Transistor needs to be switched off */	// GPIO_11 is set to Output High
	GPIO_DIRECTION_OUT(35);	GPIO_CLEAR_OUTPUT(35);	// GPIO_35 set to Output LOW

	//SW Fix for avoid LCD data bus floating during deep sleep
	//Configure the LCD data bus in debug mode.
	CONF_LCD_CAM_NAND &= ~0x0003;
	CONF_LCD_DATA_0	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_1	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_2	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_3	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_4	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_5	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_6	 = MUX_CFG(2, PULLOFF);
	CONF_LCD_DATA_7	 = MUX_CFG(2, PULLOFF);

}

/* ============================================================
*
* gpio_wakeup()
* Ddescription: Function called by the L1 during wakeup.
*
*  @param  :    none
*
*  @return   :   none
* =============================================================
*/
void gpio_wakeup(void)
{
	CONF_LCD_CAM_NAND |= 0x03;
	pin_configuration_camera_irda(1); //Configure to camera again
}

/* ============================================================
*
* AI_Set_Clear_Reg()
* Ddescription: Function used to clear the Interrupt Status Register.
*
*  param: pin number to be cleared
*
* return  : none
* =============================================================
*/
void AI_Set_Clear_Reg(SYS_UWORD8 GPIO_int_pin)
{
    if (GPIO_int_pin <= 15)
    {
        GPIO_SOFT_CLEAR_REG |= (1 << GPIO_int_pin);
    }
    else if (GPIO_int_pin >=16 && GPIO_int_pin <= 31)
    {
        GPIO_int_pin -= 16;
        GPIO1_SOFT_CLEAR_REG |= (1 << GPIO_int_pin);
    }
    else if (GPIO_int_pin >=32 && GPIO_int_pin <=47)
    {
        GPIO_int_pin -= 32;
        GPIO2_SOFT_CLEAR_REG |= (1 << GPIO_int_pin);
    }
}

/* ============================================================
*
* AI_Activate_HISR()
* Description:Function to activate the HISR.
*
*  @param : none
*
*  @return : none
* =============================================================
*/
void AI_Activate_HISR(void)
{
    NU_Activate_HISR(&GPIO_Hisr);
}

/* ============================================================
*
* Al_HisrEntry()
* Description:Entry functiopon for HISR
*
*  @param : none
*
*  @return : none
*
* =============================================================
*/
void Al_HisrEntry(void)
{
    UINT8 i                                  = 0;
    UINT8 ARMIO_int_pin                      = 0;
    volatile UINT16 ARMIO_int_status_reg     = 0;
	UINT16 ARMIO_pending_it                  = 0;
    volatile UINT16 ARMIO_it_mask[3];
	
	ARMIO_it_mask[0] = GPIO_INTERRUPT_MASK_REG;
	ARMIO_it_mask[1] = GPIO1_INTERRUPT_MASK_REG;
	ARMIO_it_mask[2] = GPIO2_INTERRUPT_MASK_REG;
	
	/* Mask all GPIO interrupts - needed to be able to detect multiple interrupts without any issue  */
    GPIO_INTERRUPT_MASK_REG  = MASK_ALL_INTERRUPT; // Mask All Interrupts 0
    GPIO1_INTERRUPT_MASK_REG = MASK_ALL_INTERRUPT; // Mask All Interrupts 1
    GPIO2_INTERRUPT_MASK_REG = MASK_ALL_INTERRUPT; // Mask All Interrupts 2	
	
    /* Check if the interrupt is raised by GPIO_0 */
    if(GPIO_INTERRUPT_STATUS_REG!=NULL)
    {	
        ARMIO_int_status_reg = GPIO_INTERRUPT_STATUS_REG;
		ARMIO_pending_it = ARMIO_int_status_reg & ~(ARMIO_it_mask[0]);
		
        for(i=0;i<=15;i++)
        {
            if ( (ARMIO_pending_it >> i) == 1)
            {
                 ARMIO_int_pin=i;
            }
        }
    }
	
    /*Check if the interrupt is raised by GPIO 1 */
   if (GPIO1_INTERRUPT_STATUS_REG != NULL)
    {
        ARMIO_int_status_reg= GPIO1_INTERRUPT_STATUS_REG;
		ARMIO_pending_it = ARMIO_int_status_reg & ~(ARMIO_it_mask[1]);
        for(i=0;i<=15;i++)
        {
            if ( (ARMIO_pending_it >>i) == 1)
            {
                ARMIO_int_pin=16+i;
            }
        }
    }

    /*Check if the interrupt is raised by GPIO 2 */
    if(GPIO2_INTERRUPT_STATUS_REG!= NULL)
    {
        ARMIO_int_status_reg= GPIO2_INTERRUPT_STATUS_REG;
		ARMIO_pending_it = ARMIO_int_status_reg & ~(ARMIO_it_mask[2]);
        for(i=0;i<=15;i++)
        {
            if ( (ARMIO_pending_it >>i) == 1)
            {
                 ARMIO_int_pin=32+i;
            }
        }
    }
	
    /*Assign the callback address to the local function pointer declaration */
    call_back_fun=func_ptr_Arr[ARMIO_int_pin];

    AI_Set_Clear_Reg(ARMIO_int_pin);

    /*Call the callback*/
    call_back_fun();

	/* Restore GPIO interrupt masks */
	GPIO_INTERRUPT_MASK_REG = ARMIO_it_mask[0];
    GPIO1_INTERRUPT_MASK_REG = ARMIO_it_mask[1];
    GPIO2_INTERRUPT_MASK_REG = ARMIO_it_mask[2];
}

/* ============================================================
*
* AI_Int_enable_in_MIR()
* Description:Function to enable the interrupts in MIR registers
*
*  @param : none
*
*  @return : none
*
* =============================================================
*/
void AI_Int_enable_in_MIR(void)
{
    UINT16 get_MIR_1_value=0;
    get_MIR_1_value = MIR_1;
    get_MIR_1_value &= ARMIO_MIR_1_MASK;
    MIR_1=get_MIR_1_value;
}

/* ============================================================
*
* AI_int_init()
* Description: Function for initializing the interrupts
*
*  @param : none
*
*  @return : none
*
* =============================================================
*/
 void AI_int_init(void)
{
STATUS status_ret;

    // Fill the entire stack with the pattern 0xFE
    #ifndef HISR_STACK_SHARING
    memset (GPIO_HisrStack, 0xFE, sizeof(GPIO_HisrStack));
    #endif

    // Create the HISR which is called when an GPIO interrupt is received.
    status_ret = NU_Create_HISR(&GPIO_Hisr, "GPIO_HISR", Al_HisrEntry, 2,
    #ifndef HISR_STACK_SHARING
    GPIO_HisrStack, sizeof(GPIO_HisrStack)); // lowest prty
    #else
    HISR_STACK_PRIO2,
    HISR_STACK_PRIO2_SIZE); // lowest prty
    #endif
    AI_Int_enable_in_MIR();

}

/* ============================================================
*
* Al_int_handler()
* Description: The interrupt handler registered in Init.c
*
*  @param : none
*
*  @return : none
*
* =============================================================
*/
void Al_int_handler(void)
{
    //disable the interrupts
    F_INTH_DISABLE_ONE_IT(C_INTH_GPIO_IT);
    F_INTH_DISABLE_ONE_IT(C_INTH_GPIO1_IT);
    F_INTH_DISABLE_ONE_IT(C_INTH_GPIO2_IT);

    AI_Activate_HISR();

    //enable the interupts
    F_INTH_ENABLE_ONE_IT(C_INTH_GPIO_IT);
    F_INTH_ENABLE_ONE_IT(C_INTH_GPIO1_IT);
    F_INTH_ENABLE_ONE_IT(C_INTH_GPIO2_IT);
}

/* ============================================================
*
* Al_int_config() The function used by the application to configure the interrupts
*
*  @param :pin number
    @param : callback function pointer
*  @param : Edge at which the interrupt should get detected
*
* @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
* =============================================================
*/
UINT8 Al_int_config(SYS_UWORD8 pin, void(*callback_func_ptr)(void),SYS_UWORD16 Edge)
{
    static UINT8 ISR_Init_Flag;
    UINT8 GPIO_pin_no=0;
    UINT8 Max_GPIO_Pin=47;

    if(pin>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

    //Interrupt status Register initialisation perform this only once
    if(ISR_Init_Flag!=1)
    	{
           for(GPIO_pin_no=0;GPIO_pin_no<Max_GPIO_Pin;GPIO_pin_no++)
              {
                  AI_MaskIT(GPIO_pin_no);
                  AI_Set_Clear_Reg(GPIO_pin_no);
              }
           ISR_Init_Flag=1;
    	}

    AI_ConfigBitAsInput(pin);
    AI_SelectIOForIT(pin,Edge);

    func_ptr_Arr[pin]=callback_func_ptr;

     return ARMIO_OK;
}

/* ============================================================
*
* AI_Enable_Debounce()
* Description: Function used to enable the debounce functionality.
*
*  @param  : pin number
*  @param  : Edge at which the the signall should get deteceted
*  @param  : Selection of clock source
*
*  @return :ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
* =============================================================
*/
UINT8  AI_Enable_Debounce(UINT8 pin, UINT8 Edge, UINT8 clock)
{
    UINT8 KHz32=0;
    UINT8 MHz13=1;
    UINT16 mask=0x00;

    if(pin>47)
    {
        return ARMIO_PIN_NO_ERR;
    }
    AI_ConfigBitAsInput(pin);
    AI_UnmaskIT(pin);

    if(pin<=15)
    mask=pin;
    else if (pin >=16 && pin <= 31)
    {
        mask = pin-16;
    }
    else if (pin >=32 && pin <=47)
    {
        mask=pin- 32;
    }
    mask=mask << 1;
    mask|=0x01;   //making the Debouce bit to 1

    if(Edge==1)
    mask|=1<<DEBOUNCE_EDGE;

    if(clock==MHz13)
    mask|=1<<DEBOUNCE_13MHZ_CLOCK;

    if (pin<=15)
    {
        GPIO_JOGDIAL_MODE_REG =mask ;
    }
    else if (pin >=16 && pin <= 31)
    {
        pin -= 16;
        GPIO1_JOGDIAL_MODE_REG = mask;
    }
    else if (pin >=32 && pin <=47)
    {
        pin -= 32;
        GPIO2_JOGDIAL_MODE_REG = mask;
    }

    return ARMIO_OK;
}

/* ============================================================
*
* AI_Disable_Debounce()
* Description:This function to disable debounce functionalty
*
*  @param  :    pin number
*
*  @return   :   ARMIO_OK if pin no is proper else ARMIO_PIN_NO_ERR
* =============================================================
*/
UINT8 AI_Disable_Debounce(SYS_UWORD8 pin)
{
    UINT16 mask=0xFFFF;

    if(pin>47)
    {
        return ARMIO_PIN_NO_ERR;
    }

    if (pin <= 15)
    {
        GPIO_JOGDIAL_MODE_REG&=(pin<<1);
    }
    else if (pin >= 16 && pin <= 31)
    {
        pin -= 16;
        GPIO1_JOGDIAL_MODE_REG&=(pin<<1);
    }
    else if (pin >= 32 && pin <= 47)
    {
        pin -= 32;
        GPIO2_JOGDIAL_MODE_REG&=(pin<<1);
    }

    return ARMIO_OK;
}

/* ============================================================
*
* AI_Set_Debounce_Time()
* Description:Function to Set the debounce time
*
*  @param    : Debounce time
*
*  @return  : none
* =============================================================
*/
void AI_Set_Debounce_Time(UINT16 Time)
{
    GPIO_JD_DEBOUNCING_REG = GPIO1_JD_DEBOUNCING_REG
		                                 =GPIO2_JD_DEBOUNCING_REG = Time;

}

