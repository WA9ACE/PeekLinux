

#ifndef UICONTENTMANAGER_H_
#define UICONTENTMANAGER_H_

#include "balfsiapi.h"
#include "balfsicfg.h"

#include "buivector.h"
#include "buisettingsdialog.h"

#include "restypedefines.h"

//! Sound type
typedef enum
{
  SOUND_TYPE_MIDI_BUF = 1, /*midi buf*/
  SOUND_TYPE_MIDI_FILE, /*midi file*/
  SOUND_TYPE_TONE,  /*TONE*/
  SOUND_TYPE_DTMF, /*DTMF*/
  SOUND_TYPE_MP3_BUF,
  SOUND_TYPE_MP3_FILE,
  /*To be added further*/
  SOUND_TYPE_NUM
}SoundTypeT;

#define SOUND_MAKE_ID(ID, TYPE) ((TYPE << 28) | (0xFFFFFFF & (ID)))  //!< make sound id
#define SOUND_GET_TYPE(ID)      (SoundTypeT)((0xF0000000 & ID) >> 28)  //!< get sound type
#define SOUND_GET_REAL_ID(ID)   (0x0FFFFFFF & ID)  //!< get real sound id
#define INVALIDATE_RINGER_ID		0  //!< invalidate ringer id

//! The path to store content database
#ifdef BYD_USE_SIM
#define CONTENT_DB_FILE_NAME "\\content.db"
#else
#define CONTENT_DB_FILE_NAME DFS2_VOLUME_NAME"/content.db"
#endif

#define MAX_CID_LENGTH                 FSI_MAX_PATH_NAME_LENGTH  //!< the max length of CID

//! The path to store music files in coyote
#ifdef BYD_USE_SIM
#define MUSIC_RESERVED_FILE_PATH       "\\music"
#define MUSIC_DOWNLOAD_FILE_PATH       "\\music\\download"
#define MUSIC_DRM_FILE_PATH            "\\music\\drm"
#ifdef FEATURE_BREW
#define MUSIC_BREW_RINGER_PATH         "\\brew\\ringer"
#endif

//! The path to store photo files in coyote
#define PHOTO_RESERVED_FILE_PATH       "\\photo"
#define PHOTO_DOWNLOAD_FILE_PATH       "\\photo\\download"
#define PHOTO_DRM_FILE_PATH            "\\photo\\drm"
#ifdef FEATURE_BREW
#define PHOTO_BREW_WALLPAPER_PATH      "\\brew\\shared\\wallpaper"
#endif

//! The path to store unknown files in coyote
#define UNSUPPORT_RESERVED_FILE_PATH   "\\unsupport"
#define UNSUPPORT_DOWNLOAD_FILE_PATH   "\\unsupport\\download"
#define UNSUPPORT_DRM_FILE_PATH        "\\unsupport\\drm"

//! The path to store voicememo files in coyote
#define VOICEMEMO_RESERVED_FILE_PATH   "\\voicememo"
#define VOICEMEMO_DOWNLOAD_FILE_PATH   "\\voicememo\\download"
#define VOICEMEMO_DRM_FILE_PATH        "\\voicememo\\drm"

#else
//! The path to store music files in sys
#define MUSIC_RESERVED_FILE_PATH       DFS2_VOLUME_NAME"/music"
#define MUSIC_DOWNLOAD_FILE_PATH       DFS2_VOLUME_NAME"/music/download"
#define MUSIC_DRM_FILE_PATH            DFS2_VOLUME_NAME"/music/drm"
#ifdef FEATURE_BREW
#define MUSIC_BREW_RINGER_PATH         DFS2_VOLUME_NAME"/brew/ringer"
#endif

//! The path to store photo files in sys
#define PHOTO_RESERVED_FILE_PATH       DFS2_VOLUME_NAME"/photo"
#define PHOTO_DOWNLOAD_FILE_PATH       DFS2_VOLUME_NAME"/photo/download"
#define PHOTO_DRM_FILE_PATH            DFS2_VOLUME_NAME"/photo/drm"
#ifdef FEATURE_BREW
#define PHOTO_BREW_WALLPAPER_PATH      DFS2_VOLUME_NAME"/brew/shared/wallpaper"
#endif

