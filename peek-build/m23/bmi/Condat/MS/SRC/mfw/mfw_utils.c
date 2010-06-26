/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	$Workfile:: mfw_utils.c			$|
| $Author:: NDH						$Revision:: 1					$|
| CREATED: 6.1.2003		       		$Modtime:: 10.04.00 14:58		$|
| STATE  : code														$|
+--------------------------------------------------------------------+

   MODULE  : MFW_UTILS

   PURPOSE : This modul contains General Functional Utilities.

   HISTORY:
	Oct 04, 2004    REF: CRR 25519     Deepa M.D
	Bug:Re-align structure members in MFW
	Fix:Structure elements have been  realigned to avoid the structure padding

   Jun 05, 2004	REF: CRR 18262  NISHIKANT KULKARNI 
   Description: The sample sends a STOP DTMF message without release of the key by the user
   Solution: Instead of sending DTMF commands in "VTS_MOD_Auto" mode, on key press DTMF tone is started
  		   using VTS_MOD_ManStart and on key release DTMF tone is stopped using VTS_MOD_ManStop mode.

*/

#include <string.h>


#if defined (NEW_FRAME)

#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "custom.h"
#include "gsm.h"

#else

#include "STDDEFS.H"
#include "custom.h"
#include "gsm.h"
#include "vsi.h"

#endif

#include "mfw_mfw.h"
#include "mfw_utils.h"

#include "cus_aci.h"
#include "prim.h"
#ifndef PCM_2_FFS
#include "pcm.h"
#endif


// xnkulkar SPR-18262: This length of array for storing DTMF mode (Start/Stop), is equal to the number 
// of DTMF tone requests that can be stored in queue.
#define MAX_DTMF_Q_ENTRIES 50
/***************************Go-lite Optimization changes start***********************/
//Oct 04, 2004    REF: CRR 25519     Deepa M.D
//Structure elements (T_MFW_CBUF_HEADER)are realigned to avoid the structure padding
typedef struct
{	//xnkulkar SPR-18262: This member is used to store the mode for various DTMF tones present in queue to be sent.
	UBYTE vts_mode[MAX_DTMF_Q_ENTRIES];
	USHORT item_size;
	UBYTE *mfw_cb;
	ULONG mfw_cb_read_pos;
	ULONG mfw_cb_write_pos;
	USHORT num_elements;
	USHORT max_num_items;
	UBYTE null_char;
	UBYTE overwrite;
	UBYTE static_buf;
	UBYTE active;
} T_MFW_CBUF_HEADER;
/***************************Go-lite Optimization changes end***********************/

static T_MFW_CBUF_HEADER cbf_hdr[MAX_CBUF_QUEUES];
static UBYTE cbf_num_of_buffers;


/*
** Static Function Prototypes
*/
static SHORT mfw_cbuf_get_free_id(void);

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_init          |
+--------------------------------------------------------------------+


   PURPOSE :  Create and Initialise the Circular Buffer

