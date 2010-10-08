/* Sim Related functions */
#include "typedefs.h"
#include "header.h"
#include "vsi.h"
#include "pconst.cdg"
#include "p_sim.h"
#include "acie.h"
#include "hwsim.h"
#include "p_em.h"
#include "p_mncc.h"
#include "p_mmreg.h"
#include "p_mnsms.h"
#include "m_fac.h"
#include "p_8010_137_nas_include.h"
#include "p_mmi.h"
#include "aci_cmh.h"
#include "p_8010_152_ps_include.h"
#include "gaci_cmh.h"
#include "p_dl.h"
#include "psa.h"
#include "cus_aci.h"
#include "m_sat.h"
#include "psa_sim.h"


static int pSlot;  
static BOOL sat_update = FALSE;
static UBYTE sim_stat = SIM_NOT_ACTIVE;
static T_HW_SIM_STATUS sim_status;
static T_HW_SIM_CONFIG sim_config;
T_HW_READ_CALLBACK read_callback_data;
T_BOOTUP_STATE BootUpState = BOOTUP_STATE_NONE;

static UBYTE sim_service_table [0x10];

void sim_read_sim(USHORT data_id, UBYTE len, UBYTE max_length);
void sim_signal (int event, void *para);

UBYTE sim_status_check (void)
{
    emo_printf("sim_status_check()");

    return sim_stat;
}

void sim_read_sst_cnf (USHORT error, UBYTE *data)
{
  emo_printf("sim_read_sst_cnf()");

  if (error EQ SIM_NO_ERROR)
    memcpy(sim_service_table, data, sizeof(sim_service_table));
  else
    emo_printf("SST read error");
}

void sim_ok_cfun (void)
{
    T_ACI_CFUN_FUN fun;

    emo_printf("sim_ok_cfun()");

    qAT_PlusCFUN( CMD_SRC_LCL, &fun );

    if( fun EQ CFUN_FUN_Full )
    {
      sim_stat = SIM_ACTIVE;
      sim_status.sim_status = HW_SIM_NO_PIN;
      sim_status.sim_operation_mode = sim_config.oper_mode;
      sim_status.sim_pin_retries = 0;
      sim_status.sim_procedure = SIM_ACTIVATION;
      //sim_status.sim_status_type = pin_ident;

      sim_signal(E_SIM_STATUS,&sim_status);
    }
    else if( fun EQ CFUN_FUN_Minimum )
    {
			//XXX fix
            //nm_ok_deregistration();
    }
}

U8 sim_check_service (U8 nr, U8 *serv_table)
{
    U8 value;

    emo_printf("sim_check_service()");

    value = *(serv_table + (nr - 1) / 4);
    value >>=  (((nr - 1) & 3) * 2);

    return (value & 3);
}

