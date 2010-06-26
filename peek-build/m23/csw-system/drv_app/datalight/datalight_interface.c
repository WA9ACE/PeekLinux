/**
 * @file   datalight_interface.c
 *
 * Implementation of DATALIGHT interfaces.
 * These functions implement the dl processing for all the messages the
 * dl task can receive.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *
 *
 */


#include "typedefs.h"
#include "datalight_api.h"
#include "datalight.h"
#include "datalight_interface.h"
#include <string.h>
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"
#include "gbi/gbi_pi_cfg.h"
#include "gbi/gbi_pi_datalight_i.h"
#include "datalight_pei.h"
#include "swconfig.cfg"      /* for Partition Macros */

#ifndef DATALIGHT_SIMULATION
#define FFXDRV_INC_LOCOSTO_DATALIGHT_PLUGIN
#include "ffxdrv.h"
#endif



/* Structure to get the disk parameters using ioctl */
#ifndef DATALIGHT_SIMULATION
  DISK_MEDIA_PARAMS sDiskMediaParams[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
  DISK_PARTITION_PARAMS sDiskPartitionParams[GBI_DATALIGHT_MAX_NR_OF_PARTITIONS];
#endif


extern  T_DL_ENV_CTRL_BLK *dl_env_ctrl_blk_p;


/* Wrapper functions for FlashFx APIs */


static unsigned long GBI_DATALIGHT_FlashFXDeviceOpen(unsigned short uDriveNum)
{

 return FlashFXDeviceOpen(uDriveNum);
}


static unsigned long GBI_DATALIGHT_FlashFXDeviceClose(unsigned short uDriveNum)
{
 return FlashFXDeviceClose(uDriveNum);
}


static unsigned long GBI_DATALIGHT_FlashFXDeviceIO(unsigned short uDriveNum, unsigned long ulSectorNum, void *pClientBuffer, unsigned short uSectorCount, unsigned short fReading)
{
  return FlashFXDeviceIO(uDriveNum,ulSectorNum,pClientBuffer,uSectorCount,fReading);
}

static unsigned long GBI_DATALIGHT_FlashFXDeviceIoctl(unsigned short uDriveNum, unsigned short command, void *buffer)
{
 return FlashFXDeviceIoctl(uDriveNum,command,buffer);
}



/**
 * function: dl_send_response_to_client
 */
static T_RV_RET dl_send_response_to_client (T_RV_HDR    *rsp_p,
        									   T_RV_RETURN *return_path)
{

    /* follow return path: callback or mailbox */
    if (return_path->callback_func != NULL)
    {
        DL_SEND_TRACE ("DL operations: callback",RV_TRACE_LEVEL_WARNING);
        return_path->callback_func (rsp_p);
    	if (rvf_free_msg ((T_RV_HDR *) rsp_p) != RVF_OK)
    	{
      		DL_SEND_TRACE ("DL operations: could not free response msg",
                       RV_TRACE_LEVEL_WARNING);
      		return RV_MEMORY_ERR;
    	}

    }
    else
    {

	 DL_SEND_TRACE ("DL operations: sending response",RV_TRACE_LEVEL_DEBUG_HIGH);
        if (rvf_send_msg(return_path->addr_id, rsp_p) != RVF_OK)
        {
             DL_SEND_TRACE ("DL operations: could not send response msg",RV_TRACE_LEVEL_WARNING);
            return RV_MEMORY_ERR;
        }
    }

    return RV_OK;
}

/**
 * function: dl_send_read_rsp
 */
static T_RV_RET dl_send_read_rsp (unsigned short disk_num,T_RV_RET	result,
       							     UINT8      *data_p,
			             			 T_RV_RETURN	rp)
{
    T_DL_READ_RSP_MSG *rsp_p;
    T_RV_RET retval;


    if (rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_DL_READ_RSP_MSG),
                         DL_READ_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
        DL_SEND_TRACE ("DL dl_send_read_rsp: out of memory",RV_TRACE_LEVEL_WARNING);
        return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->os_hdr.msg_id  = DL_READ_RSP_MSG;
	rsp_p->disk_num = disk_num;
	rsp_p->result      = result;
    rsp_p->data_p      = data_p;

    /* Send message mailbox */
    retval = dl_send_response_to_client ((T_RV_HDR *) rsp_p,
            &rp);
    return retval;
}

/**
 * function: dl_send_write_rsp
 */
static T_RV_RET dl_send_write_rsp (unsigned short disk_num,T_RV_RET    result,
        							UINT8      *data_p,
							        T_RV_RETURN rp)
{
    T_DL_WRITE_RSP_MSG *rsp_p;
    T_RV_RET retval;


    if (rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_DL_WRITE_RSP_MSG),
                         DL_WRITE_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
         DL_SEND_TRACE ("DL dl_send_read_rsp: out of memory",RV_TRACE_LEVEL_WARNING);
        return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->os_hdr.msg_id  = DL_WRITE_RSP_MSG;
	rsp_p->disk_num = disk_num;
    rsp_p->result      = result;
    rsp_p->data_p      = data_p;


    /* Send message mailbox */
#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<DBG>dl_send_write_rsp sending responce\n\r",RV_TRACE_LEVEL_WARNING);
#endif /* DL_DBG_TRACE */
    retval = dl_send_response_to_client ((T_RV_HDR *) rsp_p,
            &rp);
    return retval;
}

/**
 * function: dl_send_erase_rsp
 */
static T_RV_RET dl_send_erase_rsp (unsigned short disk_num,T_RV_RET    result,
        T_RV_RETURN rp)
{
    T_DL_ERASE_RSP_MSG *rsp_p;
    T_RV_RET retval;


    if (rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_DL_ERASE_RSP_MSG),
                         DL_ERASE_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
        DL_SEND_TRACE ("DL dl_send_read_rsp: out of memory",RV_TRACE_LEVEL_WARNING);
        return RV_MEMORY_ERR;
    }

    /* compose message */
    rsp_p->os_hdr.msg_id      = DL_ERASE_RSP_MSG;
	rsp_p->disk_num = disk_num;
    rsp_p->result      = result;

    /* Send message mailbox */
#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<DBG>dl_send_erase_rsp Sending Erase responce\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */
    retval = dl_send_response_to_client ((T_RV_HDR *) rsp_p,
            &rp);

    return retval;
}

