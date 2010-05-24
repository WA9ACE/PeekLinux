

#ifndef _HWDEFS_H_
#define _HWDEFS_H_

/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

#include "sysdefs.h"

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/

                             /*-----------------**
                             ** CP Register Map **
                             **-----------------*/

/* Note: All registers are named the same as in the Technical Manual 
 *       but with an additional "HWD_" prefix.                         
 */

/*-------------------------------------------------------------------------------------------------*/
/*      Name                  Addr           R/W  Description                               Reset  */
/*-------------------------------------------------------------------------------------------------*/

/*------------**
** Interrupts **
**------------*/

#define HWD_CP_ISR0_L          0x0b800000  /* R/W  CP Low Priority Interrupt Status 0        0x0000 */
#define HWD_CP_IMR0_L          0x0b800004  /* R/W  CP Low Priority Interrupt Mask 0          0xffff */
#define HWD_CP_SRC0_L          0x0b800008  /* R    CP Low Priority Source 0                  0x0000 */

#define HWD_CP_ISR1_L          0x0b800010  /* R/W  CP Low Priority Interrupt Status 1        0x0000 */
#define HWD_CP_IMR1_L          0x0b800014  /* R/W  CP Low Priority Interrupt Mask 1          0xffff */
#define HWD_CP_SRC1_L          0x0b800018  /* R    CP Low Priority Source 1                  0x0000 */

#define HWD_CP_ISR2_L          0x0b800020  /* R/W  CP Low Priority Interrupt Status 2        0x0000 */
#define HWD_CP_IMR2_L          0x0b800024  /* R/W  CP Low Priority Interrupt Mask 2          0xffff */
#define HWD_CP_SRC2_L          0x0b800028  /* R    CP Low Priority Source 2                  0x0000 */

#define HWD_CP_ISR3_L          0x0b800030  /* R/W  CP Low Priority Interrupt Status 3        0x0000 */
#define HWD_CP_IMR3_L          0x0b800034  /* R/W  CP Low Priority Interrupt Mask 3          0xffff */
#define HWD_CP_SRC3_L          0x0b800038  /* R    CP Low Priority Source 3                  0x0000 */

#define HWD_CP_ISR0_H          0x0b800200  /* R/W  CP High Priority Interrupt Status 0       0x0000 */
#define HWD_CP_IMR0_H          0x0b800204  /* R/W  CP High Priority Interrupt Mask 0         0xffff */
#define HWD_CP_SRC0_H          0x0b800208  /* R    CP High Priority Source 0                 0x0000 */
                                                                               
#define HWD_CP_ISR1_H          0x0b800210  /* R/W  CP High Priority Interrupt Status 1       0x0000 */
#define HWD_CP_IMR1_H          0x0b800214  /* R/W  CP High Priority Interrupt Mask 1         0xffff */
#define HWD_CP_SRC1_H          0x0b800218  /* R    CP High Priority Source 1                 0x0000 */
                                                                               
#define HWD_CP_ISR2_H          0x0b800220  /* R/W  CP High Priority Interrupt Status 2       0x0000 */
#define HWD_CP_IMR2_H          0x0b800224  /* R/W  CP High Priority Interrupt Mask 2         0xffff */
#define HWD_CP_SRC2_H          0x0b800228  /* R    CP High Priority Source 2                 0x0000 */
                                                                               
#define HWD_CP_ISR3_H          0x0b800230  /* R/W  CP High Priority Interrupt Status 3       0x0000 */
#define HWD_CP_IMR3_H          0x0b800234  /* R/W  CP High Priority Interrupt Mask 3         0xffff */
#define HWD_CP_SRC3_H          0x0b800238  /* R    CP High Priority Source 3                 0x0000 */
                                                                               
/*---------------**            
** Modem Mailbox **            
**---------------*/            

#define HWD_MBM_C2M_D_RRDY     0x0b800408  /*   W  CP->Modem Mailbox Data Read Ready */
#define HWD_MBM_C2M_C_RRDY     0x0b80040c  /*   W  CP->Modem Mailbox Control Read Ready */
#define HWD_MBM_C2M_D_WRDY     0x0b800410  /*   W  CP->Modem Mailbox Data Write Ready */
#define HWD_MBM_C2M_C_WRDY     0x0b800414  /*   W  CP->Modem Mailbox Control Write Ready */

#define HWD_MBM_DAT_TX_BASE    0x0ba00000  /* R/W  CP -> DSPM Mailbox Buffer (768 x 16)      -      */
#define HWD_MBM_DAT_RX_BASE    0x0ba00c00  /* R/W  DSPM -> CP Mailbox Buffer (768 x 16)      -      */

/*--------**                   
** Timers **                   
**--------*/                   

/* CTS Timers */
#define HWD_CTS_CMP0           0x0b810000  /* R/W  CP Timer Strobe #0 Comparator Register    0x0000 */
#define HWD_CTS_CMP1           0x0b810004  /* R/W  CP Timer Strobe #1 Comparator Register    0x0000 */
#define HWD_CTS_CMP2           0x0b810008  /* R/W  CP Timer Strobe #2 Comparator Register    0x0000 */
#define HWD_CTS_CMP3           0x0b81000c  /* R/W  CP Timer Strobe #3 Comparator Register    0x0000 */
#define HWD_CTS_CT_CNT_DOUT    0x0b810010  /* R    CP Timer Current Counter Value Register   0x0000 */
#define HWD_CTS_CT_CNTLD_TC    0x0b810014  /* R/W  CP Timer Rollover Count Register          0x00a0 */
#define HWD_CTS_CT_CNT_CTRL    0x0b810018  /* R/W  CP Timer Counter Control Register         0x0000 */
#define HWD_CTS_CT_RDY_FLG     0x0b81001c  /* R/W  CP Timer Read Status Ready Bit Register   0x0000 */

#define HWD_SSTIMER_INIT       0x0b8100e4  /* R/W  Initial value for Single Shot Timer              */

/* RTOS Timer */
#define HWD_RTOS_LOAD_VALUE    0x0b810040  /* R/W  CP RTOS timer load value reg              0x0    */
#define HWD_RTOS_EN            0x0b810044  /* R/W  CP RTOS timer enable reg                  0x0    */
#define HWD_RTOS_CNT_OUT       0x0b810048  /* R    CP RTOS timer counter reg                 0x0    */

/* Watchdog Timer */
#define HWD_WD_ABCD            0x0b810080  /* R/W  CP Watchdog timer update reg              0x0    */
#define HWD_WD_MAX_LOW         0x0b810084  /* R/W  CP Watchdog timer load value low reg      0x0    */
#define HWD_WD_MAX_HIGH        0x0b810088  /* R/W  CP Watchdog timer load value high reg     0x0    */

/*------------------------------**     
** Clock Register Configuration **     
**------------------------------*/     
#define HWD_CLK_CRL_BASE       0x0b820000
#define HWD_CLK_CTRL0          HWD_CLK_CRL_BASE + 0x0000  /* R/W Clock control register 0    0xff9a */
#define HWD_CLK_CTRL1          HWD_CLK_CRL_BASE + 0x0004  /* R/W Clock control register 1    0x7e3f */
#define HWD_CLK_CTRL2          HWD_CLK_CRL_BASE + 0x0008  /* R/W Clock control register 2    0x2613 */
#define HWD_CLK_CTRL3          HWD_CLK_CRL_BASE + 0x000C  /* R/W Clock control register 3    0x001e */
#define HWD_CLK_CTRL4          HWD_CLK_CRL_BASE + 0x0010  /* R/W Clock control register 4    0xfc00 */
#define HWD_CLK_CTRL5          HWD_CLK_CRL_BASE + 0x0064  /* R/W Clock control register 5    0x0000 */

#define HWD_CHIP_ID_LO         0x0b820014  /* R    Chip version                              0x40a1 */
#define HWD_JIT0_OFFSET        0x0b82001C  /* R/W  UART0 Jitter clock offset                 0x0000 */
#define HWD_JIT0_CONST         0x0b820020  /* R/W  UART0 Jitter clock constant               0x0000 */
#define HWD_JIT0_DIV           0x0b820024  /* R/W  UART0 Jitter clock divider                0x0000 */
#define HWD_JIT1_OFFSET        0x0b820028  /* R/W  UART1 Jitter clock offset                 0x0000 */
#define HWD_JIT1_CONST         0x0b82002C  /* R/W  UART1 Jitter clock constant               0x0000 */
#define HWD_JIT1_DIV           0x0b820030  /* R/W  UART1 Jitter clock divider                0x0000 */
#define HWD_JIT2_OFFSET        0x0b820034  /* R/W  UART2 Jitter clock offset                 0x0000 */
#define HWD_JIT2_CONST         0x0b820038  /* R/W  UART2 Jitter clock constant               0x0000 */
#define HWD_JIT2_DIV           0x0b82003C  /* R/W  UART2 Jitter clock divider                0x0000 */
#define HWD_CG_MDM_M           0x0b820040  /* R/W  Modem PLL M divider value                 0x0000 */
#define HWD_CG_MDM_N           0x0b820044  /* R/W  Modem PLL N divider value                 0x0000 */
#define HWD_CG_PROC_M          0x0b820050  /* R/W  Processor PLL M divider value             0x0000 */
#define HWD_CG_PROC_N          0x0b820054  /* R/W  Processor PLL N divider value             0x0000 */
#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_CG_MDM_CONST       0x0b820048  /* R/W  Modem PLL constant                        0x0000 */
#define HWD_CG_MDM_OFFSET      0x0b82004C  /* R/W  Modem PLL offset                          0x0000 */
#define HWD_CG_PROC_CONST      0x0b820058  /* R/W  Processor PLL constant                    0x0000 */
#define HWD_CG_PROC_OFFSET     0x0b82005C  /* R/W  Processor PLL offset                      0x0000 */
#endif
#define HWD_CG_PLL_CTRL        0x0b820060  /* R/W  PLL control                               0x0033 */
#define HWD_CG_USB_M           0x0b820068  /* R/W  USB PLL M divider value                   0x0000 */
#define HWD_CG_USB_N           0x0b82006c  /* R/W  USB PLL N divider value                   0x0000 */
#define HWD_VC_JIT_OFFSET      0x0b82020c  /* R/W  Voice Codec Jitter clock offset           0x0000 */
#define HWD_VC_JIT_CONST       0x0b820210  /* R/W  Voice Codec Jitter clock constant         0x0000 */
#define HWD_VC_JIT_DIV         0x0b820214  /* R/W  Voice Codec Jitter clock divider          0x0000 */
#define HWD_CG_DSP_M           0x0b820218  /* R/W  DSP PLL M divider value                   0x0000 */
#define HWD_CG_DSP_N           0x0b82021c  /* R/W  DSP PLL N divider value                   0x0000 */
#define HWD_CG_DSP_PLL_CTRL    0x0b820220  /* R/W  PLL control                               0x0033 */
#if ((SYS_ASIC == SA_ROM) || (SYS_ASIC == SA_CBP55))
#define HWD_CG_SPARE_REG       0x0b820224  /* R/W 2 bit CP spare register                           */
#define HWD_SCH_DISABLE_BIT    0x4000      /* SCH_DISBALE bitmap in HWD_CG_SPARE_REG reg.           */                            
#endif
/*--------------------------------**
** DSPM patch and match addresses **
**--------------------------------*/

#define HWD_DM_PTCH_EN0        0x0b820078  /* R/W  DSPM patch enable  (15-0)   */
#define HWD_DM_PTCH_EN1        0x0b82007c  /* R/W  DSPM patch enable  (17-16)  */

#define HWD_DM_PTCH_AD0        0x0b820080  /* R/W  DSPM patch address 0        */
#define HWD_DM_PTCH_AD1        0x0b820084  /* R/W  DSPM patch address 1        */
#define HWD_DM_PTCH_AD2        0x0b820088  /* R/W  DSPM patch address 2        */
#define HWD_DM_PTCH_AD3        0x0b82008c  /* R/W  DSPM patch address 3        */
#define HWD_DM_PTCH_AD4        0x0b820090  /* R/W  DSPM patch address 4        */
#define HWD_DM_PTCH_AD5        0x0b820094  /* R/W  DSPM patch address 5        */
#define HWD_DM_PTCH_AD6        0x0b820098  /* R/W  DSPM patch address 6        */
#define HWD_DM_PTCH_AD7        0x0b82009c  /* R/W  DSPM patch address 7        */
#define HWD_DM_PTCH_AD8        0x0b8200a0  /* R/W  DSPM patch address 8        */
#define HWD_DM_PTCH_AD9        0x0b8200a4  /* R/W  DSPM patch address 9        */
#define HWD_DM_PTCH_AD10       0x0b8200a8  /* R/W  DSPM patch address 10       */
#define HWD_DM_PTCH_AD11       0x0b8200ac  /* R/W  DSPM patch address 11       */
#define HWD_DM_PTCH_AD12       0x0b8200b0  /* R/W  DSPM patch address 12       */
#define HWD_DM_PTCH_AD13       0x0b8200b4  /* R/W  DSPM patch address 13       */
#define HWD_DM_PTCH_AD14       0x0b8200b8  /* R/W  DSPM patch address 14       */
#define HWD_DM_PTCH_AD15       0x0b8200bc  /* R/W  DSPM patch address 15       */
#define HWD_DM_PTCH_AD16       0x0b8200c0  /* R/W  DSPM patch address 16       */
#define HWD_DM_PTCH_AD17       0x0b8200c4  /* R/W  DSPM patch address 17       */

#define HWD_DM_PTCH_MTCH0      0x0b8200e0  /* R/W  DSPM patch match address 0  */
#define HWD_DM_PTCH_MTCH1      0x0b8200e4  /* R/W  DSPM patch match address 1  */
#define HWD_DM_PTCH_MTCH2      0x0b8200e8  /* R/W  DSPM patch match address 2  */
#define HWD_DM_PTCH_MTCH3      0x0b8200ec  /* R/W  DSPM patch match address 3  */
#define HWD_DM_PTCH_MTCH4      0x0b8200f0  /* R/W  DSPM patch match address 4  */
#define HWD_DM_PTCH_MTCH5      0x0b8200f4  /* R/W  DSPM patch match address 5  */
#define HWD_DM_PTCH_MTCH6      0x0b8200f8  /* R/W  DSPM patch match address 6  */
#define HWD_DM_PTCH_MTCH7      0x0b8200fc  /* R/W  DSPM patch match address 7  */
#define HWD_DM_PTCH_MTCH8      0x0b820100  /* R/W  DSPM patch match address 8  */
#define HWD_DM_PTCH_MTCH9      0x0b820104  /* R/W  DSPM patch match address 9  */
#define HWD_DM_PTCH_MTCH10     0x0b820108  /* R/W  DSPM patch match address 10 */
#define HWD_DM_PTCH_MTCH11     0x0b82010c  /* R/W  DSPM patch match address 11 */
#define HWD_DM_PTCH_MTCH12     0x0b820110  /* R/W  DSPM patch match address 12 */
#define HWD_DM_PTCH_MTCH13     0x0b820114  /* R/W  DSPM patch match address 13 */
#define HWD_DM_PTCH_MTCH14     0x0b820118  /* R/W  DSPM patch match address 14 */
#define HWD_DM_PTCH_MTCH15     0x0b82011c  /* R/W  DSPM patch match address 15 */
#define HWD_DM_PTCH_MTCH16     0x0b820120  /* R/W  DSPM patch match address 16 */
#define HWD_DM_PTCH_MTCH17     0x0b820124  /* R/W  DSPM patch match address 17 */

/*--------------------------------**
** DSPV patch and match addresses **
**--------------------------------*/

#define HWD_DV_PTCH_AD0        0x0b820144  /* R/W  DSPV patch address 0        */
#define HWD_DV_PTCH_AD1        0x0b820148  /* R/W  DSPV patch address 1        */
#define HWD_DV_PTCH_AD2        0x0b82014c  /* R/W  DSPV patch address 2        */
#define HWD_DV_PTCH_AD3        0x0b820150  /* R/W  DSPV patch address 3        */
#define HWD_DV_PTCH_AD4        0x0b820154  /* R/W  DSPV patch address 4        */
#define HWD_DV_PTCH_AD5        0x0b820158  /* R/W  DSPV patch address 5        */
#define HWD_DV_PTCH_AD6        0x0b82015c  /* R/W  DSPV patch address 6        */
#define HWD_DV_PTCH_AD7        0x0b820160  /* R/W  DSPV patch address 7        */
#define HWD_DV_PTCH_AD8        0x0b820164  /* R/W  DSPV patch address 8        */
#define HWD_DV_PTCH_AD9        0x0b820168  /* R/W  DSPV patch address 9        */
#define HWD_DV_PTCH_AD10       0x0b82016c  /* R/W  DSPV patch address 10       */
#define HWD_DV_PTCH_AD11       0x0b820170  /* R/W  DSPV patch address 11       */
#define HWD_DV_PTCH_AD12       0x0b820174  /* R/W  DSPV patch address 12       */
#define HWD_DV_PTCH_AD13       0x0b820178  /* R/W  DSPV patch address 13       */
#define HWD_DV_PTCH_AD14       0x0b82017c  /* R/W  DSPV patch address 14       */
#define HWD_DV_PTCH_AD15       0x0b820180  /* R/W  DSPV patch address 15       */

#define HWD_DV_PTCH_MTCH0      0x0b8201a4  /* R/W  DSPV patch match address 0  */
#define HWD_DV_PTCH_MTCH1      0x0b8201a8  /* R/W  DSPV patch match address 1  */
#define HWD_DV_PTCH_MTCH2      0x0b8201ac  /* R/W  DSPV patch match address 2  */
#define HWD_DV_PTCH_MTCH3      0x0b8201b0  /* R/W  DSPV patch match address 3  */
#define HWD_DV_PTCH_MTCH4      0x0b8201b4  /* R/W  DSPV patch match address 4  */
#define HWD_DV_PTCH_MTCH5      0x0b8201b8  /* R/W  DSPV patch match address 5  */
#define HWD_DV_PTCH_MTCH6      0x0b8201bc  /* R/W  DSPV patch match address 6  */
#define HWD_DV_PTCH_MTCH7      0x0b8201c0  /* R/W  DSPV patch match address 7  */
#define HWD_DV_PTCH_MTCH8      0x0b8201c4  /* R/W  DSPV patch match address 8  */
#define HWD_DV_PTCH_MTCH9      0x0b8201c8  /* R/W  DSPV patch match address 9  */
#define HWD_DV_PTCH_MTCH10     0x0b8201cc  /* R/W  DSPV patch match address 10 */
#define HWD_DV_PTCH_MTCH11     0x0b8201d0  /* R/W  DSPV patch match address 11 */
#define HWD_DV_PTCH_MTCH12     0x0b8201d4  /* R/W  DSPV patch match address 12 */
#define HWD_DV_PTCH_MTCH13     0x0b8201d8  /* R/W  DSPV patch match address 13 */
#define HWD_DV_PTCH_MTCH14     0x0b8201dc  /* R/W  DSPV patch match address 14 */
#define HWD_DV_PTCH_MTCH15     0x0b8201e0  /* R/W  DSPV patch match address 15 */

