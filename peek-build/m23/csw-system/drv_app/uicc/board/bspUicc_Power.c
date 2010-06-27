/*=============================================================================
 *    Copyright 1996-2004 Texas Instruments Inc. All rights reserved.
 */
#include "sys_types.h"
#include "types.h"

#include "bspUicc_Power.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspTwl3029_Sim.h"
#include "general.h"
#include "sys_conf.h"
#include "rvf_api.h"
#include "rvf_target.h"
#include "pin_config.h"
#include "nucleus.h"
static Bsp_Twl3029_I2cTransReqArray i2cTransArray;
static BspTwl3029_I2C_Callback i2cCallback={NULL,10,&i2cTransArray};

#define rvf_delay gsp_delay
# define BSP_UICC_POWER_ON(callbackFuncPtr)       \
	{ i2cCallback.callbackFunc=callbackFuncPtr; bspTwl3029_Sim_enable(&i2cCallback);}
# define BSP_UICC_POWER_INCREASE() \
	{ bspTwl3029_Sim_setUpVoltage(NULL,BSP_TWL3029_SIM_VOLTAGE_285V); }
# define BSP_UICC_POWER_OFF()      \
	{ bspTwl3029_Sim_disable(NULL); }


#define PUPD_ENABLE 3
#define PUPD_VALUE 4

static NU_SEMAPHORE uicc_mutex;
/*=============================================================================
 *  File Description:
 *  Source File for BSP_UICC interface to control power to the card
 */  

/*=============================================================================
 *   Public Functions
 *============================================================================*/
/* FIXME : See If PVIAS Configuration is required in below functions */
/*=============================================================================
 * Description:
 *   Power on the card at the lowest power level
 */

#pragma CODE_SECTION(uicc_nop_delay, ".nop_delay_sect")
void uicc_nop_delay(SYS_UWORD32 us)
{
//Obtained after experiments using a timer. 
//Assumes that the 'for' loop below is not changed and the MPU frequency is 104 MHz
#define LOOPS_FOR_1MICROSECOND 4000/2899 

   SYS_UWORD32 a;
   SYS_UWORD32 loop_count;

   loop_count = us * LOOPS_FOR_1MICROSECOND;
   
   for (a = 0; a < loop_count; a++) __asm(" nop");
}

static void uicc_callback(Uint8 i)
{
    UINT16 temp;

#ifdef LOCOSTO_PG2
    CONF_LCD_CAM_ND&= ~(1<<2);
#endif

    uicc_nop_delay(40);
    GPIO_CLEAR_OUTPUT(11);
	
/* Disabling the PULL up/down function on Locosto SIM IO trace  */
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

/*Disabling the PULL up/down function on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/* Disabling the PULL up/down function on Locosto SIM CLK trace*/
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/* Disabling the PULL up/down function on Locosto PWR CTL*/
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);
	
	
    NU_Release_Semaphore(&uicc_mutex);
  	
}



BspUicc_Power_ReturnCode
bspUicc_Power_on( void )
{
    UINT16 temp;
/*Making sure that USIM IOS power-down mode is activated.*/
    
    #ifdef LOCOSTO_PG2
    CONF_LCD_CAM_ND|= (1<<2);
    #endif
	
/* Setting of the PBIAS Cell to 1.8 V mode */
   temp=LOCOSTO_CORE_CONF;
   temp&= ~(1<<11);
   LOCOSTO_CORE_CONF=temp;

/*Activate pull down on Locosto SIM IO trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

/* Activate pull down on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/*Activate pull down on Locosto SIM CLK trace */
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/* Activate  PULL down on PWR CTL:CONF_SIM_PWCTRL   */
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);

    BSP_UICC_POWER_ON(uicc_callback);

	NU_Obtain_Semaphore(&uicc_mutex,NU_SUSPEND);
    return BSP_UICC_POWER_RETURN_CODE_SUCCESS;
}

/*=============================================================================
 * Description:
 *   Increase the power level
 */
BspUicc_Power_ReturnCode
bspUicc_Power_increase( void )
{
    UINT16 temp;
	
/*Making sure that USIM IOS power-down mode is activated.*/
    #ifdef LOCOSTO_PG2
    CONF_LCD_CAM_ND|= (1<<2);
    #endif
	
    BSP_UICC_POWER_OFF();
    BSP_UICC_POWER_INCREASE();

/*Activate pull down on Locosto SIM IO trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

/* Activate pull down on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/*Activate pull down on Locosto SIM CLK trace */
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/* Activate  PULL down on PWR CTL:CONF_SIM_PWCTRL   */
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);

