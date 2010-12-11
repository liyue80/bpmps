#pragma once

#include "ColumnTreeView.h"
#include "MPSCore.h"

// CResultTreeview ��ͼ

class CResultTreeview : public CColumnTreeView
{
	DECLARE_DYNCREATE(CResultTreeview)

public:
	CResultTreeview();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CResultTreeview();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();
public:
	BOOL UpdateData(const CQueryFilter *pQueryFilter);
};
