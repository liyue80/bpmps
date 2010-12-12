// ResultTreeview.cpp : ʵ���ļ�
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
	ON_MESSAGE(WM_USER_UI_APPEND_RECORD, &CResultTreeview::OnMessage_AppendRecord)
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
}

BOOL CResultTreeview::UpdateData( const CQueryFilter *pQueryFilter )
{
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();

	// �����ͼ����
	this->GetTreeCtrl().DeleteAllItems();

	// ������Ҫ�������Ŀ
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

	// ѭ�����㡢��ʾ���
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

// WM_USER_UI_APPEND_RECORD
LRESULT CResultTreeview::OnMessage_AppendRecord( WPARAM wParam, LPARAM lParam )
{
	ASSERT (lParam != NULL);

	APPENDING_RECORD * pParam = (APPENDING_RECORD *) lParam;

	HTREEITEM hItem = NULL;
	CString ItemString;
	CTreeCtrl &TreeCtrl = this->GetTreeCtrl();

	hItem = TreeCtrl.InsertItem(pParam->_SkuCode, TVI_ROOT, TVI_LAST);
	if (hItem != NULL)
	{
		ItemString.Format("%s\t%s", pParam->_Warehouse, pParam->_OpenInvFirst);
		TreeCtrl.InsertItem((LPCTSTR)ItemString, hItem, TVI_LAST);
	}

	delete pParam;
	return 0;
}

// CResultTreeview ��Ϣ�������
