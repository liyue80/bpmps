// WorkThread.cpp : Defines the thread
//

#include "stdafx.h"
#include "WorkThread.h"
#include "Bpmps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_WORK_THREAD_COUNT 3	// TODO: 修改为用户可配置参数

// 用于限制子工作线程的数量
CSemaphore gSubThreadCount(MAX_WORK_THREAD_COUNT, MAX_WORK_THREAD_COUNT);

// 定义传给子工作线程的参数
typedef struct tag_SubWorkThreadParam
{
	__time64_t _StartingDate;
	__time64_t _FirstWeekSale;
	char       _SkuCode[45];
	char       _WareHouse[45];
} SUBWORKTHREADPARAM, *LPSUBWORKTHREADPARAM;

/////
// 子工作线程
// 功能：计算一个 skucode 的结果
UINT AFX_CDECL SubWorkThreadFunc(LPVOID *lpParam)
{
	ASSERT(lpParam != NULL);

	CMPSCore Core;
	BOOL bRet = FALSE;
	LPSUBWORKTHREADPARAM pParam = (LPSUBWORKTHREADPARAM) lpParam;
	CString OpenInvFirst;

	TRACE1("SubWorkThreadFunc begin - %s\n", pParam->_SkuCode);

	// 连接数据库
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	if (bRet)
	{
		bRet = Core.GetFirstOpenInv(CTime(pParam->_StartingDate),
			CString(pParam->_SkuCode), CString(pParam->_WareHouse),
			OpenInvFirst);
	}

	if (bRet)
	{
		APPENDING_RECORD * pRecord = new APPENDING_RECORD;
		strcpy_s(pRecord->_SkuCode, pParam->_SkuCode);
		strcpy_s(pRecord->_Warehouse, pParam->_WareHouse);
		strcpy_s(pRecord->_OpenInvFirst, (LPCTSTR)OpenInvFirst);

		AfxGetApp()->GetMainWnd()->SendMessage(
			WM_USER_UI_APPEND_RECORD, (WPARAM)0, (LPARAM)(LPVOID)pRecord);
	}

	// 结束时释放一个子线程
	gSubThreadCount.Unlock(1);

	delete pParam;

	TRACE0("SubWorkThreadFunc end\n");
	return TRUE;
}

/////
// 主工作线程
// 功能：从数据库表 skucode_t 中读出要查询的货号，分配给子工作线程进行计算
UINT AFX_CDECL MainWorkThreadFunc(LPVOID *lpParam)
{
	ASSERT(lpParam != NULL);

	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;
	LPMAINWORKTHREADPARAM pParam = (LPMAINWORKTHREADPARAM) lpParam;
	CMPSCore Core;

	// 连接数据库
	bRet = Core.ConnectDB("127.0.0.1", "root", "", "test", 0, NULL, 0);

	if (bRet)
	{
		// 查询
		bRet = Core.SelectQuery(
			"SELECT `skucode`,`jdewh` from `skucode_t` order by skucode");
	}

	if (bRet)
	{
		while (row = Core.GetRecord(), row != NULL)
		{
			LPSUBWORKTHREADPARAM lpSubThreadParam = new SUBWORKTHREADPARAM;
			CWinThread *pWinThread = NULL;

			if ( NULL == row[0] || strlen(row[0]) == 0 )
				continue;

			// 准备子工作线程的参数
			lpSubThreadParam->_StartingDate = pParam->_StartingDate;
			lpSubThreadParam->_FirstWeekSale = pParam->_FirstWeekSale;
			strcpy_s(lpSubThreadParam->_SkuCode, row[0]);
			if ( row[1] != NULL && strlen(row[1]) != 0 )
				strcpy_s(lpSubThreadParam->_WareHouse, row[1]);

			// 等待子工作线程资源（设置了同时工作的子线程数上限）
			if (WaitForSingleObject((HANDLE)gSubThreadCount, INFINITE) != WAIT_OBJECT_0)
			{
				// 异常结束
				delete lpSubThreadParam;
				bRet = FALSE;
				break;
			}
			
			// 启动一个子工作线程
			pWinThread = AfxBeginThread((AFX_THREADPROC)SubWorkThreadFunc,
				(LPVOID)lpSubThreadParam,
				THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pWinThread->m_bAutoDelete = TRUE;
			pWinThread->ResumeThread();
		}

		Core.FreeRecord();
	}

	delete pParam;
	return 0;
}
