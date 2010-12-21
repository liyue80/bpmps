// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Bpmps.h"

#include "MainFrm.h"
#include "DlgOptions.h"
#include "DlgSkuCombMgt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_OPTIONS, &CMainFrame::OnClickMenuEditOptions)
	ON_COMMAND(ID_DATA_SKULIST, &CMainFrame::OnMenu_SkuList)
	ON_COMMAND(ID_CONNECT, &CMainFrame::OnConnect)
	ON_MESSAGE(WM_BTN_CLICK_QUERY, OnMessage_BtnClickQuery)
	ON_COMMAND(ID_DATA_SKUCOMBINATIONMANAGEMENT, &CMainFrame::OnDataSkucombinationmanagement)
	ON_MESSAGE(WM_USER_UI_APPEND_RECORD, &CMainFrame::OnMessage_UI_APPEND_RECORD)
	ON_MESSAGE(WM_USER_UI_UPDATE_PROCESS, &CMainFrame::OnMessage_UI_UPDATE_PROCESS)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_FILE_LOAD, &CMainFrame::OnFileLoad)
	ON_COMMAND(ID_NEXT_WEEK, &CMainFrame::OnNextWeek)
	ON_COMMAND(ID_PREV_WEEK, &CMainFrame::OnPrevWeek)
	ON_COMMAND(ID_GOTO_WEEK, &CMainFrame::OnGotoWeek)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_EXPAND,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Adjust status bar
	UINT nID = 0;
	UINT nStyle = 0;
	int  cxWidth = 0;
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
	nStyle &= ~SBPS_STRETCH;
	m_wndStatusBar.SetPaneInfo(0, nID, nStyle, 320);
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, cxWidth);
	nStyle |= SBPS_STRETCH;
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, cxWidth);
	//m_wndStatusBar.SetPaneText(1, "");

	if (!m_DlgSkuGroupMgt.Create(IDD_SKU_GROUP_MGT, this))
	{
		TRACE0("Failed to create dialog of SKU Group Management\n");
		return -1;		// fail to create
	}

	m_DlgSkuGroupMgt.ResetDimension();

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// connect to database automatically
	this->OnConnect();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//
// 点击菜单项 Options 打开配置窗口
//
void CMainFrame::OnClickMenuEditOptions()
{
	CDlgOptions DlgOptions;
	DlgOptions.DoModal();
}

//
// wParam - reserved
// lParam - Pointer to CQueryFilter
//
LRESULT CMainFrame::OnMessage_BtnClickQuery(WPARAM wParam, LPARAM lParam)
{
	ASSERT(lParam != NULL);

	CResultTreeview *pResultView = NULL;

	pResultView = this->m_wndView.GetResultView();

	ASSERT(pResultView != NULL);

	pResultView->UpdateData((const CQueryFilter *)lParam);

	return 0L;
}

//
// Function:
//     Show dialog SKU Group Management, hide other dialogs.
//     Switch to SKU ListView.
//     Load SKU List and SKU Group from database
//
void CMainFrame::OnMenu_SkuList()
{
	this->HideAllPanelExclude((CDialog *) &this->m_DlgSkuGroupMgt);
	this->m_DlgSkuGroupMgt.ShowWindow(SW_SHOW);
	this->m_wndView.SwitchSkuListView();
}

void CMainFrame::HideAllPanelExclude(const CDialog * pDlgExclude)
{
	CArray <CDialog *> DialogArray;
	INT_PTR ArraySize = 0;
	INT_PTR i = 0;

	ASSERT(pDlgExclude != NULL);

	// add more dialogs here
	DialogArray.Add(&this->m_DlgSkuGroupMgt);

	ArraySize = DialogArray.GetCount();

	for (i = 0; i < ArraySize; i++)
	{
		CDialog *pIterator = DialogArray.GetAt(i);

		if (pDlgExclude != pIterator)
		{
			pIterator->ShowWindow(SW_HIDE);
		}
	}
}

void CMainFrame::OnConnect()
{
#if 0
	LPCTSTR lpszHost = _T("localhost");
	LPCTSTR lpszUser = _T("root");

	if (!pCore->ConnectDB(lpszHost, lpszUser, "", "test", 0, NULL, 0))
	{
		this->MessageBox(pCore->OutErrors(), _T("Fatal Error"), MB_OK|MB_ICONERROR);
		return;
	}

	TRACE2("Connected to MySQL @%s as %s\n", lpszHost, lpszUser);
#endif
}

#if 0
CMPSCore * CMainFrame::GetCore(void) const
{
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();

	ASSERT(pTheApp != NULL);

	return &(pTheApp->m_MPSCore);
}
#endif

void CMainFrame::OnDataSkucombinationmanagement()
{
	CDlgSkuCombMgt Dlg;
	Dlg.DoModal();
}

// WM_USER_UI_APPEND_RECORD
LRESULT CMainFrame::OnMessage_UI_APPEND_RECORD( WPARAM wParam, LPARAM lParam )
{
	return m_wndView.GetResultView()->OnMessage_UI_APPEND_RECORD(wParam, lParam);
}

LRESULT CMainFrame::OnMessage_UI_UPDATE_PROCESS( WPARAM wParam, LPARAM lParam )
{
	UINT nCount = 0xFFFF & (UINT)lParam;
	UINT nAmount = (UINT)lParam >> 16;

	switch (wParam)
	{
	case RESET_PROCESS:
		{
		}
		break;
	case UPDATE_PROCESS:
		{
			CString StateBarText;
			StateBarText.Format("%u/%u", nCount, nAmount);
			m_wndStatusBar.SetPaneText(0, (LPCTSTR)StateBarText);
		}
		break;
	case END_PROCESS:
		{
			CString StateBarText;
			StateBarText.Format("Done %u/%u", nCount, nAmount);
			m_wndStatusBar.SetPaneText(0, (LPCTSTR)StateBarText);
		}
		break;
	default:
		ASSERT(FALSE);
	}

	return 0;
}


void CMainFrame::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码
	CDlgFilterPanel Dlg;
	Dlg.DoModal();
}


void CMainFrame::OnFileLoad()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnNextWeek()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnPrevWeek()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnGotoWeek()
{
	// TODO: 在此添加命令处理程序代码
	m_wndStatusBar.SetPaneText(ID_SEPARATOR, "Ready!\tDate:2010-10-12");
}
