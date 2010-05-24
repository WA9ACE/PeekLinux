#ifndef _IPCMBOX_H_
#define _IPCMBOX_H_


/* Define IPC mailbox names */
#define IPC_MAILBOX_DSPM_ASYN     EXE_MAILBOX_1_ID   
#define IPC_MAILBOX_DSPV_ASYN     EXE_MAILBOX_2_ID      
#define IPC_MAILBOX_CMD           EXE_MAILBOX_3_ID
#ifdef SYS_OPTION_DYNAMIC_CODE_DNLD
#define IPC_MAILBOX_DSPV_APPS     EXE_MAILBOX_4_ID
#else
#define IPC_MAILBOX_DSPV_APPS     IPC_MAILBOX_DSPV_ASYN
#endif
   

/* Macros which set the CP to DSP direct buffer or control mailbox status */

/* Indicate to the DSPM that the CP has finished reading the direct buffer mailbox */
#define IPC_C2M_D_RRDY            (HwdWrite(HWD_MBM_C2M_D_RRDY, HWD_MMB_RHS))

/* Indicate to the DSPM that the CP has finished writing the direct buffer mailbox */
#define IPC_C2M_D_WRDY            (HwdWrite(HWD_MBM_C2M_D_WRDY, HWD_MMB_WHS))

/* Indicate to the DSPM that the CP has finished reading the control mailbox */
#define IPC_C2M_C_RRDY            (HwdWrite(HWD_MBM_C2M_C_RRDY, HWD_MMB_RHS))

/* Indicate to the DSPM that the CP has finished writing the control mailbox */
#define IPC_C2M_C_WRDY            (HwdWrite(HWD_MBM_C2M_C_WRDY, HWD_MMB_WHS))

/* Indicate to the DSPV that the CP has finished reading the mailbox */
#define IPC_C2V_RRDY              (HwdWrite(HWD_MBV_C2V_RRDY, HWD_VMB_RHS))

/* Indicate to the DSPV that the CP has finished writing the mailbox */
#define IPC_C2V_WRDY              (HwdWrite(HWD_MBV_C2V_WRDY, HWD_VMB_WHS))

/* Indicate to the DSPV that the CP has finished reading the fast mailbox */
#define IPC_C2V_F_RRDY            (HwdWrite(HWD_MBV_C2V_ARRDY, HWD_VMB_F_RHS))

/* Macros which test the current DSP to CP mailbox status */

/* Test if the DSPM direct buffer mailbox can be written by the CP */
#define IPC_IS_M2C_D_WRDY         (HwdRead(HWD_CP_ISR0_L) & HWD_INT_M2CDW)

/* Test if the DSPM control mailbox can be written by the CP */
#define IPC_IS_M2C_C_WRDY         (HwdRead(HWD_CP_ISR0_L) & HWD_INT_M2CCW)

/* Test if the DSPM direct buffer mailbox contains data that should be read by the CP */
#define IPC_IS_M2C_D_RRDY         (HwdRead(HWD_CP_ISR0_L) & HWD_INT_M2CDR)

/* Test if the DSPM control mailbox contains data that should be read by the CP */
#define IPC_IS_M2C_C_RRDY         (HwdRead(HWD_CP_ISR0_L) & HWD_INT_M2CCR)

/* Test if the DSPV mailbox can be written by the CP */
#define IPC_IS_V2C_WRDY           (HwdRead(HWD_CP_ISR0_L) & HWD_INT_V2CW)

/* Test if the DSPV mailbox should be read by the CP */
#define IPC_IS_V2C_RRDY           (HwdRead(HWD_CP_ISR0_L) & HWD_INT_V2CR)


/* Macros which clear the DSP to CP mailbox CP status bits */

/* Clear the CP bit indicating the CP may write to the DSPM direct buffer mailbox */
#define IPC_CLR_M2C_D_WRDY        (HwdIntClrL0(HWD_INT_M2CDW))

/* Clear the CP bit indicating the CP may write to the DSPM control mailbox */
#define IPC_CLR_M2C_C_WRDY        (HwdIntClrL0(HWD_INT_M2CCW))

/* Clear the CP bit indicating the CP must read the DSPM direct buffer mailbox */
#define IPC_CLR_M2C_D_RRDY        (HwdIntClrL0(HWD_INT_M2CDR))

/* Clear the CP bit indicating the CP must read the DSPM control mailbox */
#define IPC_CLR_M2C_C_RRDY        (HwdIntClrL0(HWD_INT_M2CCR))

/* Clear the CP bit indicating the CP may write to the DSPV mailbox */
#define IPC_CLR_V2C_WRDY          (HwdIntClrL0(HWD_INT_V2CW))

/* Clear the CP bit indicating the CP may write to the DSPV mailbox */
#define IPC_CLR_V2C_RRDY          (HwdIntClrL0(HWD_INT_V2CR))



#endif
