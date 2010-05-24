
;                             *-----------------*
;                             * CP Register Map *
;                             *-----------------*

;Note: All registers are named the same as in the Technical Manual 
;      but with an additional "HWD_" prefix.                         
;
;*---------------------------------------------------------------------------------------------
;*   Name                    Addr         R/W  Description                           Reset 
;*---------------------------------------------------------------------------------------------
HWD_IRAM_BASE_ADDRESS EQU    0x00000000   ;R/W  CP IRAM
HWD_IRAM_BASE_ADDR          EQU    0x0b900000   ;R/W  CP IRAM
HWD_IRAM_BASE_ADDR_REMAP    EQU    0x00000000   ;R/W  CP IRAM
HWD_IRAM_SIZE               EQU     0x00006000
;* Li Wang Added Temporariy */
HWD_IRAM_RESET_FLAG_ADDR   EQU     (HWD_IRAM_BASE_ADDR + HWD_IRAM_SIZE - 2)

;*------------*
;* Interrupts *
;*------------*

HWD_CP_ISR0_L         EQU    0x0b800000   ;R/W  CP Low Priority Interrupt Status 0     0x0    
HWD_CP_IMR0_L         EQU    0x0b800004   ;R/W  CP Low Priority Interrupt Mask 0       0xffff 
HWD_CP_SRC0_L         EQU    0x0b800008   ;R    CP Low Priority Source 0               0x0 

HWD_CP_ISR1_L         EQU    0x0b800010   ;R/W  CP Low Priority Interrupt Status 1     0x0    
HWD_CP_IMR1_L         EQU    0x0b800014   ;R/W  CP Low Priority Interrupt Mask 1       0xffff 
HWD_CP_SRC1_L         EQU    0x0b800018   ;R    CP Low Priority Source 1               0x0 

HWD_CP_ISR2_L         EQU    0x0b800020   ;R/W  CP Low Priority Interrupt Status 2     0x0    
HWD_CP_IMR2_L         EQU    0x0b800024   ;R/W  CP Low Priority Interrupt Mask 2       0xffff 
HWD_CP_SRC2_L         EQU    0x0b800028   ;R    CP Low Priority Source 2               0x0 

HWD_CP_ISR3_L         EQU    0x0b800030   ;R/W  CP Low Priority Interrupt Status 2     0x0    
HWD_CP_IMR3_L         EQU    0x0b800034   ;R/W  CP Low Priority Interrupt Mask 2       0xffff 
HWD_CP_SRC3_L         EQU    0x0b800038   ;R    CP Low Priority Source 2               0x0 

HWD_CP_ISR0_H         EQU    0x0b800200   ;R/W  CP High Priority Interrupt Status 0    0x0    
HWD_CP_IMR0_H         EQU    0x0b800204   ;R/W  CP High Priority Interrupt Mask 0      0xffff    
HWD_CP_SRC0_H         EQU    0x0b800208   ;R    CP High Priority Source 0              0x0    

HWD_CP_ISR1_H         EQU    0x0b800210   ;R/W  CP High Priority Interrupt Status 1    0x0    
HWD_CP_IMR1_H         EQU    0x0b800214   ;R/W  CP High Priority Interrupt Mask 1      0xffff    
HWD_CP_SRC1_H         EQU    0x0b800218   ;R    CP High Priority Source 1              0x0    

HWD_CP_ISR2_H         EQU    0x0b800220   ;R/W  CP High Priority Interrupt Status 2    0x0    
HWD_CP_IMR2_H         EQU    0x0b800224   ;R/W  CP High Priority Interrupt Mask 2      0xffff    
HWD_CP_SRC2_H         EQU    0x0b800228   ;R    CP High Priority Source 2              0x0    

HWD_CP_ISR3_H         EQU    0x0b800230   ;R/W  CP High Priority Interrupt Status 3    0x0    
HWD_CP_IMR3_H         EQU    0x0b800234   ;R/W  CP High Priority Interrupt Mask 3      0xffff    
HWD_CP_SRC3_H         EQU    0x0b800238   ;R    CP High Priority Source 3              0x0    

;*---------------*            
;* CP Interface  *            
;*---------------*
HWD_CI_CSCFG01         EQU    0x0bc00000   ;R/W    CP Chip select config reg0           0x7777 
HWD_CI_CSCFG23         EQU    0x0bc00004   ;R/W    CP Chip select config reg1           0xFFFF
HWD_CI_CSCFG45         EQU    0x0bc00008   ;R/W    CP Chip select config reg2           0xFFFF 