#define HWD_DV_PTCH_EN0        0x0b820204  /* R/W  DSPV patch enable (15-0)    */

/*-------------**                 
** Voice Codec **                 
**-------------*/                 

#define HWD_V_MIC_0            0x0b830000  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_1            0x0b830004  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_2            0x0b830008  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_3            0x0b83000c  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_4            0x0b830010  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_5            0x0b830014  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_6            0x0b830018  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_MIC_7            0x0b83001c  /* R/W  Voice MIC buffer                          0x0    */
#define HWD_V_CP_DSPV_SEL      0x0b830020  /* R/W  CP/DSPV master select                     0x0    */
#define HWD_V_MIC_OFFS         0x0b830024  /* R/W  Voice Mic buffer offset                   0x0    */
#define HWD_V_SPKR_OFFS        0x0b830028  /* R/W  Voice speaker buffer offset               0x0    */

#define HWD_V_SMP_MAX          0x0B83002C	 /* R/W  Voice buffer max sample depth             0x0    */
#define HWD_V_SMP_MAX_ACTV     0x0B830030  /* R/W  Active voice buffer max sample depth      0x0    */
#define HWD_V_CTRL             0x0B830034  /* R/W  Voice Interface Control                   0x0    */
#define HWD_V_CTRL_ACTV        0x0B830038  /* R/W  Currently active voice control register value  0x0    */
#define HWD_DAI_CTRL           0x0B83003C  /* R/W  Voice DAI control                         0x0    */
#define HWD_V_DAI_MIC          0x0B830040  /* R/W  Voice MIC data, for sample test mode      0x0    */
#define HWD_RINGER_PROC_CTRL   0x0B830044  /* R/W  Ringer Shared Buffer processor-in-control selection 0x0    */
#define HWD_RINGER_CTRL        0x0B830048  /* R/W  Ringer Control                            0x0    */
#define HWD_RINGER_CLK_PD      0x0B83004C  /* R/W  Ringer interface clock power down         0x0    */
#define HWD_V_DAI_SPKR         0x0B830050  /* R/W  Voice Speaker data, for sample test mode  0x0    */
#define HWD_V_RG_BUF           0x0B830100  /* R/W  Voice/Ringer speaker sample buffer        0xXXXX */

#define HWD_DAI_RX_VOICE       0x0b830060  /* R/W  DAI RX test data                          0x0    */
#define HWD_DAI_TX_VOICE       0x0b830064  /* R    DAI TX test data                          0x0    */

/*--------------------------**
** Ringer Memory Registers  **
**--------------------------*/

#define HWD_RINGER_MEMORY      0x0b410000  /*   W  Ringer Shared Memory (896 words)                 */

/*-----**
** Tx  **                          
**-----*/                      

#define HWD_TX_CP_DSPM_SEL     0x0b850000  /* R/W  Processor Control Bit                     0x0    */
#define HWD_TX_ON0_INT_SETUP_HOLD  0x0b850004  /* R/W  Internal TX ON0 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON0_INT_CFG     0x0b850008  /* R/W  Internal TX ON0 configuration                      0x0    */
#define HWD_TX_ON0_INT_DIN     0x0b85000c  /* R/W  Internal TX ON0 override data                      0x0    */
#define HWD_TX_ON1_SETUP_HOLD  0x0b850010  /* R/W  TX ON1 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON1_CFG         0x0b850014  /* R/W  TX ON1 configuration                      0x0    */
#define HWD_TX_ON1_DIN         0x0b850018  /* R/W  TX ON1 override data                      0x0    */
#define HWD_TX_ON2_SETUP_HOLD  0x0b85001c  /* R/W  TX ON2 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON2_CFG         0x0b850020  /* R/W  TX ON2 configuration                      0x0    */
#define HWD_TX_ON2_DIN         0x0b850024  /* R/W  TX ON2 override data                      0x0    */
#define HWD_TX_ON3_SETUP_HOLD  0x0b850028  /* R/W  TX ON3 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON3_CFG         0x0b85002c  /* R/W  TX ON3 configuration                      0x0    */
#define HWD_TX_ON3_DIN         0x0b850030  /* R/W  TX ON3 override data                      0x0    */
#define HWD_TX_ON4_SETUP_HOLD  0x0b850034  /* R/W  TX ON4 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON4_CFG         0x0b850038  /* R/W  TX ON4 configuration                      0x0    */
#define HWD_TX_ON4_DIN         0x0b85003c  /* R/W  TX ON4 override data                      0x0    */
#define HWD_TX_RFON0_DIN       0x0b850040  /* R/W  Tx_RF_ON0 pending data                    0x0    */
#define HWD_TX_RFON0_CFG       0x0b850044  /* R/W  Tx_RF_ON0 configuration                   0x0    */
#define HWD_TX_RFON1_DIN       0x0b850048  /* R/W  Tx_RF_ON1 pending data                    0x0    */
#define HWD_TX_RFON1_CFG       0x0b85004c  /* R/W  Tx_RF_ON1 configuration                   0x0    */
#define HWD_TX_RFON2_DIN       0x0b850050  /* R/W  Tx_RF_ON2 pending data                    0x0    */
#define HWD_TX_RFON2_CFG       0x0b850054  /* R/W  Tx_RF_ON2 configuration                   0x0    */
#define HWD_TX_RFON3_DIN       0x0b850058  /* R/W  Tx_RF_ON3 pending data                    0x0    */
#define HWD_TX_RFON3_CFG       0x0b85005c  /* R/W  Tx_RF_ON3 configuration                   0x0    */
#define HWD_TX_RFON4_DIN       0x0b850060  /* R/W  Tx_RF_ON4 pending data                    0x0    */
#define HWD_TX_RFON4_CFG       0x0b850064  /* R/W  Tx_RF_ON4 configuration                   0x0    */
#define HWD_TX_RFON5_DIN       0x0b850068  /* R/W  Tx_RF_ON5 pending data                    0x0    */
#define HWD_TX_RFON5_CFG       0x0b85006c  /* R/W  Tx_RF_ON5 configuration                   0x0    */
#define HWD_TX_RFON6_DIN       0x0b850070  /* R/W  Tx_RF_ON6 pending data                    0x0    */
#define HWD_TX_RFON6_CFG       0x0b850074  /* R/W  Tx_RF_ON6 configuration                   0x0    */
#define HWD_TX_RFON7_DIN       0x0b850078  /* R/W  Tx_RF_ON7 pending data                    0x0    */
#define HWD_TX_RFON7_CFG       0x0b85007c  /* R/W  Tx_RF_ON7 configuration                   0x0    */
#define HWD_TX_ON_GPO          0x0b850080  /* R/W  Tx_On GPO control and data                0x0    */
#define HWD_TX_TEST_BAL        0x0b850084  /* R/W  Level of square wave inserted to Tx 
                                                   filter during test mode                   0x0    */
#define HWD_TX_TEST_CTRL       0x0b850088  /* R/W  Test Control, synchronized to the PCG     0x0    */
#define HWD_TX_I_OFFSET_INT    0x0b85008C  /* R/W  Tx DC I offset prior to int MS DAC (cal)  0x0    */
#define HWD_TX_Q_OFFSET_INT    0x0b850090  /* R/W  Tx DC Q offset prior to int MS DAC (cal)  0x0    */
#define HWD_TX_I_OFFSET_EXT    0x0b850094  /* R/W  Tx DC I offset prior to ext MS DAC (cal)  0x0    */
#define HWD_TX_Q_OFFSET_EXT    0x0b85009C  /* R/W  Tx DC Q offset prior to ext MS DAC (cal)  0x0    */
#define HWD_TX_ON0_SETUP_HOLD  0x0b8500A0  /* R/W  TX ON0 setup and hold relative to Tx PCG  0x0    */
#define HWD_TX_ON0_CFG         0x0b8500A4  /* R/W  TX ON0 configuration                      0x0    */
#define HWD_TX_ON0_DIN         0x0b8500A8  /* R/W  TX ON0 override data                      0x0    */
#define HWD_TX_GAIN_COMP1_CP   0x0b8500AC  /* R/W  TX gain compensation value                0x0    */
#define HWD_TX_GAIN_COMP2_CP   0x0b8500B0  /* R/W  TX gain compensation value                0x0    */
#define HWD_TX_SEL_CP          0x0b8500B4  /* R/W  TX gain select                            0x0    */

#define HWD_TX_TEST            0x0b8500b0  /* R/W  Tx Modem Test Mode                        0x0    */
/*---------------**            
** CP Interface  **            
**---------------*/            

#define HWD_CI_CSCFG01          0x0bc00000  /* R/W  CP Chip select config reg0                0x7777 */
#define HWD_CI_CSCFG23          0x0bc00004  /* R/W  CP Chip select config reg1                0xFFFF */
#define HWD_CI_CSCFG45          0x0bc00008  /* R/W  CP Chip select config reg2                0xFFFF */

#define HWD_EMC_AW_SN_RMAP     0x0bc0000c  /* R/W  CP Chip select config reg3                0x00AF */

#define HWD_LCD_WAIT           0x0bc00010  /* R/W  lcd wait state configuration              0x7FFF */           
#define HWD_SYNC_FLASH_CS0     0x0bc00015  /* R/W  CP 1-byte Parms for Sync Flash CS0        0x00 */
#define HWD_MISC_EBI_CFG       0x0bc0001B  /* R/W  CP 1-byte Misc EBI Parms                  0x00 */
#define HWD_EBI_CS             0x0bc0001C  /* R/W  CP 1-byte Spare EBI register              0x00 */
#define HWD_IRAM_MASK          0x0bc00020  /* R/W  CP Mask to generate IRAM write interrupt  0x00 */

/*---------------**            
** System Timing **            
**---------------*/            

#define HWD_ST_CPINT_FR        0x0b860000  /* R/W  Frame source type (20 or 26 ms)           0x?    */
#define HWD_ST_CPINT_CLR       0x0b860004  /* R/W  Clear status register                     0x?    */
#define HWD_ST_CPINT_MASK      0x0b860008  /* R/W  Interrupt Mask Register                   0x?    */
#define HWD_ST_CPINT_SRC       0x0b86000C  /* R    Interrupt Source Register                 0x?    */
#define HWD_ST_CPINT_CNT       0x0b860010  /* R    Subframe count (TBD)                      0x?    */
#if !((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_ST_CNT_LOCK        0x0b860014  /* R/W  System Time Count Lock Register           0x?    */
#define HWD_ST_CNT_LO          0x0b860018  /* R    System Time Count Register (16 LSB)       0x?    */
#define HWD_ST_CNT_HI          0x0b86001C  /* R    System Time Count Register (4 MSB)        0x?    */
#endif

/*--------------------------**
** CP Sleep and Calibration **
**--------------------------*/
#define HWD_CS_SLP_CTRL        0x0b870000  /* R/W  Sleep control registry                      0x0000 */
#define HWD_CS_RESYNC_TIME_LO  0x0b870004  /* R/W  LSB of slp counter compare value for resync 0x0000 */
#define HWD_CS_RESYNC_TIME_HI  0x0b870008  /* R/W  MSB of slp counter compare value for resync 0x0000 */
#define HWD_CS_SLP_TIME_LO     0x0b87000C  /* R/W  LSB of slp counter compare value for wakeup 0xffff */
#define HWD_CS_SLP_TIME_HI     0x0b870010  /* R/W  MSB of slp counter compare value for wakeup 0xffff */
#define HWD_CS_GPINT_CTRL      0x0b870014  /* R/W  Ctrl GPINT polarity                         0x0000 */
#define HWD_CS_EXT_WAKE_EN     0x0b870018  /* R/W  Ctrl for wakeup from deep-sleep enable      0x0000 */
#define HWD_CS_PWR_UP_WAIT     0x0b87001C  /* R/W  Count for OSC/PLL settling time on power up 0x0000 */
#define HWD_CS_CAL_SYSTIME_LO  0x0b870020  /* R/W  16bit LSB of systime on 32K edge            0x0000 */
#define HWD_CS_CAL_SYSTIME_HI  0x0b870024  /* R/W  4bit MSB of systime on 32K edge             0x0000 */
#define HWD_CS_CAL_SLPCNT_LO   0x0b870028  /* R/W  16bit LSB of 32K count on 32K edge          0x0000 */
#define HWD_CS_CAL_SLPCNT_HI   0x0b87002C  /* R/W  4bit MSB of 32K count on 32K edge           0x0000 */
#define HWD_CS_CLK32K_CNT_LO   0x0b870030  /* R/W  16bit LSB of free running on 32K edge       0x0000 */
#define HWD_CS_CLK32K_CNT_HI   0x0b870034  /* R/W  4bit MSB of free running on 32K edge        0x0000 */

/*------------------------**      
** Mixed Signal Interface **      
**------------------------*/      
/* Rx SD */
#define HWD_RXSD_ADCPD         0x0b880000  /* R/W  [0:0], RxSd Modulators Power Down          0x01   */
#define HWD_RXSD_RST           0x0b880004  /* R/W  [0:0], RxSd Modulator Reset               0x00   */
#define HWD_RXSD_RXFSW         0x0b880008  /* R/W  [0:0], RxSd Software Reset                0x00   */
#define HWD_RXSD_TESTMODE      0x0b88000C  /* R/W  [0:0], 0=>normal, 1=>uses test in to RXF  0x00   */
#define HWD_RXSD_CRSW          0x0b880010  /* R/W  [3:0], power consumation control          0x00   */
#define HWD_RXSD_FIRTAPI       0x0b880018  /* R/W  [7:0], RxSd FIR I-ch coefficient          0x54   */
#define HWD_RXSD_FIRTAPQ       0x0b88001C  /* R/W  [7:0], RxSd FIR Q-ch coefficient          0x54   */
#define HWD_RXSD_FIRGCI        0x0b880020  /* R/W  [4:0], RxSd FIR gain compensation I       0x01   */
#define HWD_RXSD_FIRGCQ        0x0b880024  /* R/W  [4:0], RxSd FIR gain compensation Q       0x01   */
#define HWD_RXSD_BITSEL_AMPS   0x0b880028  /* R/W  [3:0], RxSd bit sel from 28 to 14-bit     0x05   */
#define HWD_RXSD_BITSEL_CDMA   0x0b880030  /* R/W  [3:0], RxSd bit selection                 0x05   */
#define HWD_RXSD_GBA1I         0x0b880034  /* R/W  [3:0], RxSd I-ch gain shift value a1      0x0F   */
#define HWD_RXSD_GBA2I         0x0b880038  /* R/W  [3:0], RxSd I-ch gain shift value a2      0x0F   */
#define HWD_RXSD_GBA1Q         0x0b88003C  /* R/W  [3:0], RxSd Q-ch gain shift value a1      0x0F   */
#define HWD_RXSD_GBA2Q         0x0b880040  /* R/W  [3:0], RxSd Q-ch gain shift value a2      0x0F   */
#define HWD_RXSD_RXSMV         0x0b880044  /* R/W  [1:0], RxSd       0x00   */
#define HWD_RXSD_RXLNA_PD      0x0b88004C  /* R/W  1 bit, Rx LNA power down                  0x00   */
#define HWD_RXSD_SEL           0x0b880050  /* R/W  [2:0], Rx analog modulator output (bit 1) 0x00   */
#define HWD_RXSD_REGSEL        0x0b880054  /* R/W  [3:0], RxSd Control processor select      0x00   */
#define HWD_RXSD_ENABLES       0x0b880078  /* R/W  [4:0], RxSd Control register              0x00   */
#define HWD_RXSD_PHASE_EQ      0x0b88007C  /* R/W  [1:0], RxSd En Phase Equalizer for DRC    0x00   */
#define HWD_RXSD_BITSEL_DIG_GI 0x0b880180  /* R/W  [9:0], RxSd Bitsel [9:6], digi gain [5:0] 0x00   */
#define HWD_RXSD_BITSEL_DIG_GQ 0x0b880184  /* R/W  [9:0], RxSd Bitsel [9:6], digi gain [5:0] 0x00   */
#define HWD_RXSD_RST_SW        0x0b880188  /* R/W  [0:0], RxSd Software Reset                0x00   */ 
#define HWD_RXSD_OFFSETI       0x0b88018C  /* R/W  [13:0], RxSd I-ch DC offset               0x00   */
#define HWD_RXSD_OFFSETQ       0x0b880190  /* R/W  [13:0], RxSd Q-ch DC offset               0x00   */
#define HWD_RXSD_AP_IIR2A2I    0x0b8802BC  /* R/W  [7:0], RxSd AP IIR2A2I                    0x61   */
#define HWD_RXSD_AP_IIR2A2Q    0x0b8802C0  /* R/W  [7:0], RxSd AP IIR2A2Q                    0x61   */
#define HWD_RXSD_AP_IIR2A1I    0x0b8802C4  /* R/W  [9:0], RxSd AP IIR2A1I                    0x0247 */
#define HWD_RXSD_AP_IIR2A1Q    0x0b8802C8  /* R/W  [9:0], RxSd AP IIR2A1Q                    0x0247 */
#define HWD_RXSD_GP_RSTN_REG   0x0b8802CC  /* R/W  [3:0], RxSd ROM 4.0 revision enable       0x0F   */
/* Tx SD */
#define HWD_TXSD_OPER          0x0b88010C  /* R/W  [1:0], Tx offset cal. comparator PD       0x01   */
#define HWD_TXSD_CMVSEL        0x0b880118  /* R/W  1 bit, Tx DAC common voltage src sel      0x00   */
#define HWD_TXSD_REGSEL        0x0b880128  /* R/W  [1:0], Tx Shared RegSel                   0x00   */
#define HWD_TXSD_CNTEN         0x0b880140  /* W    1 bit, Tx start txsd tune counter         N/A    */
#define HWD_TXSD_PWRONI        0x0b880144  /* R/W  1 bit, Tx I-ch power on bit               0x00   */
#define HWD_TXSD_PWRONQ        0x0b880148  /* R/W  1 bit, Tx Q-ch power on bit               0x00   */
#define HWD_TXSD_PWRSEL        0x0b88014C  /* R/W  1 bit, Tx SD PCG gating power select      0x00   */
#define HWD_TXSD_GDIQ          0x0b880150  /* R/W  [7:0], Tx SD Trim control of I,Q chnls    0x00   */
#define HWD_TXSD_SCALE         0x0b880154  /* R/W  [3:0], TX Sd gain scale                   0x00   */
#define HWD_TXSD_TUNE          0x0b880158  /* R/W  [5:0], control tuning of RC cutoff freq.  0x00   */
#define HWD_TXSD_TUNECNT       0x0b88015C  /* R/W  [15:0], read back the TxSD tune counter   0x00   */
#define HWD_TXSD_OSC_CTRL      0x0b880160  /* R/W  [2:0], Controls the OSC                   0x00   */
#define HWD_TXSD_GAIN_SEL      0x0b880164  /* R/W  [1:0],  Tx gain select                    0x00   */
/* PDM */
#define HWD_PDM_PWR_DN         0x0b880254  /* R/W  [4:0] PDM power down control              0x1f   */
#define HWD_PDM_TESTSEL        0x0b880258  /* R/W  PDM test mode select                      0x00   */
#define HWD_PDM0_REGSEL        0x0b880240  /* R/W  PDM0 shared regsel: 00=APB, 01=DM, 10=DV  0x00   */
#define HWD_PDM1_REGSEL        0x0b880244  /* R/W  PDM1 shared regsel: 00=APB, 01=DM, 10=DV  0x00   */
#define HWD_PDM2_REGSEL        0x0b880248  /* R/W  PDM2 shared regsel: 00=APB, 01=DM, 10=DV  0x00   */
#define HWD_PDM3_REGSEL        0x0b88024C  /* R/W  PDM3 shared regsel: 00=APB, 01=DM, 10=DV  0x00   */
#define HWD_PDM4_REGSEL        0x0b880250  /* R/W  PDM4 shared regsel: 00=APB, 01=DM, 10=DV  0x00   */
#define HWD_PDM0_DOUT          0x0b88020C  /* R/W  [11:0] PDM0 data out register             0x00   */
#define HWD_PDM1_DOUT          0x0b880210  /* R/W  [11:0] PDM1 data out register             0x00   */
#define HWD_PDM2_DOUT          0x0b880214  /* R/W  [11:0] PDM2 data out register             0x00   */
#define HWD_PDM3_DOUT          0x0b880218  /* R/W  [11:0] PDM3 data out register             0x00   */
#define HWD_PDM4_DOUT          0x0b88021C  /* R/W  [11:0] PDM4 data out register             0x00   */
#define HWD_PDM0_DIN           0x0b880220  /* R/W  [11:0] PDM0 data in register              0x00   */
#define HWD_PDM1_DIN           0x0b880224  /* R/W  [11:0] PDM1 data in register              0x00   */
#define HWD_PDM2_DIN           0x0b880228  /* R/W  [11:0] PDM2 data in register              0x00   */
#define HWD_PDM3_DIN           0x0b88022c  /* R/W  [11:0] PDM3 data in register              0x00   */
#define HWD_PDM4_DIN           0x0b880230  /* R/W  [11:0] PDM4 data in register              0x00   */
/* AUXADC*/
#define HWD_CF_AA_POWER        0x0b880300  /* R/W  Aux ADC power control                     0x0003 */
#define HWD_CF_AA_CHSEL        0x0b880304  /* R/W  Input Channel select                      0x00   */
#define HWD_CF_AA_SEL          0x0b880308  /* R/W  Modulator output select control           0x00   */
#define HWD_CF_AA_DOUT         0x0b88030c  /* R    Aux Adc result, read in AA interrupt      N/A    */
#define HWD_CF_AA_SCALE        0x0b880310  /* R/W  AA Gain Control (2 bits)                  0x00   */
#define HWD_CF_AA_WAIT         0x0b880314  /* R/W  19.2 Clocks before generating interrupt   0x0C00 */
#define HWD_CF_BUFBYPASS       0x0b880318  /* R/W  AuxAdc buffer bypass select               0x0000 */
/* Voice */
#define HWD_CF_VC_ADCPD        0x0b880400  /* R/W  ADC Power down                            0x01   */
#define HWD_CF_VC_DACPOWER     0x0b880404  /* R/W  [2:0] dacamppd2, dacamppd1, dacpd         0x07   */
#define HWD_CF_VC_MICPD        0x0b880408  /* R/W  MIC Power down                            0x01   */
#define HWD_CF_VC_CLKPD        0x0b88040C  /* R/W  Voice Clock power down                     0x00   */
#define HWD_CF_VC_DIGPOWER     0x0b880410  /* R/W  1 bit, voice digital power down           0x01   */
#define HWD_CF_VC_SEL          0x0b880414  /* R/W  DAC output current and ADC input channel control */
#define HWD_CF_VC_ADCPGA       0x0b880418  /* R/W  [2:0] Voice ADC analog gain control       0x00   */
#define HWD_CF_VC_ADCRANGE     0x0b88041C  /* R/W  [1:0] Voice ADC range gain control        0x00   */

