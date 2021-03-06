#pragma once

#include "ColumnTreeView.h"
#include "MPSCore.h"

// CResultTreeview 视图

class CResultTreeview : public CColumnTreeView
{
	DECLARE_DYNCREATE(CResultTreeview)

public:
	CResultTreeview();           // 动态创建所使用的受保护的构造函数
	virtual ~CResultTreeview();

	afx_msg LRESULT OnMessage_UI_APPEND_RECORD( WPARAM wParam, LPARAM lParam );
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();
public:
	BOOL UpdateData(const CQueryFilter *pQueryFilter);

private:
	std::map<std::string, HTREEITEM> m_TreeItemMap;
};
