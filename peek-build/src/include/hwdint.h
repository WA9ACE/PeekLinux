#ifndef _HWDINT_H_
#define _HWDINT_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/

/*---------------------**
** L0 Interrupts (IRQ) **
**---------------------*/

/* Enable L0 interrupts by applying mask to IMR0 register */
#define HwdIntEnableL0(mask) \
   HwdWrite(HWD_CP_IMR0_L, HwdRead(HWD_CP_IMR0_L) & ~(mask))
   
/* Disable L0 interrupts by applying mask to IMR0 register */
#define HwdIntDisableL0(mask) \
   HwdWrite(HWD_CP_IMR0_L, HwdRead(HWD_CP_IMR0_L) | (mask))
   
/* Control L0 interrupts by writing mask to ISR0 register */
#define HwdIntClrL0(mask) \
   HwdWrite(HWD_CP_ISR0_L, (mask))  

/* Determine if a L0 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntL0(mask)     TRUE
#else
   #define HwdIsIntL0(mask) \
      ((HwdRead(HWD_CP_ISR0_L) & (mask))? TRUE : FALSE)  
#endif 
   
/*---------------------**
** L1 Interrupts (IRQ) **
**---------------------*/

/* Enable L1 interrupts by applying mask to IMR1 register */
#define HwdIntEnableL1(mask) \
   HwdWrite(HWD_CP_IMR1_L, HwdRead(HWD_CP_IMR1_L) & ~(mask))
   
/* Disable L1 interrupts by applying mask to IMR1 register */
#define HwdIntDisableL1(mask) \
   HwdWrite(HWD_CP_IMR1_L, HwdRead(HWD_CP_IMR1_L) | (mask))
   
/* Control L1 interrupts by writing mask to ISR1 register */
#define HwdIntClrL1(mask) \
   HwdWrite(HWD_CP_ISR1_L, (mask))  

/* Determine if a L1 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntL1(mask)    TRUE
#else
  #define HwdIsIntL1(mask) \
   ((HwdRead(HWD_CP_ISR1_L) & (mask))? TRUE : FALSE)  
#endif  
 
/*---------------------**
** L2 Interrupts (IRQ) **
**---------------------*/

/* Enable L2 interrupts by applying mask to IMR2 register */
#define HwdIntEnableL2(mask) \
   HwdWrite(HWD_CP_IMR2_L, HwdRead(HWD_CP_IMR2_L) & ~(mask))
   
/* Disable L2 interrupts by applying mask to IMR2 register */
#define HwdIntDisableL2(mask) \
   HwdWrite(HWD_CP_IMR2_L, HwdRead(HWD_CP_IMR2_L) | (mask))
   
/* Control L2 interrupts by writing mask to ISR2 register */
#define HwdIntClrL2(mask) \
   HwdWrite(HWD_CP_ISR2_L, (mask))  

/* Determine if a L2 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntL2(mask)    TRUE
#else
  #define HwdIsIntL2(mask) \
   ((HwdRead(HWD_CP_ISR2_L) & (mask))? TRUE : FALSE)  
#endif  

/*---------------------**
** H0 Interrupts (FIQ) **
**---------------------*/

/* Enable H0 interrupts by applying mask to IMR H0 register */
#define HwdIntEnableH0(mask) \
   HwdWrite(HWD_CP_IMR0_H, HwdRead(HWD_CP_IMR0_H) & ~(mask))
   
/* Disable H0 interrupts by applying mask to IMR H0 register */
#define HwdIntDisableH0(mask) \
   HwdWrite(HWD_CP_IMR0_H, HwdRead(HWD_CP_IMR0_H) | (mask))
   
/* Control H0 interrupts by writing mask to ISR H0 register */
#define HwdIntClrH0(mask) \
   HwdWrite(HWD_CP_ISR0_H, (mask)) 

/* Determine if a H0 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntH0(mask)   TRUE
#else
   #define HwdIsIntH0(mask) \
      ((HwdRead(HWD_CP_ISR0_H) & (mask))? TRUE : FALSE)  
#endif

/*---------------------**
** H1 Interrupts (FIQ) **
**---------------------*/

/* Enable H1 interrupts by applying mask to IMR H1 register */
#define HwdIntEnableH1(mask) \
   HwdWrite(HWD_CP_IMR1_H, HwdRead(HWD_CP_IMR1_H) & ~(mask))
   
/* Disable H1 interrupts by applying mask to IMR H1 register */
#define HwdIntDisableH1(mask) \
   HwdWrite(HWD_CP_IMR1_H, HwdRead(HWD_CP_IMR1_H) | (mask))
   
