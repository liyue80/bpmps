// DlgSkuGroupMgt.cpp : implementation file
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgSkuGroupMgt.h"


// CDlgSkuGroupMgt dialog

IMPLEMENT_DYNAMIC(CDlgSkuGroupMgt, CDialog)

CDlgSkuGroupMgt::CDlgSkuGroupMgt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkuGroupMgt::IDD, pParent)
{
	ZeroMemory(&this->m_DefaultRect, sizeof(RECT));
}

CDlgSkuGroupMgt::~CDlgSkuGroupMgt()
{
}

void CDlgSkuGroupMgt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSkuGroupMgt, CDialog)
END_MESSAGE_MAP()


// CDlgSkuGroupMgt message handlers
void CDlgSkuGroupMgt::ResetDimension(void)
{
	this->MoveWindow(&this->m_DefaultRect, TRUE);
}

BOOL CDlgSkuGroupMgt::Create(UINT nIDTemplate, CWnd* pParentWnd)
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

	return bReturnValue;
}
