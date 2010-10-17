// public-simulatorDlg.cpp : implementation file
//
#define Rectangle lguiRectangle
#include "lgui.h"
#undef Rectangle

#include "stdafx.h"
#include "public-simulator.h"
#include "public-simulatorDlg.h"

#include "lgui.h"
#include "Platform.h"
#include "ConnectionContext.h"
#include "ApplicationManager.h"
#include "RenderManager.h"
#include "DataObject.h"
#include "Mime.h"
#include "KeyMappings.h"
#include "Cache.h"
#include "Font.h"

#include "p_malloc.h"

#include <ctype.h>
#include <vector>
#include <stdarg.h>
#include <string.h>
#include <direct.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" {
unsigned char screenBuf[320*240*2];
unsigned char imgBuf[320*240*2];
unsigned char bitmapBuf[320*240*4];
}
#define GRAPH_CTRL_ID 60000
static CpublicsimulatorDlg *GlobalDialog = NULL;
extern "C" int p_getTotalAllocated(void);
extern "C" void emo_printf(const char *fmt, ...);

CCriticalSection guiCritical;

BOOL IsDots(const TCHAR* str) {
   if(_tcscmp(str,_T(".")) && _tcscmp(str,_T(".."))) return FALSE;
   return TRUE;
}

BOOL DeleteDirectory(const TCHAR* sPath) {
   HANDLE hFind;    // file handle
   WIN32_FIND_DATA FindFileData;

   TCHAR DirPath[MAX_PATH];
   TCHAR FileName[MAX_PATH];

   _tcscpy(DirPath,sPath);
   _tcscat(DirPath,_T("\\*"));    // searching all files
   _tcscpy(FileName,sPath);
   _tcscat(FileName,_T("\\"));

   // find the first file
   hFind = FindFirstFile(DirPath,&FindFileData);
   if(hFind == INVALID_HANDLE_VALUE) return FALSE;
   _tcscpy(DirPath,FileName);

   bool bSearch = true;
   while(bSearch) {    // until we find an entry
      if(FindNextFile(hFind,&FindFileData)) {
         if(IsDots(FindFileData.cFileName)) continue;
         _tcscat(FileName,FindFileData.cFileName);
         if((FindFileData.dwFileAttributes &
            FILE_ATTRIBUTE_DIRECTORY)) {

            // we have found a directory, recurse
            if(!DeleteDirectory(FileName)) {
                FindClose(hFind);
                return FALSE;    // directory couldn't be deleted
            }
            // remove the empty directory
            RemoveDirectory(FileName);
             _tcscpy(FileName,DirPath);
         }
         else {
			DeleteFile(FileName);
#if 0
			if(FindFileData.dwFileAttributes &
               FILE_ATTRIBUTE_READONLY)
               // change read-only file mode
                  _chmod(FileName, _S_IWRITE);
                  if(!DeleteFile(FileName)) {    // delete the file
                    FindClose(hFind);
                    return FALSE;
               }
#endif
               _tcscpy(FileName,DirPath);
         }
      }
      else {
         // no more files there
         if(GetLastError() == ERROR_NO_MORE_FILES)
         bSearch = false;
         else {
            // some error occurred; close the handle and return FALSE
               FindClose(hFind);
               return FALSE;
         }

      }

   }
   FindClose(hFind);                  // close the file handle

   return RemoveDirectory(sPath);     // remove the empty directory

}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CpublicsimulatorDlg dialog




CpublicsimulatorDlg::CpublicsimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CpublicsimulatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	GlobalDialog = this;
	m_isDebug = true;
	m_showInternalDebug = false;
	m_showScriptDebug = true;
}

void CpublicsimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_DISPLATOUT, m_display);
	DDX_Control(pDX, IDC_EDIT1, m_debugOuput);
}

BEGIN_MESSAGE_MAP(CpublicsimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_LOADAPPLICATION, &CpublicsimulatorDlg::OnFileLoadapplication)
	ON_COMMAND(ID_FILE_QUIT, &CpublicsimulatorDlg::OnFileQuit)
	ON_COMMAND(ID_HELP_ABOUT, &CpublicsimulatorDlg::OnHelpAbout)
	ON_COMMAND(ID_DEBUG_ENABLED, &CpublicsimulatorDlg::OnDebugEnabled)