int sim_event_cb_main (int event, void * para)
{
        UBYTE limited;

				T_HW_SIM_STATUS * status;

        //status = (T_HW_SIM_STATUS *)para;

    	limited = TRUE;

  		emo_printf("sim event handler,event:%d", event); //, status:%d, proc:%d",event, status->sim_status, status->sim_procedure);

        switch(event)
        {
                case E_SIM_INSERTED:
									emo_printf("E_SIM_INSERTED");
                	BootUpState = BOOTUP_STATE_COMPLETED;
                	nm_registration (NM_AUTOMATIC, 0, FALSE);
                	return 0;

                case E_SIM_RESET:       
                	emo_printf("E_SIM_RESET");
                	/*NM, 110702
                        the MM-entity initated the de-registration already (MMI dont have to do it); 
                        after this event we should avoid any cases to access the SIM
                        (e.g. reading phonebook, reading sms....)
                        
                        - how to do it ???
                        ->  unset the "sms_initialised" flag to FALSE

                        - when does it set back to TRUE ???
                        -> the flag will be set after the event "E_SMS_READY"
                	*/      
                	//        smsidle_unset_ready_state ();
                 	//        info_screen(0, TxtSimNot, TxtReady, NULL); 
            	 	return 0;           
             
 
                 case E_SIM_STATUS:
				 status = (T_HW_SIM_STATUS *)para;
				 emo_printf("E_SIM_STATUS - [%d]", status->sim_status);
				 /* Current we no supportie PIN's on sim's */
				 if(status->sim_status == HW_SIM_NO_PIN) {
                     BootUpState = BOOTUP_STATE_COMPLETED;
					 nm_registration (NM_AUTOMATIC,0, 0);
				 }
				 
                 /*
                 * check whether decoded message is available
                 */
                 //if( para EQ (T_MFW_SIM_STATUS *)NULL )
                  //  return MFW_RES_ILL_HND;
 
                 //status = (T_MFW_SIM_STATUS *)para;
  				 //emo_printf("sim_status: %d", status->sim_status);
		}

#if 0 
 #ifdef SIM_PERS         
 
                           //x0pleela 18 Oct, 2006 DR: OMAPS00099660
                           //Added the following code (MFW_MEPD_INVALID)inside the SIM_PERS flag
                           //x0pleela 08 Sep, 2006  DR: OMAPS00091250
                           //Write No MEPD, NO SIM string into a file
                           if(  status->sim_procedure == MFW_SIM_ACTIVATION )
                           {
                                 if(status->sim_status ==  MFW_MEPD_INVALID)
                                         mfw_flash_write("No MEPD");
                                 if(status->sim_status == MFW_SIM_NO_SIM_CARD)
                                         mfw_flash_write("No SIM");
                           }
                                         
 
                                 //x0pleela 29 Aug, 2006 DR: OMAPS00091250
                                 //Added this check so that MMI doesn't hang at boot up as 
                                 //MEPD data is not present
                                 if( ( status->sim_procedure == MFW_SIM_ACTIVATION )AND 
                                         ( ( status->sim_status !=  MFW_MEPD_INVALID) OR
                                         (status->sim_status != MFW_SIM_NO_SIM_CARD) ) )
                                 {
                                 
                                                 status1 =mfw_simlock_check_status(0,&max1, &curr1);
                                                 if(status1== MFW_SIM_PERM_BLOCKED)
                                                 {
                                                         perm_blocked = 1;
                                                         status->sim_status=MFW_SIM_PUK_REQ;
                                                 }
                                 }
                                 
 #endif
                         BootUpState = BOOTUP_STATE_INPROGRESS;
 
                  switch (status->sim_procedure)
                  {
                 /*
                  * Initial activation of the SIM card
                  */
                         case MFW_SIM_ACTIVATION:
                   /*
                    * check the result of activation
                    */
                           switch (status->sim_status)
                           {
                                   //x0pleela 18 Oct, 2006 DR: OMAPS00099660
                                   //Added the following code (MFW_MEPD_INVALID)inside the SIM_PERS flag
 #ifdef SIM_PERS                   
                           //x0pleela 29 aug, 2006  DR: OMAPS00091250
                                 //Send event No MEPD data
                                 case MFW_MEPD_INVALID:
                                         //limited = TRUE;
                                         TRACE_FUNCTION("sim_event_cb_main:no MEPD data");
                                         SEND_EVENT(pin_data->pin_win,NO_MEPD, 0, pin_data);
                                         break;
 #endif                                  
                         /*
                          * PIN 1 must be entered
                          */
                                         case MFW_SIM_PIN_REQ:
                                                 if(status->sim_pin_retries < PIN_ENTRY_MAX)
                                                 {
                                                         TRACE_FUNCTION("sim_event_cb_main:PIN1 req-retr");
                                                         pin_data->pin_retries = status->sim_pin_retries;/* number of retries */
                                                         SEND_EVENT(pin_data->pin_win,PIN1_REQ_ATT,0,pin_data);
                                                 }
                                                 else
                                                 {
 
 /********************************/
 
                                                         TRACE_FUNCTION("sim_event_cb_main:PIN1 req");
                                                         SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
                                                 }
                                                 break;
 
                         /*
                          * SIM card is blocked, PUK 1 is needed
                          */
                                         case MFW_SIM_PUK_REQ:
                                         {
 #ifdef SIM_PERS
                                                 int smlk_status, max, curr; //x0pleela 19 Apr, 2006 DR:OMAPS00067912   changed the name from stastus to smlk_status
 #endif
                                                         TRACE_FUNCTION("sim_event_cb_main:PUK1 req");
 #ifdef SIM_PERS
                                                 //x0pleela 19 Apr, 2006 DR:OMAPS00067912   
                                                 //changed the name from stastus to smlk_status
                                                 smlk_status =mfw_simlock_check_status(0,&max, &curr);
                                                 if( (smlk_status== MFW_SIM_BLOCKED) OR (smlk_status == MFW_SIM_NOT_BLOCKED) )
                                                 {
                                                         TRACE_EVENT("BMI: MFW_SIM_BLOCKED");
                                                         perm_blocked = 0;
                                                                 TRACE_EVENT("BMI: MFW_SIM_PUK1");
                                                         //x0pleela 26 Sep, 2006 DR: OMAPS00096565
                                                         //Adding a check for C_KEY_REQ. If set, display SIMP menu
                                                         //Else prompt the user for PUK entry
                                                         if( C_KEY_REQ )
                                                         {
                                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                                 //check for Master Unlock option enabled. 
                                                                 //If so, then display the menu with options Unblock ME and Master Unlock
                                                                 if( simlock_get_masterkey_status())
                                                                 {
                                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                                 }
                                                         }
                                                         else
 #endif
                                                                 SEND_EVENT(pin_data->pin_win,PUK1_REQ_ACT_INFO,0,pin_data);
 
 #ifdef SIM_PERS
                                                 }
                                                 //x0pleela 19 Apr, 2006 DR:OMAPS00067912   
                                                 //changed the name from stastus to smlk_status
                                                 else if (smlk_status == MFW_SIM_PERM_BLOCKED)
                                                 {
                                                         perm_blocked =1;
                                                         SEND_EVENT(pin_data->pin_win,PERM_BLK,0,pin_data);
                                                 }
 #endif
                                          }
                                         break;
 
                         /*
                          * no PIN is needed
                          */
                                         case MFW_SIM_NO_PIN:
                                                 
                                                 BootUpState = BOOTUP_STATE_COMPLETED;//xashmic 21 Sep 2006, OMAPS00095831
 
                                                 limited = FALSE;
                                                 TRACE_FUNCTION("sim_event_cb_main:no Pin");
                                                 //mmi_cphs_refresh();
                                                 SEND_EVENT(pin_data->pin_win,PIN_OK, 0, NULL);//back to idle
                                                 break;
 
                         /*
                          * SIM card is broken
                          */
                                         case MFW_SIM_INVALID_CARD:
                                                 TRACE_FUNCTION("sim_event_cb_main: invalid card");
                                                 SEND_EVENT(pin_data->pin_win,INVALID_CARD, 0, pin_data);
                                                 break;
 
                         /*
                          * No SIM card is inserted
                          */
                                         case MFW_SIM_NO_SIM_CARD:
                                                 
                                                 TRACE_FUNCTION("sim_event_cb_main: no SIM Card");
                                                 SEND_EVENT(pin_data->pin_win,NO_SIM_CARD, 0, pin_data);
                                                 break;
 
 // Nov 24, 2005, a0876501, DR: OMAPS00045909
                                         case MFW_IMEI_NOT_VALID:
                                                 limited = FALSE;
                                                 TRACE_FUNCTION("sim_event_cb_main: IMEI is not valid");
                                                 SEND_EVENT(pin_data->pin_win,INVALID_IMEI, 0, pin_data);
                                                 break;
 
 #ifdef SIM_PERS 
 
                                         case MFW_SIM_PLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_PLOCK_REQ");
                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options SIM unlock and Master Unlock
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_PLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                     SEND_EVENT(pin_data->pin_win,INPUT_SIM_PIN_REQ, 0, pin_data);
                                                 break;
                                                 
                                         case MFW_SIM_NLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_NLOCK_REQ");
                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options Network unlock and Master Unlock                                      
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_NLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                     SEND_EVENT(pin_data->pin_win,INPUT_NLOCK_PIN_REQ, 0, pin_data);
                                                 break;
                                                 
                                         case MFW_SIM_NSLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_NSLOCK_REQ");
                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options Network subset unlock and Master Unlock
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_NSLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                     SEND_EVENT(pin_data->pin_win,INPUT_NSLOCK_PIN_REQ, 0, pin_data);
                                                 break;
                                                 
                                         case MFW_SIM_SPLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_SPLOCK_REQ");
                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options Service Provider unlock and Master Unlock                                     
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_SPLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                         SEND_EVENT(pin_data->pin_win,INPUT_SPLOCK_PIN_REQ, 0, pin_data);
                                                 break;
                                                 
                                         case MFW_SIM_CLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_CLOCK_REQ");
                                                 //x0pleela 20 July, 2006 ER: OMAPS00087586, OMAPS00087587
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options Corporate unlock and Master Unlock                                    
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_CLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                     SEND_EVENT(pin_data->pin_win,INPUT_CLOCK_PIN_REQ, 0, pin_data);
                                                 break;
 
                                         //x0pleela 25 Sep, 2006 ER: OMAPS00095524
                                         //Request unlocking password of "Blocked Network" category
                                         case MFW_SIM_PBLOCK_REQ:
                                                 TRACE_FUNCTION("sim_event_cb_main: MFW_SIM_PBLOCK_REQ");
                                                 //check for Master Unlock option enabled. 
                                                 //If so, then display the menu with options Network unlock and Master Unlock                                      
                                                   if( simlock_get_masterkey_status())
                                                   {
                                                         mmi_simlock_locktype = MFW_SIM_PBLOCK;
                                                         pin_data->simp_list_win = mmi_simlock_category_menus(win);
                                                   }
                                                   else
                                                     SEND_EVENT(pin_data->pin_win,INPUT_PBLOCK_PIN_REQ, 0, pin_data);
                                                 break;
 #endif
                                         default:
                                                 return 0;
                           }
                   /*
                    * start limited or full service */
 
                           if (limited)
                           {
                         /*
                          * limited shall be the requested service
                          * if no service is reached, the limited
                          * service is requested*/
                                 TRACE_FUNCTION("DEBUG");
 
                                         if (nm_reg_status() EQ NOT_PRESENT_8BIT
                                                 AND
                                                 status->sim_procedure EQ MFW_SIM_ACTIVATION)
                                                 {
                                                         TRACE_FUNCTION("registration in limited mode ");
                                                         nm_registration (NM_AUTOMATIC, 0, TRUE);
                                                 }
                           }
                         break;
                         case MFW_SIM_VERIFY:
                   /*
                    * check the result of verify
                    */
                           switch (status->sim_status)
                           {
                         /*
                          * PIN 1 must be entered
                          */
                                         case MFW_SIM_PIN_REQ:
                                         if(status->sim_pin_retries < PIN_ENTRY_MAX)
                                         {
                                                         TRACE_FUNCTION("sim_event_cb_main:Ver:PIN1 req-retr");
                                                         pin_data->pin_retries = status->sim_pin_retries;
                                                         SEND_EVENT(pin_data->pin_win,PIN1_REQ_ATT,0,pin_data);
                                         }
                                         else
                                         {
                                                         TRACE_FUNCTION("SimEventPins:V:PIN1 req");
                                                         SEND_EVENT(pin_data->pin_win,PIN1_REQ,0,pin_data);
                                         }
                                         break;
                         /*
                          * SIM card is blocked, PUK 1 is needed
                          */
                                         case MFW_SIM_PUK_REQ:
                                                 TRACE_FUNCTION("SimEventPins:V:PUK1 req");
                                                 SEND_EVENT(pin_data->pin_win,PUK1_REQ_VER_INFO,0,pin_data);
                                                 break;
 
                         /*
                          * PIN entering successfull
                          */
                                         case MFW_SIM_NO_PIN:
                                                 TRACE_FUNCTION("sim_event_cb_main:PIN ok without sim unlock");
                                                 //mmi_cphs_refresh();
                                                 pin_data->pin_case = PIN_OK_INFO;
                                                 if (pin_data->set_state == PIN2_CHECK)
                                                 {
                                                         set_mode_fdn_adn(win,pin_data);
                                                 }
                                                 SEND_EVENT(pin_data->pin_win,PIN_OK_INFO,0,pin_data);
                                                 break;
                         /*
                          * SIM card is broken
                          */
                                         case MFW_SIM_INVALID_CARD:
                                                 TRACE_FUNCTION("sim_event_cb_main: invalid card");
                                                 SEND_EVENT(pin_data->pin_win,INVALID_CARD, 0, pin_data);
                                                 break;
 
                         /*
                          * No SIM card is inserted
                          */
                                         case MFW_SIM_NO_SIM_CARD:
                                                 TRACE_FUNCTION("sim_event_cb_main: no SIM Card");
                                                 SEND_EVENT(pin_data->pin_win,NO_SIM_CARD, 0, pin_data);
                                                 break;
                                         default:
                                                 return 0;
                           }
                         break;
                         /*
                          * response to PUK entering
                          */
                         case MFW_SIM_UNBLOCK:
                           switch (status->sim_status)
                           {
                                 case MFW_SIM_PUK_REQ:
                                         TRACE_FUNCTION("sim_event_cb_main:unblock failure");
                                         SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FAIL_INFO,0,pin_data);
                                         break;
                                 case MFW_SIM_SUCCESS:
                                         TRACE_FUNCTION("sim_event_cb_main:unblock success");
                                         //mmi_cphs_refresh();
                                         SEND_EVENT(pin_data->pin_win,PUK1_UNBL_SUCC,0,pin_data);
                                         break;
                                 case MFW_SIM_FAILURE:
                                         TRACE_FUNCTION("sim_event_cb_main:unblock fatal error");
                                         SEND_EVENT(pin_data->pin_win,PUK1_UNBL_FATAL_ERROR,0,pin_data);
                                         break;                          
                                 case MFW_SIM_INVALID_CARD:
                                         TRACE_FUNCTION("sim_event_cb_main:unblock failed, no more retries");
                                         SEND_EVENT(pin_data->pin_win,INVALID_CARD,0,pin_data);
                                         break;
                                 default:
                                         return 0;
                           }
                         break;
 
                         case MFW_SIM_REMOVED:/*  Handle the removal of SIM card */
                             /* OMAPS00050447: a0393130, handle removal of sim card */
                             switch (status->sim_status)
                             {
                                  case MFW_SIM_NO_SIM_CARD: 
                                       TRACE_FUNCTION("sim_event_cb_main: SIM Card removed");
                                                 /*OMAPS00115179 - support SIM insertion without powering down - a0393213
                                                     SMS state should be unset immediately after removing the SIM card. This ensures
                                                     icons/display related to messages are NOT shown in the idle screen after the removal of SIM card*/                                          
                                                smsidle_unset_ready_state ();                                                                      
                                        SEND_EVENT(idle_get_window(),NETWORK_NO_SERVICE, 0, 0);
                                        break;
                                         
                                  default:
                                        return 0;
                             }
                          break;
                               /* OMAPS00050447: a0393130, handle removal of sim card */
                           
                         default:
                                 return 0;
                  }
 
         return 1;
         #ifdef BMI_TEST_MC_SIM_EVENT
         break;
         case BMI_TEST_MC_SIM_EVENT:
         {
                 T_MFW_READ_CALLBACK* sim_read_data;
 
                 sim_read_data = (T_MFW_READ_CALLBACK*)para;
 
                 TRACE_EVENT_P3("SIM READ Error: %d, %x%x", sim_read_data->error_code,
                         sim_read_data->read_buffer[0], sim_read_data->read_buffer[1]);
         }
         break;
         #endif
         }
     } // June 16, 2005  REF: CRR 31267  x0021334
 #endif
     return 0;
 }

