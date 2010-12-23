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

	InitializeListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//
// Called by CDlgLoad::OnInitDialog()
//
BOOL CDlgLoad::InitializeListCtrl(void)
{
	m_ListCtrl.SetExtendedStyle(
		m_ListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	//设置列名
	static const int COLUMN_NUM = 3;

	LPCTSTR ColumnText[] = 
	{
		"StartingDate", "SaleWk1", "RecordCount"
	};
	ASSERT ( sizeof(ColumnText) == COLUMN_NUM * sizeof(LPCTSTR) );

	int ColumnWidth[] =
	{
		100, 100, 80
	};
	ASSERT( sizeof(ColumnWidth) == COLUMN_NUM * sizeof(int) );

	for ( int i = 0; i < COLUMN_NUM; i++ )
	{
		m_ListCtrl.InsertColumn(i, ColumnText[i]);
		m_ListCtrl.SetColumnWidth(i, ColumnWidth[i]);
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
			if (CMPSCore::DecodeTableName(
				SavedTableNames.GetAt(i), StartingDate, SaleWk1) != FALSE)
			{
				int iItem = m_ListCtrl.InsertItem(ItemCount,
					CMPSCore::ConvertDateToString(StartingDate));
				m_ListCtrl.SetItemText(iItem, 1,
					CMPSCore::ConvertDateToString(SaleWk1));
			}
		}
	}

	return bRet;
}
