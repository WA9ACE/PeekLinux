#ifndef __BTC_ATA_SMS_H_20080224__
#define __BTC_ATA_SMS_H_20080224__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IDENTS          0x14 
#define MAX_SCTP_DIGITS        2
#define MAX_SMSC_LEN             21




/* definition for validity period mode */
#define SMS_VPF_NOTPRESENT  0
#define SMS_VPF_NUMERICAL   2  // Corresponds to relative period
#define SMS_VPF_STRING      3  // Corresponds to absolute period


#define VP_REL_1_HOUR 		11
#define VP_REL_12_HOURS	    143
#define VP_REL_24_HOURS	    167
#define VP_REL_1_WEEK		173
#define VP_REL_MAXIMUM		255    //  May 10, 2004    REF: CRR 15753  Deepa M.D 

/* Parameter for cell broadcast message */

typedef struct dd
{
  unsigned char   mode;                         /* acception mode: 0 - accept, 1 - not accept */
  unsigned short  msg_id[MAX_IDENTS];   /* message identifier              */
  unsigned char   dcs[MAX_IDENTS];      /* data coding schemes             */
  unsigned char	  dummy;
} T_ATA_SMS_CB_INFO;


typedef struct
{
  unsigned char year     [MAX_SCTP_DIGITS];
  unsigned char month    [MAX_SCTP_DIGITS];
  unsigned char day      [MAX_SCTP_DIGITS];
  unsigned char hour     [MAX_SCTP_DIGITS];
  unsigned char minute   [MAX_SCTP_DIGITS];
  unsigned char second   [MAX_SCTP_DIGITS];
  short timezone;
} T_ATA_SMS_SCTP;

/* SMS parameters */
typedef struct	
{
  char              sc_addr[MAX_SMSC_LEN]; /* service centre address   */
  short	            prot_id;	      /* protocol identifier      */
  short	            dcs;              /* data coding scheme       */
  unsigned char	            srr;              /* status report request    */
  unsigned char	            rp;               /* reply path               */
  unsigned char	            rd;               /* reject duplicates        */
  unsigned char	            vp_mode;	      /* validity period mode     */
  unsigned char	            vp_rel;	          /* validity period relative */
  T_ATA_SMS_SCTP    vp_abs;           /* validity period absolute */
  short            first_oct;     		/* First Octet*/
} T_ATA_SMS_INFO;


extern int ata_sms_init(void);

#ifdef __cplusplus
}
#endif
#endif //__BTC_ATA_SMS_H_20080224__

