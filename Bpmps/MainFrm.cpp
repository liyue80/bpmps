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
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
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

	if (!m_DlgFilterPanel.Create(IDD_FILTER_PANEL, this))
	{
		TRACE0("Failed to create filter panel\n");
		return -1;		// fail to create
	}

	m_DlgFilterPanel.ResetDimension();
	m_DlgFilterPanel.ShowWindow(SW_SHOW);

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

	CResultView *pResultView = NULL;

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
	DialogArray.Add(&this->m_DlgFilterPanel);

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
	CMPSCore *pCore = this->GetCore();
	LPCTSTR lpszHost = _T("localhost");
	LPCTSTR lpszUser = _T("root");

	if (!pCore->ConnectDB(lpszHost, lpszUser, "", "test", 0, NULL, 0))
	{
		this->MessageBox(pCore->OutErrors(), _T("Fatal Error"), MB_OK|MB_ICONERROR);
		return;
	}

	TRACE2("Connected to MySQL @%s as %s\n", lpszHost, lpszUser);
}

CMPSCore * CMainFrame::GetCore(void) const
{
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();

	ASSERT(pTheApp != NULL);

	return &(pTheApp->m_MPSCore);
}

void CMainFrame::OnDataSkucombinationmanagement()
{
	CDlgSkuCombMgt Dlg;
	Dlg.DoModal();
}