#define HWD_CF_VC1_POP         0x0b880424  /* R/W  Pop Control register                      0x00C0 */
#define HWD_CF_VC_LOOPBACKA    0x0b880428  /* R/W  [2:0] Analog loopback select              0x00   */
#define HWD_CF_VC_LOOPBACKD    0x0b88042C  /* R/W  Digital loopback select                   0x00   */
#define HWD_CF_VC_BPA          0x0b880430  /* R/W  [1:0] ADC test configuration bits         0x00   */
#define HWD_CF_VC_BP_SEL       0x0b880434  /* R/W  DAC bypass select                         0x00   */
#if !((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_CF_VC_HPBYPASS     0x0b880438  /* R/W  Voice Codec Filter High-Pass selection    0x00   */
#endif
#define HWD_CF_VC_REGSEL       0x0b880440  /* R/W  Voice shared regsel: 0=>APB, 1=>DV        0x00   */

/* Bandgap */
#define HWD_CF_BG_BIASPD       0x0b880600  /* R/W  Bias PD                                   0x00   */
#define HWD_CF_BG_REFPD        0x0b880604  /* R/W  Bandgap Ref PD                            0x00   */
#define HWD_CF_BG_VTRIM        0x0b880608  /* R/W  [5:0] Bandgap voltage trim                0x00   */

#define HWD_CF_RESERVED        0x0b880444  /* R/W  Reserved register                         0x00   */

/*-------------------**        
** Serial Programmer **        
**-------------------*/        
/* RF SPI */
#define HWD_RFSPI_BASE         0x0b890200
/* CP SPI */
#define HWD_CPSPI_BASE         0x0b890000

#define HWD_RF_SER_GENL        HWD_RFSPI_BASE + 0x00 /* R/W  Serial Device General Information         0x0    */
#define HWD_RF_SER_INTMD       HWD_RFSPI_BASE + 0x04 /* R/W  Serial Device Interrupt Mode              0x0    */
#define HWD_RF_SER_TRIG        HWD_RFSPI_BASE + 0x08 /* R/W  Serial Device Control                     0x0    */
#define HWD_RF_SER1_BC         HWD_RFSPI_BASE + 0x14 /* R/W  Serial Device #1 Bit Count                0x0    */
#define HWD_RF_SER1_CTRL       HWD_RFSPI_BASE + 0x18 /* R/W  Serial Device #1 Control                  0x0    */
#define HWD_RF_SER1_D1         HWD_RFSPI_BASE + 0x1c /* R/W  Serial Device #1 Data, MSB                0x0    */
#define HWD_RF_SER1_D0         HWD_RFSPI_BASE + 0x20 /* R/W  Serial Device #1 Data, LSB                0x0    */
#define HWD_RF_SER1_LE         HWD_RFSPI_BASE + 0x24 /* R/W  Serial Device #1 Level Control            0x0    */
#define HWD_RF_SER0_BC         HWD_RFSPI_BASE + 0x28 /* R/W  Serial Device #1 Bit Count                0x0    */
#define HWD_RF_SER0_CTRL       HWD_RFSPI_BASE + 0x2c /* R/W  Serial Device #0 Control                  0x0    */
#define HWD_RF_SER0_D1         HWD_RFSPI_BASE + 0x30 /* R/W  Serial Device #0 Data, MSB                0x0    */
#define HWD_RF_SER0_D0         HWD_RFSPI_BASE + 0x34 /* R/W  Serial Device #0 Data, LSB                0x0    */
#define HWD_RF_SER0_LE         HWD_RFSPI_BASE + 0x38 /* R/W  Serial Device #0 Level Control            0x0    */
#define HWD_RF_SER_PROC        0x0b890300            /* R/W  Serial Device Processor Ownership         0x0    */

#define HWD_SER_GENL           HWD_CPSPI_BASE + 0x00 /* R/W  Serial Device General Information         0x0    */
#define HWD_SER_INTMD          HWD_CPSPI_BASE + 0x04 /* R/W  Serial Device Interrupt Mode              0x0    */
#define HWD_SER_TRIG           HWD_CPSPI_BASE + 0x08 /* R/W  Serial Device Control                     0x0    */
#define HWD_SER1_BC            HWD_CPSPI_BASE + 0x14 /* R/W  Serial Device #1 Bit Count                0x0    */
#define HWD_SER1_CTRL          HWD_CPSPI_BASE + 0x18 /* R/W  Serial Device #1 Control                  0x0    */
#define HWD_SER1_D1            HWD_CPSPI_BASE + 0x1c /* R/W  Serial Device #1 Data, MSB                0x0    */
#define HWD_SER1_D0            HWD_CPSPI_BASE + 0x20 /* R/W  Serial Device #1 Data, LSB                0x0    */
#define HWD_SER1_LE            HWD_CPSPI_BASE + 0x24 /* R/W  Serial Device #1 Level Control            0x0    */
#define HWD_SER0_BC            HWD_CPSPI_BASE + 0x28 /* R/W  Serial Device #1 Bit Count                0x0    */
#define HWD_SER0_CTRL          HWD_CPSPI_BASE + 0x2c /* R/W  Serial Device #0 Control                  0x0    */
#define HWD_SER0_D1            HWD_CPSPI_BASE + 0x30 /* R/W  Serial Device #0 Data, MSB                0x0    */
#define HWD_SER0_D0            HWD_CPSPI_BASE + 0x34 /* R/W  Serial Device #0 Data, LSB                0x0    */
#define HWD_SER0_LE            HWD_CPSPI_BASE + 0x38 /* R/W  Serial Device #0 Level Control            0x0    */

/*--------**                   
** UARTs  **                   
**--------*/                   

#define HWD_UART_BASE   0x0b8a0000 /* Internal UART Base address */

/*--------------------------------------------------------------------
* UART Port Definitions For The 16550 serial interface
*--------------------------------------------------------------------*/

#define HWD_UART0_BASE  HWD_UART_BASE + 0x0000 /* UART port 0 base */
#define HWD_UART1_BASE  HWD_UART_BASE + 0x0100 /* UART port 1 base */
#define HWD_UART2_BASE  HWD_UART_BASE + 0x0200 /* UART port 2 base */

/*--------------------------------------------------------------------
* Define UART 0 control and status registers
*--------------------------------------------------------------------*/

#define HWD_UART0_RHR  HWD_UART0_BASE + 0x00  /* Rx holding data */
#define HWD_UART0_THR  HWD_UART0_BASE + 0x00  /* Tx holding data */
#define HWD_UART0_IER  HWD_UART0_BASE + 0x04  /* Interrupt enable */
#define HWD_UART0_FCR  HWD_UART0_BASE + 0x08  /* Fifo control */
#define HWD_UART0_ISR  HWD_UART0_BASE + 0x08  /* Interrupt status */
#define HWD_UART0_LCR  HWD_UART0_BASE + 0x0C  /* Line control */
#define HWD_UART0_MCR  HWD_UART0_BASE + 0x10  /* Modem control */
#define HWD_UART0_LSR  HWD_UART0_BASE + 0x14  /* Line status */
#define HWD_UART0_MSR  HWD_UART0_BASE + 0x18  /* Modem status */
#define HWD_UART0_SPR  HWD_UART0_BASE + 0x1C  /* Scratchpad */
#define HWD_UART0_DLL  HWD_UART0_BASE + 0x00  /* LSB divisor latch */
#define HWD_UART0_DLM  HWD_UART0_BASE + 0x04  /* MSB divisor latch */
#define HWD_UART0_SSL  HWD_UART0_BASE + 0x20  /* SpeedSense low register */
#define HWD_UART0_SSM  HWD_UART0_BASE + 0x24  /* SpeedSense med register */
#define HWD_UART0_SSH  HWD_UART0_BASE + 0x28  /* SpeedSense high register */
#define HWD_UART0_SSR  HWD_UART0_BASE + 0x2c  /* Speed Sense Complete Register */
#define HWD_UART0_PSW  HWD_UART0_BASE + 0x34  /* Power Savings word */

#define HWD_UART0_FIFO_DEPTH 64               /* Uart0 fifo depth */

/*--------------------------------------------------------------------
* Define UART 1 control and status registers
*--------------------------------------------------------------------*/

#define HWD_UART1_RHR  HWD_UART1_BASE + 0x00  /* Rx holding data */
#define HWD_UART1_THR  HWD_UART1_BASE + 0x00  /* Tx holding data */
#define HWD_UART1_IER  HWD_UART1_BASE + 0x04  /* Interrupt enable */
#define HWD_UART1_FCR  HWD_UART1_BASE + 0x08  /* Fifo control */
#define HWD_UART1_ISR  HWD_UART1_BASE + 0x08  /* Interrupt status */
#define HWD_UART1_LCR  HWD_UART1_BASE + 0x0C  /* Line control */
#define HWD_UART1_MCR  HWD_UART1_BASE + 0x10  /* Modem control */
#define HWD_UART1_LSR  HWD_UART1_BASE + 0x14  /* Line status */
#define HWD_UART1_MSR  HWD_UART1_BASE + 0x18  /* Modem status */
#define HWD_UART1_SPR  HWD_UART1_BASE + 0x1C  /* Scratchpad */
#define HWD_UART1_DLL  HWD_UART1_BASE + 0x00  /* LSB divisor latch */
#define HWD_UART1_DLM  HWD_UART1_BASE + 0x04  /* MSB divisor latch */
#define HWD_UART1_SSL  HWD_UART1_BASE + 0x20  /* SpeedSense low register */
#define HWD_UART1_SSM  HWD_UART1_BASE + 0x24  /* SpeedSense med register */
#define HWD_UART1_SSH  HWD_UART1_BASE + 0x28  /* SpeedSense high register */
#define HWD_UART1_SSR  HWD_UART1_BASE + 0x2c  /* Speed Sense Complete Register */
#define HWD_UART1_PSW  HWD_UART1_BASE + 0x34  /* Power Savings word */

#define HWD_UART1_FIFO_DEPTH 64               /* Uart1 fifo depth */

/*--------------------------------------------------------------------
* Define UART 2 control and status registers
*--------------------------------------------------------------------*/

#define HWD_UART2_RHR  HWD_UART2_BASE + 0x00  /* Rx holding data */
#define HWD_UART2_THR  HWD_UART2_BASE + 0x00  /* Tx holding data */
#define HWD_UART2_IER  HWD_UART2_BASE + 0x04  /* Interrupt enable */
#define HWD_UART2_FCR  HWD_UART2_BASE + 0x08  /* Fifo control */
#define HWD_UART2_ISR  HWD_UART2_BASE + 0x08  /* Interrupt status */
#define HWD_UART2_LCR  HWD_UART2_BASE + 0x0C  /* Line control */
#define HWD_UART2_MCR  HWD_UART2_BASE + 0x10  /* Modem control */
#define HWD_UART2_LSR  HWD_UART2_BASE + 0x14  /* Line status */
#define HWD_UART2_MSR  HWD_UART2_BASE + 0x18  /* Modem status */
#define HWD_UART2_SPR  HWD_UART2_BASE + 0x1C  /* Scratchpad */
#define HWD_UART2_DLL  HWD_UART2_BASE + 0x00  /* LSB divisor latch */
#define HWD_UART2_DLM  HWD_UART2_BASE + 0x04  /* MSB divisor latch */
#define HWD_UART2_SSL  HWD_UART2_BASE + 0x20  /* SpeedSense low register */
#define HWD_UART2_SSM  HWD_UART2_BASE + 0x24  /* SpeedSense med register */
#define HWD_UART2_SSH  HWD_UART2_BASE + 0x28  /* SpeedSense high register */
#define HWD_UART2_SSR  HWD_UART2_BASE + 0x2c  /* Speed Sense Complete Register */
#define HWD_UART2_PSW  HWD_UART2_BASE + 0x34  /* Power Savings word */

#define HWD_UART2_FIFO_DEPTH 64               /* Uart2 fifo depth */

/*----------**                 
** MMI      **                 
**----------*/                 
#define HWD_KPD_DBT            0x0b8b0000  /* R/W  Keypad Debounce Time                      0x0000 */
#define HWD_KPD_STATE          0x0b8b0020  /* R/W  Keypad State machine state                0x0000 */
#define HWD_KPD_TST            0x0b8b0028  /* R/W  Keypad test                               0x0000 */
#define HWD_KPD_COL0           0x0b8b0078  /* R    Keypad column 0 state                     0x0000 */
#define HWD_KPD_COL1           0x0b8b007c  /* R    Keypad column 1 state                     0x0000 */
#define HWD_KPD_COL2           0x0b8b0080  /* R    Keypad column 2 state                     0x0000 */
#define HWD_KPD_COL3           0x0b8b0084  /* R    Keypad column 3 state                     0x0000 */
#define HWD_KPD_COL4           0x0b8b0088  /* R    Keypad column 4 state                     0x0000 */
#define HWD_KPD_CTRL           0x0b8b008c  /* R/W  Keypad control                            0x0000 */
#define HWD_KPD_SCAN_MAX       0x0b8b00a8  /* R/W  scan duration for each keypad column      0x01ff */
/* PWMs */
#define HWD_KPD_BLPWM          0x0b8b0004  /* R/W  Keypad Back Light Mode                    0x0    */
#define HWD_LCD_CTRST          0x0b8b000c  /* R/W  Display Contrast Mode                     0x0    */
#define HWD_LCD_BLPWM          0x0b8b0010  /* R/W  Display Back Light Mode                   0x0    */
#if !((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_PWM_BYP            0x0b8b00a0  /* R/W  PWMs bypass control                       0x0    */
#endif

#define HWD_PWR_OFF_CTRL       0x0b8b0038  /* R/W  Power off control                         0x2    */

#define HWD_RNGR_CTRL_A        0x0b8b0018  /* R/W  Ringer control                            0x0    */
#define HWD_RNGR_CMPF_A        0x0b8b001C  /* R/W  Ringer compare for frequency              0x0    */
#define HWD_RNGR_CMPV_A        0x0b8b0024  /* R/W  Ringer compare for volume                 0x0    */
#define HWD_RNGR_CMPD_A        0x0b8b0050  /* R/W  Ringer duration                           0x0    */
#define HWD_RNGR_STAT          0x0b8b0054  /* R    Ringers status                            0x0    */

/*----------**                 
** PLL      **                 
**----------*/                 

#define HWD_PLL_LOCK_PLS       0x0b8b0030  /* R/W  PLL lock pulse threshold                  0x0    */
#define HWD_PLL_LOCK_CTRL      0x0b8b0034  /* R/W  PLL lock control                          0x0    */

/*------------------------**                 
** AMPS Related Registers **                 
**------------------------*/ 

#define HWD_RXD_FSKDATA1       0x0b8c8000  /* R   16 MSB Rx Data register                    ---    */
#define HWD_RXD_FSKDATA2       0x0b8c8004  /* R   12 LSB Rx Data register                    ---    */
#define HWD_RXD_ERR_STAT       0x0b8c8008  /* R   Error status indicator register            ---    */
#define HWD_RXD_DOTTING        0x0b8c800c  /* R   Dotting indicator status register          ---    */
#define HWD_RXD_WSYNC          0x0b8c8010  /* R   Word Sync indicator status register        ---    */
#define HWD_RXD_BI_STAT        0x0b8c8014  /* R   Busy/Idle status register                  ---    */
#define HWD_RXD_FSKEN          0x0b8c8018  /* R/W FSK enable register                        0x0    */
#define HWD_RXD_CNTL           0x0b8c801c  /* R/W Rx Data control register                   0x0    */
#define HWD_AMPS_MODE_SEL      0x0b8c8020  /* R/W AMPS Mode Select                           0x0    */
#if (SYS_ASIC == SA_RAM)
#define HWD_SPARE_REG          0x0b8c8024  /* R/W 4 bit CP spare register                           */
#endif

