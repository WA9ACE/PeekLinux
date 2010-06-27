/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1_PWMGR.H
 *
 *        Filename l1_pwmgr.h
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/


#ifndef __L1_PWMGR_H__
#define __L1_PWMGR_H__

#include "types.h"
#include "l1_types.h"
#include "bspTwl3029.h"
#include "bspTwl3029_I2c.h"
#include "bspI2c.h"
#include "bspTwl3029_Aux_Llif.h"
#include "bspTwl3029_Aux_Map.h"
#include "bspTwl3029_Aud_Llif.h"
#include "bspTwl3029_Audio.h"
#include "bspTwl3029_Aud_Map.h"
#include "bspTwl3029_Int_Map.h"
#include "Armio.h"

#define PWDNSTATUS                       0x6A /*Decimal 106*/


/* If BT is enabled */
#if (BTS == 1)
	extern unsigned char BTHAL_PM_HandleSleepManagerReq(unsigned char ucCmd);
#endif


#if(OP_L1_STANDALONE == 1)
	Uint8 madc_outen_check(Uint8 cmd);
#else
	Uint8 audio_madc_sleep(Uint8 cmd);
#endif

#if ((PSP_STANDALONE == 0)&&(OP_L1_STANDALONE == 0)) //omaps00090550
	#include "uart/uart_pwr.h"
	#include "usb/usb_pwr.h"
	#include "i2c/i2c_pwr.h"
	#include "camd/camera_pwr.h"
	#include "uicc/board/usim_pwr.h"
	#include "r2d/LCDs/I_Sample/lcd_pwr.h"
	Uint8 backlight_pwr_interface(Uint8 cmd);

	#if (ANLG_PG == 1)
	#include "pg1x/bspTwl3029_I2c_pg1x.h"
	#elif (ANLG_PG == 2)
	#include "pg2x/bspTwl3029_I2c_pg2x.h"
	#endif

#endif


#define UART_ID 		0
#define USB_ID			1
#define USIM_ID			2
#define I2C_ID			3
#define LCD_ID			4
#define CAMERA_ID		5
#define BACKLIGHT_ID	6
#define MADC_ID			7  // Not used
#define MADC_AS_ID		8  // Combined ID for MADC and Audio
#define BCI_ID          9  // Added for battery charger check

#define BT_Stack_ID			0
#define IRDA_Stack_ID		1
#define USB_Fax_ID			2
#define USB_MS_ID			3
#define USB_Trace_ID		4
#define AS_ID				5  // Not used
#define BMI_MFW_ID			6



#define ASLEEP		0
#define AWAKE		1


#define	NO_CLOCK	0
#define	DCXO_CLOCK	1
#define DPLL_CLOCK	2
#define APLL_CLOCK	3

#define FAILURE		0
#define SUCCESS		1

#define PM_ACTIVE		1
#define PM_INACTIVE		0

#undef CLK_MASK       // omaps00090550
#define CLK_MASK	0

#define SLEEP_CMD	1
#define WAKE_CMD	2

#define APP_ACTIVITY	0

#define MAX_PERIPHERAL		16
#define MAX_APPLICATIONS 	16


#define RHSW_ARM_CNF		(*(volatile UWORD32 *)0xFFFF8800)
#define DSP_PERIPH_LOCK		0x1
#define ARM_PERIPH_LOCK		0x2
#define APCCTRL2			(*(volatile UWORD32 *)0xFFFF500C)
#define BGEN				0x100

#if (PSP_STANDALONE == 0)

	typedef   Uint8   (* t_peripheral_interface) (Uint8);
	typedef   Uint8   (* t_application_interface) (Uint8);


	Uint8 f_peripheral_interface_dummy(Uint8);
	Uint8 f_application_interface_dummy(Uint8);

    extern const t_peripheral_interface  Peripheral_interface [MAX_PERIPHERAL];
	extern const t_application_interface  Application_interface [MAX_APPLICATIONS];

	static Uint32 SLEEP_STATE=0x3F; // only Six peripherals initialized to active (AWAKE = 1, ASLEEP = 0)//OMAPS00090550

	void   Update_Sleep_Status( Uint8 , Uint8);

	UWORD32 Check_Peripheral_App(void);

#endif  //(PSP_STANDALONE == 0)


