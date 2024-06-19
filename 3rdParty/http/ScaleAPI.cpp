//httplib
#include "httplib.h"
#include "ScaleAPI.h"
#include "json.h"
#include "..\\..\\Measure\LogFile.h"
#include <tchar.h>

//正式地址
const char* g_scale_domain = "192.168.2.1";
const int   g_scale_port = 5000;

const char* g_preview_api = "/preview";
const char* g_photo_api = "/photo";
const char* g_scale_api = "/scale";

int PostPreview(std::string& url)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(30, 0);
	auto res = cli.Post(g_preview_api);
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false)) return -1;
		int code = value["code"].asInt();
		if (code != 200)
		{
			WriteLog(_T("PostPreview fail, code:%d"), code);
			return -1;
		}

		url = value["url"].asString();
		
		WriteLog(_T("PostPreview - code: %d"), code);
		return 1;
	}
	WriteLog(_T("PostPreview fail"));
	return -1;
}


int PostPhoto(std::string &limg)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(30, 0);
	auto res = cli.Post(g_photo_api);
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false)) return -1;
		int code = value["code"].asInt();
		if (code != 200)
		{
			WriteLog(_T("PostPhoto fail, code:%d"), code);
			return -1;
		}

		limg = value["limg"].asString();

		WriteLog(_T("PostPhoto - code: %d"), code);
		return 1;
	}
	WriteLog(_T("PostPhoto fail"));
	return -1;
}

int PostScale(ScaleWood& scalewood)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(30, 0);
	auto res = cli.Post(g_scale_api);
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false)) return -1;
		int code = value["code"].asInt();
		if (code != 200)
		{
			WriteLog(_T("PostScale fail, code:%d"), code);
			return -1;
		}

		scalewood.img = value["img"].asString();

		WriteLog(_T("PostScale - code: %d"), code);
		return 1;
	}
	WriteLog(_T("PostScale fail"));
	return -1;
}
