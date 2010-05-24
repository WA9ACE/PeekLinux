

#ifndef BALFSICONFIG_H
#define BALFSICONFIG_H
/***********************************************************


************************************************************/

/*FS VOLUME NAME*/
#define DFS1_VOLUME_NAME		"/sys"	//add root "/"
#define DOSFS1_VOLUME_NAME		"/dos"	//add root "/"
#define NFFS1_VOLUME_NAME		"/nffs"	//add root "/"

#define DFS2_VOLUME_NAME 		"/user"	//add root "/"



/*SIDB type for UI*/
#define SIDB_TYPE_UI_START	0

#define SIDB_TYPE_UI_END	255

#define SIDB_TYPE_BAL_START 256

#define SIDB_TYPE_BAL_END	512

#define SIDB_TYPE_BM_START 513

#define SIDB_TYPE_BM_END    576

/*FILE NAME FOR STORAGE*/

//add root directory "/" for every pathname Randolph Wang
#define BAL_DB_DIRECTORY		"/user/valdb"  		//add root "/"
#define BAL_DB_DATA_PATH		"/user/valdb/data"	//add root "/"


/* phb file */
#define  BAL_PHB_CONTACT_FILE   "phone"

/*sms file */
#define BAL_SMS_FILE_NAME "smsfile"
#define BAL_SMS_DATA_FILE_NAME "smsdatafile"
#define BAL_SMS_EXTRA_FILE_NAME "smsextrafile"

/*smart message db name*/
#define BAL_SMARTMESSAGE_DB_NAME	"smartmessage"

/*scheduler db*/
#define BAL_SCHEDULER_DB_NAME	"scheduler"

/*recent call*/
#define BAL_RC_DB_NAME	"recentcall"

/*DBM file*/
#define DBM_PATH  "/sys/dbmfile"				//add root "/"
#define CP_DBM_FILE "/sys/dbmfile/cpdata"		//add root "/"
#define RF_DBM_FILE "/sys/dbmfile/rfdata"		//add root "/"


/*******************************************/
/*MP3*/

#define MP3_MAIN_DIR			"/user/mp3"		//add root "/"
#define MP3_PATH_VT			"/user/mp3"		//add root "/"
#define MP3_PATH_NAND		"/nffs/mp3"		//add root "/"
#define MP3_PATH_SD			"/dos/mp3"		//add root "/"


/*PHOTO*/
#define CAMERA_SUFFIX_NAME      ".jpg"     


#define CAMERA_PATH_SD        	"/dos/PHOTO"	//add root "/"
#define CAMERA_PATH_FLASH     "/user/photo"		//add root "/"


/*vmemo*/
#define BAL_VOICE_PATH        	"/user/voiceMemo"	//add root "/"

#endif




