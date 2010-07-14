/* For CSQ */
/*
#include "p_mmi.h"
#include "m_fac.h"
#include "p_mmreg.h"
#include "p_mncc.h"
#include "p_mnsms.h"
#include "p_em.h"
#include "aci_lst.h"
#include "aci_cmh.h"
*/
void EmoTask(void) {

	BalMemInit();
	/*
        sim_init();
        nm_init();
        sim_activate();
        sAT_PercentCSQ ( CMD_SRC_LCL, CSQ_Enable );
	*/

	EmoStatusSet();
	while(1) {
		TCCE_Task_Sleep(50);
	}
}
