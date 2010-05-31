// MarkupSTL.h: interface for the CMarkup class.
//
// Markup Release 8.3
// Copyright (C) 1999-2006 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.


#ifndef TXTBL_PROTOTYPE_MARKUP_H
#define  TXTBL_PROTOTYPE_MARKUP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
#define _DS(i) (i?&(m_strDoc.c_str())[m_aPos[i].nStart]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif

#if _MSC_VER >= 1200
#define strnicmp  _strnicmp
#endif
#define INIT_SIZE 20
enum MarkupDocFlags
	{
		MDF_IGNORECASE = 8
	};
	enum MarkupNodeFlags
	{
		MNF_WITHCDATA      = 0x01,
		MNF_WITHNOLINES    = 0x02,
		MNF_WITHXHTMLSPACE = 0x04,
		MNF_WITHREFS       = 0x08,
		MNF_WITHNOEND      = 0x10,
		MNF_ESCAPEQUOTES  = 0x100,
		MNF_NONENDED   = 0x100000,
		MNF_ILLDATA    = 0x200000
	};
	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
		MNT_LONE_END_TAG			= 128,// 0x80
		MNT_NODE_ERROR              = 32768 // 0x8000
	};

enum MarkupNodeFlagsInternal
	{
		MNF_REPLACE    = 0x001000,
		MNF_INSERT     = 0x002000,
		MNF_CHILD      = 0x004000,
		MNF_QUOTED     = 0x008000,
		MNF_EMPTY      = 0x010000,
		MNF_DELETED    = 0x020000,
		MNF_FIRST      = 0x080000,
		MNF_PUBLIC     = 0x300000,
		MNF_ILLFORMED  = 0x800000,
		MNF_USER      = 0xf000000
	};
class stringC 
{
	public:
		stringC() {m_szStr = NULL; m_nLen = 0;};
		stringC(const char*pStr)
		{ 
			if (pStr== NULL)
				return;
			m_nLen = strlen(pStr) + INIT_SIZE; 
			m_szStr = new char [m_nLen];
			if (m_szStr!=NULL)
			{
				memset(m_szStr,0x00,m_nLen);
				strcpy(m_szStr,pStr);
			}
		};
		stringC(const char*pStr,int len) 
		{ 
			m_nLen = len + INIT_SIZE; 
			m_szStr = new char [m_nLen];
			if (m_szStr!=NULL)
			{
				memset(m_szStr,0x00,m_nLen);
				strncpy(m_szStr,pStr,len);
			}
		};
		char *c_str() const
		{
			if (m_szStr)
				return m_szStr;
			else
				return NULL;
		};
		int size()
		{
			if(m_szStr)
				return strlen(m_szStr);
			else
				return 0;
		};
		int empty()
		{ 
			if (m_szStr != NULL) 
			{
				delete m_szStr; 
				m_szStr = NULL;
				m_nLen = 0;
			}
			return 1;
		};
		int erase()
		{
			memset(m_szStr,0x00,m_nLen);
			return 1;
		};
		 