/*------------------**
** I2C Interface    **
**------------------*/

#define HWD_EE_DEV_ADR_RW      0x0b8d0000  /* R/W  Device address and read_write bit         0x0    */
#define HWD_EE_LOC_ADR_DAT     0x0b8d0004  /* R/W  Local address MSB or Write Data byte      0x0    */
#define HWD_EE_LOC_ADR1        0x0b8d0008  /* R/W  Local address LSB                         0x0    */
#define HWD_EE_CTL             0x0b8d000c  /* R/W  Mode select control bits                  0x0    */
#define HWD_EE_GO              0x0b8d0010  /* W    Write transaction start pulse             0x0    */
#define HWD_EE_STATUS          0x0b8d0014  /* R    Interface Status register                 0x0    */
#define HWD_EE_RDATA           0x0b8d0018  /* R    Read data from EEPROM                     0x0    */
#define HWD_EE_RD_ACK          0x0b8d001c  /* R/W  Acknowledge for Read data from EEPROM     0x0    */
#define HWD_EE_CPDONE          0x0b8d0020  /* W    CP Response to EEPROM interface interrupt 0x0    */
#define HWD_EE_SCL_CNT_MAX     0x0b8d0024  /* R/W                                            0xd    */
#define HWD_EE_WBCNT           0x0b8d0028  /* R/W  Byte cnt - 1, used when EE_CTL[BW_PW]=0   0x0    */

/*-----------------**          
** Vocoder Mailbox **          
**-----------------*/          
#define HWD_MBV_C2V_RRDY       0x0b800400  /*   W  CP->Vocoder Mailbox Read Ready */
#define HWD_MBV_C2V_WRDY       0x0b800404  /*   W  CP->Vocoder Mailbox Write Ready */
#define HWD_MBV_C2V_ARRDY      0x0b800418  /*   W  CP->Vocoder Fast Mailbox Read Ready */
#define HWD_MBV_C2V_AWRDY      0x0b80041c  /*   W  CP->Vocoder Fast Mailbox Write Ready */

#define HWD_MBV_DAT_TX_BASE    0x0bb00000  /* R/W  CP -> DSPV Mailbox Buffer (256 x 16)      -      */
#define HWD_MBV_DAT_RX_BASE    0x0bb00400  /* R/W  DSPV -> CP Mailbox Buffer (256 x 16)      -      */

#define HWD_MBV_FIQ_DAT_RX_BASE 0x0bb00800  /* R/W  DSPV -> CP Fast Mailbox Buffer (8 x 16)  -      */
#define HWD_MBV_FIQ_DAT_TX_BASE 0x0bb00810  /* R/W  CP -> DSPV Fast Mailbox Buffer (8 x 16)  -      */

/*-----------------**          
** Mode Config     **          
**-----------------*/          
#define HWD_MC_MODECFG_1       0x0b8e0000  /* ModeCfg1 */
#define HWD_MC_MODECFG_2       0x0b8e0004  /* ModeCfg2 */
#define HWD_MC_TESTBUSEN       0x0b8e0008  /* TestBusEn */
#define HWD_MC_MXSIGCFG        0x0b8e000c  /* MxSigCfg */

/*-----------------**          
** CP GPIO         **          
**-----------------*/          

#define HWD_GPIO_0_A           0x0b8f0000  /* R/W  CP GPIO 0                                 0x0    */
#define HWD_GPIO_1_A           0x0b8f0004  /* R/W  CP GPIO 1                                 0x0    */
#define HWD_GPIO_2_A           0x0b8f0008  /* R/W  CP GPIO 2                                 0x0    */
#define HWD_GPIO_3_A           0x0b8f000c  /* R/W  CP GPIO 3                                 0x0    */
#define HWD_GPIO_4_A           0x0b8f0010  /* R/W  CP GPIO 4                                 0x0    */
#define HWD_GPIO_5_A           0x0b8f0014  /* R/W  CP GPIO 5                                 0x0    */
#define HWD_GPIO_6_A           0x0b8f0018  /* R/W  CP GPIO 6                                 0x0    */
#define HWD_GPIO_7_A           0x0b8f001c  /* R/W  CP GPIO 7                                 0x0    */
#define HWD_GPIO_8_A           0x0b8f0020  /* R/W  CP GPIO 8                                 0x0    */
#define HWD_GPIO_9_A           0x0b8f0024  /* R/W  CP GPIO 9                                 0x0    */
#define HWD_GPIO_10_A          0x0b8f0028  /* R/W  CP GPIO 10                                0x0    */
#define HWD_GPIO_11_A          0x0b8f002c  /* R/W  CP GPIO 11                                0x0    */
#define HWD_GPIO_12_A          0x0b8f0030  /* R/W  CP GPIO 12                                0x0    */
#define HWD_GPIO_13_A          0x0b8f0034  /* R/W  CP GPIO 13                                0x0    */
#define HWD_GPIO_14_A          0x0b8f0038  /* R/W  CP GPIO 14                                0x0    */
#define HWD_GPIO_15_A          0x0b8f003c  /* R/W  CP GPIO 15                                0x0    */
#define HWD_GPIO_16_A          0x0b8f0040  /* R/W  CP GPIO 16                                0x0    */
#define HWD_GPIO_17_A          0x0b8f0044  /* R/W  CP GPIO 17                                0x0    */
#define HWD_GPIO_18_A          0x0b8f0048  /* R/W  CP GPIO 18                                0x0    */
#define HWD_GPIO_19_A          0x0b8f004c  /* R/W  CP GPIO 19                                0x0    */
#define HWD_GPIO_20_A          0x0b8f0050  /* R/W  CP GPIO 20                                0x0    */
#define HWD_GPIO_21_A          0x0b8f0054  /* R/W  CP GPIO 21                                0x0    */
#define HWD_GPIO_22_A          0x0b8f0058  /* R/W  CP GPIO 22                                0x0    */
#define HWD_GPIO_23_A          0x0b8f005c  /* R/W  CP GPIO 23                                0x0    */
#define HWD_GPIO_24_A          0x0b8f0060  /* R/W  CP GPIO 24                                0x0    */
#define HWD_GPIO_25_A          0x0b8f0064  /* R/W  CP GPIO 25                                0x0    */
#define HWD_GPIO_26_A          0x0b8f0068  /* R/W  CP GPIO 26                                0x0    */
#define HWD_GPIO_27_A          0x0b8f006c  /* R/W  CP GPIO 27                                0x0    */
#define HWD_DIR0               0x0b8f0070  /* R/W  CP GPIO direction control for GPIO[15:0]  0xffff */
#define HWD_DIR1               0x0b8f0074  /* R/W  CP GPIO direction control for GPIO[31:16] 0xffff */
#define HWD_NM_GP0             0x0b8f0078  /* R/W  CP GPIO normal function for GPIO[15:0]    0x0    */
#define HWD_NM_GP1             0x0b8f007c  /* R/W  CP GPIO normal function for GPIO[31:16]   0x0    */
#define HWD_UART_DSPICE        0x0b8f0080  /* R/W  UART function / DSP ICE function          0x0    */
#define HWD_MBIST              0x0b8f0084  /* R/W  Memory BIST select for BIST only          0x0    */
#define HWD_TST_BUS_EN         0x0b8f0088  /* R/W  Test bus enable                           0x0    */
#define HWD_GPIO_28_A          0x0b8f008c  /* R/W  CP GPIO 28                                0x0    */
#define HWD_GPIO_29_A          0x0b8f0090  /* R/W  CP GPIO 29                                0x0    */
#define HWD_GPIO_30_A          0x0b8f0094  /* R/W  CP GPIO 30                                0x0    */
#define HWD_GPIO_31_A          0x0b8f0098  /* R/W  CP GPIO 31                                0x0    */
#define HWD_GPIO_32_A          0x0b8f009c  /* R/W  CP GPIO 32                                0x0    */
#define HWD_GPIO_33_A          0x0b8f00a0  /* R/W  CP GPIO 33                                0x0    */
#define HWD_GPIO_34_A          0x0b8f00a4  /* R/W  CP GPIO 34                                0x0    */
#define HWD_GPIO_35_A          0x0b8f00a8  /* R/W  CP GPIO 35                                0x0    */
#define HWD_GPIO_36_A          0x0b8f00ac  /* R/W  CP GPIO 35                                0x0    */
#define HWD_GPIO_37_A          0x0b8f00b0  /* R/W  CP GPIO 37                                0x0    */
#define HWD_GPIO_38_A          0x0b8f00b4  /* R/W  CP GPIO 38                                0x0    */
#define HWD_GPIO_39_A          0x0b8f00b8  /* R/W  CP GPIO 39                                0x0    */
#define HWD_GPIO_40_A          0x0b8f00bc  /* R/W  CP GPIO 40                                0x0    */
#define HWD_GPIO_41_A          0x0b8f00c0  /* R/W  CP GPIO 41                                0x0    */
#define HWD_GPIO_42_A          0x0b8f00c4  /* R/W  CP GPIO 42                                0x0    */
#define HWD_GPIO_43_A          0x0b8f00c8  /* R/W  CP GPIO 43                                0x0    */
#define HWD_GPIO_44_A          0x0b8f00cc  /* R/W  CP GPIO 44                                0x0    */
#define HWD_GPIO_45_A          0x0b8f00d0  /* R/W  CP GPIO 45                                0x0    */
#define HWD_GPIO_46_A          0x0b8f00d4  /* R/W  CP GPIO 46                                0x0    */
#define HWD_GPIO_47_A          0x0b8f00d8  /* R/W  CP GPIO 47                                0x0    */
#define HWD_DIR2               0x0b8f00dc  /* R/W  CP GPIO direction control for GPIO[47:32] 0x0fff */
#define HWD_NM_GP2             0x0b8f00e0  /* R/W  CP GPIO normal function for GPIO[47:32]   0x0    */
#define HWD_GPIO_8BIT          0x0b8f0100  /* R/W  CP Group of 8 GPIOs                       0x0    */
#define HWD_GPIO_7BIT          0x0b8f0104  /* R/W  CP Group of 7 GPIOs                       0x0    */
#define HWD_GPIO_DIR_8BIT      0x0b8f0108  /* R/W  CP Dir for 8-bit group                    0x00ff */
#define HWD_GPIO_DIR_7BIT      0x0b8f010C  /* R/W  CP Dir for 7-bit group                    0x007f */
#define HWD_NM_GP_8BIT         0x0b8f0110  /* R/W  CP Function Control for group of 8 GPIOs  0x0    */
#define HWD_NM_GP_7BIT         0x0b8f0114  /* R/W  CP Function Control for group of 7 GPIOs  0x0    */
#define HWD_GPOUT_5BIT         0x0b8f0118  /* R/W  CP Group of 5 GPO only                    0x0    */
#define HWD_GPOUT_DIR_5BIT     0x0b8f011C  /* R/W  CP Function mode, no dir                  0x001f */
#define HWD_GPIO_SB_STATUS     0x0b8f0120  /* R    CP 9-bit inputs/dir GPIO 7/8 bits         0000000111000xxx    */
#define HWD_INT_MSK_REG        0x0b8f0124  /* R/W  Interrupt Mask Register                   0x0000 */
#define HWD_IN_POL_REG         0x0b8f0128  /* R/W  Input interrupt polarity register         0x0000 */
#define HWD_DSPMV_GPIO_DIR     0x0b8f012C  /* R/W  9-bit direction of DSPM/V GPIOs           0x01FF */
#define HWD_GPOUT_8BIT         0x0b8f0130  /* R    CP Read gpout_8bit register               0x0000 */
#define HWD_GPOUT_7BIT         0x0b8f0134  /* R    CP Read gpout_7bit register               0x0000 */
#define HWD_INT_STUS_REG       0x0b8f0138  /* R    Read GPIO Interrupt Register              0xXXXX */
#define HWD_NM_GP_DSPMV        0x0b8f013C  /* R/W  Normal/Functional for DSPM/V              0x0000 */

/*-----------------**          
** IRAM            **          
**-----------------*/          

#define HWD_IRAM_BASE_ADDR       0x0b900000  /* IRAM start address */
#define HWD_IRAM_BASE_ADDR_REMAP 0x00000000  /* IRAM start address */
#define HWD_IRAM_SIZE            0x00006000  /* IRAM size in bytes */
 
 /* Li Wang Added */
#define HWD_IRAM_END_ADDR        (HWD_IRAM_BASE_ADDR + HWD_IRAM_SIZE)
#define HWD_IRAM_RESET_FLAG_ADDR          (HWD_IRAM_END_ADDR - 2)
#define HWD_IRAM_CALIBRATION_FLAG_ADDR    (HWD_IRAM_END_ADDR - 4)
/*#define HWD_IRAM_CPTOBOOT_JUMP_FLAG_ADDR  (HWD_IRAM_END_ADDR - 8)*/
#define HWD_IRAM_APPLICATION_BUFF_ADDR    (HWD_IRAM_END_ADDR - 42)
#define HWD_IRAM_POWER_UP_FLAG_ADDR       (HWD_IRAM_END_ADDR - 44)

/*Flag for HWD_IRAM_POWER_UP_FLAG_ADDR*/
#define POWER_UP_BY_POWER_KEY       (uint16)(0x0001)
#define POWER_UP_BY_USB_CABLE       (uint16)(0x0004)
#define POWER_UP_BY_CHARGER_CABLE   (uint16)(0x0010)
 
#define HWD_IRAM_APPLICATION_BUFFER_SIZE   34 
 /*
 * low iram add -->  boot code , RW , ZI
 *                   power on flag  2 bytes
 *                   iram buffer for application  32 bytes
 *                     
 *                   hot flag (cp to boot ) 4 bytes
 *                   
 *                   calibrate flag = 2 bytes
 * high iram add --> hot flag = 2 bytes
 */

 /* for phone reset, skip PIN/poweron animation & ringer if phone reset by watchdog */
 #define HWD_IRAM_RESET_FLAG      0x5AA5      /* Flag to distinguish poweron and reset */
 
 /* for calibration mode, UI is dummy task in calibration mode */
 #define HWD_IRAM_NORMALSTART     0x1234
 #define HWD_IRAM_CALIBRATION     0x7868 
/*#define HWD_IRAM_CPTOBOOT_FLAG   0x72798466*/
/*------**                     
** Test **                     
**------*/                     

#if (SYS_BOARD == SB_CDS4)

#define HWD_TEST_BASE          0x03000000   /* Test regs base address CS3 */

#define HWD_SER_LOAD4          HWD_TEST_BASE + 0x00  /* W    Serial load 4                   0x0    */
#define HWD_SER_LOAD5          HWD_TEST_BASE + 0x04  /* W    Serial load 5                   0x0    */
#define HWD_PWR_OFF_N          HWD_TEST_BASE + 0x08  /* W    Power off line                  0x0    */
#define HWD_SW_CFG             HWD_TEST_BASE + 0x0C  /* R    DIP switch for SW configuration 0x0    */
#define HWD_LED_CTRL           HWD_TEST_BASE + 0x10  /* W    LED  control                    0x0    */
#define HWD_LCD_DAT            HWD_TEST_BASE + 0x14  /* R/W  Display Port                    0x0    */
#define HWD_CDMA_ON            HWD_TEST_BASE + 0x18  /* W    CDMA test signal on             0x0    */
#define HWD_FLASH_STAT         HWD_TEST_BASE + 0x1c  /* R    Flash status bit                0x0    */
#endif

/*-----------------**          
** Flash Memory    **          
**-----------------*/          

#define HWD_FLASH_BASE_ADDRESS  0x07000000   /* Flash base address CS7 */   
#define HWD_OFFLINE_VALUE       0x59595959   /* Pattern indicating CP to Offline mode */
 #define HWD_CONTROL_VALUE       0xc4c4c4c4   /* Pattern indicating CP to Boot loader mode transition */



/*-----------------------------------*/
/*	USB Registers and bitfields defs */
/*-----------------------------------*/

#define	HWD_USB_BASE	0x0B400000	/* base register addr for USB */

	/* register addresses */

#define	HWD_USB_MODSTAT_ADDR          (HWD_USB_BASE + 0x7000)
#define	HWD_USB_EP06INTSTAT_ADDR      (HWD_USB_BASE + 0x7004)
#define	HWD_USB_EP06INTMASK_ADDR      (HWD_USB_BASE + 0x7008)
#define	HWD_USB_EP06INTMASKEVEN_ADDR  (HWD_USB_BASE + 0x7008)
#define	HWD_USB_EP06INTMASKODD_ADDR   (HWD_USB_BASE + 0x7020)
#define	HWD_USB_MISCINTSTAT_ADDR      (HWD_USB_BASE + 0x700C)
#define	HWD_USB_MISCINTMASK_ADDR      (HWD_USB_BASE + 0x7010)
#define	HWD_USB_CFGDATA_ADDR          (HWD_USB_BASE + 0x7014)
#define	HWD_USB_SOFTMSTAMP_ADDR       (HWD_USB_BASE + 0x7018)
#define	HWD_USB_APPCTRL_ADDR          (HWD_USB_BASE + 0x701C)
#define	HWD_USB_INTERFACE_ADDR        (HWD_USB_BASE + 0x7030)
#define	HWD_USB_DEBUG_ADDR            (HWD_USB_BASE + 0x7034)
#define	HWD_USB_12MHZCOUNTER_ADDR     (HWD_USB_BASE + 0x7038)
	
#define	HWD_USB_EP0DLENGTH_ADDR       (HWD_USB_BASE + 0x0000)
#define	HWD_USB_EP0BUFCMD_ADDR        (HWD_USB_BASE + 0x0008)
#define	HWD_USB_EP0BUFSTAT_ADDR       (HWD_USB_BASE + 0x000C)
#define	HWD_USB_EP0BUFDATA_ADDR       (HWD_USB_BASE + 0x0800)
#define	HWD_USB_EP0BUFSIZE            0x3F	/* max HW is 64 bytes */
	
#define	HWD_USB_EP1RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x1000)
#define	HWD_USB_EP1TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x1004)
#define	HWD_USB_EP1BUFCMD_ADDR        (HWD_USB_BASE + 0x1008)
#define	HWD_USB_EP1BUFSTAT_ADDR       (HWD_USB_BASE + 0x100C)
#define	HWD_USB_EP1BUFDATA_ADDR       (HWD_USB_BASE + 0x1800)
#define	HWD_USB_EP1BUFSIZE            0x600	/* max HW size */