/**
 * function: dl_send_init_rsp
 */
 static T_RV_RET dl_send_init_rsp( T_RV_RET    result,
        T_RV_RETURN rp)
{
    T_DL_INIT_RSP_MSG *rsp_p;
    T_RV_RET retval;

    /*	ext printf message  */
#ifdef DL_DBG_TRACE
	DL_SEND_TRACE("<DBG>Inside dl_send_init_rsp\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */
    /* compose message */

    if (rvf_get_msg_buf (dl_env_ctrl_blk_p->prim_mb_id,
                         sizeof (T_DL_INIT_RSP_MSG),
                         DL_INIT_RSP_MSG,
                         (T_RV_HDR **) &rsp_p) == RVF_RED)
    {
        DL_SEND_TRACE ("DL dl_send_read_rsp: out of memory",RV_TRACE_LEVEL_DEBUG_HIGH);
        return RV_MEMORY_ERR;
    }
    rsp_p->os_hdr.msg_id  = DL_INIT_RSP_MSG;
    rsp_p->result  = result;

    /* Send message mailbox */
    retval = dl_send_response_to_client ((T_RV_HDR *) rsp_p,
            &rp);

    return retval;
}


T_RV_RET dl_read_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p)
{
    T_RV_RET retval = RV_OK;
    BOOL read_operation = TRUE;
    unsigned long   ulStartSector;
    unsigned long   ulSectorCount;
    void           *pClientBuffer;
    unsigned long  uStatus;

    ulStartSector = start_sector+ sDiskPartitionParams[disk_num].ulStartSector;
    ulSectorCount = number_of_sectors;
    pClientBuffer = data_p;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIO(disk_num,ulStartSector,pClientBuffer,ulSectorCount,read_operation);

    if(uStatus != FFXSTAT_SUCCESS)
    {
        retval = RV_INTERNAL_ERR;
    }
    return retval;
}

