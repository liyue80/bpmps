#pragma once

#include "../mysql/include/mysql.h"

class CMyDatabase
{
public:
	CMyDatabase(void);
	~CMyDatabase(void);

public:
	BOOL UnLockTable(); //����
	BOOL LockTable(LPCTSTR lpszTableName, LPCTSTR lpszPriority); //����
	int Reload(); //���µ�½������ʱ���ش�����Ϣ
	LPCTSTR GetState(void); //������״̬
	LPCTSTR GetServerInfo(); //��������Ϣ
	int GetProtocolInfo(); //Э����Ϣ
	LPCTSTR GetHostInfo(); //������Ϣ
	LPCTSTR GetClientInfo(); //�ͻ�����Ϣ
	LPCTSTR GetFieldName(int FieldNum); //�ֶ���
	BOOL IsEnd(); //�Ƿ����
	int DropDB(LPCTSTR db); //ɾ�����ݿ�,����ʱ���ش�����Ϣ
	void SeekData(int offset); //����ָ������
	int CreateDB(LPCTSTR db); //�������ݿ⣬����ʱ���ش�����Ϣ
	void FreeRecord(); //�ͷŽ����
	int GetFieldNum(); //�õ��ֶ���
	BOOL ConnectDB(LPCTSTR lpszHost, LPCTSTR lpszUser,
		LPCTSTR lpszPassword, LPCTSTR lpszDatabase, int iPort,
		LPCTSTR lpszUnixSocket, ULONG uClientFlag); //�������ݿ�

	//�õ������һ����¼��	
	MYSQL_ROW GetRecord() const
	{
		ASSERT(this->m_query != NULL);
		return mysql_fetch_row(this->m_query);
	};

	// �õ���¼��
	my_ulonglong GetRowNum() const;/*
	{
		ASSERT(this->m_query != NULL);
		return mysql_num_rows(this->m_query);
	};*/

	BOOL SelectDB(LPCTSTR lpszDatabase); //ѡ�����ݿ�

	BOOL SelectQuery(LPCTSTR lpszSQL); //ѡ���¼

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
	MYSQL m_mysql;         //���ݿ����Ӿ��
	MYSQL_RES   * m_query; //�����
	MYSQL_ROW     m_row;   //��¼��
	MYSQL_FIELD * m_field; //�ֶ���Ϣ���ṹ�壩
};
