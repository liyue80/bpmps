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

	afx_msg LRESULT OnMessage_AppendRecord( WPARAM wParam, LPARAM lParam );
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();
public:
	BOOL UpdateData(const CQueryFilter *pQueryFilter);

private:
	CMap<CString, LPCTSTR, HTREEITEM, HTREEITEM> m_TreeItemMap;
};
