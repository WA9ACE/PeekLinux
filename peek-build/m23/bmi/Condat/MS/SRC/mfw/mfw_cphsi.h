/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (8417)               $Workfile:: mfw_cphsi.h     $|
| $Author:: Vo                          $Revision:: 1               $|
| CREATED: 14.07.00                     $Modtime::                  $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

  PURPOSE : Internal definitions for CPHS management of MMI framework
  
   $History:: mfw_cphs.c					      $

	Jan 12, 2006	OMAPS00062632	x0018858
	Description: The differnace in the type of the data passed to the function cphs_read_information()
	is causing an incorrect value being received as the output.
	Solution: The errcode being passed to the function has been modified to USHORT


*/

#ifndef DEF_MFW_CPHSI_HEADER
#define DEF_MFW_CPHSI_HEADER

#define CPHS_SERVICE_TABLE   2 /* size of CPHS service table            */



//Code 16:
#define MFW_CPHS_INFO_SIZE   3 /* size of CPHS information field (6F16) */
#define MFW_CPHS_ONS_SIZE   20 /* size of operator name string (6F14) - read first 20 bytes only */ 
#define MFW_CPHS_ONSF_SIZE  10 /* size of operator name short form (6F18) */
#define MFW_CPHS_MIN_MBN_SIZE 14
#define MFW_CPHS_MBS_SIZE    2 /* size of voice message waiting flag (6F11) */
#define MFW_CPHS_CFF_SIZE    2 /* size of call forwarding flags (6F13)  */
#define MFW_CPHS_ALS_SIZE    1 /* size of alternate line service (6F9F) */
#define MFW_CPHS_ALSS_SIZE   1 /* size of alternate line service status (6F92) */
#define MFW_CPHS_CSP_SIZE   20 /* size of customer service profile (6F15) */
#define MFW_CPHS_MIN_INS_SIZE 5
//Code 16 END


/* SMS Control Block */
typedef struct
{
  T_MFW_EVENT      emask;          /* events of interest */
  T_MFW_EVENT      event;          /* current event */
  T_MFW_CB         handler;
  T_MFW_CPHS_PARA  para;
} T_MFW_CPHS;

void  cphs_support_check        (void);

//x0018858 OMAPS00062632 The errcode type has been modified from SHORT to USHORT.
void  cphs_read_information     (USHORT errCode, UBYTE *data, UBYTE dataLen);
void cphs_read_ons(USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_onsf            (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_mbs             (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_cff             (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_als             (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_alss            (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_csp             (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_update_info_cnf      (USHORT errCode, UBYTE *data, UBYTE dataLen);
void  cphs_read_mbn             (SHORT table_id);
void  cphs_read_info_num        (SHORT table_id);
void  cphs_req_info_num_read    (UBYTE rcd_num, UBYTE dataLen);
void  cphs_read_eeprom_mailbox  (void);
void  cphs_write_eeprom_mailbox (T_MFW_CPHS_ENTRY *entry);
void  cphs_read_eeprom_als      (T_MFW_CPHS_ALS_STATUS *info);
void  cphs_write_eeprom_als     (UBYTE *res);
void  cphs_write_eeprom_alss    (UBYTE *res);
BOOL  cphs_read_sim_dat         (USHORT data_id, UBYTE len, UBYTE max_length);
void  cphs_read_sim_dat_cb      (SHORT table_id);
void cphs_read_sim_default_cb(SHORT table_id);
void cphs_write_sim_default_cb(SHORT table_id);
BOOL  cphs_read_sim_rcd         (USHORT data_id, UBYTE rcd_num, UBYTE len);
void  cphs_read_sim_rcd_cb      (SHORT table_id);
BOOL  cphs_write_sim_dat        (USHORT data_id, UBYTE *data, UBYTE length);
void  cphs_write_sim_dat_cb     (SHORT table_id);
BOOL  cphs_write_sim_rcd        (USHORT data_id, UBYTE rcd_num,
                                 UBYTE *data,       UBYTE dataLen);
void  cphs_write_sim_rcd_cb     (SHORT table_id);
BOOL  cphs_build_mbn_data       (UBYTE *data, UBYTE len);
UBYTE cphs_ssc                  (UBYTE nr, UBYTE * serv_table);
void  cphs_sat_file_update      (USHORT dataId);
#endif