//! The path to store unknown files in sys
#define UNSUPPORT_RESERVED_FILE_PATH   DFS2_VOLUME_NAME"/unsupport"
#define UNSUPPORT_DOWNLOAD_FILE_PATH   DFS2_VOLUME_NAME"/unsupport/download"
#define UNSUPPORT_DRM_FILE_PATH        DFS2_VOLUME_NAME"/unsupport/drm"

//! The path to store voicememo files in coyote
#define VOICEMEMO_RESERVED_FILE_PATH   DFS2_VOLUME_NAME"/voicememo"
#define VOICEMEMO_DOWNLOAD_FILE_PATH   DFS2_VOLUME_NAME"/voicememo/download"
#define VOICEMEMO_DRM_FILE_PATH        DFS2_VOLUME_NAME"/voicememo/drm"

#endif

//! The path to store photo files in SD card
#ifdef SYS_OPTION_MMC
#define SD_PHOTO_FILE_PATH             DOSFS1_VOLUME_NAME"/PHOTO"
#define SD_MUSIC_FILE_PATH             DOSFS1_VOLUME_NAME"/MUSIC"
#define SD_UNSUPPORT_FILE_PATH         DOSFS1_VOLUME_NAME"/OTHER"
#define SD_VOICEMEMO_FILE_PATH         DOSFS1_VOLUME_NAME"/VMEMO"
#endif

//! File path seperator
#ifdef BYD_USE_SIM
#define FILE_PATH_SEPARATOR "\\"
#define CHAR_FILE_PATH_SEPARATOR '\\'
#else
#define FILE_PATH_SEPARATOR "/"
#define CHAR_FILE_PATH_SEPARATOR '/'
#endif

//! Content Action type
typedef enum
{
  CONTENT_PLAY = ( 1 << 1),
  CONTENT_DISPLAY = ( 1 << 2),
  CONTENT_EXECUTE = ( 1 << 3),
  CONTENT_PRINT = ( 1 << 4),
  CONTENT_UNKNOW = ( 1 << 5 )
} ContentActionT;

//! Content right type
typedef struct
{
  bool NeedRight;      //!< denote specified permission can or cannot grant to the content
  bool Expired;        //!< indicate whether the specified permission is expired
  bool NoLimit;        //!< indicate no right limitation
  bool UseCount;       //!< indicate validity of constrain COUNT
  bool UseInterval;    //!< indicate validity of constrain INTERVAL
  bool UseTimeRange;   //!< indicate validity of constrain DATETIME
  uint32 Count;        //!< Left COUNT to use the content
  uint32 Interval;     //!< Left INTERVAL to use the content
  uint32 StartTime;    //!< Start time
  uint32 EndTime;      //!< End time
} ContentRightT;

//! Content operation result type
typedef enum
{
  CONTENT_RESULT_SUCCESS = 0,              /*Success */
  CONTENT_RESULT_NOT_EXIST,                /*The actual content file not exist */
  CONTENT_RESULT_NO_RIGHT,                 /*Invalid rights permission, thus */
  /*  no rights for content rendering  */
  /*  for the given permission. */
  CONTENT_RESULT_FAIL_PARSE_RIGHT,
  CONTENT_RESULT_FAIL_PARSE_CONTENT,
  CONTENT_RESULT_FAIL_CID_MISMATCH,        /*DRM package parsing failure, CID */
  /*  in rights part and CIDin content */
  /*  part mismatch. */
  CONTENT_RESULT_FAIL_OPERATE_RIDB,
  CONTENT_RESULT_INVALID_INPUT,
  CONTENT_RESULT_INVALID_MESSAGE_TYPE,
  CONTENT_RESULT_FAIL_FILE_OPERATE,
  CONTENT_RESULT_FAIL_MEM_ALLOC,
  CONTENT_RESULT_FAIL_PARSE_DCF,
  CONTENT_RESULT_FAIL_GET_KEY,             /* Fail to get key and ken len to decrypt DCF */
  CONTENT_RESULT_FAIL_DECRYPT,
  CONTENT_RESULT_CID_NOT_EXIST,            /*CID doesn't exist in Right Information */
  /*  Database */
  CONTENT_RESULT_RIGHT_NOT_FOUND,          /*no rights infomation found corresponding */
  /*  to the CID given. */
  CONTENT_RESULT_IMSI_MISMATCH,            /* IMSINo mismatch */
  CONTENT_RESULT_FAIL_GET_RIGHT,           /* Rights retrievement (from database) failed. */
  CONTENT_RESULT_RIGHT_EXPIRED,
  CONTENT_RESULT_INVALID_PERMISSION_TYPE,
  CONTENT_RESULT_START_TIME_NOT_REACH,     /*datatime hasn't reached the start time */
  CONTENT_RESULT_START_END_NOT_SET,        /*both start and end time not set */
  CONTENT_RESULT_FAIL_OPEN_FILE,
  CONTENT_RESULT_E_GET_MEDIA_LIST,         /*Fail to scan media files */
  CONTENT_RESULT_E_READ_CONTENT_DB,        /*Fail to read content database */
  CONTENT_RESULT_E_SAVE_CONTENT_DB,        /*Fail to save content database */
  CONTENT_RESULT_E_NOT_FOUND,              /*Content crosponding to CID not found */
  CONTENT_RESULT_E_READ_SIDB,              /*Fail to read SIDB */
  CONTENT_RESULT_UNKNOWN_FILE_TYPE,        /*Unknown media file type. */
  CONTENT_RESULT_FILE_TYPE_DCF,            /*The type of file want to open is DCF. */
  CONTENT_RESULT_FILE_TYPE_UNKNOWN,        /*The type of file want to open is UNKNOWN */
  CONTENT_RESULT_E_UNDEFINED,              /*Undefined error. */
  
  CONTENT_RESULT_ERROR,
  CONTENT_RESULT_FILE_EXIST
} ContentResultT;

