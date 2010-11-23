#pragma once

#include "MyDatabase.h"

#define QUERY_CLASS_INVALID 0x0000
#define QUERY_CLASS_MPS     0x0001
#define QUERY_CLASS_TEST	0x1000

class CQueryFilter
{
public:
	CString SkuGroup;
	CTime   StartingDate;
};

////////////////////////////////////////////////////////

class CMPSCore : public CMyDatabase
{
public:

	CMPSCore(void);

	virtual ~CMPSCore(void);

	//BOOL QueryAndSave(const CQueryFilter * pQueryFilter);

	//BOOL QueryOpenInv(const CTime &Data, LPCTSTR lpszViewName);

	//BOOL QueryTest(const CQueryFilter * pQueryFilter);

	BOOL GetResultHeader(CArray<CString> & HeaderNames);

	BOOL QueryDirectionTableSku(void);

	BOOL SelectAllFromSkuTable(void)
	{
		return this->SelectAll(_T("sku_d"));
	};

	// 根据用户选择的条件，查询所有物品的7项预测结果
	BOOL QueryFinalResult(CTime StartingDate, CString FullIndex, CArray<CString> &ResultArray);

protected:
	BOOL QueryFinalResult_OpenInv(CTime StartingDate, CString FullIndex, CString &ResultStr);

	BOOL QueryFinalResult_OutstandingPO(CTime StartingDate, CString FullIndex, CString &ResultStr);

	BOOL QueryFinalResult_LTForecast( CTime StartingDate, CString FullIndex, CString &ResultStr );

	BOOL QueryFinalResult_SaleVSForecast( CTime StartingDate, CString FullIndex, CString &ResultStr );

protected:
	int  GetWorkdayPerMonth( int year, int month );

private:

	CQueryFilter m_QueryFilter;

	CArray<CString> m_ResultFieldsName;

private:
	// 用于QueryFinalResult，保存中间变量，避免重复计算
	CString m_ConditionWh;
	CString m_ConditionSku;
	int     m_LeadTime;  // in weeks
};
