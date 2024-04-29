#include "stdafx.h"
#include <Shlwapi.h>
#include "HttpUtil.h"
#include "CStringUtil.hpp"
#include "GlobalSetting.h"
#include "LogFile.h"

static CHttpUtil* g_httpUtil=NULL;
static CCriticalSection m_httpCS;

//#include "vld.h"
LJU_Handle CHttpUtil::CreatLJUHandleFromResponse(CString &response){
	int nPos = response.Find(_T("success"));
	if (nPos == -1)
	{
		return 0;
	}

	char* utf8Response = nullptr;
	int len = 0;
	CHttpUtil::UnicodeToUtf8(response, &utf8Response, &len);

	LJU_Handle lju = LJU_CreateFromBuffer(utf8Response, len);
	if (utf8Response)
	{
		delete[] utf8Response;
	}
	return lju;
}

CHttpUtil* CHttpUtil::GetInstance(){
	if (g_httpUtil==NULL)
	{
		g_httpUtil = new CHttpUtil();
	}
	return g_httpUtil;
}

int CHttpUtil::ReleaseInstance(){
	if (g_httpUtil)
	{
		delete g_httpUtil;
		g_httpUtil = NULL;
	}
	return 1;
}
CHttpUtil::CHttpUtil()
{
	m_webServerAddr = TEXT("http://zs.zrodo.com/nanjingtest");//todo: 通过配置文件读取
	m_token = TEXT("");
}


CHttpUtil::~CHttpUtil()
{
}

void CHttpUtil::UTF8ToUnicode(const char* strUTF8, CString &strUnicode) {
	int nLen = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCCH)strUTF8, -1, NULL, 0);  //返回需要的unicode长度,包括\0字符
	WCHAR * wszUnicode = new WCHAR[nLen];
	memset(wszUnicode, 0, nLen * 2);
	nLen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8, -1, wszUnicode, nLen);    //把utf8转成unicode
	strUnicode = wszUnicode;
	delete[] wszUnicode;
}
void  CHttpUtil::UnicodeToUtf8(const CString &unicode, char** strUTF8, int *strUTF8Len) {
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL); //返回需要的utf8长度,包括\0字符
	char *szUtf8 = new char[len];
	memset(szUtf8, 0, len);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, szUtf8, len, NULL, NULL);
	*strUTF8 = szUtf8;
	*strUTF8Len = len;
}
void  CHttpUtil::AnsiToUnicode(const char* strAnsi, CString &strUnicode) {
	int nLen = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, (LPCCH)strAnsi, -1, NULL, 0);  //返回需要的unicode长度,包括\0字符
	WCHAR * wszUnicode = new WCHAR[nLen];
	memset(wszUnicode, 0, nLen * 2);
	nLen = MultiByteToWideChar(CP_ACP, 0, (LPCCH)strAnsi, -1, wszUnicode, nLen);    //把utf8转成unicode
	strUnicode = wszUnicode;
	delete[] wszUnicode;
}
void  CHttpUtil::UnicodeToAnsi(const CString &unicode, char** strAnsi, int *strAnsiLen) {
	int len;
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL); //返回需要的utf8长度,包括\0字符
	char *szAnsi = new char[len];
	memset(szAnsi, 0, len);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, szAnsi, len, NULL, NULL);
	*strAnsi = szAnsi;
	*strAnsiLen = len;
}

