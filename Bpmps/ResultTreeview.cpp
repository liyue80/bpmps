// ResultTreeview.cpp : 实现文件
//

#include "stdafx.h"
#include "Bpmps.h"
#include "ResultTreeview.h"


// CResultTreeview

IMPLEMENT_DYNCREATE(CResultTreeview, CColumnTreeView)

CResultTreeview::CResultTreeview()
{

}

CResultTreeview::~CResultTreeview()
{
}

BEGIN_MESSAGE_MAP(CResultTreeview, CColumnTreeView)
END_MESSAGE_MAP()

void CResultTreeview::OnInitialUpdate()
{
	CColumnTreeView::OnInitialUpdate();

	//
	// Create object of CResultView
	//
	static const int COLUMN_NUM = 12;

	LPCTSTR ColumnText[] = 
	{
		"FullIndex",
		"OpenInv",
		"Outstanding PO",
		"LT Forecast",
		"Rolling FC vs Sales (3 Months)",
		"Recommended Order Volume",
		"Action Flag",
		"Last Week Order",
		"Recommend Without Last Week Order",
		"Production Tag",
		"Arrival",
		"Sales"
	};
	ASSERT ( sizeof(ColumnText) == COLUMN_NUM * sizeof(LPCTSTR) );

	int ColumnWidth[] =
	{
		190, 80, 140, 80, 120, 140, 140, 140, 140, 140, 80, 80
	};
	ASSERT( sizeof(ColumnWidth) == COLUMN_NUM * sizeof(int) );

	char Buffer[128] = {0}; // Large than max length of ColumnText

	for ( int i = 0; i < COLUMN_NUM; i++ )
	{
		HDITEM Item = {0};
		Item.mask = HDI_TEXT | HDI_WIDTH | HDI_FORMAT;
		Item.fmt  = HDF_LEFT | HDF_STRING;
		Item.cxy  = ColumnWidth[i];
		strcpy_s(Buffer, ColumnText[i]);
		Item.pszText = &Buffer[0];
		Item.cchTextMax = strlen(Buffer);

		int iRet = this->GetHeaderCtrl().InsertItem(i, &Item);
		if ( -1 == iRet)
		{
			AfxMessageBox("Initialize column tree view failed");
			break;
		}
	}

	// Refresh tree view based on its header controller
	this->UpdateColumns();

#if defined(DEBUG)
	HTREEITEM p1 = this->GetTreeCtrl().InsertItem("p1");
	HTREEITEM c1 = this->GetTreeCtrl().InsertItem("c1", p1);
	ModifyStyle(NULL, TVS_HASLINES |TVS_LINESATROOT);
#endif
}

BOOL CResultTreeview::UpdateData( const CQueryFilter *pQueryFilter )
{
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();

	// 清空视图内容
	this->GetTreeCtrl().DeleteAllItems();

	// 加载需要计算的项目
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

	// 循环计算、显示结果
	CArray <CString> ResultLine;
	INT_PTR SkuCount = FullIndexArray.GetCount();
	for (INT_PTR i = 0; i < SkuCount; i++)
	{
		BOOL bRet = pTheApp->m_MPSCore.QueryFinalResult(
			pQueryFilter->StartingDate, pQueryFilter->FirstWeekSale,
			FullIndexArray.GetAt(i), ResultLine);

		if ( bRet == TRUE )
		{
			CString ItemText = FullIndexArray.GetAt(i);
			INT_PTR ColumnCount = ResultLine.GetCount();
			for (INT_PTR j = 0; j < ColumnCount; j++)
			{
				ItemText.Append("\t");
				ItemText.Append(ResultLine.GetAt(j));
			}

			HTREEITEM hItem = this->GetTreeCtrl().InsertItem((LPCTSTR)ItemText);
			if (hItem != NULL)
				this->GetTreeCtrl().InsertItem("Child Testing\t1\t2\t3", hItem);
		}
	}

	return TRUE;
}


// CResultTreeview 消息处理程序
