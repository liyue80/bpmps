// WorkThread.cpp : Defines the thread
//

#include "stdafx.h"
#include "WorkThread.h"
#include "Bpmps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_WORK_THREAD_COUNT 1	// TODO: �޸�Ϊ�û������ò���

#define IS_EMPTY(xx) (((xx)==NULL)||(strlen(xx)==0))

// ���������ӹ����̵߳�����
CSemaphore gSubThreadCount(MAX_WORK_THREAD_COUNT, MAX_WORK_THREAD_COUNT);

// ���崫���ӹ����̵߳Ĳ���
typedef struct tag_SubWorkThreadParam
{
	__time64_t _StartingDate;
	__time64_t _FirstWeekSale;
	char       _ParentSkuCode[45];
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
		strcpy_s(pRecord->_ParentSkuCode, pParam->_ParentSkuCode);
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

	// �������ݿ�
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	// ׼������SKUCODE�ļ���
	// Map key: SkuCode
	// Map value: Sub SkuCode
	std::multimap <std::string, std::string> SkuCodeMap;
	if (bRet)
	{
		bRet = Core.SelectQuery(
			"select indexcode, mapping1, mapping2, mapping3, mapping4, mapping5, mapping6, mapping7 from skuinfo");
	}
	if (bRet)
	{
		while (row = Core.GetRecord(), row != NULL)
		{
			BOOL bIncludeSelf = FALSE;
			if (IS_EMPTY(row[0]))
				continue;
			for (int i=1; i<=7; i++)
			{
				if (IS_EMPTY(row[i]))
					continue;
				SkuCodeMap.insert(std::pair<std::string,std::string>(row[0],row[i]));
				if ( strcmp(row[0], row[i]) == 0 )
					bIncludeSelf = TRUE;
			}
			if (!bIncludeSelf)
				SkuCodeMap.insert(std::pair<std::string,std::string>(row[0],row[0]));
		}
		Core.FreeRecord();
	}

	// �������ͼ�¼��������������ʾ��UI��
	UINT nCount = 0;
	UINT nAmount = 0;
	nAmount = SkuCodeMap.size();

	if (bRet)
	{
		std::multimap<std::string,std::string>::iterator it;
		CString SQL;
		for ( it = SkuCodeMap.begin() ; bRet && it != SkuCodeMap.end(); it++ )
		{
			//////////////////////////////////////////////////////////////////////////
			// ˢ��UI�ϵĽ���
			//////////////////////////////////////////////////////////////////////////
			nCount++;
			AfxGetApp()->GetMainWnd()->PostMessageA(
				WM_USER_UI_UPDATE_PROCESS,
				(WPARAM) UPDATE_PROCESS,
				(LPARAM) ((nAmount << 16) | nCount));

			//////////////////////////////////////////////////////////////////////////
			SQL.Format("select DISTINCT(`jdewh`) from `openinv_o` where `jdeskucode`='%s'", it->second.c_str());
			bRet = Core.SelectQuery((LPCTSTR)SQL);
			if ( !bRet )
				break;
			while ( row = Core.GetRecord(), row != NULL)
			{
				if (IS_EMPTY(row[0]))
					continue;

				// �ȴ��ӹ����߳���Դ��������ͬʱ���������߳������ޣ�
				if (WaitForSingleObject((HANDLE)gSubThreadCount, INFINITE) != WAIT_OBJECT_0)
				{
					// �쳣����
					bRet = FALSE;
					break;
				}
				else
				{
					LPSUBWORKTHREADPARAM lpSubThreadParam = new SUBWORKTHREADPARAM;
					CWinThread *pWinThread = NULL;

					// ׼���ӹ����̵߳Ĳ���
					lpSubThreadParam->_StartingDate = pParam->_StartingDate;
					lpSubThreadParam->_FirstWeekSale = pParam->_FirstWeekSale;
					strcpy_s(lpSubThreadParam->_ParentSkuCode, it->first.c_str());
					strcpy_s(lpSubThreadParam->_SkuCode, it->second.c_str());
					strcpy_s(lpSubThreadParam->_WareHouse, row[0]);

					// ����һ���ӹ����߳�
					pWinThread = AfxBeginThread((AFX_THREADPROC)SubWorkThreadFunc,
						(LPVOID)lpSubThreadParam,
						THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
					pWinThread->m_bAutoDelete = TRUE;
					pWinThread->ResumeThread();
				}
			}
			Core.FreeRecord();
		}
	}

	AfxGetApp()->GetMainWnd()->PostMessageA(
		WM_USER_UI_UPDATE_PROCESS,
		(WPARAM) END_PROCESS,
		(LPARAM) ((nAmount << 16) | nCount));

	//TODO: ��ȴ����߳̽�����
	delete pParam;
	return 0;
}
