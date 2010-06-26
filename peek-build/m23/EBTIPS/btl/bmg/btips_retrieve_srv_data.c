/*******************************************************************************\
*
*   FILE NAME:      btips_retrieve_srv_data.c
*
*   DESCRIPTION:    This file implements the functions for holding
*				  the data retrieved from SDP server in the application supplied buffer.
*
*   AUTHOR:         Yuval Hevrony
*
\*******************************************************************************/

/* descripton of buffer's data */
/*
 * The buff in the BTIPS_RS_SERVICE_SEARCH_ATTR_PARAMS structure holds the services attributes data.
 * buffer structure:
 * service data record structure:
 *
 * |attribute count (2 ) |attribute found num (2 ) |attr1 data description (sizeof(BTIPS_RS_ATT_DESC))|...|attrK data description (sizeof(BTIPS_RS_ATT_DESC))|attr1 value|...|attrK value|
 *
 * attr data description:
 * |attr ID (2)|attr size (2)|attr data offset from service starting(2)|
 */

#include "btips_retrieve_srv.h"
#include "btips_retrieve_srv_data.h"
#include "stdio.h"
#include "btl_defs.h"
#include "btl_log_modules.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

#define MAX_QUERY_PARAMS_SIZE 300

/* Functions Prototypes */
static U8 * BTIPS_RS_GetServicePosition(U8 *, U16);
static void BTIPS_RS_InitServiceAttributes(SdpAttributeId *, U16, U8 *);

/*
 * Min service size is the sum of:
 * 1) size of attribute requested count field 
 * 2) size of attribute (U16)found count field (U16)
 * 3) size of attribute description array 
 */
#define MIN_SERVICE_DATA_SIZE(attributeCount) \
	(2*sizeof(U16))+sizeof(BTIPS_RS_ATT_DESC)*attributeCount

/*
 * The function initializes the data buffer
 * and the first service's attribute list
 */
U8 * BTIPS_RS_InitBuff(U8 * buff, U16 len, SdpAttributeId * attributeIds, U16 attributeCount)
{
	U16 i;
	/* min size is: first service attr count,first service attr found count 
		and first service attributes description
	 */
	U16 buff_min_size = (U16)(MIN_SERVICE_DATA_SIZE(attributeCount));
	BTIPS_RS_ATT_DESC *attr_desc_p;
	
	/* Check if buffer is not too small */
	if (len < buff_min_size) {
		BTL_LOG_DEBUG(("BTIPS_RS_InitBuff: Buff min size : %d. bigger then actual %d", buff_min_size, len));  
		return 0;
	}
	
	/* Get attribute description starting point */
	attr_desc_p = GET_ATTRIBUTE_DESCR(buff);
		
	/* set attribute count */
	BTIPS_RS_SET_TOT_ATTRIBUTE_COUNT(buff, attributeCount);

	/* set attribute found count */
	BTIPS_RS_SET_ATTRIBUTE_FOUND_NUM(buff, 0);

	/* Attribute description has 3 fields:
	 * ID, size and data.
	 * Walk overthe attribute description array ant initialize it */
	for (i=0; i < attributeCount; ++i) {
		attr_desc_p[i].id = attributeIds[i];
		attr_desc_p[i].size = 0;
		attr_desc_p[i].data_offset = 0;
	}

	BTL_LOG_DEBUG(("tot = %d, buff = %p", attributeCount, buff));

	return buff;
}

