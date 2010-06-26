#ifndef __GBI_PI_DATALIGHT_I_H_
#define __GBI_PI_DATALIGHT_I_H_

#include "gbi_i.h"
#include "gbi_pi_cfg.h"
#include "gbi_cfg.h"


extern void			rvf_delay(UINT32 ticks);
typedef  short    T_GBI_RETURN;

#define GBI_DATALIGHT_ASSERT_PREFIX "GBI_DATALIGHT Assertion failed: "


#define GBI_DATALIGHT_ASSERT GBI_ASSERT

#define GBI_DATALIGHT_ASSERT2 GBI_ASSERT2

#define DL_DBG_TRACE

static T_GBI_RESULT gbi_plugin_datalight_handle_multiple_request(T_RV_HDR  *msg_p,
                                                          void      **data_p, 
                                                          void      **nmb_items_p);

static T_GBI_RESULT gbi_plugin_datalight_handle_single_request(T_RV_HDR *msg_p);
static T_GBI_RESULT gbi_plugin_datalight_handle_response(T_RV_HDR *msg_p);


static T_GBI_RESULT gbi_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p,
                                           void **data_p, 
                                           void **nmb_items_p);

static T_GBI_RESULT gbi_get_media_info_req( T_GBI_MEDIA_INFO_REQ_MSG *msg_p, 
                                           void **data_p, 
                                           void **nmb_items_p);



static T_GBI_RESULT gbi_datalight_init_req(unsigned short disk_num);
static T_GBI_RESULT gbi_read_req(T_GBI_READ_REQ_MSG *msg_p);
static T_GBI_RESULT gbi_write_req(T_GBI_WRITE_REQ_MSG *msg_p);
static T_GBI_RESULT gbi_erase_req(T_GBI_ERASE_REQ_MSG *msg_p);
static T_GBI_RESULT gbi_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p);



static T_GBI_RETURN gbi_datalight_wt_for_read_rsp(T_RV_HDR *msg_p);
static T_GBI_RETURN gbi_datalight_wt_for_write_rsp(T_RV_HDR *msg_p);
static T_GBI_RETURN gbi_datalight_wt_for_erase_rsp(T_RV_HDR *msg_p);

static T_GBI_RETURN gbi_datalight_read_resp(T_GBI_RESULT read_result, T_RV_RETURN return_path);
static T_GBI_RETURN gbi_datalight_write_resp(T_GBI_RESULT write_result, T_RV_RETURN return_path);
static T_GBI_RETURN gbi_datalight_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN return_path);
static T_GBI_RESULT gbi_flush_resp(T_GBI_RESULT flush_result, T_RV_RETURN  return_path);



T_GBI_RETURN gbi_datalight_hndl_read_req(T_GBI_READ_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT   media_bytes_per_blocks,
                                 T_GBI_BLOCK      first_block_nmb);

T_GBI_RETURN gbi_datalight_hndl_write_req(T_GBI_WRITE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK      first_block_nmb);
T_GBI_RETURN gbi_datalight_hndl_erase_req(T_GBI_ERASE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK        first_block_nmb);


typedef enum {
	DATALIGHT_OK			= 0,
	DATALIGHT_NOT_SUPPORTED		= -1,
	DATALIGHT_NOT_READY		= -2,
	DATALIGHT_MEMORY_WARNING	= -3,
	DATALIGHT_MEMORY_ERR		= -4,
	DATALIGHT_MEMORY_REMAINING	= -5,
	DATALIGHT_INTERNAL_ERR		= -6,
	DATALIGHT_INVALID_PARAMETER	= -7,
	DATALIGHT_INVALID_BLOCK_ERR    	= -8,
	DATALIGHT_PAGE_NOT_EMPTY_ERR   	= -9,
	DATALIGHT_ECC_ERROR            	= -11,
	DATALIGHT_ALLOCATION_ERROR     	= -12

} T_DATALIHGT_RET;




														/* The partition number starts from 1 */
#define GBI_DATALIGHT_MAX_NR_OF_PARTITIONS              (GBI_MAX_NR_OF_PARTITIONS+1)



#define GBI_DATALIGHT_MEDIA_NR_0                    (GBI_NOR_BM_MEDIA_NR_0) 
#define GBI_DATALIGHT_MEDIA_0_NR_OF_PARTITIONS      (0x01)  
#define GBI_DATALIGHT_MEDIA_0_MEDIA_TYPE            (GBI_INTERNAL_NOR)
#define GBI_DATALIGHT_MEDIA_0_MEDIA_ID              (0x00000001)
#define GBI_DATALIGHT_MEDIA_0_BYTES_PER_BLOCK       (512)
// These values are NOT kb/s but a 'speed-factor' that gets converted to kb/sec 
// by RFS (in 'rfs_stat()') with the following constants:
// RFS_READ_SPEED_DIVISION_FACTOR and RFS_WRITE_SPEED_DIVISION_FACTOR
#define GBI_DATALIGHT_MEDIA_0_READ_SPEED            (950 * 2000L)   /* 950 kbits/sec */
#define GBI_DATALIGHT_MEDIA_0_WRITE_SPEED           (480 * 8400L)   /* 480 kbits/sec */
#define GBI_DATALIGHT_MEDIA_0_ADMIN_NBR		   		(257*32)          /* include MBR with this */


