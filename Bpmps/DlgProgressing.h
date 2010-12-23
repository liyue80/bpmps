#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgProgressing 对话框

class CDlgProgressing : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgressing)

public:
	CDlgProgressing(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProgressing();
	void SetRange(UINT nUpper);
	BOOL SetProgress(UINT nProgress);

// 对话框数据
	enum { IDD = IDD_PROGRESSING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	void UpdateText(UINT nPos, UINT nRangeUpper);

private:
	CProgressCtrl m_ProgressCtrl;
	CStatic m_StaticProgress;
	UINT m_nRangeUpper; // m_ProgressCtrl控件中Range的备份，加快速度。
};