		char* substr(int n, int l) const
		{
			char *p = new char[l + 10];
			if (p!=NULL)
			{
				memset(p,0x00,l+10);
				strncpy(p,&m_szStr[n],l);
			}
			return p;
		};
		~stringC()
		{ 
			if (m_szStr != NULL   )
			{
				delete m_szStr;
				m_szStr = NULL;
				m_nLen = 0;
			}
		}
		//int assign(int n, int l){ };
		int capacity(){return 1;};
		int replace(int i, int r,int s){return 1;};
		int findadd(char ch, char addch)
		{			
			int n = 0 ,j= 0;
			for(j = 0; j <= strlen(m_szStr); j++)
			{
				if(m_szStr[j] == ch)
					n++;
			}
			char *m_szStrTemp = new char[m_nLen];
			if (m_szStrTemp!=NULL)
					strcpy(m_szStrTemp,m_szStr);			
			m_nLen = strlen(m_szStr) + n + INIT_SIZE;
			delete m_szStr;
			m_szStr = new char [m_nLen];
			if (m_szStr!=NULL)
				memset(m_szStr,0x00,m_nLen); 
			n = 0;
			for(j = 0; j <= strlen(m_szStrTemp); j++)
			{
				m_szStr[n] = m_szStrTemp[j];
				if(m_szStrTemp[j] == ch)
					m_szStr[++n]=addch;
				n++;
			}
			return 0;
		};
		int reserve(int nLen)
		{
			if (m_szStr != NULL)
			{
				delete m_szStr; 
			}		
			m_nLen = nLen + INIT_SIZE;	
			m_szStr = new char[m_nLen];
			if (m_szStr!=NULL)
				memset(m_szStr,0x00,m_nLen); 
			return 1;
		};
		int append(const char*pStr,int nLen)
		{ 
			if (m_szStr == NULL)
			{
				m_nLen = nLen + INIT_SIZE;
				m_szStr = new char[m_nLen];
				if (m_szStr!=NULL)
				{
					memset(m_szStr,0x00,m_nLen);
					strncpy(m_szStr,pStr,nLen);
				}
			}
			else
			{
				if ((int)(strlen(m_szStr) + nLen) >= m_nLen)
				{
					char *m_szStrTemp = new char[m_nLen];
					if (m_szStrTemp!=NULL)
					{
						m_nLen = strlen(m_szStr) + nLen + INIT_SIZE;
						strcpy(m_szStrTemp,m_szStr);
						delete m_szStr;
						m_szStr = new char[m_nLen];
						if (m_szStr!=NULL)
						{ 
							memset(m_szStr,0x00,m_nLen);
							strcpy(m_szStr,m_szStrTemp);
							strncat(m_szStr,pStr,nLen);
						}
						delete m_szStrTemp;		
					}		
				}
				else
				{
					strncat(m_szStr,pStr,nLen);
				}
			}
			return m_nLen;
		};
		int assign(const char*pStr,int nLen)
		{
			if(m_szStr != NULL  )
				delete m_szStr;
			m_nLen = nLen + INIT_SIZE; 
			m_szStr = new char [m_nLen];
			if (m_szStr!= NULL)
			{
				memset(m_szStr,0x00,m_nLen);
				strncpy(m_szStr,pStr,nLen);
			}
			return 1;
		};
		int append(const char*pStr)
		{
			int nLen = strlen(pStr);
			return append(pStr,nLen);
		};
		int append(stringC StrC){ return append(StrC.c_str()) ;};
		int appendChar(char nChar)
		{
			char szTemp[2]={nChar,0x00};
			if ((int)(strlen(m_szStr) + 1) >= m_nLen)
			{
				char * m_szStrTemp = new char[m_nLen];
				if (m_szStrTemp!=NULL)
				{
					m_nLen = strlen(m_szStr) + 1 + INIT_SIZE;
					strcpy(m_szStrTemp,m_szStr);
					delete m_szStr;
					m_szStr = new char[m_nLen];
					if (m_szStr!=NULL)
					{
						strcpy(m_szStr,m_szStrTemp);
						strcat(m_szStr,szTemp);
					}
					delete m_szStrTemp;				
				}
			}
			else
			{
				strcat(m_szStr,szTemp);
			}
			return m_nLen;
		};
		int copy(const char*pStr)
		{
			if(m_szStr != NULL && m_nLen )
			{
				delete m_szStr;
				m_nLen = 0;
				m_szStr = NULL;
			}
			if (pStr == NULL)
			{
				m_nLen = 0;
				m_szStr = NULL;
			}
			else
			{
				m_nLen = strlen(pStr) + INIT_SIZE;
				m_szStr = new char [m_nLen];
				if(m_szStr!=NULL)
				{
					memset(m_szStr,0x00,m_nLen);
					strcpy(m_szStr,pStr);
				}
			}
			return 1;
		};

		int copy(const char*pStr, int nLen)
		{
			if(m_szStr != NULL && m_nLen )
			{
				delete m_szStr;
				m_szStr = NULL;
				m_nLen = 0;
			}
			if (pStr == NULL)
			{
				m_nLen = 0;
				m_szStr = NULL;
			}
			else
			{
				m_nLen = nLen + INIT_SIZE;
				m_szStr = new char [m_nLen];
				if (m_szStr!=NULL)
				{
					memset(m_szStr,0x00,m_nLen);
					strcpy(m_szStr,pStr);
				}
			}
			return 1;
		};

		int copy(stringC strC)
		{			
			if (strC.size() == 0)
			{
				m_nLen = 0;
				m_szStr = NULL;
			}
			else
			{
				if(m_szStr != NULL )
				{
					delete m_szStr;
					m_szStr = NULL;
					m_nLen = 0;
				}
				m_nLen = strlen(strC.c_str()) + INIT_SIZE;
				m_szStr = new char [m_nLen];
				if(m_szStr!=NULL)
				{
					memset(m_szStr,0x00,m_nLen);
					strcpy(m_szStr,strC.c_str());
				}
			}
			return 1;
		};
	
		bool operator== ( const stringC& str ){return true;};
		char operator[] ( int ){return 1;};
		void operator+ ( const stringC& str ){return;};
	protected:
		char *m_szStr;
		int m_nLen;
};

	struct NodePos
	{
		NodePos() {};
		NodePos( int n ) { nFlags=n; nNodeType=0; nStart=0; nLength=0; };
		
		int nNodeType;
		int nStart;
		int nLength;
		int nFlags;
		stringC strMeta;
	};
