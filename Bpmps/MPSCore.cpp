#include "StdAfx.h"
#include "MPSCore.h"

CMPSCore::CMPSCore(void) : CMyDatabase()
{

}

CMPSCore::~CMPSCore(void)
{
}


#if 0
BOOL CMPSCore::QueryTest(const CQueryFilter * pQueryFilter)
{
	ASSERT(pQueryFilter);

	if (!this->SelectAll("openinv_o"))
	{
		return FALSE;
	}

	this->m_QueryFilter.SkuGroup = pQueryFilter->SkuGroup;
	this->m_QueryFilter.StartingDate = pQueryFilter->StartingDate;

	int nFieldNum = this->GetFieldNum();

	for (int i = 0; i < nFieldNum; i++)
	{
		this->m_ResultFieldsName.Add(CString(this->GetFieldName(i)));
	}

	return TRUE;
}
#endif

BOOL CMPSCore::GetResultHeader(CArray<CString> & HeaderNames)
{
	HeaderNames.RemoveAll();


	return TRUE;
}

BOOL CMPSCore::QueryFinalResult( CTime StartingDate, CString FullIndex, CArray<CString> &ResultArray )
{
	CString ResultStr;

	ResultArray.RemoveAll();

	if (!QueryFinalResult_OpenInv(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	if (!QueryFinalResult_OutstandingPO(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	if (!QueryFinalResult_LTForecast(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	if (!QueryFinalResult_SaleVSForecast(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_OpenInv( CTime StartingDate, CString FullIndex, CString &ResultStr )
{
	CString SqlStr;
	CString Time1, Time2;
	CTime   EndTime = StartingDate + CTimeSpan(7, 0, 0, 0);

	Time1.Format("%d-%d-%d", StartingDate.GetYear(), StartingDate.GetMonth(), StartingDate.GetDay());
	Time2.Format("%d-%d-%d", EndTime.GetYear(), EndTime.GetMonth(), EndTime.GetDay());

	CString WarehouseDesc;
	CString SkuIndex;

	if (!AfxExtractSubString(WarehouseDesc, FullIndex, 1, '_'))
		return FALSE;
	if (!AfxExtractSubString(SkuIndex, FullIndex, 2, '_'))
		return FALSE;

	// 仓库名
	MYSQL_ROW Row = NULL;
	CArray <CString> WarehouseArray;
	SqlStr.Format("select jdewh from warehouse_d where description='%s'", WarehouseDesc);
	if (!this->SelectQuery(SqlStr))
		return FALSE;
	while ((Row = this->GetRecord()) != NULL)
	{
		WarehouseArray.Add(CString((LPCTSTR)Row[0]).Trim());
	}
	this->FreeRecord();
	if (WarehouseArray.GetSize() == 0)
		return FALSE;

	// SKU编号
	CArray <CString> SkuArray;
	SqlStr.Format("select jdeskucode from sku_d where newindex='%s'", SkuIndex);
	if (!this->SelectQuery(SqlStr))
		return FALSE;
	while ((Row = this->GetRecord()) != NULL)
	{
		SkuArray.Add(CString((LPCTSTR)Row[0]).Trim());
	}
	this->FreeRecord();
	if (SkuArray.GetSize() == 0)
		return FALSE;

	// OpenInv SUM
	ULONG OpenInvSum = 0;
	CString ConditionWh;
	CString ConditionSku;

	for (int i = 0; i < WarehouseArray.GetSize(); i++)
	{
		if (0 == i)
			ConditionWh.Format("(jdewh='%s')", WarehouseArray.GetAt(i));
		else
			ConditionWh.AppendFormat(" or (jdewh='%s')", WarehouseArray.GetAt(i));
	}

	for (int i = 0; i < SkuArray.GetSize(); i++)
	{
		if (0 == i)
			ConditionSku.Format("(jdeskucode='%s')", SkuArray.GetAt(i));
		else
			ConditionSku.AppendFormat(" or (jdeskucode='%s')", SkuArray.GetAt(i));
	}

	// backup conditions
	this->m_ConditionWh = ConditionWh;
	this->m_ConditionSku = ConditionSku;

	SqlStr.Format("select sum(skuvolume) from openinv_o "
			"where (%s) and (%s) and ((opendate>= '%s') and (opendate<'%s'))",
			(LPCTSTR) ConditionWh,
			(LPCTSTR) ConditionSku,
			(LPCTSTR) Time1,
			(LPCTSTR) Time2
			);

	if (!this->SelectQuery(SqlStr))
		return FALSE;
	Row = this->GetRecord();

	if (!Row)
	{
		this->FreeRecord();
		return FALSE;
	}
	ResultStr = (LPCTSTR) Row[0];
	this->FreeRecord();

	return TRUE;	
}

BOOL CMPSCore::QueryFinalResult_OutstandingPO( CTime StartingDate, CString FullIndex, CString &ResultStr )
{
	// Get LeadTime
	int iLeadDays = 0;
	{
		CString SqlStr;
		SqlStr.Format("SELECT leadtime FROM modelengine_o where fullindex='%s'", FullIndex);

		this->SelectQuery(SqlStr);

		MYSQL_ROW Row = this->GetRecord();
		if (!Row)
		{
			this->FreeRecord();
			return FALSE;
		}

		m_LeadTime = atoi(Row[0]);
		this->FreeRecord();

		if (m_LeadTime < 1)
		{
			TRACE2("Invalid LeadTime %s, %d\n", FullIndex, m_LeadTime);
			return FALSE;
		}

		iLeadDays = 7 * m_LeadTime - 7;
	}

	ASSERT(this->m_ConditionSku.GetLength() != 0);
	ASSERT(this->m_ConditionWh.GetLength() != 0);

	{
		CTime OrderDate = StartingDate - CTimeSpan(iLeadDays, 0, 0, 0);
		CString Time1, Time2;
		CString SqlStr;

		Time1.Format("%d-%d-%d", OrderDate.GetYear(), OrderDate.GetMonth(), OrderDate.GetDay());
		Time2.Format("%d-%d-%d", StartingDate.GetYear(), StartingDate.GetMonth(), StartingDate.GetDay());

		SqlStr.Format("Select sum(jdevolume) from plannedarrival_o "
			"where (%s) and (%s) and jdeorderdate>='%s' and jdeorderdate<='%s'",
			(LPCTSTR)this->m_ConditionWh,
			(LPCTSTR)this->m_ConditionSku,
			(LPCTSTR)Time1,
			(LPCTSTR)Time2);

		if (!this->SelectQuery(SqlStr))
			return FALSE;
		MYSQL_ROW Row = this->GetRecord();
		if (!Row)
		{
			this->FreeRecord();
			return FALSE;
		}
		ResultStr = (LPCTSTR) Row[0];
		this->FreeRecord();
	}

	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_LTForecast( CTime StartingDate, CString FullIndex, CString &ResultStr )
{
	CMap<CString,LPCTSTR,double,double> MonthlyForecastMap;
	int iLeadTimeDay = (m_LeadTime - 1) * 7;
	double Forecast = 0.0f;

	ASSERT(m_LeadTime >= 1);

	for (int i = 0; i < iLeadTimeDay; i++)
	{
		CString Month;
		double  DailyForecast = 0;

		if (StartingDate.GetDayOfWeek() == 1 || StartingDate.GetDayOfWeek() == 7)
		{
			StartingDate = StartingDate + CTimeSpan(1, 0, 0, 0);
			continue;
		}

		Month.Format("%d-%d-1", StartingDate.GetYear(), StartingDate.GetMonth());

		if (!MonthlyForecastMap.Lookup(Month, DailyForecast))
		{
			CString SqlStr;
			//CString Wh, Loc;

			//AfxExtractSubString(Wh, FullIndex, 1, '_');
			//AfxExtractSubString(Loc, Wh, 0, '-');

			SqlStr.Format("SELECT forecastvolume FROM monthlyforecast_o"
				" where fullindex like '%%%s%%' and forecastmonth='%s'",
				(LPCTSTR)FullIndex,
				(LPCTSTR)Month);

			this->SelectQuery(SqlStr);

			MYSQL_ROW Row = this->GetRecord();
			if (Row == NULL)
			{
				DailyForecast = 0;
			}
			else
			{
				DailyForecast = atof((LPCTSTR) Row[0]);
				DailyForecast = DailyForecast / GetWorkdayPerMonth(StartingDate.GetYear(), StartingDate.GetMonth());
			}
			MonthlyForecastMap.SetAt(Month, DailyForecast);
			this->FreeRecord();
		}

		StartingDate = StartingDate + CTimeSpan(1, 0, 0, 0);

		Forecast += DailyForecast;
		//TRACE2("%f => %f\n", DailyForecast, Forecast);
	}

	ResultStr.Format("%0.3f", Forecast);
	return TRUE;
}

int CMPSCore::GetWorkdayPerMonth( int year, int month )
{
	int days[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int ret = 20;

	if ( month == 2 && year % 4 != 0 )
	{
		return ret;	// 非闰2月份正好整4周
	}

	for (int i = 28; i < days[month - 1]; i++)
	{
		CTime time(year, month, i + 1, 0, 0, 0);
		int tmp = time.GetDayOfWeek();
		if (tmp != 1 && tmp != 7)
		{
			ret++;
		}
	}

	return ret;
}

BOOL CMPSCore::QueryFinalResult_SaleVSForecast( CTime StartingDate, CString FullIndex, CString &ResultStr )
{
	static const int RollingMonths = 3;

	// 向前退3个月 (RollingMonths)
	int month = StartingDate.GetMonth() - RollingMonths;
	int year = StartingDate.GetYear();
	while (month < 1)
	{
		month += 12;
		year--;
	}

	//////////////////////////////////
	CString SqlStr;
	double ForecastAverage = 0.0f;
	SqlStr.Format("SELECT sum(forecastvolume) FROM monthlyforecast_o "
		"where fullindex like '%%%s%%' "
		"and forecastmonth>='%d-%d-1' and forecastmonth<'%d-%d-1'",
		(LPCTSTR) FullIndex,
		year, month, StartingDate.GetYear(), StartingDate.GetMonth()
		);
	if (!this->SelectQuery(SqlStr))
		return FALSE;
	MYSQL_ROW Row = this->GetRecord();
	if (Row == NULL || Row[0] == NULL)
	{
		ResultStr = "No Forecast";
		this->FreeRecord();
		return TRUE;
	}
	ForecastAverage = atof(Row[0]) / RollingMonths;
	this->FreeRecord();

	////////////////////////////////////
	double SalesAverage = 0.0f;
	SqlStr.Format("SELECT sum(salesvolume) from monthlysales_o "
		"where fullindex like '%%%s%%' "
		"and salesmonth>='%d-%d-1' and salesmonth<'%d-%d-1'",
		(LPCTSTR) FullIndex,
		year, month, StartingDate.GetYear(), StartingDate.GetMonth()
		);

	if (!this->SelectQuery(SqlStr))
		return FALSE;

	Row = this->GetRecord();
	if (Row != NULL && Row[0] != NULL)
	{
		SalesAverage = atof(Row[0]) / RollingMonths;
	}
	this->FreeRecord();

	double legend = SalesAverage / ForecastAverage - 1.0;
	//ResultStr.Format("%f", (SalesAverage - ForecastAverage) / ForecastAverage);

	if ( legend < -0.25f )
		ResultStr = "Undersell";
	else if ( legend > 0.25f )
		ResultStr = "Oversell";
	else
		ResultStr = "Ok";

	return TRUE;
}
