// SkuListView.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "SkuListView.h"

// CSkuListView

IMPLEMENT_DYNCREATE(CSkuListView, CListView)

CSkuListView::CSkuListView()
: m_bInitialized(FALSE)
{

}

CSkuListView::~CSkuListView()
{
}

BEGIN_MESSAGE_MAP(CSkuListView, CListView)
END_MESSAGE_MAP()


// CSkuListView diagnostics

#ifdef _DEBUG
void CSkuListView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CSkuListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

//
// Function: Sync data between database and user interface list view
// Parameter:
//     bSave - TRUE, copy data from list view to database
//             FALSE, copy data from database to list view
//
BOOL CSkuListView::UpdateData(BOOL bSave)
{
	CBpmpsApp * pTheApp = NULL;

	pTheApp = (CBpmpsApp *) AfxGetApp();

	ASSERT(pTheApp != NULL);
	ASSERT(pTheApp->m_MPSCore.GetState() != NULL);

	// if load data from database
	if (bSave != FALSE)
	{
	}

	// else, save data to database
	else
	{
		pTheApp->m_MPSCore.SelectAllFromSkuTable();
		int ColumnNum = pTheApp->m_MPSCore.GetFieldNum();

		// initialize list ctrl head
		if (!this->m_bInitialized)
		{

			for (int i = 0; i < ColumnNum; i++)
			{
				this->GetListCtrl().InsertColumn(i, pTheApp->m_MPSCore.GetFieldName(i), LVCFMT_LEFT, 120);
			}
		}
		else
		{
			this->GetListCtrl().DeleteAllItems();
		}

		//int RowCount = (int)pTheApp->m_MPSCore.GetRowNum();
		MYSQL_ROW Row = NULL;

		//this->GetListCtrl().SetItemCount((int)RowCount);
		this->LockWindowUpdate();
		int i = 0;

		while ((Row = pTheApp->m_MPSCore.GetRecord()) != NULL)
		{
			for (int j = 0; j < ColumnNum; j++)
			{
				if ( j == 0 )
				{
					this->GetListCtrl().InsertItem(i, Row[j]);
				}
				else
				{
					this->GetListCtrl().SetItemText(i, j, Row[j]);
				}

			}

			i++;
		}

		this->UnlockWindowUpdate();
		this->m_bInitialized = TRUE;
	}

	return FALSE;
}

BOOL CSkuListView::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: Add your specialized code here and/or call the base class
	dwExStyle |= LVS_EX_CHECKBOXES;
	return CListView::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
}
