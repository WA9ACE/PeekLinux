	/******************************************************************************
	 * Power Task (pwr)
	 * Design and coding by Svend Kristian Lindholm, skl@ti.com
	 *
	 * PWR Message Processing
	 *
	 * $Id: pwr_handle_message.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
	 *
	 ******************************************************************************/
#ifndef __LCC_HANDLE_MESSAGE_H_
#define __LCC_HANDLE_MESSAGE_H_

#include "lcc/lcc.h"
#include "abb/bspTwl3029_Bci.h"
	/******************************************************************************
	 * Function prototypes
	 ******************************************************************************/

	T_RVM_RETURN pwr_task_init(void);
	T_RV_RET process_pwr_tm_read_request         (T_PWR_REQ *request);
	T_RV_RET process_pwr_tm_write_request        (T_PWR_REQ *request);
	T_RV_RET process_adc_indication          (T_PWR_REQ *request);
	T_RV_RET process_pwr_WD_overflow 		(T_PWR_REQ *request);
	void pwr_send_msg(uint32 msg_id ,T_RVF_ADDR_ID src_addr_id, T_RVF_ADDR_ID  dest_addr_id);
	void pwr_modulate_on(void);
	void pwr_modulate_off(void);
	void start_q401_charge(void);
	void stop_q401_charge(void);
	void start_q402_charge(void);
	void stop_q402_charge(void);
	void start_bci_charge(BspTwl3029_Bci_pathType path_type);
	void stop_bci_charge(void);
	void stop_bci_linCharge(void);
	void stop_bci_pwmCharge(void);

	void build_name(const char *ch_pre, char *cfg_id , UINT8 index, const char * ch_post,  char * name);
	void cv_charging_house_keeping(BspTwl3029_Bci_pathType path_type);
	void update_duty_cycle(void);
	void end_charging_house_keeping(void);
	void dummy_callback(BspI2c_TransactionId transID);
#endif
