#pragma once

#include "MyDatabase.h"

class CQueryFilter
{
public:
	CString SkuGroup;
	CTime   StartingDate;
	CTime   FirstWeekSale;
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

	// �����û�ѡ�����������ѯ������Ʒ��7��Ԥ����
	BOOL QueryFinalResult(
		CTime StartingDate, CTime Wk1, CString FullIndex, CArray<CString> &ResultArray
		);

public:
	// �����ݿ��н�һ���µ�SKU�����
	BOOL CreateNewSkuCombination(
		const CString &CbName, CString *pErrStr
		);

	// �����ݿ���ɾ��һ��SKU���
	BOOL DeleteSkuCombination(const CString &CbName, CString *pErrStr);

	// ��ȡSKU���������SKUCODE
	BOOL GetAllSkuCodesOfCombination(
		const CString &strName, CArray<CString> *pSkuCodes, CString *pErrStr
		);

	// ���һ��SKUCODE�������
	BOOL AddSkuCodeToCombination(
		const CString &CbName, const CString &SkuCode, CString *pErrStr
		);

	// �������ɾ��һ��SKUCODE
	BOOL RemoveSkuCodeFromCombination(
		const CString &CbName, const CString &SkuCode, CString *pErrStr
		);

protected:
	BOOL QueryFinalResult_OpenInv(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	BOOL QueryFinalResult_OutstandingPO(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	BOOL QueryFinalResult_LTForecast(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	BOOL QueryFinalResult_SaleVSForecast(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	BOOL QueryFinalResult_RecommendedOrderVolume(
		CTime Wk1ForSale, CString FullIndex,
		double OpenInv, double OutstandingPO, double LTForecast,
		CString &ResultStr
		);

	BOOL QueryFinalResult_ActionFlag(
		const CString &SaleVSForecast,
		const CString &RecommendedOrderVolume,
		CString &ResultStr
		);

	BOOL QueryFinalResult_LastWeekOrder(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	BOOL QueryFinalResult_RecommendedWithoutLastWeekOrder(
		double OpenInv,
		double OutstandingPO,
		double LastWeekOrder,
		double LTForecast,
		CString &ResultStr
		);

	BOOL QueryFinalResult_ProductionTag(
		double LastWeekOrder,
		double RecommendWithoutLastWeekOrder,
		CString &ResultStr
		);

	BOOL QueryFinalResult_Arrival(
		CString &ResultStr
		);

	BOOL QueryFinalResult_Sales(
		CTime StartingDate, CString FullIndex, CString &ResultStr
		);

	// ����һ���·��еĹ���������
	int  GetWorkdayPerMonth(int year, int month);

public: // TODO: change to protected functions
	// ����53������ÿ�ܵ�������
	BOOL GetSalesVolumeOfWeeks(CTime StartDate4Sales, CString FullIndex,
		CArray<double> &WeeksSaleVolume, CString *pErrStr
		);

	// �����һ�������ۼ�¼�����ں������
	unsigned GetWeeksAfterFirstSale(
		const CArray<double> &WeekSales // Result of GetSalesVolumeOfWeeks()
		);

	// ������ת�����ַ��������� 2010-1-31
	CString ConvertDateToString(const CTime &Date);

	// �������е�ֵ���
	double SumOf(const CArray<double> &Array);

	double StdDev(const CArray<double> &Array);

	// ��������������ʹ�������̬�ֲ�
	BOOL AdjustWeekSaleStd(
		const CArray<double> &SaleVolume, unsigned filter,
		CArray<double> &SaleVolumeAfterAdjust);

	// 04_1 ModelEngine: $GS
	double GetAdjustedAveWeeklySaleVol(const CArray<double> &SaleVolume);

	double GetROP(const CString &FullIndex);

	double GetROQ(const CString &FullIndex);

	double GetSafeStock(const CString &FullIndex);
	
	//
	// monthlyforecast_o
	// ��ȡ����Ʒ����ĳ���Ԥ����������
	// ˵�������۰����·�Ԥ������˸���ÿ���µĹ���������ƽ̯�������ݵ�ÿ��
	double GetDailyForecast(const CTime &date, const CString &FullIndex);

private:

	CQueryFilter m_QueryFilter;

	CArray<CString> m_ResultFieldsName;

	// Key CString,   like 2010-9-1
	// value double,  ����ÿ��Ԥ�������۶�
	CMap<CString,LPCTSTR,double,double> m_MonthlyForecastMap;

	// ����QueryFinalResult�������м�����������ظ�����
	CString m_ConditionWh;
	CString m_ConditionSku;
	double  m_arrival;
	double  m_AdjustedAveWeeklySale;
	int     m_LeadTime;  // in weeks
	double  m_ROP;
	double  m_ROQ;
	double  m_SafeStock;
};
