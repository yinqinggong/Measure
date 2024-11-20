//httplib
#include "httplib.h"
#include "ScaleAPI.h"
#include "json.h"
#include "..\\..\\Measure\LogFile.h"
#include <tchar.h>

//正式地址
const char* g_scale_domain = "192.168.2.1";
const int   g_scale_port = 5000;

const char* g_cloud_scale_domain = "6oh00qla.cloud.lanyun.net";
const int   g_cloud_port = 8866;

const char* g_preview_api = "/preview";
const char* g_photo_api = "/photo";
#if CloudAPI
const char* g_scale_api = "/infer";
#else
const char* g_scale_api = "/scale";
#endif
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


int PostPhoto(std::string &limg, int& errorCode, std::string& rimg, std::string& m_camparam)
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
		// 新建 JSON 对象K1, D1, K2, D2, R, T, E, F, R1, R2, P1, P2, Q
		Json::Value root(Json::arrayValue);// 给 JSON 对象添加键值对
		root.append(value["K1"].asString());
		root.append(value["D1"].asString());
		root.append(value["K2"].asString());
		root.append(value["D2"].asString());
		root.append(value["R"].asString());
		root.append(value["T"].asString());
		root.append(value["E"].asString());
		root.append(value["F"].asString());
		root.append(value["R1"].asString());
		root.append(value["R2"].asString());
		root.append(value["P1"].asString());
		root.append(value["P2"].asString());
		root.append(value["Q"].asString());
		Json::StyledWriter writer;
		m_camparam = writer.write(root);// 将字符串转为 char*const char* data = json_str.c_str();// 打印结果
		if (m_camparam.length() > 0)
		{
			//去掉引号
			m_camparam.erase(std::remove_if(m_camparam.begin(), m_camparam.end(), [&](char ch) {
				return ch == '\"';
				}), m_camparam.end());
			//去掉换行
			m_camparam.erase(std::remove_if(m_camparam.begin(), m_camparam.end(), [&](char ch) {
				return ch == '\n';
				}), m_camparam.end());
			//去掉空格
			m_camparam.erase(std::remove_if(m_camparam.begin(), m_camparam.end(), [&](char ch) {
				return ch == ' ';
				}), m_camparam.end());
			//去掉内部[]
			std::string::size_type start_pos = 0;
			std::string from = "],[";
			std::string to = ",";
			while ((start_pos = m_camparam.find(from, start_pos)) != std::string::npos) {
				m_camparam.replace(start_pos, from.length(), to);
				start_pos += to.length(); // 防止无限循环，如果`to`包含`from`
			}
			m_camparam.erase(m_camparam.begin());
			m_camparam.erase(m_camparam.end() - 1);
		}
#endif
		errorCode = code;
		WriteLog(_T("PostPhoto - code: %d"), code);
		return 1;
	}
	errorCode = (int)res.error();
	WriteLog(_T("PostPhoto fail, httplib.err:%d"), res.error());
	return -1;
}

int PostScale(ScaleWood& scalewood, int& errorCode)
{
	httplib::Client cli(g_scale_domain, g_scale_port);
	cli.set_read_timeout(90, 0);
	auto res = cli.Post(g_scale_api);
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false))
		{
			errorCode = -1;
			WriteLog(_T("PostScale Json fail"));
			return -1;
		}
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
			errorCode = -2;
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