int CHttpUtil::HttpGetRequest(CString url, CString &response, BOOL hasToken) {
	int ret = TRUE;
	response = TEXT("");

	CInternetSession session;
	CHttpFile *pHFile = nullptr;
	InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 6000);//网络连接超时
	session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 6000);//发送请求的超时时间
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 6000);//接受数据的超时时间
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 500);//两次重试之间的间隔时间
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);//超时重试次数，默认值是5


	try
	{
		CString headstr;
		headstr.Format(_T("token: %s"), m_token);
		if (hasToken)
		{
			pHFile = (CHttpFile*)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD, headstr, headstr.GetLength());
		}
		else{
			pHFile = (CHttpFile*)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
		}
		
		if (pHFile != nullptr)
		{
			DWORD dwRet;
			pHFile->QueryInfoStatusCode(dwRet);
			ret = dwRet;
			if (dwRet != HTTP_STATUS_OK)
			{
				CString logStr;
				logStr.Format(_T("Get出错，错误码：%d，url：%s"), dwRet, url);
				WriteLog(_T("%s"), logStr);
				session.Close();
				if (pHFile)
				{
					pHFile->Close();
					delete pHFile;
					pHFile = nullptr;
				}
				return FALSE;
			}
			else {
				CString utfbuf;
				while (pHFile->ReadString(utfbuf) > 0)
				{
					CString s;
					char *pStr = (char*)utfbuf.GetBuffer(utfbuf.GetLength());
					CHttpUtil::UTF8ToUnicode(pStr, s);
					response += s;
					utfbuf.ReleaseBuffer();
				}
			}
		}
		else
		{
			ret = FALSE;
		}
	}
	catch (CInternetException *e)
	{
		e->Delete();
		ret = FALSE;
	}

	session.Close();
	if (pHFile)
	{
		pHFile->Close();
		delete pHFile;
		pHFile = nullptr;
	}
	return ret;
}

int CHttpUtil::HttpDownloadFile(CString url, CString filePath) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http下载图片失败"));
		return FALSE;
	}
	int ret = TRUE;

	CInternetSession session;
	CHttpFile *pHFile = nullptr;
	InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 6000);//网络连接超时
	session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 6000);//发送请求的超时时间
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 6000);//接受数据的超时时间
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 500);//两次重试之间的间隔时间
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);//超时重试次数，默认值是5

	try
	{
		pHFile = (CHttpFile*)session.OpenURL(url, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
		if (pHFile != nullptr)
		{
			DWORD dwRet;
			pHFile->QueryInfoStatusCode(dwRet);
			ret = dwRet;
			if (dwRet != HTTP_STATUS_OK)
			{
				CString logStr;
				logStr.Format(_T("Get出错，错误码：%d，url：%s"), dwRet, url);
				WriteLog(_T("%s"), logStr);
				session.Close();
				if (pHFile)
				{
					pHFile->Close();
					delete pHFile;
					pHFile = nullptr;
				}
				return FALSE;
			}
			else {
				//int totallen = pHFile->GetLength();
				int numread = 0;
				char filebuf[2048];
				int k = sizeof(filebuf);
				CFile myfile(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
				while ((numread = pHFile->Read(filebuf, sizeof(filebuf) - 1)) > 0)
				{
					filebuf[numread] = '\0';
					myfile.Write(filebuf, numread);
				}
			}
		}
		else
		{
			ret = FALSE;
		}
	}
	catch (CInternetException *e)
	{
		e->Delete();
		ret = FALSE;
	}
	catch (CFileException *e)
	{
		//处理本地图片路径异常
		e->Delete();
		ret = FALSE;
	}

	session.Close();
	if (pHFile)
	{
		pHFile->Close();
		delete pHFile;
		pHFile = nullptr;
	}
	return ret;
}

int CHttpUtil::HttpPutRequest(CString url, CString &strHeaders, const char* postData, int iLen, CString &response) {
	response = TEXT("");
	int ret = TRUE;
	unsigned short nPort;       //用于保存目标HTTP服务端口
	CString strServer, strObject;   //strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType;      //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号
	//解析URL，获取信息
	if (!AfxParseURL(url, dwServiceType, strServer, strObject, nPort))
	{
		//解析失败，该Url不正确
		return false;
	}

	CInternetSession session;
	InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 60000);//网络连接超时
	session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 60000);//发送请求的超时时间
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 60000);//接受数据的超时时间
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 500);//两次重试之间的间隔时间
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 5);//超时重试次数，默认值是5
	DWORD dwRet = 0;
	//建立网络连接
	CHttpConnection *pConnection = session.GetHttpConnection(strServer, nPort);
	if (pConnection == NULL)
	{
		//建立网络连接失败
		session.Close();
		return false;
	}
	CHttpFile* pHFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_PUT, strObject);
	if (pHFile == NULL)
	{
		//发起GET请求失败
		session.Close();
		delete pConnection;
		pConnection = NULL;
		return false;
	}

	//开始发送请求
	try
	{
		pHFile->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)postData, iLen);
	}
	catch (CInternetException *e)
	{
		CString str;
		str.Format(TEXT("发送数据失败的原因是:%d"), e->m_dwError);
		WriteLog(_T("%s"), str);
		e->Delete();
	}

	pHFile->QueryInfoStatusCode(dwRet);
	ret = dwRet;
	int index = 0;
	if (dwRet != HTTP_STATUS_OK)
	{
		CString logStr;
		logStr.Format(_T("PUT出错，错误码：%d"), dwRet);
		WriteLog(_T("%s"), logStr);
		session.Close();
		if (pConnection)
		{
			delete pConnection;
			pConnection = NULL;
		}
		if (pHFile)
		{
			delete pHFile;
			pHFile = NULL;
		}
		return false;
	}
	else
	{
		CString utfbuf;
		while (pHFile->ReadString(utfbuf) > 0)
		{
			CString s;
			char *pStr = (char*)utfbuf.GetBuffer(utfbuf.GetLength());
			CHttpUtil::UTF8ToUnicode(pStr, s);
			response += s;
			utfbuf.ReleaseBuffer();
		}
	}
	session.Close();
	if (pConnection)
	{
		delete pConnection;
		pConnection = NULL;
	}
	if (pHFile)
	{
		delete pHFile;
		pHFile = NULL;
	}
	return ret;
}