HWD_EMC_AW_SN_RMAP    EQU    0x0bc0000c   ;R/W    CP External Memory Config AW_SN_RMAP 0x00AF

;*---------------*            
;* System Timing *            
;*---------------*            

HWD_ST_CPINT_FR       EQU    0x0b860000   ;R/W  Frame source type (20 or 26 ms)        0x?
HWD_ST_CPINT_CLR      EQU    0x0b860004   ;R/W  Clear status register                  0x?
HWD_ST_CPINT_MASK     EQU    0x0b860008   ;R/W  Interrupt Mask Register                0x?
HWD_ST_CPINT_SRC      EQU    0x0b86000C   ;R    Interrupt Source Register              0x?
HWD_ST_CPINT_CNT      EQU    0x0b860010   ;R    Subframe count (TBD)                   0x?


;*--------*                   
;* UARTs  *                   
;*--------*

HWD_UART_EXT_BASE     EQU    0x04000000
HWD_UART_INT_BASE     EQU    0x0b8a0000

 ; Internal UART only otherwise
HWD_UART_BASE         EQU    HWD_UART_INT_BASE       ;Uart base address
HWD_UART0_BASE        EQU    HWD_UART_BASE + 0x0000  ;Uart 0 base address
HWD_UART1_BASE        EQU    HWD_UART_BASE + 0x0100  ;Uart 1 base address
HWD_UART2_BASE        EQU    HWD_UART_BASE + 0x0200  ;Uart 2 base address

;*-----------*
;* CP GPIO   *          
;*-----------*          

HWD_NM_GP0            EQU    0x0b8f0078   ;R/W  CP GPIO normal function for GPIO[15:0]   0x0    
HWD_NM_GP1            EQU    0x0b8f007c   ;R/W  CP GPIO normal function for GPIO[31:16]  0x0    
HWD_NM_GP2            EQU    0x0b8f00e0   ;R/W  CP GPIO normal function for GPIO[47:32]  0x0    

;*------*
;* Test *
;*------*

 IF SYS_BOARD=SB_CDS4
HWD_TEST_BASE         EQU    0x03000000              ;Test reg base address CS3
HWD_LED_CTRL          EQU    HWD_TEST_BASE + 0x10    ;LED  control register
HWD_SW_CFG            EQU    HWD_TEST_BASE + 0x0C    ;DIP switch for SW configuration 0x0
 ENDIF
     
;*-----------------*          
;* Flash Memory    *          
;*-----------------*          

HWD_FLASH_BASE_ADDRESS      EQU 0x07000000              ;Flash base address CS7
HWD_FLASH_CP_CODE_OFFSET    EQU 0x060000             ;Offset to start of CP code in flash 1
HWD_START_FLASH_BOOT_CODE_ADDR   EQU  HWD_FLASH_BASE_ADDRESS
FLASH_VALID_PATTERN              EQU  0x4c53494c   ; stands for "LSIL" in ascii
HWD_CONTROL_VALUE                EQU  0xc4c4c4c4
HWD_OFFLINE_VALUE                EQU  0x59595959
;            *--------------------------------------------*
;            * Register Bitfield And Constant Definitions *
;            *--------------------------------------------*

;*------------*
;* Interrupts *
;*------------*

; HWD_CP_ISR0_L & HWD_CP_ISR0_H - Interrupt bit positions 
HWD_INT_SLP_RESYNC    EQU    0x8000  ; System resync event
HWD_INT_SER0          EQU    0x4000  ; Serial Programmer 0 interrupt flag
HWD_INT_SER1          EQU    0x2000  ; Serial Programmer 1 interrupt flag
HWD_INT_SER2          EQU    0x1000  ; Serial Programmer 2 interrupt flag
HWD_INT_UU0_11        EQU    0x0800  ; Unused
HWD_INT_SLPTW         EQU    0x0400  ; Sleep timer wakeup
HWD_INT_SLPTE         EQU    0x0200  ; Sleep Timer end flag 
HWD_INT_V2CR          EQU    0x0100  ; Voice to CP Read Ready
HWD_INT_V2CW          EQU    0x0080  ; Voice to CP Write Ready 
HWD_INT_EED           EQU    0x0040  ; Eeprom programming done flag 
HWD_INT_UU3           EQU    0x0020  ; Unused
HWD_INT_KEYPAD        EQU    0x0010  ; Keypad is pressed/released
HWD_INT_M2CDR         EQU    0x0008  ; Modem to CP Data Read Ready
HWD_INT_M2CCR         EQU    0x0004  ; Modem to CP Control Read Ready
HWD_INT_M2CDW         EQU    0x0002  ; Modem to CP Data Write Ready
HWD_INT_M2CCW         EQU    0x0001  ; Modem to CP Control Write Ready
HWD_INT_SER_ALL       EQU    (HWD_INT_SER0 | HWD_INT_SER1)

