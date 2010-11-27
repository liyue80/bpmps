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

	// �����û�ѡ�����������ѯ������Ʒ��7��Ԥ����
	BOOL QueryFinalResult(CTime StartingDate, CString FullIndex, CArray<CString> &ResultArray);

public:
	// �����ݿ��н�һ���µ�SKU�����
	BOOL CreateNewSkuCombination(const CString &CbName, CString *pErrStr);

	// �����ݿ���ɾ��һ��SKU���
	BOOL DeleteSkuCombination(const CString &CbName, CString *pErrStr);

	// ��ȡSKU���������SKUCODE
	BOOL GetAllSkuCodesOfCombination(const CString &strName, CArray<CString> *pSkuCodes, CString *pErrStr);

	// ���һ��SKUCODE�������
	BOOL AddSkuCodeToCombination(const CString &CbName, const CString &SkuCode, CString *pErrStr);

	// �������ɾ��һ��SKUCODE
	BOOL RemoveSkuCodeFromCombination(const CString &CbName, const CString &SkuCode, CString *pErrStr);

protected:
	BOOL QueryFinalResult_OpenInv(CTime StartingDate, CString FullIndex, CString &ResultStr);

	BOOL QueryFinalResult_OutstandingPO(CTime StartingDate, CString FullIndex, CString &ResultStr);

	BOOL QueryFinalResult_LTForecast( CTime StartingDate, CString FullIndex, CString &ResultStr );

	BOOL QueryFinalResult_SaleVSForecast( CTime StartingDate, CString FullIndex, CString &ResultStr );

	int  GetWorkdayPerMonth( int year, int month );

private:

	CQueryFilter m_QueryFilter;

	CArray<CString> m_ResultFieldsName;

	// ����QueryFinalResult�������м�����������ظ�����
	CString m_ConditionWh;
	CString m_ConditionSku;
	int     m_LeadTime;  // in weeks
};