//! Content type
typedef enum
{
  CONTENT_TYPE_NORMAL = 0,
  CONTENT_TYPE_DRM,
  CONTENT_TYPE_DCF,
  CONTENT_TYPE_DELETED,
  CONTENT_TYPE_UNKNOW
} ContentTypeT;

//! Media type
typedef enum
{
  MEDIA_PHOTO_JPG,
  MEDIA_PHOTO_BMP,
  MEDIA_PHOTO_PNG,
  MEDIA_PHOTO_MEMORY,
  MEDIA_MUSIC_MIDI,
  MEDIA_MUSIC_MP3,
  MEDIA_MUSIC_MMF,
  MEDIA_VM_MEMO,
  MEDIA_VM_VOICE,
  MEDIA_VM_RECORD,
  MEDIA_VM_ANSWER_REC,
  MEDIA_OTHER
} MediaTypeT;

//! storage device type
typedef enum
{
  MEMORY_PHONE,
  MEMORY_CARD
} MemoryTypeT;

//! Media file info
typedef struct
{
  uint32 Id;
  MediaTypeT Type ;
  uint32 Directory;
  BalFsiFileInfoT FileInfo;
  ContentTypeT ContentType;
  char CID[MAX_CID_LENGTH];
  SidbStringT UserFriendlyName;
} MediaFileInfoT;

typedef MediaFileInfoT PhotoFileInfoT;
typedef MediaFileInfoT MusicFileInfoT;

//! Media category
typedef enum
{
  MEDIA_CATEGORY_MUSIC = 0,
  MEDIA_CATEGORY_PHOTO,
  MEDIA_CATEGORY_VOICEMEMO,
  MEDIA_CATEGORY_UNSUPPORT,
  MEDIA_CATEGORY_MAX
} MediaCategoryT;

//! Media path
typedef enum
{
  MEDIA_PATH_RESERVED = 0,
  MEDIA_PATH_DOWNLOAD,
  MEDIA_PATH_DRM,
#ifdef FEATURE_BREW
  MEDIA_PATH_BREW,
#endif
  //add new types here

  MEDIA_PATH_MAX_USED,
  //don't add new types behind MEDIA_PATH_MAX_USED
  
  MEDIA_PATH_MAX = 10
} MediaPathT;

//! Sort criterion type
typedef enum
{
  SORT_BY_FILENAME = 0,
  SORT_BY_FILESIZE,
  SORT_BY_FILEDATE,
  SORT_BY_MAX
} SortCriterionT;

//! Sort order type
typedef enum
{
  ASCEND = 0,
  DESCEND
} SortOrderT;

//! Contentmanager class define
class ContentmanagerC
{
public:
  ~ContentmanagerC(void);
  static ContentmanagerC* GetInstance(void);
  static void DelInstance(void);
  
