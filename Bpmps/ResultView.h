#pragma once

#include "MPSCore.h"

// CResultView view

class CResultView : public CListView
{
	DECLARE_DYNCREATE(CResultView)

public:
	CResultView();           // protected constructor used by dynamic creation
	virtual ~CResultView();
	BOOL UpdateContent(MYSQL_RES *pMySqlRes, int nFieldCount);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL UpdateData(const CQueryFilter *pQueryFilter);
};


