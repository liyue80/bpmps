// DlgInputName.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgInputName.h"


// CDlgInputName dialog

IMPLEMENT_DYNAMIC(CDlgInputName, CDialog)

CDlgInputName::CDlgInputName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputName::IDD, pParent)
	, m_Name(_T(""))
{

}

CDlgInputName::~CDlgInputName()
{
}

void CDlgInputName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
}


BEGIN_MESSAGE_MAP(CDlgInputName, CDialog)
END_MESSAGE_MAP()


// CDlgInputName message handlers
