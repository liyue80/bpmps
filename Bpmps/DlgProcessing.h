#pragma once


// CDlgProcessing 对话框

class CDlgProcessing : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcessing)

public:
	CDlgProcessing(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessing();

// 对话框数据
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
