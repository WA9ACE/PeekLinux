/*
* ============================================================================
*
*	project :	I-Sample Power Optimization
*
*	Author :	Jesper Pedersen
*
* =============================================================================*/

#ifndef __PIN_CONFIG_H_
#define __PIN_CONFIG_H_

typedef volatile unsigned short REG_UWORD16;
#define REG16(A)    (*(REG_UWORD16*)(A))


#define GPIO_BASE_ADDR(no) (0xFFFE4800 + ((no)*0x800)) /*(JP)*/
#define GPIO_DATA_INPUT_OFFSET         0x00
#define GPIO_DATA_OUTPUT_OFFSET        0x02
#define GPIO_DIRECTION_CONTROL_OFFSET  0x04
#define GPIO_CONTROL_OFFSET	0x06

#define GPIO_INPUT(gpio)  REG16((GPIO_BASE_ADDR((gpio & 0xf0) >> 4) + GPIO_DATA_INPUT_OFFSET)) & (1 << (gpio & 0x0f))
#define GPIO_SET_OUTPUT(gpio) REG16((GPIO_BASE_ADDR((gpio & 0xf0) >> 4) + GPIO_DATA_OUTPUT_OFFSET)) |= (1 << (gpio & 0x0f))
#define GPIO_CLEAR_OUTPUT(gpio) REG16((GPIO_BASE_ADDR((gpio & 0xf0) >> 4) + GPIO_DATA_OUTPUT_OFFSET)) &=  ~(1 << (gpio & 0x0f))
#define GPIO_DIRECTION_IN(gpio) REG16((GPIO_BASE_ADDR((gpio & 0xf0) >> 4) + GPIO_DIRECTION_CONTROL_OFFSET)) |= (1 << (gpio & 0x0f))
#define GPIO_DIRECTION_OUT(gpio) REG16((GPIO_BASE_ADDR((gpio & 0xf0) >> 4) + GPIO_DIRECTION_CONTROL_OFFSET)) &= ~(1 << (gpio & 0x0f))

#define	CONF_ABB_IRQ	REG16(0xFFFEF112)
#define	CONF_ADD_21	REG16(0xFFFEF1B4)
#define	CONF_ADV	REG16(0xFFFEF1A0)
#define	CONF_CDO	REG16(0xFFFEF118)
#define	CONF_CK13MHZ_EN	REG16(0xFFFEF1E4)
#define	CONF_CSCLK	REG16(0xFFFEF116)
#define	CONF_CSYNC	REG16(0xFFFEF114)
#define	CONF_GPIO_0	REG16(0xFFFEF100)
#define	CONF_GPIO_1	REG16(0xFFFEF102)
//#define CONF_GPIO_3	REG16(0xFFFEF106)
#define	CONF_GPIO_10	REG16(0xFFFEF146)
#define	CONF_GPIO_11	REG16(0xFFFEF148)
#define	CONF_GPIO_12	REG16(0xFFFEF14A)
#define	CONF_GPIO_13	REG16(0xFFFEF14C)
#define	CONF_GPIO_17	REG16(0xFFFEF156)
#define	CONF_GPIO_18	REG16(0xFFFEF158)
#define	CONF_GPIO_19	REG16(0xFFFEF16A)
#define	CONF_GPIO_2	REG16(0xFFFEF104)
#define	CONF_GPIO_20	REG16(0xFFFEF16C)
#define	CONF_GPIO_21	REG16(0xFFFEF16E)
#define	CONF_GPIO_22	REG16(0xFFFEF170)
#define	CONF_GPIO_23	REG16(0xFFFEF172)
#define	CONF_GPIO_24	REG16(0xFFFEF174)
#define	CONF_GPIO_25	REG16(0xFFFEF176)
#define	CONF_GPIO_26	REG16(0xFFFEF178)
#define	CONF_GPIO_27	REG16(0xFFFEF17A)
#define	CONF_GPIO_28	REG16(0xFFFEF17C)
#define	CONF_GPIO_29	REG16(0xFFFEF17E)
#define	CONF_GPIO_30	REG16(0xFFFEF180)
#define	CONF_GPIO_31	REG16(0xFFFEF186)
#define	CONF_GPIO_32	REG16(0xFFFEF18A)
#define	CONF_GPIO_33	REG16(0xFFFEF18E)
#define	CONF_GPIO_34	REG16(0xFFFEF190)
#define	CONF_GPIO_35	REG16(0xFFFEF194)
#define	CONF_GPIO_36	REG16(0xFFFEF196)
#define	CONF_GPIO_37	REG16(0xFFFEF198)
#define	CONF_GPIO_38	REG16(0xFFFEF19A)
#define	CONF_GPIO_39	REG16(0xFFFEF19C)
#define	CONF_GPIO_4	REG16(0xFFFEF11A)
#define	CONF_GPIO_42	REG16(0xFFFEF1A2)
#define	CONF_GPIO_43	REG16(0xFFFEF1A4)
#define	CONF_GPIO_44	REG16(0xFFFEF1A6)
#define	CONF_GPIO_45	REG16(0xFFFEF1A8)
#define	CONF_GPIO_46	REG16(0xFFFEF1AA)
#define	CONF_GPIO_47	REG16(0xFFFEF1B6)
#define	CONF_GPIO_5	REG16(0xFFFEF11C)
#define	CONF_GPIO_7	REG16(0xFFFEF1BA)
#define	CONF_GPIO_8	REG16(0xFFFEF142)
#define	CONF_GPIO_9	REG16(0xFFFEF144)
#define	CONF_KBC_0	REG16(0xFFFEF128)
#define	CONF_KBC_1	REG16(0xFFFEF12A)
#define	CONF_KBC_2	REG16(0xFFFEF12C)
#define	CONF_KBC_3	REG16(0xFFFEF12E)
#define	CONF_KBR_0	REG16(0xFFFEF13A)
#define	CONF_KBR_1	REG16(0xFFFEF13C)
#define	CONF_KBR_2	REG16(0xFFFEF13E)
#define	CONF_KBR_3	REG16(0xFFFEF140)
#define	CONF_LCD_DATA_0	REG16(0xFFFEF15A)
#define	CONF_LCD_DATA_1	REG16(0xFFFEF15C)
#define	CONF_LCD_DATA_2	REG16(0xFFFEF15E)
#define	CONF_LCD_DATA_3	REG16(0xFFFEF160)
#define	CONF_LCD_DATA_4	REG16(0xFFFEF162)
#define	CONF_LCD_DATA_5	REG16(0xFFFEF164)
#define	CONF_LCD_DATA_6	REG16(0xFFFEF166)
#define	CONF_LCD_DATA_7	REG16(0xFFFEF168)
#define	CONF_LCD_NRST	REG16(0xFFFEF14E)
#define	CONF_LCD_RNW	REG16(0xFFFEF152)
#define	CONF_LCD_RS	REG16(0xFFFEF154)
#define	CONF_LCD_STB	REG16(0xFFFEF150)
#define	CONF_NBSCAN	REG16(0xFFFEF1B2)
#define	CONF_NCS3	REG16(0xFFFEF1E2)
#define	CONF_ND_CE1	REG16(0xFFFEF192)
#define	CONF_ND_NWP	REG16(0xFFFEF184)
#define	CONF_NMOE	REG16(0xFFFEF1E0)
#define	CONF_NRDY	REG16(0xFFFEF19E)
#define	CONF_RNW	REG16(0xFFFEF1DE)
#define	CONF_SIM_CLK	REG16(0xFFFEF124)
#define	CONF_SIM_IO	REG16(0xFFFEF122)
#define	CONF_SIM_PWCTRL	REG16(0xFFFEF126)
#define	CONF_SIM_RST	REG16(0xFFFEF120)
#define	CONF_SPARE_3	REG16(0xFFFEF106)
#define	CONF_TCK	REG16(0xFFFEF188)
#define	CONF_TDI	REG16(0xFFFEF18C)
#define	CONF_TDO	REG16(0xFFFEF1BC)
#define	CONF_TMS	REG16(0xFFFEF182)
#define	CONF_TRST	REG16(0xFFFEF110)
#define	CONF_TSPACT_11	REG16(0xFFFEF130)
#define	CONF_TSPACT_12	REG16(0xFFFEF132)
#define	CONF_TSPACT_13	REG16(0xFFFEF134)
#define	CONF_TSPACT_14	REG16(0xFFFEF136)
#define	CONF_TSPACT_15	REG16(0xFFFEF138)
#define	CONF_UART_CTS	REG16(0xFFFEF1B0)
#define	CONF_UART_RX	REG16(0xFFFEF1AE)
#define	CONF_UART_TX	REG16(0xFFFEF1AC)
#define	CONF_USB_BOOT	REG16(0xFFFEF11E)
#define	CONF_USB_DAT	REG16(0xFFFEF10C)
#define	CONF_USB_RCV	REG16(0xFFFEF108)
#define	CONF_USB_SE0	REG16(0xFFFEF10A)
#define	CONF_USB_TXEN	REG16(0xFFFEF10E)
#define	CONF_VDR	REG16(0xFFFEF1E6)
#define	CONF_VFSRX	REG16(0xFFFEF1B8)