/* Control H1 interrupts by writing mask to ISR H1 register */
#define HwdIntClrH1(mask) \
   HwdWrite(HWD_CP_ISR1_H, (mask)) 

/* Determine if a H1 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntH1(mask)   TRUE
#else
   #define HwdIsIntH1(mask) \
      ((HwdRead(HWD_CP_ISR1_H) & (mask))? TRUE : FALSE)  
#endif

/*---------------------**
** H2 Interrupts (FIQ) **
**---------------------*/

/* Enable H2 interrupts by applying mask to IMR H2 register */
#define HwdIntEnableH2(mask) \
   HwdWrite(HWD_CP_IMR2_H, HwdRead(HWD_CP_IMR2_H) & ~(mask))
   
/* Disable H2 interrupts by applying mask to IMR H2 register */
#define HwdIntDisableH2(mask) \
   HwdWrite(HWD_CP_IMR2_H, HwdRead(HWD_CP_IMR2_H) | (mask))
   
/* Control H2 interrupts by writing mask to ISR H2 register */
#define HwdIntClrH2(mask) \
   HwdWrite(HWD_CP_ISR2_H, (mask)) 

/* Determine if a H2 interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntH2(mask)   TRUE
#else
   #define HwdIsIntH2(mask) \
      ((HwdRead(HWD_CP_ISR2_H) & (mask))? TRUE : FALSE)  
#endif

/*------------------------**
** System Time Interrupts **
**-------------------------*/

/* FOR ST: 0 is masked, 1 is enabled
   Disable ST interrupts by applying mask to ST_CPINT_MASK register */

#define HwdStIntDisable(mask) \
   HwdWrite(HWD_ST_CPINT_MASK,HwdRead(HWD_ST_CPINT_MASK) & ~(mask))
   
/* Enable ST interrupts by applying mask to ST_CPINT_MASK register */
#define HwdStIntEnable(mask) \
   HwdWrite(HWD_ST_CPINT_MASK, HwdRead(HWD_ST_CPINT_MASK) | (mask))
   
/* Clear Status Register */
#define HwdStIntClr(mask) \
   HwdWrite(HWD_ST_CPINT_CLR, (mask))  

#define StIntEnable(mask) \
   SysIntDisable(SYS_IRQ_INT);\
   HwdStIntClr(mask);\
   HwdStIntEnable(mask);\
   SysIntEnable(SYS_IRQ_INT);

#define StReadIntMask() \
	HwdRead(HWD_ST_CPINT_MASK)

/* Determine if a ST interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdStIsInt(mask)     TRUE
#else
   #define HwdStIsInt(mask) \
      ((HwdRead(HWD_ST_CPINT_SRC) & (mask))? TRUE : FALSE)  
#endif 
   
/*--------------------------------------------------**
** Group GPIO Interrupts - mask and polarity config **
**---------------------------------------------------*/

/* Enable Gpio interrupts by applying mask to HWD_INT_MSK_REG register 
	bits 0:7 can be set to 0 to mask the incoming interrupt from group_8bit
	bits 8:14 can be set to 0 to mask the incoming interrupt from group_7bit
	bit 15 masks all incoming interrupts  */
#define HwdGpioIntEnable(mask) \
   HwdWrite(HWD_INT_MSK_REG, HwdRead(HWD_INT_MSK_REG) & ~(mask))
   
/* Disable Gpio interrupts by applying mask to HWD_INT_MSK_REG register */
#define HwdGpioIntDisable(mask) \
   HwdWrite(HWD_INT_MSK_REG, HwdRead(HWD_INT_MSK_REG) | (mask))
   
/* Invert Gpio interrupt polarity by setting bits to 1 in HWD_IN_POL_REG register */
#define HwdGpioIntInvertPol(mask) \
   HwdWrite(HWD_IN_POL_REG, (HwdRead(HWD_IN_POL_REG) | (mask))  

/* Set Gpio interrupt polarity to normal by setting bits to 0 in HWD_IN_POL_REG register */
#define HwdGpioIntNormalPol(mask) \
   HwdWrite(HWD_IN_POL_REG, (HwdRead(HWD_IN_POL_REG) & ~(mask))  

/* Control Gpio polarity interrupts by writing mask to HWD_IN_POL_REG register */
#define HwdGpioIntPolInit(mask) \
   HwdWrite(HWD_IN_POL_REG, (mask))  

/* Determine if a Gpio interrupt defined in mask is active */
#ifdef SYS_TARGET_SIM
   #define HwdIsIntGpio(mask)    TRUE
#else
  #define HwdIsIntGpio(mask) \
   ((HwdRead(HWD_INT_MSK_REG) & (mask))? TRUE : FALSE)  
#endif  





#endif