void sim_signal (int event, void *para)
{
	//XXX send event
	emo_printf("sim_signal()");
	sim_event_cb_main (event, para);

}

void sim_invalid_ind(void)
{
	emo_printf("sim_invalid_ind()");

	sim_status.sim_status = HW_SIM_INVALID_CARD;
	sim_signal(E_SIM_STATUS,&sim_status);
}

void sim_gid1_cnf(USHORT error, UBYTE *data)
{
  emo_printf("sim_gid1_cnf()");

  if (error EQ SIM_NO_ERROR)
    memcpy(sim_config.sim_gidl1, data, 5);

  if (!sat_update)
  {
    if (sim_check_service(16,sim_service_table)
        == ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_GID2, NOT_PRESENT_8BIT, 5);
  }
}

void sim_gid2_cnf(USHORT error, UBYTE *data)
{
  emo_printf("sim_gid2_cnf()");
  if (error EQ SIM_NO_ERROR)
    memcpy(sim_config.sim_gidl2, data, 5);
}

void sim_read_sim_cb(SHORT table_id)
{
  emo_printf("sim_read_sim_cb()");

  simShrdPrm.atb[table_id].ntryUsdFlg = FALSE;

  switch (simShrdPrm.atb[table_id].reqDataFld)
  {
    /* Service provide name */
    case SIM_SPN:
      nm_spn_cnf(simShrdPrm.atb[table_id].errCode, (T_EF_SPN *)simShrdPrm.atb[table_id].exchData);
      break;

    /* group identifier level 1 */
    case SIM_GID1:
      sim_gid1_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;

    /* group identifier level 2 */
    case SIM_GID2:
      sim_gid2_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;

    case SIM_SST:
      sim_read_sst_cnf(simShrdPrm.atb[table_id].errCode, simShrdPrm.atb[table_id].exchData);
      break;
        /*MC CONQ5862, 13/06/02*, if not one of above data fileds, send event to MMI*/
    default:
        read_callback_data.error_code = simShrdPrm.atb[table_id].errCode;
        read_callback_data.read_buffer = simShrdPrm.atb[table_id].exchData;
        sim_signal(E_SIM_READ_CALLBACK, (void*)&read_callback_data);
      break;
  }
}

