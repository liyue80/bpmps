// DlgSkuCombMgt.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgSkuCombMgt.h"
#include "DlgInputName.h"

// CDlgSkuCombMgt dialog

IMPLEMENT_DYNAMIC(CDlgSkuCombMgt, CDialog)

CDlgSkuCombMgt::CDlgSkuCombMgt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkuCombMgt::IDD, pParent)
{

}

CDlgSkuCombMgt::~CDlgSkuCombMgt()
{
}

void CDlgSkuCombMgt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COMB, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSkuCombMgt, CDialog)
	ON_WM_CREATE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDlgSkuCombMgt::OnHdnItemclickListComb)
END_MESSAGE_MAP()


// CDlgSkuCombMgt message handlers

int CDlgSkuCombMgt::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CDlgSkuCombMgt::OnInitDialog()
{
	CString ErrMsg;

	CDialog::OnInitDialog();
	DWORD dwStyle = ::GetWindowLongA(this->m_ListCtrl.GetSafeHwnd(), GWL_STYLE);
	dwStyle |= LVS_REPORT;
	::SetWindowLongA(this->m_ListCtrl.GetSafeHwnd(), GWL_STYLE, dwStyle);
	dwStyle = this->m_ListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	this->m_ListCtrl.SetExtendedStyle(dwStyle);

	static const char * ColumnHead[] = {"Index Code", "Mapping 1", "Mapping 2",
		"Mapping 3", "Mapping 4", "Mapping 5", "Mapping 6", "Mapping 7"
	};
	for (int i=0; i<8; i++)
		m_ListCtrl.InsertColumn(i, ColumnHead[i], 0, 80);

	CMyDatabase db;
	if (db.ConnectDB("localhost", "root", "", "test", 0, NULL, 0) != TRUE)
		return FALSE;
	if (db.SelectAll("skuinfo") != TRUE)
		return FALSE;
	MYSQL_ROW row = NULL;

	while ( (row = db.GetRecord()) != NULL )
	{
		int index = m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), row[0] );
		m_ListCtrl.SetItemData(index, atoi(row[0]));
		for (int i = 0; i < 7; i++)
		{
			if (row[i+2] != 0)
				m_ListCtrl.SetItemText(index, i + 1, row[i+2]);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

struct _tagListViewSortFuncParam
{
	CListCtrl *pListCtrl;
	BOOL bDesc;	// ÄæÐò£¿
};

static int CALLBACK ListViewSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	LVITEM *pItemFirst = (LVITEM *)lParam1;
	LVITEM *pItemSecond = (LVITEM *)lParam2;
	_tagListViewSortFuncParam * pParamSort
		= (_tagListViewSortFuncParam *) lParamSort;

	if (lParam1 == lParam2)
		return 0;

	if (pParamSort->bDesc)
	{
		if (lParam1 > lParam2)
			return 1;
		else
			return -1;
	}
	else
	{
		if (lParam1 > lParam2)
			return -1;
		else
			return 1;
	}
}

void CDlgSkuCombMgt::OnHdnItemclickListComb(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	_tagListViewSortFuncParam ParamSort = {0};

	if (phdr->iItem != 0)
		return;

	//////////////////////////////////////////////////////////////////////////
	// Ë³Ðò ÄæÐò
	LONG UserData = GetWindowLong(m_ListCtrl.GetSafeHwnd(), GWL_USERDATA);

	switch (UserData & 0x03)
	{
	case 0x00:
		ParamSort.bDesc = FALSE;
		UserData |= 0x02;
		break;
	case 0x01:
		ASSERT(FALSE);
		break;
	case 0x02:
		ParamSort.bDesc = TRUE;
		UserData |= 0x03;
		break;
	case 0x03:
		ParamSort.bDesc = FALSE;
		UserData &= ~0x01;
		break;
	default:
		ASSERT(FALSE);
	}

	SetWindowLong(m_ListCtrl.GetSafeHwnd(), GWL_USERDATA, UserData);

	//////////////////////////////////////////////////////////////////////////
	ParamSort.pListCtrl = &m_ListCtrl;
	m_ListCtrl.SortItems(ListViewSortFunc, (LPARAM)&ParamSort);
	*pResult = 0;
}
