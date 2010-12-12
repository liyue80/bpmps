#include "StdAfx.h"
#include "MPSCore.h"
#include "math.h"

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

BOOL CMPSCore::QueryFinalResult( CTime StartingDate, CTime Wk1, CString FullIndex, CArray<CString> &ResultArray )
{
	CString ResultStr;

	ResultArray.RemoveAll();
	this->m_MonthlyForecastMap.RemoveAll();

	m_ROP = this->GetROP(FullIndex);
	m_ROQ = this->GetROQ(FullIndex);
	m_SafeStock = this->GetSafeStock(FullIndex);

	if (!QueryFinalResult_OpenInv(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);
	double OpenInv = atof(ResultStr);

	if (!QueryFinalResult_OutstandingPO(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);
	double OutstandingPO = atof(ResultStr);

	if (!QueryFinalResult_LTForecast(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);
	double LTForecast = atof(ResultStr);

	if (!QueryFinalResult_SaleVSForecast(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);
	CString SaleVSForecast = ResultStr;

	CTime SelTime4Sale(2009, 3, 30, 0, 0, 0); //TODO: 从界面上取得日期
	if (!QueryFinalResult_RecommendedOrderVolume(
		SelTime4Sale, FullIndex, OpenInv, OutstandingPO, LTForecast, ResultStr))
	{
		return FALSE;
	}
	ResultArray.Add(ResultStr);
	CString RecommendedOrderVolume = ResultStr;

	if (!QueryFinalResult_ActionFlag(SaleVSForecast, RecommendedOrderVolume, ResultStr))
	{
		return FALSE;
	}
	ResultArray.Add(ResultStr);

	///////////////////////////////
	if (!QueryFinalResult_LastWeekOrder(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);
	double LastWeekOrder = atof(ResultStr);

	///////////////////////////////
	if (!QueryFinalResult_RecommendedWithoutLastWeekOrder(
		OpenInv, OutstandingPO, LastWeekOrder, LTForecast, ResultStr))
	{
		return FALSE;
	}
	ResultArray.Add(ResultStr);
	double RecommendWithoutLastWeekOrder = atof(ResultStr);

	///////////////////////////////
	if (!QueryFinalResult_ProductionTag(LastWeekOrder, RecommendWithoutLastWeekOrder, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	//////////////////////////////
	if (!QueryFinalResult_Arrival(ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	//////////////////////////////
	if (!QueryFinalResult_Sales(StartingDate, FullIndex, ResultStr))
		return FALSE;
	ResultArray.Add(ResultStr);

	return TRUE;
}

/////
// 从数据库表格 openinv_o 中累加符合条件的库存
//     Warehouse条件可以为空，则累加所有仓库的库存
//
BOOL CMPSCore::QueryGetFirstOpenInv(
	/* IN */  const CTime & StartingDate,
	/* IN */  const CString & SkuCode,
	/* IN */  const CString & Warehouse,
	/* OUT */ CString & Volume)
{
	// SQL语句中可能出现的条件
	CString CondSkuCode, CondWarehouse, CondTime;
	CString SQL;
	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;

	// 条件时间 CondTime
	CTime EndTime = StartingDate + CTimeSpan(7, 0, 0, 0);
	CondTime.Format("((`opendate`>= '%s') and (`opendate`<'%s'))",
		ConvertDateToString(StartingDate), ConvertDateToString(EndTime));

	// 条件 CondSkuCode
	ASSERT(SkuCode.GetLength() != 0);
	if (SkuCode.GetLength() <= 0)
		return FALSE;
	CondSkuCode.Format("(`jdeskucode`='%s')", (LPCTSTR)SkuCode);

	// 条件 CondWarehouse
	if (Warehouse.GetLength() > 0)
		CondWarehouse.Format("(`jdewh`='%s')", (LPCTSTR)Warehouse);

	// 组成完整的SQL语句
	SQL.Format("SELECT SUM(`skuvolume`) from `openinv_o` where %s and %s",
		CondTime, CondSkuCode);
	if (CondWarehouse.GetLength() > 0)
	{
		SQL.Append(" and ");
		SQL.Append(CondWarehouse);
	}

	// 执行SQL
	bRet = SelectQuery(SQL);
	if (bRet)
	{
		row = GetRecord();
		if ( !row || !row[0] )
			Volume = "0";
		else
			Volume.Format("%s", row[0]);
	}

	FreeRecord();

	return bRet;
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

/////
// 从数据库表格 openinv_o 中累加符合条件的库存
//     Warehouse条件可以为空，则累加所有仓库的库存
//
BOOL CMPSCore::QueryGetOutstandingPO(
	/* IN */  const CTime & StartingDate,
	/* IN */  const CString & SkuCode,
	/* IN */  const CString & Warehouse,
	/* OUT */ CString & OutstandingPO)
{
	CString CondSkuCode, CondWarehouse, CondTime;
	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;

	// 条件 CondSkuCode
	ASSERT(SkuCode.GetLength() != 0);
	if (SkuCode.GetLength() <= 0)
		return FALSE;
	CondSkuCode.Format("(`jdeskucode`='%s')", (LPCTSTR)SkuCode);

	// 条件 CondWarehouse
	if (Warehouse.GetLength() > 0)
		CondWarehouse.Format("(`jdewh`='%s')", (LPCTSTR)Warehouse);

	// 准备“常量”
	// 条件 CondTime
	int iLeadTime = 0;
	bRet = QueryGetLeadTime(SkuCode, Warehouse, iLeadTime);
	if (bRet)
	{
		CTime OrderDate = StartingDate - CTimeSpan(iLeadTime * 7, 0, 0, 0);
		CondTime.Format("(`jdeorderdate`>='%s' and `jdeorderdate`<='%s')",
			ConvertDateToString(OrderDate), ConvertDateToString(StartingDate));
	}

	if (bRet)
	{
		// 组成完整的SQL语句
		CString SQL;
		SQL.Format(
			"SELECT SUM(`jdevolume`) FROM `plannedarrival_o` WHERE %s AND %s",
			(LPCTSTR)CondSkuCode, (LPCTSTR)CondTime);
		if (CondWarehouse.GetLength() > 0)
		{
			SQL.Append(" and ");
			SQL.Append(CondWarehouse);
		}

		// 执行SQL
		bRet = SelectQuery(SQL);
	}

	if (bRet)
	{
		row = GetRecord();
		if ( !row || !row[0] )
			OutstandingPO = "0";
		else
			OutstandingPO.Format("%s", row[0]);
	}

	FreeRecord();

	return bRet;

}

/////
// 从数据库 warehouse_d 中查得仓库的所在地 如：TC-BD
//     输入 
BOOL CMPSCore::QueryGetWarehouseLocation(
	/* IN */  const CString & Warehouse,
	/* OUT */ CString & Location)
{
	CString SQL;
	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;

	SQL.Format("SELECT `description` FROM `warehouse_d` WHERE `jdewh`='%s'",
		(LPCTSTR)Warehouse);

	bRet = SelectQuery((LPCTSTR)SQL);

	if (bRet)
	{
		row = GetRecord();
		if (!row || !row[0])
			bRet = FALSE;
	}

	if (bRet)
	{
		Location = row[0];
	}

	FreeRecord();
	return bRet;
}

/////
// 从数据库表 modelengine_o 中查得leadtime
// 如何获得LeadTime
// 根据 Warehouse（如“MJCNN365”）查表 warehouse_d，获得仓库所在地地址
// 然后根据所在地址和 SkuCode 到 modalengine_o 表中查找 LeadTime
//
BOOL CMPSCore::QueryGetLeadTime(
	/* IN */  const CString & SkuCode,
	/* IN */  const CString & Warehouse,
	/* OUT */ int &iLeadTime
	)
{
	CString SQL;
	CString WarehouseLoc;
	BOOL bRet = FALSE;
	MYSQL_ROW row = NULL;

	SQL.Format("SELECT `description` FROM `warehouse_d` WHERE `jdewh`='%s'",
		(LPCTSTR)Warehouse);

	bRet = SelectQuery((LPCTSTR)SQL);

	if (bRet)
	{
		row = GetRecord();
		if (!row || !row[0])
		{
			TRACE0("Debug: QueryGetLeadTime return FALSE\n");
			bRet = FALSE;
		}
	}

	if (bRet)
	{
		WarehouseLoc = row[0];
		FreeRecord();

		SQL.Format("SELECT `leadtime` FROM `modelengine_o` " \
			"WHERE (`fullindex` LIKE '%%\\_%s\\_%%') AND (`jdeskucode`='%s')",
			(LPCTSTR)WarehouseLoc, (LPCTSTR)SkuCode);

		bRet = SelectQuery((LPCTSTR)SQL);
	}

	if (bRet)
	{
		row = GetRecord();
		if (!row || !row[0])
		{
			TRACE0("Debug: QueryGetLeadTime return FALSE\n");
			bRet = FALSE;
		}
	}

	if (bRet)
	{
		iLeadTime = atoi(row[0]);
	}

	FreeRecord();
	return bRet;
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
	//CMap<CString,LPCTSTR,double,double> MonthlyForecastMap;
	int iLeadTimeDay = (m_LeadTime - 1) * 7;
	double Forecast = 0.0f;

	ASSERT(m_LeadTime >= 1);

	for (int i = 0; i < iLeadTimeDay; i++)
	{
		// TODO: 用单个函数来计算是否工作日
		if (StartingDate.GetDayOfWeek() == 1 || StartingDate.GetDayOfWeek() == 7)
		{
			StartingDate = StartingDate + CTimeSpan(1, 0, 0, 0);
			continue;
		}

		Forecast += this->GetDailyForecast(StartingDate, FullIndex);

		// Next
		StartingDate = StartingDate + CTimeSpan(1, 0, 0, 0);
	}

	ResultStr.Format("%0.3f", Forecast);
	return TRUE;
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

BOOL CMPSCore::QueryFinalResult_RecommendedOrderVolume(
	CTime Wk1ForSale, CString FullIndex,
	double OpenInv, double OutstandingPO, double LTForecast,
	CString &ResultStr
	)
{
	CArray<double> SaleVolume;
	CString ErrStr;

	if (this->GetSalesVolumeOfWeeks(Wk1ForSale,
		FullIndex, SaleVolume, &ErrStr) == FALSE)
	{
		return FALSE;
	}

	double m_AdjustedAveWeeklySale // to be used in QueryFinalResult_RecommendedWithoutLastWeekOrder
		= GetAdjustedAveWeeklySaleVol(SaleVolume);
	double RecommendedOrderVolume = 0;

	if ((OpenInv+OutstandingPO-max(0,LTForecast-m_LeadTime*m_AdjustedAveWeeklySale)) < m_ROP)
	{
		RecommendedOrderVolume = max(m_ROQ,m_ROQ + m_SafeStock +
			max(LTForecast,m_LeadTime*m_AdjustedAveWeeklySale) - OpenInv - OutstandingPO);
	}

	ResultStr.Format("%0.3f", RecommendedOrderVolume);
	return TRUE;
}


BOOL CMPSCore::QueryFinalResult_ActionFlag(
	const CString &SaleVSForecast,
	const CString &RecommendedOrderVolume,
	CString &ResultStr
	)
{
	double volume = atof(RecommendedOrderVolume);
	ASSERT ( volume >= 0 );

	if (volume < 0)
		return FALSE;

	if ((SaleVSForecast.CompareNoCase("Undersell") == 0) && (volume > 0))
		ResultStr = "Review FC Down";
	else if ((SaleVSForecast.CompareNoCase("Oversell") == 0) && (volume == 0))
		ResultStr = "Review FC Upwards";
	else
		ResultStr = "-";

	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_LastWeekOrder(
	CTime StartingDate, CString FullIndex, CString &ResultStr
	)
{
	CString SqlStr;

	this->m_arrival = 0; // 避免重复计算，这个函数中直接把本周将到达的数量算出来

	SqlStr.Format("SELECT `jdeorderdate`, `jdevolume` FROM `plannedarrival_o` "
			"WHERE (%s) and (%s)",
			(LPCTSTR) this->m_ConditionWh,
			(LPCTSTR) this->m_ConditionSku
			);

	if (!this->SelectQuery(SqlStr))
		return FALSE;
	MYSQL_ROW Row = this->GetRecord();

	double VolumeSum = 0;

	while ( Row && Row[0] && Row[1])
	{
		CString strYear, strMonth, strDay;
		CString debug;

		AfxExtractSubString(strYear,  Row[0], 0, '-');
		AfxExtractSubString(strMonth, Row[0], 1, '-');
		AfxExtractSubString(strDay,   Row[0], 2, '-');
	
		CTime OrderDate(atoi(strYear), atoi(strMonth), atoi(strDay), 0, 0, 0);
		debug = OrderDate.Format("%A, %B %d, %Y");
		
		CTime ExpectedArrivalDate = OrderDate + CTimeSpan(this->m_LeadTime * 7, 0, 0, 0);
		debug = ExpectedArrivalDate.Format("%A, %B %d, %Y");

		CTimeSpan span = ExpectedArrivalDate - StartingDate;
		if (span.GetDays() >= 7 && span.GetDays() < 14)
		{
			m_arrival += atoi(Row[1]);
		}

		CTime ModifiedArrivalDate = (ExpectedArrivalDate < StartingDate)
			? StartingDate+CTimeSpan(1,0,0,0) : ExpectedArrivalDate;
		debug = ModifiedArrivalDate.Format("%A, %B %d, %Y");

		CTimeSpan offset = ModifiedArrivalDate - StartingDate;
		ULONGLONG a = offset.GetDays();

		CTime ModifiedOrderDate = StartingDate - CTimeSpan((LONG)(this->m_LeadTime - offset.GetDays() / 7) * 7, 0,0,0);
		debug = ModifiedOrderDate.Format("%A, %B %d, %Y");

		CTimeSpan offset2 = StartingDate - ModifiedOrderDate;
		a = offset2.GetDays();

		if ( offset2.GetDays() == 7 )
			VolumeSum += atoi(Row[1]);

		// Next
		Row = this->GetRecord();
	}

	ResultStr.Format("%0.3f", VolumeSum);
	this->FreeRecord();

	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_RecommendedWithoutLastWeekOrder(
	double OpenInv,
	double OutstandingPO,
	double LastWeekOrder,
	double LTForecast,
	CString &ResultStr
	)
{
	double tmp = OpenInv + OutstandingPO - LastWeekOrder
		- max(0, LTForecast - m_LeadTime * m_AdjustedAveWeeklySale);

	if ( tmp >= m_ROP )
		ResultStr.Empty();
	else
	{
		tmp = max(m_ROQ, m_ROQ + m_SafeStock + max(LTForecast,
			m_LeadTime * m_AdjustedAveWeeklySale) - OpenInv - OutstandingPO);
		ResultStr.Format("%0.3f", tmp);
	}

	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_ProductionTag(
	double LastWeekOrder,
	double RecommendWithoutLastWeekOrder,
	CString &ResultStr
	)
{
	if (RecommendWithoutLastWeekOrder == 0 && LastWeekOrder != 0)
		ResultStr = "Cut Production";
	else
		ResultStr = "-";

	return TRUE;
}

// 根据原算法理解，此函数返回的是根据已下订单推算出的下一个星期(7~14天)的到货量
BOOL CMPSCore::QueryFinalResult_Arrival(
	CString &ResultStr
	)
{
	ResultStr.Format("%0.3f", this->m_arrival);
	return TRUE;
}

BOOL CMPSCore::QueryFinalResult_Sales(
	CTime StartingDate, CString FullIndex, CString &ResultStr
	)
{
	double sum = 0;
	for ( int i = 0; i < 7; i++)
	{
		CTime date = StartingDate + CTimeSpan(i, 0, 0, 0);
		// TODO: 用单个函数来计算是否工作日
		if (date.GetDayOfWeek() == 1 || date.GetDayOfWeek() == 7)
			continue;

		sum += this->GetDailyForecast(date, FullIndex);
	}

	ResultStr.Format("%0.3f", sum);
	return TRUE;
}

// 在数据库中建一个新的SKU组合名
BOOL CMPSCore::CreateNewSkuCombination(const CString &CbName, CString *pErrStr)
{
	return AddSkuCodeToCombination(CbName, "", pErrStr);
}

// 获取SKU组合中所有SKUCODE
BOOL CMPSCore::GetAllSkuCodesOfCombination( const CString &strName, CArray<CString> *pSkuCodes, CString *pErrStr )
{
	CString SqlStr;

	ASSERT(strName.GetLength() != NULL);
	ASSERT(pSkuCodes != NULL);

	SqlStr.Format("SELECT DISTINCT `skucode` from `skucombine_s` where `cbname`='%s' and not `skucode`=''", (LPCTSTR)strName);
	if (!this->SelectQuery(SqlStr))
	{
		if (pErrStr)
			*pErrStr = this->OutErrors();
		return FALSE;
	}

	pSkuCodes->RemoveAll();

	MYSQL_ROW row = NULL;

	while ( NULL != (row = this->GetRecord()))
	{
		pSkuCodes->Add(row[0]);
	}

	this->FreeRecord();

	return TRUE;
}

// 添加一个SKUCODE到组合中
BOOL CMPSCore::AddSkuCodeToCombination( const CString &CbName, const CString &SkuCode, CString *pErrStr )
{
	CString SqlStr;

	ASSERT(CbName.GetLength() != 0);

	if (CbName.GetLength() == 0)
	{
		if (pErrStr != NULL)
			*pErrStr = "Invalid combination name";
		return FALSE;
	}

#ifdef _DEBUG

#endif

	SqlStr.Format( "INSERT INTO skucombine_s values ('%s','%s')", (LPCTSTR) CbName, (LPCTSTR) SkuCode );
	if (!this->NonSelectQuery(SqlStr))
	{
		if (pErrStr != NULL)
			*pErrStr = this->OutErrors();
		return FALSE;
	}

	return TRUE;
}

BOOL CMPSCore::RemoveSkuCodeFromCombination( const CString &CbName, const CString &SkuCode, CString *pErrStr )
{
	CString SqlStr;

	SqlStr.Format("DELETE from `skucombine_s` where `cbname`='%s' and `skucode`='%s'", (LPCTSTR) CbName, (LPCTSTR)SkuCode);
	if (!this->NonSelectQuery(SqlStr))
	{
		*pErrStr = this->OutErrors();
		return FALSE;
	}

	if ( 0 == this->GetAffectedRow() )
	{
		*pErrStr = "Affected none record";
		return FALSE;
	}

	return TRUE;
}

BOOL CMPSCore::DeleteSkuCombination( const CString &CbName, CString *pErrStr )
{
	CString SqlStr;

	SqlStr.Format("DELETE from `skucombine_s` where `cbname`='%s'", (LPCTSTR) CbName);
	if (!this->NonSelectQuery(SqlStr))
	{
		*pErrStr = this->OutErrors();
		return FALSE;
	}

	if ( 0 == this->GetAffectedRow() )
	{
		*pErrStr = "Affected none record";
		return FALSE;
	}

	return TRUE;
}

// 计算53个星期每周的销售量
BOOL CMPSCore::GetSalesVolumeOfWeeks(CTime StartDate4Sales, CString FullIndex,
	CArray<double> &WeeksSaleVolume, CString *pErrStr
	)
{
	CString SqlStr;
	CTime TimeWeekend;
	MYSQL_ROW row = NULL;
	ASSERT(StartDate4Sales.GetDayOfWeek() == 2);

	for ( int i = 0; i < 53; i++)
	{
		TimeWeekend = StartDate4Sales + CTimeSpan(7, 0, 0, 0);
		SqlStr.Format("SELECT sum(`jdefgsalesvolume`) FROM `fgsales_o` "
			"WHERE `fullindex` like '%%%s%%' AND `jdefgsalesdate`>='%s' AND `jdefgsalesdate`<'%s'",
			(LPCTSTR) FullIndex,
			(LPCTSTR) this->ConvertDateToString(StartDate4Sales),
			(LPCTSTR) this->ConvertDateToString(TimeWeekend)
			);

		if ( !this->SelectQuery(SqlStr))
		{
			if (pErrStr)
				*pErrStr = this->OutErrors();
			return FALSE;
		}

		row = this->GetRecord();
		if (row != NULL && row[0] != NULL)
			WeeksSaleVolume.Add(atof((LPCTSTR)row[0]));
		else
			WeeksSaleVolume.Add(0.0);

		this->FreeRecord();

		// 累加日期，准备下一个for循环
		StartDate4Sales = TimeWeekend;
	}

	return TRUE;
}

// 计算第一个有销售记录的星期后的周数
// 输入参数，GetSalesVolumeOfWeeks的返回值
// 返回值：0~53
unsigned CMPSCore::GetWeeksAfterFirstSale(
	const CArray<double> &WeekSales // Result of GetSalesVolumeOfWeeks()
	)
{
	unsigned i = 0;

	ASSERT(WeekSales.GetCount() == 53);

	if (WeekSales.GetCount() != 53)
		return 0;

	for (i = 0; i < 52; i++)
	{
		if (WeekSales.GetAt(i) != 0)
			break;
	}

	return 53 - i;
}

// 对数组中的值求和
double CMPSCore::SumOf(const CArray<double> &Array)
{
	double sum = 0;

	for ( int i = 0; i < Array.GetSize(); i++)
	{
		sum += Array.GetAt(i);
	}

	return sum;
}

// 把日期转换成字符串，形如 2010-1-31
CString CMPSCore::ConvertDateToString(const CTime &Date)
{
	CString String;
	String.Format("%u-%u-%u", Date.GetYear(), Date.GetMonth(), Date.GetDay());
	return String;
}

double CMPSCore::StdDev(const CArray<double> &Array)
{
	CArray<double> PowOfArray;
	int count = Array.GetCount();

	for (int i = 0; i < count; i++)
	{
		double a = Array.GetAt(i);
		PowOfArray.Add(a * a);
	}

	double sum = this->SumOf(Array);
	double pow_sum = this->SumOf(PowOfArray);
	//count -= 1;
	return sqrt((pow_sum*count - sum*sum) / count / (count-1));
	//double s_dev = sqrt(((pow(sum,2.0))-((1.0/count)*(pow(sum,2.0))))/(count-1.0));
	//return s_dev;
}

// 调整周销售量，使其符合正态分布
// 如果数组中全部是零时？？？
BOOL CMPSCore::AdjustWeekSaleStd(
	const CArray<double> &SaleVolume, unsigned filter,
	CArray<double> &SaleVolumeAfterAdjust)
{
	CArray<double> TempArray;
	TempArray.Append(SaleVolume);

	// 去掉数组前面的为零的数据
	// 注解：建议调整时去掉这段代码
	while ( TempArray.GetCount() > 0 && TempArray.GetAt(0) == 0 )
		TempArray.RemoveAt(0);

	double stddev = this->StdDev(TempArray);
	double u = this->SumOf(TempArray) / TempArray.GetCount();

	// 挨个检查数据，存到输出队列中
	int count = SaleVolume.GetCount();
	for (int i = 0; i < count; i++)
	{
		double value = SaleVolume.GetAt(i);
		//if (value < 0)
		//	continue;
		if ( value >= u + stddev * filter )
			value = u;
		SaleVolumeAfterAdjust.Add(value);
	}

	return TRUE;
}

double CMPSCore::GetAdjustedAveWeeklySaleVol(const CArray<double> &SaleVolume)
{
	unsigned WeeksNumAfterFirstSale	= this->GetWeeksAfterFirstSale(SaleVolume);

	if (WeeksNumAfterFirstSale == 0)
		return 0;

	CArray<double> AdjustedVolumes;
	this->AdjustWeekSaleStd(SaleVolume, 2, AdjustedVolumes);

	double AveWeeklySale = this->SumOf(AdjustedVolumes) / WeeksNumAfterFirstSale;

	if (WeeksNumAfterFirstSale >= 25)
	{
		double SumHalf = 0;
		for (int i=27; i<52; i++)
			SumHalf += AdjustedVolumes.GetAt(i);
		SumHalf = SumHalf / 26;

		AveWeeklySale += SumHalf;
		AveWeeklySale /= 2;
	}

	return AveWeeklySale;
}

double CMPSCore::GetROP(const CString &FullIndex)
{
	CString SqlStr;
	SqlStr.Format("SELECT `rop` FROM `modelengine_o` where `fullindex`='%s'", FullIndex);

	this->SelectQuery(SqlStr);

	MYSQL_ROW Row = this->GetRecord();

	if (!Row || !Row[0])
	{
		this->FreeRecord();
		return FALSE;
	}

	double ROP = atoi(Row[0]);
	this->FreeRecord();

	return ROP;
}

double CMPSCore::GetROQ(const CString &FullIndex)
{
	CString SqlStr;
	SqlStr.Format("SELECT `roq` FROM `modelengine_o` where `fullindex`='%s'", FullIndex);

	this->SelectQuery(SqlStr);

	MYSQL_ROW Row = this->GetRecord();

	if (!Row || !Row[0])
	{
		this->FreeRecord();
		return FALSE;
	}

	double ROQ = atoi(Row[0]);
	this->FreeRecord();

	return ROQ;
}

double CMPSCore::GetSafeStock(const CString &FullIndex)
{
	CString SqlStr;
	SqlStr.Format("SELECT `safestock` FROM `modelengine_o` where `fullindex`='%s'", FullIndex);

	this->SelectQuery(SqlStr);

	MYSQL_ROW Row = this->GetRecord();

	if (!Row || !Row[0])
	{
		this->FreeRecord();
		return FALSE;
	}

	double SS = atoi(Row[0]);
	this->FreeRecord();

	return SS;
}

// 计算一个月份中的工作日数量
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

//
// monthlyforecast_o
// 获取该物品，在某天的预计销售数量
// 说明，销售按照月份预估，因此根据每个月的工作日数，平摊销售数据到每日
double CMPSCore::GetDailyForecast(const CTime &date, const CString &FullIndex)
{
	CString key;
	double  value = 0;

	key.Format("%d-%d-1", date.GetYear(), date.GetMonth());

	if ( !this->m_MonthlyForecastMap.Lookup(key, value))
	{
		CString SqlStr;

		SqlStr.Format("SELECT forecastvolume FROM monthlyforecast_o"
			" where fullindex like '%%%s%%' and forecastmonth='%s'",
			(LPCTSTR)FullIndex,
			(LPCTSTR)key);

		this->SelectQuery(SqlStr);

		MYSQL_ROW Row = this->GetRecord();
		if (Row == NULL)
		{
			value = 0;
		}
		else
		{
			value = atof((LPCTSTR) Row[0]);
			value = value / GetWorkdayPerMonth(date.GetYear(), date.GetMonth());
		}
		m_MonthlyForecastMap.SetAt(key, value);
		this->FreeRecord();
	}

	return value;
}