void sim_read_sim(USHORT data_id, UBYTE len, UBYTE max_length)
{
  SHORT table_id;

  emo_printf("sim_read_sim()");

  table_id = psaSIM_atbNewEntry();

  if(table_id NEQ NO_ENTRY)
  {
    simShrdPrm.atb[table_id].ntryUsdFlg = TRUE;
    simShrdPrm.atb[table_id].accType    = ACT_RD_DAT;
    // R99 SIM Interface changes . Indicates whether path_info variable has valid values
    simShrdPrm.atb[table_id].v_path_info  = FALSE;
    simShrdPrm.atb[table_id].reqDataFld = data_id;
    simShrdPrm.atb[table_id].dataOff    = 0;
    simShrdPrm.atb[table_id].dataLen    = len;
    simShrdPrm.atb[table_id].recMax     = max_length;
    simShrdPrm.atb[table_id].exchData   = NULL;
    simShrdPrm.atb[table_id].rplyCB     = sim_read_sim_cb;

    simShrdPrm.aId = table_id;

    if(psaSIM_AccessSIMData( ) < 0)
    {
      emo_printf("sim_read_sim() - FATAL ERROR");
    }
  }
}

void sim_spn_req (void)
{
    T_EF_SPN spn;

    emo_printf("sim_spn_req()");

    memset(&spn,0,sizeof(T_EF_SPN));

    if (sim_check_service(17,sim_service_table)
        EQ ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_SPN,17,17);
    else
        nm_spn_cnf(SIM_CAUSE_ACCESS_PROHIBIT, &spn);
}

