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

	// 根据用户选择的条件，查询所有物品的7项预测结果
	BOOL QueryFinalResult(
		CTime StartingDate, CTime Wk1, CString FullIndex, CArray<CString> &ResultArray
		);

public:
	// 在数据库中建一个新的SKU组合名
	BOOL CreateNewSkuCombination(
		const CString &CbName, CString *pErrStr
		);

	// 从数据库中删除一个SKU组合
	BOOL DeleteSkuCombination(const CString &CbName, CString *pErrStr);

	// 获取SKU组合中所有SKUCODE
	BOOL GetAllSkuCodesOfCombination(
		const CString &strName, CArray<CString> *pSkuCodes, CString *pErrStr
		);

	// 添加一个SKUCODE到组合中
	BOOL AddSkuCodeToCombination(
		const CString &CbName, const CString &SkuCode, CString *pErrStr
		);

	// 从组合中删除一个SKUCODE
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

	// 计算一个月份中的工作日数量
	int  GetWorkdayPerMonth(int year, int month);

public: // TODO: change to protected functions
	// 计算53个星期每周的销售量
	BOOL GetSalesVolumeOfWeeks(CTime StartDate4Sales, CString FullIndex,
		CArray<double> &WeeksSaleVolume, CString *pErrStr
		);

	// 计算第一个有销售记录的星期后的周数
	unsigned GetWeeksAfterFirstSale(
		const CArray<double> &WeekSales // Result of GetSalesVolumeOfWeeks()
		);

	// 把日期转换成字符串，形如 2010-1-31
	CString ConvertDateToString(const CTime &Date);

	// 对数组中的值求和
	double SumOf(const CArray<double> &Array);

	double StdDev(const CArray<double> &Array);

	// 调整周销售量，使其符合正态分布
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
	// 获取该物品，在某天的预计销售数量
	// 说明，销售按照月份预估，因此根据每个月的工作日数，平摊销售数据到每日
	double GetDailyForecast(const CTime &date, const CString &FullIndex);

private:

	CQueryFilter m_QueryFilter;

	CArray<CString> m_ResultFieldsName;

	// Key CString,   like 2010-9-1
	// value double,  该月每日预估的销售额
	CMap<CString,LPCTSTR,double,double> m_MonthlyForecastMap;

	// 用于QueryFinalResult，保存中间变量，避免重复计算
	CString m_ConditionWh;
	CString m_ConditionSku;
	double  m_arrival;
	double  m_AdjustedAveWeeklySale;
	int     m_LeadTime;  // in weeks
	double  m_ROP;
	double  m_ROQ;
	double  m_SafeStock;
};
