/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_gprs.h      $|
| $Author: KGT                          $Revision:: 1               $|
| CREATED: 13.02.2001                   $Modtime::  13.02.2001      $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_GPRS

   PURPOSE : This modul contains definition needed for the GPRS 
             management in MFW.
             
   $History: Mfw_gprs.h

	Feb 24, 2006 REF:DR OMAPS00068976 x0035544
	Description: GPRS attach icon - MMI follow up issue of OMAPS00051034
	solution: Added the structure variable p_reg_State to the union and named it
	as Reg_state. This structure stores the network registration status information
	given by qAT_percentCREG().
	
	15/05/2003 - SPR#1983 - SH - Updated to latest from 1.6.3 version.

   $End
*/

#ifndef DEF_MFW_GPRS_HEADER
#define DEF_MFW_GPRS_HEADER


/* GPRS events -----------------------------------------------------*/
#define E_MFW_GPRS_S_CNTXT       0x00000001
#define E_MFW_GPRS_S_ATT         0x00000002
#define E_MFW_GPRS_S_ACT         0x00000004
#define E_MFW_GPRS_R_ACT         0x00000008
#define E_MFW_GPRS_S_DATA        0x00000010
#define E_MFW_GPRS_R_DATA        0x00000020
#define E_MFW_GPRS_S_QOS         0x00000040
#define E_MFW_GPRS_R_QOS         0x00000080
#define E_MFW_GPRS_S_QOS_MIN     0x00000100
#define E_MFW_GPRS_S_CLASS       0x00000200
#define E_MFW_GPRS_S_PDPADDR     0x00000400
#define E_MFW_GPRS_S_AUTORESP    0x00000800
#define E_MFW_GPRS_S_ANS         0x00001000
#define E_MFW_GPRS_R_ANS         0x00002000
#define E_MFW_GPRS_S_EREP        0x00004000
#define E_MFW_GPRS_R_EREP_RJ     0x00008000
#define E_MFW_GPRS_R_EREP_ATT    0x00010000
#define E_MFW_GPRS_R_EREP_ACT    0x00020000
#define E_MFW_GPRS_R_EREP_DEACT  0x00040000
#define E_MFW_GPRS_R_EREP_CLASS  0x00080000
#define E_MFW_GPRS_R_REG         0x00200000
#define E_MFW_GPRS_S_SMSSERV     0x00400000
#define E_MFW_GPRS_OK            0x00800000
#define E_MFW_GPRS_ERROR         0x01000000
#define E_MFW_GPRS_CONNECT       0x02000000
#define E_MFW_GPRS_ERR_CONNECT   0x04000000
#define E_MFW_GPRS_COUNTER			0x08000000

#define E_MFW_GPRS_ALL_EVENTS		0x0FFFFFFF


/* GPRS Parameter --------------------------------------------------*/

/* Data Counter */
typedef struct
{
	UBYTE c_id;
	ULONG octets_uplink;				/* No. of bytes transferred in uplink */
	ULONG octets_downlink;				/* No. of bytes transferred in downlink */
	ULONG packets_uplink;				/* No. of packets transferred in uplink */
	ULONG packets_downlink;				/* No. of packets transferred in uplink */
} T_MFW_GPRS_COUNTER;

/* Structure to store cell reselection information. */
typedef struct
{
	T_CGREG_STAT stat;
	USHORT       lac; 
	USHORT       ci;
} T_MFW_GPRS_CELL_STRUCT;

typedef struct
{
	T_P_CGREG_STAT stat;
	USHORT       lac; 
	USHORT       ci;
} T_MFW_GPRS_CELL_STRUCT_P;

typedef struct        
{
#ifdef FF_2TO1_PS
  U8                     id;           /* context id                  */
  T_PDP_CONTEXT_INTERNAL data;         /* context data                */
#else
  USHORT               id;           /* context id                  */
  T_PDP_CONTEXT_INTERNAL      data;         /* context data                */
#endif
  BOOL                 activated;    /* context mode                */
  char                 *L2P;          /* layer 2 protocol            */
} T_MFW_GPRS_CONTEXT;

typedef struct        
{
  USHORT               *ids;         /* context ids                 */
  BOOL                 state;        /* activation state            */
} T_MFW_GPRS_CNTXT_ACT;

typedef struct        
{
  USHORT              *ids;          /* context ids                 */
  char                *L2P;          /* layer 2 protocol            */
} T_MFW_GPRS_CNTXT_L2P;

