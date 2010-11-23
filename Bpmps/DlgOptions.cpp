// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgOptions.h"


// CDlgOptions dialog

IMPLEMENT_DYNAMIC(CDlgOptions, CDialog)

CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptions::IDD, pParent)
{

}

CDlgOptions::~CDlgOptions()
{
}

void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOptions, CDialog)
END_MESSAGE_MAP()


// CDlgOptions message handlers
