#pragma once


// CDlgProcessing �Ի���

class CDlgProcessing : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcessing)

public:
	CDlgProcessing(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessing();

// �Ի�������
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
