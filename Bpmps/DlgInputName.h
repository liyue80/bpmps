#pragma once
#include "afxwin.h"


// CDlgInputName dialog

class CDlgInputName : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputName)

public:
	CDlgInputName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgInputName();

// Dialog Data
	enum { IDD = IDD_INPUT_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Name;
};
