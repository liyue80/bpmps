#pragma once
#include "afxdtctl.h"
#include "afxwin.h"


// CDlgFilterPanel dialog

class CDlgFilterPanel : public CDialog
{
	DECLARE_DYNAMIC(CDlgFilterPanel)

public:
	CDlgFilterPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFilterPanel();

	// Dialog Data
	enum { IDD = IDD_FILTER_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedStartQuery();
	DECLARE_MESSAGE_MAP()

private:
	// The starting date of wk1 for sales
	CDateTimeCtrl m_CtrlDateTime2;
	CDateTimeCtrl m_CtrlDataTime;
};
