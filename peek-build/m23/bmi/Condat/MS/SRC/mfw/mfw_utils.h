/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	$Workfile:: mfw_utils.h			$|
| $Author:: NDH						$Revision:: 1					$|
| CREATED: 6.1.2003		       		$Modtime:: 10.04.00 14:58		$|
| STATE  : code														$|
+--------------------------------------------------------------------+

   MODULE  : MFW_UTILS

   PURPOSE : This modul contains General Functional Utilities.

   HISTORY:
   Jun 05, 2004	REF: CRR 18262  NISHIKANT KULKARNI 
   Description: The sample sends a STOP DTMF message without release of the key by the user
   Solution: Instead of sending DTMF commands in "VTS_MOD_Auto" mode, on key press DTMF tone is started
  		   using VTS_MOD_ManStart and on key release DTMF tone is stopped using VTS_MOD_ManStop mode.

*/

/*
** Limiting Macros
*/
#define MAX_CBUF_QUEUES 3

/*
** Return Value Macros
*/
#define MFW_CBUF_OK					1
#define MFW_CBUF_OK_DATA_LOSS		2

#define MFW_CBUF_INVALID_BUF_ID			(-1)
#define MFW_CBUF_NO_BUFS_AVAILABLE		(-2)
#define MFW_CBUF_MEM_ALLOC_FAILURE		(-3)
#define MFW_CBUF_INVALID_ITEM_SIZE		(-4)
#define MFW_CBUF_INVALID_MAX_ITEMS		(-5)
#define MFW_CBUF_INVALID_BUF_PTR		(-6)
#define MFW_CBUF_INVALID_STATIC_BUF	 (-7)
#define MFW_CBUF_PUT_FAILED_Q_FULL		(-8)
#define MFW_CBUF_BUFFER_EMPTY			(-9)

/*
** Function Prototypes
    xnkulkar SPR-18262: Added the prototypes for  mfw_cbuf_put_mode() and  mfw_cbuf_get_mode()
*/
SHORT mfw_cbuf_create (USHORT max_num_items, USHORT item_size,
						 UBYTE overwrite, UBYTE null_char,
						 UBYTE static_buf, void *buffer_ptr);
SHORT mfw_cbuf_delete (SHORT bufId);
SHORT mfw_cbuf_reset (SHORT bufId);
SHORT mfw_cbuf_put (SHORT bufId, void *data_ptr);
SHORT mfw_cbuf_get (SHORT bufId, void *data_ptr);
SHORT  mfw_cbuf_put_mode (SHORT bufId,UBYTE vts_mode); //xnkulkar SPR-18262
SHORT  mfw_cbuf_get_mode  (SHORT bufId); //xnkulkar SPR-18262
USHORT mfw_cbuf_num_elements (SHORT bufId);

