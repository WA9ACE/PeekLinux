
#ifndef UIFILEMANAGER_H_
#define UIFILEMANAGER_H_

#include "buifile.h"
#include "buimsguser.h"

#define COMBINE_MSG(Result, FileCount)  ((FileCount & 0xffffff) | ((Result & 0xff)<<24))
#define SPLIT_MSG_RESULT(Msg) ((Msg>>24) & 0xff)
#define SPLIT_MSG_FILE_COUNT(Msg) (Msg & 0xffffff)


//! storage device type
typedef enum
{
  UI_MEMORY_PHONE,
  UI_MEMORY_CARD
} UiMemoryTypeT;

typedef enum
{
  FILE_ATTRIB_FILE  = 0x01,
  FILE_ATTRIB_SYS   = 0x02,
  FILE_ATTRIB_DIR   = 0x04,
  FILE_ATTRIB_BOOKMARK = 0x08,
  FILE_ATTRIB_COMP_BIT_MARK = 0x0f,
  FILE_ATTRIB_MARK  = 0x10
}UiFileAttrT;/*file attribution*/

typedef enum
{
  FILE_TYPE_SYS_FILE = 0,
  FILE_TYPE_SYS_DIR,
  FILE_TYPE_USER_FILE,
  FILE_TYPE_USER_DIR,
  FILE_TYPE_NOT_SUPPORT
}UiFileFolderTypeT;/*file attribution*/

//! Media type
typedef enum
{
  FILE_MUSIC_MEMO = 0,
  FILE_PHOTO_BMP,
  FILE_PHOTO_GIF,
  FILE_PHOTO_JPG,
  FILE_MUSIC_MIDI,
  FILE_MUSIC_MP3,
  FILE_PHOTO_PNG,
  FILE_OTHER
} UiFileFormatT;
//filter type
typedef enum
{
  FILE_FILTER_MEMO = (1<<FILE_MUSIC_MEMO),
  FILE_FILTER_BMP  = (1<<FILE_PHOTO_BMP),
  FILE_FILTER_GIF  = (1<<FILE_PHOTO_GIF),
  FILE_FILTER_JPG  = (1<<FILE_PHOTO_JPG),
  FILE_FILTER_MIDI = (1<<FILE_MUSIC_MIDI),
  FILE_FILTER_MP3  = (1<<FILE_MUSIC_MP3),
  FILE_FILTER_PNG  = (1<<FILE_PHOTO_PNG),
  FILE_FILTER_OTHER= (1<<FILE_OTHER),
  FILE_ALL         = 0xffff
} UiFileFilterT;

typedef enum
{
  FILE_FILTER_FILE = FILE_ATTRIB_FILE,
  FILE_FILTER_DIR = FILE_ATTRIB_DIR,
  FILE_FILTER_ALL = FILE_FILTER_FILE | FILE_FILTER_DIR
}UiFileOrFolderT;

typedef struct
{
  uint16 Width;
  uint16 Height;
}UiResolutionT;

/*file infomation class*/
typedef struct
{
  char *FileName;
  uint32 CreateTime;  /*Include date and time of day*/
  uint32 Size; /*file length*/
  uint16 Attr; /*file attribution, include mark flag*/
  UiFileFormatT Format;
}UiFileInfoT;
/*file detail*/
typedef struct
{
  char FileName[MAX_FILE_NAME_LENGTH];
  uint32 CreateTime;  /*Include date and time of day*/
  uint32 Size; /*file length*/
  UiFileFormatT Format;
  
  UiResolutionT Resolution;
  uint32 PlayTime;
}UiFileInfoDetailT;

//! Sort criterion type
typedef enum
{
  SORT_BY_FILE_NAME = 0,
  SORT_BY_FILE_DATE,
  SORT_BY_FILE_FORMAT,
  SORT_BY_FILE_SIZE,
  SORT_BY_FILE_MAX
} UiSortCriterionT;

//! Sort order type
typedef enum
{
  SORT_ASCEND = 0,
  SORT_DESCEND
} UiSortOrderT;

/*******************************************************************************************************/
//! FileManager class define
/*******************************************************************************************************/
class BFileManagerC: public MailClientC
{
public:
  virtual ~BFileManagerC(void);
  static BFileManagerC* GetInstance(void);
  static void DelInstance(void);
  //init
  bool Init(void);
  //get file list
  bool GetAllFilesInDir(const char *DirName, uint32 FileListEndEvent);
  BVectorC *GetFileListToShow(const char *NameFilter, uint32 Filter, UiFileOrFolderT FileOrFolder);
  //mark
  BVectorC *GetMarkFileList(bool Marked);
  bool MarkAllFile(bool Marked);
  inline uint16 GetMarkFileNum();
  bool SetMarkFile(int index, bool Marked);
  bool SetMarkFile(UiFileInfoT *FileItem, bool Marked);
  bool GetMarkFile(int index);
  bool GetMarkFile(UiFileInfoT *FileItem);
  //sort
  inline UiSortCriterionT GetSortCriterion(void) const;
  void SetSortCriterion(UiSortCriterionT SortCriterion);
  inline UiSortOrderT GetSortOrder(void) const;
  void SetSortOrder(UiSortOrderT SortOrder);