struct ElemPos
	{
		ElemPos() {};
		ElemPos( const ElemPos& pos ) { *this = pos; };
		enum { EP_STBITS=22, EP_STMASK=0x2fffff, EP_LEVMASK=0xffff };
		int StartTagLen() const { return (nTagLengths & EP_STMASK); };
		void SetStartTagLen( int n ) { nTagLengths = (nTagLengths & ~EP_STMASK) + n; };
		void AdjustStartTagLen( int n ) { nTagLengths += n; };
		int EndTagLen() const { return (nTagLengths >> EP_STBITS); };
		void SetEndTagLen( int n ) { nTagLengths = (nTagLengths & EP_STMASK) + (n << EP_STBITS); };
		bool IsEmptyElement() { return (StartTagLen()==nLength)?true:false; };
		int StartContent() const { return nStart + StartTagLen(); };
		int ContentLen() const { return nLength - StartTagLen() - EndTagLen(); };
		int StartAfter() const { return nStart + nLength; };
		int Level() const { return nFlags & EP_LEVMASK; };
		void SetLevel( int nLev ) { nFlags = (nFlags & ~EP_LEVMASK) | nLev; };
		void ClearVirtualParent() { memset(this,0,sizeof(ElemPos)); };

		// Memory size: 8 32-bit integers == 32 bytes
		int nStart;
		int nLength;
		int nTagLengths; // 22 bits 4MB limit for start tag, 10 bits 1K limit for end tag
		int nFlags; // 16 bits flags, 16 bits level 65536 depth limit
		int iElemParent;
		int iElemChild; // first child
		int iElemNext;
		int iElemPrev; // if this is first child, iElemPrev points to last
	};
	struct PosArray
	{
		PosArray() { Clear(); };
		~PosArray() { Release(); };
		enum { PA_SEGBITS = 16, PA_SEGMASK = 0xffff };
		void RemoveAll() { Release(); Clear(); };
		void Release() { for (int n=0;n<SegsUsed();++n) delete[] (char*)pSegs[n]; if (pSegs) delete[] (char*)pSegs; };
		void Clear() { nSegs=0; nSize=0; pSegs=NULL; };
		int GetSize() const { return nSize; };
		int SegsUsed() const { return ((nSize-1)>>PA_SEGBITS) + 1; };
		ElemPos& operator[](int n) const { return pSegs[n>>PA_SEGBITS][n&PA_SEGMASK]; };
		ElemPos** pSegs;
		int nSize;
		int nSegs;
	};

	struct SavedPos
	{
		SavedPos() { nSavedPosFlags=0; iPos=0; };
		stringC strName;
		int iPos;
		int nSavedPosFlags;
	};

	struct SavedPosMap
	{
		SavedPosMap() { pTable = NULL; };
		~SavedPosMap() { RemoveAll(); };
		void RemoveAll() { if (pTable) Release(); pTable=NULL; };
		enum { SPM_SIZE = 7, SPM_MAIN = 1, SPM_CHILD = 2, SPM_USED = 4, SPM_LAST = 8 };
		void Release() { for (int n=0;n<SPM_SIZE;++n) if (pTable[n]) delete[] pTable[n]; delete[] pTable; };
		void AllocMapTable() { pTable = new SavedPos*[SPM_SIZE]; if (pTable==NULL) return; for (int n=0; n<SPM_SIZE; ++n) pTable[n]=NULL; };
		int Hash( const char* szName ) { unsigned int n=0; while (*szName) n += (unsigned int)(*szName++); return n % SPM_SIZE; };
		SavedPos** pTable;
	};


