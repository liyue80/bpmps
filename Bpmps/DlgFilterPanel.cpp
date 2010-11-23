// DlgFilterPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgFilterPanel.h"
#include "MPSCore.h"

// CDlgFilterPanel dialog

IMPLEMENT_DYNAMIC(CDlgFilterPanel, CDialog)

CDlgFilterPanel::CDlgFilterPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFilterPanel::IDD, pParent)
{
	ZeroMemory(&this->m_DefaultRect, sizeof(RECT));
}

CDlgFilterPanel::~CDlgFilterPanel()
{
}

void CDlgFilterPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_CtrlDataTime);
	DDX_Control(pDX, IDC_SKU_GROUP_PICKER, m_cbSkuGroupPicker);
}


BEGIN_MESSAGE_MAP(CDlgFilterPanel, CDialog)
	ON_BN_CLICKED(ID_START_QUERY, &CDlgFilterPanel::OnBnClickedStartQuery)
	ON_BN_CLICKED(IDC_IMPOER_GROUP, &CDlgFilterPanel::OnBnClickedImportGroup)
END_MESSAGE_MAP()


// CDlgFilterPanel message handlers

void CDlgFilterPanel::OnBnClickedStartQuery()
{
	CTime SelTime;
	this->m_CtrlDataTime.GetTime(SelTime);

	// check user select date, must be Monday.
	if (SelTime.GetDayOfWeek() != 2)
	{
		::MessageBox(NULL,
			"Sorry, you selected date is not Monday.\nPlease try again.",
			"Warning", MB_OK | MB_ICONWARNING);
		return;
	}

	CQueryFilter *pFilter = new CQueryFilter;
	pFilter->StartingDate = SelTime;
	pFilter->SkuGroup = _T("<ALL>");

	// wParam - Reserved
	// lParam - Pointer to Query Filter
	AfxGetApp()->GetMainWnd()->SendMessage(WM_BTN_CLICK_QUERY, 0, (LPARAM)pFilter);
}

void CDlgFilterPanel::ResetDimension(void)
{
	this->MoveWindow(&this->m_DefaultRect, TRUE);
}

BOOL CDlgFilterPanel::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	BOOL bReturnValue = CDialog::Create(nIDTemplate, pParentWnd);

	this->GetWindowRect(&m_DefaultRect);

	int iScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);

	LONG cx = this->m_DefaultRect.right - this->m_DefaultRect.left;
	this->m_DefaultRect.right = iScreenWidth - ::GetSystemMetrics(SM_CXVSCROLL) - 20;
	this->m_DefaultRect.left = this->m_DefaultRect.right - cx;

	LONG cy = this->m_DefaultRect.bottom - this->m_DefaultRect.top;
	this->m_DefaultRect.top = 100;
	this->m_DefaultRect.bottom = this->m_DefaultRect.top + cy;

	this->InitSkuGroupPicker();

	return bReturnValue;
}

BOOL CDlgFilterPanel::InitSkuGroupPicker(void)
{
	//TODO: Load data from database

	// add default "<ALL>"
	this->m_cbSkuGroupPicker.SetCurSel(0);

	return TRUE;
}

void CDlgFilterPanel::OnBnClickedImportGroup()
{
	CFileDialog FileDlg(true, _T("*.*"), NULL,
		OFN_ENABLESIZING|OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), AfxGetApp()->GetMainWnd());

	if (FileDlg.DoModal() == IDCANCEL)
	{
		return;
	}

	CString PathName = FileDlg.GetPathName();

	CStdioFile File;

	if (!File.Open(PathName, CFile::modeRead))
	{
		AfxGetApp()->GetMainWnd()->MessageBox(
			_T("Open file error"), "Error", MB_OK|MB_ICONERROR);
		return;
	}

	CString FileLine;
	CBpmpsApp * pTheApp = NULL;
	pTheApp = (CBpmpsApp *) AfxGetApp();
	pTheApp->m_MPSCore.NonSelectQuery("truncate table fullindex_t");
	CString SqlStr;
	int InsertCount = 0;

	while ( File.ReadString(FileLine))
	{
		FileLine.Trim();
		if (FileLine.GetLength() <= 0)
			continue;

		SqlStr.Format("insert into fullindex_t (fullindex) values ('%s')", FileLine);
		if (pTheApp->m_MPSCore.NonSelectQuery(SqlStr))
		{
			InsertCount++;
		}
	}

	CString DispMsg;
	DispMsg.Format("Import %d records", InsertCount);
	AfxGetApp()->GetMainWnd()->MessageBox(DispMsg);
}