#define	HWD_USB_EP2RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x2000)
#define	HWD_USB_EP2TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x2004)
#define	HWD_USB_EP2BUFCMD_ADDR        (HWD_USB_BASE + 0x2008)
#define	HWD_USB_EP2BUFSTAT_ADDR       (HWD_USB_BASE + 0x200C)
#define	HWD_USB_EP2BUFDATA_ADDR       (HWD_USB_BASE + 0x2800)
#define	HWD_USB_EP2BUFSIZE            0x600

#define	HWD_USB_EP3RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x3000)
#define	HWD_USB_EP3TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x3004)
#define	HWD_USB_EP3BUFCMD_ADDR        (HWD_USB_BASE + 0x3008)
#define	HWD_USB_EP3BUFSTAT_ADDR       (HWD_USB_BASE + 0x300C)
#define	HWD_USB_EP3BUFDATA_ADDR       (HWD_USB_BASE + 0x3800)
#define	HWD_USB_EP3BUFSIZE            0x600
	
#define	HWD_USB_EP4RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x4000)
#define	HWD_USB_EP4TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x4004)
#define	HWD_USB_EP4BUFCMD_ADDR        (HWD_USB_BASE + 0x4008)
#define	HWD_USB_EP4BUFSTAT_ADDR       (HWD_USB_BASE + 0x400C)
#define	HWD_USB_EP4BUFDATA_ADDR       (HWD_USB_BASE + 0x4800)
#define	HWD_USB_EP4BUFSIZE            0x600
	
#define	HWD_USB_EP5RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x5000)
#define	HWD_USB_EP5TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x5004)
#define	HWD_USB_EP5BUFCMD_ADDR        (HWD_USB_BASE + 0x5008)
#define	HWD_USB_EP5BUFSTAT_ADDR       (HWD_USB_BASE + 0x500C)
#define	HWD_USB_EP5BUFDATA_ADDR       (HWD_USB_BASE + 0x5800)
#define	HWD_USB_EP5BUFSIZE            0x600
	
#define	HWD_USB_EP6RXBUFCTRL_ADDR     (HWD_USB_BASE + 0x6000)
#define	HWD_USB_EP6TXBUFCTRL_ADDR     (HWD_USB_BASE + 0x6004)
#define	HWD_USB_EP6BUFCMD_ADDR        (HWD_USB_BASE + 0x6008)
#define	HWD_USB_EP6BUFSTAT_ADDR       (HWD_USB_BASE + 0x600C)
#define	HWD_USB_EP6BUFDATA_ADDR       (HWD_USB_BASE + 0x6800)
#define	HWD_USB_EP6BUFSIZE            0x600

/*
 *	register fields:
 *
 *	each register has 1 or more bitfields; the following definitions
 *	describe each fields as its size (in bits, hi byte) and its origin
 *	(in bits, from bit0).
 *
 *	For example, the Configuration Number bitfield is defined as  
 *		HWD_USB_MODSTAT_CFGNUM			0x0204
 *	where
 *		_MODSTAT_ refers to the register the field belongs to
 *		0x02 is the number of bits in the field
 *		0x04 is the position of the field's LSB in the register
 *
 *	This structure greatly simplifies and reduces the number of
 *	macro used to manipulate bitfields within registers.
 *	Bitfield macros are in hwdusbdev.c.
 *
 *	Assembly macros are defined, and can be used to create bitfield
 *	definitions; for example
 *		HWD_USB_MODSTAT_CFGNUM		0x0204
 *	could be rewritten using macros as
 *		HWD_USB_MODSTAT_CFGNUM		HWD_USB_FIELD( 2,4 )
*/

/* field assembly macro */
#define	HWD_USB_FIELD( Size, Start )	\
	( ((uint8)(Size) & 0xFF) << 8 | ((uint8)(Start) & 0xFF) )

/* get field size in bits from defined value */
#define	HWD_USB_FIELD_SIZE( Field )		( ((uint16)(Field) & 0xFF00) >> 8 )

/* get field start in bits from defined value */
#define	HWD_USB_FIELD_START( Field )	( (uint16)(Field) & 0x00FF )

/* HWD_USB_MODSTAT_ADDR bits */
#define	HWD_USB_MODSTAT_CFGLOCK			0x0100
#define	HWD_USB_MODSTAT_WAKEUP			0x0108
#define	HWD_USB_MODSTAT_EP0STLD			0x010C
#define	HWD_USB_MODSTAT_EP1STLD			0x010D
#define	HWD_USB_MODSTAT_EP2STLD			0x010E
#define	HWD_USB_MODSTAT_EP3STLD			0x010F
#define	HWD_USB_MODSTAT_EP4STLD			0x0110
#define	HWD_USB_MODSTAT_EP5STLD			0x0111
#define	HWD_USB_MODSTAT_EP6STLD			0x0112

/* HWD_USB_EP06INTSTAT_ADDR and HWD_USB_EP06INTMASK_ADDR bits */
#define	HWD_USB_EP6_TxPE		0x011B
#define	HWD_USB_EP6_TxTE		0x011A
#define	HWD_USB_EP6_RxPE		0x0119
#define	HWD_USB_EP6_RxTF		0x0118
#define	HWD_USB_EP6_TX_INT		0x0218	/* all TX ints */
#define	HWD_USB_EP6_RX_INT		0x021A	/* all RX ints */
#define	HWD_USB_EP6_INT			0x0418	/* all EP6 ints */

#define	HWD_USB_EP5_TxPE		0x0117
#define	HWD_USB_EP5_TxTE		0x0116
#define	HWD_USB_EP5_RxPE		0x0115
#define	HWD_USB_EP5_RxTF		0x0114
#define	HWD_USB_EP5_TX_INT		0x0214	
#define	HWD_USB_EP5_RX_INT		0x0216	
#define	HWD_USB_EP5_INT	   		0x0414

#define	HWD_USB_EP4_TxPE		0x0113
#define	HWD_USB_EP4_TxTE		0x0112
#define	HWD_USB_EP4_RxPE		0x0111
#define	HWD_USB_EP4_RxTF		0x0110
#define	HWD_USB_EP4_TX_INT		0x0210	
#define	HWD_USB_EP4_RX_INT		0x0212	
#define	HWD_USB_EP4_INT			0x0410

#define	HWD_USB_EP3_TxPE		0x010F
#define	HWD_USB_EP3_TxTE		0x010E
#define	HWD_USB_EP3_RxPE		0x010D
#define	HWD_USB_EP3_RxTF		0x010C
#define	HWD_USB_EP3_TX_INT		0x020C
#define	HWD_USB_EP3_RX_INT		0x020E
#define	HWD_USB_EP3_INT			0x040C

#define	HWD_USB_EP2_TxPE		0x010B
#define	HWD_USB_EP2_TxTE		0x010A
#define	HWD_USB_EP2_RxPE		0x0109
#define	HWD_USB_EP2_RxTF		0x0108
#define	HWD_USB_EP2_TX_INT		0x0208
#define	HWD_USB_EP2_RX_INT		0x020A
#define	HWD_USB_EP2_INT			0x0408
								 
#define	HWD_USB_EP1_TxPE		0x0107
#define	HWD_USB_EP1_TxTE		0x0106
#define	HWD_USB_EP1_RxPE		0x0105
#define	HWD_USB_EP1_RxTF		0x0104
#define	HWD_USB_EP1_TX_INT		0x0206
#define	HWD_USB_EP1_RX_INT		0x0204
#define	HWD_USB_EP1_INT			0x0404

#define	HWD_USB_EP0_FIN			0x0103
#define	HWD_USB_EP0_TxPE		0x0102
#define	HWD_USB_EP0_RxPE		0x0101
#define	HWD_USB_EP0_SVAL		0x0100

/* HWD_USB_MISCINTSTAT_ADDR and HWD_USB_MISCINTMASK_ADDR bits */
#define	HWD_USB_MISC_SOF	0x0115
#define	HWD_USB_MISC_RESET	0x0114
#define	HWD_USB_MISC_SUSP	0x0113
#define	HWD_USB_MISC_RESUME	0x0112
#define	HWD_USB_MISC_DEVF	0x0111
#define	HWD_USB_MISC_SETI	0x0109
#define	HWD_USB_MISC_SETC	0x0108
#define	HWD_USB_MISC_EP6F	0x0106
#define	HWD_USB_MISC_EP5F	0x0105
#define	HWD_USB_MISC_EP4F	0x0104
#define	HWD_USB_MISC_EP3F	0x0103
#define	HWD_USB_MISC_EP2F	0x0102
#define	HWD_USB_MISC_EP1F	0x0101
#define	HWD_USB_MISC_EP0F	0x0100

/* HWD_USB_SOFTMSTAMP_ADDR bits */
#define	HWD_USB_SOFTMSTAMP_TS	0x0B00

/* HWD_USB_EP0DLENGTH_ADDR bits */
#define	HWD_USB_EP0DLENGTH_LEN	0x1000

/* HWD_USB_EP0BUFCMD bits */
#define	HWD_USB_EP0BUFCMD_FLUSH	0x0104	/* flush EP0DATA */
#define	HWD_USB_EP0BUFCMD_TXVAL	0x0100	/* EP0DATA has valid data to write to host */

/* HWD_USB_EP0BUFSTAT_ADDR bits */
#define	HWD_USB_EP0BUFSTAT_LEN	0x0700

/* HWD_USB_EPRXBUFCTRL bits */
#define	HWD_USB_EPRXBUFCTRL_RXSZ  	0x0600
#define	HWD_USB_EPRXBUFCTRL_RXTH  	0x0608
#define	HWD_USB_EPRXBUFCTRL_RXTYPE 	0x0210
#define	HWD_USB_EPRXBUFCTRL_RXTGL  	0x0113
#define	HWD_USB_EPRXBUFCTRL_RXPEINT	0x0114
#define	HWD_USB_EPRXBUFCTRL_RXLCKEN	0x0115
#define	HWD_USB_EPRXBUFCTRL_RXPKSZ 	0x0216
#define	HWD_USB_EPRXBUFCTRL_RXADDR 	0x0618

/* HWD_USB_EPTXBUFCTRL bits */
#define	HWD_USB_EPTXBUFCTRL_TXSZ  	0x0500
#define	HWD_USB_EPTXBUFCTRL_TXTH  	0x0508
#define	HWD_USB_EPTXBUFCTRL_TXTYPE 	0x0210
#define	HWD_USB_EPTXBUFCTRL_TXTGL  	0x0112
#define	HWD_USB_EPTXBUFCTRL_TXZPE  	0x0113
#define	HWD_USB_EPTXBUFCTRL_TXPEINT	0x0114
#define	HWD_USB_EPTXBUFCTRL_TXLCKEN	0x0115
#define	HWD_USB_EPTXBUFCTRL_TXPKSZ 	0x0216
#define	HWD_USB_EPTXBUFCTRL_TXADDR 	0x0518

/* HWD_USB_EPBUFCMD bits */
#define	HWD_USB_EPBUFCMD_TXEOP		0x0100
#define	HWD_USB_EPBUFCMD_RXFLUSH	0x0104
#define	HWD_USB_EPBUFCMD_TXFLUSH	0x0108
#define	HWD_USB_EPBUFCMD_RXEMPTD	0x010C

/* HWD_USB_EPBUFSTAT bits */
#define	HWD_USB_EPBUFSTAT_RXUSED	0x0D00
#define	HWD_USB_EPBUFSTAT_TXFREE	0x0510
#define	HWD_USB_EPBUFSTAT_TXEMPTY	0x0118
#define	HWD_USB_EPBUFSTAT_TXLOCK	0x011C
#define	HWD_USB_EPBUFSTAT_RXLOCK	0x011D

/* HWD_USB_APPCTRL_ADDR bits */
#define	HWD_USB_APPCTRL_VIAXCVR		0x0204

/* HWD_USB_INTERFACE bits */
#define	HWD_USB_INTERFACE_AS		0x0300
#define	HWD_USB_INTERFACE_INTF		0x0204
#define	HWD_USB_INTERFACE_CONF		0x0208

/* HWD_USB_DEBUG_ADDR bits */
#define	HWD_USB_DEBUG_XCVR_ON		0x0100
#define	HWD_USB_DEBUG_XCVR_PUON		0x0101
#define	HWD_USB_DEBUG_XCVR_TXEN		0x0102
#define	HWD_USB_DEBUG_XCVR_SEO		0x0103
#define	HWD_USB_DEBUG_XCVR_DP		0x0104
#define	HWD_USB_DEBUG_XCVR_DIFF		0x0105
#define	HWD_USB_DEBUG_NOSLEEP		0x0108
#define	HWD_USB_DEBUG_GOODSYNC		0x0109
#define	HWD_USB_DEBUG_RESET			0x010A
#define	HWD_USB_DEBUG_WAKEUP		0x010B
#define	HWD_USB_DEBUG_COUNTER		0x010D	/* 1: start; 0: stop and reset */
#define	HWD_USB_DEBUG_CFGDONE		0x010E
#define	HWD_USB_DEBUG_SOFTRESET		0x010F
#define	HWD_USB_DEBUG_SYNCEN		0x0418

            /*--------------------------------------------**
            ** Register Bitfield And Constant Definitions **
            **--------------------------------------------*/

#if (SYS_BOARD == SB_CDS4)
/* LED_CTRL */
#define HWD_LED_1          0x0001
#define HWD_LED_2          0x0002
#define HWD_LED_3          0x0004
#define HWD_LED_4          0x0008

/* SW_CFG SW Config switch settings */
#define HWD_SWITCH_1       0x0001
#define HWD_SWITCH_2       0x0002	/*0= max wait DSPV rdy	1= no wait DSPV rdy */
#define HWD_SWITCH_3       0x0004
#define HWD_SWITCH_4       0x0008
#define HWD_SWITCH_5       0x0010	/*0= download DSPV	1= no download DSPV */
#define HWD_SWITCH_6       0x0020	/*0= download DSPM	1= no download DSPM */
#define HWD_SWITCH_7       0x0040
#define HWD_SWITCH_8       0x0080	/*0= cp boot mode	1= cp normal mode   */

#endif

/* HWD_RINGER_CLK_PD clock control */
#define HWD_RG_CLK_ENB     0x0001   /* 0 = enable, 1 = disable */
#define HWD_RG_BIT_MODE    0x0002   /* 0 = 10 bits, 1 = 11 bits (PDM/PWM bit resolution) */

/* CLK_CTRL0 Clock Control */
/* Bit 15 is Ringer clock select (0: USB, 1: Modem PLL) */
#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_RNG_CLK_SEL      0x0800
#endif
#define HWD_CLK1_EN          0x4000
#define HWD_CLK2_EN          0x2000
#define HWD_GTXI_CLK9_EN     0x1000
#define HWD_GFN_CLK9_EN      0x0800 
#define HWD_GTX_CLK9_EN      0x0400
#define HWD_GRX_CLK9_EN      0x0200
#define HWD_G_CLK19_EN       0x0100
#define HWD_RX_SD_ACK_EN     0x0080
#define HWD_DSPV_BOOT        0x0040
#define HWD_DSPM_BOOT        0x0020
#define HWD_DSPV_RST         0x0010
#define HWD_DSPM_RST         0x0008
#define HWD_CLK32SEL         0x0004
#define HWD_GUIM_CLK9_EN     0x0002
#define HWD_MXSG_RST         0x0001

/* CLK_CTRL1 Clock Control */
#define HWD_TST_32K_SEL      0x8000
#define HWD_GTX_CLK39_ENB    0x4000
#define HWD_GPCTL_CLK9_ENB   0x2000
#define HWD_GSRCH_CLK_ENB    0x1000
#define HWD_GVD_DM_CLK_ENB   0x0800
#ifdef SYS_OPTION_AMPS
#define HWD_CLK360K_ENB      0x0400
#define HWD_CLK80K_ENB       0x0200
#endif
#define HWD_BIST_CLK_ENB     0x0100
#define HWD_M_OCEM_KILL      0x0080
#define HWD_V_OCEM_KILL      0x0040
#define HWD_G_CLK8K_ENB      0x0020
#define HWD_JIT0_CLK_ENB     0x0010
#define HWD_JIT1_CLK_ENB     0x0008
#define HWD_JIT2_CLK_ENB     0x0004
#define HWD_GSPI_CLK9_ENB    0x0002
#define HWD_GRX_CLK39_ENB    0x0001

/* CLK_CTRL2 Clock Control */
#define HWD_FREQ_MASK        0x7FFF   /* mask to clear bit 15 */
#define HWD_FREQ_2048K       0x0000   /* bit 15 = [0] Select jittered  clock (2.048 or 2.4 MHz) */
#define HWD_FREQ_128K        0x8000   /* bit 15 = [1] */
#define HWD_PLL_BUF_BYP      0x2000
#define HWD_DSPV_CLK_SEL     0x1000
#define HWD_DSPM_CLK_SEL     0x0800
#define HWD_RHA_CLK_EN       0x0400
#define HWD_THA_CLK_EN       0x0200
#define HWD_DSPM_CLK_EN      0x0100
#define HWD_DSPV_CLK_EN      0x0080
#define HWD_CLK48_ENB        0x0040
#define HWD_MXS_RX_SD_CDMA_DCK_EN  0x0010
#define HWD_OSC_EN_EN        0x0008
#define HWD_OSC_EN_POL       0x0004
#ifdef SYS_OPTION_AMPS
#define HWD_MXS_RX_SD_AMPS_DCK_EN  0x0002
#define HWD_G_CLK19A_EN      0x0001
#endif
/* CLK_CTRL3 Clock Control */
#define HWD_CLK_SILENT       0x0000
#define HWD_SRCHR_PLL_SEL    0x8000 /* selects searcher clock from CP PLL/MDM PLL */
#define HWD_CLK_MUX_2_SEL    0x1f00
#define HWD_CLK_MUX_1_SEL    0x007F

#define HWD_CLK_CG_G_CLK19       0x001E
#define HWD_CLK_CG_UT0_BAUDCLK   0x0023

/* CLK_CTRL4 Clock Control */
#define HWD_MXS_AA_CLK_EN    0x8000
#define HWD_MXS_MDM_CLK_EN   0x4000
#define HWD_MXS_TX_SD_CLK_EN 0x2000
#define HWD_MXS_PDM_CLK_EN   0x0800
#define HWD_MXS_VC_CLK_EN    0x0400

/* CLK_CTRL5 Clock Control */
#define HWD_DSP_PLL_CLK_EN   0x8000
#define HWD_USB_PLL_SEL_EN   0x4000
#define HWD_USB_DVCLK_SEL    0x2000  /* selects between MDM and DSP PLL */
#if !((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_RNG_CLK_SEL      0x0800
#endif
#define HWD_DSP_DMCLK_SEL    0x0400
#define HWD_DSP_DVCLK_SEL    0x0200
#define HWD_UIM_FREQ_SEL     0x0080
#define HWD_JIT2_CLK_SEL     0x0040
#define HWD_JIT1_CLK_SEL     0x0020
#define HWD_USB_EXTCK_SEL    0x0010
#define HWD_JIT0_CLK_SEL     0x0008
#define HWD_MXS_AA_PD_SEL    0x0004
#define HWD_VCLK_PRGM_CLK_SEL 0x0002
#define HWD_MXS_TX_SD_PD_SEL 0x0001

