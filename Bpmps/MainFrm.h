// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "DlgFilterPanel.h"
#include "DlgSkuGroupMgt.h"
#include "MPSCore.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CChildView  m_wndView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	//afx_msg LRESULT OnStarQuery(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessage_BtnClickQuery(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessage_UI_APPEND_RECORD(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessage_UI_UPDATE_PROCESS(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	CDlgSkuGroupMgt m_DlgSkuGroupMgt;

public:
	afx_msg void OnClickMenuEditOptions();
	afx_msg void OnMenu_SkuList();
protected:
	void HideAllPanelExclude(const CDialog *pDlgExclude);

public:
	afx_msg void OnConnect();
	afx_msg void OnDataSkucombinationmanagement();
	afx_msg void OnFileNew();
	afx_msg void OnFileLoad();
	afx_msg void OnNextWeek();
	afx_msg void OnPrevWeek();
	afx_msg void OnGotoWeek();
};
