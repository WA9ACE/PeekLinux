


#ifndef __BALDB_H__

#define __BALDB_H__


#include "balfsiapi.h"
#include "balfsicfg.h"
#ifdef __cplusplus
extern "C" {
#endif

enum
{
	VDB_ERR_NONE = 0,
	VDB_ERR_PARAM = 0x2000,
	VDB_ERR_SYS,
	VDB_ERR_MEM,
	VDB_ERR_OPEN,
	VDB_ERR_READ,
	VDB_ERR_SEEK,
	VDB_ERR_WRITE,
	VDB_ERR_REMOVE,
	VDB_ERR_REC_EMPTY,
	VDB_ERR_NOSPACE,
	VDB_ERR_MAXOPEN,
	VDB_ERR_CHECKSUM,
	VDB_ERR_DBTYPE,
	VDB_ERR_TAG,
	VDB_ERR_VERSION,
	VDB_ERR_CREATE,
	VDB_ERR_OPENED,
	VDB_ERR_SIZE,
	VDB_ERR_NOTEXIST,
	/* Add new error code here */

	VDB_MAX_ERR_NUM
};

typedef struct
{
	uint8 iTaskId;
	uint8 iMailboxId;
	uint8 iMsgId;
}BalDbSendMsgT;

typedef struct 
{
	uint8	IsUsed;
	uint16	RecSize;
} BalDbRecInfoT;


typedef struct
{
	uint16		BlkSize;	/* 0 ~ 255 */
	uint16		BlkNum;
	void *		pRam;
}VarTypeDBInfoT;




#define BAL_DB_TAG				0xDBDB	

#define MAX_DB_HANDLES			8/*4*/

#define BAL_DB_INVALID_HANDLE	0xFFFF

#define	BAL_DB_BLK_FREE			0xFFFF
#define BAL_DB_BLK_EOL			0xFFFE

#define MAX_DB_NAME_LEN			FSI_MAX_FILE_NAME_LENGTH
#define MAX_DB_PATH_LEN			64

#define BAL_DB_CUR_VERSION		2 

#define BAL_DB_REC_FREE			0xFF
#define BAL_DB_REC_USED			0x5A


#define TEMP_BUF_SIZE			1024

enum
{
	BAL_DB_TYPE_FIX = 0,
	BAL_DB_TYPE_VARIABLE
};


/* ------------------------------------
Macro of Database File Header 
------------------------------------ */
typedef struct
{
	uint16		DbFileTag;	/* 0xDBDB */
	uint8		DbType;
	uint8		Version;
	uint8		Checksum;
	uint8		pad[3];
} BalDbFileHdrT;

/* ---------------------------------------
Macro of Fix/Variable Type Database DB header 
---------------------------------------- */
typedef struct
{
	uint16		ItemSize;
	uint16		NumItem;
	uint32		DataOffset;
	uint8		Checksum;
	uint8		pad[3];
} BalDbInfoHdrT;

#ifdef BYD_USE_SIM
#pragma pack (1)
#endif
/* ------------------------------------
Macro of Fix Type Record Header
------------------------------------- */
typedef  PACKED struct
{
	uint8		UseFlag;
	uint8		Checksum;
} BalDbRecHdrT;

/* -----------------------------------------
Macro of Var Type Record Index Header
------------------------------------------ */
typedef PACKED struct
{
	uint32		DataLen;
	uint16		FirstBlkId;
} BalDbVarDbIdxRecT;

typedef PACKED struct
{
	BalDbRecHdrT		RecHead;
	BalDbVarDbIdxRecT	IndexRec;
} BalDbVarDbExtIdxRecT;

/* -----------------------------------------
Macro of Var Type DB Block Header
------------------------------------------ */
typedef  PACKED struct
{
	uint16		NextBlkId;
} BalDbVarDbBlockHdrT;
#ifdef BYD_USE_SIM
#pragma pack ()
#endif

/* -------------------------------------
Macro of Database Handle Manager
-------------------------------------- */
typedef struct
{
	uint8			DbType;

	BalFsiHandleT		IdxFileHandle;
	BalFsiHandleT		DataFileHandle;

	uint32			IdxDbOffset;
	uint32			DataDbOffset;

	uint16			FreeBlks;
	uint16			BlkSize;
	uint16			MaxBlks;

	uint16			MaxRecs;
	uint16			RecSize;

	bool			IsUsed;
	bool 			IsDataInit;
	/*	ExeSemaphoreT	CurDbLock;	*/ /* Maybe no use, mutex access at apllication layer */

	/* In memory pointed by pBlkBitMap,
   	Bit set to 1 means the corresponding block is free. 
   	bit set to 0 means the corresponding block is used. */
	uint8	*		pBlkBitMap;

	char			DbName[MAX_DB_NAME_LEN + 1];
} BalDbHandleT;


/* --------------------------
  Database operation APIs  
-------------------------- */

/* 
* Prototype:	uint32   BalDbExit ( void );
* Inputs: 
*	N/A
* Outputs:
*	ERR_NONE, if initialize BAL Database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to initialize BAL Database module.
*/

uint32   BalDbInit ( void );

/*
* Prototype:	uint32   BalDbExit ( void );
* Inputs: 
*	N/A
* Outputs:
*	ERR_NONE, if terminate BAL Database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to terminate BAL Database module.
*/

uint32   BalDbExit ( void );

/*
* Prototype:	uint32   BalDbOpenFixType(const char* DBName, uint16* pDbHandle);
* Inputs: 
*	DBName:	name of sub-database module. 
*	pDbHandle:      handle of the opened sub-database module. This field will be used in the following 
                        operation of this sub-database, such as read/write record from/to it.	
* Outputs:
*	ERR_NONE, if open or initialize specified sub-database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to open or initialize specified sub-database module.
*/

uint32 BalDbOpenFixType(const char* DBName, uint16* pDbHandle);

/*
* Prototype:	uint32 BalDbCreateFixType  (const char* DBName, uint16 *  pDbHandle, uint16 RecCount, uint16 RecSize);
* Inputs: 
* 	DBName:   	name of the database.
*	RecCount:	minimum number of records/blocks
*	RecSize:	size of each record/block. 
*	PDbHandle: 	Relevant DB Handle, when read and write to this DB will use this field.
* Outputs:
*	ERR_NONE, if create a specified new fix type sub-database successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to create a specified new fix type sub-database.
*/

uint32 BalDbCreateFixType  (const char* DBName, uint16 *  pDbHandle, uint16 RecCount, uint16 RecSize);

/* -------------------------------------------------------------------------------------
*
* Prototype:	uint32 BalDbCreateData (uint16 DbHandle, 
*											uint16 RecCount, uint16 RecSize);
* Inputs: 
 *	DbHandle: 	Relevant DB Handle, when read and write to this DB will use this field.
*  pSendMsg: 	The handle of send msg
* Outputs:
*	ERR_NONE, if create and init data successfully;
*	otherwise, the relevant error code. 
* Description:
*	Create and init data after create file.
*
* ------------------------------------------------------------------------------------ */

uint32 BalDbCreateData(uint16 DbHandle,BalDbSendMsgT* pSendMsg);

/*
* Prototype:	uint32   BalDbOpen (const char* DBName, uint16* pDbHandle);
* Inputs: 
*	DBName:	name of sub-database module. 
*	pDbHandle:      handle of the opened sub-database module. This field will be used in the following 
                        operation of this sub-database, such as read/write record from/to it.	
* Outputs:
*	ERR_NONE, if open or initialize specified sub-database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to open or initialize specified sub-database module.
*/

uint32 BalDbOpenVarType(const char* DBName, uint16* pDbHandle, void * pRam);

/*
*Prototype:	uint32 BalDbCreateVarType  (const char* DBName, uint16 *  pDbHandle, uint16 RecCount, VarTypeDBInfoT * pInfo);
* Inputs: 
*	DBName:   	name of the database.
*	RecCount:	minimum number of records/blocks
*	pInfo:		the pointer of the information of the variable length type data base.
*	PDbHandle: 	Relevant DB Handle, when read and write to this DB will use this field.
* Outputs:
*	ERR_NONE, if create a specified new variable length type sub-database successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to create a specified new variable length type sub-database. 
*/

uint32 BalDbCreateVarType  (const char* DBName, uint16 *  pDbHandle, uint16 RecCount, VarTypeDBInfoT * pInfo);

/*
* Prototype:	uint32   BalDbClose (uint16 DbHandle);
* Inputs: 
*	pDbHandle:      handle of the opened sub-database module, returned by the previous BalDbClose function. 
* Outputs:
*	ERR_NONE, if close or exit specified sub-database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to close or exit specified sub-database module.
*/

uint32 BalDbClose(uint16 DbHandle);

/*
* Prototype:	uint32   BalDbDelete (const char* DBName);
* Inputs: 
*	DBName:	name of sub-database module you want to delete. 
* Outputs:
*	ERR_NONE, if delete specified sub-database module successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to delete specified sub-database module.
*/

uint32 BalDbDelete(const char* DBName);

/* --------------------------
  Record Operation APIs  
-------------------------- */


/*
* Prototype:	uint32  BalDbRecGet( uint16 DbHandle, uint16 RecId, uint32 * size, void * pRecBuf );
* Inputs: 
* 	DbHandle: 	Relevant DB Handle.
*	RecId:   	physical index of specified record.
*	size:		size in byte to be read, and output the real read size in byte.
*	pRecBuf:	buffer used to store the read content.  
* Outputs:
*	ERR_NONE, if get specified record content successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to get specified record content.
*/

uint32 BalDbRecGet( uint16 DbHandle, uint16 RecId, uint32 * size, void * pRecBuf );

/*
* Prototype:	uint32  BalDbGetRecInfo ( uint16 DbHandle, uint16 RecId,  BalDbRecInfoT * pRecInfo );
* Inputs: 
* 	DbHandle: 	Relevant DB Handle.
*	RecId:   	physical index of specified record.
*	pRecInfo:	buffer used to store the read information.  
* Outputs:
*	ERR_NONE, if get specified record information successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to get specified record information.
*/

uint32 BalDbGetRecInfo( uint16 DbHandle, uint16 RecId,  BalDbRecInfoT * pRecInfo );

/*
* Prototype:	uint32  BalDbRecordDelete(uint16 DbHandle, uint32 RecId);
* Inputs: 
* 	DbHandle: 	Relevant DB Handle.
*	RecId:   	physical index of specified record. 
* Outputs:
*	ERR_NONE, if delete specified record successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to delete specified record.
*/

uint32 BalDbRecordDelete(uint16 DbHandle, uint16 RecId);


/*
* Prototype:	uint32  BalDbRecUpdate( uint16 DbHandle, uint16 RecId, uint32 * size, void * pRecBuf );
* Inputs: 
*	DbHandle: 	Relevant DB Handle.
*	RecId:   	physical index of specified record.
*	size:		size in byte to update, and output the real update size in byte.
*	pRecBuf:	buffer used to store the update content.  
* Outputs:
*	ERR_NONE, if update specified record content successfully; otherwise, the relevant error code. 
* Description:
*	Call this member function to update specified record content.
*/

uint32 BalDbRecUpdate(uint16 DbHandle, uint16 RecId, uint32  size, void * pRecBuf );


/* -------------------------
RTOS Semaphore APIs
-------------------------- */
/*
uint32 VdbCreateMtxSem(VDbMtxSemT *SemCbP, uint8 state);

uint32 VdbDeleteMtxSem(VDbMtxSemT *SemCbP);

uint32 VdbGetMtxSem(VDbMtxSemT *SemCbP);

uint32 VdbReleaseMtxSem(VDbMtxSemT *SemCbP);
*/
#ifdef __cplusplus
}
#endif

#endif /* __BALDB_H__ */