void rAT_PlusCFUNP (T_SIM_MMI_INSERT_IND *mmi_insert_ind)
{
    emo_printf("rAT_PlusCFUNP()");

    sim_stat = SIM_ACTIVE;

    sim_config.oper_mode = mmi_insert_ind->func;

    nm_mmi_parameters(&mmi_insert_ind->imsi_field);
    memcpy(sim_service_table,mmi_insert_ind->sim_serv, sizeof(sim_service_table));
    sim_config.phase = mmi_insert_ind->phase;
    sim_config.access_acm = mmi_insert_ind->access_acm;
    sim_config.access_acmmax = mmi_insert_ind->access_acmmax;
    sim_config.access_puct = mmi_insert_ind->access_puct;


 /*  SPN must always be read, not only when nm_registration is called    */
    sim_spn_req();

   if (sim_check_service(15,sim_service_table) == ALLOCATED_AND_ACTIVATED)
        sim_read_sim(SIM_GID1, NOT_PRESENT_8BIT, 5);
}

/* convert imsi (packed bcd to ASCIIZ; ->11.11) */

void simDecodeIMSI (UBYTE * imsi_field, 
                    UBYTE   imsi_c_field, 
                    UBYTE * imsi_extracted)
{
  UBYTE length;
  UBYTE i;
  UBYTE digit;

  emo_printf("simDecodeImsi()");

  /*
   * calculate number of digits
   */
  length = (imsi_c_field-1)*2;

  /*
   * if odd number of digits add one
   */
  if (imsi_field[0] & 0x08)
    length++;

  /*
   * extract all digits
   */
  for (i = 0; i < length; i++)
  {
    digit = (i & 1) ?
        imsi_field[(i + 1) / 2] & 0x0f :
       (imsi_field[(i + 1) / 2] & 0xf0) >> 4;

    if (i < LONG_NAME)
      imsi_extracted [i] = digit + 0x30;
    else
    {
      imsi_extracted [i] = 0;
      return;
    }
  }
}

