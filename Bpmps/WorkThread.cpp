// WorkThread.cpp : Defines the thread
//

#include "stdafx.h"
#include "WorkThread.h"
#include "Bpmps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_WORK_THREAD_COUNT 1	// TODO: �޸�Ϊ�û������ò���

// ���������ӹ����̵߳�����
CSemaphore gSubThreadCount(MAX_WORK_THREAD_COUNT, MAX_WORK_THREAD_COUNT);

// ���崫���ӹ����̵߳Ĳ���
typedef struct tag_SubWorkThreadParam
{
	__time64_t _StartingDate;
	__time64_t _FirstWeekSale;
	char       _SkuCode[45];
	char       _WareHouse[45];
} SUBWORKTHREADPARAM, *LPSUBWORKTHREADPARAM;

/////
// �ӹ����߳�
// ���ܣ�����һ�� skucode �Ľ��
UINT AFX_CDECL SubWorkThreadFunc(LPVOID *lpParam)
{
	ASSERT(lpParam != NULL);

	CMPSCore Core;
	BOOL bRet = FALSE;
	LPSUBWORKTHREADPARAM pParam = (LPSUBWORKTHREADPARAM) lpParam;
	CString OpenInvFirst;
	CString OutstandingPO;

	TRACE1("SubWorkThreadFunc begin - %s\n", pParam->_SkuCode);

	// �������ݿ�
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	if (bRet)
	{
		bRet = Core.QueryGetFirstOpenInv(CTime(pParam->_StartingDate),
			CString(pParam->_SkuCode), CString(pParam->_WareHouse),
			OpenInvFirst);
	}

	if (bRet)
	{
		bRet = Core.QueryGetOutstandingPO(CTime(pParam->_StartingDate),
			CString(pParam->_SkuCode), CString(pParam->_WareHouse),
			OutstandingPO);
		if (bRet != TRUE)
		{
			OutstandingPO = "-";
			bRet = TRUE;
		}
	}

	if (bRet)
	{
		APPENDING_RECORD * pRecord = new APPENDING_RECORD;
		strcpy_s(pRecord->_SkuCode, pParam->_SkuCode);
		strcpy_s(pRecord->_Warehouse, pParam->_WareHouse);
		strcpy_s(pRecord->_OpenInvFirst, (LPCTSTR)OpenInvFirst);
		strcpy_s(pRecord->_OutstandingPO, (LPCTSTR)OutstandingPO);

		AfxGetApp()->GetMainWnd()->SendMessage(
			WM_USER_UI_APPEND_RECORD, (WPARAM)0, (LPARAM)(LPVOID)pRecord);
	}

	// ����ʱ�ͷ�һ�����߳�
	gSubThreadCount.Unlock(1);

	delete pParam;

	TRACE0("SubWorkThreadFunc end\n");
	return TRUE;
}

/////
// �������߳�
// ���ܣ������ݿ�� skucode_t �ж���Ҫ��ѯ�Ļ��ţ�������ӹ����߳̽��м���
UINT AFX_CDECL MainWorkThreadFunc(LPVOID *lpParam)
{
	ASSERT(lpParam != NULL);

	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;
	LPMAINWORKTHREADPARAM pParam = (LPMAINWORKTHREADPARAM) lpParam;
	CMPSCore Core;

	AfxGetApp()->GetMainWnd()->PostMessageA(
		WM_USER_UI_UPDATE_PROCESS,
		(WPARAM) RESET_PROCESS,
		(LPARAM) 0);

	// �������ͼ�¼��������������ʾ��UI��
	UINT nCount = 0;
	UINT nAmount = 0;

	// �������ݿ�
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	// ��ȡ��¼������������ʾ��UI��
	if (bRet)
	{
		if (Core.SelectQuery("SELECT COUNT(*) FROM `skucode_t`"))
		{
			row = Core.GetRecord();
			if (row != NULL && row[0] != NULL)
				nAmount = atoi(row[0]);
			Core.FreeRecord();
		}
	}

	if (bRet)
	{
		// ��ѯ
		bRet = Core.SelectQuery(
			"SELECT `skucode`,`jdewh` from `skucode_t` order by skucode");
	}

	if (bRet)
	{
		while (row = Core.GetRecord(), row != NULL)
		{
			LPSUBWORKTHREADPARAM lpSubThreadParam = new SUBWORKTHREADPARAM;
			CWinThread *pWinThread = NULL;

			// ˢ��UI�ϵĽ���
			nCount++;
			AfxGetApp()->GetMainWnd()->PostMessageA(
				WM_USER_UI_UPDATE_PROCESS,
				(WPARAM) UPDATE_PROCESS,
				(LPARAM) ((nAmount << 16) | nCount));

			if ( NULL == row[0] || strlen(row[0]) == 0 )
				continue;

			// ׼���ӹ����̵߳Ĳ���
			lpSubThreadParam->_StartingDate = pParam->_StartingDate;
			lpSubThreadParam->_FirstWeekSale = pParam->_FirstWeekSale;
			strcpy_s(lpSubThreadParam->_SkuCode, row[0]);
			if ( row[1] != NULL && strlen(row[1]) != 0 )
				strcpy_s(lpSubThreadParam->_WareHouse, row[1]);

			// �ȴ��ӹ����߳���Դ��������ͬʱ���������߳������ޣ�
			if (WaitForSingleObject((HANDLE)gSubThreadCount, INFINITE) != WAIT_OBJECT_0)
			{
				// �쳣����
				delete lpSubThreadParam;
				bRet = FALSE;
				break;
			}
			
			// ����һ���ӹ����߳�
			pWinThread = AfxBeginThread((AFX_THREADPROC)SubWorkThreadFunc,
				(LPVOID)lpSubThreadParam,
				THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pWinThread->m_bAutoDelete = TRUE;
			pWinThread->ResumeThread();
		}

		Core.FreeRecord();
	}

	AfxGetApp()->GetMainWnd()->PostMessageA(
		WM_USER_UI_UPDATE_PROCESS,
		(WPARAM) END_PROCESS,
		(LPARAM) ((nAmount << 16) | nCount));

	delete pParam;
	return 0;
}