int CHttpUtil::HttpPostRequest(CString url, CString &strHeaders, const char* postData, int iLen, CString &response){

	response = TEXT("");
	int ret = TRUE;
	unsigned short nPort;       //用于保存目标HTTP服务端口
	CString strServer, strObject;   //strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType;      //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号
	//解析URL，获取信息
	if (!AfxParseURL(url, dwServiceType, strServer, strObject, nPort))
	{
		//解析失败，该Url不正确
		return false;
	}

	CInternetSession session;
	InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 60000);//网络连接超时
	session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 60000);//发送请求的超时时间
	session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 60000);//接受数据的超时时间
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 500);//两次重试之间的间隔时间
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 5);//超时重试次数，默认值是5
	DWORD dwRet = 0;
	//建立网络连接
	CHttpConnection *pConnection = session.GetHttpConnection(strServer, nPort);
	if (pConnection == NULL)
	{
		//建立网络连接失败
		session.Close();
		return false;
	}
	CHttpFile* pHFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
	if (pHFile == NULL)
	{
		//发起GET请求失败
		session.Close();
		delete pConnection;
		pConnection = NULL;
		return false;
	}

	//开始发送请求
	try
	{
		pHFile->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)postData, iLen);
	}
	catch (CInternetException *e)
	{
		CString str;
		str.Format(TEXT("发送数据失败的原因是:%d"), e->m_dwError);
		WriteLog(_T("%s"), str);
		e->Delete();
	}

	pHFile->QueryInfoStatusCode(dwRet);
	ret = dwRet;
	int index = 0;
	if (dwRet != HTTP_STATUS_OK)
	{
		CString logStr;
		logStr.Format(_T("PUT出错，错误码：%d"), dwRet);
		WriteLog(_T("%s"), logStr);
		session.Close();
		if (pConnection)
		{
			delete pConnection;
			pConnection = NULL;
		}
		if (pHFile)
		{
			delete pHFile;
			pHFile = NULL;
		}
		return false;
	}
	else
	{
		CString utfbuf;
		while (pHFile->ReadString(utfbuf) > 0)
		{
			CString s;
			char *pStr = (char*)utfbuf.GetBuffer(utfbuf.GetLength());
			CHttpUtil::UTF8ToUnicode(pStr, s);
			response += s;
			utfbuf.ReleaseBuffer();
		}
	}
	session.Close();
	if (pConnection)
	{
		delete pConnection;
		pConnection = NULL;
	}
	if (pHFile)
	{
		delete pHFile;
		pHFile = NULL;
	}
	return ret;
}
int CHttpUtil::LogIn(CString username, CString password, CString &response) {
	CString url = TEXT("");
	url.Format(TEXT("%s/login?username=%s&password=%s"), m_webServerAddr, username, password);
	return HttpGetRequest(url, response);
}

