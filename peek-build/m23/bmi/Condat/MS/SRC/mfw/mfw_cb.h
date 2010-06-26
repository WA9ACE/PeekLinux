/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_cb.h       $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 14.10.98                     $Modtime:: 27.10.98 17:32   $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

	PURPOSE :  Types definitions for MMI framework call backs distribution

	$History:: mfw_cm.h                                              $

        May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
        Description: Submission of ACI and SAP changes for Imaging ERs 
        Solution: Using the mechanism where MFW can receive primitives without
        ACI to know about it.

	x0018858 24 Mar, 2005 MMI-FIX-11321
	Issue Description :- If the inbox is full and a new message is receivd, no indication is received.
	Fix: Added support for handling indication when message box is full and
	there is a pending message.


    .
*/

#ifndef DEF_MFW_CB_HEADER
#define DEF_MFW_CB_HEADER

// May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
#if defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)

/*Defining the premitive*/
#define MSL_MMI_IND      0x110   /*Specifying hex value */

typedef struct
{
  U32                       handle;                   /*<  0:  4> Basic Element                                      */
  U32                       appData;                  /*<  4:  4> Basic Element                                      */
  U32                       message;                  /*<  8:  4> Basic Element                                      */
  U32                       ucp;                      /*< 12:  4> Basic Element                                      */
} T_MSL_MMI_IND;



#endif //defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)
/*
 * not yet defined in aci_cmh.h
 */
#define CME_ERR_SimPin2Req  35
#define CME_ERR_SimPuk2Req  36
//	x0018858 24 Mar, 2005 MMI-FIX-11321
extern UBYTE sms_check_message_pending(void);
// May 10, 2006    REF: DRT OMAPS00075829  xdeepadh
#if defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)
GLOBAL BOOL mslPrimHandler (USHORT opc, void * data) ;
GLOBAL  void Mslil_msl_mmi_ind ( T_MSL_MMI_IND *msl_mmi_ind); /*a0393213 compiler warning removal - removed const*/
/*a0393213 cphs rel4*/
#ifdef FF_CPHS_REL4
GLOBAL void rAT_PercentCFIS ( T_ACI_CFIS_CFU *cfis );
#endif
#endif //#if defined(FF_MMI_FILEMANAGER)||defined(FF_MMI_CAMERA_APP)
#endif
