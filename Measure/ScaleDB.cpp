#include "ScaleDB.h"
#include "sqlite3.h"
#include <json/json.h>
#include "ScaleAPI.h"

#define DB_PATHNAME    "./scale.db"

std::string json2string() {// �½� JSON ����
    Json::Value root;// �� JSON ������Ӽ�ֵ��
    root["name"] = "Alice";
    root["age"] = 25;// �� JSON ����תΪ�ַ���
    Json::StyledWriter writer;
    std::string json_str = writer.write(root);// ���ַ���תΪ char*const char* data = json_str.c_str();// ��ӡ���
    std::cout << json_str << std::endl;
    return json_str;
}
int string2json() {// JSON�ַ���const char* data = "{\"name\":\"Alice\",\"age\":25}";// ���ַ���תΪ JSON ����
    const char* data = "{\"name\":\"Alice\",\"age\":25}";
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(data, root);
    if (!parsingSuccessful) {
        std::cout << "���� JSON ʧ��" << std::endl;
        return -1;
    }// �� JSON �����ж�ȡ�ض�����ֵ
    std::string name = root["name"].asString();
    int age = root["age"].asInt();// ��ӡ���
    std::cout << "������" << name << std::endl;
    std::cout << "���䣺" << age << std::endl; 
    return 0;
}

// �ص����������ڴ����н��������������£���ֻ�ᴦ��һ�Σ�
int rowCountCallback(void* NotUsed, int argc, char** argv, char** azColName) {
    if (argc == 1) {
        int count = atoi(argv[0]);
        std::cout << "���е�����: " << count << std::endl;
    }
    return 0;
}

int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

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

int db_query_all_n()
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

        sqlite3_snprintf(512, cSql, "select count(*) from scale_result");
        nRes = sqlite3_exec(pDB, cSql, rowCountCallback, NULL, &pcErrMsg);
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
        nRes = sqlite3_exec(pDB, cSql, callback, NULL, &pcErrMsg);
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

int db_query_by_time_range_n(int start_time, int end_time)
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
        sqlite3_snprintf(512, cSql, "select count(*) from scale_result where create_time>%d and create_time<%d order by create_time desc", start_time, end_time);
        nRes = sqlite3_exec(pDB, cSql, rowCountCallback, NULL, &pcErrMsg);
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
        nRes = sqlite3_exec(pDB, cSql, callback, NULL, &pcErrMsg);
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
        nRes = sqlite3_exec(pDB, cSql, callback, NULL, &pcErrMsg);
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

int db_query_by_time_range2(int start_time, int end_time)
{
    char* pcErrMsg = NULL;
    sqlite3* pDB = NULL;
    // ��ʽ��SQL���
    char cSql[512] = { 0 };
    int ret = 0;
    sqlite3_stmt* stmt;
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
        nRes = sqlite3_prepare_v2(pDB, cSql, -1, &stmt, NULL);
        if (nRes != SQLITE_OK)
        {
            std::cerr << "׼��SQL���ʧ��: " << sqlite3_errmsg(pDB) << std::endl;
            ret = -2;
            break;
        }
        printf("insert test_table successful. \n");
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            // �������� ����
            ScaleWood scaleWood;
            //����ID
            int id = sqlite3_column_int(stmt, 0);

            //createtime
            scaleWood.id = sqlite3_column_int(stmt, 1);
            //amount
            int amount = sqlite3_column_int(stmt, 2);
            double lenght = sqlite3_column_double(stmt, 3);
            double total_volume = sqlite3_column_double(stmt, 4); 
            std::string wood_list = (const char*)sqlite3_column_text(stmt, 5);
        }
        sqlite3_finalize(stmt);
    } while (false);

    // �����ر��������ݿ�
    sqlite3_close(pDB);
    pDB = NULL;

    if (pcErrMsg != NULL)
    {
        sqlite3_free(pcErrMsg); //�ͷ��ڴ�
        pcErrMsg = NULL;
    }
    return 0;
}