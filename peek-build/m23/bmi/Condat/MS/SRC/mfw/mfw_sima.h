/*
+--------------------------------------------------------------------+
| PROJECT: GSM-MFW (?)                  $Workfile:: mfw_sima.h      $|
| $Author:: Es                          $Revision::  1              $|
| CREATED: 14.10.98                     $Modtime:: 6.10.99 12:21    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

	PURPOSE :  Types definitions for MMI framework 
    .

	Jan 16, 2006 DR: OMAPS00061460 - Shashi Shekar B.S.
	Description: SAT Icon support
	Solution : SAT icon support added.

*/

#ifndef DEF_MFW_SIMA_HEADER
#define DEF_MFW_SIMA_HEADER

/* Contents of the Elementary Files from SIM card */
typedef struct
{
  UBYTE display_condition;
  UBYTE service_provider [LONG_NAME];
} T_EF_SPN;

// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
/* SIM record read parameters */
typedef struct
{
	USHORT data_id;			/* EF file to be read */
	USHORT offset;			/* Offset to file to be read */
	UBYTE  record_number;	/* Record number of EF file */
	UBYTE  len;				/* Length of record */
	UBYTE  max_length;		/* Max records */
} T_SIM_ACCESS_PARAM;

/* Structure to hold the Image instance contents */
typedef struct
{
  UBYTE  image_width;				/* Image Width */
  UBYTE  image_height;				/* Image Height*/
  UBYTE  image_coding;				/* 11 -> B/W, 21 -> Color image, xx -> RFU */
  USHORT image_instance_file_id;	/* File Id of the image instance */
  USHORT image_offset;				/* Offset of the image instance */
  USHORT image_instance_length;     /* Length of image instance data */
} T_IMAGE_INSTANCE;

/* Contents of the Elementary Files from SIM card for IMG */
typedef struct
{
  UBYTE number_of_image_instance;
  T_IMAGE_INSTANCE image_instance;
} T_EF_IMG;
#endif

EXTERN void  sima_init       (void);
EXTERN void  rAT_PlusCFUNP   (T_SIM_MMI_INSERT_IND * mmi_insert_ind);
EXTERN UBYTE qAT_PlusCPINNO  (void);
EXTERN void  sim_read_sim    (USHORT data_id, UBYTE len, UBYTE max_length);
// Shashi Shekar B.S., a0876501, Jan 16, 2006, DR: OMAPS00061460
#ifdef FF_MMI_SAT_ICON
EXTERN void  sim_read_sim_icon(T_SIM_ACCESS_PARAM *sim_icon_read_param);
#endif
EXTERN void  sim_read_sim_cb (SHORT table_id);
EXTERN void  sim_write_sim   (USHORT data_id, UBYTE * data, UBYTE length);
EXTERN void  sim_write_sim_cb(SHORT table_id);


#endif
