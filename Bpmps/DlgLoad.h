#pragma once
#include "afxcmn.h"


// CDlgLoad �Ի���

class CDlgLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLoad)

public:
	CDlgLoad(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLoad();

// �Ի�������
	enum { IDD = IDD_FILE_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
protected:
	BOOL InitializeListCtrl(void);
};
