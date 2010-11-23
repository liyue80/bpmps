// ChildView.h : interface of the CChildView class
//


#pragma once
#include "ResultView.h"
#include "SkuListView.h"

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	CResultView* GetResultView(void)
	{
		return this->m_pResultView;
	};

	CSkuListView* GetSkuListView(void)
	{
		return this->m_pSkuListView;
	};

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();
	void SwitchSkuListView(void);
	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	CResultView* m_pResultView;
	CSkuListView* m_pSkuListView;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	void HideAllChildViewExclude(const CWnd * pViewExclude);
};