/* function: dl_op_read_req
*/
T_RV_RET dl_op_read_req(T_DL_READ_REQ_MSG *msg_p)
{
    T_RV_RET retval = RV_OK;
#ifndef DATALIGHT_SIMULATION
    BOOL read_operation = TRUE;
    unsigned long   ulStartSector;
    unsigned long   ulSectorCount;
    void           *pClientBuffer;
    unsigned long  uStatus;
    unsigned int start_time,end_time;


#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<DBG>Inside dl_op_read_req\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */

    ulStartSector = msg_p->startsector+ sDiskPartitionParams[msg_p->disk_num].ulStartSector;
    ulSectorCount = msg_p->numsector;
    pClientBuffer = msg_p->data_p;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIO(msg_p->disk_num,ulStartSector,pClientBuffer,ulSectorCount,read_operation);



    if(uStatus != FFXSTAT_SUCCESS)
    {
        retval = RV_INTERNAL_ERR;
    }
#endif

#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<SIMULATION>call FFX driver function read data\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif
    /** Call DL commands */
    retval = dl_send_read_rsp (msg_p->disk_num,retval,
            msg_p->data_p,
            msg_p->rp);

    return retval;
}

T_RV_RET dl_write_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p)
{
    T_RV_RET retval = RV_OK;
    BOOL read_operation = FALSE;
    unsigned long   ulStartSector;
    unsigned long   ulSectorCount;
    void           *pClientBuffer;
    unsigned long  uStatus;

    ulStartSector = start_sector+ sDiskPartitionParams[disk_num].ulStartSector;
    ulSectorCount = number_of_sectors;
    pClientBuffer = data_p;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIO(disk_num,ulStartSector,pClientBuffer,ulSectorCount,read_operation);

    if(uStatus != FFXSTAT_SUCCESS)
    {
        retval = RV_INTERNAL_ERR;
    }
    return retval;
}
/**
 * function: dl_op_write_req
 */
T_RV_RET dl_op_write_req(T_DL_WRITE_REQ_MSG *msg_p)
{
    T_RV_RET retval = RV_OK;
#ifndef DATALIGHT_SIMULATION
    BOOL read_operation = FALSE;
    unsigned long   ulStartSector;
    unsigned long   ulSectorCount;
    void           *pClientBuffer;
    unsigned long  uStatus;
    unsigned int start_time,end_time;


#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<DBG>Inside dl_op_write_req\n\r",RV_TRACE_LEVEL_DEBUG_MEDIUM);
#endif /* DL_DBG_TRACE */

    ulStartSector = msg_p->startsector+ sDiskPartitionParams[msg_p->disk_num].ulStartSector;
    ulSectorCount = msg_p->numsector;
    pClientBuffer = msg_p->data_p;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIO(msg_p->disk_num,ulStartSector,pClientBuffer,ulSectorCount,read_operation);

    if(uStatus != FFXSTAT_SUCCESS)
    {
    	DL_SEND_TRACE("dl_op_write_req fail\n\r",RV_TRACE_LEVEL_WARNING);
        retval = RV_INTERNAL_ERR;
    }
#endif

#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<SIMULATION>call FFX driver function to write data\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif

    /** Call DL commands */
    retval = dl_send_write_rsp (msg_p->disk_num,retval,
            msg_p->data_p,
            msg_p->rp);
    return retval;
}

T_RV_RET dl_erase_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors)
{
    T_RV_RET retval = RV_OK;
    DISCARD_PARAMS  sParams;
    unsigned long  uStatus;

    sParams.ulStartSector = start_sector+ sDiskPartitionParams[disk_num].ulStartSector;
    sParams.ulNumSectors= number_of_sectors;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIoctl(disk_num,FLASHFX_IOCTL_DISCARD, &sParams);

    if(uStatus != FFXSTAT_SUCCESS)
    {
        retval = RV_INTERNAL_ERR;
    }
    return retval;
}

/**
 * function: dl_op_erase_req
 */
