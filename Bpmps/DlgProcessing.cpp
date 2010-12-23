// DlgProcessing.cpp : 实现文件
//

#include "stdafx.h"
#include "Bpmps.h"
#include "DlgProcessing.h"
#include "afxdialogex.h"


// CDlgProcessing 对话框

IMPLEMENT_DYNAMIC(CDlgProcessing, CDialogEx)

CDlgProcessing::CDlgProcessing(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgProcessing::IDD, pParent)
{

}

CDlgProcessing::~CDlgProcessing()
{
}

void CDlgProcessing::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgProcessing, CDialogEx)
END_MESSAGE_MAP()


// CDlgProcessing 消息处理程序
