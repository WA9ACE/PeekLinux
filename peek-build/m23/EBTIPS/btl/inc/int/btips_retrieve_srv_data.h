#ifndef __BTIPS_RETRIEVE_SRV_DATA_H
#define __BTIPS_RETRIEVE_SRV_DATA_H

#include "btips_retrieve_srv.h"

/* The struct that will hold the attribute data */
typedef struct
{
	/* attribute ID */
	SdpAttributeId id;

	/* attribute data size */
	U16  size;

	/* offset to attribute data, from the begining of the service data */
	U16 data_offset;
} BTIPS_RS_ATT_DESC;


U8 * BTIPS_RS_InitBuff(U8 * buff, U16 len, SdpAttributeId * attributeIds, U16 attributeCount);
BTIPS_RS_Status BTIPS_RS_InsertServiceAttributeData(	SdpAttributeId 	attr_id, 
																U8 				*attr_data, 
																U16 				attr_size, 
																U8 				*buff, 
																U16 				buff_len, 
																U8 				mode);
void BTIPS_RS_PrintAttributesDataInternal(U8 * buff);

#define BTIPS_RS_GET_TOT_ATTRIBUTE_COUNT(data_desc_p) \
	((U16 *)data_desc_p)[0]

#define BTIPS_RS_GET_ATTRIBUTE_FOUND_NUM(data_desc_p) \
	((U16 *)data_desc_p)[1]

#define BTIPS_RS_SET_TOT_ATTRIBUTE_COUNT(data_desc_p, attributeCount) \
	((U16 *)data_desc_p)[0] = attributeCount

#define BTIPS_RS_SET_ATTRIBUTE_FOUND_NUM(data_desc_p, attributeFoundCount) \
	((U16 *)data_desc_p)[1] = attributeFoundCount

#define GET_ATTRIBUTE_DESCR(data_desc_p) \
	(BTIPS_RS_ATT_DESC *)((U8 *)data_desc_p + (2 * sizeof(U16)))


#endif /* __BTIPS_RETRIEVE_SRV_DATA_H */