T_RV_RET dl_op_erase_req(T_DL_ERASE_REQ_MSG *msg_p)
{
    T_RV_RET retval      = RV_OK;



#ifndef DATALIGHT_SIMULATION
    DISCARD_PARAMS  sParams;
    unsigned long  uStatus;
	unsigned int start_time,end_time;

#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<DBG>Inside dl_op_erase_req\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif /* DL_DBG_TRACE */

    sParams.ulStartSector = msg_p->startsector+ sDiskPartitionParams[msg_p->disk_num].ulStartSector;
    sParams.ulNumSectors = msg_p->numsector;

    uStatus = GBI_DATALIGHT_FlashFXDeviceIoctl(msg_p->disk_num,FLASHFX_IOCTL_DISCARD, &sParams);

    if(uStatus != FFXSTAT_SUCCESS)
    {
        retval = RV_INTERNAL_ERR;
    }
#endif

#ifdef DL_DBG_TRACE
    DL_SEND_TRACE("<SIMULATION>Writing Datao To Ffx\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);
#endif
    retval = dl_send_erase_rsp(msg_p->disk_num,retval,
            msg_p->rp);
    return retval;

}


/**
 * function: dl_op_init_req
 */
T_RV_RET dl_op_init_req(T_DL_INIT_REQ_MSG *msg_p)
{
    T_RV_RET retval      = RV_OK;
	 unsigned long uStatus;


 DL_SEND_TRACE("<SIMULATION>FfxInitialization\n\r",RV_TRACE_LEVEL_DEBUG_HIGH);

    retval = dl_send_init_rsp(retval,msg_p->rp);
    return retval;
}



unsigned long FlashFx_Single_Disk_Init(unsigned int disk_num)
{
 unsigned long uStatus;

  DATALIGHT_TRACE_DEBUG_ERROR(" FFX Initialization ...1");

  dl_env_ctrl_blk_p->disk_initialised[disk_num] = FALSE;  /* Un initialized */

  /* Reset the sector size of SDiskMediaPrams to 0 */
  sDiskMediaParams[disk_num].ulSectorSize = 0;

  /* Initialization*/
  uStatus = GBI_DATALIGHT_FlashFXDeviceOpen(disk_num);
  DATALIGHT_TRACE_DEBUG_ERROR(" FFX Initialization ...2");

  if(uStatus != FFXSTAT_SUCCESS)
	{
			DATALIGHT_TRACE_DEBUG_ERROR_PARAM(" FFX Initialization Failed, device", disk_num);
	}
	else
	{
	  DATALIGHT_TRACE_DEBUG_ERROR(" FFX Initialization ...3");
        uStatus = GBI_DATALIGHT_FlashFXDeviceIoctl(disk_num,FLASHFX_IOCTL_MEDIA_PARAMS, &sDiskMediaParams[disk_num]);
	 	if(uStatus != FFXSTAT_SUCCESS)
       	{
	 	DATALIGHT_TRACE_DEBUG_ERROR_PARAM("FFX getting media info ioctl Failed", disk_num);

	 	}
		else
			{
			  DATALIGHT_TRACE_DEBUG_ERROR(" FFX Initialization ...4");
			uStatus = GBI_DATALIGHT_FlashFXDeviceIoctl(disk_num,FLASHFX_IOCTL_PARTITION_PARAMS, &sDiskPartitionParams[disk_num]);
	 			if(uStatus != FFXSTAT_SUCCESS)
       	 		{
	 			DATALIGHT_TRACE_DEBUG_ERROR_PARAM("FFX getting partition info ioctl Failed",disk_num);
   				}

			}

	 }

  DATALIGHT_TRACE_DEBUG_ERROR(" FFX Initialization ...5");

 if(sDiskMediaParams[disk_num].ulSectorSize != 0)
 	{
  		dl_env_ctrl_blk_p->disk_initialised[disk_num] = TRUE;
 	}

  return uStatus;

}





/* *************************************************************
						    EXTENDABLE FUNCTION
   *************************************************************
*/


unsigned long FlashFx_Disks_Init()
{

#if ((DATALIGHT_NAND == 1) || (DATALIGHT_NAND == 2))



 FlashFx_Single_Disk_Init(GBI_DATALIGHT_NAND_PART_ID_0);
#endif



#if ((DATALIGHT_NOR == 1) || (DATALIGHT_NOR == 2))

 FlashFx_Single_Disk_Init(GBI_DATALIGHT_NOR_PART_ID_0);

#endif


  return 0;

}





