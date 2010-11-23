#pragma once
#include "afxwin.h"


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
private:
	CListBox m_ctrListAll;
	BOOL IsValidName( const CString & Name );
public:
	virtual BOOL OnInitDialog();

	BOOL InitList_AllSku( CString &ErrMsg );

	BOOL InitList_AllSkuCombine( CString &ErrMsg );
	CListBox m_ctrListCombineName;
	afx_msg void OnBnClickedSkucbCreate();
	CListBox m_ctrListSelected;
	CButton m_btnDelete;
	afx_msg void OnLbnSelchangeSkucbListSkuid();
	afx_msg void OnBnClickedSkucdAdd();
	CButton m_btnAdd;
	CButton m_btnRemove;
	afx_msg void OnBnClickedSkucdRemove();
	afx_msg void OnBnClickedSkucdDelete();
};
