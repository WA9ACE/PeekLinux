/* Entry point for mmi */
/* NOTE: All code in here will be excuted by aci task */

#include "typedefs.h"
#include "header.h"
#include "Armio.h"
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


static int pSlot;

extern BOOL appdata_response_cb (ULONG opc, void * data);

void mmi_main(void) {

 // Setup battery callbacks
	system_battery_init();
	gprs_dataobject_init();

 // while(1)
//	TCCE_Task_Sleep(1000);

 /* Wait for HW task and UI task to complete */
 while(!HwStatusGet() && !uiStatusGet())
   TCCE_Task_Sleep(100);

 emo_printf("mmi_main()");
 //pSlot = aci_create(appdata_response_cb,NULL);

 // Setup/Start baseband related functionality
 /* Enable signal quality */
 sAT_PercentCSQ ( CMD_SRC_LCL, CSQ_Enable );
 /* Register SIM management handler */
 sim_init();
 /* Register network management handler */
 nm_init();
 /* Start SIM which triggers network registration*/
#ifndef EMO_SIM
 sim_activate();
#endif
}