*/
SHORT mfw_cbuf_create (USHORT max_num_items, USHORT item_size,
						 UBYTE overwrite, UBYTE null_char,
						 UBYTE static_buf, void *buffer_ptr)
{
	SHORT bufId;
	
	if (cbf_num_of_buffers < MAX_CBUF_QUEUES)
	{
		/*
		** Get the first available Id for a free buffer
		*/
		bufId = mfw_cbuf_get_free_id();
		if (bufId == MFW_CBUF_NO_BUFS_AVAILABLE)
			return (MFW_CBUF_NO_BUFS_AVAILABLE);
	}
	else
	{
		/*
		** return -1 as an indication that there are no more buffer handles available
		*/
		return (MFW_CBUF_NO_BUFS_AVAILABLE);
	}

	/*
	** We now have a valid bufferId but check the validity of the incoming parameters
	** before we allocate the buffer.
	*/
	if (item_size == 0)
		return (MFW_CBUF_INVALID_ITEM_SIZE);

	if (max_num_items == 0)
		return (MFW_CBUF_INVALID_MAX_ITEMS);

	if ((static_buf) && (buffer_ptr == (void *)0))
		return (MFW_CBUF_INVALID_BUF_PTR);

	if ((!static_buf) && (buffer_ptr != (void *)0))
		return (MFW_CBUF_INVALID_BUF_PTR);
	
	/*
	** Set the selected buffer to active
	*/
	cbf_hdr[bufId].active = TRUE;

	if (!static_buf)
	{
		/*
		** If the buffer isn't static, then get the dynamic memory for it.
		*/
		cbf_hdr[bufId].mfw_cb = mfwAlloc(item_size * max_num_items);
		cbf_hdr[bufId].static_buf = FALSE;

		if (cbf_hdr[bufId].mfw_cb == (void *)0)
		{
			/*
			** The memory Allocation failed, mark the buffer as inactive and return an error
			*/
			cbf_hdr[bufId].active = FALSE;
			return (MFW_CBUF_MEM_ALLOC_FAILURE);
		}
	}
	else
	{
		/*
		** If the buffer is static, the calling function will have provided a
		** pointer to the buffer to be used.
		*/
		cbf_hdr[bufId].mfw_cb = (UBYTE *)buffer_ptr;
		cbf_hdr[bufId].static_buf = TRUE;
	}

	/*
	** Initialise the buffer header which contains the information needed to
	** maintain the buffer
	*/
	cbf_hdr[bufId].max_num_items = max_num_items;
	cbf_hdr[bufId].item_size = item_size;
	cbf_hdr[bufId].null_char = null_char;
	cbf_hdr[bufId].overwrite = overwrite;

	/*
	** The buffer is created with no elements in it.
	*/
	cbf_hdr[bufId].num_elements = 0;
	cbf_hdr[bufId].mfw_cb_read_pos = 0;
	cbf_hdr[bufId].mfw_cb_write_pos = 0;

	/*
	** Ensure the buffer is initialised with the required null character
	*/
	memset(cbf_hdr[bufId].mfw_cb,
			cbf_hdr[bufId].null_char,
			item_size * max_num_items);

	return (bufId);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_delete	     |
+--------------------------------------------------------------------+


   PURPOSE :  delete a dynamically allocated buffer, freeing the memory
   			  A statically allocated buffer CANNOT be released.

*/
SHORT mfw_cbuf_delete (SHORT bufId)
{
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return (MFW_CBUF_INVALID_BUF_ID);

	if (cbf_hdr[bufId].active == FALSE)
		return (MFW_CBUF_INVALID_BUF_ID);

	if (cbf_hdr[bufId].static_buf)
		return (MFW_CBUF_INVALID_STATIC_BUF);

	/*
	** Free the dynamically allocated memory and set buffer to inactive
	** all the other information is irrelevant as once the buffer is marked inactive
	** it cannot be used.
	*/
	mfwFree(cbf_hdr[bufId].mfw_cb,
			 cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);

	cbf_hdr[bufId].active = FALSE;

	return (MFW_CBUF_OK);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_reset	     |
+--------------------------------------------------------------------+


   PURPOSE :  clears all the data from the buffer and resets the read and write pointers.

*/
SHORT mfw_cbuf_reset (SHORT bufId)
{
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return (MFW_CBUF_INVALID_BUF_ID);

	if (cbf_hdr[bufId].active == FALSE)
		return (MFW_CBUF_INVALID_BUF_ID);

	/*
	** The buffer is reset with no elements in it.
	*/
	cbf_hdr[bufId].num_elements = 0;
	cbf_hdr[bufId].mfw_cb_read_pos = 0;
	cbf_hdr[bufId].mfw_cb_write_pos = 0;

	/*
	** Ensure the buffer is initialised with the required null character
	*/
	memset(cbf_hdr[bufId].mfw_cb,
			cbf_hdr[bufId].null_char,
			cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);

	return (MFW_CBUF_OK);
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_put	         |
+--------------------------------------------------------------------+


   PURPOSE :  Add an item to the appropriate circular buffer

*/
SHORT mfw_cbuf_put (SHORT bufId, void *data_ptr)
{
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return (MFW_CBUF_INVALID_BUF_ID);

	if (cbf_hdr[bufId].active == FALSE)
		return (MFW_CBUF_INVALID_BUF_ID);

	if (data_ptr == (void *)0)
		return (MFW_CBUF_INVALID_BUF_PTR);

	if (cbf_hdr[bufId].num_elements < cbf_hdr[bufId].max_num_items)
	{
		/*
		** Standard Add data ... no data lost.
		** Memcpy the input data into the circular buffer
		*/
		memcpy(&cbf_hdr[bufId].mfw_cb[cbf_hdr[bufId].mfw_cb_write_pos],
				data_ptr,
				cbf_hdr[bufId].item_size);

		/*
		** Move the write_pointer along to the next required position
		*/
		cbf_hdr[bufId].mfw_cb_write_pos = (cbf_hdr[bufId].mfw_cb_write_pos + cbf_hdr[bufId].item_size) %
										(cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);
		cbf_hdr[bufId].num_elements++;
		return (MFW_CBUF_OK);
	}
	else if (cbf_hdr[bufId].overwrite)
	{
		/*
		** Overwrite Add data ... The oldest Data in the buffer is lost.
		** Memcpy the input data into the circular buffer
		*/
		memcpy(&cbf_hdr[bufId].mfw_cb[cbf_hdr[bufId].mfw_cb_write_pos],
				data_ptr,
				cbf_hdr[bufId].item_size);

		/*
		** Move the write_pointer along to the next required position
		*/
		cbf_hdr[bufId].mfw_cb_write_pos = (cbf_hdr[bufId].mfw_cb_write_pos + cbf_hdr[bufId].item_size) %
										(cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);
		/*
		** Move the read pointer along to the next required position as the data it points to
		** has been overwritten
		*/
		cbf_hdr[bufId].mfw_cb_read_pos = (cbf_hdr[bufId].mfw_cb_read_pos + cbf_hdr[bufId].item_size) %
									(cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);
		return (MFW_CBUF_OK_DATA_LOSS);
	}
	else
	{
		/*
		** The Queue is full ... return an error
		*/
		return (MFW_CBUF_PUT_FAILED_Q_FULL);
	}
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_get	         |
+--------------------------------------------------------------------+


   PURPOSE :  Get an event from the DTMF Q

*/
SHORT mfw_cbuf_get (SHORT bufId, void *buffer_ptr)
{
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return (MFW_CBUF_INVALID_BUF_ID);

	if (cbf_hdr[bufId].active == FALSE)
		return (MFW_CBUF_INVALID_BUF_ID);

	if (buffer_ptr == (void *)0)
		return (MFW_CBUF_INVALID_BUF_PTR);

	if (cbf_hdr[bufId].num_elements != 0)
	{
		/*
		** Copy the data from the internal buffer into the output buffer, and reset the internal buffer
		*/
		memcpy(buffer_ptr,
				 &cbf_hdr[bufId].mfw_cb[cbf_hdr[bufId].mfw_cb_read_pos],
				 cbf_hdr[bufId].item_size);
		
		memset(&cbf_hdr[bufId].mfw_cb[cbf_hdr[bufId].mfw_cb_read_pos],
				cbf_hdr[bufId].null_char,
				cbf_hdr[bufId].item_size);
		
		/*
		** Move the read pointer along to the next required position
		*/
		cbf_hdr[bufId].mfw_cb_read_pos = (cbf_hdr[bufId].mfw_cb_read_pos + cbf_hdr[bufId].item_size) %
									(cbf_hdr[bufId].item_size * cbf_hdr[bufId].max_num_items);

		cbf_hdr[bufId].num_elements--;

		return (MFW_CBUF_OK);
	}
	else
	{
		/*
		** The Queue is empty ... put valid (null) data into the buffer and return an error
		*/
		memset(buffer_ptr,
				cbf_hdr[bufId].null_char,
				cbf_hdr[bufId].item_size);

		return (MFW_CBUF_BUFFER_EMPTY);
	}
}


/*
+-----------------------------------------------------------------------+
|xnkulkar SPR-18262														 |
|ROUTINE: SHORT mfw_cbuf_put_mode()										 |
|PURPOSE :  Put the mode (Start/Stop) for the DTMF tone in queue					 |
|                   												 				 |
+-----------------------------------------------------------------------+
*/

SHORT  mfw_cbuf_put_mode (SHORT bufId,UBYTE vts_mode)
{
	TRACE_FUNCTION("mfw_cbuf_put_mode()");
	
	// Check for the validity of buffer ID and "limit" for the number of elements
	// if ok, put the mode (Start / Stop) for the specified DTMF tone in the queue
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return MFW_CBUF_INVALID_BUF_ID;
	if (cbf_hdr[bufId].num_elements >= cbf_hdr[bufId].max_num_items)
		return MFW_CBUF_PUT_FAILED_Q_FULL;
	cbf_hdr[bufId].vts_mode[cbf_hdr[bufId].mfw_cb_write_pos] = vts_mode;
	return (MFW_CBUF_OK);
}


/*
+-----------------------------------------------------------------------+
|xnkulkar SPR-18262														 |
|ROUTINE: SHORT mfw_cbuf_get_mode()										 |
|PURPOSE :  Get the mode (Start/Stop) for the DTMF tone in queue					 |
|                 			 													 | 			
+-----------------------------------------------------------------------+
*/
SHORT  mfw_cbuf_get_mode  (SHORT bufId)
{
	TRACE_FUNCTION("mfw_cbuf_get_mode()");

	// Check for the validity of buffer ID and "limit" for the number of elements
	// if ok,  return the mode (Start / Stop) for the requested DTMF tone
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return MFW_CBUF_INVALID_BUF_ID;
	if (cbf_hdr[bufId].num_elements >= cbf_hdr[bufId].max_num_items)
		return MFW_CBUF_PUT_FAILED_Q_FULL;
	return cbf_hdr[bufId].vts_mode[cbf_hdr[bufId].mfw_cb_read_pos];
}

/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417) 	MODULE: MFW_CBUF	             |
| STATE  : code 			ROUTINE: (static) mfw_cbuf_num_elements  |
+--------------------------------------------------------------------+


   PURPOSE :  Get the number of events on the DTMF Q

*/
USHORT mfw_cbuf_num_elements (SHORT bufId)
{
	/*
	** In this function, if the buffer Id is invalid in any way, we will need to
	** return 0 for the number of elements and know that any other action on
	** the buffer will result in an error state.
	*/
	if ((bufId < 0) || (bufId >= MAX_CBUF_QUEUES))
		return (0);

	if (cbf_hdr[bufId].active == FALSE)
		return (0);

	/*
	** Having got to here, the buffer id is valid so return the number of elements
	*/
	return (cbf_hdr[bufId].num_elements);
}

static SHORT mfw_cbuf_get_free_id(void)
{
	SHORT i;

	for (i=0; i<MAX_CBUF_QUEUES; i++)
	{
		if (cbf_hdr[i].active == FALSE)
		{
			/*
			** This is the first inactive buffer, pass the index back
			*/
			return (i);
		}
	}

	/*
	** There are no inaqctive buffers, return an Error
	*/
	return (MFW_CBUF_NO_BUFS_AVAILABLE);
}

