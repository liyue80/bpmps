// Bpmps.h : main header file for the Bpmps application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "MPSCore.h"

#define WM_BTN_CLICK_QUERY (WM_USER + 200)
#define WM_USER_UI_APPEND_RECORD (WM_USER + 300)

// CBpmpsApp:
// See Bpmps.cpp for the implementation of this class
//

class CBpmpsApp : public CWinApp
{
public:
	CBpmpsApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	CMPSCore m_MPSCore;
};

extern CBpmpsApp theApp;

// 
// 自定义消息 WM_USER_UI_APPEND_RECORD 的参数 lParam
typedef struct tagAppendingRecord
{
	char _SkuCode[45];
	char _Warehouse[45];
	char _OpenInvFirst[32];
} APPENDING_RECORD;
