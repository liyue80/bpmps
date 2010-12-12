// WorkThread.h : 
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

///////////////////////////////////////////////////////////
typedef struct tag_MainWorkThreadParam
{
	__time64_t _StartingDate;
	__time64_t _FirstWeekSale;
} MAINWORKTHREADPARAM, *LPMAINWORKTHREADPARAM;

UINT AFX_CDECL MainWorkThreadFunc(LPVOID *lpParam);

///////////////////////////////////////////////////////////

// end of file
