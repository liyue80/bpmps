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
	void ResetDimension(void);

// Dialog Data
	enum { IDD = IDD_FILTER_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartQuery();
private:
	CDateTimeCtrl m_CtrlDataTime;
	RECT m_DefaultRect;
public:
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
protected:
	CComboBox m_cbSkuGroupPicker;
	BOOL InitSkuGroupPicker(void); // Add SKU Groups' name into ComboBox
public:
	afx_msg void OnBnClickedImportGroup();
	// The starting date of wk1 for sales
	CDateTimeCtrl m_CtrlDateTime2;
	afx_msg void OnBnClickedButton2();
};
