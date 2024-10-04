//httplib
#include "httplib.h"
#include "ScaleAPI.h"
#include "json.h"
#include "..\\..\\Measure\LogFile.h"
#include <tchar.h>

//正式地址
const char* g_scale_domain = "192.168.2.1";
const int   g_scale_port = 5000;

const char* g_cloud_scale_domain = "api.tensorplus.cn";

const char* g_preview_api = "/preview";
const char* g_photo_api = "/photo";
const char* g_scale_api = "/scale";

int PostPreview(std::string& url)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(10, 0);
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


int PostPhoto(std::string &limg, int& errorCode, std::string& rimg,
	std::string& D1,
	std::string& D2,
	std::string& E,
	std::string& F,
	std::string& K1,
	std::string& K2,
	std::string& P1,
	std::string& P2,
	std::string& Q,
	std::string& R,
	std::string& R1,
	std::string& R2,
	std::string& T)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(20, 0);
	auto res = cli.Post(g_photo_api);
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false)) return -1;
		int code = value["code"].asInt();
		if (code != 200)
		{
			errorCode = code;
			WriteLog(_T("PostPhoto fail, code:%d"), code);
			return -1;
		}
		limg = value["limg"].asString();
#if CloudAPI
		rimg = value["rimg"].asString();
		//cam_params = value["cam_params"].asString();
		D1 = value["D1"].asString();
		D2 = value["D2"].asString();
		E = value["E"].asString();
		F = value["F"].asString();
		K1 = value["K1"].asString();
		K2 = value["K2"].asString();
		P1 = value["P1"].asString();
		P2 = value["P2"].asString();
		Q = value["Q"].asString();
		R = value["R"].asString();
		R1 = value["R1"].asString();
		R2 = value["R2"].asString();
		T = value["T"].asString();
#endif
		errorCode = code;
		WriteLog(_T("PostPhoto - code: %d"), code);
		return 1;
	}
	errorCode = (int)res.error();
	WriteLog(_T("PostPhoto fail, httplib.err:%d"), res.error());
	return -1;
}

int PostScale(ScaleWood& scalewood, int& errorCode, std::string& limg, std::string& rimg,
	std::string& D1,
	std::string& D2,
	std::string& E,
	std::string& F,
	std::string& K1,
	std::string& K2,
	std::string& P1,
	std::string& P2,
	std::string& Q,
	std::string& R,
	std::string& R1,
	std::string& R2,
	std::string& T)
{
#if CloudAPI
	httplib::Client cli(g_cloud_scale_domain, g_scale_port);
	cli.set_read_timeout(60, 0);
	httplib::Params params{
		{ "limg", limg },
		{ "rimg", rimg },
		{ "D1",D1},
		{ "D2",D2},
		{ "E",E},
		{ "F",F},
		{ "K1",K1},
		{ "K2",K2},
		{ "P1",P1},
		{ "P2",P2},
		{ "Q",Q},
		{ "R",R},
		{ "R1",R1},
		{ "R2",R2},
		{ "T",T }
	};
	auto res = cli.Post(g_scale_api, params);
#else
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(60, 0);
	auto res = cli.Post(g_scale_api);
#endif // Cloud_API
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false)) return -1;
		int code = value["code"].asInt();
		if (code != 200)
		{
			errorCode = code;
			WriteLog(_T("PostScale fail, code:%d"), code);
			return -1;
		}
		
		//scalewood.id = value["id"].asUInt();
		scalewood.img = value["img"].asString();
		Json::Value arrayData = value["wood_list"];
		if (arrayData.size() <= 0)
		{
			WriteLog(_T("wood_list empty"));
			return -1;
		}
		for (unsigned int i = 0; i < arrayData.size(); i++)
		{
			WoodAttr woodAttr = { 0 };
			woodAttr.diameter = arrayData[i]["diameter"].asDouble();
			woodAttr.volumn = arrayData[i]["volumn"].asDouble();
			Json::Value diametersData = arrayData[i]["diameters"];
			if (diametersData.size() == 2)
			{
				int j = 0;
				woodAttr.diameters.d1 = diametersData[j++].asDouble();
				woodAttr.diameters.d2 = diametersData[j].asDouble();
			}
			Json::Value ellipseData = arrayData[i]["ellipse"];
			if (ellipseData.size() == 13)
			{
				int j = 0;
				woodAttr.ellipse.cx = ellipseData[j++].asDouble();
				woodAttr.ellipse.cy = ellipseData[j++].asDouble();
				woodAttr.ellipse.ab1 = ellipseData[j++].asDouble();
				woodAttr.ellipse.ab2 = ellipseData[j++].asDouble();
				woodAttr.ellipse.angel = ellipseData[j++].asDouble();
				woodAttr.ellipse.lx1 = ellipseData[j++].asDouble();
				woodAttr.ellipse.ly1 = ellipseData[j++].asDouble();
				woodAttr.ellipse.lx2 = ellipseData[j++].asDouble();
				woodAttr.ellipse.ly2 = ellipseData[j++].asDouble();
				woodAttr.ellipse.sx1 = ellipseData[j++].asDouble();
				woodAttr.ellipse.sy1 = ellipseData[j++].asDouble();
				woodAttr.ellipse.sx2 = ellipseData[j++].asDouble();
				woodAttr.ellipse.sy2 = ellipseData[j++].asDouble();
			}
			scalewood.wood_list.push_back(woodAttr);
		}
		errorCode = code;
		WriteLog(_T("PostScale - code: %d"), code);
		return 1;
	}
	errorCode = (int)res.error();
	WriteLog(_T("PostScale fail, httplib.err:%d"), res.error());
	return -1;
}
