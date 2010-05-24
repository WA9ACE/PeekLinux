

#ifndef PST_API_H
#define PST_API_H

#include "exedefs.h"
#include "balfsiapi.h"
#include "balmiscapi.h"

#define PST_FSI_MAILBOX EXE_MAILBOX_1_ID

#define PST_ETS_MAILBOX  EXE_MAILBOX_2_ID


/******************PST START****************************/

#define PST_MAX_DATA_LEN     2000
#define PST_MAX_MON_SPY_SIZE   (MON_MAX_SPY_SIZE-1)
#define PST_MAX_ALPHA_LENGTH         32

typedef enum
{
		PST_CALENDAR_GETSIZE_MSG,
   	PST_CALENDAR_GETEVENT_MSG, 
   	PST_CALENDAR_SETEVENT_MSG,
   	PST_CALENDAR_DELETEEVENT_MSG,
   	PST_CALENDAR_DELETEALL_MSG,
   	PST_SMS_READ_MSG,
   	PST_SMS_WRITE_MSG,
   	PST_SMS_DELETE_MSG,
   	PST_SMS_DELETEALL_MSG,
   	PST_SMS_SEND_MSG,
   	PST_SMS_SENDINDEX_MSG,
   	PST_PB_CMD_MSG,
   	PST_PB_ACK_MSG, 
   	PST_NUM_MSGS
}PstMsgIdT;


typedef enum
{
    Ok,                           /* Normal completion.*/
    Error,                        /* Generic Error completion*/
    Index_OutOfRange,    /* Index requested was either to large or to small.*/
    LastEntry,                 /*The value returned is ok and it is the last entry in the list.*/
    Not_Implemented,     /* Requested message is not supported.*/
    Storage_Full,             /*Storage space is not available*/
    Calendar_Changed,    /* The Calendar has been modified during a read process*/
    SMS_Send_Fail,          /* The transmission of a SMS or EMS message has failed.*/
    SMS_Save_Fail,
    EMS_Seg_Error,          /* Did not receive last segment or duplicat segment in an EMS writing or sending cmd*/
    EMS_Not_FirstSeg,
    Empty_Index,
    PhbRec_Rearranged,
    PhbRec_Compressing
}PstReturnStatT;

typedef enum
{
    REC_ADDED,
    REC_DELETED,
    REC_MODIFIED
}PstCalendarUpdateStatT;

typedef PACKED struct/*ETS Calendar Get Size Cmd*/
{
    ExeRspMsgT  RspInfo;

}EtsGetCalendarRecSizeT;

typedef PACKED struct/*ETS Calendar Get Size Rsp*/
{
    uint8                          MaxSize;
    uint8                          Size;
} PstGetCalendarSizeRspT;


typedef PACKED struct
{
    uint16   Year;
    uint8     Month;
    uint8     Day;
    uint8     Hour;
    uint8     Minute;
    uint8     Second;
} PstTimeDataT;

typedef PACKED struct/*ETS Calendar Get Event Cmd*/
{
    ExeRspMsgT                 RspInfo;
    uint8                          Index;
} EtsGetCalendarRecT;

typedef PACKED struct/*ETS Calendar Get Event Rsp*/
{
    PstReturnStatT         status;    
    PstTimeDataT           Date;
 /*   bool                            Expired;*/
    uint32                         RingerId;
    uint8                          CodePage;/*0-ASCII, 1-UNICODE*/
    uint8                          ByteCount;
    uint8                          Text[1];
} PstGetCalendarRspT;

typedef PACKED struct/*ETS Calendar Set Event Cmd*/
{
    ExeRspMsgT                 RspInfo;
    PstTimeDataT           Date;
    uint32                         RingerId;
    uint8                          CodePage;/*0-ASCII, 1-UNICODE*/
    uint8                          ByteCount;
    uint8                          Text[1];
} EtsSetCalendarRecT;

typedef PACKED struct/*ETS Calendar Set Event Rsp*/
{
    PstReturnStatT         status;    
    uint8                           RecId;
} PstSetCalendarRspT;