//	ON_COMMAND(ID_DEBUG_INTERNALDEBUGENABLED, &CpublicsimulatorDlg::OnDebugInternaldebugenabled)
	ON_COMMAND(ID_DEBUG_SCRIPTDEBUGENABLED, &CpublicsimulatorDlg::OnDebugScriptdebugenabled)
//	ON_COMMAND(ID_DEBUG_INTERNALDEBUGENABLED, &CpublicsimulatorDlg::OnDebugInternaldebugenabled)
	ON_COMMAND(ID_DEBUG_INTERNAL, &CpublicsimulatorDlg::OnDebugShowinternalmessages)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_CLEARCACHEANDREBOOT, &CpublicsimulatorDlg::OnFileClearcacheandreboot)
END_MESSAGE_MAP()

extern "C" void appProtocolStatus(int n)
{

}

extern "C" void NetworkTask(void);
Thread *netThread;
extern "C" {
void net_thread(void *d)
{
	NetworkTask();
}
}

void DataGenerator(LPVOID param)
{
	CStatusGraphCtrl* pCtrl = (CStatusGraphCtrl*)param;
	while(true)
	{
		pCtrl->SetCurrentValue(p_getTotalAllocated());
		Sleep(50);
	}
}

// CpublicsimulatorDlg message handlers

//extern "C" {
//CRITICAL_SECTION emoCrit;
//}