typedef struct        
{
  T_CGEREP_MODE        mode;         /* event reporting mode        */
  T_CGEREP_BFR         bfr;          /* effect control on buffered events */
} T_MFW_GPRS_EVENTREP;

typedef struct
{
  T_MFW_GPRS_CONTEXT   *contexts;    /* list of defined contexts    */
  USHORT               numContexts;  /* number of defined contexts  */
  T_CGATT_STATE           attached;    /* Now T_CGATT_STATE instead of BOOL*/
  BOOL                 autoResponse; /* auto response mode          */
  T_MFW_GPRS_EVENTREP  eventRep;     /* event reporting             */
  T_CGCLASS_CLASS      tclass;        /* GPRS class mode             */
//x0035544 Feb 23, 2006 DR:OMAPS00068976
  union
  	{
  T_CGREG_STAT         regState;     /* network registration state  */
  T_P_CGREG_STAT	p_reg_State;/* present network registration state  */
  	}Reg_state;
  T_CGSMS_SERVICE      SMSService;   /* service to use for SMS      */
  USHORT               link_id;      /* link identifier for data transfer  */
  T_MFW_GPRS_COUNTER	counter;	/* Data uplink/downlink counter */
  USHORT				lac;		/* Location area code */
  USHORT				ci;			/* Cell id */
} T_MFW_GPRS_DATA;

typedef union
{
  SHORT                cID;
  T_MFW_GPRS_CONTEXT   context;
  T_MFW_GPRS_CNTXT_ACT contextAct;
  T_MFW_GPRS_CNTXT_L2P contextL2P;
  BOOL                 mode;
  T_CGATT_STATE			attached; /* Now T_CGATT_STATE instead of BOOL*/
  T_CGCLASS_CLASS      tclass;
  T_MFW_GPRS_EVENTREP  eventRep;
  T_EVENT_REJECT       erepReject;
  T_EVENT_ACT          erepAct;
  T_MFW_GPRS_CELL_STRUCT cell;	/* Cell reselection data */
  T_CGSMS_SERVICE      service;
  USHORT               link_id;
  T_ACI_AT_CMD         cmdId;
  T_MFW_GPRS_COUNTER	counter;	/* Data uplink/downlink counter */
} T_MFW_GPRS_PARA;


/* MFW Control Block for GPRS --------------------------------------*/
typedef struct
{
  T_MFW_EVENT          emask;          /* events of interest        */
  T_MFW_EVENT          event;          /* current event             */
  T_MFW_CB             handler;        /* callback handler          */
  T_MFW_GPRS_DATA      data;           /* GPRS data                 */
} T_MFW_GPRS;



/* MFW internal functions ------------------------------------------*/

void      gprs_init   ( void );

void      gprs_exit   ( void );

T_MFW_HND gprs_create ( T_MFW_HND   hWin,
                        T_MFW_EVENT event,
                        T_MFW_CB    cbfunc );

T_MFW_RES gprs_delete ( T_MFW_HND   h );

int       gprsCommand ( U32         cmd, 
                        void        *h ); 

void      gprs_signal ( T_MFW_EVENT event, 
                        void        *para );

void      gprs_ok(T_ACI_AT_CMD cmdId);
void      gprs_error(T_ACI_AT_CMD cmdId, T_ACI_CME_ERR err);
void      gprs_connect(SHORT cId);
void      gprs_err_connect(SHORT cId);

/* MMI-MFW Interface Functions -------------------------------------*/

#ifndef FF_2TO1_PS
T_MFW_RES gprs_definePDPContext ( SHORT           cid, 
                                  T_PDP_CONTEXT *inputCtxt );

T_MFW_RES gprs_setQOS           ( SHORT           cid ,
                                  T_PS_qos           *inputQos );

T_MFW_RES gprs_setQOSMin        ( SHORT           cid ,
                                  T_PS_qos           *inputQos);

T_MFW_RES gprs_attach           ( T_CGATT_STATE   state);
/* SPR#1983 - SH - Added function to cancel CGATT command */
T_MFW_RES gprs_attach_abort(void);

T_MFW_RES gprs_contextActivation( T_CGACT_STATE   state, 
                                   SHORT           *cids);

T_MFW_RES gprs_setData          ( char            *L2P, 
                                  SHORT           *cids);

T_MFW_RES gprs_showPDPAddress   ( SHORT           *cids, 
                                  T_NAS_ip   *pdp_address);
