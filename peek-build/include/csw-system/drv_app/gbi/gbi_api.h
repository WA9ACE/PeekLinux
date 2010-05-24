/**
 * @file	gbi_api.h
 *
 * API Definition for GBI SWE.
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

#ifndef __GBI_API_H_
#define __GBI_API_H_

#include "gbi/gbi_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* This type definition must be large enough to hold the largest possible block number 
 * plus one.
 */
typedef UINT32 T_GBI_BLOCK;

/* This type definition must be large enough to address the bytes within one data block.
 */
typedef UINT32 T_GBI_BYTE_CNT;

/* This type definition specifies the number of bytes in a single block. */
typedef UINT16 T_GBI_BLKSIZE;

/* This type defines the unique identifier of the media. Removable media has such an 
 * identifier. When the same media is re-inserted, this is recognisable by comparing 
 * this ID.
 */
typedef UINT32 T_GBI_MEDIA_ID;

/* This structure defines the different media types.*/
typedef enum{
  GBI_MMC = 0,			  //MultiMedia Card
	GBI_SD,			        //Secure Digital
  GBI_MS,			        //Memory Stick
  GBI_CF,			        //Compact Flash
  GBI_SM,			        //Smart Media
  GBI_INTERNAL_NAND, 	//onboard NAND flash		
  GBI_INTERNAL_NOR,		//onboard NOR flash
  GBI_MEDIA_TYPE_END	//terminator
} T_GBI_MEDIA_TYPE;

//AntvB 03-03-2006 UNI CODE impl
//temporariliy define this Enum here! Eventually it should be placed in the GBI API(?)
//This enum should match with the automatically generated Mountpointtable
typedef enum {
  GBI_MN_MMC_A0 = 0,        //MultiMedia Card
  GBI_MN_MMC_A1,
  GBI_MN_MMC_A2,
  GBI_MN_MMC_A3,
  GBI_MN_MMC_B0,
  GBI_MN_MMC_B1,
  GBI_MN_MMC_B2,
  GBI_MN_MMC_B3,
  GBI_MN_MMC_C0,
  GBI_MN_MMC_C1,
  GBI_MN_MMC_C2,
  GBI_MN_MMC_C3,
  GBI_MN_SD_A0,                   //Secure Digital
  GBI_MN_SD_A1,
  GBI_MN_SD_A2,
  GBI_MN_SD_A3,
  GBI_MN_SD_B0,
  GBI_MN_SD_B1,
  GBI_MN_SD_B2,
  GBI_MN_SD_B3,
  GBI_MN_SD_C0,
  GBI_MN_SD_C1,
  GBI_MN_SD_C2,
  GBI_MN_SD_C3,
  GBI_MN_MS_A0,                //Memory Stick
  GBI_MN_MS_A1,
  GBI_MN_MS_A2,
  GBI_MN_MS_A3,
  GBI_MN_MS_B0,
  GBI_MN_MS_B1,
  GBI_MN_MS_B2,
  GBI_MN_MS_B3,
  GBI_MN_MS_C0,
  GBI_MN_MS_C1,
  GBI_MN_MS_C2,
  GBI_MN_MS_C3,
  GBI_MN_CF_A0,             //Compact Flash
  GBI_MN_CF_A1,
  GBI_MN_CF_A2,
  GBI_MN_CF_A3,
  GBI_MN_CF_B0,
  GBI_MN_CF_B1,
  GBI_MN_CF_B2,
  GBI_MN_CF_B3,
  GBI_MN_CF_C0,
  GBI_MN_CF_C1,
  GBI_MN_CF_C2,
  GBI_MN_CF_C3,
  GBI_MN_SM_A0,             //Smart Media
  GBI_MN_SM_A1,
  GBI_MN_SM_A2,
  GBI_MN_SM_A3,
  GBI_MN_SM_B0,
  GBI_MN_SM_B1,
  GBI_MN_SM_B2,
  GBI_MN_SM_B3,
  GBI_MN_SM_C0,
  GBI_MN_SM_C1,
  GBI_MN_SM_C2,
  GBI_MN_SM_C3,
  GBI_MN_INTERNAL_NAND_A0,  //onboard NAND flash    
  GBI_MN_INTERNAL_NAND_A1,
  GBI_MN_INTERNAL_NAND_A2,
  GBI_MN_INTERNAL_NAND_A3,
  GBI_MN_INTERNAL_NAND_B0,
  GBI_MN_INTERNAL_NAND_B1,
  GBI_MN_INTERNAL_NAND_B2,
  GBI_MN_INTERNAL_NAND_B3,
  GBI_MN_INTERNAL_NAND_C0,
  GBI_MN_INTERNAL_NAND_C1,
  GBI_MN_INTERNAL_NAND_C2,
  GBI_MN_INTERNAL_NAND_C3,
  GBI_MN_INTERNAL_NOR_A0,   //onboard NOR flash
  GBI_MN_INTERNAL_NOR_A1,
  GBI_MN_INTERNAL_NOR_A2,
  GBI_MN_INTERNAL_NOR_A3,
  GBI_MN_INTERNAL_NOR_B0,
  GBI_MN_INTERNAL_NOR_B1,
  GBI_MN_INTERNAL_NOR_B2,
  GBI_MN_INTERNAL_NOR_B3,
  GBI_MN_INTERNAL_NOR_C0,
  GBI_MN_INTERNAL_NOR_C1,
  GBI_MN_INTERNAL_NOR_C2,
  GBI_MN_INTERNAL_NOR_C3,
  GBI_MN_MEDIA_TYPE_END  //terminator
} T_GBI_MEDIA_NAMES;
#define GBI_NR_OF_MEDIAS_AND_PARTITIONS (12)
/* This structure defines the media specific information. */
typedef struct{
	UINT8			spare_area_size;  //numb byte in spare area
	UINT8			reserved1;		    //for future extension
	UINT8			reserved2;		    //for future extension
	UINT8			reserved3;		    //for future extension
	UINT32		reserved4;		    //for future extension
	UINT32		reserved5;		    //for future extension
} T_GBI_MEDIA_SPEC;