/* To be Cleaned Used for DBB_Configure_DS()*/

// omaps00090550 #303warning removal typedef unsigned char  UWORD_8;

            #undef REG16
            typedef volatile unsigned short REG_UWORD16;
			#define REG16(A)    (*(REG_UWORD16*)(A))


	typedef volatile unsigned short REGISTER_UWORD16;


	#define MAP_ULPD_REG	                0xFFFE2000	//ULPD registers start address            (CS4)
	#define ULPD_SETUP_CLK13_REG   	       (*(REGISTER_UWORD16*)((REGISTER_UWORD16 *)(MAP_ULPD_REG) + 14))
	#define ULPD_SETUP_SLICER_REG	       (*(REGISTER_UWORD16*)((REGISTER_UWORD16 *)(MAP_ULPD_REG) + 15))
	#define ULPD_SETUP_VTCXO_REG           (*(REGISTER_UWORD16*)((REGISTER_UWORD16 *)(MAP_ULPD_REG) + 16))

	#define MAP_CLKM_REG	          0xFFFFFD00	 //CLOCKM         registers start address  (CS31)
	#define CLKM_CNTL_CLK_OFFSET         0x02
	#define CLKM_CNTL_CLK_REG            REG16 (MAP_CLKM_REG + CLKM_CNTL_CLK_OFFSET)

	#define EMIF_CONFIG_PWD_POS                      0
	#define EMIF_CONFIG_PDE_POS                      1
	#define EMIF_CONFIG_PREFETCH_POS                3
	#define EMIF_CONFIG_FLUSH_PREFETCH_POS          5
	#define EMIF_CONFIG_WP_POS                      6

	#define            EMIF_CONFIG                              REG16(EMIF_CONFIG_BASE_ADDR+EMIF_CONFIG_REG_OFFSET)
	#define EMIF_CONFIG_BASE_ADDR      0xFFFFFB00      //External Memory inter registers address (CS31)   (NEW)
	#define EMIF_CONFIG_REG_OFFSET          0x02    // Emif configuration register


	#define RNG_CONFIG					REG16(0x09A00028)
    #define	GPIO1_CNTL_REG				REG16(0xFFFE5004)
	#define	CONF_VDR					REG16(0xFFFEF1E6)
	#define	CONF_VFSRX					REG16(0xFFFEF1B8)
    #define SHA_MASK					REG16(0x09800060)
	#define DES_MASK					REG16(0x09900030)
	#define RNG_MASK					REG16(0x09A00040)

	#define COMMAND_REG				(*(unsigned int*)(0x09D00000))
	#define CONTROL_REG				(*(unsigned int*)(0x09D00004))
	#define STATUS_IT_REG			(*(unsigned int*)(0x09D00008))
	#define CNTL_APLL_DIV_CLK		REG16(0xFFFFFD10)
	  // Statements below are not required for the current hardware version.
		 // This was done to solve the problem of DCXO taking 10 frames
		 // to wake-up from Deep Sleep in older hardware versions.
	//#define  DCXO_THRESH_L			*((volatile UWORD16 *)0xFFFF0488)
	//#define  DCXO_THRESH_H			*((volatile UWORD16 *)0xFFFF048A)

  #define FAIL_SLEEP_PERIPH_CHECK   1 /* When initial Check_Peripheral_App */
  #define FAIL_SLEEP_OSTIMERGAUGE   2 /* When checking for Osload, HWtimer or min_time_gauging */
  #define FAIL_SLEEP_PERIPH_SLEEP   3 /* When the peripherals are put to sleep */
  #define FAIL_SLEEP_L1SYNCH        4 /* Failed at l1s_synch level itself */

  #define FAIL_SLEEP_DUE_TO_OSLOAD  0
  #define FAIL_SLEEP_DUE_TO_HWTIMER 1
  #define FAIL_SLEEP_DUE_TO_MINTIMEGAUGING 2

  #define L1_PWMGR_APP_OFFSET    0x80 /* Since the Application id's as BT_Stack_ID and
                                         Peripheral Id's as UART start from zero we need an
                                         application offset in trace to distinguish between the two */
  typedef struct
  {
    UWORD8  fail_id;
    UWORD8  fail_ret_val;
  }T_PWMGR_DEBUG;

/* To be Cleaned Used for DBB_Configure_DS()*/


#endif

