// public-simulatorDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "CStatusGraphCtrl.h"

// CpublicsimulatorDlg dialog
class CpublicsimulatorDlg : public CDialog
{
// Construction
public:
	CpublicsimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PUBLICSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void DrawScreen(void);
	CBitmap m_display;
	afx_msg void OnFileLoadapplication();
	afx_msg void OnFileQuit();
	afx_msg void OnHelpAbout();
	CEdit m_debugOuput;
	CStatusGraphCtrl m_graph;
	bool m_isDebug, m_showInternalDebug, m_showScriptDebug;
	CMenu m_menu;
	afx_msg void OnDebugEnabled();
	void UpdateDebugStatus();
	void LoadSettings();
	void SaveSettings();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
//	afx_msg void OnDebugInternaldebugenabled();
	afx_msg void OnDebugScriptdebugenabled();
//	afx_msg void OnDebugInternaldebugenabled();
	afx_msg void OnDebugShowinternalmessages();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnFileClearcacheandreboot();
};
