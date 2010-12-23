#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgProgressing �Ի���

class CDlgProgressing : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProgressing)

public:
	CDlgProgressing(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProgressing();
	void SetRange(UINT nUpper);
	BOOL SetProgress(UINT nProgress);

// �Ի�������
	enum { IDD = IDD_PROGRESSING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	void UpdateText(UINT nPos, UINT nRangeUpper);

private:
	CProgressCtrl m_ProgressCtrl;
	CStatic m_StaticProgress;
	UINT m_nRangeUpper; // m_ProgressCtrl�ؼ���Range�ı��ݣ��ӿ��ٶȡ�
};