struct TokenPos
	{
		TokenPos( const char* sz, int n ) { Clear(); szDoc=sz; nTokenFlags=n; };
//		TokenPos( const stringC& str, int n ) { Clear(); szDoc=str.c_str(); nTokenFlags=n; };
		void Clear() { nL=0; nR=-1; nNext=0; };
		int Length() const { return nR - nL + 1; };
//		bool Match( const stringC& strName ) { return Match(strName.c_str()); };
		bool Match( const char* szName )
		{
			int nLen = nR - nL + 1;
			if ( nTokenFlags & MDF_IGNORECASE )
//				return ( (strnicmp( &szDoc[nL], szName, nLen ) == 0)
					return ( (strncmp( &szDoc[nL], szName , nLen) == 0)
					&& ( szName[nLen] == '\0' || strchr(" =/[]",szName[nLen]) ) );
			else
				return ( (strncmp( &szDoc[nL], szName, nLen ) == 0)
					&& ( szName[nLen] == '\0' || strchr(" =/[]",szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		const char* szDoc;
		int nTokenFlags;
		int nPreSpaceStart;
		int nPreSpaceLength;
	};

class CMarkup  
{
public:
	CMarkup() { SetDoc( NULL ); InitDocFlags(); };
	CMarkup( const char* szDoc ) { SetDoc( szDoc ); InitDocFlags(); };
	CMarkup( int nFlags ) { SetDoc( NULL ); m_nFlags = nFlags; };
	CMarkup( const CMarkup& markup ) { *this = markup; };
	void operator=( const CMarkup& markup );
	~CMarkup() {m_strDoc.empty();m_strError.empty();};

	// Navigate
	bool LoadFromString(char *pStr);	
	bool SetDoc( const char* szDoc );
	bool IsWellFormed();
	bool FindElem( const char* szName=NULL );
	bool FindChildElem( const char* szName=NULL );
	bool IntoElem();
	bool OutOfElem();
	void ResetChildPos() { x_SetPos(m_iPosParent,m_iPos,0); };
	void ResetMainPos() { x_SetPos(m_iPosParent,0,0); };
	void ResetPos() { x_SetPos(0,0,0); };	
	char*  GetData() const { return x_GetData(m_iPos); };
	char* GetChildData() const { return x_GetData(m_iPosChild); };
	int FindNode( int nType=0 );
	int GetNodeType() { return m_nNodeType; };
	bool SavePos( const char* szPosName="" );
	bool RestorePos( const char* szPosName="" );
	const stringC& GetError() const { return m_strError; };
	int GetDocFlags() const { return m_nFlags; };
	
	// Create
	// Modify
	// Utility
	static stringC EscapeText( const char* szText, int nFlags = 0 );
	static char* UnescapeText( const char* szText, int nTextLength = -1 );
	
protected:

#ifdef _DEBUG
	const char* m_pMainDS;
	const char* m_pChildDS;
#endif

	stringC m_strDoc;
	stringC m_strError;

	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_iPosDeleted;
	int m_nNodeType;
	int m_nNodeOffset;
	int m_nNodeLength;
	int m_nFlags;

	

	
	SavedPosMap m_mapSavedPos;

	
	PosArray m_aPos;
	
	struct NodeStack
	{
		NodeStack() { nTop=-1; nSize=0; pN=NULL; };
		~NodeStack() { if (pN) delete [] pN; };
		NodePos& Top() { return pN[nTop]; };
		NodePos& At( int n ) { return pN[n]; };
		void Add() 
		{ 
			++nTop; 
			if (nTop==nSize) 
				Alloc(nSize*2+16); 
		};
		void Remove() { --nTop; };
		int TopIndex() { return nTop; };
	protected:
		void Alloc( int nNewSize ) 
		{ 
			NodePos* pNNew = new NodePos[nNewSize];
			if (pNNew!=NULL) 
			{
				Copy(pNNew); 
				nSize=nNewSize; 
			}
		};
		void Copy( NodePos* pNNew ) 
		{ 
			for(int n=0;n<nSize;++n) 
			{
				pNNew[n].nFlags=pN[n].nFlags; 
				pNNew[n].nNodeType=pN[n].nNodeType; 
				pNNew[n].nStart=pN[n].nStart;
				pNNew[n].nLength=pN[n].nLength;
				pNNew[n].strMeta.copy(pN[n].strMeta.c_str());
			}
			if (pN) 
			{
				delete pN; 
				pN = NULL;
			}
			pN=pNNew; 
		};
		NodePos* pN;
		int nSize;
		int nTop;
	};

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};
	int x_GetFreePos()
	{
		if ( m_iPosFree == m_aPos.GetSize() )
			x_AllocPosArray();
		return m_iPosFree++;
	};
	bool x_AllocPosArray( int nNewSize = 0 );

	void InitDocFlags()
	{
		// To always ignore case, define MARKUP_IGNORECASE
	#ifdef MARKUP_IGNORECASE
		m_nFlags = MDF_IGNORECASE;
	#else
		m_nFlags = 0;
	#endif
	};

	bool x_ParseDoc();
	int x_ParseElem( int iPos, TokenPos& token );
	static bool x_FindAny( const char* szDoc, int& nChar );
	static bool x_FindName( TokenPos& token );
	char* x_GetToken( const TokenPos& token );
	int x_FindElem( int iPosParent, int iPos, const char* szPath ) const;
	char* x_GetData( int iPos ) const;
	static bool x_FindAttrib( TokenPos& token, const char* szAttrib, int n=0 );
	static int x_ParseNode( TokenPos& token, NodePos& node );
	static stringC x_GetLastError();
};

#endif