/* TX_TEST_CTRL Bit definitions */
#define HWD_TX_TC_SWAP_IQ_CDMA    0x0100
#define HWD_TX_TC_OBS_FORMAT      0x0080
#define HWD_TX_TC_AMPS_RATE_SEL   0x0040
#define HWD_TX_TC_CALIB_EN_EXT    0x0020
#define HWD_TX_TC_CALIB_EN_INT    0x0010
#define HWD_TX_TC_MSB_INV_EXT     0x0008
#define HWD_TX_TC_MSB_INV_INT     0x0004
#define HWD_TX_TC_SWAP_IQ_AMPS    0x0002
#define HWD_TX_TC_TEST_MODE       0x0001

/* TX_ON_GPO */
#define HWD_TX_GPO_SEL_TXON0    0x0020
#define HWD_TX_GPO_SEL_TXON1    0x0040
#define HWD_TX_GPO_SEL_TXON2    0x0080
#define HWD_TX_GPO_SEL_TXON3    0x0100
#define HWD_TX_GPO_SEL_TXON4    0x0200
#define HWD_TX_GPO_SEL_TXON_ALL (HWD_TX_GPO_SEL_TXON0 | \
                                 HWD_TX_GPO_SEL_TXON1 | \
                                 HWD_TX_GPO_SEL_TXON2 | \
                                 HWD_TX_GPO_SEL_TXON3 | \
                                 HWD_TX_GPO_SEL_TXON4 )


/* CP Timers  */
/* Counter Control Reg Bit positions */
#define HWD_CTS_UNMSK_ST_RST 0x4
#define HWD_CTS_ENABLE_CTR  0x02
#define HWD_CTS_RESET_CNT   0x01

/* Ready Flag */
#define HWD_CTS_READY_BIT    0x01


/* HWD_CP_ISR0_L & HWD_CP_ISR0_H - Interrupt bit positions */
#define HWD_INT_SLP_RESYNC   0x8000  /* System resync event */ 
#define HWD_INT_SER0         0x4000  /* Serial Programmer 0 interrupt flag */
#define HWD_INT_SER1         0x2000  /* Serial Programmer 1 interrupt flag */
#define HWD_INT_SER2         0x1000  /* Serial Programmer 2 interrupt flag */
#define HWD_INT_UU0_11       0x0800  /* Unused */
#define HWD_INT_SLPTW        0x0400  /* Sleep Timer Wakeup */
#define HWD_INT_SLPTE        0x0200  /* Sleep Timer end flag */
#define HWD_INT_V2CR         0x0100  /* Voice to CP Read Ready */
#define HWD_INT_V2CW         0x0080  /* Voice to CP Write Ready */
#define HWD_INT_EED          0x0040  /* Eeprom programming done flag */
#define HWD_INT_UU3          0x0020  /* Unused */
#define HWD_INT_KEYPAD       0x0010  /* Keypad is pressed/released */
#define HWD_INT_M2CDR        0x0008  /* Modem to CP Data Read Ready */
#define HWD_INT_M2CCR        0x0004  /* Modem to CP Control Read Ready */
#define HWD_INT_M2CDW        0x0002  /* Modem to CP Data Write Ready */
#define HWD_INT_M2CCW        0x0001  /* Modem to CP Control Write Ready */
#define HWD_INT_SER_ALL      (HWD_INT_SER0 | HWD_INT_SER1 | HWD_INT_SER2)

/* HWD_CP_ISR1_L & HWD_CP_ISR1_H - Interrupt bit positions */
#define HWD_INT_GPIO5        0X8000  /* GPIO 5 Interrupt */
#define HWD_INT_GPIO4        0x4000  /* GPIO 4 Interrupt */
#define HWD_INT_GPIO3        0x2000  /* GPIO 3 Interrupt */
#define HWD_INT_GPIO2        0x1000  /* GPIO 2 Interrupt */
#define HWD_INT_GPIO1        0x0800  /* GPIO 1 Interrupt */
#define HWD_INT_GPIO0        0x0400  /* GPIO 0 Interrupt */
#define HWD_INT_PLLU         0x0200  /* PLL Unlocked */
#define HWD_INT_EOC          0x0100  /* End of Compare Interrupt */
#define HWD_INT_V2CFM        0x0080  /* Fast MailBox to CP from DSPV Interrupt */
#define HWD_INT_M2CFM        0x0040  /* Fast MailBox to CP from DSPM Interrupt */
#define HWD_INT_GPINTX       0x0020  /* GPIO_GI Interrupt */
#define HWD_INT_IRAM_WF      0x0010  /* IRAM Write Failure Interrupt */
#define HWD_INT_TIM3         0x0008  /* Timer 3 Interrupt */
#define HWD_INT_TIM2         0x0004  /* Timer 2 Interrupt */
#define HWD_INT_TIM1         0x0002  /* Timer 1 Interrupt */
#define HWD_INT_TIM0         0x0001  /* Timer 0 Interrupt */
#define HWD_INT_CTS_ALL      0x000F  

/* HWD_CP_ISR2_L & HWD_CP_ISR2_H - Interrupt bit positions */
#define HWD_INT_POLYRINGER   0x8000  /* Polyphonic Ringer Fast Interrupt */
#define HWD_INT_UU2_14       0x4000  /* Unused */
#define HWD_INT_UAWAK        0x2000  /* UART Wakeup Interrupt */   
#define HWD_INT_SWINT0       0x1000  /* Polyphonic Ringer Slow Interrupt */
#define HWD_INT_RTOS         0x0800  /* RTOS Interrupt */
#define HWD_INT_VMIC         0x0400  /* Voice Codec Microphone Interrupt */
#define HWD_INT_VSPK         0x0200  /* Voice Codec Speaker Interrupt */
#define HWD_INT_VMS          0x0100  /* Combined Microphone/Speaker Interrupt */
#define HWD_INT_UITX         0x0080  /* User Interface Transmit Interrupt */
#define HWD_INT_UIRX         0x0040  /* User Interface Receive Interrupt */
#define HWD_INT_UU2_05       0x0020  /* Unused */
#define HWD_INT_UU2_04       0x0010  /* Unused */
#define HWD_INT_RNG_A        0x0008  /* Ringer Interrupt */
#define HWD_INT_SSTIM        0x0004  /* Single Shot Timer Interrupt */
#define HWD_INT_USBWAL       0x0002  /* USB Wakeup Interrupt */
#define HWD_INT_USB          0x0001  /* USB Interrupt */
#define HWD_INT_DAI          0x0700  /* All Voice Codec Interrupts */

/* HWD_CP_ISR3_L & HWD_CP_ISR3_H - Interrupt bit positions */
#define HWD_INT_RFSP1        0x0002  /* RF Serial Programmer Interface Interrupt 1 */
#define HWD_INT_RFSP0        0x0001  /* RF Serial Programmer Interface Interrupt 0 */

/* System time interrupt mask definitions */
#define HWD_FRAME_CNT_INT_MASK      0x8000 /* Frame counter mask */

/* mixed signal block control bit positions */
#define HWD_VOICE_DIGITAL_ENABLE_MASK     0x0010 /* bit mask for voice codec digital control*/
#define HWD_VOICE_ADC_ANALOG_ENABLE_MASK  0x0001 /* bit mask for voice ADC analog enable */
#define HWD_VOICE_DAC_ANALOG_ENABLE_MASK  0x0001 /* bit mask for voice DAC analog enable */
#define HWD_VOICE_MIC_BIAS_ENABLE_MASK    0x0100 /* bit mask for voice microphone bias enable */
#define HWD_VOICE_CLK_JITTER_ENABLE_MASK  0x0040 /* bit mask for voice clock jitter circuit enable */
#define HWD_BANDGAP_REF_VOLT_ENABLE_MASK  0x0001 /* bit mask for bandgap ref voltage enable */
#define HWD_MS_BIAS_CELL_ENABLE_MASK	   0x0001 /* bit mask for mixed signal bias cell enable */
#define HWD_PDM_CELL0_ENABLE_MASK		   0x0001 /* bit mask for PDM cell 0 enable */
#define HWD_PDM_CELL1_ENABLE_MASK	      0x0002 /* bit mask for PDM cell 1 enable */
#define HWD_PDM_CELL2_ENABLE_MASK		  0x0004 /* bit mask for PDM cell 2 enable */
#define HWD_PDM_CELL3_ENABLE_MASK		  0x0008 /* bit mask for PDM cell 3 enable */
#define HWD_RX_ADC_ENABLE_MASK			  0x0001 /* bit mask for Rx ADC enable */

#define HWD_TX_DAC_ENABLE_MASK			  0x0001 /* bit mask for Tx DAC enable */
#define HWD_AUX_ADC_ENABLE_MASK			  0x0008 /* bit mask for AUX ADC enable */
#define HWD_VOICE_DAC_SETTING_MASK		  0x0011 /* bit mask to retain dac setting */
#define HWD_VOICE_DAC_VOLUME_MASK		  0x000E /* bit mask to retain volume setting */
#define HWD_AMPS_COMPS_ENABLE_MASK        0x0003 /* bit mask for AMPS Comparitors enable */

/* CP_MBM_CHS Modem Mailbox Handshake Register bit position */
#define HWD_MMB_WHS         0x01
#define HWD_MMB_RHS         0x01

/* Vocoder Fast Mailbox Handshake Register bit position */
#define HWD_VMB_F_RHS       0x01

/* CP_MBV_CHS Vocoder Mailbox Handshake Register bit position */
#define HWD_VMB_WHS         0x01    /* lsb of HWD_MBV_C2V_WRDY */
#define HWD_VMB_RHS         0x01    /* lsb of HWD_MBV_C2V_RRDY */

/* TST_RAMMD register mode values */
#define HWD_TST_NORMAL      0x00
#define HWD_TST_RX_SAMP     0x08
#define HWD_TST_CP_WR       0x09
#define HWD_TST_TX_SAMP     0x0A
#define HWD_TST_RX_RD       0x0C
#define HWD_TST_CP_RD       0x0D
#define HWD_TST_RX_SIGDEL   0x0E

/*---------------------**
**    I2C Interface    **
**---------------------*/

/* EE_CTL I2C EEPROM control register bit position */
#define HWD_EE_RRD          0x01
#define HWD_EE_BW_PW        0x02
#define HWD_EE_SW_RST       0x04
#define HWD_EE_2K_N         0x08

/* EE_RD_ACK I2C EEPROM read ack register values */
#define HWD_EE_ACK_CONT     0x00    /* Continuous read */
#define HWD_EE_ACK_SING     0x01    /* Single read */

/* EE_STATUS I2C EEPROM status register bit position */
#define HWD_EE_BUSY         0x01
#define HWD_EE_R_W          0x02
#define HWD_EE_RRW          0x04
#define HWD_EE_RDY          0x08
#define HWD_EE_ERR          0x10

/*-----------------------**
** Serializer Registers  **
**-----------------------*/

/* SER_TRIG - Serial Device Trigger Mode */
#define HWD_SER_TMR1_CONT 0x0020   /* Program using block 1, every match                 */
#define HWD_SER_TMR1_ONCE 0x0010   /* Program using block 1, once                        */
#define HWD_SER_IMMED1    0x0008   /* Program using block 1, immediately                 */
#define HWD_SER_TMR0_CONT 0x0004   /* Program using block 0, every match                 */
#define HWD_SER_TMR0_ONCE 0x0002   /* Program using block 0, once                        */
#define HWD_SER_IMMED0    0x0001   /* Program using block 0, immediately                 */

/* SER_TRIG - Serial device interrupt mode */
#define HWD_SER_INTMD_0     1
#define HWD_SER_INTMD_1     2
#define HWD_SER_INTMD_2     4
#define HWD_SER_INTMD_3     8

/* RX_CHAN_SEL - Rx Channel Select */
#define HWD_RX_CHAN_SEL_SYNC      0
#define HWD_RX_CHAN_SEL_TRAFFIC   1
#define HWD_RX_CHAN_SEL_PAGING    3

/* RF and TX ON control */
#define HWD_ON_CFG_ACTIVE_HI 4
#define HWD_ON_CFG_CMOS    2
#define HWD_ON_CFG_DRIVE   1
#define HWD_TX_MODEM_ON    0x0001   /* enable Tx modem, Tx modem state machines
                                     * are proceed as normal */
#define HWD_TX_MODEM_OFF   0        /* disable Tx modem, Tx modem state machines
                                     * are forced to initial states */

#define HWD_ON_MODE_IMD    0

#define HWD_ON_OFF         0
#define HWD_ON_ON          1
#define HWD_ON_AUTO        2

/*--------------------------------------------------------------------
* Sleep Control (SLP_CTRL) Register Bit definitions
*--------------------------------------------------------------------*/

/* enables, to be OR'ed into the register */
#define HWD_SLPCTRL_CALIB_ENABLE_MASK        0x0001  /* sleep ctrl register, bit0 = 0 */
#define HWD_SLPCTRL_RESYNC_ENABLE_MASK       0x0002  /* sleep ctrl register, bit1 = 0 */
#define HWD_SLPCTRL_RESYNC_FORCE_MASK        0x0004  /* sleep ctrl register, bit2 = 0 */
#define HWD_SLPCTRL_SLP_ENABLE_MASK          0x0008  /* sleep ctrl register, bit3 = 0 */
#define HWD_SLPCTRL_CPCK_SEL_MASK            0x0010  /* sleep ctrl register, bit4 = 0 */
#define HWD_SLPCTRL_CKMN_BYP_MASK            0x0020  /* sleep ctrl register, bit5 = 0 */ 
#define HWD_SLPCTRL_PLLBYP_CTL_MASK          0x0040  /* sleep ctrl register, bit6 = 0 */
#define HWD_SLPCTRL_PLLBUF_BYP_MASK          0x0080  /* sleep ctrl register, bit7 = 0 */
#define HWD_SLPCTRL_PLLBYP_CTL_PROC_MASK     0x0100  /* sleep ctrl register, bit8 = 0 */
#define HWD_SLPCTRL_PLLBYP_BYP_MASK          0x0200  /* sleep ctrl register, bit9 = 0 */
#define HWD_SLPCTRL_MDM_PLLEN_BYP_MASK       0x0400  /* sleep ctrl register, bit10 = 0 */
#define HWD_SLPCTRL_PROC_PLLEN_BYP_MASK      0x0800  /* sleep ctrl register, bit11 = 0 */
#define HWD_SLPCTRL_USB_PLLEN_BYP            0x1000  /* sleep ctrl register, bit12 = 0 */
#define HWD_SLPCTRL_PLLBYP_CTL_USB           0x2000  /* sleep ctrl register, bit13 = 1 */
#define HWD_SLPCTRL_USB_PLLEN                0x4000  /* sleep ctrl register, bit14 = 0 */
#define HWD_SLPCTRL_DSP_PLL_BYP              0x8000  /* sleep ctrl register, bit15 = 0 */

#define HWD_PLL_BYPASS_MASK_ALL (HWD_SLPCTRL_PLLBYP_CTL_MASK | HWD_SLPCTRL_PLLBYP_CTL_PROC_MASK |\
          HWD_SLPCTRL_CPCK_SEL_MASK | HWD_SLPCTRL_CKMN_BYP_MASK | HWD_SLPCTRL_PLLBUF_BYP_MASK | \
          HWD_SLPCTRL_MDM_PLLEN_BYP_MASK | HWD_SLPCTRL_PROC_PLLEN_BYP_MASK | \
          HWD_SLPCTRL_USB_PLLEN_BYP | HWD_SLPCTRL_PLLBYP_CTL_USB | HWD_SLPCTRL_DSP_PLL_BYP)      

/* disables, to be AND'ed into the register */
#define HWD_SLPCTRL_CALIB_DISABLE_MASK       0xFFFE  /* sleep ctrl register, bit1 = 0 */
#define HWD_SLPCTRL_RESYNC_DISABLE_MASK      0xFFFD  /* sleep ctrl register, bit3 = 0 */

/* forced events (self clearing), to be OR'ed into the register */
#define HWD_SLPCTRL_CALIB_START_MASK         0x0001  /* sleep ctrl register, bit0 = 1 32KHz reg is clear */
#define HWD_SLPCTRL_SLEEP_CMD_MASK           0x0004  /* sleep ctrl register, bit2 = 1, sleep command */

#define HWD_SLPCTRL_PLLBYP_CTL_PROC          0x0100  /* bit #8, set to bypass Proc PLL on wakeup */
#define HWD_SLPCTRL_MDM_PLLEN_BYP            0x0400  /* bit #10, set to enable Modem PLL during sleep */
#define HWD_SLPCTRL_PROC_PLLEN_BYP           0x0800  /* bit #11, set to enable Proc PLL during sleep */

/*--------------------------------------------------------------------------------
* External wake enable Sleep Control (HWD_CS_EXT_WAKE_EN) Register Bit definitions
*--------------------------------------------------------------------------------*/
#define HWD_EXT_WAKE_GPINT0      0x0001   /* External wake enable register, bit 0 = 1 */
#define HWD_EXT_WAKE_GPINT1      0x0002   /* External wake enable register, bit 1 = 1 */
#define HWD_EXT_WAKE_GPINT2      0x0004   /* External wake enable register, bit 2 = 1 */
#define HWD_EXT_WAKE_GPINT3      0x0008   /* External wake enable register, bit 3 = 1 */
#define HWD_EXT_WAKE_GPINT4      0x0010   /* External wake enable register, bit 4 = 1 */
#define HWD_EXT_WAKE_GPINT5      0x0020   /* External wake enable register, bit 5 = 1 */
#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
#define HWD_EXT_WAKE_UN1         0x0040   /* External wake enable register, bit 6 = 1 */
#define HWD_EXT_WAKE_GPINTX      0x0080   /* External wake enable register, bit 7 = 1 */
#define HWD_EXT_WAKE_UART0IN     0x0100   /* External wake enable register, bit 8 = 1 */
#define HWD_EXT_WAKE_UART0CTS    0x0200   /* External wake enable register, bit 9 = 1 */
#else
#define HWD_EXT_WAKE_UART1IN     0x0040   /* External wake enable register, bit 6 = 1 */
#define HWD_EXT_WAKE_GPINTX      0x0080   /* External wake enable register, bit 7 = 1 */
#define HWD_EXT_WAKE_UART0IN     0x0100   /* External wake enable register, bit 8 = 1 */
#define HWD_EXT_WAKE_UART2IN     0x0200   /* External wake enable register, bit 9 = 1 */
#endif
#define HWD_EXT_WAKE_USBSUSP     0x0400   /* External wake enable register, bit 10 = 1 */
/*--------------------------------------------------------------------------------
* External wake enable Sleep Control (HWD_CS_EXT_WAKE_EN) Register Bit definitions
*--------------------------------------------------------------------------------*/
#define HWD_EXT_WAKE_GPINT(num)  (1<<num) /* External wake enable register, bit num = 1 
                                             num=0..11; defines GPINT0..GPINT11 */
#define HWD_EXT_WAKE_USB         (1<<12)  /* External wake enable register, bit 12 = 1 */
#define HWD_EXT_WAKE_UART0       (1<<13)  /* External wake enable register, bit 13 = 1 */
#define HWD_EXT_WAKE_UART1       (1<<14)  /* External wake enable register, bit 14 = 1 */
#define HWD_EXT_WAKE_UART2       (1<<15)  /* External wake enable register, bit 15 = 1 */

