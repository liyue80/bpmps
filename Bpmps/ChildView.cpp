// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Bpmps.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
: m_pResultView(NULL)
, m_pSkuListView(NULL)
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT( this->m_pResultView == NULL );

	RECT rc = {0};

	//
	// Create and Initialize header controller
	//
	this->m_pResultView = new CResultTreeview;
	this->m_pResultView->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES, rc, this, ID_RESULT_VIEW);

	//
	// Create object of CSkuListView
	//
	this->m_pSkuListView = new CSkuListView;
	this->m_pSkuListView->CreateEx(0, NULL, NULL, WS_CHILD | LVS_REPORT, rc, this, ID_SKULIST_VIEW);

	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ASSERT( this->m_pResultView != NULL );

	RECT rc = {0};
	this->GetClientRect(&rc);
	if (this->m_pResultView->IsWindowVisible())
	{
		this->m_pResultView->MoveWindow(&rc, TRUE);
		return;		// only one child window is visible at the same time
	}

	if (this->m_pSkuListView->IsWindowVisible())
	{
		this->m_pSkuListView->MoveWindow(&rc, TRUE);
		return;		// only one child window is visible at the same time
	}
}

//
// Function :
//     Hide all of child view, exclude the parameter specified.
//
void CChildView::HideAllChildViewExclude(const CWnd * pViewExclude)
{
	CArray <CWnd *> WndArray;
	INT_PTR ArraySize = 0;
	INT_PTR i = 0;

	// Add all of child view here
	WndArray.Add((CWnd *)this->m_pResultView);
	WndArray.Add((CWnd *)this->m_pSkuListView);

	ArraySize = WndArray.GetSize();

	for ( i = 0; i < ArraySize; i++ )
	{
		CWnd * pIterator = WndArray.GetAt(i);
		if (pIterator != pViewExclude)
		{
			pIterator->ShowWindow(SW_HIDE);
		}
	}
}

void CChildView::SwitchSkuListView(void)
{
	RECT rc = {0};

	if (this->m_pSkuListView->IsWindowVisible())
	{
		return;
	}

	this->HideAllChildViewExclude((CWnd *) this->m_pSkuListView);

	this->m_pSkuListView->ShowWindow(SW_SHOW);

	this->GetClientRect(&rc);

	this->m_pSkuListView->MoveWindow(&rc, TRUE);

	if (!this->m_pSkuListView->IsInitialized())
	{
		this->m_pSkuListView->UpdateData(FALSE);
	}
}