int CHttpUtil::FaceUpload(CString operatorId, CString marketId, CString filePath,  CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http上传图片失败"));
		return -1;
	}
	//put form-data multipart

	//打开图片文件
	BOOL  fexist = PathFileExists(filePath);
	if (fexist == FALSE){
		AfxMessageBox(TEXT("找不到人脸文件 "));
		return -1;
	}
	CFile faceFile;
	char* pngData = NULL;
	int pngFileLen = 0;
	try
	{
		if (!faceFile.Open(filePath, CFile::modeRead))
		{
			TRACE(TEXT("FaceUpload:没有文件!"));
			return -2;
		}
		
		pngFileLen = faceFile.GetLength();
		pngData = new char[pngFileLen];
		faceFile.Read(pngData, pngFileLen);
		faceFile.Close();
	}
	catch (CFileException *e)
	{
		CString str;
		str.Format(TEXT("读取数据失败的原因是:%d"), e->m_cause);
		AfxMessageBox(str);
		faceFile.Abort();
		e->Delete();
	}
	//构造url
	CString fileFullName = CStringUtil::GetFileFullName(filePath);
	CString fileExt = CStringUtil::GetFileExtName(fileFullName);
	if (fileFullName.IsEmpty() || fileExt.IsEmpty())
	{
		AfxMessageBox(TEXT("解析图片文件名出错"));
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/faceupload?operatorid=%s&marketid=%s&filename=%s"), 
		m_webServerAddr, 
		operatorId,
		marketId,
		fileFullName);
	//构造header
	CString headers;
	headers.Format(_T("token: %s\r\n"), m_token);
	headers.Append(TEXT("Content-Type: multipart/form-data; charset=utf-8; boundary=__X_PAW_BOUNDARY__"));
	
	//构造body
	CString postDataHeader =TEXT("\r\n--__X_PAW_BOUNDARY__\r\n");
	
	CString formdata;
	formdata.Format(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"),fileFullName);
	
	CString contenttype;
	CString ftype;
	if (fileExt.Compare(TEXT("jpg")) == 0 || fileExt.Compare(TEXT("jpeg")) == 0)
	{
		ftype = TEXT("jpeg");
	}
	else if (fileExt.Compare(TEXT(".png")) == 0)
	{
		ftype = TEXT("png");
	}
	contenttype.Format(TEXT("Content-Type: image/%s\r\n\r\n"), ftype);
	postDataHeader = postDataHeader + formdata + contenttype;
	char *postDataHeaderUtf8 = NULL;
	int postDataHeadUtf8Len = 0;
	UnicodeToUtf8(postDataHeader, &postDataHeaderUtf8, &postDataHeadUtf8Len);
	postDataHeadUtf8Len -= 1;//去除\0字符

	char *postDataTail = "\r\n--__X_PAW_BOUNDARY__--\r\n";
	int postDataTailLen = strlen(postDataTail);

	int postDataTotalLen = postDataHeadUtf8Len + pngFileLen + postDataTailLen;
	char *postData = new char[postDataTotalLen];
	memset(postData, 0, postDataTotalLen);
	memcpy(postData, postDataHeaderUtf8, postDataHeadUtf8Len);
	memcpy(postData + postDataHeadUtf8Len, pngData, pngFileLen);
	memcpy(postData + postDataHeadUtf8Len + pngFileLen, postDataTail, postDataTailLen);

	//int ret = HttpPutRequest(url, headers, postData, postDataTotalLen, response);
	int ret = HttpPostRequest(url, headers, postData, postDataTotalLen, response);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}

	if (pngData)
	{
		delete[] pngData;
	}
	if (postData)
	{
		delete[] postData;
	}
	if (postDataHeaderUtf8)
	{
		delete[] postDataHeaderUtf8;
	}
	return ret;
}