GLOBAL BOOL sima_response_cb (ULONG opc, void * data)
{
  //emo_printf("sim_response_cb()");

  switch (opc)
  {
    case SIM_MMI_INSERT_IND:
      /* MMI relevant parameters from the SIM card has received. */
      rAT_PlusCFUNP ((T_SIM_MMI_INSERT_IND *)data);
      return FALSE;   /* processed also by ACI */

    case SIM_ACTIVATE_CNF:
      //sim_mmi_parameter((T_SIM_ACTIVATE_CNF *)data);
      return FALSE;

    case SIM_ACTIVATE_IND:
      //sim_mmi_update_parameter((T_SIM_ACTIVATE_IND *)data);
      return FALSE;
  }

  return FALSE;       /* not processed by extension */
}


/* sim_init
 * - Registers sima_reponse handler with aci
 * 
 */

int sim_init (void)
{
    emo_printf("sim_init()");

    /* 
     * install prim handler     
     */
    pSlot = aci_create(sima_response_cb,NULL);

    return TRUE;
}

/* 
 *  sim_activate
 * - This sends AT+CFUN which is the start of the network
 * - registration process.
 */

void sim_activate (void)
{

	T_ACI_RETURN cfun;
	T_ACI_ERR_DESC cfun_err;

	cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Full,CFUN_RST_NotPresent);

	if (cfun EQ AT_FAIL)
	{
		cfun_err = qAT_ErrDesc();

		if((cfun_err & 0x0000FFFF) EQ EXT_ERR_IMEICheck)
		{
			sim_stat = IMEI_NOT_VALID;
			sim_status.sim_status = HW_IMEI_NOT_VALID;
			sim_status.sim_operation_mode = sim_config.oper_mode;
			sim_status.sim_pin_retries = 0;
			sim_status.sim_procedure = SIM_ACTIVATION;
			sim_status.sim_status_type = HW_SIM_UNKNOWN;

			sim_signal(E_SIM_STATUS,&sim_status);
		}
    } else if (cfun NEQ AT_EXCT) {
		emo_printf("sAT_PlusCFUN error");
	}
}

void sim_disable (void)
{

	T_ACI_RETURN cfun;

    emo_printf("sim_disable()");

	cfun = sAT_PlusCFUN(CMD_SRC_LCL,CFUN_FUN_Minimum,CFUN_RST_NotPresent);
 
    if (cfun NEQ AT_EXCT)
		emo_printf("sAT_PlusCFUN error");
}

/*
UBYTE getCdByteFromImei(UBYTE *imei)
{
	T_ACI_IMEI aci_imei;

	emo_printf("getCdByteFromImei");

    // For calculating the Cd byte, copy the buffer contents.
	aci_imei.tac1 = imei[0];
	aci_imei.tac2 = imei[1];
	aci_imei.tac3 = imei[2];
	aci_imei.fac  = imei[3];
	aci_imei.snr1 = imei[4];
	aci_imei.snr2 = imei[5];
	aci_imei.snr3 = imei[6];
	aci_imei.svn  = imei[7];

	return(cmhSS_getCdFromImei(&aci_imei));
}
*/
