/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      Btl_ddb.c
*
*   DESCRIPTION:    This file implements the API of device data base.
*
*   AUTHOR:         Gili Friedman
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_bmg_ddb.h"
#include "Btl_config.h"
#include "bttypes.h"
#include "utils.h"
#include "ddb.h"
#include "bthal_fs.h"
#include "osapi.h"
#include "btl_defs.h"
#include "debug.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BMG);

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define BTL_BMG_DDB_NOT_FOUND    0xffff

#define BTL_BMG_DDB_MAX_NAME_LEN		100

/****************************************************************************
 *
 * Data
 *
 ****************************************************************************/

/* The device database is kept in memory. */
static U32            			numDdbRecs = 0;
static BtlBmgDeviceRecord 		devDb[BTL_BMG_DDB_CONFIG_MAX_ENTRIES];
static U8           				dbName[BTL_BMG_DDB_MAX_NAME_LEN + 1] = {0};
static BtlBmgDdbNotificationCb	ddbNotificationsCb = NULL;

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

static I16  DdbFindRecord(const BD_ADDR *bdAddr);
static void DdbCreateName(void);

BtStatus BTL_BMG_DDB_RegisterForNotifications(BtlBmgDdbNotificationCb cbFunc)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DDB_RegisterForNotifications");

	if (cbFunc == NULL)
	{
		ddbNotificationsCb = NULL;
	}
	else
	{
		BTL_VERIFY_ERR((ddbNotificationsCb == NULL), BT_STATUS_IN_USE, ("A client is already registered for DDB notifications"));

		ddbNotificationsCb = cbFunc;
	}
	
	BTL_FUNC_END();
	
	return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_Open()
{
    BtStatus status = BT_STATUS_FAILED;
    BthalFsFileDesc		fd;
    I16		count;
	BTHAL_U32	numRead;
	
	BTL_FUNC_START("BTL_BMG_DDB_GetNumOfDeviceRecords");
	

    /* See if the device database file exists. If it does not exist then
	* create a new one.
	*/
    numDdbRecs = 0;
	
    /* Create the ddb file based on the radio's BD_ADDR */
    DdbCreateName();
    if ((BTHAL_FS_Open(dbName,(BTHAL_FS_O_RDONLY | BTHAL_FS_O_BINARY),&fd)) != BT_STATUS_HAL_FS_SUCCESS) {
        /* The file does not exist so we are done */
        Report(("DeviceDB: Device Database file does not exist\n"));
        status = BT_STATUS_SUCCESS;
        goto done;
    } else {
        Report(("DeviceDB: Device database file opened\n"));
    }
	
    /* Read in the contents of the database file. The first item in the 
	* database file is the number of items 
	*/
	
    if (((BTHAL_FS_Read(fd, &numDdbRecs, sizeof(numDdbRecs), &numRead)) != BT_STATUS_HAL_FS_SUCCESS)  || (numDdbRecs > BTL_BMG_DDB_CONFIG_MAX_ENTRIES)) {
        Report(("DeviceDB: Error reading the number of items\n"));
        /* Reset the number of records in the database, since 
		* reading the number of items failed. 
		*/
        numDdbRecs = 0;
        goto close;
    }
    Report(("DeviceDB: Database contains %d records\n", numDdbRecs));
	
    /* Read the elements */
    for (count = 0; count < numDdbRecs; count++) {
        if ((BTHAL_FS_Read(fd, &(devDb[count]), sizeof(BtlBmgDeviceRecord), &numRead)) != BT_STATUS_HAL_FS_SUCCESS) {
            Report(("DeviceDB: Error reading item %d\n",count));
            /* Set the number of records in the database to
			* the position that just failed.  Our next additional
			* record will start here.
			*/
            numDdbRecs = count;
            goto close;
        }
    }
    status = BT_STATUS_SUCCESS;
close:
	BTHAL_FS_Close(fd);
done:
	BTL_FUNC_END();
	
    return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_Close(void)
{
    BtStatus status = BT_STATUS_FAILED;

	BTL_FUNC_START("BTL_BMG_DDB_Close");
	
    if (BTL_BMG_DDB_Flush() == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }

    dbName[0] = 0;
    numDdbRecs = 0;

	BTL_FUNC_END();
		
    return status;
}


/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_Flush()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Force the database to be written to disk. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_Flush(void)
{
	BtStatus	status = BT_STATUS_FAILED;
	BthalFsFileDesc	fd;
	I16		count;
	BTHAL_U32 numWritten;

	BTL_FUNC_START("BTL_BMG_DDB_Flush");

    /* Create the database file and copy in the entries if any exist */
    if (dbName[0] == 0) {
        goto done;
    }
    Assert(numDdbRecs <= BTL_BMG_DDB_CONFIG_MAX_ENTRIES);
      
    Report(("DeviceDB: Creating Device database file %s.\n", dbName));
    if ((BTHAL_FS_Open(dbName, (BTHAL_FS_O_CREATE |BTHAL_FS_O_WRONLY |BTHAL_FS_O_BINARY), &fd)) != BT_STATUS_HAL_FS_SUCCESS) {
        Report(("DeviceDB: Error creating %s.", dbName));
        goto done;
    }

    /* Write out the number of items */
    if ((BTHAL_FS_Write(fd, &numDdbRecs, sizeof(numDdbRecs), &numWritten)) != BT_STATUS_HAL_FS_SUCCESS) {
        Report(("DeviceDB: Error writing numDdbRecs to database file\n"));
        goto close;
    }

    /* Write the entries */
    for (count = 0; count < numDdbRecs; count++) {
         if ((BTHAL_FS_Write(fd, &(devDb[count]), sizeof(BtlBmgDeviceRecord), &numWritten)) != BT_STATUS_HAL_FS_SUCCESS) {
            Report(("DeviceDB: Error writing entry %d\n", count));
            goto close;
        }
    }
    Report(("DeviceDB: Wrote %d items to database\n", count));
    status = BT_STATUS_SUCCESS;
close:
    BTHAL_FS_Close(fd);
done:
	
		BTL_FUNC_END();

		return status;
}


/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_AddRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Add a record to the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_AddRecord(const BtlBmgDeviceRecord* record)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	BOOL	recordAdded;	
	U32 count;
	
	BTL_FUNC_START("BTL_BMG_DDB_AddRecord");
	
	BTL_VERIFY_FATAL((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));
	
    count = DdbFindRecord(&(record->bdAddr));
    if (count == BTL_BMG_DDB_NOT_FOUND) 
	{
		/* A record with this bdAddr does not exist so add it */
		
		/* Test if we reached max number of entries */
		BTL_VERIFY_ERR(numDdbRecs < BTL_BMG_DDB_CONFIG_MAX_ENTRIES, BT_STATUS_FAILED, 
			("DeviceDB: Error, reached max number of entries (%d).", BTL_BMG_DDB_CONFIG_MAX_ENTRIES))
			
		count = numDdbRecs;
		numDdbRecs++;
		BTL_BMG_DDB_InitRecord(&devDb[count]);

		recordAdded = TRUE;
    }
	else
	{
		recordAdded = FALSE;
	}
	
	if (record->keyType != KEY_NOT_DEFINED)
	{
		devDb[count] = *record;
	}
	else
	{
		/* do not copy link key fields - they are not valid.*/
		devDb[count].bdAddr = record->bdAddr;
		devDb[count].trusted = record->trusted;
		devDb[count].trustedPerService = record->trustedPerService;
		devDb[count].classOfDevice = record->classOfDevice;
		devDb[count].services = record->services;
		OS_MemCopy(&(devDb[count].name[0]), &(record->name[0]), sizeof(devDb[count].name));
		devDb[count].psi = record->psi;
		OS_MemCopy((U8*)&(devDb[count].userData[0]), (U8*)&(record->userData[0]), BTL_BMG_DDB_CONFIG_DEVICE_REC_USER_DATA_SIZE);
	}

	if (ddbNotificationsCb != NULL)
	{
		if (recordAdded == TRUE)
		{
			(ddbNotificationsCb)(BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_ADDED, &devDb[count]);
		}
		else
		{
			(ddbNotificationsCb)(BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_MODIFIED, &devDb[count]);
		}
	}
	
	if (BTL_BMG_DDB_Flush() != BT_STATUS_SUCCESS) {
        Report(("DeviceDB: Error saving record.", count));
    }
	
	BTL_FUNC_END();
	
    return status;
}