int CHttpUtil::FaceGet(CString operatorId,  CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http获取图片失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/faceget?operatorid=%s"), m_webServerAddr, operatorId);
	int ret = HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::FaceDelete(CString operatorId, CString marketId, CString  filePathOnServer,  CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http删除图片失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/facedelete?operatorid=%s&marketid=%s&file=%s"), m_webServerAddr,
		operatorId,
		marketId,
		filePathOnServer);
	int ret = HttpGetRequest(url, response, TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::FaceCheckIn(CString operatorId, CString marketId, CString filePath,  CString &response){
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致人脸识别上传图片失败"));
		return -1;
	}
	//put form-data multipart

	//打开图片文件
	BOOL  fexist = PathFileExists(filePath);
	if (fexist == FALSE){
		AfxMessageBox(TEXT("找不到人脸文件 "));
		return -1;
	}
	CFile faceFile;
	char* pngData = NULL;
	int pngFileLen = 0;
	try
	{
		if (!faceFile.Open(filePath, CFile::modeRead))
		{
			TRACE(TEXT("FaceCheckIn:没有文件!"));
			return -2;
		}

		pngFileLen = faceFile.GetLength();
		pngData = new char[pngFileLen];
		faceFile.Read(pngData, pngFileLen);
		faceFile.Close();
	}
	catch (CFileException *e)
	{
		CString str;
		str.Format(TEXT("读取数据失败的原因是:%d"), e->m_cause);
		AfxMessageBox(str);
		faceFile.Abort();
		e->Delete();
	}
	//构造url
	CString fileFullName = CStringUtil::GetFileFullName(filePath);
	CString fileExt = CStringUtil::GetFileExtName(fileFullName);
	if (fileFullName.IsEmpty() || fileExt.IsEmpty())
	{
		AfxMessageBox(TEXT("解析图片文件名出错"));
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/facecheckin?operatorid=%s&marketid=%s&filename=%s"),
		m_webServerAddr,
		operatorId,
		marketId,
		fileFullName);
	//构造header
	CString headers;
	headers.Format(_T("token: %s\r\n"), m_token);
	headers.Append(TEXT("Content-Type: multipart/form-data; charset=utf-8; boundary=__X_PAW_BOUNDARY__"));

	//构造body
	CString postDataHeader = TEXT("\r\n--__X_PAW_BOUNDARY__\r\n");

	CString formdata;
	formdata.Format(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"), fileFullName);

	CString contenttype;
	CString ftype;
	if (fileExt.Compare(TEXT("jpg")) == 0 || fileExt.Compare(TEXT("jpeg")) == 0)
	{
		ftype = TEXT("jpeg");
	}
	else if (fileExt.Compare(TEXT(".png")) == 0)
	{
		ftype = TEXT("png");
	}
	contenttype.Format(TEXT("Content-Type: image/%s\r\n\r\n"), ftype);
	postDataHeader = postDataHeader + formdata + contenttype;
	char *postDataHeaderUtf8 = NULL;
	int postDataHeadUtf8Len = 0;
	UnicodeToUtf8(postDataHeader, &postDataHeaderUtf8, &postDataHeadUtf8Len);
	postDataHeadUtf8Len -= 1;//去除\0字符

	char *postDataTail = "\r\n--__X_PAW_BOUNDARY__--\r\n";
	int postDataTailLen = strlen(postDataTail);

	int postDataTotalLen = postDataHeadUtf8Len + pngFileLen + postDataTailLen;
	char *postData = new char[postDataTotalLen];
	memset(postData, 0, postDataTotalLen);
	memcpy(postData, postDataHeaderUtf8, postDataHeadUtf8Len);
	memcpy(postData + postDataHeadUtf8Len, pngData, pngFileLen);
	memcpy(postData + postDataHeadUtf8Len + pngFileLen, postDataTail, postDataTailLen);

	//int ret = HttpPutRequest(url, headers, postData, postDataTotalLen, response);
	int ret = HttpPostRequest(url, headers, postData, postDataTotalLen, response);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	if (pngData)
	{
		delete[] pngData;
	}
	if (postData)
	{
		delete[] postData;
	}
	if (postDataHeaderUtf8)
	{
		delete[] postDataHeaderUtf8;
	}
	return ret;
}

int CHttpUtil::FaceQueryCheckIn(CString operatorId, CString begindate, CString enddate,  CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http查询签到失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/facequerycheckin?operatorid=%s&begindate=%s&enddate=%s"), m_webServerAddr,
		operatorId,
		begindate,
		enddate);
	int ret = HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::BehaviorRectGet(CString marketId, CString machineId,  CString &response, int &x, int &y, int &w, int &h) {
	x = 0;
	y = 0;
	w = 0;
	h = 0;
	CString url = TEXT("");
	url.Format(TEXT("%s/behaviorrectget?marketid=%s&machineid=%s"), m_webServerAddr,
		marketId,
		machineId);
	
	HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response)==FALSE)
	{
		return -1;
	}
	LJU_Handle lju = CreatLJUHandleFromResponse(response);
	if (lju)
	{
		if (LJU_StartReadStruct(lju, "", "rect", 0)){
			LJU_ReadInt(lju, "", "x", 0, &x, 0);
			LJU_ReadInt(lju, "", "y", 0, &y, 0);
			LJU_ReadInt(lju, "", "w", 0, &w, 0);
			LJU_ReadInt(lju, "", "h", 0, &h, 0);
			LJU_EndReadWrite(lju);
		}
		LJU_Release(&lju);
	}else
	{
		return -1;
	}
	return 1;
}