; HWD_CP_ISR1_L & HWD_CP_ISR1_H - Interrupt bit positions
HWD_INT_GPIO5        EQU     0x8000  ; GPIO 5 Interrupt 
HWD_INT_GPIO4        EQU     0x4000  ; GPIO 4 Interrupt
HWD_INT_GPIO3        EQU     0x2000  ; GPIO 3 Interrupt
HWD_INT_GPIO2        EQU     0x1000  ; GPIO 2 Interrupt
HWD_INT_GPIO1        EQU     0x0800  ; GPIO 1 Interrupt
HWD_INT_GPIO0        EQU     0x0400  ; GPIO 0 Interrupt
HWD_INT_PLLU         EQU     0x0200  ; PLL Unlocked
HWD_INT_EOC          EQU     0x0100  ; End of Compare Interrupt
HWD_INT_V2CFM        EQU     0x0080  ; Fast MailBox to CP from DSPV Interrupt
HWD_INT_M2CFM        EQU     0x0040  ; Fast MailBox to CP from DSPM Interrupt
HWD_INT_GPINTX       EQU     0x0020  ; GPIO_GI Interrupt
HWD_INT_IRAM_WF      EQU     0x0010  ; IRAM Write Failure Interrupt
HWD_INT_TIM3         EQU     0x0008  ; Timer 3 Interrupt
HWD_INT_TIM2         EQU     0x0004  ; Timer 2 Interrupt
HWD_INT_TIM1         EQU     0x0002  ; Timer 1 Interrupt
HWD_INT_TIM0         EQU     0x0001  ; Timer 0 Interrupt
HWD_INT_CTS_ALL      EQU     0x000F  

; HWD_CP_ISR2_L & HWD_CP_ISR2_H - Interrupt bit positions 
HWD_INT_POLYRINGER   EQU     0x8000  ; Polyphonic Ringer Fast Interrupt
HWD_INT_UU2_14       EQU     0x4000  ; Unused
HWD_INT_UAWAK        EQU     0x2000  ; UART Wakeup Interrupt
HWD_INT_SWINT0       EQU     0x1000  ; Polyphonic Ringer Slow Interrupt
HWD_INT_RTOS         EQU     0x0800  ; RTOS Interrupt
HWD_INT_VMIC         EQU     0x0400  ; Voice Codec Microphone Interrupt
HWD_INT_VSPK         EQU     0x0200  ; Voice Codec Speaker Interrupt
HWD_INT_VMS          EQU     0x0100  ; Combined Microphone/Speaker Interrupt
HWD_INT_UITX         EQU     0x0080  ; User Interface Transmit Interrupt
HWD_INT_UIRX         EQU     0x0040  ; User Interface Receive Interrupt
HWD_INT_UU2_05       EQU     0x0020  ; Unused
HWD_INT_UU2_04       EQU     0x0010  ; Unused
HWD_INT_RNG_A        EQU     0x0008  ; Ringer Interrupt
HWD_INT_SSTIM        EQU     0x0004  ; Single Shot Timer Interrupt
HWD_INT_USBWAL       EQU     0x0002  ; USB Wakeup Interrupt
HWD_INT_USB          EQU     0x0001  ; USB Interrupt
HWD_INT_DAI          EQU     0x0700  ; All Voice Codec Interrupts

; HWD_CP_ISR3_L & HWD_CP_ISR3_H - Interrupt bit positions 
HWD_INT_RFSP1        EQU     0x0002  ;RF Serial Programmer Interface Interrupt 1 
HWD_INT_RFSP0        EQU     0x0001  ;RF Serial Programmer Interface Interrupt 0 

;*--------*                   
;* UARTs  *                   
;*--------*

HWD_ISR_INT_PEND     EQU     0x01           ;UART ISR pending
HWD_UART_ISR         EQU     0x08           ;Uart Interrupt status reg offset

;*--------*                   
;* Test   *                   
;*--------*

 IF SYS_BOARD=SB_CDS4
; LED_CTRL 
HWD_LED_1            EQU     0x0001
HWD_LED_2            EQU     0x0002
HWD_LED_3            EQU     0x0004
HWD_LED_4            EQU     0x0008

