

/*****************************************************************************
 
        .
*****************************************************************************/

#ifndef HWDUSBDEV_H
#define HWDUSBDEV_H

#ifdef SYS_OPTION_USB_ENABLE    /* build only if usb is enabled */

#define SYS_DEBUG_USB
#define SYS_DEBUG_USB_LOOPBACK

#include "sysdefs.h"

/*------------------------------------------------------------------------
 * macros to translate byte order for words and dwords
 *------------------------------------------------------------------------*/

    /* convert a ARM uint16 to USB uint16 */
#define HwdUsbCvt16Arm( Arm16 ) (uint16)( (((uint16)Arm16 & 0x00FF) << 8) | \
                                          (((uint16)Arm16 & 0xFF00) >> 8) )

    /* convert a USB uint16 to ARM uint16 */
#define HwdUsbCvt16Usb( Usb16 ) HwdUsbCvt16Arm( (Usb16) )

    /* convert a ARM uint32 to USB uint32 */
#define HwdUsbCvt32Arm( Arm32 ) (uint32)( (((uint32)Arm32 & 0x000000FF) << 24) | \
                                          (((uint32)Arm32 & 0x0000FF00) << 8)  | \
                                          (((uint32)Arm32 & 0x00FF0000) >> 8)  | \
                                          (((uint32)Arm32 & 0xFF000000) >> 24) )

    /* convert a USB uin32 to ARM uint32 */
#define HwdUsbCvt32Usb( Usb32 ) HwdUsbCvt32Arm( Usb32 )

/*------------------------------------------------------------------------
 * ENDPOINT definitions
 *------------------------------------------------------------------------*/

    /*
     *  Users of each EP should use a #define to rename their HwdUsbEPT
     *  id to something more meaningful, like:
     *  #define HWD_USB_ETS_EP  HWD_USB_EP_1
     */
typedef enum {
    HWD_USB_EP_1,
    HWD_USB_EP_FIRST = HWD_USB_EP_1,
    HWD_USB_EP_2,
    HWD_USB_EP_3,
    HWD_USB_EP_4,
    HWD_USB_EP_5,
    HWD_USB_EP_6,
    HWD_USB_EP_LAST = HWD_USB_EP_6
} HwdUsbEPT;

    /* symbolic constants for flushing of EPs */
typedef enum {
    HWD_USB_EP_FLUSH_RX,
    HWD_USB_EP_FLUSH_TX,
    HWD_USB_EP_FLUSH_RXTX
} HwdUsbEPFlushT;

/*------------------------------------------------------------------------
 * Endpoint EVENT definitions
 *------------------------------------------------------------------------*/
    /*
     *  Endpoint data and status event id. Event notification may contain
     *  more than one bit set
     */
#define HWD_USB_EP_DATARCVD_EVT     0x0001  /* Rx FIFO full */
#define HWD_USB_EP_DATASENT_EVT     0x0002  /* Tx data sent */
#define HWD_USB_EP_FEATURE_EVT      0x0004  /* EP feature notification */

    /* typedef for event notification; bitmask as above */
typedef uint16  HwdUsbEPEventT;

    /* typedef for info related to events */
typedef struct {
    uint8   Stalled;    /* 1 == EP is stalled */
} HwdUsbEPEventDataT;

/*EP2will be used by ETS AND U-disk, now we support up to 3 applications */
typedef enum
{
    HWD_USB_MODEM_ETS,
    HWD_USB_MODEM_UDISK,
    HWD_USB_INTERF_FUNCTION,
    HWD_USB_INTERF_APP_MAX_NUM
}HwdUsbInterfaceT;
/*------------------------------------------------------------------------
 * Device EVENT definitions
 *------------------------------------------------------------------------*/
    /*
     *  Device events id
     */
#define HWD_USB_DEV_SETCONF_EVT     0x0001  /* new configuration set */
#define HWD_USB_DEV_SETINTF_EVT     0x0002  /* new interface set */
#define HWD_USB_DEV_FEATURE_EVT     0x0004  /* change in wakeup mode */
#define HWD_USB_DEV_RESUME_EVT      0x0008  /* resume event */
#define HWD_USB_DEV_SUSPEND_EVT     0x0010  /* suspend event */
#define HWD_USB_DEV_RESET_EVT       0x0020  /* reset received */
#define HWD_USB_DEV_SOF_EVT         0x0040  /* start of frame received */

    /* typedef for event notification; bitmask as above */
typedef uint16  HwdUsbDevEventT;

    /* typedef for info related to events */
typedef struct {
    uint8   ConfigVal;  /* current Configuration */
    uint8   IntfVal;    /* current Interface */
    uint8   ASVal;      /* alternate settings */
    uint16  Timestamp;  /* timestamp value (low 11bits only) */
} HwdUsbDevEventDataT;

/*------------------------------------------------------------------------
 *  exported functions
 *------------------------------------------------------------------------*/
extern void                 HwdUsbInit( void ); 
extern bool                 HwdUsbCheckCable( void ); 

extern bool                 HwdUsbIsEnabled( void );

extern HwdUsbDevEventT      HwdUsbStatus( HwdUsbDevEventDataT *EvtData );
extern HwdUsbEPEventT       HwdUsbEPStatus( HwdUsbEPT LogEP, HwdUsbEPEventDataT *EvtData );

extern uint16               HwdUsbEPRead( HwdUsbEPT LogEP, uint8 *Data, uint16 Size );
extern uint16               HwdUsbEPRxSize( HwdUsbEPT LogEP );
extern uint16               HwdUsbEPWrite( HwdUsbEPT LogEP, uint8 *Data, uint16 Size );
extern uint16               HwdUsbEPTxSize( HwdUsbEPT LogEP );
extern uint16               HwdUsbEPAppend( HwdUsbEPT LogEP, uint8 *DataPtr, uint16 Size );
extern void                 HwdUsbEPFlush( HwdUsbEPT LogEP, HwdUsbEPFlushT Dir );
extern void                 HwdUsbEPForceTX( HwdUsbEPT LogEP );

extern void                 HwdUsbResumeFromSleep( void );
extern void                 HwdUsbXcvrClose( void );

extern void                 HwdUsbLisr( uint32 Unused );

extern void                 HwdUsbDevEnable(void);
extern void                 HwdUsbDevDisable(void);
extern void                 HwdUsbIntDisable(void);
extern void                 HwdUsbSetInterface( HwdUsbInterfaceT  Interf );
extern HwdUsbInterfaceT    HwdUsbGetConfig(void);

#endif /* SYS_OPTION_USB_ENABLE */
#endif /* HWDUSBDEV_H */