/*
 * insert attribute data.
 * return ptr to the attr data
 * The mode parameter indicates if to replace or append the data
 * 0 - replace (or add new)
 * 1 - append
 */
 BTIPS_RS_Status BTIPS_RS_InsertServiceAttributeData(	SdpAttributeId 	attr_id, 
										 	   U8 * attr_data,
										 	   U16 attr_size,
										 	   U8 * buff,
										 	   U16 buff_len,
										 	   U8 mode)
{
	U16 i;
	U16 last_size = 0;
	U16 attributeCount;	/* number of user requested attributes */
	U16 attribute_data_start_pos;	/* start of attributes data.*/
	BTIPS_RS_ATT_DESC * attPos;	/*attribute descriptor */

	attributeCount = BTIPS_RS_GET_TOT_ATTRIBUTE_COUNT(buff);
	attPos = GET_ATTRIBUTE_DESCR(buff);
	attribute_data_start_pos = (U16)(MIN_SERVICE_DATA_SIZE(attributeCount));
	
	
	/* search for the attribute Id.
	 * In order to find the data position,
	 * find the previous attribute data ending pos */
	for (i=0; i < attributeCount; ++i) {
		if (attr_id < attPos[i].id)
			break;

		if (attPos[i].id == attr_id) 
		{			
			/* add new */
			if (mode==0 || attPos[i].data_offset==0) 
			{
				attPos[i].data_offset = (U16)(attribute_data_start_pos+last_size);
				attPos[i].size = attr_size;
				if (buff+attribute_data_start_pos+last_size+attr_size > buff+buff_len) 
				{
					BTL_LOG_DEBUG(("BTIPS_RS_InsertServiceAttributeData: Cannot insert attribute %d. Buff too small",attr_id));
					return BTIPS_RS_STATUS_BUFFER_OVERFLOW;
				}	
				OS_MemCopy(buff+attribute_data_start_pos+last_size, attr_data, attr_size);
			} 
			else 
			{ /* Append */
				if (buff+attribute_data_start_pos+last_size+attPos[i].size+attr_size > buff+buff_len) 
				{
					BTL_LOG_DEBUG(("BTIPS_RS_InsertServiceAttributeData: Cannot insert attribute %d . Buff too small",attr_id));
					return BTIPS_RS_STATUS_BUFFER_OVERFLOW;
				}	
				OS_MemCopy(buff+attribute_data_start_pos+last_size+attPos[i].size, attr_data, attr_size);
				attPos[i].size = (U16)(attPos[i].size + attr_size);
			}

			return BTIPS_RS_STATUS_OK;
		}
		else if (attPos[i].data_offset != 0) 
		{
			attribute_data_start_pos = attPos[i].data_offset;
			last_size = attPos[i].size;

		}
	}

	/* REPORT attribute ID not found */
	return BTIPS_RS_STATUS_ATTIRBUTE_ID_NO_FOUND;
}


/*
 * The function returns a pointer to the attribute data
 * for a given service and attribute ID
 */
U8 * BTIPS_RS_GetAttributeData(U8 * buff, SdpAttributeId attributeId, U16 *size)
{
	int i;
	U16 attributeCount;
	BTIPS_RS_ATT_DESC * attr_desc;

	if (buff == 0)
	{
		BTL_LOG_DEBUG(("BTIPS_RS_GetAttributeData: Null buffer. Cannot get attributes"));
		return 0;
	}

	attributeCount = BTIPS_RS_GET_TOT_ATTRIBUTE_COUNT(buff);
	attr_desc = GET_ATTRIBUTE_DESCR(buff);

	*size = 0;

	/* search for the attribute ID.
	 * All attributes ID must be sorted asc. order */
	for (i=0; i < attributeCount; ++i) {
		if ((attributeId == attr_desc[i].id) && (attr_desc[i].size > 0)) {
			*size = attr_desc[i].size;
			return buff + attr_desc[i].data_offset;
		}
		else if (attributeId < attr_desc[i].id) {
			BTL_LOG_DEBUG(("BTIPS_RS_GetAttributeData: Attribute ID %d not found ", attributeId));
			return 0;
		}
	}

	return 0;
}

/*
  * Used for debug. print the attributes that were kept
  */
void BTIPS_RS_PrintAttributesDataInternal(U8 * buff)
{
	int i;
	U16 attributeCount;
	BTIPS_RS_ATT_DESC * attPos;

	if (buff == 0) {
		BTL_LOG_DEBUG(("BTIPS_RS_PrintAttributesDataInternal: Null buffer. Cannot print attributes"));
		return;
	}
		
	attributeCount = BTIPS_RS_GET_TOT_ATTRIBUTE_COUNT(buff);
	attPos = GET_ATTRIBUTE_DESCR(buff);

	for (i=0; i<attributeCount; ++i) {
		BTL_LOG_DEBUG(("I : %d, ID: %d, size %d, offset %d",i,  attPos[i].id, attPos[i].size, attPos[i].data_offset));
		if (attPos[i].size > 0) {
			BTL_LOG_DEBUG(("Attribute %d desc pos : %p", i, attPos+i));
			BTL_LOG_DEBUG(("Attribute %d data pos : %p", i, buff+attPos[i].data_offset));
			BTL_LOG_DEBUG(("value: %s", buff+ attPos[i].data_offset));
		}
	}
}

