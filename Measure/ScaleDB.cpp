#include "ScaleDB.h"
#include "sqlite3.h"

#define DB_PATHNAME    "./scale.db"
int create_db() 
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    do
    {
        //�����ݿ�
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        // ��ʽ��SQL���
        char cSql[512] = { 0 };
        // ����һ����,����ñ���ڣ��򲻴�������������ʾ��Ϣ���洢�� zErrMsg ��
        sqlite3_snprintf(512, cSql, "CREATE TABLE scale_result(\
				id INTEGER PRIMARY KEY AUTOINCREMENT,\
				create_time INTEGER NOT NULL,\
				amount INTEGER NULL default 0,\
				lenght INTEGER NULL default 0,\
                total_volume Float Default 0,\
				wood_list VARCHAR(40960) NULL default ''\
			);");
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("create test_table successful. \n");
    } while (false);

    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}

int db_insert_record(int create_time, int amount, double lenght, double total_volume, std::string wood_list)
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    do
    { 
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        // ��������
        sqlite3_snprintf(512, cSql, "INSERT INTO scale_result(create_time, amount, lenght, total_volume, wood_list)\
         VALUES(%d, %d, %f, %f, '%s')", create_time, amount, lenght, total_volume, wood_list.c_str());
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");

    } while (false);
   

    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}
int db_query_all()
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    do
    {
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        sqlite3_snprintf(512, cSql, "select * from scale_result order by create_time desc");
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");

    } while (false);


    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}

int db_query_by_time_range(int start_time, int end_time)
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    do
    {
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        sqlite3_snprintf(512, cSql, "select * from scale_result where create_time>%d and create_time<%d order by create_time desc", start_time, end_time);
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");

    } while (false);


    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}

int db_query_by_id(int create_time)
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    do
    {
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        sqlite3_snprintf(512, cSql, "select * from scale_result where create_time = %d", create_time);
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");

    } while (false);


    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}
int db_delete_all()
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    int ret = 0;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    do
    {
        int nRes = sqlite3_open(DB_PATHNAME, &pDB);
        if (nRes != SQLITE_OK)
        {
            //�����ݿ�ʧ��
            // writeLog
            printf("sqlite3_open, �����ݿ�ʧ��: %s --------------------\n", sqlite3_errmsg(pDB));
            ret = -1;
            break;
        }

        sqlite3_snprintf(512, cSql, "delete * from scale_result");
        nRes = sqlite3_exec(pDB, cSql, NULL, NULL, &pcErrMsg);
        if (nRes != SQLITE_OK)
        {
            printf("�������ݿ��test_table ʧ��: %s --------------------\n", pcErrMsg);
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");

    } while (false);


    //�ر����ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }

    return ret;
}