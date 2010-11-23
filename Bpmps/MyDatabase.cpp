#include "StdAfx.h"
#include "MyDatabase.h"

CMyDatabase::CMyDatabase(void)
: m_query(NULL)
, m_row(NULL)
, m_field(NULL)
{
	mysql_init( &this->m_mysql );
}

CMyDatabase::~CMyDatabase(void)
{
	if (this->m_query != NULL)
	{
		mysql_free_result( this->m_query );
		mysql_close(&this->m_mysql);
	}
}

BOOL CMyDatabase::ConnectDB(LPCTSTR lpszHost, LPCTSTR lpszUser,
							LPCTSTR lpszPassword, LPCTSTR lpszDatabase,
							int iPort, LPCTSTR lpszUnixSocket,
							ULONG uClientFlag)
{
	if (!mysql_real_connect(&this->m_mysql, lpszHost, lpszUser,
		lpszPassword, lpszDatabase, iPort, lpszUnixSocket, uClientFlag))
	{
		TRACE1("%s\n", mysql_error(&this->m_mysql));
		return FALSE;
	}

	return TRUE;
}

BOOL CMyDatabase::SelectAll(LPCTSTR lpszTable)
{
	CString Query;

	ASSERT(lpszTable);

	Query.Format("SELECT * FROM %s", lpszTable);

	return FindAndSave((LPCTSTR)Query);
}

BOOL CMyDatabase::FindAndSave(LPCTSTR str)
{
	TRACE1("%s\n", str);
	if (mysql_query(&this->m_mysql, str) != 0)
	{
		return FALSE;
	}

	this->m_query = mysql_use_result(&this->m_mysql);
	this->m_field = mysql_fetch_fields(this->m_query);

	return TRUE;
}

int CMyDatabase::GetFieldNum(void)
{
	return (int) mysql_field_count(&m_mysql);
}

LPCTSTR CMyDatabase::GetFieldName(int FieldNum)
{
	ASSERT(m_field);

	return m_field[FieldNum].name;
}

//
// Description
//     Returns a character string containing information similar to
//     that provided by the mysqladmin status command. This includes
//     uptime in seconds and the number of running threads, questions,
//     reloads, and open tables.
// Return Values
//     A character string describing the server status.
//     NULL if an error occurred. 
//
LPCTSTR CMyDatabase::GetState(void)
{
	return mysql_stat(&this->m_mysql);
}

BOOL CMyDatabase::IsEnd()
{
	ASSERT(this->m_query != NULL);

	return mysql_eof(this->m_query) ? TRUE : FALSE;
}

BOOL CMyDatabase::NonSelectQuery( LPCTSTR lpszSQL )
{
	ASSERT(lpszSQL != NULL);

	if (mysql_query(&this->m_mysql, lpszSQL) != 0)
		return FALSE;

	return TRUE;
}

BOOL CMyDatabase::SelectQuery( LPCTSTR lpszSQL )
{
	return this->FindAndSave(lpszSQL);
}

void CMyDatabase::FreeRecord()
{
	if (this->m_query == NULL)
		return;

	mysql_free_result(this->m_query);
	this->m_query = NULL;
	this->m_field = NULL;
	this->m_row = NULL;
}
