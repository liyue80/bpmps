#pragma once


// CSkuListView view

class CSkuListView : public CListView
{
	DECLARE_DYNCREATE(CSkuListView)

public:
	CSkuListView();           // protected constructor used by dynamic creation

	virtual ~CSkuListView();

	BOOL IsInitialized(void) const
	{
		return m_bInitialized;
	};

	BOOL UpdateData(BOOL bSave);

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
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
private:
	BOOL m_bInitialized;	// TRUE, if data has been loaded from database
};


