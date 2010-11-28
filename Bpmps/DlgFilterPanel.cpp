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
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_CtrlDateTime2);
}


BEGIN_MESSAGE_MAP(CDlgFilterPanel, CDialog)
	ON_BN_CLICKED(ID_START_QUERY, &CDlgFilterPanel::OnBnClickedStartQuery)
	ON_BN_CLICKED(IDC_IMPOER_GROUP, &CDlgFilterPanel::OnBnClickedImportGroup)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgFilterPanel::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgFilterPanel message handlers

void CDlgFilterPanel::OnBnClickedStartQuery()
{
	// Get CTime from 1st DataTime Control on the dialog
	CTime SelTime;
	this->m_CtrlDataTime.GetTime(SelTime);

	// Get CTime from 2nd DataTime Control on the dialog
	CTime SaleWk1;
	this->m_CtrlDateTime2.GetTime(SaleWk1);

	// Check user select date, must be Monday
	if ( (SelTime.GetDayOfWeek() != 2) || (SaleWk1.GetDayOfWeek() != 2) )
	{
		::MessageBox(NULL,
			"Sorry, you selected date is not Monday.\nPlease try again.",
			"Warning", MB_OK | MB_ICONWARNING);
		return;
	}

	CQueryFilter *pFilter = new CQueryFilter;
	pFilter->StartingDate = SelTime;
	pFilter->FirstWeekSale = SaleWk1;
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

void CDlgFilterPanel::OnBnClickedButton2()
{
	CArray<double> volumes;
	CString ErrStr;
	CBpmpsApp * pTheApp = (CBpmpsApp *) AfxGetApp();
	CTime SelTime4Sale(2009, 3, 30, 0, 0, 0);

	BOOL ret = pTheApp->m_MPSCore.GetSalesVolumeOfWeeks(SelTime4Sale,
		"M_DL-MD_INDEX2117", volumes, &ErrStr);

	if (!ret)
	{
		AfxMessageBox(ErrStr);
		return;
	}

#if 0
	CArray<double> AdjustedVolumes;
	pTheApp->m_MPSCore.AdjustWeekSaleStd(volumes, 2, AdjustedVolumes);

	unsigned WeeksNumAfterFirstSale
		= pTheApp->m_MPSCore.GetWeeksAfterFirstSale(volumes);

	if (WeeksNumAfterFirstSale < 25 && WeeksNumAfterFirstSale != 0)
	{
		double SumHalf = 0;

		for (int i=27; i<52; i++)
			SumHalf += AdjustedVolumes.GetAt(i);

		(SumHalf/(52-27) + pTheApp->m_MPSCore.SumOf(AdjustedVolumes) / WeeksNumAfterFirstSale) / 2;
	}
	else if (WeeksNumAfterFirstSale == 0)
	{
		0;
	}
	else
	{
		pTheApp->m_MPSCore.SumOf(AdjustedVolumes) / WeeksNumAfterFirstSale;
	}
#endif

	double a = pTheApp->m_MPSCore.GetAdjustedAveWeeklySaleVol(volumes);
	pTheApp = NULL;
}


BOOL CDlgFilterPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  便于调试，初始化DateTimePicker为特殊的日期
	CTime t1(2010, 7, 19, 0, 0, 0);
	CTime t2(2009, 3, 30, 0, 0, 0);
	this->m_CtrlDataTime.SetTime(&t1);
	this->m_CtrlDateTime2.SetTime(&t2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