  bool ContentInit(void);
  ContentResultT ContentSaveDownloaded(char * ContentP, uint32 ContentLen, const char * ContentTypeP, int32 RightXmlType, StringT * SuggestedFileNameP, const char * BoundaryP, char CID[], bool SaveAsWallpaper);
  bool ContentExist(const char * CID);
  bool ContentOpen(const char* CID, ContentActionT OpenType, BalFsiHandleT * FileHandleP = NULL);
  bool ContentClose(BalFsiHandleT FileHandle = NULL);
  bool ContentOpenGetFileName(const char* CID, ContentActionT OpenType, char * FilenameP);
  bool ContentDeleteFileOnly(const char * CID);
  bool ContentDeleteDir(uint32 Directory);
  bool ContentDelete(const char * CID);
  bool ContentDeleteAll(MediaCategoryT MediaCategory, bool WithRight = TRUE);
  ContentResultT ContentRename(const char * CID, const SidbStringT *NewNameP);
  ContentResultT ContentCreateNewFolder(const StringT* DirNameP);
  bool ContentCopyFile(uint8 *FileData, uint32 FileDataLen, const char *NewFileNameP, bool IsContent);
  bool ContentQueryRight(const char* CID, ContentActionT ActionType, ContentRightT* ContentRightP);
  bool ContentQueryRightUrl(const char* CID, char* URLP);
  bool ContentMemoryCapacity(uint32 *TotalSpaceP, uint32 *FreeSpaceP, uint32 * UsedP, MemoryTypeT MemoryType);
  MediaFileInfoT* ContentFindByName(const char *FileNameP, MediaCategoryT MediaCategory);
  bool ContentAddFileToCatagory(const char *FileNameP, MediaCategoryT MediaCategory);
  bool ContentDeleteFileFromCatagory(const char *FileNameP, MediaCategoryT MediaCategory);
  bool ContentGetActualFileName(const char * CID, char FileFullNameP[]);
  MediaFileInfoT* GetMediaInfoById(uint32 id, MediaCategoryT MediaCategory);
#if 0
  MusicFileInfoT* GetMusicFileInfo(uint32 id);
  PhotoFileInfoT* GetPhotoFileInfo(uint32 id);
  MediaFileInfoT* GetUnsupportInfo(uint32 id);
#endif
  void GetSubPhotoFiles(uint32 Directory);
  void SortVector(MediaCategoryT MediaCategory);
  
  inline uint32 GetNumOfSubDir(void) const;
  inline char* GetDirPathString(MediaCategoryT MediaCategory, uint32 directory) const;
  inline int GetNumOfMediaInfo(MediaCategoryT MediaCategory) const;
  inline MediaFileInfoT* GetMediaInfoByIndex(MediaCategoryT MediaCategory, int index);
  inline int GetNumOfSubMediaInfo(void) const;
  inline MediaFileInfoT* GetSubMediaInfoByIndex(int index);
  inline int GetIndexByMediaInfo(MediaCategoryT MediaCategory, MediaFileInfoT* MediaFileInfo);
    
  inline SortCriterionT GetSortCriterion(void) const;
  void SetSortCriterion(void);
  inline SortOrderT GetSortOrder(void) const;
  void SetSortOrder(void);
  
private:
  ContentmanagerC(void);

  void ContentMediaFilePathInit(void);
  void InitDirectoryPath(void);
  void ContentEnd(void);
  
  void GetAllMediaFiles(void);
  void GetAllMusicFiles(void);
  void GetAllPhotoFiles(void);
  void GetAllUnsupportFiles(void);
  void GetAllVoicememoFiles(void);
  
  int8 IsPhotoSpecifiedDir(const char *DirNameP);
  void GetOneMediaFileInDir(const BalFsiFileInfoT *FileInfoP, uint32 directory, MediaCategoryT MediaCategory);
  #ifdef FEATURE_BREW
  void GetMediaFileInBrew(void);
  #endif
  #ifdef SYS_OPTION_MMC 
  void GetMediaFileInSD(MediaCategoryT MediaCategory);
  #endif

  ContentTypeT PathToContentType(MediaPathT Directory);
  MediaPathT ContentTypeToPath(ContentTypeT ContentType);
  
