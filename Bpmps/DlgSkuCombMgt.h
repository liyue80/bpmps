#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgSkuCombMgt dialog

class CDlgSkuCombMgt : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkuCombMgt)

public:
	CDlgSkuCombMgt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSkuCombMgt();

// Dialog Data
	enum { IDD = IDD_SKU_COMB_MGT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	virtual BOOL OnInitDialog();

private:
	CListCtrl m_ListCtrl;
};