typedef PACKED struct/*ETS Calendar Delete Event Cmd*/
{
    ExeRspMsgT                 RspInfo;
    PstTimeDataT           Date;
    uint32                         RingerId;
    uint8                          CodePage;/*0-ASCII, 1-UNICODE*/
    uint8                          ByteCount;
    uint8                          Text[1];
} EtsDeleteCalendarRecT;



typedef PACKED struct/*ETS Calendar DeleteAll Event Cmd*/
{
    ExeRspMsgT                 RspInfo;

} EtsDeleteAllCalendarRecT;

typedef PACKED struct
{
    PstCalendarUpdateStatT      changestate;    
} PstCalendarSpyMsgT;

typedef PACKED struct
{
  char          CmdData[PST_MAX_DATA_LEN];
  uint16        cmdLen;
}PstCmdBufT;



typedef PACKED struct/*CP Spy AT Responses Spy Rsp*/
{
   uint8        last;
   uint8        Data[PST_MAX_MON_SPY_SIZE];
}PstPbRspSpyMsgT;

typedef PACKED struct/*ETS AT Command Rsp*/
{
   PstReturnStatT         status;
} PstSendRspMsgT;

typedef PACKED struct/*ETS AT Command Cmd*/
{
    ExeRspMsgT  RspInfo;

   uint8     last;
   char      Data[1];
}EtsPhbReqMsgT;

typedef PACKED struct/*ETS SMS_EMS Read Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8                    storage;
    uint16                  Index;
    uint8                    EmsSegment;
}EtsSmsReadMsgT;

typedef PACKED struct/*ETS SMS_EMS Read Message Rsp*/
{
    PstReturnStatT    status;
    uint8                      RecType;
    uint8                     storage;
    uint16                    Index;
    uint8                      EmsFlag;
    uint8                      EmsSegment;
    uint8                      EmsLast;
    uint8                      AlphaType;
    uint8                      AlphaLen;
    uint8                      Alpha[PST_MAX_ALPHA_LENGTH];
    uint8                      PduData[1];    
}EtsSmsReadRspMsgT;

typedef PACKED struct/*ETS SMS_EMS Delete Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8            storage;
    uint16          Index;
}EtsSmsDeleteMsgT;

typedef PACKED struct/*ETS SMS_EMS Delete All Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8  storage;
}EtsSmsDeleteAllMsgT;

typedef PACKED struct/*ETS SMS_EMS Write Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8       storage;
    uint16      Index;
    uint8        EmsFlag;    
    uint8        EmsSegment;
    uint8        EmsLast;
    uint8        PduData[1];    
}EtsSmsWriteMsgT;

typedef PACKED struct/*ETS SMS_EMS Write Message Rsp*/
{
    PstReturnStatT    status;
    uint8                      storage;
    uint16                    Index;
    uint8                      EmsSegment;  
    uint16                      RefNum;
}EtsSmsWriteRspMsgT;

typedef PACKED struct/*ETS SMS_EMS Send Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8         storage;
    uint16        Index;
    uint8          EmsFlag;    
    uint8          EmsSegment;
    uint8          EmsLast;
    bool           NeedSave;
    bool           NeedConfirm;
    uint8          PduData[1];    
}EtsSmsSendMsgT;

typedef PACKED struct/*ETS SMS_EMS Send Message Rsp*/
{
    PstReturnStatT    status;
    uint8                     storage;
    uint16                    Index;
    uint8                      EmsSegment; 
    uint16                      RefNum;
}EtsSmsSendRspMsgT;

typedef PACKED struct/*ETS SMS_EMS Send Index Message Cmd*/
{
    ExeRspMsgT  RspInfo;

    uint8      storage;
    uint16     Index;
    bool        NeedSave;
    bool        NeedConfirm;  
}EtsSmsSendIndexMsgT;

typedef PACKED struct/*ETS SMS_EMS Send Index Message Rsp*/
{
    PstReturnStatT    status;
    uint8                      storage;
    uint16                    Index;
}EtsSmsSendIndexRspMsgT;

/*********************PST END*************************************/

