/**
 * @file	gbi_message.h
 *
 * Data structures:
 * 1) used to send messages to the GBI SWE,
 * 2) GBI can receive.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	12/29/2003	 ()		Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_MESSAGE_H_
#define __GBI_MESSAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "gbi/gbi_api.h"

/** 
 * The message offset must differ for each SWE in order to have 
 * unique msg_id in the system.
 */

#define GBI_MESSAGE_OFFSET	 BUILD_MESSAGE_OFFSET(GBI_USE_ID)


/**
 * @name GBI_GBI_READ_REQ_MSG
 *
 * Detailled description
 * To initiate a read block operation, the client can send the message 
 * T_GBI_READ_REQ_MSG.  This message is similar to the gbi_read() function. The 
 * driver responds with a T_GBI_READ_RSP_MSG message when the read block command is 
 * completed. 
 */
/*@{*/

/** Read request. */
#define GBI_READ_REQ_MSG (GBI_MESSAGE_OFFSET | 0x001)

typedef struct {
    T_RV_HDR 		    hdr;
    UINT8			      media_nmb;
    UINT8         	partition_nmb;
    T_GBI_BLOCK		  first_block_nmb;
    T_GBI_BLOCK		  number_of_blocks;
    T_GBI_BYTE_CNT	remainder_length;
    UINT32		      *buffer_p;
    T_RV_RETURN 		return_path;
} T_GBI_READ_REQ_MSG;

/** Read response. */
#define GBI_READ_RSP_MSG (GBI_MESSAGE_OFFSET | 0x002)

typedef struct {
    T_RV_HDR 		    hdr;
    T_GBI_RESULT	  result;
} T_GBI_READ_RSP_MSG;

/*@}*/

/**
 * @name GBI_GBI_WRITE_REQ_MSG
 *
 * Detailled description
 * To initiate a write block operation, the client can send the message 
 * T_BI_WRITE_REQ_MSG.  This message is similar to the xxx_gbi_write() function.
 * The driver responds with a T_GBI_WRITE_RSP_MSG message when the write block 
 * command is completed.
 */
/*@{*/

/** Write request. */

#define GBI_WRITE_REQ_MSG (GBI_MESSAGE_OFFSET | 0x003)

typedef struct {
    T_RV_HDR 		    hdr;
    UINT8			      media_nmb;
    UINT8          	partition_nmb;
    T_GBI_BLOCK		  first_block_nmb;
    T_GBI_BLOCK		  number_of_blocks;
    T_GBI_BYTE_CNT	remainder_length;
    UINT32		      *buffer_p;
    T_RV_RETURN 		return_path;
} T_GBI_WRITE_REQ_MSG;

/** Write response. */
#define GBI_WRITE_RSP_MSG (GBI_MESSAGE_OFFSET | 0x004)

typedef struct {
    T_RV_HDR 	      hdr;
		T_GBI_RESULT	  result;
} T_GBI_WRITE_RSP_MSG;

/*@}*/

/**
 * @name GBI_ERASE_REQ_MSG
 *
 * Detailled description
 *
 */
/*@{*/

/** Erase request. */

#define GBI_ERASE_REQ_MSG (GBI_MESSAGE_OFFSET | 0x005)

typedef struct {
    T_RV_HDR 		    hdr;
    UINT8			      media_nmb;
    UINT8         	partition_nmb;
    T_GBI_BLOCK		  first_block_nmb;
    T_GBI_BLOCK		  number_of_blocks;
    T_RV_RETURN 		return_path;
} T_GBI_ERASE_REQ_MSG;

/** Erase response. */
#define GBI_ERASE_RSP_MSG (GBI_MESSAGE_OFFSET | 0x006)

typedef struct {
    T_RV_HDR        hdr;
    T_GBI_RESULT    result;
} T_GBI_ERASE_RSP_MSG;

/*@}*/


