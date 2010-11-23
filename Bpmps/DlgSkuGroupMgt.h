#pragma once


// CDlgSkuGroupMgt dialog

class CDlgSkuGroupMgt : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkuGroupMgt)

public:
	CDlgSkuGroupMgt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSkuGroupMgt();

// Dialog Data
	enum { IDD = IDD_SKU_GROUP_MGT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	RECT m_DefaultRect;
public:
	void ResetDimension(void);
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
};