; SW_CFG SW Config switch settings
HWD_SWITCH_1         EQU     0x0001
HWD_SWITCH_2         EQU     0x0002
HWD_SWITCH_3         EQU     0x0004
HWD_SWITCH_4         EQU     0x0008
HWD_SWITCH_5         EQU     0x0010
HWD_SWITCH_6         EQU     0x0020
HWD_SWITCH_7         EQU     0x0040
HWD_SWITCH_8         EQU     0x0080

 ENDIF

;HWD_RNGR_CTRL bit definitions
HWD_RNGR_FOREVER     EQU     0x0020
HWD_RNGR_IMMED       EQU     0x0010
HWD_RNGR_TSTMEM_EN   EQU     0x0008
HWD_RNGR_MEMSEL      EQU     0x0004
HWD_RNGR_DAT         EQU     0x0002
HWD_RNGR_EN          EQU     0x0001

;HWD_RNGR_STAT bit definitions
HWD_RNGR_UNDERFLOW_A EQU     0x0001
HWD_RNGR_UNDERFLOW_B EQU     0x0002
HWD_RNGR_UNDERFLOW_C EQU     0x0004

;HWD_ST_CPINT_SRC bit definitions
 
HWD_ST_CPINT_ALL     EQU     0xFFFF

;Configuration register definitions

;HWD_CI_CSCFG01 definitions
HWD_CI_CSCFG0_RWT0 EQU     (5)            ;CS0 Read Wait States, bits 0-3
HWD_CI_CSCFG0_WWT0 EQU     (5 :SHL: 4)    ;CS0 Write Wait States, bits 4-7
HWD_CI_CSCFG0_RWT1 EQU     (5 :SHL: 8)    ;CS1 Read Wait States, bits 8-11
HWD_CI_CSCFG0_WWT1 EQU     (5 :SHL: 12)   ;CS1 Write Wait States, bits 12-15

;HWD_CI_CSCFG23 definitions
HWD_CI_CSCFG1_RWT2 EQU     (2)            ;CS2 Read Wait States, bits 0-3
HWD_CI_CSCFG1_WWT2 EQU     (2 :SHL: 4)    ;CS2 Write Wait States, bits 4-7
HWD_CI_CSCFG1_RWT3 EQU     (4 :SHL: 8)    ;CS3 Read Wait States, bits 8-11
HWD_CI_CSCFG1_WWT3 EQU     (4 :SHL: 12)   ;CS3 Write Wait States, bits 12-15

;HWD_CI_CSCFG45 definitions
HWD_CI_CSCFG2_RWT4 EQU     (15)           ;CS4 Read Wait States, bits 0-3
HWD_CI_CSCFG2_WWT4 EQU     (15 :SHL: 4)   ;CS4 Write Wait States, bits 4-7
HWD_CI_CSCFG2_RWT5 EQU     (15 :SHL: 8)   ;CS5 Read Wait States, bits 8-11
HWD_CI_CSCFG2_WWT5 EQU     (15 :SHL: 12)  ;CS5 Write Wait States, bits 12-15


;HWD_EMC_AW_SN_REMAP bit definitions
 IF SYS_ASIC=SA_RAM
HWD_EMC_UIM_VLTSEL   EQU (1)          ;Voltage select, UIM at 1.8V, bit 0
HWD_EMC_CPBUS_VLTSEL EQU (1 :SHL: 1)  ;Voltage select, CP Bus at 1.8V, bit 1
 ELSE
HWD_EMC_CPBUS_VLTSEL EQU (1)          ;Voltage select, CP Bus at 1.8V, bit 0
HWD_EMC_UIM_VLTSEL   EQU (1 :SHL: 1)  ;Voltage select, UIM at 1.8V, bit 1
 ENDIF
HWD_EMC_RESERVED     EQU (3 :SHL: 2)  ;Reserved, bits 2-3
HWD_EMC_LCD_ADDR     EQU (2 :SHL: 4)  ;LCD Controller Addr, bits 4-6
HWD_EMC_BOOT_ROM     EQU (1 :SHL: 7)  ;BOOTROM (bit 7), don't care for CBP4
HWD_EMC_CSPOL        EQU (0 :SHL: 8)  ;Active chip select polarity for CS2-5, bits 8-11
HWD_EMC_SN           EQU (0 :SHL: 14) ;Snooze/Normal execution, bit 14
HWD_EMC_REMAP        EQU (1 :SHL: 15) ;Remap IRAM to address 0, bit 15



   END
