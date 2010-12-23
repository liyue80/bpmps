#pragma once
#include "afxcmn.h"


// CDlgLoad 对话框

class CDlgLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLoad)

public:
	CDlgLoad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLoad();

// 对话框数据
	enum { IDD = IDD_FILE_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
protected:
	BOOL InitializeListCtrl(void);
};
