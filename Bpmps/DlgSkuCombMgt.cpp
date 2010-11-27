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
	DDX_Control(pDX, IDC_SKUCB_LIST_ALL, m_ctrListAll);
	DDX_Control(pDX, IDC_SKUCB_LIST_SKUID, m_ctrListCombineName);
	DDX_Control(pDX, IDC_SKUCB_LIST_SELECTED, m_ctrListSelected);
	DDX_Control(pDX, IDC_SKUCD_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_SKUCD_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_SKUCD_REMOVE, m_btnRemove);
}


BEGIN_MESSAGE_MAP(CDlgSkuCombMgt, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SKUCB_CREATE, &CDlgSkuCombMgt::OnBnClickedSkucbCreate)
	ON_LBN_SELCHANGE(IDC_SKUCB_LIST_SKUID, &CDlgSkuCombMgt::OnLbnSelchangeSkucbListSkuid)
	ON_BN_CLICKED(IDC_SKUCD_ADD, &CDlgSkuCombMgt::OnBnClickedSkucdAdd)
	ON_BN_CLICKED(IDC_SKUCD_REMOVE, &CDlgSkuCombMgt::OnBnClickedSkucdRemove)
	ON_BN_CLICKED(IDC_SKUCD_DELETE, &CDlgSkuCombMgt::OnBnClickedSkucdDelete)
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

	if ( !InitList_AllSku( ErrMsg ) )
	{
		AfxMessageBox(ErrMsg, MB_OK | MB_ICONERROR);
		return TRUE;
	}

	if ( !InitList_AllSkuCombine( ErrMsg ) )
	{
		AfxMessageBox(ErrMsg, MB_OK | MB_ICONERROR);
		return TRUE;
	}

	this->m_ctrListSelected.EnableWindow(FALSE);
	this->m_btnDelete.EnableWindow(FALSE);
	this->m_btnAdd.EnableWindow(FALSE);
	this->m_btnRemove.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//
// 给对话框上罗列所有skucode的列表赋以初值
//
BOOL CDlgSkuCombMgt::InitList_AllSku( CString &ErrMsg )
{
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;
	MYSQL_ROW Row = NULL;

	if (!pCore->SelectQuery("SELECT DISTINCT `jdeskucode` from `sku_d` order by `jdeskucode`"))
	{
		ErrMsg = pCore->OutErrors();
		return FALSE;
	}

	while (Row = pCore->GetRecord(), (Row && Row[0]))
	{
		m_ctrListAll.AddString(Row[0]);
	}

	pCore->FreeRecord();

	return TRUE;
}

BOOL CDlgSkuCombMgt::InitList_AllSkuCombine( CString &ErrMsg )
{
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;
	MYSQL_ROW Row = NULL;

	if (!pCore->SelectQuery("SELECT DISTINCT `cbname` FROM `skucombine_s`"))
	{
		ErrMsg = pCore->OutErrors();
		return FALSE;
	}

	while (Row = pCore->GetRecord(), (Row && Row[0]))
	{
		m_ctrListCombineName.AddString(Row[0]);
	}

	pCore->FreeRecord();

	return TRUE;
}

void CDlgSkuCombMgt::OnBnClickedSkucbCreate()
{
	CDlgInputName Dlg;
	CString Name;
	int ItemIndex = 0;
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;

	if (Dlg.DoModal() != IDOK)
		return;

	Dlg.m_Name.Trim();

	if ( !IsValidName(Dlg.m_Name) || Dlg.m_Name.GetLength() == 0 )
	{
		AfxMessageBox("Wrong name");
		return;
	}

	// Check duplication
	ItemIndex = this->m_ctrListCombineName.FindString(-1, Dlg.m_Name);

	if (ItemIndex == LB_ERR) // good., not duplicated
	{
		CString ReturnErrorString;
		if ( !pCore->CreateNewSkuCombination(Dlg.m_Name, &ReturnErrorString) )
		{
			AfxMessageBox( ReturnErrorString, MB_OK | MB_ICONERROR );
			return;
		}
		else
		{
			ItemIndex = this->m_ctrListCombineName.AddString(Dlg.m_Name);

			// move focus to this new item, and clean the "selected" list
			while (this->m_ctrListSelected.DeleteString(0) != LB_ERR)
			{
				;
			}

			if (this->m_ctrListCombineName.SetCurSel(ItemIndex) != LB_ERR)
			{
				// No notify with SetCurSel(), so call it manually
				this->OnLbnSelchangeSkucbListSkuid();
			}
		}
	}
	else
	{
		AfxMessageBox( "Failed, because the new name has been exist",
			MB_OK | MB_ICONWARNING );
	}
}

