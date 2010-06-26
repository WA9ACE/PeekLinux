#include "btl_configi.h"
#include "xaTypes.h"
#include "bttypes.h"

BTL_CONFIGI_INIT_DECLARATIONS
BTL_CONFIGI_BTL_INIT_DECLARATIONS

#define _BTL_INIT_PROTOTYPE(a) 	BOOL a##_Init(void)
#define _BTL_BTL_INIT_PROTOTYPE(a) 	BtStatus BTL_##a##_Init(void)
#define _BTL_BTL_DEINIT_PROTOTYPE(a) 	BtStatus BTL_##a##_Deinit(void)

#define _BTL_STACK_STUB_INIT(a)	\
	_BTL_INIT_PROTOTYPE(a)			\
	{							\
		return TRUE;			\
	}

#define _BTL_BTL_STUB_INIT(a)				\
	_BTL_BTL_INIT_PROTOTYPE(a)				\
	{										\
		return BT_STATUS_SUCCESS;			\
	}										\
											\
	_BTL_BTL_DEINIT_PROTOTYPE(a)			\
	{										\
		return BT_STATUS_SUCCESS;			\
	}
  


/*
	AG
*/
#if (BTL_CONFIG_AG == BTL_CONFIG_DISABLED)
	_BTL_STACK_STUB_INIT(HFG)
	_BTL_STACK_STUB_INIT(HSG)
	/*_BTL_BTL_STUB_INIT(AG) - Take Off only BTL stub */
#endif