/******************FSI START*************************/
/*----------------------------------------------------------------------------
 Defines Constants used in this file
----------------------------------------------------------------------------*/

/* Define FSI command mailbox ids */
#define MAX_FILE_HANDLE_NUM     16  /*Max number of files to open by ETS*/

#define MAX_FIND_HANDLE_NUM     8   /*Max number of files finding to open by ETS*/

#define INVALID_ATTRIBUTE_VALUE 0xFFFF;

/*------------------------------------------------------------------------
* Define typedefs used in FSI API
*------------------------------------------------------------------------*/
/* Define FSI task message Ids.*/
typedef enum 
{
   PST_FILE_SYS_FORMAT_MSG   = 1,   /*Format a volume device*/
   PST_GET_SPACE_INFO_MSG    = 2,   /*Get space using information of a volume*/
   PST_FILE_OPEN_MSG         = 3,   /*Open a file by 6 optional modes*/
   PST_FILE_BLOCK_READ_MSG   = 4,   /*Read data block from a opened file*/
   PST_FILE_BLOCK_WRITE_MSG  = 5,   /*Write data block to a opened file */
   PST_FILE_CLOSE_MSG        = 6,   /*Close a file opened by ETS*/
   PST_FILE_CLOSE_ALL_MSG    = 7,   /*Close all files opened by ETS*/
   PST_FILE_GET_ATTRIB_MSG   = 8,   /*Get file attribute of a file*/
   PST_FILE_SET_ATTRIB_MSG   = 9,   /*Set file attribute of a file */
   PST_VOLUME_LIST_MSG       = 10,  /*Get all volume name list*/
   PST_FILE_DIR_FIND_MSG     = 11,  /*Search files or directories*/
   PST_FILE_TRUNCATE_MSG     = 12,  /*Truncate a file*/
   PST_FILE_DIR_RENAME_MSG   = 13,  /*Rename a file or a directory*/
   PST_FILE_DIR_DELETE_MSG   = 14,  /*Delete a file or a directory*/
   PST_MAKE_DIR_MSG          = 15,  /*Create a directory*/
   PST_SIDB_READ_MSG         = 16,  /*Read data from a data item*/
   PST_SIDB_WRITE_MSG        = 17,  /*Write data to a data item */
   PST_SIDB_DELETE_MSG       = 18,  /*Delete a data item*/
   PST_SIDB_LIST_MSG         = 19,  /*Search data items*/
   PST_FFS_SIDB_READ_MSG = 20,/* Only for back compatible */
   PST_FFS_SIDB_WRITE_MSG =21,/* Only for back compatible */
   

   PST_NUM_FSI_MESSAGES
   
} PSTFsiMsgIdT;

/* Define FSI error return codes. These error codes are combined
 * with the NU error codes (0 and 3000's), and must therefore 
 * be defined past the end of the NU error codes.
 */

typedef enum
{
   FILE_HANDLE_ERROR       = 200,
   FILE_FIND_NOT_ACTIVE    = 201,   /*File find not creation*/
   FILE_FIND_SUCCESS_END   = 202,   /*File find success terminate*/
   FILE_FIND_SUCCESS_CLOSE = 203,
   SIDB_FIND_SUCCESS_END   = 204,   /*Data item list success terminate*/
   NO_OPENED_FILE          = 205,
   FILE_HANDLE_FULL        = 206,
   FIND_HANDLE_FULL        = 207,
   FIND_HANDLE_ERROR       = 208,
   VOLUME_LIST_SUCCESS_END = 209   /*Volume name list end*/
} ErrCodesT;

typedef enum
{
   FIND_OUT_FILE   = 0,  /*FileDirFind function find out a file*/ 
   FIND_OUT_DIR    = 1,  /*FileDirFind function find out a directory*/ 
   FIND_OUT_NONE   = 2   /*FileDirFind function find out none*/ 
} FileFindResultTypeT;

typedef enum
{
   FILE_FIND_FIRST = 0,
   FILE_FIND_NEXT  = 1,
   FILE_FIND_ALL   = 2,
   FILE_FIND_CLOSE = 3,
   FILE_FIND_CLOSEALL = 4
} FileFindCmdT;

