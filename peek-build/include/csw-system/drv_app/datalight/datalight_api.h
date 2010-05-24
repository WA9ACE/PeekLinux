/**
 * @file	datalight_api.h
 *
 * API Definition for DL SWE.
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *
 */

#ifndef __DATALIGHT_API_H_
#define __DATALIGHT_API_H_
#include "datalight.h"
#include "swconfig.cfg"




#ifdef __cplusplus
extern "C"
{
#endif

//typedef unsigned char   UINT8;
//typedef unsigned long   UINT32;
typedef INT32		T_DL_RESULT;



/*
NOTE:

  Device Configuration : GBI Datalight plugin

|------------------|--------------------|
|  Device Number   |     Device			|
|---------------------------------------|
|	0			   |    NOR				|
|	1			   |	NAND			|
|---------------------------------------|


If NOR is not existing then
|------------------|--------------------|
|  Device Number   |     Device			|
|---------------------------------------|
|	0			   |	NAND			|
|---------------------------------------|




  Disk configuration configuration:  FlashFx, when all the plugins are existing

|--------------------------------------------------------------------------------------
|Partition Number  |  Device     |          Descrption
|------------------|--------------------------------------------------------------------
|    0			   |	  NOR 	 |			GBI datalight does not access it,
|    			   |(Mirror bit) |	 	bacuase this partion is reserved for Reliance File system
|				   |		     |
|    1 			   | NOR (Sibly) |		Required for FAT over NOR
|     			   |	         |
|    2 			   |  NAND		 |		Required for FAT over NAND
|     			   |			 |
|    3			   | MirroBit NOR|	    Required for FAT over Mirror Bit
|---------------------------------------------------------------------------------------
*/

#if (DATALIGHT_NOR == 1)
#define GBI_DATALIGHT_NOR_PART_ID_0			1

#else
#define GBI_DATALIGHT_NOR_PART_ID_0			0
#endif

#define GBI_DATALIGHT_NAND_PART_ID_0 		(GBI_DATALIGHT_NOR_PART_ID_0+1)




#define DL_DBG_TRACE  /* Enable debug trace message */
/**
 * The message offset must differ for each SWE in order to have
 * unique msg_id in the system.
 */
#define DATALIGHT_MESSAGE_OFFSET   BUILD_MESSAGE_OFFSET(DATALIGHT_USE_ID)


#define DL_ERASE_REQ_MSG (DATALIGHT_MESSAGE_OFFSET | 0x001)
#define DL_ERASE_RSP_MSG (DATALIGHT_MESSAGE_OFFSET | 0x002)
#define DL_WRITE_REQ_MSG (DATALIGHT_MESSAGE_OFFSET | 0x003)
#define DL_WRITE_RSP_MSG (DATALIGHT_MESSAGE_OFFSET | 0x004)
#define DL_READ_REQ_MSG  (DATALIGHT_MESSAGE_OFFSET | 0x005)
#define DL_READ_RSP_MSG  (DATALIGHT_MESSAGE_OFFSET | 0x006)
#define DL_INIT_REQ_MSG  (DATALIGHT_MESSAGE_OFFSET | 0x007)
#define DL_INIT_RSP_MSG  (DATALIGHT_MESSAGE_OFFSET | 0x008)


/* dl communication msg id. random value assigned */



 /*==========================================================================*/
/*!
 * @struct T_DL_READ_REQ_MSG
 *
 * @discussion
 * <b> Description </b><br>
 *  The T_DL_READ_REQ_MSG message can be used to read data from a DL card.
 * This message is simular to the dl_read() function. The driver responds
 * with a T_DL_READ_REQ_MSG message.
 *
 */

/** Read request */
typedef struct {
  T_RV_HDR     		os_hdr;
  unsigned short	disk_num;
  UINT32            startsector;
  UINT32		    numsector;
  UINT8             *data_p;
  T_RV_RETURN 		rp;
} T_DL_READ_REQ_MSG;

/** Read response */
typedef struct {
  T_RV_HDR     	os_hdr;
  unsigned short	disk_num;
  T_DL_RESULT   result;
  UINT8     	*data_p;
} T_DL_READ_RSP_MSG;

/*@}*/

/**
 * @name DL_WRITE_REQ_MSG
 *
 * Detailled description
 * The T_DL_ WRITE_REQ_MSG message can be used to write data to a DL card.
 * This message is simular to the dl_write() function. The driver responds
 * with a T_DL_WRITE_RSP_MSG message.
 *
 */
/*@{*/

/** Write request */
typedef struct {
  T_RV_HDR          os_hdr;
  unsigned short	disk_num;
  UINT32            startsector;
  UINT32		    numsector;
  UINT8             *data_p;
  T_RV_RETURN rp;
} T_DL_WRITE_REQ_MSG;

/** Write response */
typedef struct {
  T_RV_HDR  	os_hdr;
  unsigned short	disk_num;
  T_DL_RESULT   result;
  UINT8     	*data_p;
} T_DL_WRITE_RSP_MSG;

/*@}*/

/**
 * @name DL_ERASE_GROUP_REQ_MSG
 *
 * Detailled description
 * The T_DL_ERASE_GROUP_REQ_MSG message can be used to erase a range of erase
 * groups on the card. This message is simular to the dl_erase_group ()
 * function. The driver re-sponds with a T_DL_ERASE_GROUP_RSP_MSG message.
 *
 */
/*@{*/

/** Erase group request */
typedef struct {
  T_RV_HDR          os_hdr;
  unsigned short	disk_num;
  UINT32            startsector;
  UINT32		    numsector;
  UINT8    			*data_p;
  T_RV_RETURN rp;
} T_DL_ERASE_REQ_MSG;

/** Erase group respond */
typedef struct {
  T_RV_HDR  os_hdr;
   unsigned short	disk_num;
  T_DL_RESULT   result;
} T_DL_ERASE_RSP_MSG;

typedef struct {
T_RV_HDR          os_hdr;
T_RV_RETURN rp;
} T_DL_INIT_REQ_MSG;

typedef struct {
T_RV_HDR          os_hdr;
T_DL_RESULT   result;
} T_DL_INIT_RSP_MSG;


/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/*==========================================================================*/
/*!
 * @function dl_read
 *
 * @discussion
 * <b> Discription </b><br>
 *     This function will read specifed page from datalight media.
 * This functions returns after posting DL_READ_REQ_MSG message to the datalight
 * mailbox.Processing is done asynchronous and the result is returned through the
 * return path.
 *
 * @param   start_sector   start sector number to erase
 * @param   number_of_sector	number of sector to erase
 * @param   data_p       Pointer to dump the read data
 * @param   return_path          Return path
 *
 * @return  <br><ul>
 *    <li>  DL_OK         Success
 *    <li>  DL_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *    <li>  DL_MEMORY_ERR Memory allocation error. </ul>
 */
  T_DL_RESULT dl_read (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p,
								T_RV_RETURN   return_path);


/*==========================================================================*/
/*!
 * @function dl_write
 *
 * @discussion
 * <b> Discription </b><br>
 *     This function will write specifed page on datalight datalight media.
 * This functions returns after posting DL_F_PROGRAM_REQ_MSG message to the dl
 * mailbox.Processing is done asynchronous and the result is returned through the
 * return path.
 *
 * @param   start_sector   start sector number to erase
 * @param   number_of_sector	number of sector to erase
 * @parm     data_p	Pointer to write data buffer
 * @param   return_path	Return path
 *
 * @return  <br><ul>
 *    <li>  DL_OK         Success
 *    <li>  DL_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *    <li>  DL_MEMORY_ERR Memory allocation error. </ul>
 */

  T_DL_RESULT dl_write (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p,
								T_RV_RETURN    return_path);

/*==========================================================================*/
/*!
 * @function dl_erase
 *
 * @discussion
 * <b> Discription </b><br>
 *     This function will erase the specifed block on datalight media.
 * This functions returns after posting DL_F_ERASE_REQ_MSG message to the dl
 * mailbox.Processing is done asynchronous and the result is returned through the
 * return path.
 *
 * @param   start_sector   start sector number to erase
 * @param   number_of_sector	number of sector to erase
 * @param   return_path	Return path
 *
 *
 * @return  <br><ul>
 *    <li>  DL_OK         Success
 *    <li>  DL_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *    <li>  DL_MEMORY_ERR Memory allocation error. </ul>
 */


  T_DL_RESULT dl_erase (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sector, T_RV_RETURN return_path );

/*==========================================================================*/
/*!
 * @function dl_init
 *
 * @discussion
 * <b> Discription </b><br>
 *
 *
 *
 * @param   return_path	Return path
 *
 *
 * @return  <br><ul>
 *    <li>  DL_OK         Success
 *    <li>  DL_NOT_READY  The driver is not able to handle this request at
 *                        this moment.
 *    <li>  DL_MEMORY_ERR Memory allocation error. </ul>
 */

T_DL_RESULT dl_init (T_RV_RETURN   return_path);


/*-------------------------------------------------------------------
  This values are used in the sample device driver only.
  -------------------------------------------------------------------*/
#define  FFXSTAT_SUCCESS       0


#ifdef __cplusplus
}
#endif


#endif /*__DATALIGHT_API_H_*/