/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_FindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_FindRecord(const BD_ADDR *bdAddr, BtlBmgDeviceRecord* record)
{
		BtStatus status = BT_STATUS_SUCCESS;
    I16 count;

		BTL_FUNC_START("BTL_BMG_DDB_FindRecord");
	
		BTL_VERIFY_FATAL((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
		BTL_VERIFY_FATAL((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));

    count = DdbFindRecord(bdAddr);

	if (count == BTL_BMG_DDB_NOT_FOUND)
	{
		BTL_RET(BT_STATUS_FAILED);
	}

    *record = devDb[count];
		
	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_DeleteRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete a record from the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_DeleteRecord(const BD_ADDR *bdAddr)
{
	BtStatus status = BT_STATUS_SUCCESS;
  I16 count;
	
	BTL_FUNC_START("BTL_BMG_DDB_DeleteRecord");
	
	BTL_VERIFY_FATAL((0 != bdAddr), BT_STATUS_INVALID_PARM, ("Null bdAddr"));
	
  	count = DdbFindRecord(bdAddr);
	BTL_VERIFY_FATAL((count != BTL_BMG_DDB_NOT_FOUND), BT_STATUS_FAILED, (""));
	
	/* If it is the last record then all we need to do is reduce
	* numDdbRecs by 1. Otherwise we need to shift the array
	*/
	numDdbRecs--;
	if (count < numDdbRecs) {
		/* We need to shift the array */
		OS_MemCopy((U8*)(&devDb[count]), (U8*)(&devDb[count+1]),
			(U16)((numDdbRecs - count) * sizeof(BtlBmgDeviceRecord)));
	}

	status = BTL_BMG_DDB_Flush();
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_Flush Failed (%s)", pBT_Status(status)));

	BTL_FUNC_END();

  return status;	
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_DeleteAllRecords()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete all records from the database.
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */
BtStatus BTL_BMG_DDB_DeleteAllRecords(void)
{
	BtStatus 		status = BT_STATUS_SUCCESS;
	BtFsStatus	fsStatus = BT_STATUS_HAL_FS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_DDB_DeleteAllRecords");

	/* Delete ddb file if it exists */
	fsStatus = BTHAL_FS_Remove(dbName);

	BTL_VERIFY_ERR((BT_STATUS_HAL_FS_SUCCESS == fsStatus) || (BT_STATUS_HAL_FS_ERROR_NOTFOUND == fsStatus), 
					BT_STATUS_FAILED, ("DeviceDB: Error deleting DB file."));
	
	/* Delete all records */
	numDdbRecs = 0;

	BTL_FUNC_END();

   return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_EnumDeviceRecords()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enumerate the records in the device database. 
 *
 * Return:    status of the operation.
 */
BtStatus BTL_BMG_DDB_EnumDeviceRecords(U32 i, BtlBmgDeviceRecord* record)
{
	BtStatus status = BT_STATUS_SUCCESS;
	
	BTL_FUNC_START("BTL_BMG_DDB_EnumDeviceRecords");
	
	BTL_VERIFY_FATAL((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));
	BTL_VERIFY_FATAL((i < numDdbRecs), BT_STATUS_FAILED, (""));
		
  *record = devDb[i];

	BTL_FUNC_END();

  return status;
}

/*---------------------------------------------------------------------------
 *            BTL_BMG_DDB_InitRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes record - should be called before adding a new record 
 *
 * Return:    status of the operation.
 */
BtStatus BTL_BMG_DDB_InitRecord(BtlBmgDeviceRecord* record)
{
	BtStatus 			status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DDB_InitRecord");

	BTL_VERIFY_FATAL(0 != record, BT_STATUS_INVALID_PARM, ("Null record"));

	OS_MemSet((U8 *)record, 0, sizeof(BtlBmgDeviceRecord));

	record->keyType = KEY_NOT_DEFINED;

	record->psi.psMode = BTL_BMG_DDB_INVALID_PSI_MODE;
	record->psi.psRepMode = BTL_BMG_DDB_INVALID_PSI_REP_MODE;

	BTL_FUNC_END();

	return status;
}

BtStatus BTL_BMG_DDB_ClearSecurityInfo(BtlBmgDeviceRecord* record)
{
	BtStatus 	status = BT_STATUS_SUCCESS;
	I16 count;
	
	BTL_FUNC_START("BTL_BMG_DDB_ClearSecurityInfo");
	
	BTL_VERIFY_FATAL((0 != record), BT_STATUS_INVALID_PARM, ("Null record"));
	
	count = DdbFindRecord(&(record->bdAddr));
	BTL_VERIFY_FATAL((BTL_BMG_DDB_NOT_FOUND != count), BT_STATUS_INVALID_PARM, ("Record is not found"));
	
	devDb[count].trusted = 0;
	devDb[count].trustedPerService = 0;
	devDb[count].keyType = KEY_NOT_DEFINED;

	status = BTL_BMG_DDB_Flush();
	BTL_VERIFY_FATAL(status == BT_STATUS_SUCCESS, status, ("BTL_BMG_DDB_Flush Failed (%s)", pBT_Status(status)));
		
	BTL_FUNC_END();
	
	return status;
}

BtStatus BTL_BMG_DDB_GetNumOfDeviceRecords(U32 *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DDB_GetNumOfDeviceRecords");
	
	BTL_VERIFY_FATAL((0 != answer), BT_STATUS_INVALID_PARM, ("Null answer"));

	*answer = numDdbRecs;
	
	BTL_FUNC_END();

	return status;

}

BtStatus BTL_BMG_DDB_IsFull(BOOL *answer)
{
	BtStatus status = BT_STATUS_SUCCESS;

	BTL_FUNC_START("BTL_BMG_DDB_IsFull");
	
	BTL_VERIFY_FATAL((0 != answer), BT_STATUS_INVALID_PARM, ("Null answer"));

	*answer = (numDdbRecs == BTL_BMG_DDB_CONFIG_MAX_ENTRIES);
	
	BTL_FUNC_END();

	return status;
}

/*---------------------------------------------------------------------------
 *            DdbFindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    Index of BtlBmgDeviceRecord if found otherwise BTL_BMG_DDB_NOT_FOUND.
 */
static I16 DdbFindRecord(const BD_ADDR *bdAddr)
{
    I16 count;

    for (count = 0; count < numDdbRecs; count++) {
        if (OS_MemCmp(bdAddr->addr, 6, devDb[count].bdAddr.addr, 6)) {
            /* The record is found so return it */
            return count;
        }
    }
    return BTL_BMG_DDB_NOT_FOUND;
}

/*---------------------------------------------------------------------------
 *            DdbCreateName()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates a database filename based on the radio's bluetooth
 *            device address.
 */
static void DdbCreateName()
{
	BTL_FUNC_START("DdbCreateName");
	
	BTL_VERIFY_FATAL_NO_RETVAR((OS_StrLen(BTHAL_FS_DDB_FILE_NAME) <= BTL_BMG_DDB_MAX_NAME_LEN),
									("BMG Device DB Name Too long (Max: %d", BTL_BMG_DDB_MAX_NAME_LEN));

	dbName[0] = dbName[BTL_BMG_DDB_MAX_NAME_LEN] = 0;
	
	OS_StrCpy((char *)dbName, BTHAL_FS_DDB_FILE_NAME);

	BTL_FUNC_END();
}


 