#else
T_MFW_RES gprs_definePDPContext ( SHORT           cid, 
                                  T_PDP_CONTEXT   *inputCtxt );

T_MFW_RES gprs_setQOS           ( SHORT           cid ,
                                  T_PS_qos           *inputQos );

T_MFW_RES gprs_setQOSMin        ( SHORT           cid ,
                                  T_PS_qos           *inputQos);

T_MFW_RES gprs_attach           ( T_CGATT_STATE   state);
/* SPR#1983 - SH - Added function to cancel CGATT command */
T_MFW_RES gprs_attach_abort(void);

T_MFW_RES gprs_contextActivation( T_CGACT_STATE   state, 
                                   SHORT           *cids);

T_MFW_RES gprs_setData          ( char            *L2P, 
                                  SHORT           *cids);

T_MFW_RES gprs_showPDPAddress   ( SHORT           *cids, 
                                  T_NAS_ip   *pdp_address);
#endif

T_MFW_RES gprs_setAutoResponse  ( T_CGAUTO_N      mode);

T_MFW_RES gprs_ManualResponse   ( USHORT          response, 
                                  char            *l2p, 
                                  SHORT           cid);

T_MFW_RES gprs_setClass         ( T_CGCLASS_CLASS m_class);

T_MFW_RES gprs_setEventReporting( T_CGEREP_MODE   mode, 
                                  T_CGEREP_BFR    bfr);

T_MFW_RES gprs_setServiceSMS    ( T_CGSMS_SERVICE service);

T_CGSMS_SERVICE gprs_getServiceSMS(void);

/* Interface for SNCNT command */
T_MFW_RES gprs_counter(BOOL reset_counter);

/* SPR#1983 - SH - Added function to cancel SNCNT command */
T_MFW_RES gprs_counter_abort(void);

T_CGREG_STAT gprs_status(void);
//x0035544 Feb 23, 2006 DR:OMAPS00068976
/* Added function to querry and update the gprs status */
T_P_CGREG_STAT p_gprs_status(void);

#ifdef NEPTUNE_BOARD
/* OMAPS00074454 - 24-04-2006 */
int mfw_gprs_edge_status(int *iStatus);
/* END */
#endif


/* ACI callback functions ------------------------------------------*/

void rAT_PlusCGACT  ( SHORT link_id );
                    /* link_id: Link identifier              */

void rAT_PlusCGDATA ( SHORT link_id );
                    /* link_id: Link identifier              */

void rAT_PlusCGANS  ( SHORT link_id );
                    /* link_id: Link identifier              */

void rAT_PlusCGEREP ( T_CGEREP_EVENT            event, 
                      T_CGEREP_EVENT_REP_PARAM *param );
                    /* event  : GPRS event                        
                       param  : event parameter              */
void rAT_PercentCGEV ( T_CGEREP_EVENT event,
                      T_CGEREP_EVENT_REP_PARAM *param );
                    /* event  : GPRS event                        
                       param  : event parameter              */

void rAT_PlusCGREG  ( T_CGREG_STAT stat, 
                      USHORT       lac, 
                      USHORT       ci );
                    /* stat   : indicates the current registration 
                       lac    : two byte location area code         
                       ci     : two byte cell ID             */
                       
/* ACI CHANGES - 5-10-2005 */
void rAT_PercentCGREG  ( T_P_CGREG_STAT stat,
                      USHORT       lac,
                      USHORT       ci,
					  BOOL         bActiveContext );	
//                      BOOL         bActiveContext );
                    /* stat   : indicates the current registration
                       lac    : two byte location area code
                       ci     : two byte cell ID             
             bActiveContext   : boolean: shows wether a PDP context is 
                                         established or not*/

#ifdef FF_2TO1_PS
void rAT_changedQOS ( U8 cid, 
                      T_PS_qos *qos );
                     /* cid    : context id                   
                        *qos   : quality of service profile   */
#else 
void rAT_changedQOS ( SHORT cid, 
                      T_PS_qos *qos );   
                     /* cid    : context id                   
                        *qos   : quality of service profile   */

#endif

/* END */
GLOBAL void rAT_PercentSNCNT ( UBYTE c_id,
                               ULONG octets_uplink,
                               ULONG octets_downlink,
                               ULONG packets_uplink,
                               ULONG packets_downlink );

/* Added to remove warning Aug - 11 */
EXTERN UBYTE dspl_Enable (UBYTE   in_Enable);
/* End - remove warning Aug - 11 */
#endif