/* This structure defines the information for one media. */
typedef struct{
	UINT8			        media_nmb;
  T_GBI_MEDIA_TYPE	media_type;
  BOOL		          media_pressent;
	BOOL			        writeable; 
	T_GBI_MEDIA_ID	  media_id;	    //unique media value(ser.nmb)
	UINT32		        read_speed;	  //speed in kilo bits per second
	UINT32		        write_speed;  //speed in kilo bits per second
	T_GBI_MEDIA_SPEC	specific;     //media specific information
} T_GBI_MEDIA_INFO;

/* Note: If the structure member 'spare_area_size' is zero, then the driver does not 
 * support the spare area feature.
 */

/* This type defines the different file system format.*/
typedef UINT16 T_GBI_FS_TYPE;

/* The following possible values: */

#define GBI_EMPTY 	    0x00 // Unused partition table entry
#define GBI_FAT12 	    0x01 // DOS 12-bit FAT
#define GBI_FAT16_A   	0x04 // DOS FAT16 smaller than 32 Mb
#define GBI_EXT_DOS   	0x05 // Extended DOS
#define GBI_FAT16_B     0x06 // DOS FAT16 equal to or greater than 32 Mb
#define GBI_NTFS 	    0x07 // Windows NT NTFS
#define GBI_HPFS 		0x07 // OS/2 IFS (e.g.:HPFS)
#define GBI_FAT32 	    0x0B // Win95 OSR2 32-bit FAT
#define GBI_FAT32_LBA 	0x0C // Win95 OSR2 32-bit FAT LBA
#define GBI_FAT16_LBA 	0x0E // Win95-DOS 16-bit FAT LBA
#define GBI_JFS 		0x35 // Journaling File System on OS/2
#define GBI_TINAFFS 	0x7E // Texas Instruments NAND Flash File System
#define GBI_TINOFFS 	0x7F // Texas Instruments NOR Flash File System
#define GBI_EXT2 		0x83 // Linux native partition
#define GBI_YAFFS 	    0x83 // Linux based yaffs file system
#define GBI_RELIANCE    0x18 // Reliance File system  , The GBI does not know this file system 
#define GBI_UNKNOWN	    0xFF // Unknown file system

/* Note: The above list is placed here as reference only. Not all file system types 
 * are supported. 
 */

/* This structure defines the information for one partition. */
typedef struct{
  UINT8			      media_nmb;	            //GBI API parameter
  UINT8			      partition_nmb;          //GBI API parameter
  T_GBI_FS_TYPE 	filesystem_type;
  UINT8			      filesystem_name[GBI_MAX_FS_NAME];
  UINT8			      partition_name[GBI_MAX_PARTITION_NAME];
  T_GBI_MEDIA_NAMES partition_media_names;//defined media names (enum)
  T_GBI_BLKSIZE   bytes_per_block;        //API block size
  T_GBI_BLOCK	 	  nmb_of_blocks;          //partition size in blocks
  T_GBI_BLOCK	 	  first_block_nmb;	      //start of partition
  T_GBI_BLOCK	 	  last_block_nmb;	        //end of partition
} T_GBI_PARTITION_INFO;