typedef struct 
{
   BalFsiHandleT HandleArray[MAX_FILE_HANDLE_NUM];
   uint8     HandleCount;
}FileHandleArrayT;


typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char  VolumeName[FSI_MAX_VOLUME_NAME_LENGTH];
} FileSysFormatMsgT;

typedef PACKED struct
{
   uint16 Status;
} FileSysFormatRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char  VolumeName[FSI_MAX_VOLUME_NAME_LENGTH];
} GetSpaceInfoMsgT;

typedef PACKED struct
{
   uint16  Status;
   uint32  TotalSpace;
   uint32  FreeSpace;
} GetSpaceInfoRspMsgT;

typedef PACKED struct 
{
   ExeRspMsgT RspInfo;
   BalFsiFileOpenModeT  OpenMode;
   char       FileName [FSI_MAX_PATH_NAME_LENGTH];
}FileOpenMsgT;

typedef PACKED struct
{
   uint16    Status;
   BalFsiHandleT FileHandle;
} FileOpenRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   BalFsiHandleT FileHandle;
   uint32    Offset;    /* Offset in bytes from beginning of file */
   uint16    Length;    /* Number of bytes to read from file. Max is 512*/
} FileBlockReadMsgT;

typedef PACKED struct
{
   uint16   Status; 
   uint16   Length;   /* Actual bytes read */
   uint8    Data[1];  /* Pointer to data, length is defined by Length field */
} FileBlockReadRspMsgT;


typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   BalFsiHandleT FileHandle;
   uint32  Offset;   /* Offset in bytes from beginning of file */
   uint16  Length;   /* Length of the data field, in bytes; max size 512*/
   uint8   Data[1];  /* Pointer to data, length is defined by Length field */
} FileBlockWriteMsgT;

typedef PACKED struct
{
   uint16  Status;   /* FSI status codes (defined above) */
   uint16  Length;   /* Length offset field returned  */
} FileBlockWriteRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   BalFsiHandleT  FileHandle; 
} FileCloseMsgT;

typedef PACKED struct
{
   uint16  Status; 
} FileCloseRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
} FileCloseAllMsgT;

typedef PACKED struct
{
   uint16    Status; 
   BalFsiHandleT FileHandle;
} FileCloseAllRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char  FileName [FSI_MAX_PATH_NAME_LENGTH];
} FileGetAttribMsgT;

typedef PACKED struct
{
   uint16  Status;
   uint32  FileSize;
   uint32  CreatTime;
   uint16  Attrib;
   bool    bReadOnly;
   bool    bHiden;
   bool    bSystem;
   bool    bDir;
   bool    bFile;
} FileGetAttribRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char    FileName [FSI_MAX_PATH_NAME_LENGTH];
   bool    bReadOnly;
   bool    bHiden;
   bool    bSystem;
} FileSetAttribMsgT;

typedef PACKED struct
{
   uint16  Status;
} FileSetAttribRspMsgT; 

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char  OldName [FSI_MAX_PATH_NAME_LENGTH];
   char  NewName [FSI_MAX_FILE_NAME_LENGTH];
} FileDirRenameMsgT;

typedef PACKED struct
{
   uint16  Status;
} FileDirRenameRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char  Name [FSI_MAX_PATH_NAME_LENGTH];
} FileDirDeleteMsgT;

typedef PACKED struct
{
   uint16  Status;
} FileDirDeleteRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   char    FileName [FSI_MAX_PATH_NAME_LENGTH];
   uint32  NewSize; 
} FileTruncateMsgT;

typedef PACKED struct
{
   uint16  Status;
} FileTruncateRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT      RspInfo;
} VolumeListMsgT;

typedef PACKED struct
{
   uint16          Status;
   uint8           SeqNum;
   char            VolumeName[FSI_MAX_VOLUME_NAME_LENGTH];
} VolumeListRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT    RspInfo;
   FileFindCmdT  FindCmd;
   uint8         HandleIndex;
   char          NamePattern [FSI_MAX_PATH_NAME_LENGTH];
} FileDirFindMsgT;