/*--------------------------------------------------------------------------------
* GPINT CTL (HWD_CS_GPINT_CTRL) Register Bit definitions
*--------------------------------------------------------------------------------*/
#define HWD_CS_GPINT_CTL_GPINT0 0x0001
#define HWD_CS_GPINT_CTL_GPINT1 0x0002
#define HWD_CS_GPINT_CTL_GPINT2 0x0004
#define HWD_CS_GPINT_CTL_GPINT3 0x0008
#define HWD_CS_GPINT_CTL_GPINT4 0x0010
#define HWD_CS_GPINT_CTL_GPINT5 0x0020

/*--------------------------------------------------------------------
* Define RTOS timer control register bit definitions
*--------------------------------------------------------------------*/

#define HWD_RTOS_ENABLE       0x01
#define HWD_RTOS_TIMER_VALUE  327   /* RTOS 10 msec timer value */

/*--------------------------------------------------------------------
* UART Interrupt Enable Register (IER) Bit definitions
*--------------------------------------------------------------------*/

#define HWD_IER_RX_HOLDING_INT 0x01 /* b0 - Receive Holding Register interrupt  - Enabled When Set   */
#define HWD_IER_TX_HOLDING_INT 0x02 /* b1 - Transmit Holding Register interrupt - Enabled When Set   */
#define HWD_IER_LINE_STAT_INT  0x04 /* b2 - Receiver Line Status interrupt      - Enabled When Set   */
#define HWD_IER_MODEM_STAT_INT 0x08 /* b3 - Modem Status Register interrupt     - Enabled When Set   */
#define HWD_IER_INTS_OFF       0x00 /*      Turn off all interrupts                                  */
#define HWD_IER_INTS_ON        0x07 /*      Turn on all interrupts except modem status               */

/*--------------------------------------------------------------------
* UART FIFO Control Register (FCR) Bit definitions
*--------------------------------------------------------------------*/

#define HWD_FCR_FIFO_ENABLE     0x01 /* b0 - Tx and Rx FIFO Enable               - Enabled When Set   */
#define HWD_FCR_RX_FIFO_RESET   0x02 /* b1 - Clear Rx FIFO and reset its counter - Clears When Set    */
#define HWD_FCR_TX_FIFO_RESET   0x04 /* b2 - Clear Tx FIFO and reset its counter - Clears When Set    */
#define HWD_FCR_DMA_MODE_SELECT 0x08 /* b3 - Change DMA Mode State from m0 to m1 - Mode 1 When Set    */
#define HWD_FCR_FIFO_TRIG_1     0x00 
#define HWD_FCR_FIFO_TRIG_4     0x40 
#define HWD_FCR_FIFO_TRIG_8     0x80 
#define HWD_FCR_FIFO_TRIG_14    0xC0 

/* FCR b7 - b6 FIFO Trigger Level  */
 
#define HWD_FCR_RX_TRIG_LVL_01  0x00 /* 0 0 - FIFO Rx Trigger Level 01 */              
#define HWD_FCR_RX_TRIG_LVL_04  0x40 /* 0 1 - FIFO Rx Trigger Level 04 */          
#define HWD_FCR_RX_TRIG_LVL_08  0x80 /* 1 0 - FIFO Rx Trigger Level 08 */
#define HWD_FCR_RX_TRIG_LVL_16  0xC0 /* 1 1 - FIFO Rx Trigger Level 16 */

/*--------------------------------------------------------------------
* UART Latch Control Register (LCR) Bit definitions
*--------------------------------------------------------------------*/

/* LCR b2 defines the stop bits setup b1 - b0 define the Tx - Rx Word Length                        */
/* The following defines cover all of the available options                                         */

#define HWD_LCR_5_BIT_WORD_1    0x00 /* 0 0 0  - 5 Bit Word - 1 Stop Bit   */
#define HWD_LCR_6_BIT_WORD_1    0x01 /* 0 0 1  - 6 Bit Word - 1 Stop Bit   */ 
#define HWD_LCR_7_BIT_WORD_1    0x02 /* 0 1 0  - 7 Bit Word - 1 Stop Bit   */
#define HWD_LCR_8_BIT_WORD_1    0x03 /* 0 1 1  - 8 Bit Word - 1 Stop Bit   */
#define HWD_LCR_5_BIT_WORD_1P5  0x04 /* 1 0 0  - 5 Bit Word - 1.5 Stop Bit */
#define HWD_LCR_6_BIT_WORD_2    0x05 /* 1 0 1  - 6 Bit Word - 2 Stop Bit   */
#define HWD_LCR_7_BIT_WORD_2    0x06 /* 1 1 0  - 6 Bit Word - 1 Stop Bit   */
#define HWD_LCR_8_BIT_WORD_2    0x07 /* 1 1 1  - 6 Bit Word - 1 Stop Bit   */

#define HWD_LCR_PARITY_ENABLE   0x08 /* b3 - Enable Parity Bit Generation and Check - Enabled When Set */
#define HWD_LCR_PARITY_EVEN     0x10 /* b4 - Odd/Even Parity Generation and Check   - Even When Set    */
#define HWD_LCR_PARITY_SET      0x20 /* b5 - Toggle Generated Parity Bit 0/1        - 0 When Set       */
#define HWD_LCR_BREAK_SET       0x40 /* b6 - Force Break Control ( Tx o/p low)      - Forced When Set  */
#define HWD_LCR_DIVISOR_LATCH   0x80 /* b7 - Enable internal Baud Rate Latch        - Enabled When Set */

/*--------------------------------------------------------------------
* UART Modem Control Register (MCR) Bit definitions
*--------------------------------------------------------------------*/

#define HWD_MCR_DTR_BIT         0x01 /* b0 - Set DTR Signal Low/High - DTR Low when Set */
#define HWD_MCR_DTR_ASSERT      0x01 /* Assert DTR */
#define HWD_MCR_DTR_DEASSERT    0x00 /* Deassert DTR */

#define HWD_MCR_RTS_LOW         0x02 /* b1 - Set RTS Signal Low/High - RTS Low when Set */
                                     /* MCR b2 is not used                              */
#define HWD_MCR_INTERRUPT_EN    0x08 /* b3 - interrupt output pin Operate/3-State  - Operate when Set */
#define HWD_MCR_LOOPBACK_MODE   0x10 /* b4 - Loopback(Test) Mode Enable            - Enabled When Set */

#define HWD_MCR_FLOW_CTRL_EN    0x20 /* b5 - automatic flow control enable */

/* The Following Registers are Status Registers which Report conditions within the UART/PPP during  *
 * operation. The defined values are masks to ensure that the register flags are correctly accessed */

/*--------------------------------------------------------------------
* UART Interrupt Status Register (ISR) Bit definitions
*--------------------------------------------------------------------*/

/* ISR b0 indicates that an interrupt is pending when clear. b3 - b1 signal which interrupt as per:- */

#define HWD_ISR_INT_PEND        0x01 
#define HWD_ISR_MODEM_SOURCE    0x00 /* 0 0 0 - Modem Status Register         Priority 4 */
#define HWD_ISR_TX_RDY_SOURCE   0x02 /* 0 0 1 - Transmitter Holding Reg Empty Priority 3 */
#define HWD_ISR_RX_RDY_SOURCE   0x04 /* 0 1 0 - Received Data Ready           Priority 2 */
#define HWD_ISR_LSR_SOURCE      0x06 /* 0 1 1 - Receiver Line Status Register Priority 1 */
#define HWD_ISR_RX_RDY_TO_SRC   0x0C /* 1 1 0 - Received Data Ready Time Out  Priority 2 */

/* ISR b7 - b4 are not used - in st16c552 b7 - b6 are Set b5 - b4 are Clear */

/*--------------------------------------------------------------------
* UART Line Status Register (LSR) Bit definitions
*--------------------------------------------------------------------*/

#define HWD_LSR_RX_DATA_READY   0x01 /* b0 - Data Received and Saved in Holding Reg - Set when Valid */
#define HWD_LSR_OVERRUN_ERROR   0x02 /* b1 - Overrun Error Occurred                 - Set When Valid */
#define HWD_LSR_PARITY_ERROR    0x04 /* b2 - Received Data has Incorrect Parity     - Set When Valid */
#define HWD_LSR_FRAMING_ERROR   0x08 /* b3 - Framing Error (No Stop Bit)            - Set When Valid */
#define HWD_LSR_BREAK_INTERRUPT 0x10 /* b4 - Break Signal Received                  - Set When Valid */
#define HWD_LSR_TX_HOLD_EMPTY   0x20 /* b5 - Tx Holding Register is empty and ready - Set When Valid */
#define HWD_LSR_TX_FIFO_EMPTY   0x40 /* b6 - Tx Shift Registers and FIFO are Empty  - Set When Valid */
#define HWD_LSR_FIFO_ERROR      0x80 /* b7 - At Least one of b4 - b2 has occurred   - Set When Valid */

/*--------------------------------------------------------------------
* UART Modem Status Register (MSR) Bit definitions
*--------------------------------------------------------------------*/

#define HWD_MSR_CTS_CHANGE      0x01 /* b0 - Set When CTS Input has Changed State */
#define HWD_MSR_DSR_CHANGE      0x02 /* b1 - Set When DSR Input has Changed State */
#define HWD_MSR_RI_CHANGE       0x04 /* b2 - Set When RI  Input has Changed State */
#define HWD_MSR_CD_CHANGE       0x08 /* b3 - Set When CD  Input has Changed State */

#define HWD_MSR_CTS_BIT         0x10 /* b4 - RTS Equivalent during loopback - inverse of CTS */
#define HWD_MSR_CTS_ASSERT      0x00 /*      CTS asserted */
#define HWD_MSR_CTS_DEASSERT    0x10 /*      CTS deasserted */

#define HWD_MSR_DSR_LP_STATE    0x20 /* b5 - DTR Equivalent during loopback - inverse of DSR */
#define HWD_MSR_RI_LP_STATE     0x40 /* b6 - MCR b2 Equivalent during loopback - inverse of RI */
#define HWD_MSR_CD_LP_STATE     0x88 /* b7 - int EN Equivalent during loopback - inverse of CD */

/*--------------------------------------------------------------------
* UART ScratchPad Register (SPR) Bit definitions
*--------------------------------------------------------------------*/

/* This is a user register for any required bit storage required */

#define HWD_SPR_USER0           0x01
#define HWD_SPR_USER1           0x02
#define HWD_SPR_USER2           0x04
#define HWD_SPR_USER3           0x08
#define HWD_SPR_USER4           0x10
#define HWD_SPR_USER5           0x20
#define HWD_SPR_USER6           0x40
#define HWD_SPR_USER7           0x80

/*------------------------------------------------------------------------
* UART Divisor Latch Lower and Upper Byte Values (DLL DLM) Bit definitions
*------------------------------------------------------------------------*/

/* Baud Rate Time Constant t = (Baud Rate Clock Frequency)/(Baud Rate * 16) 
   These are the required 16 bit divisor values for the internal baud rate 
   based on the define baud clock frequency */
#define BAUD_CLOCK_FREQ         3686400

/* This macro adds 0.5 to the divisor constant which the compiler truncates */
#define BAUD_DIVISOR(baud)      ((BAUD_CLOCK_FREQ/(baud*8)+1)>>1)

#define BAUD_DIV_MSB(baud)      (((BAUD_DIVISOR(baud)) >> 8) & 0xff)
#define BAUD_DIV_LSB(baud)      ((BAUD_DIVISOR(baud)) & 0xff)

#define HWD_DLM_57600_BAUD      BAUD_DIV_MSB(57600)
#define HWD_DLL_57600_BAUD      BAUD_DIV_LSB(57600)

#define HWD_DLM_115200_BAUD     BAUD_DIV_MSB(115200)
#define HWD_DLL_115200_BAUD     BAUD_DIV_LSB(115200)

#define HWD_DLM_230400_BAUD     BAUD_DIV_MSB(230400)
#define HWD_DLL_230400_BAUD     BAUD_DIV_LSB(230400)

/* Baud Rate Time Constant t = (Baud Rate Clock Frequency)/(Baud Rate * 16) 
   These are the required 16 bit divisor values for the internal baud rate 
   based on the define baud clock frequency */
#define BAUD_CLOCK_FREQ_DATA    2457600

/* This macro adds 0.5 to the divisor constant which the compiler truncates */
#define BAUD_DIVISOR_DATA(baud) ((BAUD_CLOCK_FREQ_DATA/(baud*8)+1)>>1)

#define BAUD_DIV_MSB_DATA(baud) (((BAUD_DIVISOR_DATA(baud)) >> 8) & 0xff)
#define BAUD_DIV_LSB_DATA(baud) ((BAUD_DIVISOR_DATA(baud)) & 0xff)

#define HWD_DLM_9600_BAUD       BAUD_DIV_MSB_DATA(9600)
#define HWD_DLL_9600_BAUD       BAUD_DIV_LSB_DATA(9600)

#define HWD_DLM_19200_BAUD      BAUD_DIV_MSB_DATA(19200)
#define HWD_DLL_19200_BAUD      BAUD_DIV_LSB_DATA(19200)

#define HWD_DLM_38400_BAUD      BAUD_DIV_MSB_DATA(38400)
#define HWD_DLL_38400_BAUD      BAUD_DIV_LSB_DATA(38400)

#define HWD_DLM_153600_BAUD     BAUD_DIV_MSB_DATA(153600)
#define HWD_DLL_153600_BAUD     BAUD_DIV_LSB_DATA(153600)

#define HWD_DLM_600_BAUD       BAUD_DIV_MSB_DATA(600)
#define HWD_DLL_600_BAUD       BAUD_DIV_LSB_DATA(600)

#define HWD_DLM_300_BAUD       BAUD_DIV_MSB_DATA(300)
#define HWD_DLL_300_BAUD       BAUD_DIV_LSB_DATA(300)

/* Baud Rate Time Constant t = (Baud Rate Clock Frequency)/(Baud Rate * 16) 
   These are the required 16 bit divisor values for the internal baud rate 
   based on the define baud clock frequency*/
#define BAUD_DS_CLOCK_FREQ_DATA    9600

/* This macro adds 0.5 to the divisor constant, which the compiler truncates */
#define BAUD_DS_DIVISOR_DATA(baud) ((BAUD_DS_CLOCK_FREQ_DATA/(baud*8)+1)>>1)

#define BAUD_DS_DIV_MSB_DATA(baud) (((BAUD_DS_DIVISOR_DATA(baud)) >> 8) & 0xff)
#define BAUD_DS_DIV_LSB_DATA(baud) ((BAUD_DS_DIVISOR_DATA(baud)) & 0xff)

#define HWD_DS_DLM_600_BAUD        BAUD_DS_DIV_MSB_DATA(600)
#define HWD_DS_DLL_600_BAUD        BAUD_DS_DIV_LSB_DATA(600)

#define HWD_DS_DLM_300_BAUD        BAUD_DS_DIV_MSB_DATA(300)
#define HWD_DS_DLL_300_BAUD        BAUD_DS_DIV_LSB_DATA(300)

/*------------------------------------------------------------------------
 * HWD_KPD_CTRL bit definitions
 *------------------------------------------------------------------------*/
#define HWD_KPD_CTRL_EN_6x5     0x00000001  /* Enables 6x5 keypad         */
#define HWD_KPD_CTRL_RESET      0x00000002  /* Resets keypad logic to power on conditions. This
                                               bit is automaticly cleared by h/w after reset takes effect */

/*------------------------------------------------------------------------
 * PWR_OFF_CTRL bit definitions
 *------------------------------------------------------------------------*/
#define HWD_PWR_OFF_DAT        0x00000001  /* power off level, active low                           */
#define HWD_PWR_OFF_OE         0x00000002  /* output enable, active low                             */

/*------------------------------------------------------------------------
* HWD_RNGR_CTRL bit definitions
*------------------------------------------------------------------------*/

#define	HWD_RNGR_FOREVER     0x0020
#define	HWD_RNGR_IMMED       0x0010
#define	HWD_RNGR_TSTMEM_EN   0x0008
#define	HWD_RNGR_MEMSEL      0x0004
#define	HWD_RNGR_DAT         0x0002
#define	HWD_RNGR_EN          0x0001

/*------------------------------------------------------------------------
* Ringer Memory definitions
*------------------------------------------------------------------------*/

#define HWD_RINGER_MEMORY_SIZE         896   /* words */
#define HWD_RINGER_MEMORY_CHANNELS     1     /* words */

/* HWD_RINGER_PROC_CTRL bit definitions */
#define HWD_RINGER_MEMORY_CTRL_CP      0
#define HWD_RINGER_MEMORY_CTRL_DSPV    1

/*------------------------------------------------------------------------
* Voice Codec dividers - HWD_VC_JIT_OFFSET/CONST/DIV
*------------------------------------------------------------------------*/

/* Voice Codec clock is sourced from TCXO (19.2 MHz)
   Frequency is 2.4 MHz
*/
#define  HWD_VC_JIT_OFFSET_VALUE    0x0000
#define  HWD_VC_JIT_CONST_VALUE     0x0000
#define  HWD_VC_JIT_DIV_VALUE       0x0008

/* Voice Codec dividers for use with EDAI configuration sourced from 19.2MHz TCXO */
#define  HWD_VC_JIT_EDAI_OFFSET_VALUE  0x7fb5
#define  HWD_VC_JIT_EDAI_CONST_VALUE   0x0003
#define  HWD_VC_JIT_EDAI_DIV_VALUE     0x0009

/*------------------------------------------------------------------------
* HWD_ST_CPINT_FR bit definitions
*------------------------------------------------------------------------*/

#define HWD_ST_FR_TYPE_BIT   0x0001
#define HWD_ST_FR_TYPE_20MS  0x0000
#define HWD_ST_FR_TYPE_26MS  0x0001

/*------------------------------------------------------------------------
* Configuration register definitions
*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
* HWD_CI_CSCFG01 definitions
*------------------------------------------------------------------------*/
 #define HWD_CI_CSCFG0_RWT0 (5)         /* CS0 Read Wait States, bits 0-3 */
 #define HWD_CI_CSCFG0_WWT0 (5 << 4)    /* CS0 Write Wait States, bits 4-7 */
 #define HWD_CI_CSCFG0_RWT1 (5 << 8)    /* CS1 Read Wait States, bits 8-11 */
 #define HWD_CI_CSCFG0_WWT1 (5 << 12)   /* CS1 Write Wait States, bits 12-15 */

/*------------------------------------------------------------------------
* HWD_CI_CSCFG23 definitions
*------------------------------------------------------------------------*/
#define HWD_CI_CSCFG1_RWT2 (2)         /* CS2 Read Wait States, bits 0-3 */
#define HWD_CI_CSCFG1_WWT2 (2 << 4)    /* CS2 Write Wait States, bits 4-7 */
#define HWD_CI_CSCFG1_RWT3 (4 << 8)    /* CS3 Read Wait States, bits 8-11 */
#define HWD_CI_CSCFG1_WWT3 (4 << 12)   /* CS3 Write Wait States, bits 12-15 */