BOOL CpublicsimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//InitializeCriticalSection(&emoCrit);

	FILE *input;
	input = fopen("clean_cache", "r");
	if (input != NULL) {
		fclose(input);
		_unlink("clean_cache");
		DeleteDirectory(_T("cache"));
	}

	HMENU hmenu = AfxGetMainWnd()->GetMenu()->GetSafeHmenu();
	m_menu.Attach(hmenu);

	LoadSettings();

	dataobject_platformInit();
	renderman_init();
	cache_init();
	netThread = thread_run(net_thread, NULL);

	manager_init();
	lgui_attach(screenBuf);

	m_graph.Create(_T("Memory Usage"),WS_CHILD|WS_VISIBLE,CRect(425,0,875,100),
		static_cast<CWnd*>(this),GRAPH_CTRL_ID);
	m_graph.SetMaxValue(2*1024*1024);
	m_graph.SetMinValue(0);
	m_graph.StartUpdate();
	m_graph.SetGraphMode(LINE_GRAPH);
	//AfxBeginThread((AFX_THREADPROC)DataGenerator,(LPVOID)&m_graph);

	SetTimer(1, 200, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CpublicsimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CpublicsimulatorDlg::OnPaint()
{
	
if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		guiCritical.Lock();
		CDialog::OnPaint();
		guiCritical.Unlock();	
		DrawScreen();
	}
	
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CpublicsimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CpublicsimulatorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

extern long int cury;
extern void DrawPoint(CStatusGraphCtrl* TheCtrl,long int cury,bool Update);
BOOL CpublicsimulatorDlg::PreTranslateMessage(MSG* pMsg)
{
	/*m_graph.SetCurrentValue(p_getTotalAllocated());*/
	bool used = false;

	if (pMsg->message == WM_KEYDOWN)
		if (pMsg->wParam == VK_ESCAPE) {
			//EnterCriticalSection(&emoCrit);
			used = true;
			manager_debugPrint();
			//LeaveCriticalSection(&emoCrit);
			return TRUE;
		}

	POINT pt = pMsg->pt;
    ScreenToClient(&pt);

	/* This needs to be better eventually */
	if (pt.x > 423 || pt.y < 80) {
		return CDialog::PreTranslateMessage(pMsg);
	}

	if (pMsg->message == WM_LBUTTONDOWN) {
		//EnterCriticalSection(&emoCrit);
		used = true;
		manager_handleKey(13);
		//LeaveCriticalSection(&emoCrit);
	}
	if (pMsg->message == WM_RBUTTONDOWN) {
		//EnterCriticalSection(&emoCrit);
		used = true;
		manager_handleKey(12);
		//LeaveCriticalSection(&emoCrit);
	}
	if (pMsg->message == WM_KEYDOWN) {
		int key = pMsg->wParam;
		switch (key) {
			case VK_OEM_MINUS: key = '.'; break;
			case VK_OEM_PLUS: key = '='; break;
			case VK_OEM_COMMA: key = ','; break;
			case VK_OEM_PERIOD: key = '.'; break;
			case VK_RETURN: key = 13; break;
			case 106: key = 42; break;
			case 40:
				//EnterCriticalSection(&emoCrit);
				used = true;
				manager_handleKey(EKEY_FOCUSNEXT);
				DrawScreen();
				//LeaveCriticalSection(&emoCrit);
				return CDialog::PreTranslateMessage(pMsg);
			case 38:
				//EnterCriticalSection(&emoCrit);
				used = true;
				manager_handleKey(EKEY_FOCUSPREV);
				DrawScreen();
				//LeaveCriticalSection(&emoCrit);
				return CDialog::PreTranslateMessage(pMsg);
			default:
				break;
		}

		short isShift = GetKeyState(VK_SHIFT) & 0xFF00;
		if (isShift)
			key = toupper(key);
		else
			key = tolower(key);

		if (isShift) {
			switch (key) {
				case '0': key = ')'; break;
				case '1': key = '!'; break;
				case '2': key = '@'; break;
				case '3': key = '#'; break;
				case '4': key = '$'; break;
				case '5': key = '%'; break;
				case '6': key = '^'; break;
				case '7': key = '&'; break;
				case '8': key = '*'; break;
				case '9': key = '('; break;
				case '=': key = '+'; break;
				case '-': key = '_'; break;
				default: break;
			}
		}

		if (key != -1 && (isprint(key) || key == '\b' || key == 13)) {
			//EnterCriticalSection(&emoCrit);		
			used = true;
			manager_handleKey(key);
			//LeaveCriticalSection(&emoCrit);
		}
	} else
	if (pMsg->message == WM_MOUSEWHEEL) {
		//EnterCriticalSection(&emoCrit);
		used = true;
		if (GET_WHEEL_DELTA_WPARAM(pMsg->wParam) > 0)
			manager_handleKey(EKEY_FOCUSPREV);
		else
			manager_handleKey(EKEY_FOCUSNEXT);
		//LeaveCriticalSection(&emoCrit);
	}

	if (used || 1) {
		//EnterCriticalSection(&emoCrit);
		DrawScreen();
		//LeaveCriticalSection(&emoCrit);
	}
	//if (pMsg->message == WM_KEYDOWN)
	//	if (pMsg->wParam == VK_RETURN)
			return TRUE;

	//return CDialog::PreTranslateMessage(pMsg);
}

void CpublicsimulatorDlg::DrawScreen(void)
{
	guiCritical.Lock();
	renderman_flush();
	manager_drawScreen();

	if (!lgui_is_dirty())
		goto show_prev_screen;

	//fprintf(stderr, "in blit\n");

	// we really use glBuffer to make sure drawing works properly
	int index, upper;
	upper = lgui_index_count();
	if (upper == 0) {
		memcpy(imgBuf, screenBuf, 320*240*2);
	} else {
		lguiRectangle *rect;
		for (index = 0; index < upper; ++index) {
			rect = lgui_get_region(index);

			for (int ypos = 0; ypos < rect->height; ++ypos) {
				memcpy(imgBuf + rect->x*2 + (rect->y+ypos)*320*2,
						screenBuf + rect->x*2 + (rect->y+ypos)*320*2,
						rect->width*2);
			}
		}
	}
show_prev_screen:

	unsigned char red, green, blue;
	for (int i = 0; i < 320*240; ++i) {
		red = (((unsigned short *)(imgBuf))[i] & 0xF800) >> 8;
		green = (((unsigned short *)(imgBuf))[i] & 0x7E0) >> 3;
		blue = (((unsigned short *)(imgBuf))[i] & 0x1F) << 3;
		bitmapBuf[i*4+2] = red;
		bitmapBuf[i*4+1] = green;
		bitmapBuf[i*4] = blue;
	}

	lgui_blit_done();

	CDC *cdc = GetDC();
	CImage screenBitmap;
	screenBitmap.Create(320, -240, 32);
	void *bits = screenBitmap.GetBits();
	memcpy(bits, bitmapBuf, 320*240*4);
	screenBitmap.Draw(cdc->m_hDC, 53, 78, 320, 240);

/*	CDC memDC;
	memDC.CreateCompatibleDC(cdc);
	memDC.SelectObject((CBitmap *)&screenBitmap);
	cdc->BitBlt(10, 10, 320, 240, &memDC, 0, 0, SRCCOPY);
*/
	ReleaseDC(cdc);

	guiCritical.Unlock();

	/*if (cury != 0) {
		DrawPoint(&m_graph,cury, false);
		cury = 0;
	}*/

	return;
}

/* Windows has issues with MSXML and XercesC confusing eachother */
#define __msxml_h__
#define __ISoftDistExt_INTERFACE_DEFINED__

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE

class LayoutErrorHandler : public DefaultHandler {
public:
    LayoutErrorHandler(): isWarn(false), isError(false), isFatal(false) {}
    void warning(const SAXParseException& e)
    {
        int line, column;
        std::string errorStr;
        char tmpstr[32];
        char* message = XMLString::transcode( e.getMessage() );
        line = e.getLineNumber();
        column = e.getColumnNumber();
        sprintf(tmpstr, "Warning:%d:%d -> ", line, column);
        errorStr = tmpstr;
        errorStr += message;
        errors.push_back(errorStr);
        XMLString::release( &message );
        isWarn = true;
    }
    void error(const SAXParseException& e)
    {
        int line, column;
        std::string errorStr;
        char tmpstr[32];
        char* message = XMLString::transcode( e.getMessage() );
        line = e.getLineNumber();
        column = e.getColumnNumber();
        sprintf(tmpstr, "Error:%d:%d -> ", line, column);
        errorStr = tmpstr;
        errorStr += message;
        errors.push_back(errorStr);
        XMLString::release( &message );
        isError = true;
    }
    void fatalError(const SAXParseException& e)
    {
        int line, column;
        std::string errorStr;
        char tmpstr[32];
        char* message = XMLString::transcode( e.getMessage() );
        line = e.getLineNumber();
        column = e.getColumnNumber();
        sprintf(tmpstr, "Fatal:%d:%d -> ", line, column);
        errorStr = tmpstr;
        errorStr += message;
        errors.push_back(errorStr);
        XMLString::release( &message );
        isFatal = true;
    }

	std::vector<std::string> errors;
    bool isWarn;
    bool isError;
    bool isFatal;
};

static XMLCh* UTF8_ENCODING = NULL; 
static XMLTranscoder* UTF8_TRANSCODER  = NULL;

DataObject *LoadObject(DOMNode *node)
{
    DOMNamedNodeMap *attr;
    DOMNode *nAttr = NULL;
    int index;
    char *astr;
    const XMLCh* xstr;
    DataObject *output;

    //fprintf(stderr, "LoadObject(%p)\n", node);

    std::string o_id, o_class, o_typefunc, o_constructor;

	if (UTF8_TRANSCODER == NULL) {
		XMLTransService::Codes failReason;
		XMLPlatformUtils::Initialize();
		UTF8_ENCODING = XMLString::transcode("UTF-8");
		UTF8_TRANSCODER =
		  XMLPlatformUtils::fgTransService->makeNewTranscoderFor(UTF8_ENCODING,
																 failReason,
																 1024);
		/*if (!UTF8_TRANSCODER)
			::MessageBo("Failed to allocate utf8 transcoder" NL);*/
	}

	output = dataobject_new();

	xstr = node->getNodeName();
	astr = XMLString::transcode(xstr);
	dataobject_setValue(output, "type", dataobjectfield_string(astr));
	XMLString::release(&astr);

    /* parse attributes of node */
    attr = node->getAttributes();
	if (attr != NULL)
    for (index = 0; index < (int)attr->getLength();
            ++index) {
		std::string attrName, attrValue;
        nAttr = attr->item(index);

        xstr = nAttr->getNodeName();
        astr = XMLString::transcode(xstr);
        attrName = astr;
        XMLString::release(&astr);

        xstr = nAttr->getNodeValue();
        astr = XMLString::transcode(xstr);
        attrValue = astr;
        XMLString::release(&astr);

        /*fprintf(stderr, "LoadObject(%p) attr '%s' = '%s'\n", node,
                attrName.c_str(), attrValue.c_str());*/

		if (strcmp(attrName.c_str(), "src") == 0) { 
			FILE *input;
			int filesize;
			void *data;

			input = fopen(attrValue.c_str(), "rb");
			if (input == NULL) {
				emo_printf("Failed to load %s\n", attrValue.c_str());
				continue;
			}
			fseek(input, 0, SEEK_END);
			filesize = (int)ftell(input);
			fseek(input, 0, SEEK_SET);
			data = p_malloc(filesize);
			fread(data, 1, filesize, input);
			fclose(input);
			dataobject_setValue(output, "data",
					dataobjectfield_data(data, filesize));
		}
		dataobject_setValue(output, attrName.c_str(),
				dataobjectfield_string(attrValue.c_str()));
	}

	DOMNode *childNode;
    /* process children */
    for (childNode = node->getFirstChild(); childNode;
            childNode = childNode->getNextSibling()) {
        char *iastr;
        const XMLCh* ixstr;
        std::string p_name, p_value;
		DataObject *child;

        ixstr = childNode->getNodeName();
        iastr = XMLString::transcode(ixstr);

		/*fprintf(stderr, "LoadObject(%p) - child? - '%s'\n", node,
                iastr);*/

		if (strcmp(iastr, "#comment") == 0)
			continue;

		if (strcmp(iastr, "#text") == 0 || strcmp(iastr, "#cdata-section") == 0) {
			ixstr = childNode->getNodeValue();

			unsigned int charsEaten = 0;
			int length  = XMLString::stringLen(ixstr);
			XMLByte* res = new XMLByte[length * 8+1];
			unsigned int total_chars =
					UTF8_TRANSCODER->transcodeTo((const XMLCh*)ixstr,
						(unsigned int) length,
						(XMLByte*) res,
						(unsigned int) length*8,
						charsEaten,
						XMLTranscoder::UnRep_Throw
						);
			res[total_chars] = '\0';
			if (strcmp(iastr, "#cdata-section") != 0)
				XMLString::trim((char * const)res);

			if (res[0] != 0) {
					dataobject_setValue(output, "data", dataobjectfield_string((const char *)res));
			}
			delete res;
		} else {
			child = LoadObject(childNode);
			dataobject_pack(output, child);
		}
    }

    return output;
}

std::string xmlLoadFilename;
URL *xmlLoadURL;
extern "C" void xmlLoadObject(URL *url)
{
	xmlLoadFilename = url->path;
	xmlLoadFilename = xmlLoadFilename.substr(1);
	xmlLoadURL = url;
	GlobalDialog->OnFileLoadapplication();
	xmlLoadFilename = "";
}

void CpublicsimulatorDlg::OnFileLoadapplication()
{
    XercesDOMParser *configFileParser;
    xercesc::DOMDocument *xmlDoc;
    LayoutErrorHandler leh;
	
	CFileDialog *fDlg = NULL;
	char filename[2048];
	char wdbuffer[2048];
	int len;

	if (xmlLoadFilename.empty()) {
		fDlg = new CFileDialog(TRUE, _T("xml"));

		if (fDlg->DoModal() != IDOK) {
			delete fDlg;	
			return;
		}
		wcstombs(filename, fDlg->m_ofn.lpstrFile, 2048);
		_getcwd(wdbuffer, 2048);
		len = strlen(wdbuffer)+1;
		memmove(filename, filename+len, strlen(filename+len)+1);
	} else {
		strcpy(filename, xmlLoadFilename.c_str());
	}

    XMLPlatformUtils::Initialize();

    configFileParser = new XercesDOMParser;
    configFileParser->setValidationScheme( XercesDOMParser::Val_Never );
    configFileParser->setDoNamespaces( false );
    configFileParser->setDoSchema( false );
    configFileParser->setLoadExternalDTD( false );
    configFileParser->setErrorHandler(&leh);
    configFileParser->parse(filename);
    xmlDoc = configFileParser->getDocument();

	if (fDlg != NULL)
		delete fDlg;

    if (leh.isWarn || leh.isError | leh.isFatal) {
		wchar_t etext[2048];
		std::vector<std::string>::iterator iter;
		std::string errString;

		for (iter = leh.errors.begin(); iter != leh.errors.end(); ++iter)
			errString += (*iter);
		mbstowcs(etext, errString.c_str(), 2048);
		MessageBox(etext, _T("XML Error"));
        //ErrorDialog(leh.errors);
        if (leh.isFatal) {
			if (!xmlLoadFilename.empty())
				dataobject_construct(xmlLoadURL, 0);
			return;
		}
    }

	DataObject *dobj;
	DataObjectField *type;
    dobj = LoadObject(static_cast<DOMNode *>(xmlDoc->getDocumentElement()));
	if (dobj != NULL) {
		URL *url;
		std::string ustr;

		ustr = "xml://local/";
		ustr += filename;
		url = url_parse(ustr.c_str(), URL_ALL);

		cache_commitServerSide(dobj, url);

		mime_loadAll(dobj);
		dataobject_exportGlobal(dobj, url, 0);
		type = dataobject_getValue(dobj, "type");
		if (dataobjectfield_isString(type, "application")) {
			manager_loadApplication(dobj, xmlLoadFilename.empty(),
					url);
			/*app = application_load(dobj);
			manager_launchApplication(app);*/
		}
	}

	if (!xmlLoadFilename.empty()) {
		if (dobj == NULL)
			dataobject_construct(xmlLoadURL, 0);
		else
			dataobject_exportGlobal(dobj, xmlLoadURL, 0);
	}

    delete configFileParser;
    XMLPlatformUtils::Terminate();
	this->RedrawWindow();
}

void CpublicsimulatorDlg::OnFileQuit()
{
	OnOK();
}

void CpublicsimulatorDlg::OnHelpAbout()
{
	CAboutDlg about;

	about.DoModal();
}

extern "C" void emo_printf(const char *fmt, ...)
{
	char dest[16048];
	wchar_t wdest[16048];
	va_list ap;

	if (!GlobalDialog->m_isDebug || !GlobalDialog->m_showInternalDebug)
		return;

	int nLength = GlobalDialog->m_debugOuput.GetWindowTextLength();
	GlobalDialog->m_debugOuput.SetSel(nLength, nLength);
	va_start(ap, fmt);
    vsprintf(dest, fmt, ap);
    va_end(ap);

	mbstowcs(wdest, dest, 2048);
	GlobalDialog->m_debugOuput.ReplaceSel(wdest);
}

extern "C" void script_emo_printf(const char *fmt, ...)
{
	char dest[2048];
	wchar_t wdest[2048];
	va_list ap;

	if (!GlobalDialog->m_isDebug || !GlobalDialog->m_showScriptDebug)
		return;

	int nLength = GlobalDialog->m_debugOuput.GetWindowTextLength();
	GlobalDialog->m_debugOuput.SetSel(nLength, nLength);
	va_start(ap, fmt);
    vsprintf(dest, fmt, ap);
    va_end(ap);

	mbstowcs(wdest, dest, 2048);
	GlobalDialog->m_debugOuput.ReplaceSel(wdest);
}

void CpublicsimulatorDlg::OnDebugEnabled()
{
	if (m_isDebug)
		m_menu.CheckMenuItem(ID_DEBUG_ENABLED, MF_BYCOMMAND|MF_UNCHECKED);
	else
		m_menu.CheckMenuItem(ID_DEBUG_ENABLED, MF_BYCOMMAND|MF_CHECKED);
	m_isDebug = !m_isDebug;
	UpdateDebugStatus();
	this->RedrawWindow();
}

void CpublicsimulatorDlg::UpdateDebugStatus()
{
	RECT rect;
	this->GetWindowRect(&rect);

	if (m_isDebug)
		MoveWindow(rect.left, rect.top, 893, 678);
	else
		MoveWindow(rect.left, rect.top, 431, 678);
}

void CpublicsimulatorDlg::LoadSettings()
{
	wchar_t strr[1024];

	GetPrivateProfileString(_T("Debug"), _T("Enabled"), _T("true"), (LPWSTR)&strr, 1024, _T("peeksimulator.ini"));
	if (wcscmp(strr, _T("true")) == 0) {
		m_isDebug = true;
		m_menu.CheckMenuItem(ID_DEBUG_ENABLED, MF_BYCOMMAND|MF_CHECKED);
	} else {
		m_isDebug = false;
		m_menu.CheckMenuItem(ID_DEBUG_ENABLED, MF_BYCOMMAND|MF_UNCHECKED);
	}
	UpdateDebugStatus();

	GetPrivateProfileString(_T("Debug"), _T("ScriptDebug"), _T("true"), (LPWSTR)&strr, 1024, _T("peeksimulator.ini"));
	if (wcscmp(strr, _T("true")) == 0) {
		m_showScriptDebug = true;
		m_menu.CheckMenuItem(ID_DEBUG_SCRIPTDEBUGENABLED, MF_BYCOMMAND|MF_CHECKED);
	} else {
		m_showScriptDebug = false;
		m_menu.CheckMenuItem(ID_DEBUG_SCRIPTDEBUGENABLED, MF_BYCOMMAND|MF_UNCHECKED);
	}

	GetPrivateProfileString(_T("Debug"), _T("InternalDebug"), _T("false"), (LPWSTR)&strr, 1024, _T("peeksimulator.ini"));
	if (wcscmp(strr, _T("true")) == 0) {
		m_showInternalDebug = true;
		m_menu.CheckMenuItem(ID_DEBUG_INTERNAL, MF_BYCOMMAND|MF_CHECKED);
	} else {
		m_showInternalDebug = false;
		m_menu.CheckMenuItem(ID_DEBUG_INTERNAL, MF_BYCOMMAND|MF_UNCHECKED);
	}
}

void CpublicsimulatorDlg::SaveSettings()
{
	WritePrivateProfileString(_T("Debug"), _T("Enabled"), m_isDebug ? _T("true") : _T("false"), _T("peeksimulator.ini"));
	WritePrivateProfileString(_T("Debug"), _T("ScriptDebug"), m_showScriptDebug ? _T("true") : _T("false"), _T("peeksimulator.ini"));
	WritePrivateProfileString(_T("Debug"), _T("InternalDebug"), m_showInternalDebug ? _T("true") : _T("false"), _T("peeksimulator.ini"));
}
void CpublicsimulatorDlg::OnOK()
{
	SaveSettings();
	thread_kill(netThread);
	CDialog::OnOK();
	exit(0);
}

void CpublicsimulatorDlg::OnCancel()
{
	SaveSettings();
	CDialog::OnCancel();
}

//void CpublicsimulatorDlg::OnDebugInternaldebugenabled()
//{
//	if (m_showInternalDebug)
//		m_menu.CheckMenuItem(ID_DEBUG_INTERNALDEBUGENABLED, MF_BYCOMMAND|MF_UNCHECKED);
//	else
//		m_menu.CheckMenuItem(ID_DEBUG_INTERNALDEBUGENABLED, MF_BYCOMMAND|MF_CHECKED);
//	m_showInternalDebug = !m_showInternalDebug;
//}

void CpublicsimulatorDlg::OnDebugScriptdebugenabled()
{
	if (m_showScriptDebug)
		m_menu.CheckMenuItem(ID_DEBUG_SCRIPTDEBUGENABLED, MF_BYCOMMAND|MF_UNCHECKED);
	else
		m_menu.CheckMenuItem(ID_DEBUG_SCRIPTDEBUGENABLED, MF_BYCOMMAND|MF_CHECKED);
	m_showScriptDebug = !m_showScriptDebug;
}

//void CpublicsimulatorDlg::OnDebugInternaldebugenabled()
//{
//	if (m_showInternalDebug)
//		m_menu.CheckMenuItem(ID_DEBUG_INTERNALDEBUGENABLED, MF_BYCOMMAND|MF_UNCHECKED);
//	else
//		m_menu.CheckMenuItem(ID_DEBUG_INTERNALDEBUGENABLED, MF_BYCOMMAND|MF_CHECKED);
//	m_showInternalDebug = !m_showInternalDebug;
//}

void CpublicsimulatorDlg::OnDebugShowinternalmessages()
{
	if (m_showInternalDebug)
		m_menu.CheckMenuItem(ID_DEBUG_INTERNAL, MF_BYCOMMAND|MF_UNCHECKED);
	else
		m_menu.CheckMenuItem(ID_DEBUG_INTERNAL, MF_BYCOMMAND|MF_CHECKED);
	m_showInternalDebug = !m_showInternalDebug;
}

void CpublicsimulatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	DrawScreen();

	CDialog::OnTimer(nIDEvent);
}