typedef PACKED struct
{
   uint16  Status; 
   uint32  HandleIndex;
   uint16  SeqNum;     /*SeqNum is a sequence number of find result starts at 1*/
   uint32  FileSize;
   uint32  CreateTime;
   uint8   ResultType; /*If 0, ETS display <File>. If 1, ETS display <DIR>*/
   char    ResultName [FSI_MAX_FILE_NAME_LENGTH];
} FileDirFindRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT      RspInfo;
   char  DirName [FSI_MAX_PATH_NAME_LENGTH];
} MakeDirMsgT;

typedef PACKED struct
{
   uint16  Status;
} MakeDirRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
   uint16      Type;
   uint16      ID;
   uint32      Offset;
   uint16      Length;    /*max to 128*/
} SidbReadMsgT;

typedef PACKED struct
{
   uint16  Status;
   uint16  Length;   /* Actual bytes read */
   uint8   Data[1];  /* Pointer to data, length is defined by Length field */
}SidbReadRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
   uint16      Type;
   uint16      ID;
   uint32      Offset;
   uint16      Length;   /* Length of the data field, in bytes; max size 128   */
   uint8       Data[1];  /* Pointer to data, length is defined by Length field */
}SidbWriteMsgT;

typedef PACKED struct
{
   uint16  Status;
   uint16  Length;
}SidbWriteRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
   uint16      Type;
   uint16      ID;
}SidbDeleteMsgT;

typedef PACKED struct
{
   uint16  Status;
}SidbDeleteRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
   uint16      Type;
   uint16      ID;
} SidbListMsgT;

typedef PACKED struct
{
   uint16  Status;
   uint16  SeqNum;
   uint16  Type;
   uint16  ID;
   uint16  Length;
} SidbListRspMsgT;

typedef PACKED struct
 {
 	ExeRspMsgT      RspInfo;
 	uint16	type;
 	uint16	id;
     uint16  Length;      /* Length of the data field, in bytes; max size 512   */
 	uint16	Offset;
     uint8   Data[1];     /* Pointer to data, length is defined by Length field */
 }FfsSidbWriteMsgT;
 
 typedef PACKED struct
 {
 	int16	Status;
 }FfsSidbWriteRspMsgT;
 
 typedef PACKED struct
 {
 	ExeRspMsgT      RspInfo;
 	uint16	type;
 	uint16	id;
 	uint16  Length;
 	uint16  Offset;
 } FfsSidbReadMsgT;
 
 typedef PACKED struct
 {
 	int16	Status;
 	uint16	Length;      /* Actual bytes read */
 	uint8	Data[1];     /* Pointer to data, length is defined by Length field */
 }FfsSidbReadRspMsgT;

typedef enum
 {
 	  PST_EVT_1,
     PST_EVT_2,
    PST_EVT_3,
     PST_EVT_MAX
 } PstEventIdT;

 /*------------------------------------------------------------------------
 * Global function prototypes
 *------------------------------------------------------------------------*/
  #ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*****************************************************************************

  FUNCTION NAME: PstFsiRegister
  
  DESCRIPTION:   This routine is used to register a callback to notify the 
                 caller when the Fsi Event has happened. 
                 
  PARAMETERS:    EventFuncP  - the function pointer
  RETURNS:       Register Identifier

*****************************************************************************/
RegIdT PstEventRegister(BalEventFunc EventFuncP);


#ifdef __cplusplus
}
#endif /* __cplusplus */

/*****************************************************************************
 
  FUNCTION NAME: PstFsMboxHandler                       

  DESCRIPTION:

    This routine is the FS message handler

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
void  PstFsMboxHandler (uint32 MsgId, void* MsgDataP, uint32 MsgSize);

/*****************************************************************************
 
  FUNCTION NAME: VfsMailboxInit                       

  DESCRIPTION:

    This routine initializes the FS message handler

  PARAMETERS:

    None
    
  RETURNED VALUES:

    None

*****************************************************************************/
void  VfsMailboxInit(void);

/******************FSI END*************************/




#endif