/*------------------------------------------------------------------------
* HWD_CI_CSCFG45 definitions
*------------------------------------------------------------------------*/
#define HWD_CI_CSCFG2_RWT4 (15)        /* CS4 Read Wait States, bits 0-3 */
#define HWD_CI_CSCFG2_WWT4 (15 << 4)   /* CS4 Write Wait States, bits 4-7 */
#define HWD_CI_CSCFG2_RWT5 (15 << 8)   /* CS5 Read Wait States, bits 8-11 */
#define HWD_CI_CSCFG2_WWT5 (15 << 12)  /* CS5 Write Wait States, bits 12-15 */

/*------------------------------------------------------------------------
* HWD_EMC_AW_SN_RMAP definitions
*------------------------------------------------------------------------*/
 #define HWD_EMC_AW           (4)     /* APB Wait States, bits 0-3 */
#if (SYS_ASIC == SA_RAM)
#define HWD_EMC_UIM_VLTSEL   (1)     /* Voltage select, UIM at 1.8V, bit 0 */
#define HWD_EMC_CPBUS_VLTSEL (1 << 1)/* Voltage select, CP Bus at 1.8V, bit 1 */
#else
#define HWD_EMC_CPBUS_VLTSEL (1)     /* Voltage select, CP Bus at 1.8V, bit 0 */
#define HWD_EMC_UIM_VLTSEL   (1 << 1)/* Voltage select, UIM at 1.8V, bit 1 */
#endif
#define HWD_EMC_RESERVED     (3 << 2)/* Reserved, bits 2-3 */

#define HWD_EMC_LCD_ADDR   (2 << 4)  /* LCD Controller Addr, bits 4-6 */
#define HWD_EMC_BOOT_ROM   (1 << 7)  /* Internal BootRom (not used for CBP4), bit 7 */
#define HWD_EMC_CSPOL      (0 << 8)  /* Active chip select polarity for CS2-5, bits 8-11 */
#define HWD_EMC_SN_NORMAL  (0 << 14) /* Snooze/Normal execution, bit 14 */
#define HWD_EMC_SN_SNOOZE  (1 << 14) /* Snooze/Normal execution, bit 14 */
#define HWD_EMC_REMAP      (1 << 15) /* Remap IRAM to address 0, bit 15 */

/*------------------------------------------------------------------------
* Clock/PLL definitions
*------------------------------------------------------------------------*/
#define HWD_CLK_PROC_PLL_M            0x001d
#define HWD_CLK_PROC_PLL_N            0x000a


#if ((SYS_ASIC == SA_ROM) && (SYS_VERSION == SV_REV_C3))  /* ROM C3: DSP Clock set to 19.2 x 70 / 20 = 67.2 MHz */
#define HWD_CLK_DSP_PLL_M             0x0046
#define HWD_CLK_DSP_PLL_N             0x0014
#elif ((SYS_ASIC == SA_ROM) && (SYS_VERSION == SV_REV_C4))/* ROM C4: DSP Clock set to 19.2 x 58 / 16 = 69.6 MHz */
#define HWD_CLK_DSP_PLL_M             0x003a
#define HWD_CLK_DSP_PLL_N             0x0010
#else                                                     /* RAM: DSP Clock set to 19.2 x 74 / 20 = 71.04 MHz */
#define HWD_CLK_DSP_PLL_M             0x004a
#define HWD_CLK_DSP_PLL_N             0x0014
#endif


#define HWD_CLK_USB_PLL_M             0x0014
#define HWD_CLK_USB_PLL_N             0x0008

/* CG_PLL_CTRL bit masks */
#define HWD_CLK_CG_PLL_USB_PWRDWN     0x0100  /* bit #8 */
#define HWD_CLK_CG_PLL_PROC_PWRDWN    0x0010  /* bit #4 */
#define HWD_CLK_CG_PLL_MDM_PWRDWN     0x0001  /* bit #1 */

#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))
 #if (SYS_OPTION_PLL_DEFINITION == SYS_MDM_PLL_FRACTIONAL_M)

  #define HWD_CLK_CDMA_MDM_PLL_M_1920         0x000a
  #define HWD_CLK_CDMA_MDM_PLL_N_1920         0x0005
  #define HWD_CLK_CDMA_MDM_PLL_CONST_1920     0x0006
  #define HWD_CLK_CDMA_MDM_PLL_OFFSET_1920    0x1f00

 #elif  (SYS_OPTION_PLL_DEFINITION == SYS_MDM_PLL_INTEGER_M)

  #define HWD_CLK_CDMA_MDM_PLL_M_1920         0x0100
  #define HWD_CLK_CDMA_MDM_PLL_N_1920         0x007d
  #define HWD_CLK_CDMA_MDM_PLL_CONST_1920     0
  #define HWD_CLK_CDMA_MDM_PLL_OFFSET_1920    0
 #endif

 #define HWD_CLK_AMPS_MDM_PLL_M_1920          0x0002
 #define HWD_CLK_AMPS_MDM_PLL_N_1920          0x0001
 #define HWD_CLK_AMPS_MDM_PLL_CONST_1920      0
 #define HWD_CLK_AMPS_MDM_PLL_OFFSET_1920     0

#else /* if not RAM C1/C2 */

#define HWD_CLK_CDMA_MDM_PLL_M_1920          0x0100
#define HWD_CLK_CDMA_MDM_PLL_N_1920          0x007d

#endif

/*------------------------------------------------------------------------
* HWD_MC_MODECFG_1 Register Bit definitions
*------------------------------------------------------------------------*/
#define HWD_MC_DSP_ICE_EN_N                   0x8000
#define HWD_MC_RF_ON7_SEL                     0x2000
#define HWD_MC_RF_ON6_SEL                     0x1000
#define HWD_MC_RF_ON5_SEL                     0x0800
#define HWD_MC_RF_ON4_SEL                     0x0400
#define HWD_MC_RF_ON3_SEL                     0x0200
#define HWD_MC_UIM_EN                         0x0080


#define HWD_MC_UART0_4PIN                     0x0004
#define HWD_MC_UART1_EN                       0x0008
#define HWD_MC_UART1_4PIN                     0x0010
#define HWD_MC_UART2_EN                       0x0020
#define HWD_MC_UART2_4PIN                     0x0040

/*------------------------------------------------------------------------
* HWD_MC_MODECFG_2 Register Bit definitions
*------------------------------------------------------------------------*/
#define HWD_MC2_ECLCK_SEL                     0x0004

/*------------------------------------------------------------------------
* HWD_SYNC_FLASH_CS0 definitions
*------------------------------------------------------------------------*/
#define HWD_CS0_USE_2xARM_CLOCK    (1 << 1)    /* cp_clk_out = 2x cg_arm_clk */
#define HWD_CS0_SYNC_BURST_FLASH   (1 << 2)    /* enables clock for synchronious burst flash */
#define HWD_CS0_BURST_FLASH_EN     (1 << 3)    /* enables burst flash mode */
#define HWD_CS0_4WORDS_BURST_SIZE  (0 << 4)    /* bits 4:5 define burst size: 00 - 4 words page */
#define HWD_CS0_8WORDS_BURST_SIZE  (1 << 4)    /* bits 4:5 define burst size: 01 - 8 words page */
#define HWD_CS0_16WORDS_BURST_SIZE (2 << 4)    /* bits 4:5 define burst size: 01 - 8 words page */
#define HWD_CS0_CONT_BURST_SIZE    (3 << 4)    /* bits 4:5 define burst size: 11 - Continuous page */
#define HWD_CS0_EXTRA_WAIT_ST      (0 << 6)    /* bits 6:7 define extra wait states for CS 0
                                                  for asynchronous page mode */

/*------------------------------------------------------------------------
* HWD_MISC_EBI_CFG definitions
*------------------------------------------------------------------------*/
#define HWD_READY_POL           (0)       /* Ready polarity WAIT/READY */

/*------------------------------------------------------------------------
* HWD_EBI_CS definitions
*------------------------------------------------------------------------*/
#define HWD_CP_CS_SZ_16M         0x00   /* each chip-select is 16MB */
#define HWD_CP_CS_SZ_32M         0x01   /* each chip-select is 32MB */
#define HWD_CP_CS_SZ_64M         0x02   /* each chip-select is 64MB */
#define HWD_CP_CS_SZ_128M        0x03   /* each chip-select is 128MB */
#define HWD_CP_CS_SZ_256M        0x04   /* each chip-select is 256MB */
#define HWD_CP_ADD23_ON          (1<<3) /* GPIO4 is ADDR[23] */
#define HWD_CP_ADD24_ON          (1<<4) /* GPIO34 is ADDR[24] */
#define HWD_CP_ADD25_ON          (1<<5) /* GPIO36 is ADDR[25] */
#define HWD_CP_ADD26_ON          (1<<6) /* GPIO40 is ADDR[26] */
#define HWD_CP_CLK_ON            (1<<7) /* GPIO 15 is Clock E for Burst transactions */

/*------------------------------------------------------------------------
* HWD_IRAM_MASK definitions
* Generate the IRAMWF interrupt when a write operation has been executed to address range:
*------------------------------------------------------------------------*/
#define HWD_MASK_SECTOR_0        (1<<0) /* 0000_0000 to 0000_1000 of IRAM */
#define HWD_MASK_SECTOR_1        (1<<1) /* 0000_1000 to 0000_2000 of IRAM */
#define HWD_MASK_SECTOR_2        (1<<2) /* 0000_2000 to 0000_3000 of IRAM */
#define HWD_MASK_SECTOR_3        (1<<3) /* 0000_3000 to 0000_4000 of IRAM */
#define HWD_MASK_SECTOR_4        (1<<4) /* 0000_4000 to 0000_5000 of IRAM */
#define HWD_MASK_SECTOR_5        (1<<5) /* 0000_5000 to 0000_6000 of IRAM */
#define HWD_MASK_SECTOR_6        (1<<6) /* 0000_6000 to 0000_7000 of IRAM */
#define HWD_MASK_SECTOR_7        (1<<7) /* 0000_7000 to 0000_8000 of IRAM */

/* here is defined the currently used protection mask */
#define HWD_IRAM_PROTECTION_MASK (HWD_MASK_SECTOR_0)

/*------------------------------------------------------------------------
* HWD_DAI_CTRL Bit definitions
*------------------------------------------------------------------------*/
#define DAI_MIC_EN      0x0001
#define DAI_SPKR_EN     0x0002
#define EXT_CODEC_EN    0x0004
#define FSYNC_MODE0     0x0008
#define FSYNC_MODE1     0x0010
#define FSYNC_MODE2     0x0020
#define INV_CLK_BIT     0x0040
#define INV_VDIFS_BIT   0x0080
#define INV_VDOFS_BIT   0x0100
#define INV_DI          0x0200
#define INV_DO          0x0400
#define DO_EDGE_SEL     0x0800
#define DI_EDGE_SEL     0x1000

/*------------------------------------------------------------------------
* Mixed Signal TX SC CF_TX_COMPPOWER Register Bit definitions
*------------------------------------------------------------------------*/
#define HWD_TXSC_CF_TX_COMPPD  0x0001      /* bit 0 */
#define HWD_TXSC_CF_TX_TUNEPD  0x0002      /* bit 1 */

/*------------------------------------------------------------------------
* Watchdog timeout value in msecs, Max value = 31,999 msecs
*------------------------------------------------------------------------*/
#define HWD_WD_TIMEOUT_VALUE        8000

/*------------------------------------------------------------------------
* CP spare register bit definitions 
*------------------------------------------------------------------------*/
#if ((SYS_ASIC == SA_ROM) || (SYS_ASIC == SA_CBP55))
#define VD_ENABLE_VIT            0x2  /* Viterbi output buffer generates DSPM NMI */
#define VD_RST_ENABLE            0x1  /* Viterbi reset control given to DSPM */
#else
#define DSPM_NMI_ENABLE          0x4  /* Enable NMI */
#endif

/*------------------------------------------------------------------------
* HWD_TXSD_OSC_CTRL Bit definitions (for external 32K)
*------------------------------------------------------------------------*/
#define HWD_TXSD_OSC_BYPASS        0x0001  /* Bypass bit for 32K Osc */
#define HWD_TXSD_OSC_CURRENT_MODE  0x0002  /* Current mode bit for 32K Osc */
#define HWD_TXSD_OSC_POWER_DOWN    0x0004  /* Power bit for 32K Osc */

/*******************
** UIM REGISTER   **
********************/
#define HWD_UIM_CTL              0x0b840000  /* UIM control register */
#define HWD_UIM_TX_BUF           0x0b840004  /* UIM Tx Data buffer */
#define HWD_UIM_RX_BUF           0x0b840008  /* UIM Rx data buffer */
#define HWD_UIM_RX_STAT          0x0b84000c  /* UIM Rx status */
#define HWD_UIM_RX_WD_CNT        0x0b840010  /* UIM Rx Buffer word count */
#define HWD_UIM_WWT_CTL          0x0b840014  /* UIM work waiting time control register */
#define HWD_UIM_WWT_END_HI       0x0b840018  /* UIM work waiting time end count, high */
#define HWD_UIM_WWT_END_LO       0x0b84001c  /* UIM work waiting time end count, low */
#define HWD_UIM_TX_INT_MSK       0x0b840020  /* UIM Tx interrupt mask */
#define HWD_UIM_TX_STAT          0x0b840024  /* UIM Tx interrupt status */
#define HWD_UIM_BUF_TRIG         0x0b840028  /* UIM Buffer trigger level */
#define HWD_UIM_TX_WD_CNT        0x0b84002c  /* UIM Buffer word count during Tx mode*/

/*------------------------------------------------------------------------
* UIM control register bit definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_VLT_SEL          0x4000   /* bit 14 UIM voltage select (0==3.0 V, 1==1.8V */
#define HWD_UIM_PWR              0x1000   /* bit 12 UIM Power */
#define HWD_UIM_RSTN             0x0800   /* bit 11 UIM card reset */
#define HWD_UIM_RX_TEST          0x0400   /* bit 10 Rx test mode */
#define HWD_UIM_TX_TEST          0x0200   /* bit 9 Tx test mode */
#define HWD_UIM_SFT_RSTN         0x0100   /* bit 8 UIM soft reset */
#define HWD_UIM_TX_EN            0x0080   /* bit 7 Tx mode enable */
#define HWD_UIM_RX_EN            0x0040   /* bit 6 Rx mode enable */
#define HWD_UIM_SPEED_BITS       0x0030   /* bit 5:4 00 for default speed(UIM_CLK/372) */
#define HWD_UIM_SPEED_ENHANCE_64 0x0010   /* 01 for UIM_CLK/64 */
#define HWD_UIM_SPEED_ENHANCE_32 0x0020   /* 1x for UIM_CLK/32 */
#define HWD_UIM_CK_OFF           0x0008   /* Inactive level of UIM_CLK */
#define HWD_UIM_CKE              0x0004   /* clock enable   */
#define HWD_UIM_CONV             0x0002   /* direct convention */
#define HWD_UIM_AUTO             0x0001   /* automatic direct convention */

/*------------------------------------------------------------------------
* UIM Rx status register bit definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_RX_GT_TRIG       0x0020   /* buffer level is equal to or below UIM_RX_BUF_TRIG */
#define HWD_UIM_RX_FRERR         0x0010   /* Rx framing error */
#define HWD_UIM_RX_OVRERR        0x0008   /* Rx overrun */
#define HWD_UIM_RX_PERR          0x0004   /* Rx parity error   */
#define HWD_UIM_INV              0x0002   /* inverse convention */
#define HWD_UIM_WWT_EXP          0x0001   /* WWT timer expired */

/*------------------------------------------------------------------------
* UIM WWT control register bit definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_WWT_EN           0x0001   /* WWT timer enable */

/*------------------------------------------------------------------------
* UIM Tx interrupt mask register bit definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_BUF_TRIG_MSK     0x0002   /* disable trigger level interrupt */
#define HWD_UIM_BUF_EMP_MSK      0x0001   /* disable empty interrupt */

/*------------------------------------------------------------------------
* UIM Tx status register bit definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_TX_PERR          0x0004   /* Tx parity error */
#define HWD_UIM_BUF_LT_TRIG      0x0002   /* buffer level has fallen below UIM_TX_BUF_TRIG  */
#define HWD_UIM_BUF_EMP          0x0004   /* Tx buffer is empty */

/*------------------------------------------------------------------------
* UIM Tx/Rx FIFO buffer size definitions
*------------------------------------------------------------------------*/
#define HWD_UIM_TX_BUF_SIZE      64
#define HWD_UIM_RX_BUF_SIZE      64

/*------------------------------------------------------------------------
* Mixed signal Voice Codec CF_VC_SEL register bit definitions 
*------------------------------------------------------------------------*/
#define HWD_CF_VC_SEL_AUX             0x0001    

/*------------------------------------------------------------------------
* PWM registers bit definitions 
*------------------------------------------------------------------------*/
#if ((SYS_ASIC == SA_RAM) && (SYS_VERSION <= SV_REV_C3))

#define HWD_KPD_BLPWM_BP_EN        0x0010   /* Keypad Backlight PWM bypass enable */
#define HWD_KPD_BLPWM_LEV_MASK     0x000f   /* Keypad Backlight PWM level mask */

#define HWD_LCD_PWM_BP_EN          0x0010   /* LCD Contrast PWM bypass enable */
#define HWD_LCD_PWM_LEV_MASK       0x000f   /* LCD Contrast PWM level mask */

#define HWD_LCD_BLPWM_BP_EN        0x0010   /* LCD Backlight PWM bypass enable */
#define HWD_LCD_BLPWM_LEV_MASK     0x000f   /* LCD Backlight PWM level mask */

#else

#define HWD_KPD_BLPWM_BP_EN        0x0001   /* Keypad Backlight PWM bypass enable */
#define HWD_KPD_BLPWM_BP_LEV       0x0002   /* Keypad Backlight PWM bypass level */
#define HWD_KPD_BLPWM_LEV_MASK     0x000f   /* Keypad Backlight PWM level mask */

#define HWD_LCD_PWM_BP_EN          0x0004   /* LCD Contrast PWM bypass enable */
#define HWD_LCD_PWM_BP_LEV         0x0008   /* LCD Contrast PWM bypass level */
#define HWD_LCD_PWM_LEV_MASK       0x000f   /* LCD Contrast PWM level mask */

#define HWD_LCD_BLPWM_BP_EN        0x0010   /* LCD Backlight PWM bypass enable */
#define HWD_LCD_BLPWM_BP_LEV       0x0020   /* LCD Backlight PWM bypass level */
#define HWD_LCD_BLPWM_LEV_MASK     0x000f   /* LCD Backlight PWM level mask */

#endif

/* CDS4 Revision macros */
#define CDS4_REV_REG   0x3000020
#define CDS4_REVB      0xbb
#define CDS4_REVC      0xc0
 /*for power off issue*/
 #ifdef SYS_OPTION_USB_ENABLE
 #define POWEROFF_MODE 0x12344321
#endif




#endif
