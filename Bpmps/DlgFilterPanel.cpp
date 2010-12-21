// DlgFilterPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgFilterPanel.h"
#include "WorkThread.h"

// CDlgFilterPanel dialog

IMPLEMENT_DYNAMIC(CDlgFilterPanel, CDialog)

CDlgFilterPanel::CDlgFilterPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFilterPanel::IDD, pParent)
{

}

CDlgFilterPanel::~CDlgFilterPanel()
{
}

void CDlgFilterPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_CtrlDataTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_CtrlDateTime2);
}


BEGIN_MESSAGE_MAP(CDlgFilterPanel, CDialog)
	ON_BN_CLICKED(ID_START_QUERY, &CDlgFilterPanel::OnBnClickedStartQuery)
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

	LPMAINWORKTHREADPARAM lpThreadParam = new MAINWORKTHREADPARAM;
	lpThreadParam->_StartingDate = SelTime.GetTime();
	lpThreadParam->_FirstWeekSale = SaleWk1.GetTime();
	CWinThread *pWinThread = AfxBeginThread(
		(AFX_THREADPROC) MainWorkThreadFunc, (LPVOID) lpThreadParam,
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	pWinThread->m_bAutoDelete = TRUE;
	pWinThread->ResumeThread();

	OnOK();
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
