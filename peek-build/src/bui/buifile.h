#ifndef UIFILE_H_
#define UIFILE_H_

#include "balfsiapi.h"
#include "balfsicfg.h"

#include "buisystemfiledef.h"
#include "buivector.h"

#define FILE_READ_WRITE_BLOCK 0x2000

#define UI_NOT_SYS_FILE 0xff
//! Content operation result type
typedef enum
{
  FILE_SUCCESS         = FSI_SUCCESS, /*No errors. Function completed successfully.*/
  FILE_ERR_PARAMETER   = FSI_ERR_PARAMETER, /*Incorrect parameter to the function.*/
  FILE_ERR_READ        = FSI_ERR_READ, /*file read operation is failed.*/
  FILE_ERR_WRITE       = FSI_ERR_WRITE, /*file write operation is failed.*/
  FILE_ERR_SYSTEM      = FSI_ERR_SYSTEM, /*Indicates that a system error has occurred.*/
  FILE_ERR_EXIST       = FSI_ERR_EXIST, /*The specified object has existed already.*/
  FILE_ERR_NOTEXIST    = FSI_ERR_NOTEXIST, /*No matched object in specified media.*/
  FILE_ERR_EOF         = FSI_ERR_EOF, /*file pointer reaches the end-of-file.*/
  FILE_ERR_FULL        = FSI_ERR_FULL, /*Flash device is full*/
  FILE_ERR_NOSUPPORT   = FSI_ERR_NOSUPPORT, /*FSI does not support this function now .*/
  FILE_ERR_FORMAT      = FSI_ERR_FORMAT, /*Volume is in the incorrect format.*/
  FILE_ERR_ACCESS_DENY = FSI_ERR_ACCESS_DENY, /*Insufficient permissions to access object.*/
  /* reach to a limitation of the maximum number of the files that can be open
  simultaneously.*/
  FILE_ERR_MAX_OPEN    = FSI_ERR_MAX_OPEN, 

  FILE_ERR_SYSTEM_FILE = 50,/*error code defined in filemanager*/
  FILE_ERR_SAME_NAME,
  FILE_ERR_LONG_NAME,
  FILE_ERR_TYPE,
  
  FILE_NO_RIGHT        = 100, /*Invalid rights permission, thus */
  
  FILE_ERR_UNKNOWN     = 255 /*Other unknowned error occar*/
} UiFileResultT;

//! Content Action type
typedef enum
{
  FILE_IMAGE = 0,
  FILE_MUSIC,
  FILE_TONE,
  FILE_UNSUPPORT,
  FILE_SUPPORT_MAX
}UiFileTypeT;

/*****************************************************************************
common function
*****************************************************************************/
int StrCmp(const char* str1, const char* str2);
void SplitName(const char *FullName, char *Name, bool GetPre, char SplitChar = FILE_PATH_SEPARATOR_CHAR);
bool StrMatch(const char* substr, const char* mainstr);
/*****************************************************************************
BFileC define
*****************************************************************************/
class BFileC
{
public:
  BFileC();
  BFileC(const char *PathName);
  virtual ~BFileC();

  UiFileResultT Open(const char *PathName, BalFsiFileOpenModeT Mode);
  UiFileResultT Close();
  UiFileResultT Read(void *BufferP, uint32 ItemSize, uint32 *ItemNumP);
  UiFileResultT Write(void *BufferP, uint32 ItemSize, uint32 *ItemNumP);
  UiFileResultT Seek(BalFsiFileSeekTypeT SeekFrom, int32 MoveDistance);
  UiFileResultT SetAttrib(uint16 Attrib);
  
  uint32 Tell();
  uint32 GetLength();
  BalFsiFileAttribT GetAttrib();

  void GetFileName(char *FileName);
  void GetDirName(char *DirName);

  static uint8 IsSysFile(const char *PathName);
  
private:
  BalFsiHandleT mFile;
  uint8 mSysFileIndex;
  char *mFileName;
};

/*settingtable */
typedef enum
{
  SET_AS_WALLPAPER = 0,
  SET_AS_SCREEN_SAVER,
  SET_AS_CONTACT_IMAGE,
  SET_AS_GROUP_IMAGE,

  SET_AS_RING_TONE,
  SET_AS_MSM_ALERT,
  SET_AS_ALARM_TONE,
  SET_AS_CALENDAR_TONE,
  SET_AS_CONTACT_TONE,

  SET_AS_MAX
}SettingType;

typedef struct
{
  uint16 ID;
  char *FileName;
  SettingType Type;
  uint16 NextID;
}UiSettingInofT;

typedef struct
{
  uint16 ID;
  char FileName[MAX_FILE_NAME_LENGTH];
  SettingType Type;
  uint16 NextID;
}UiSettingInofDBT;
/*******************************************************************************************************/
//! SettingTable class define
/*******************************************************************************************************/
class BSettingTableC
{
public:
  BSettingTableC();
  virtual ~BSettingTableC();

  uint16 UpdataItem(const char *PathName, uint16 Id);
  uint16 InsertItem(const char *PathName, SettingType Type);
  uint16 DeleteItem(uint16 Id);
  const char *GetPathName(uint16 Id);
  SettingType CheckFileIsSetAs(const char *PathName);
    
  bool LoadFromDB();
private:
  uint16 CreateId(const char *PathName);
  UiSettingInofT *FindItem(uint16 Id);
  uint32 mFirstRecId;
  BVectorC *mSettingTable;
};

/*dir info*/
struct UiDirInfoT
{
  char *DirName;
  uint16 FileNum[FILE_SUPPORT_MAX];
  uint32 Size;
  UiDirInfoT *Root;
};
/*******************************************************************************************************/
//! DirTree class define
/*******************************************************************************************************/
class BDirTreeC
{
public:
  BDirTreeC();
  virtual ~BDirTreeC();

  bool InitTree();
  
  void DeleteDirInTree(const char *DirName);
  void AddDirInTree(const char *DirName, uint16 *FileNum, uint32 Size);
  void AddDirInTree(UiDirInfoT *Dir);
  void UpdataDirInTree(const char *OldDirName, const char *NewDirName);
  void MoveDirInTree(const char *SrcDirName, const char *DesDirName);
  void DeleteFileInTree(const char *PathName, UiFileTypeT FileType, uint32 FileSize);
  void AddFileInTree(const char *PathName, UiFileTypeT FileType, uint32 FileSize);

  uint32 GetDirSize(const char *DirName);
  uint16 GetDirFileNum(const char *DirName, UiFileTypeT FileType);
private:
  UiDirInfoT *FindDirInTree(const char *DirName);
  UiDirInfoT *FindUpDirInTree(const char *DirName);
  
  BVectorC *mDirInfoTree;
};

#endif