#define	CONF_LOCOSTO_DEBUG  REG16(0xFFFEF020)
#define CONF_LCD_CAM_NAND	REG16(0xFFFEF01E)

#define PULLUP 3
#define PULLDOWN 1
#define PULLOFF 0
#define MUX_CFG(mode, pull_value) (mode|(pull_value<<3))

/*******************************************************
	Configure ALL I/O pins
*******************************************************/
void pin_configuration_all(void);

//#ifdef T_FLASH_HOT_SWAP
/*	Function declaration for new function to configure GPIO 12 for card Insertion/Removal detect	*/
void pin_configuration_mmc(void);
//#endif

/*******************************************************
	Configure Bluetooth I/O pins
*******************************************************/
void pin_configuration_bluetooth(void);

/*******************************************************
	Configure EMIFS I/O pins
*******************************************************/
void pin_configuration_emifs(void);

/*******************************************************
	Configure system I/O pins
*******************************************************/
void pin_configuration_system(void);

/*******************************************************
	Configure LCD and NAND Flash I/O pins
   Mode = 0 : LCD functional. NAND not functional
   Mode = 1 : NAND functional. LCD not functional
*******************************************************/
void pin_configuration_lcd_nand(int mode);

/*******************************************************
	Configure keypad pins
*******************************************************/
void pin_configuration_keypad(void);

/*******************************************************
	Configure SIM I/O pins
*******************************************************/
void pin_configuration_sim(void);

/*******************************************************
	Configure radio I/O pins
*******************************************************/
void pin_configuration_radio(void);

/*******************************************************
	Configure USB I/O pins
*******************************************************/
void pin_configuration_usb(void);

/*******************************************************
	Configure Camera and IrDA I/O pins
   Mode = 0 : IrDA functional. Camera not functional
   Mode = 1 : Camera functional. IrDA not functional
  Use mode 1 ONLY when camera is active i.e. use
  mode 0 during sleep
*******************************************************/
void pin_configuration_camera_irda(int mode);

void pin_configuration_others();

	

#define LCD_LED_GPIO   14
//#define LCD_LED_R_GPIO   9

#endif /* __PIN_CONFIG_H_ */