  bool IsMediaIdExist(uint32 Id, MediaCategoryT MediaCategory);
  void CreateSingleId(MediaFileInfoT **MediaFileInfo, MediaCategoryT MediaCategory);
  MediaFileInfoT* GetMediaFileInfo(MediaCategoryT MediaCategory, uint32 Directory, const char * FilenameP);
  char* GetPredifnedCID(void);
  MediaFileInfoT* ContentFind(BVectorC* VectorFileInfoP, const char * CID, MediaCategoryT* MediaCategoryP = NULL);
  MediaFileInfoT* ContentFindMediaFileInfo(BVectorC* VectorFileInfo, const char * CID);
  uint32 GetDirIndex(const char * Path, MediaCategoryT MediaCategory);
  bool UserNameExist(MediaCategoryT MediaCategory, const SidbStringT *UserNameP);
  bool IsDirNameExist(char *DirNameP, MediaCategoryT MediaCategory);
  SidbStringT* GetUserName(MediaCategoryT MediaCategory, SidbStringT *UserNameP);
  void GetMediaCategory(char *FileTypeP, char *FileExtensionP, MediaCategoryT *MediaCategoryP);
  ContentResultT GetRightResultToContentResult(uint8 Result);
  bool GetUniqueFileName(MediaCategoryT MediaCategory, MediaPathT MediaPath, const char * FileExtP, char * FileNameP);
  bool ContentSave(const char * Content, const uint32 ContentLen, MediaCategoryT MediaCategory, 
			       ContentTypeT ContentType, const char * FileName);
  bool ContentAddToVector(uint32 Id, MediaTypeT Type , uint32 Directory, BalFsiFileInfoT *FileInfoP, 
                          uint32 ContentType, char *CID, SidbStringT *UserFriendlyNameP, MediaCategoryT MediaCategory);
  int32 SortCompare(MediaFileInfoT *Info1P, MediaFileInfoT *Info2P, SortCriterionT SortCriterion);
  void InsertToVector(MediaCategoryT MediaCategory, MediaFileInfoT *MediaFileInfoP);

  bool ContentReadDatabase(void);
  bool ContentSaveDatabase(void);
  
  static ContentmanagerC *_instance;
  static uint32 MediaFileUsed;

  BVectorC mVectorSubFileInfo;
  uint32 mNumberOfSubDir;
  BVectorC mVectorFileInfoA[MEDIA_CATEGORY_MAX];
  char* mMediaFilePathA[MEDIA_CATEGORY_MAX][MEDIA_PATH_MAX];

  SortCriterionT mSortCriterion;
  SortOrderT mSortOrder;
};

uint32 ContentmanagerC::GetNumOfSubDir(void) const
{
  return mNumberOfSubDir;
}

char* ContentmanagerC::GetDirPathString(MediaCategoryT MediaCategory, uint32 directory) const
{
  return mMediaFilePathA[MediaCategory][directory];
}

int ContentmanagerC::GetNumOfMediaInfo(MediaCategoryT MediaCategory) const
{
  return mVectorFileInfoA[MediaCategory].GetSize();
}

MediaFileInfoT* ContentmanagerC::GetMediaInfoByIndex(MediaCategoryT MediaCategory, int index)
{
  return (MediaFileInfoT *)mVectorFileInfoA[MediaCategory].GetElement(index);
}

int ContentmanagerC::GetNumOfSubMediaInfo(void) const
{
  return mVectorSubFileInfo.GetSize();
}

MediaFileInfoT* ContentmanagerC::GetSubMediaInfoByIndex(int index)
{
  return (MediaFileInfoT *)mVectorSubFileInfo.GetElement(index);
}

int ContentmanagerC::GetIndexByMediaInfo(MediaCategoryT MediaCategory, MediaFileInfoT* MediaFileInfo)
{
  return mVectorFileInfoA[MediaCategory].Find((void *)MediaFileInfo);
}

SortCriterionT ContentmanagerC::GetSortCriterion(void) const
{
  return mSortCriterion;
}

SortOrderT ContentmanagerC::GetSortOrder(void) const
{
  return mSortOrder;
}

inline ContentmanagerC* UiGetContentManager(void)
{
  return ContentmanagerC::GetInstance();
}

extern "C" int strcmplower(const char* str1, const char* str2);
extern "C" char* ToLower(char *str);
extern "C" bool SplitFileName(const char *FileName, char Path[], char **NameP);
extern "C" bool CheckBit(uint8 ToBeChecked, uint8 BitPos);

MediaTypeT GetMediaTypeByName(const char *name, MediaCategoryT MediaCategory);
SidbStringT CreateSidbStringFromString(const StringT *tempString);



#endif


