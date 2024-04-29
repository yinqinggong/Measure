#pragma once
#include <afxinet.h>
#include "LibJsonUtil.hpp"

class CHttpUtil
{

private:
	CHttpUtil();
public:
    ~CHttpUtil();
public:
	static CHttpUtil* GetInstance();//第一次使用时需要设置服务器地址
	static int ReleaseInstance();//程序退出时需要调用该函数释放资源
public:
    //字符转换功能
    static void  UTF8ToUnicode(const char* strUTF8, CString &strUnicode);
    static void  UnicodeToUtf8(const CString &unicode, char** strUTF8, int *strUTF8Len);
    static void  AnsiToUnicode(const char* strAnsi, CString &strUnicode);
    static void  UnicodeToAnsi(const CString &unicode, char** strAnsi, int *strAnsiLen);
public:
	void SetServerAddr(CString serverAddr){
		m_webServerAddr = serverAddr;
	}
    //基础http通信功能
    int HttpGetRequest(CString url, CString &response,BOOL hasToken=FALSE);
    static int HttpDownloadFile(CString url, CString filePath);
    static int HttpPutRequest(CString url, CString &strHeaders, const char* postData, int iLen, CString &response);
	static int HttpPostRequest(CString url, CString &strHeaders, const char* postData, int iLen, CString &response);
	static LJU_Handle   CreatLJUHandleFromResponse(CString &response);
public:
    //业务相关http通信功能

    //登录
    int LogIn(CString username, CString password, CString &response);

    //检测师人脸上传
	int FaceUpload(CString operatorId, CString marketId, CString filePath, CString &response);
    //检测师人脸获取 ，从response中获取出下载地址后调用HttpDownloadFile方法下载文件
    int FaceGet(CString operatorId, CString &response);
    //检测师人脸删除
    int FaceDelete(CString operatorId, CString marketId, CString  filePathOnServer, CString &response);
    //检测师人脸签到
	int FaceCheckIn(CString operatorId, CString marketId, CString filePath, CString &response);
    //检测师人脸签到查询
    int FaceQueryCheckIn(CString operatorId, CString begindate, CString enddate, CString &response);

    //行为分析区域获取
    int BehaviorRectGet(CString marketId, CString machineId, CString &response,int &x,int &y, int &w,int &h);
    //行为分析区域更新
    int BehaviorRectSet(CString marketId, CString machineId, int x, int y, int w, int h, CString &response);
    //行为上传
    int BehaviorNew(CString marketId, CString machineId,
                    CString date, CString beginTime, CString endTime, int behaviorType, CString &response);
    //行为记录查询
    int BehaviorQuery(CString marketId, CString machineId, CString beginTime, CString endTime, CString &response);

	//获取参数
	int ParametersGet();

	int SetToken(CString token);
	CString GetToken(){ return m_token; }
	//检查服务器心跳
	int CheckServerHeart();
	//图像检测
	int ImageDetection(CString imageId, int type, CString param, char* image_data, int data_len, CString &response);
public:
	//解析response内容
	
	static BOOL IsContainSuccessedStatus(CString& response);
	static BOOL IsTokenInvalid(CString& response);

private:
    CString m_webServerAddr;
	CString m_token;
};