int CHttpUtil::ParametersGet(){
	CString url = TEXT("");
	url.Format(TEXT("%s/parametersget"), m_webServerAddr);
	CString response;
	HttpGetRequest(url, response, TRUE);
	if (IsTokenInvalid(response) == TRUE)
	{
		return -2;
	}
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	LJU_Handle lju = CreatLJUHandleFromResponse(response);
	if (lju)
	{
		if (LJU_StartReadStruct(lju, "", "params", 0)){
			int faceRecoSplitTime = 10, eventRecoFinishTime = 20, eventNoWorkerTimeInterval = 40;
			int threshFaceDetConf = 5, threshMinFaceSize = 40, threshFaceAngleRoll = 20;
			double faceMatchSimilarity = 0.75, minFaceRatio = 0.1;
			CString newVersion;
			char tmpdata[256];
			memset(tmpdata, 0, 256);

			LJU_ReadInt(lju, "", "faceRecoSplitTime", 0, &faceRecoSplitTime, 0);
			LJU_ReadInt(lju, "", "eventRecoFinishTime", 0, &eventRecoFinishTime, 0);
			LJU_ReadInt(lju, "", "eventNoWorkerTimeInterval", 0, &eventNoWorkerTimeInterval, 0);
			LJU_ReadInt(lju, "", "threshFaceDetConf", 0, &threshFaceDetConf, 0);
			LJU_ReadInt(lju, "", "threshMinFaceSize", 0, &threshMinFaceSize, 0);
			LJU_ReadInt(lju, "", "threshFaceAngleRoll", 0, &threshFaceAngleRoll, 0);
			LJU_ReadDouble(lju, "", "faceMatchSimilarity", 0, &faceMatchSimilarity, 0);
			LJU_ReadString(lju, "", "newVersion", "", 0, tmpdata, 256, 0);
			CHttpUtil::UTF8ToUnicode(tmpdata, newVersion);
			LJU_ReadDouble(lju, "", "minFaceRatio", 0, &minFaceRatio, 0);
			//测试更新
			//newVersion = _T("1.0.2.1");

			LJU_EndReadWrite(lju);

			CGlobalSetting::GetInstance()->m_faceRecoSplitTime = faceRecoSplitTime>0 ? faceRecoSplitTime : 10;
			CGlobalSetting::GetInstance()->m_eventRecoFinishTime = eventRecoFinishTime>0 ? eventRecoFinishTime : 20;
			CGlobalSetting::GetInstance()->m_eventNoWorkerTimeInterval = eventNoWorkerTimeInterval>0 ? eventNoWorkerTimeInterval : 40;
			CGlobalSetting::GetInstance()->m_threshFaceDetConf = threshFaceDetConf>0 ? threshFaceDetConf : 5;
			CGlobalSetting::GetInstance()->m_threshMinFaceSize = threshMinFaceSize>0 ? threshMinFaceSize : 40;
			CGlobalSetting::GetInstance()->m_threshFaceAngleRoll = threshFaceAngleRoll>0 ? threshFaceAngleRoll : 20;
			CGlobalSetting::GetInstance()->m_faceMatchSimilarity = faceMatchSimilarity>0 ? faceMatchSimilarity : 0.75;
			CGlobalSetting::GetInstance()->m_newVersion = newVersion;
			CGlobalSetting::GetInstance()->m_minFaceRatio = minFaceRatio > 0 ? minFaceRatio : 0.1;
		}
		LJU_Release(&lju);
		
	}
	else
	{
		return -1;
	}
	return 1;
}
int CHttpUtil::BehaviorRectSet(CString marketId, CString machineId,  int x, int y, int w, int h, CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http设置区域失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/behaviorrectset?marketid=%s&machineid=%s&x=%d&y=%d&w=%d&h=%d"), m_webServerAddr,
		marketId,
		machineId,
		 x, y, w, h);
	int ret = HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::BehaviorNew(CString marketId, CString machineId, 
	CString date, CString beginTime, CString endTime, int behaviorType, CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http行为分析失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/behaviornew?marketid=%s&machineid=%s&date=%s&begintime=%s&endtime=%s&type=%d"), m_webServerAddr,
		marketId,
		machineId,
		 date, beginTime, endTime, behaviorType);
	int ret = HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::BehaviorQuery(CString marketId, CString machineId,  CString beginTime, CString endTime, CString &response) {
	if (CGlobalSetting::GetInstance()->m_loginSuccess == FALSE)
	{
		WriteLog(_T("登录失败导致http行为分析查询失败"));
		return -1;
	}
	CString url = TEXT("");
	url.Format(TEXT("%s/behaviorquery?marketid=%s&machineid=%s&begindate=%s&enddate=%s"), m_webServerAddr,
		marketId,
		machineId,
		 beginTime, endTime);
	int ret = HttpGetRequest(url, response,TRUE);
	if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}
	return ret;
}