/* Note: Size of the partition (number of bytes) can be determined by multiplying 
 * nmb_of_blocks with bytes_per_block.
 * The member's first_block_nmb and last_block_nmb are for information only 
 * (re-partitioning is not pos-sible).
 */

/* This type defines the events to which a client can subscribe. */
typedef UINT16 T_GBI_EVENTS;

/* The value is a logical OR if the next possible values: */
#define	GBI_EVENT_MEDIA_NONE	  	0x00
#define	GBI_EVENT_MEDIA_INSERT		0x01
#define	GBI_EVENT_MEDIA_REMOVEAL	0x02
#define	GBI_EVENT_NAN_MEDIA_AVAILABLE	0x04



/**
 * @name GBI Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/

typedef INT32 T_GBI_RESULT;

/* Riviera compliant return values */
#define	GBI_OK					    		RV_OK				
#define	GBI_NOT_SUPPORTED				RV_NOT_SUPPORTED		
#define	GBI_MEMORY_ERR					RV_MEMORY_ERR			
#define	GBI_INTERNAL_ERR				RV_INTERNAL_ERR	
#define	GBI_NOT_READY						RV_NOT_READY
#define	GBI_INVALID_PARAMETER	  RV_INVALID_PARAMETER

/* Non-riviera compliant return values, starting at an offset
 * lower than last riviera return value 
 */
#define GBI_BUSY						-20
#define GBI_PROCESSING			-21

/*@}*/

