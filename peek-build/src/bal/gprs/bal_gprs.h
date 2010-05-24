#ifndef __BTC_GPRS_H_20080222__
#define __BTC_GPRS_H_20080222__

#ifdef __cplusplus
extern "C" {
#endif

#include "ata_aci_send.h"
#include "gprs.h"
#include "custom.h"
#include "dti_conn_mng.h"
#include "p_8010_142_smreg_sap.h"

#include "gaci.h"


/* GPRS events -----------------------------------------------------*/
#define E_BAL_GPRS_S_CNTXT       0x00000001
#define E_BAL_GPRS_S_ATT         0x00000002
#define E_BAL_GPRS_S_ACT         0x00000004
#define E_BAL_GPRS_R_ACT         0x00000008
#define E_BAL_GPRS_S_DATA        0x00000010
#define E_BAL_GPRS_R_DATA        0x00000020
#define E_BAL_GPRS_S_QOS         0x00000040
#define E_BAL_GPRS_R_QOS         0x00000080
#define E_BAL_GPRS_S_QOS_MIN     0x00000100
#define E_BAL_GPRS_S_CLASS       0x00000200
#define E_BAL_GPRS_S_PDPADDR     0x00000400
#define E_BAL_GPRS_S_AUTORESP    0x00000800
#define E_BAL_GPRS_S_ANS         0x00001000
#define E_BAL_GPRS_R_ANS         0x00002000
#define E_BAL_GPRS_S_EREP        0x00004000
#define E_BAL_GPRS_R_EREP_RJ     0x00008000
#define E_BAL_GPRS_R_EREP_ATT    0x00010000
#define E_BAL_GPRS_R_EREP_ACT    0x00020000
#define E_BAL_GPRS_R_EREP_DEACT  0x00040000
#define E_BAL_GPRS_R_EREP_CLASS  0x00080000
#define E_BAL_GPRS_R_REG         0x00200000
#define E_BAL_GPRS_S_SMSSERV     0x00400000
#define E_BAL_GPRS_OK            0x00800000
#define E_BAL_GPRS_ERROR         0x01000000
#define E_BAL_GPRS_CONNECT       0x02000000
#define E_BAL_GPRS_ERR_CONNECT   0x04000000
#define E_BAL_GPRS_COUNTER	     0x08000000

#define E_BAL_GPRS_ALL_EVENTS	 0x0FFFFFFF


/* GPRS Parameter --------------------------------------------------*/

typedef int T_BAL_EVENT;

/* Data Counter */
typedef struct
{
	UBYTE c_id;
	ULONG octets_uplink;				/* No. of bytes transferred in uplink */
	ULONG octets_downlink;				/* No. of bytes transferred in downlink */
	ULONG packets_uplink;				/* No. of packets transferred in uplink */
	ULONG packets_downlink;				/* No. of packets transferred in uplink */
} T_BAL_GPRS_COUNTER;

/* Structure to store cell reselection information. */
typedef struct
{
	T_CGREG_STAT stat;
	USHORT       lac; 
	USHORT       ci;
} T_BAL_GPRS_CELL_STRUCT;

typedef struct
{
	T_P_CGREG_STAT stat;
	USHORT       lac; 
	USHORT       ci;
} T_BAL_GPRS_CELL_STRUCT_P;

typedef struct        
{
  USHORT               id;                  /* context id                  */
  T_PDP_CONTEXT_INTERNAL      data;         /* context data                */
  BOOL                 activated;           /* context mode                */
  char                 *L2P;                /* layer 2 protocol            */
} T_BAL_GPRS_CONTEXT;

typedef struct        
{
  USHORT               *ids;                /* context ids                 */
  BOOL                 state;               /* activation state            */
} T_BAL_GPRS_CNTXT_ACT;

typedef struct        
{
  USHORT              *ids;                 /* context ids                 */
  char                *L2P;                 /* layer 2 protocol            */
} T_BAL_GPRS_CNTXT_L2P;

typedef struct        
{
  T_CGEREP_MODE        mode;               /* event reporting mode        */
  T_CGEREP_BFR         bfr;                /* effect control on buffered events */
} T_BAL_GPRS_EVENTREP;

typedef struct
{
  T_BAL_GPRS_CONTEXT   *contexts;          /* list of defined contexts    */
  USHORT               numContexts;        /* number of defined contexts  */
  T_CGATT_STATE        attached;           /* Now T_CGATT_STATE instead of BOOL*/
  BOOL                 autoResponse;       /* auto response mode          */
  T_BAL_GPRS_EVENTREP  eventRep;           /* event reporting             */
  T_CGCLASS_CLASS      tclass;             /* GPRS class mode             */

  union
  {
      T_CGREG_STAT      regState;          /* network registration state  */
      T_P_CGREG_STAT	p_reg_State;       /* present network registration state  */
  }Reg_state;

  T_CGSMS_SERVICE      SMSService;         /* service to use for SMS      */
  USHORT               link_id;            /* link identifier for data transfer  */
  T_BAL_GPRS_COUNTER   counter;	           /* Data uplink/downlink counter */
  USHORT			   lac;		           /* Location area code */
  USHORT			   ci;			       /* Cell id */
} T_BAL_GPRS_DATA;

typedef union
{
  SHORT                cID;
  T_BAL_GPRS_CONTEXT   context;
  T_BAL_GPRS_CNTXT_ACT contextAct;
  T_BAL_GPRS_CNTXT_L2P contextL2P;
  BOOL                 mode;
  T_CGATT_STATE			attached;      
  T_CGCLASS_CLASS      tclass;
  T_BAL_GPRS_EVENTREP  eventRep;
  T_EVENT_REJECT       erepReject;
  T_EVENT_ACT          erepAct;
  T_BAL_GPRS_CELL_STRUCT cell;	
  T_CGSMS_SERVICE      service;
  USHORT               link_id;
  T_ACI_AT_CMD         cmdId;
  T_BAL_GPRS_COUNTER	counter;	
} T_BAL_GPRS_PARA;


/* MFW Control Block for GPRS --------------------------------------*/
typedef struct
{
  T_BAL_EVENT          emask;          /* events of interest        */
  T_BAL_EVENT          event;          /* current event             */
  T_BAL_GPRS_DATA      data;           /* GPRS data                 */
} T_BAL_GPRS;


/* ATA internal functions ------------------------------------------*/

void      gprs_init   ( void );
void      gprs_exit   ( void );

void      gprs_ok(T_ACI_AT_CMD cmdId);
void      gprs_error(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err);
void      gprs_connect(SHORT cId);
void      gprs_err_connect(SHORT cId);
void      gprs_signal ( T_BAL_EVENT event, void *para );

/* BAL GPRS Interface Functions -------------------------------------*/

int gprs_definePDPContext ( SHORT cid, T_PDP_CONTEXT *inputCtxt );
int gprs_setQOS           ( SHORT cid ,T_PS_qos *inputQos );
int gprs_setQOSMin        ( SHORT cid ,T_PS_qos *inputQos);
int gprs_attach           ( T_CGATT_STATE state);
int gprs_attach_abort     ( void );
int gprs_contextActivation( T_CGACT_STATE state, SHORT *cids);
int gprs_setData          ( char *L2P, SHORT *cids);
int gprs_showPDPAddress   ( SHORT *cids, T_NAS_ip *pdp_address);
int gprs_setAutoResponse  ( T_CGAUTO_N mode);
int gprs_ManualResponse   ( USHORT response, char *l2p, SHORT cid);
int gprs_setClass         ( T_CGCLASS_CLASS m_class);
int gprs_setEventReporting( T_CGEREP_MODE   mode, T_CGEREP_BFR bfr);
int gprs_setServiceSMS    ( T_CGSMS_SERVICE service);
T_CGSMS_SERVICE gprs_getServiceSMS(void);
int gprs_counter(BOOL reset_counter);
int gprs_counter_abort(void);
T_CGREG_STAT gprs_status(void);
T_P_CGREG_STAT p_gprs_status(void);

#ifdef __cplusplus
}
#endif


#endif //__BTC_GPRS_H_20080222__
