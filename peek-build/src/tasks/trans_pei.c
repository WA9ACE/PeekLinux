#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"
#include "gsm.h"
#include "emopei.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

/*=========================== MACROS =========================================*/

#define VSI_CALLER trans_handle, 
#define pei_create trans_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/

T_HANDLE trans_handle;
T_HANDLE TranshComm = VSI_ERROR;

static BOOL first_access = TRUE;
static BOOL exit_flag = FALSE;

extern int bal_sock_api_inst;

/*===========================Function Definition================================*/
/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_monitor
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame in case sudden entity
	 |               specific data is requested (e.g. entity Version).
	 |
	 | Parameters  : out_monitor       - return the address of the data to be
	 |                                   monitoredCommunication handle
	 |
	 | Return      : PEI_OK            - successful (address in out_monitor is valid)
	 |               PEI_ERROR         - not successful
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_monitor (void ** out_monitor)
{
	RVM_TRACE_DEBUG_HIGH ("trans_pei_monitor");

	return PEI_OK;
} /* End pei_monitor(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_config
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when a primitive is
	 |               received indicating dynamic configuration.
	 |
	 |               This function is not used in this entity.
	 |
	 |Parameters   :  in_string   - configuration string
	 |
	 | Return      : PEI_OK            - sucessful
	 |               PEI_ERROR         - not successful
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_config (char *inString)
{
	RVM_TRACE_DEBUG_HIGH ("trans_pei_config");

	return PEI_OK;

}/* End pei_config(..) */


/*
	 +------------------------------------------------------------------------------
	 |  Function     :  pei_timeout
	 +------------------------------------------------------------------------------
	 |  Description  :  Process timeout.
	 |
	 |  Parameters   :  index     - timer index
	 |
	 |  Return       :  PEI_OK    - timeout processed
	 |                  PEI_ERROR - timeout not processed
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_timeout (unsigned short index)
{
	RVM_TRACE_DEBUG_HIGH ("trans_pei_timeout");

	return PEI_OK;
}/* End pei_timeout(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_signal
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when a signal has been
	 |               received.
	 |
	 | Parameters  : opc               - signal operation code
	 |               *data             - pointer to primitive
	 |
	 | Return      : PEI_OK            - signal processed
	 |               PEI_ERROR         - signal not processed
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
	RVM_TRACE_DEBUG_HIGH ("trans_pei_signal");

	return(PEI_OK);
}/* End pei_signal(..) */


LOCAL VOID primitive_not_supported(void *data)
{
  TRACE_FUNCTION ("primitive_not_supported");
  PFREE (data);
}

/*
	 +------------------------------------------------------------------------------
	 |  Function     :  pei_primitive
	 +------------------------------------------------------------------------------
	 |  Description  :  Process protocol specific primitive.
	 |
	 |  Parameters   :  prim      - pointer to the received primitive
	 |
	 |  Return       :  PEI_OK    - function succeeded
	 |                  PEI_ERROR - function failed
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_primitive (void * primptr)
{
	T_PRIM * prim = primptr;
	bal_sock_api_inst = 0;

	RVM_TRACE_DEBUG_HIGH("trans_pei_primitive");
	if (sock_api_handles_primitive(bal_sock_api_inst, prim) == FALSE)
	{
		if(prim->custom.opc & SYS_MASK)
		{
			vsi_c_primitive(VSI_CALLER prim);
		}
		else
		{
			primitive_not_supported(P2D(prim));
			return PEI_ERROR;
		}
	}
	return PEI_OK;
}/* End pei_primitive(..) */

/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_exit
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when the entity is
	 |               terminated. All open resources are freed.
	 |
	 | Parameters  : -
	 |
	 | Return      : PEI_OK            - exit successful
	 |               PEI_ERROR         - exit not successful
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_exit (void)
{
	RVM_TRACE_DEBUG_HIGH ("trans_pei_exit");

	bal_sock_api_deinitialize();
	exit_flag = 1;

	return PEI_OK;
}/* End pei_exit(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_init
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame. It is used to initialise
	 |               the entitiy.
	 |
	 | Parameters  : handle            - task handle
	 |
	 | Return      : PEI_OK            - entity initialised
	 |               PEI_ERROR         - entity not (yet) initialised
	 |
	 +------------------------------------------------------------------------------
 */

extern BOOL powered_on;

LOCAL SHORT pei_init (T_HANDLE handle)
{
	trans_handle = handle;

	RVM_TRACE_DEBUG_HIGH("trans_pei_init");

  	while(!EmoStatusGet())
        	TCCE_Task_Sleep(100);

	if (TranshComm < VSI_OK)
		if ((TranshComm = vsi_c_open (VSI_CALLER "TRANS")) < VSI_OK)
			return PEI_ERROR;

	TCCE_Task_Sleep(50);

	if (!bal_sock_api_initialize(handle, "TRANS"))
		return PEI_ERROR;

	return PEI_OK;
} /* End pei_init(..) */

/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_create
	 +------------------------------------------------------------------------------
	 | Description :  This function is called by the frame when the process is
	 |                created.
	 |
	 | Parameters  : out_name          - Pointer to the buffer in which to locate
	 |                                   the name of this entity
	 |
	 | Return      : PEI_OK            - entity created successfuly
	 |               PEI_ERROR         - entity could not be created
	 |
	 +------------------------------------------------------------------------------
 */

GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

	static const T_PEI_INFO pei_info =
	{
		"TRANS",         /* name */
		{              /* pei-table */
			pei_init,
			pei_exit,
			pei_primitive,           /* NO pei_primitive */
			NULL,           /* NO pei_timeout */
			pei_signal,           /* NO pei_signal */   
			NULL,        /*-- ACTIVE Entity--*/
			NULL,           /* NO pei_config */
			NULL            /* NO pei_monitor */
		},
		0xc00,            /* stack size */
		0xA,                        /* queue entries */
		BAL_TRANS_PRIORITY,     /* priority (1->low, 255->high) */
		1,                         /* number of timers */
		COPY_BY_REF|PASSIVE_BODY   /* Flags Settings */
	};

	RVM_TRACE_DEBUG_HIGH("trans_pei_create");

	/*
	 * Close Resources if open
	 */
	if (first_access)
		first_access = FALSE;
	else
		pei_exit();

	/*
	 * Export startup configuration data
	 */
	*info = (T_PEI_INFO *)&pei_info;

	return PEI_OK;
}/* End pei_create(..) */

/*======================== END OF FILE =========================================*/