/**
 * gbi_read
 *
 * Detailled description.
 * This function reads a number of data blocks from the partition on the specified 
 * media. The data is copied to the buffer that is to be reserved by the client. 
 * The read starts from the first logical block first_block_nmb and reads 
 * number_of_blocks blocks.  After the last block it completes by reading 
 * remainder_length bytes. 
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned by means of the return
 * path.
 *
 * @param   media_nmb	        This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is 
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is 
 *                            provided when retrieving the overall partition table 
 *                            from the GBI. 
 * @param   first_block_nmb   The first logical block number from where the data 
 *							              is requested.
 * @param   number_of_blocks  The number of logical blocks to read. The number may be 
 *                            from one to the last possible block number. The block 
 *                            size (the number of bytes per block) can be obtained by 
 *                            reading overall partition table from the GBI. 
 * @param   remainder_length  The number of bytes to read after the last whole block. 
 *                            A value of zero indicates no remainder. 
 * @param   buffer_p          This is a pointer to the buffer to which the data shall 
 *                            be copied.
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_read ( UINT8           media_nmb,
                            UINT8           partition_nmb,
				        			      T_GBI_BLOCK		  first_block_nmb,
							              T_GBI_BLOCK		  number_of_blocks,
							              T_GBI_BYTE_CNT	remainder_length,
  						              UINT32			    *buffer_p,
                            T_RV_RETURN     return_path);

/**
 * gbi_write_with_spare
 *
 * Detailled description.
 * This function writes a number of data blocks to the partition on the specified media.
 * The data is copied to the buffer, which is to be reserved by the client. The write 
 * starts from the first logical block first_block_nmb and continues for
 * number_of_blocks blocks. After this, remainder_length bytes are written.
 * When spare_buffer_p is used for each block the spare data will be written.
 * This function is only available for media where spare data is available.
 * 
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned through the return path. 
 *
 * If the media requires this, the driver shall erase the blocks prior to the actual 
 * write.
 *
 * @param   media_nmb	        This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is 
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is 
 *                            provided when retrieving the overall partition table 
 *                            from the GBI. 
 * @param   first_block_nmb   The first logical block number to where the data must
 *							              be written.
 * @param   number_of_blocks  The number of logical blocks to write. The number may be 
 *                            from one to the last possible block number. The block 
 *                            size (the number of bytes per block) can be obtained by 
 *                            reading overall partition table from the GBI. 
 * @param   remainder_length  The number of bytes to write after the last whole block
 *                            is written. A value of zero indicates no remainder. 
 * @param   data_buffer_p     A pointer to the buffer from where the data is copied.
 *
 * @param   spare_buffer_p    A pointer to the buffer from where the spare data is copied.
 * 
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
 T_RV_RET gbi_write_with_spare (UINT8           media_nmb,
                               UINT8           partition_nmb,
                               T_GBI_BLOCK     first_block_nmb,
                               T_GBI_BLOCK     number_of_blocks,
                               T_GBI_BYTE_CNT remainder_length,
                               UINT32         *data_buffer_p,
                               UINT32         *spare_buffer_p,
                               T_RV_RETURN     return_path);

/**
 * gbi_write
 *
 * Detailled description.
 * This function writes a number of data blocks to the partition on the specified media.
 * The data is copied to the buffer, which is to be reserved by the client. The write 
 * starts from the first logical block first_block_nmb and continues for
 * number_of_blocks blocks. After this, remainder_length bytes are written.
 * 
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned through the return path. 
 *
 * If the media requires this, the driver shall erase the blocks prior to the actual 
 * write.
 *
 * @param   media_nmb	        This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is 
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is 
 *                            provided when retrieving the overall partition table 
 *                            from the GBI. 
 * @param   first_block_nmb   The first logical block number to where the data must
 *							              be written.
 * @param   number_of_blocks  The number of logical blocks to write. The number may be 
 *                            from one to the last possible block number. The block 
 *                            size (the number of bytes per block) can be obtained by 
 *                            reading overall partition table from the GBI. 
 * @param   remainder_length  The number of bytes to write after the last whole block
 *                            is written. A value of zero indicates no remainder. 
 * @param   buffer_p          A pointer to the buffer from where the data is copied.
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_write (UINT8           media_nmb,
                            UINT8           partition_nmb,
				        			      T_GBI_BLOCK		  first_block_nmb,
							              T_GBI_BLOCK		  number_of_blocks,
							              T_GBI_BYTE_CNT	remainder_length,
  						              UINT32			    *buffer_p,
                            T_RV_RETURN     return_path);


/**
 * gbi_erase
 *
 * Detailled description.
 * This function erases a number of data blocks from the partition on the specified 
 * media. The erase starts from the first logical block first_block_nmb and continues 
 * for number_of_blocks blocks.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned through the return path. 
 *
 * @param   media_nmb	        This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is 
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is 
 *                            provided when retrieving the overall partition table 
 *                            from the GBI. 
 * @param   first_block_nmb   The first logical block number to where the data must
 *							              be written.
 * @param   number_of_blocks  The number of logical blocks to erase. The number may be 
 *                            from one to the last possible block number. The block 
 *                            size (the number of bytes per block) can be obtained by 
 *                            reading overall partition table from the GBI. 
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_erase (UINT8           media_nmb,
                            UINT8           partition_nmb,
				        			      T_GBI_BLOCK		  first_block_nmb,
							              T_GBI_BLOCK		  number_of_blocks,
                            T_RV_RETURN     return_path);

/**
 * gbi_flush
 *
 * Detailled description.
 * When a client writes data to a media partition, this data is often not immediately
 * written on the physical media. This may be caused by different buffer sizes that 
 * are used by various software and hardware components. Another reason may be task 
 * scheduling or hardware delays. To ensure that the data is consistent and really 
 * written on the physical media, the flush function can be used.
 *
 * The function returns immediately. Only the return path is verified immediately. 
 * Processing is done asynchronous and the result is returned through the return path. 
 *
 * @param   media_nmb         This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is
 *                            provided when retrieving the overall partition table from
 *                            the GBI. 
 * @param   return_path       This structure provides information about the way the 
 *                            driver must react asynchronous (call-back pointer or a 
 *                            return message).
 * 
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_flush (UINT8           media_nmb,
                            UINT8           partition_nmb,
                            T_RV_RETURN     return_path);

/**
 * gbi_get_media_info
 *
 * Detailled description.
 * This function return detailed information about all the media known by the GBI 
 * entity.
 * 
 * The function returns immediately. Only the return_path parameter is verified 
 * immediately. Processing (checking and filling the media information structure) is 
 * done asynchronous and the result is returned through the return path. 
 * 
 * When the processing is started, GBI first allocates the client's memory where the 
 * media information shall be copied in. After the information is copied into the 
 * allocated memory, the response message is send. This message contains the pointer 
 * to this media information structure. The client accesses the information through
 * this pointer. 
 * 
 * The client is responsible for de-allocating the memory when the data is not longer
 * needed.
 *
 * @param   mb_id         Memory bank ID of the client's memory bank (and where the 
 *                        media information shall be copied to).
 * @param   return_path   This is the return path of the client. The structure provides 
 *                        information about the way the driver must react synchronous 
 *                        (call-back pointer or a return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_get_media_info(T_RVF_MB_ID   mb_id,
                                    T_RV_RETURN 	return_path);

/**
 * gbi_get_partition_info
 *
 * This function return detailed information about all the partitions that are known 
 * by the GBI entity.
 * 
 * The function returns immediately. Only the return_path parameter is verified 
 * immediately. Processing (copying the partition information) is done asynchronous 
 * and the result is returned through the return path. 
 *
 * When the processing is started, GBI first allocates the client's memory where the 
 * partition information shall be copied in. After the information is copied into the 
 * allocated memory, the response message is send. This message contains the pointer 
 * to this partition information. The client then accesses the information through this
 * pointer. 
 * 
 * The client is responsible for de-allocating the memory when the data is not longer 
 * needed.
 *
 * @param   mb_id         Memory bank ID of the client's memory bank (and where the 
 *                        partition information shall be copied to).
 * @param   return_path   This is the return path of the client. The structure provides 
 *                        information about the way the driver must react synchronous 
 *                        (call-back pointer or a return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_get_partition_info(T_RVF_MB_ID   mb_id,
                                        T_RV_RETURN 	return_path);

/**
 * gbi_subscribe_events
 *
 * Detailled description.
 * This function allows the client to subscribe to certain events. A maximum of 
 * GBI_MAX_EVENT_SUBSCRIBERS can subscribe to a specific event.
 *
 * The function returns immediately after the parameters are verified. After this 
 * subscription, the client can expect to be notified asynchronous in the way specified 
 * in the return_path parameter.
 *
 * @param   events            A combination of events (logical OR) to which the client
 *                            can subscribe. For example is media-removal. 
 * @param   return_path       The return path of the client. The structure provides 
 *                            information about the way the driver must react 
 *                            asynchronous (whether to use a call-back principle or a 
 *                            return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_subscribe_events(T_GBI_EVENTS	event,
                                      T_RV_RETURN 	return_path);

/**
 * gbi_read_spare_data
 *
 * Detailled description.
 * This function reads the spare area for a number of blocks. The blocks are 
 * consecutive and belong to the media and partition specified. The data is copied to 
 * the buffer, which is to be reserved by the cli-ent. The read starts from the first 
 * logical block first_block. 
 *
 * The function returns immediately (non-blocking). Only the return path is verified 
 * immediately.
 * Processing is done asynchronous and the result is returned through the return path. 
 * 
 * @param   media_nmb         This identifies the media for which the command is 
 *                            intended. This number is provided when retrieving the 
 *                            overall partition table from the GBI. 
 * @param   partition_nmb     This identifies the partition for which the command is
 *                            intended. The partition must be located on the media 
 *                            specified by the media_nmb parameter. This number is
 *                            provided when retrieving the overall partition table from
 *                            the GBI. 
 * @param   first_block       The first logical block number from which the information
 *                            is requested.
 * @param   number_of_blocks  The number of logical blocks for which the information 
 *                            is to be read. The number may be from one to the last 
 *                            possible block number. 
 * @param   info_data_p       This is a pointer to the buffer to which the block 
 *                            information is to be copied.
 * @param   return_path       This is the return path of the client. The structure 
 *                            provides information about the way the driver must react 
 *                            synchronous (call-back pointer or a return message).
 *
 * @return  RV_OK, RV_INVALID_PARAMETER, RV_NOT_READY or RV_MEMORY_ERR
 */
extern  T_RV_RET gbi_read_spare_data( UINT8         media_nmb,
                                      UINT8         partition_nmb,
                                      T_GBI_BLOCK	  first_block,
                                      T_GBI_BLOCK   number_of_blocks,
                                      UINT32        *info_data_p,
                                      T_RV_RETURN   return_path);
/**
 * gbi_get_sw_version
 *
 * Detailled description.
 * This function returns the version of this service entity.
 * 
 * @return    [0-15]	BUILD	Build number
 *            [16-23]	MINOR	Minor version number
 *            [24-31]	MAJOR	Major version number
 */
extern  UINT32 gbi_get_sw_version(void);

/*@}*/

#ifdef __cplusplus
}
#endif


#endif /*__GBI_API_H_*/