/* Set GPIO_11 as active high*/
//    CONF_GPIO_11	 = MUX_CFG(1, PULLUP);	
    GPIO_SET_OUTPUT(11);	// GPIO_11 is set to Output High

/*Setting of the PBIAS Cell to Fast start 3 V*/
   temp=LOCOSTO_CORE_CONF;
   temp&= (~(1<<13));
   temp|=(1<<12);
   LOCOSTO_CORE_CONF=temp;

   temp=LOCOSTO_CORE_CONF;
   temp|= (1<<11);
   LOCOSTO_CORE_CONF=temp;

/* 50 us delay is introduced.*/
//    wait_ARM_cycles(convert_nanosec_to_cycles(50*1000));
    rvf_delay(10); //10 TDMA ticks

    BSP_UICC_POWER_ON(uicc_callback);
	NU_Obtain_Semaphore(&uicc_mutex,NU_SUSPEND);

/* 40 us delay is introduced.*/
//    wait_ARM_cycles(convert_nanosec_to_cycles(40*1000));
//        uicc_nop_delay(40);

#if 0
/* Disable pull down on Locosto SIM IO trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

/* Disable pull down on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/*Disable pull down on Locosto SIM CLK trace */
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/* Disabling the PULL up/down on PWR CTL:CONF_SIM_PWCTRL   */
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp&=~(1<<PUPD_ENABLE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);

/* Set GPIO_11 as active low*/
//  CONF_GPIO_11	 = MUX_CFG(1, PULLDOWN);	
//    GPIO_CLEAR_OUTPUT(11);

/* USIM IOS power on mode.*/
/*
#ifdef LOCOSTO_PG2
    CONF_LCD_CAM_ND&= ~(1<<2);
    #endif
*/    
#endif

    return BSP_UICC_POWER_RETURN_CODE_SUCCESS;
}

/*=============================================================================
 * Description:
 *   Power off the card
 */
BspUicc_Power_ReturnCode
bspUicc_Power_off( void )
{
    UINT16 temp;
/* 5ms delay is introduced.*/
   rvf_delay(1); /*1 tdma tick*/

/* Setting of the PBIAS Cell to default Value */
   temp=LOCOSTO_CORE_CONF;
   temp&= (~(1<<11));
   LOCOSTO_CORE_CONF=temp;


/* Activate pull down on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/*Activate pull down on Locosto SIM CLK trace */
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/*Activate pull down on Locosto SIM IO trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

//disable usim
    #ifdef LOCOSTO_PG2
    CONF_LCD_CAM_ND|= 0x04;
    #endif

    BSP_UICC_POWER_OFF();

/* Activate the PULL up/down on PWR CTL:CONF_SIM_PWCTRL   */
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);


/* Set GPIO_11 as active high*/
//    CONF_GPIO_11	 = MUX_CFG(1, PULLUP);	
    GPIO_SET_OUTPUT(11);	// GPIO_11 is set to Output High

    return BSP_UICC_POWER_RETURN_CODE_SUCCESS;
}

/*=============================================================================
 * Description:
 *   SIM initialization
 */
BspUicc_Power_ReturnCode
bspUicc_Init_sim( void )
{
    UINT16 temp;

/*Activate pull down on Locosto SIM IO trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_IO);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_IO,temp);

/* Activate pull down on Locosto SIM RST trace*/
    temp=C_CONF_IO_REG(C_CONF_SIM_RST);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_RST,temp);

/*Activate pull down on Locosto SIM CLK trace */
    temp=C_CONF_IO_REG( C_CONF_SIM_CLK);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_CLK,temp);

/* Activate the PULL down on PWR CTL:CONF_SIM_PWCTRL   */
    temp=C_CONF_IO_REG( C_CONF_SIM_PWRCTRL);
    temp|=(1<<PUPD_ENABLE);
    temp&=~(1<<PUPD_VALUE);
    F_IO_CONFIG(C_CONF_SIM_PWRCTRL,temp);

/* Disable pull up on GPIO 11*/
    CONF_GPIO_11	 = MUX_CFG(1, PULLOFF);	

/*Set GPIO 11 on Locosto as output and active high*/
    GPIO_DIRECTION_OUT(11);
    GPIO_SET_OUTPUT(11);	// GPIO_11 is set to Output High
//    CONF_GPIO_11	 = MUX_CFG(1, PULLUP);	
    NU_Create_Semaphore(&uicc_mutex,"U_SEM",1,NU_PRIORITY);
    NU_Obtain_Semaphore(&uicc_mutex,NU_SUSPEND);
	
    return BSP_UICC_POWER_RETURN_CODE_SUCCESS;
}