#define GBI_DATALIGHT_MEDIA_0_PARTITION_NR_0	    (0)
#define GBI_DATALIGHT_MEDIA_0_PAR_0_FILESYSTEM      (GBI_FAT12)
#define GBI_DATALIGHT_MEDIA_0_PAR_0_NMB_OF_BLOCKS	(4092*32-GBI_DATALIGHT_MEDIA_0_ADMIN_NBR)
#define GBI_DATALIGHT_MEDIA_0_PAR_0_FIRST_BLOCK     (0)
#define GBI_DATALIGHT_MEDIA_0_PAR_0_LAST_BLOCK      (GBI_DATALIGHT_MEDIA_0_PAR_0_NMB_OF_BLOCKS)
#define GBI_DATALIGHT_MEDIA_0_PAR_0_NAME	  		"NOR\0"
#define GBI_DATALIGHT_MEDIA_0_FS_NAME		   		"rfsfat\0"





#define GBI_DATALIGHT_MEDIA_NR_1                    (GBI_NAND_BM_MEDIA_NR_0) 
#define GBI_DATALIGHT_MEDIA_1_NR_OF_PARTITIONS      (0x01)  
#define GBI_DATALIGHT_MEDIA_1_MEDIA_TYPE            (GBI_INTERNAL_NAND)
#define GBI_DATALIGHT_MEDIA_1_MEDIA_ID              (0x00000001)
#define GBI_DATALIGHT_MEDIA_1_BYTES_PER_BLOCK       (512)
// These values are NOT kb/s but a 'speed-factor' that gets converted to kb/sec 
// by RFS (in 'rfs_stat()') with the following constants:
// RFS_READ_SPEED_DIVISION_FACTOR and RFS_WRITE_SPEED_DIVISION_FACTOR
#define GBI_DATALIGHT_MEDIA_1_READ_SPEED            (950 * 2000L)   /* 950 kbits/sec */
#define GBI_DATALIGHT_MEDIA_1_WRITE_SPEED           (480 * 8400L)   /* 480 kbits/sec */
#define GBI_DATALIGHT_MEDIA_1_ADMIN_NBR		   		(257*32)          /* include MBR with this */


#define GBI_DATALIGHT_MEDIA_1_PARTITION_NR_0	    (0)
#define GBI_DATALIGHT_MEDIA_1_PAR_0_FILESYSTEM      (GBI_FAT16_LBA)
#define GBI_DATALIGHT_MEDIA_1_PAR_0_NMB_OF_BLOCKS	(4092*32-GBI_DATALIGHT_MEDIA_1_ADMIN_NBR)
#define GBI_DATALIGHT_MEDIA_1_PAR_0_FIRST_BLOCK     (0)
#define GBI_DATALIGHT_MEDIA_1_PAR_0_LAST_BLOCK      (GBI_DATALIGHT_MEDIA_1_PAR_0_NMB_OF_BLOCKS)
#define GBI_DATALIGHT_MEDIA_1_PAR_0_NAME	  		"NANDA0\0"
#define GBI_DATALIGHT_MEDIA_1_FS_NAME		   		"rfsfat\0"

typedef enum
{
	GBI_DATALIGHT_NOT_INITIALISED,
	GBI_DATALIGHT_IDLE,
	GBI_DATALIGHT_INTERNAL_ERROR,
	GBI_DATALIGHT_WT_FOR_INIT_RSP,
	/** gbi_read*/
	GBI_DATALIGHT_WT_FOR_READ_RSP,

	/** gbi_write*/
	GBI_DATALIGHT_WT_FOR_WRITE_RSP,

	/** gbi_erase*/
	GBI_DATALIGHT_WT_FOR_ERASE_RSP
} T_GBI_DATALIGHT_INTERNAL_STATE;


/**
 * The Control Block buffer of GBI, which gathers all 'Global variables'
 * used by GBI instance.
 *
 * A structure should gathers all the 'global variables' of GBI instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_GBI_ENV_CTRL_BLK buffer is allocated when creating GBI instance and is 
 * then always refered by GBI instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{

	T_GBI_DATALIGHT_INTERNAL_STATE state;
	BOOL 				  initialised;
	
	T_GBI_MEDIA_INFO      media_tbl[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
	T_GBI_PARTITION_INFO  partition_tbl[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
	T_RV_RETURN  	  	  org_msg_retpath;  
	UINT32				  nr_medias;
	UINT32				  nr_partitions;	
		

	
} T_GBI_DATALIGHT_ENV_CTRL_BLK;

/**environment controlblock needed for datalight plugin*/
extern T_GBI_DATALIGHT_ENV_CTRL_BLK *gbi_datalight_env_ctrl_blk_p;
/**/



extern T_RV_RET (*bs_msghandler)(T_RV_HDR *msg_p);


extern const T_GBI_PLUGIN_FNCTBL gbi_plugin_fnctbl_datalight; 

#endif /**__GBI_PI_DATALIGHT_BIM_H_*/

