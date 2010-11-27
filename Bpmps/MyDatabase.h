#pragma once

#include "../mysql/include/mysql.h"

class CMyDatabase
{
public:
	CMyDatabase(void);
	~CMyDatabase(void);

public:
	BOOL UnLockTable(); //解锁
	BOOL LockTable(LPCTSTR lpszTableName, LPCTSTR lpszPriority); //加锁
	int Reload(); //重新登陆，非零时返回错误信息
	LPCTSTR GetState(void); //服务器状态
	LPCTSTR GetServerInfo(); //服务器信息
	int GetProtocolInfo(); //协议信息
	LPCTSTR GetHostInfo(); //主机信息
	LPCTSTR GetClientInfo(); //客户机信息
	LPCTSTR GetFieldName(int FieldNum); //字段名
	BOOL IsEnd(); //是否最后
	int DropDB(LPCTSTR db); //删除数据库,非零时返回错误信息
	void SeekData(int offset); //查找指定数据
	int CreateDB(LPCTSTR db); //创建数据库，非零时返回错误信息
	void FreeRecord(); //释放结果集
	int GetFieldNum(); //得到字段数
	BOOL ConnectDB(LPCTSTR lpszHost, LPCTSTR lpszUser,
		LPCTSTR lpszPassword, LPCTSTR lpszDatabase, int iPort,
		LPCTSTR lpszUnixSocket, ULONG uClientFlag); //连接数据库

	//得到结果（一个记录）	
	MYSQL_ROW GetRecord() const
	{
		ASSERT(this->m_query != NULL);
		return mysql_fetch_row(this->m_query);
	};

	// 得到记录数
	my_ulonglong GetRowNum() const;/*
	{
		ASSERT(this->m_query != NULL);
		return mysql_num_rows(this->m_query);
	};*/

	BOOL SelectDB(LPCTSTR lpszDatabase); //选择数据库

	BOOL SelectQuery(LPCTSTR lpszSQL); //选择记录

	BOOL NonSelectQuery(LPCTSTR lpszSQL);

	BOOL SelectAll(LPCTSTR lpszTable);  // select all records

	inline LPCTSTR OutErrors(void) // return error message
	{
		return mysql_error(&this->m_mysql);
	};

	my_ulonglong GetAffectedRow();

protected:
	BOOL FindAndSave(LPCTSTR str);

private:
	MYSQL m_mysql;         //数据库连接句柄
	MYSQL_RES   * m_query; //结果集
	MYSQL_ROW     m_row;   //记录集
	MYSQL_FIELD * m_field; //字段信息（结构体）
};