#if 0
extern "C" {
Font *defaultFont;
Gradient *defaultGradient;
}

extern "C" DataObject *RootApplication(void)
{
	URL *rootAppURL;
	static DataObject *output = NULL;
	DataObject *fontObject1;
	Color color;

	if (output != NULL)
		return output;

	fontObject1 = dataobject_new();
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_string("DroidSans.ttf"));

	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);
	else {
		AfxMessageBox(_T("Failed to load DroidSans.ttf"));
		exit(1);
	}

	defaultGradient = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(defaultGradient, 0, color);
	color.value = 0xC9C9C9FF;
	gradient_addStop(defaultGradient, 50, color);
	color.value = 0xB1B1B1FF;
	gradient_addStop(defaultGradient, 50, color);
	color.value = 0xDEDEDEFF;
	gradient_addStop(defaultGradient, 100, color);

	rootAppURL = url_parse("xml://local/RootApplication/RootApplication.xml", URL_ALL);
	connectionContext_syncRequest(NULL, rootAppURL);
	output = dataobject_locate(rootAppURL);

	if (output == NULL) {
		AfxMessageBox(_T("Unable to open root application xml://local/RootApplication/RootApplication.xml"));
		exit(1);
	}

	return output;
}

extern "C" DataObject *BootApplication(void)
{
	URL *bootAppURL;
	static DataObject *output = NULL;

	if (output != NULL)
		return output;

	bootAppURL = url_parse("xml://local/RootApplication/BootApplication.xml", URL_ALL);
	connectionContext_syncRequest(NULL, bootAppURL);
	output = dataobject_locate(bootAppURL);

	if (output == NULL) {
		AfxMessageBox(_T("Unable to open root application xml://local/RootApplication/BootApplication.xml"));
		exit(1);
	}

	dataobject_codePrint(output);

	return output;
}

extern "C" DataObject *RootStyle(void)
{
	URL *rootStyleURL;
	static DataObject *output = NULL;

	if (output != NULL)
		return output;

	rootStyleURL = url_parse("xml://local/RootApplication/RootStyle.xml", URL_ALL);
	connectionContext_syncRequest(NULL, rootStyleURL);
	output = dataobject_locate(rootStyleURL);

	if (output == NULL) {
		AfxMessageBox(_T("Unable to open root application xml://local/RootApplication/RootStyle.xml"));
		exit(1);
	}

	return output;
}
#endif

void CpublicsimulatorDlg::OnFileClearcacheandreboot()
{
	FILE *input;

	input = fopen("clean_cache", "w");
	if (input != NULL) {
		fprintf(input, "\n");
		fclose(input);
	}
	_execl("peek-simulator.exe", "peek-simulator.exe", NULL);
}