void CDlgSkuCombMgt::OnLbnSelchangeSkucbListSkuid(void)
{
	CString CbName;
	int ItemIndex = this->m_ctrListCombineName.GetCurSel();

	if ( ItemIndex == LB_ERR )
		return;

	this->m_ctrListCombineName.GetText(ItemIndex, CbName);
	ASSERT(CbName.GetLength() != 0);
	if (CbName.GetLength() == 0)
		return;

	this->m_ctrListSelected.EnableWindow();
	this->m_btnDelete.EnableWindow();
	this->m_btnAdd.EnableWindow();
	this->m_btnRemove.EnableWindow();

	// 刷新"SELECTED"列表
	// 刷新 步骤1 清空
	while ( this->m_ctrListSelected.DeleteString( 0 ) != LB_ERR )
	{
		;
	}

	// 刷新 步骤2 添加新字符串
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;
	CArray<CString> SkuCodesAry;
	CString ErrStr;
	INT_PTR i = 0;
	if (!pCore->GetAllSkuCodesOfCombination(CbName, &SkuCodesAry, &ErrStr))
	{
		AfxMessageBox(ErrStr, MB_OK | MB_ICONERROR);
		return;
	}
	for ( i = 0; i < SkuCodesAry.GetCount(); i++)
	{
		this->m_ctrListSelected.AddString( (LPCTSTR) SkuCodesAry.GetAt(i) );
	}
}

BOOL CDlgSkuCombMgt::IsValidName( const CString & Name )
{
	for (int i = 0; i < Name.GetLength(); i++)
	{
		TCHAR c = Name.GetAt(i);

		if ( ! (( c >= '0' && c <= '9') || ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z')) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

//
// 响应用户的按键，把选中的SKUCODE添加到组合中
void CDlgSkuCombMgt::OnBnClickedSkucdAdd()
{
	int ItemIndex1 = 0, ItemIndex2 = 0, ItemIndex3 = 0;
	CString SkuCode, CbName, ErrStr;
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;

	// 取出选中的SKUCODE
	ItemIndex1 = m_ctrListAll.GetCurSel( );
	if ( ItemIndex1 == LB_ERR )
		return;
	m_ctrListAll.GetText( ItemIndex1, SkuCode );

	// 检查SKUCODE是否已经存在于组合中
	ItemIndex2 = m_ctrListSelected.FindString( -1, SkuCode );
	if ( ItemIndex2 != LB_ERR )
		return; // 用户操作无效：SKUCODE已经在组合中了

	// 取出当前的组合名称
	ItemIndex3 = m_ctrListCombineName.GetCurSel();
	if ( ItemIndex3 == LB_ERR )
		return;
	m_ctrListCombineName.GetText(ItemIndex3, CbName);

	// 在数据库中添加
	if ( !pCore->AddSkuCodeToCombination(CbName, SkuCode, &ErrStr) )
	{
		AfxMessageBox(ErrStr, MB_OK | MB_ICONERROR);
		return;
	}

	// 在用户界面上添加
	ItemIndex2 = m_ctrListSelected.AddString( SkuCode );
	m_ctrListSelected.SetCurSel( ItemIndex2 );
}

//
// 响应用户按键，从组合中删除一个SKUCODE
void CDlgSkuCombMgt::OnBnClickedSkucdRemove()
{
	int ItemIndex1 = 0, ItemIndex2 = 0;
	CString SkuCode, CbName, ErrStr;
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;

	// 取出组合中欲移除的SKUCODE
	ItemIndex1 = m_ctrListSelected.GetCurSel();
	if ( ItemIndex1 == LB_ERR )
		return;
	m_ctrListSelected.GetText(ItemIndex1, SkuCode);

	// 取出当前的组合名称
	ItemIndex2 = m_ctrListCombineName.GetCurSel();
	if ( ItemIndex2 == LB_ERR )
		return;
	m_ctrListCombineName.GetText(ItemIndex2, CbName);

	// 从数据库中移除
	if (!pCore->RemoveSkuCodeFromCombination(CbName, SkuCode, &ErrStr))
	{
		AfxMessageBox(ErrStr, MB_OK | MB_ICONERROR);
		return;
	}

	// 从界面上移除
	m_ctrListSelected.DeleteString( ItemIndex1 );
}

//
// 响应用户按键，删除一个组合
void CDlgSkuCombMgt::OnBnClickedSkucdDelete()
{
	CString CbName, ErrStr;
	int ItemIndex = 0;
	CMPSCore *pCore = &((CBpmpsApp *) AfxGetApp())->m_MPSCore;

	// 取出要删除的组合名
	ItemIndex = this->m_ctrListCombineName.GetCurSel( );
	if ( ItemIndex == LB_ERR )
		return;
	this->m_ctrListCombineName.GetText (ItemIndex, CbName);

	// 从数据库中删除
	if (!pCore->DeleteSkuCombination(CbName, &ErrStr))
	{
		AfxMessageBox(ErrStr, MB_OK | MB_ICONERROR);
		return;
	}

	// 从界面上删除
	this->m_ctrListCombineName.DeleteString( ItemIndex );
	while ( this->m_ctrListSelected.DeleteString( 0 ) != LB_ERR )
	{
		;
	}
}
