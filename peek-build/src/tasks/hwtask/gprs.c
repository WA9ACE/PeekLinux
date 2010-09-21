/* GPRS related functions */
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

#include "aci_all.h"
#include "aci_cmh.h"  /* include types declared in ACI              */
#include "ati_cmd.h"
#include "aci_cmd.h"
#include "aci.h"
#include "dti_conn_mng.h"
#include "dti_cntrl_mng.h"
#include "gaci.h" /* include types declared in GACI             */
#include "gaci_cmh.h" /* include types declared in GACI             */
#include "prim.h"

#include "gprs.h"

//XXX: Send gprs update event
void gprs_signal( int event, 
                  void       *para)
{
  emo_printf("gprs_signal() event[%d]", event);
}

int gprs_attach(T_CGATT_STATE state)
{
  T_ACI_RETURN result;
  
  TRACE_FUNCTION ("gprs_attach()");

  result = sAT_PlusCGATT(CMD_SRC_LCL, state);
  if ( result!= AT_EXCT && result!=AT_CMPL)
  {
  	TRACE_EVENT("MFW_GPRS: Error. sAT_PlusCGATT failed");
    return -1;
  }
  
  gprs_signal(1, &state);


  return 0;
}

void gprs_ok(T_ACI_AT_CMD cmdId)
{
  T_EMOBIIX_MESSAGE *attachMessage;

  emo_printf("gprs_ok");

  /*
  gprs_signal(E_HW_GPRS_OK, &cmdId);

  attachMessage = P_ALLOC(EMOBIIX_MESSAGE);
  attachMessage->attach = 1;
  P_OPC(attachMessage) = EMOBIIX_SOCK_CREA;
  PSENDX(APP, attachMessage);
  */
  return;
}