int CHttpUtil::SetToken(CString token){
	m_token = token;
	return 1;
}

BOOL CHttpUtil::IsContainSuccessedStatus(CString& response){
	int nPos = response.Find(_T("success"));
	if (nPos != -1)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}


BOOL CHttpUtil::IsTokenInvalid(CString& response) {
	int mPos = response.Find(_T("respCode"));
	int nPos = response.Find(_T("1000007"));
	if (mPos != -1 && nPos != -1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}


}

int  CHttpUtil::CheckServerHeart(){
	return ParametersGet();
	/*CString response;
	int a,b,c,d;
	CGlobalSetting* gs = CGlobalSetting::GetInstance();
	BehaviorRectGet(gs->GetMarketId(), gs->GetMachineId(), response, a, b, c, d);
	return IsContainSuccessedStatus(response);*/
}

int CHttpUtil::ImageDetection(CString imageId, int type, CString param, char* image_data, int data_len, CString &response) {

	int ret = -1;
	if (!image_data) return -1;

	CString url = TEXT("");
	url.Format(TEXT("%s/imagedetection?imageid=%s&type=%d&param=%s"),
		TEXT("http://192.168.1.112:8000"),
		imageId,
		type,
		param);
	//构造header
	CString headers = TEXT("Content-Type: application/octet-stream;");
	ret = HttpPostRequest(url, headers, image_data, data_len, response);

	/*if (IsContainSuccessedStatus(response) == FALSE)
	{
		return -1;
	}*/

	return ret;
}