  UiFileFolderTypeT GetFileFolderType(const char *PathName);
  //dir
  UiFileResultT MakeDir(const char *DirName);
  bool DeleteDir(const char *DirName, uint32 DirDeleteEndEvent);
  bool MoveDir(const char *SrcDir, const char *DesDir, uint32 DirMoveEndEvent);
  UiFileResultT RenameDir(const char *OldDir, const char *NewDir);
  
  uint16 GetFileNumInDir(const char *DirName);
  uint32 GetSizeInDir(const char *DirName);
  //file
  bool AddFile(const char *PathName);
  UiFileResultT DeleteFile(const char *PathName);
  UiFileResultT MoveFile(const char *SrcPathName, const char *DesPathName);
  UiFileResultT RenameFile(const char *OldPathName, const char *NewPathName);

  UiFileFormatT GetFormatByFileName(const char *Name);
  UiFileTypeT GetFileType(UiFileFormatT Format);

  UiFileResultT OpenFile(const char * PathName, BalFsiFileOpenModeT Mode);
  UiFileResultT CloseFile();
  bool HugeRead(void * DataP, uint32 Length, uint32 FileReadEndEvent);
  bool HugeWrite(void * DataP, uint32 Length, uint32 FileWriteEndEvent);
  UiFileResultT Read(void * DataP, uint32 Length);
  UiFileResultT Write(void * DataP, uint32 Length);

  void SortList();
  void StopAllOperate();
  //setting
  uint16 SetFileAs(const char *PathName, uint16 OldId, SettingType Type);
  const char* GetPathNameFromId(uint16 Id);
  SettingType IsSetAsType(const char *PathName);
private:
  BFileManagerC(void);
  
  void OnListFiles(void);
  void OnListFilesOver(void);
  void OnReadFile(void);
  void OnReadFileOver(uint32 Result);
  void OnWriteFile(void);
  void OnWriteFileOver(uint32 Result);
  void OnDeleteDir(void);
  void OnDeleteDirOver(uint32 Result);
  void OnMoveDir(void);
  void OnMoveDirOver(uint32 Result);

  bool MakeSysDir(void);
  UiFileResultT CopyFile(const char *SrcPathName, const char *DesPathName);
  bool IsSysDir(const char *DirName);
  
  bool GetAllFileInfo(void);
  bool GetAllSubDir(const char *DirName);

  bool FindNextFile(const char *DirName, BalFsiFileInfoT *FileInfo);
  void ReleaseFileList(void);

  bool IsInCurrentDir(const char *PathName);
  void AddItemToCurrentDir(const char *PathName);
  void DeleteItemFromCurrentDir(const char *PathName);
  void UpdataItemInCurrentDir(const char *OldPathName, const char *NewPathName);

  int8 SortCompare(UiFileInfoT *FileInfo1P, UiFileInfoT *FileInfo2P, UiSortCriterionT SortCriterion);
  void Insert(BVectorC *FileList, UiFileInfoT *NewNode, UiSortCriterionT SortCriterion, UiSortOrderT SortOrder);
  void Sort(BVectorC *FileList, UiSortCriterionT SortCriterion, UiSortOrderT SortOrder);
  void InsertSysFolder(BVectorC *FileList, UiFileInfoT *Node, UiFileInfoT *NewNode);

  static BFileManagerC *_instance;

  BalFsiFindHandle mFileFindHandle; //find file handle

  char *mCurrentDirName;
  uint32 mEndEvent;
  void *mDataBuffer;
  uint32 mCurDataPos;
  uint32 mDataLength;
  uint32 mDirFilePos;
  char *mMoveToDir;

  bool mStopAllOperate;//true to stop all operation

  char *mNameFilter;
  uint32 mFormatFilter;
  UiFileOrFolderT mFolderFilter;

  BVectorC *mFileListInCurrentDir;
  BVectorC *mFileListForShow;
  BVectorC *mFileListForMark;
  uint16 mMarkFileNum;
  
  BVectorC *mSubDirList;//for delete dir, to save sub direcory
  uint16 mOperatedFileNum;

  BDirTreeC mDirInfoTree; //dir info tree
  BSettingTableC mSettingInfoTable; //setting info table

  BFileC mFile;
  
  UiSortCriterionT mSortCriterion;//for sort
  UiSortOrderT mSortOrder;

  DECLARE_MAIL_MAP()
};

UiSortCriterionT BFileManagerC::GetSortCriterion(void) const
{
  return mSortCriterion;
}

UiSortOrderT BFileManagerC::GetSortOrder(void) const
{
  return mSortOrder;
}
uint16 BFileManagerC::GetMarkFileNum()
{
  return mMarkFileNum;
}

inline BFileManagerC* UiGetFileManager(void)
{
  return BFileManagerC::GetInstance();
}


#endif
