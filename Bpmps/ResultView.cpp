// ResultView.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "ResultView.h"


// CResultView

IMPLEMENT_DYNCREATE(CResultView, CListView)

CResultView::CResultView()
{

}

CResultView::~CResultView()
{
}

BEGIN_MESSAGE_MAP(CResultView, CListView)
END_MESSAGE_MAP()


// CResultView diagnostics

#ifdef _DEBUG
void CResultView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CResultView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

BOOL CResultView::UpdateContent(MYSQL_RES *pMySqlRes, int nFieldCount)
{
	// 检查参数
	ASSERT(pMySqlRes != NULL);
	if (pMySqlRes == NULL)
	{
		return FALSE;
	}

	// 清空控件上的旧数据
	{
		this->GetListCtrl().DeleteAllItems();
		int nColumnCount = this->GetListCtrl().GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
		{
			this->GetListCtrl().DeleteColumn(0);
		}
	}

	// 设置列
	MYSQL_FIELD * pFields = mysql_fetch_fields(pMySqlRes);
	for (int i=0; i < nFieldCount; i++)
	{
		this->GetListCtrl().InsertColumn(i, pFields[i].name, 0, 100);
	}

	MYSQL_ROW Row = mysql_fetch_row(pMySqlRes);
	while (Row != NULL)
	{
		this->GetListCtrl().InsertItem(0, Row[0]);
		Row = mysql_fetch_row(pMySqlRes);
	}

	return TRUE;
}

// This function is called when user clicks "OK" on the filter dialog.
BOOL CResultView::UpdateData(const CQueryFilter *pQueryFilter)
{
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();

	this->GetListCtrl().DeleteAllItems();

	CString SqlStr("select fullindex from fullindex_t");

	if (!pTheApp->m_MPSCore.SelectQuery(SqlStr))
		return FALSE;

	MYSQL_ROW Row = NULL;
	CArray <CString> FullIndexArray;

	while ((Row = pTheApp->m_MPSCore.GetRecord()) != NULL)
	{
		if (!Row[0])
			break;
		FullIndexArray.Add(CString((LPCTSTR)Row[0]));
	}

	pTheApp->m_MPSCore.FreeRecord();

	CArray <CString> ResultLine;

	for (INT_PTR i = 0; i < FullIndexArray.GetSize(); i++)
	{
		BOOL bRet = pTheApp->m_MPSCore.QueryFinalResult(
			pQueryFilter->StartingDate, FullIndexArray.GetAt(i), ResultLine);
		
		if (!bRet)
		{
			pTheApp->GetMainWnd()->MessageBox("Unexpected error",
				"Error", MB_OK | MB_ICONERROR);
			break;
		}

		int index = this->GetListCtrl().InsertItem((int)i, FullIndexArray.GetAt(i));
		if (index != -1)
		{
			for (INT_PTR j = 0; j < ResultLine.GetSize(); j++)
			{
				this->GetListCtrl().SetItemText(
					index, (int)j + 1, ResultLine.GetAt(j));
			}
		}
	}

	return TRUE;
}