/**
 * @name GBI_FLUSH_REQ_MSG
 *
 * Detailled description
 * To initiate a flush operation, the client can send the message T_GBI_FLUSH_REQ_MSG.  
 * This message is similar to the gbi_flush() function. The driver responds with 
 * a T_GBI_FLUSH_RSP_MSG message when the flush operation is completed.
 */
/*@{*/

/** Flush request. */

#define GBI_FLUSH_REQ_MSG (GBI_MESSAGE_OFFSET | 0x007)

typedef struct {
    T_RV_HDR 		    hdr;
    UINT8			      media_nmb;
    UINT8         	partition_nmb;
    T_RV_RETURN 		return_path;
} T_GBI_FLUSH_REQ_MSG;

/** Flush response. */
#define GBI_FLUSH_RSP_MSG (GBI_MESSAGE_OFFSET | 0x008)

typedef struct {
    T_RV_HDR        hdr;
    T_GBI_RESULT    result;
} T_GBI_FLUSH_RSP_MSG;

/*@}*/

/**
 * @name GBI_MEDIA_INFO_REQ_MSG
 *
 * Detailled description
 * To retrieve media information, the client can send the message 
 * T_GBI_MEDIA_INFO_REQ_MSG. This message is similar to the gbi_get media_info()
 * function. The driver responds with a T_GBI_MEDIA_INFO_RSP_MSG message when the 
 * operation is completed.
 */
/*@{*/

/** Retrieve media info request. */

#define GBI_MEDIA_INFO_REQ_MSG (GBI_MESSAGE_OFFSET | 0x009)

typedef struct {
    T_RV_HDR 		    hdr;
    T_RVF_MB_ID     mb_id;
    T_RV_RETURN 	  return_path;
} T_GBI_MEDIA_INFO_REQ_MSG;

/** Retrieve media info response. */
#define GBI_MEDIA_INFO_RSP_MSG (GBI_MESSAGE_OFFSET | 0x00A)

typedef struct {
    T_RV_HDR 	        hdr;
		T_GBI_RESULT      result;
		UINT8			        nmb_of_media; //see note
    T_GBI_MEDIA_INFO  *info_p;      //see note
} T_GBI_MEDIA_INFO_RSP_MSG;

/*
 * Note: the members "nmb_of_media" and "info_p" are only valid if the "result" member
 * has the value RV_OK. The "info_p" member is a pointer to an array of structures of 
 * type "T_GBI_MEDIA_INFO". The array is "nmb_of_media" long.
 */

/*@}*/

/**
 * @name GBI_PARTITION_INFO_REQ_MSG
 *
 * Detailled description
 * To retrieve partition information of a media, the client can send the message 
 * T_GBI_PARTITION_INFO_REQ_MSG.  This message is similar to the 
 * gbi_get_partition_info() function. The driver responds with a 
 * T_GBI_PARTITION_INFO_RSP_MSG message when the operation is completed.
 */
/*@{*/

/** Retrieve partition info request. */

#define GBI_PARTITION_INFO_REQ_MSG (GBI_MESSAGE_OFFSET | 0x00B)

typedef struct {
    T_RV_HDR 			  hdr;
    T_RVF_MB_ID     mb_id;
    T_RV_RETURN 	  return_path;
} T_GBI_PARTITION_INFO_REQ_MSG;

/** Retrieve partition info response. */
#define GBI_PARTITION_INFO_RSP_MSG (GBI_MESSAGE_OFFSET | 0x00C)

typedef struct {
    T_RV_HDR 			        hdr;
    T_GBI_RESULT			    result;
    UINT8				          nmb_of_partitions;  //see note
    T_GBI_PARTITION_INFO	*info_p;            //see note
} T_GBI_PARTITION_INFO_RSP_MSG;

/*
 * Note: the members "nmb_of_partitions" and "info_p" are only valid if the "result" 
 * member has the value RV_OK. The "info_p" member is a pointer to an array of 
 * structures of type "T_GBI_MEDIA_INFO". The array is "nmb_of_media" long.
 */

