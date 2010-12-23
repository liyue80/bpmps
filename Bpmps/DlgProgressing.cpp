// DlgProgressing.cpp : 实现文件
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgProgressing.h"
#include "afxdialogex.h"


// CDlgProgressing 对话框

IMPLEMENT_DYNAMIC(CDlgProgressing, CDialogEx)

CDlgProgressing::CDlgProgressing(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgProgressing::IDD, pParent)
	, m_nRangeUpper(0)
{

}

CDlgProgressing::~CDlgProgressing()
{
}

void CDlgProgressing::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_StaticProgress);
}


BEGIN_MESSAGE_MAP(CDlgProgressing, CDialogEx)
END_MESSAGE_MAP()


// CDlgProgressing 消息处理程序

void CDlgProgressing::SetRange(UINT nUpper)
{
	if (m_nRangeUpper != nUpper)
	{
		m_nRangeUpper = nUpper;
		m_ProgressCtrl.SetRange32(0, (int)nUpper);
		m_ProgressCtrl.SetPos(0);
		UpdateText(0, nUpper);
	}
}

BOOL CDlgProgressing::SetProgress(UINT nProgress)
{
#ifdef _DEBUG
	int iRangeLower = 0;
	int iRangeUpper = 0;
	m_ProgressCtrl.GetRange(iRangeLower, iRangeUpper);
	ASSERT(iRangeLower >= 0 && iRangeUpper >= 0);
	ASSERT(nProgress >= (UINT)iRangeLower);
	ASSERT(nProgress <= (UINT)iRangeUpper);
#endif

	UpdateText(nProgress, m_nRangeUpper);
	m_ProgressCtrl.SetPos((int)nProgress);
	return 0;
}


void CDlgProgressing::UpdateText(UINT nPos, UINT nRangeUpper)
{
	CString Text;
	Text.Format("%u/%u", nPos, nRangeUpper);
	m_StaticProgress.SetWindowTextA(Text);
}