int PostInfer(ScaleWood& scalewood, int& errorCode, std::string& limg, std::string& rimg, std::string& m_camparam, int& w, int& h, int& c)
{
	httplib::Client cli(g_cloud_scale_domain, g_cloud_port);
	cli.set_read_timeout(90, 0);
	//limg = "123456";
	//rimg = "654321";
	std::string params = "{\"camparam\":" + m_camparam;
	params += ",\"limg\":\"" + limg;
	params += "\",\"rimg\":\"" + rimg;
	params += "\"}";
	auto res = cli.Post(g_scale_api, params, "application/json");
	if (res && res->status == 200) {
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(res->body, value, false))
		{
			errorCode = -1;
			WriteLog(_T("PostInfer Json fail"));
			return -1;
		}
		int code = value["error"].asInt();
		if (code != 0)
		{
			errorCode = code;
			WriteLog(_T("PostInfer fail, code:%d"), code);
			return -1;
		}
		/*std::ofstream file("D:\\example.txt", std::ios::out);
		if (!file.is_open()) {
			std::cerr << "无法打开文件" << std::endl;
			return 1;
		}
		file << res->body;
		file.close();*/
		Json::Value arrayWinfo = value["winfo"];
		if (arrayWinfo.size() <= 0)
		{
			errorCode = -2;
			WriteLog(_T("wood_list empty"));
			return -1;
		}

		for (size_t i = 0; i < arrayWinfo.size(); i++)
		{
			WoodAttr woodAttr = { 0 };

			std::string ellipseStr = arrayWinfo[i].asString();
			Json::Value valueEllipse;
			Json::Reader readerEllipse;

			if (!readerEllipse.parse(ellipseStr, valueEllipse, false))
			{
				errorCode = -3;
				WriteLog(_T("ellipseStr empty"));
				return -1;
			}

			woodAttr.ellipse.angel = valueEllipse["ellipse"]["angle"].asDouble();
			int j = 0;
			woodAttr.ellipse.ab1 = round(valueEllipse["ellipse"]["ab"][j].asDouble() / 2);
			woodAttr.ellipse.ab2 = round(valueEllipse["ellipse"]["ab"][++j].asDouble() / 2);
			j = 0;
			woodAttr.ellipse.cx = valueEllipse["ellipse"]["center"][j].asDouble();
			woodAttr.ellipse.cy = valueEllipse["ellipse"]["center"][++j].asDouble();
			j = 0;
			woodAttr.ellipse.lx1 = valueEllipse["longaxis"]["pt1"][j].asDouble();
			woodAttr.ellipse.lx2 = valueEllipse["longaxis"]["pt1"][++j].asDouble();
			j = 0;
			woodAttr.ellipse.ly1 = valueEllipse["longaxis"]["pt2"][j].asDouble();
			woodAttr.ellipse.ly2 = valueEllipse["longaxis"]["pt2"][++j].asDouble();
			j = 0;
			woodAttr.ellipse.sx1 = valueEllipse["shortaxis"]["pt1"][j].asDouble();
			woodAttr.ellipse.sx2 = valueEllipse["shortaxis"]["pt1"][++j].asDouble();
			j = 0;
			woodAttr.ellipse.sy1 = valueEllipse["shortaxis"]["pt2"][j].asDouble();
			woodAttr.ellipse.sy2 = valueEllipse["shortaxis"]["pt2"][++j].asDouble();

			woodAttr.diameters.d1 = round((valueEllipse["shortaxis"]["plength"].asDouble()) * 100 * 10) / 10.0;
			woodAttr.diameters.d2 = round((valueEllipse["longaxis"]["plength"].asDouble()) * 100 * 10) / 10.0;
			woodAttr.diameter = std::min(woodAttr.diameters.d1, woodAttr.diameters.d2);

			scalewood.wood_list.push_back(woodAttr);
		}

		Json::Value rgbData = value["rgb"];
		scalewood.img = rgbData["data"].asString();
		Json::Value arrayShape = rgbData["shape"];
		unsigned int i = 0;
		w = arrayShape[i++].asInt();
		h = arrayShape[i++].asInt();
		c = arrayShape[i++].asInt();
		
		errorCode = code;
		WriteLog(_T("PostInfer - code: %d"), code);
		return 1;
	}
	errorCode = (int)res.error();
	WriteLog(_T("PostInfer fail, httplib.err:%d"), res.error());
	return -1;
}