/*@}*/

/**
 * @name GBI_EVENT_IND_MSG
 *
 * Detailled description
 * 
 */
/*@{*/

/** Event message. */
#define GBI_EVENT_IND_MSG (GBI_MESSAGE_OFFSET | 0x00D)

typedef struct {
    T_RV_HDR 	      hdr;
    UINT16	        event;
} T_GBI_EVENT_MSG;

/*@}*/


/**
 * @name GBI_READ_SPARE_DATA_REQ_MSG
 *
 * Detailled description
 * To retrieve block information of a media, the client can send the message 
 * T_GBI_READ_SPARE_DATA_REQ_MSG.  This message is similar to the 
 * gbi_read_spare_data() function. The driver responds with a 
 * T_GBI_READ_SPARE_DATA_RSP_MSG message when the operation is completed.
 */
/*@{*/

/** Read spare data request. */

#define GBI_READ_SPARE_DATA_REQ_MSG (GBI_MESSAGE_OFFSET | 0x00E)

typedef struct {
    T_RV_HDR 		  hdr;
    UINT8         media_nmb;
    UINT8         partition_nmb;
    T_GBI_BLOCK	  first_block;
    T_GBI_BLOCK   number_of_blocks;
    UINT32        *info_data_p;
    T_RV_RETURN   return_path;
} T_GBI_READ_SPARE_DATA_REQ_MSG;

/** Read spare data response. */
#define GBI_READ_SPARE_DATA_RSP_MSG (GBI_MESSAGE_OFFSET | 0x00F)

typedef struct {
    T_RV_HDR 	      hdr;
    T_GBI_RESULT	  result;
} T_GBI_READ_SPARE_DATA_RSP_MSG;

/*@}*/

/**
 * @name GBI_WRITE_WITH_SPARE_REQ_MSG
 *
 * Detailled description
 * To initiate a write block operation with spare data, the client can send the message 
 * T_GBI_WRITE_WITH_SPARE_REQ_MSG.  
 * The driver responds with a T_GBI_WRITE_WITH_SPARE_RSP_MSG message when the write block 
 * command is completed.
 */
/*@{*/

/** Write request. */

#define GBI_WRITE_WITH_SPARE_REQ_MSG (GBI_MESSAGE_OFFSET | 0x010)

typedef struct {
    T_RV_HDR 		    hdr;
    UINT8			      media_nmb;
    UINT8          	partition_nmb;
    T_GBI_BLOCK		  first_block_nmb;
    T_GBI_BLOCK		  number_of_blocks;
    T_GBI_BYTE_CNT	remainder_length;
    UINT32		      *data_buffer_p;
    UINT32		      *spare_buffer_p;    
    T_RV_RETURN 		return_path;
} T_GBI_WRITE_WITH_SPARE_REQ_MSG;

/** Write response. */
#define GBI_WRITE_WITH_SPARE_RSP_MSG (GBI_MESSAGE_OFFSET | 0x011)

typedef struct {
    T_RV_HDR 	      hdr;
		T_GBI_RESULT	  result;
} T_GBI_WRITE_WITH_SPARE_RSP_MSG;

/** Garbage collection request. */
#define GBI_NAND_GC_MSG (GBI_MESSAGE_OFFSET | 0X012)

typedef struct {	
    T_RV_HDR 		hdr;
} T_GBI_NAND_GC_MSG;

/** Asynchronous BTM continue message (just for internal use). */
#define GBI_NAND_BTM_MSG (GBI_MESSAGE_OFFSET | 0X013)

typedef struct {	
    T_RV_HDR 		hdr;
} T_GBI_NAND_BTM_MSG;


#define LAST_USED_GBI_EXTERNAL_MSG_OFFSET 0x013

#ifdef __cplusplus
}
#endif

#endif /* __GBI_MESSAGE_H_ */
