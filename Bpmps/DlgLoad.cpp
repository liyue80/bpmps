// DlgLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgLoad.h"
#include "afxdialogex.h"
#include "MPSCore.h"

// CDlgLoad 对话框

IMPLEMENT_DYNAMIC(CDlgLoad, CDialogEx)

CDlgLoad::CDlgLoad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLoad::IDD, pParent)
{

}

CDlgLoad::~CDlgLoad()
{
}

void CDlgLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOAD, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLoad, CDialogEx)
END_MESSAGE_MAP()


// CDlgLoad 消息处理程序


BOOL CDlgLoad::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化m_ListCtrl
	//设置列名
	static const int COLUMN_NUM = 3;

	LPCTSTR ColumnText[] = 
	{
		"StartingDate", "SaleWk1", "RecordCount"
	};
	ASSERT ( sizeof(ColumnText) == COLUMN_NUM * sizeof(LPCTSTR) );

	int ColumnWidth[] =
	{
		200, 200, 200
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

		int iRet = m_ListCtrl.GetHeaderCtrl()->InsertItem(i, &Item);
		if ( -1 == iRet)
		{
			AfxMessageBox("Initialize column tree view failed");
			break;
		}
	}

	//添加m_ListCtrl中的数据
	CMPSCore Core;
	BOOL bRet = FALSE;
	CArray<CString> SavedTableNames;
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	if (bRet)
	{
		bRet = Core.GetAllSavedTable(SavedTableNames);
	}

	if (bRet)
	{
		CTime StartingDate, SaleWk1;
		INT_PTR nCount = SavedTableNames.GetCount();
		int ItemCount = 0;
		for (INT_PTR i = 0; i < nCount; i++)
		{
			CString TableName = SavedTableNames.GetAt(i);
			BOOL bRetLocal
				= CMPSCore::DecodeTableName(TableName, StartingDate, SaleWk1);
			ASSERT(bRetLocal == TRUE);
			if (bRetLocal != TRUE)
				continue;
			CString ItemText = CMPSCore::ConvertDateToString(StartingDate);
			CString SubItemText = CMPSCore::ConvertDateToString(SaleWk1);

			LVITEM lvi = {0};
			// Insert the item
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)(LPCTSTR)(ItemText);
			m_ListCtrl.InsertItem(&lvi);
			// Set subitem 1
			lvi.iSubItem = 1;
			lvi.pszText = (LPTSTR)(LPCTSTR)(SubItemText);
			m_ListCtrl.SetItem(&lvi);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
