#include <iostream>
#include <asio.hpp>
#include <vector>
#include <fstream>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>
#include "AsioAPI.h"
#include "json.h"
#include "LogFile.h"

std::string img_file_name;

static void send_request(asio::ip::tcp::socket& socket, const std::string& command)
{
    // 在请求命令后添加结束标志
    std::string request = command + "\n";
    asio::write(socket, asio::buffer(request));
}

static void receive_response(asio::ip::tcp::socket& socket)
{
    try
    {
        // 读取响应，直到遇到 "\n"
        asio::streambuf buf;
        asio::read_until(socket, buf, "\n");
        std::cout << buf.size() << std::endl;
        std::istream is(&buf);

        std::string response;
        std::getline(is, response);
        std::cout << response.size() << std::endl;

        std::cout << "Received response: " << response << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error receiving response: " << e.what() << std::endl;
    }
}

static void receive_image(asio::ip::tcp::socket& socket, const std::string& img_file_path)
{
    try
    {
        uint32_t size;
        asio::read(socket, asio::buffer(&size, sizeof(size)));
        std::cout << "image size: " << size << std::endl;

        std::vector<char> image_data(size);
        asio::read(socket, asio::buffer(image_data));

        // 读取结束标志
        asio::streambuf buf;
        asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string end_signal;
        std::getline(is, end_signal);  // 获取 "\n" 标志

        std::cout << "Image received, saving as: " << img_file_path << std::endl;
        std::ofstream file(img_file_path, std::ios::binary);
        file.write(image_data.data(), image_data.size());
        // 图像解码
        //cv::Mat image_2d = cv::imdecode(image_data, cv::IMREAD_UNCHANGED);
        // for debug
        //std::cout << "image size: " << image_2d.cols << ", " << image_2d.rows << std::endl;
        //cv::imwrite("1.png", image_2d);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error receiving image: " << e.what() << std::endl;
    }
}

static DWORD __stdcall capture_thread(LPVOID param)
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    int cam_id = *((int*)param);

    if (cam_id == 1)
    {
        auto endpoints = resolver.resolve("192.168.1.211", "12345");
        asio::connect(socket, endpoints);
        std::cout << "connect left cam succeed" << std::endl;
    }
    else
    {
        auto endpoints = resolver.resolve("192.168.1.212", "12345");
        asio::connect(socket, endpoints);
        std::cout << "connect right cam succeed" << std::endl;
    }

    send_request(socket, "capture_2d");
    if (cam_id == 1)
    {
        std::string img_file_path = "./left/" + img_file_name;
        receive_image(socket, img_file_path);
    }
    else
    {
        std::string img_file_path = "./right/" + img_file_name;
        receive_image(socket, img_file_path);
    }
    socket.close();

    return 0;
}

static void stereo_capture()
{
    int left_cam_id = 1;
    int right_cam_id = 2;
    HANDLE h_thread1, h_thread2;

    std::time_t time_stamp = std::time(0);
    img_file_name = std::to_string(time_stamp) + ".png";
    std::cout << "image file name: " << img_file_name << std::endl;

    h_thread1 = CreateThread(NULL, 0, capture_thread, (void*)&left_cam_id, 0, NULL);
    h_thread2 = CreateThread(NULL, 0, capture_thread, (void*)&right_cam_id, 0, NULL);
    if (h_thread1 != NULL)
        WaitForSingleObject(h_thread1, INFINITE);
    if (h_thread2 != NULL)
        WaitForSingleObject(h_thread2, INFINITE);
}

static void test_api()
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("192.168.1.110", "12346");

    try
    {
        // 连接到 TCPServer_GPU
        asio::connect(socket, endpoints);
        std::cout << "connect server succeed" << std::endl;

        // intrinsics
        //send_request(socket, "intrinsics");
        //receive_response(socket);

        // capture_2d
        //std::time_t time_stamp = std::time(0);
        //img_file_name = "./captured_images/" + std::to_string(time_stamp) + ".png";
        //send_request(socket, "capture_2d");
        //receive_image(socket, img_file_name);

        // chessboard_pose
        //std::cout << "chessobard pose" << std::endl;
        //send_request(socket, "chessboard_pose");
        //receive_response(socket);

        // solar_panel_pose
        //std::cout << "solar panel pose" << std::endl;
        //send_request(socket, "solar_panel_pose2");
        //receive_response(socket);

        // 保持连接打开，直到所有请求都完成
        std::cout << "All requests processed, closing connection." << std::endl;
        socket.close();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

bool log_scale(const std::string& ip_addr, const std::string& port, const std::string& lfileName, const std::string& rfileName, const std::string& imagePath, ScaleWood& scaleWood, int wndIndex, 
    int debarked, 
    float fDebark_4_8,
    float fDebark_8_10,
    float fDebark_10_16,
    float fDebark_16_20,
    float fDebark_20)
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(ip_addr, port);

    try
    {
        // 连接到 TCPServer_GPU
        asio::connect(socket, endpoints);
        std::cout << "connect server succeed" << std::endl;

        cv::Mat left_img = cv::imread(lfileName);
        cv::Mat right_img = cv::imread(rfileName);

        asio::write(socket, asio::buffer("log-scale "));
        if (g_wnd_num > 1)
        {
            //log-scale接口添加 相机ID 范围：1-4
            asio::write(socket, asio::buffer(std::to_string(wndIndex + 1) + " "));
        }
        std::vector<uchar> img_data;
        uint32_t size;

        cv::imencode(".png", left_img, img_data);
        std::cout << "send left image begin" << std::endl;
        size = img_data.size();
        asio::write(socket, asio::buffer(&size, sizeof(size)));
        asio::write(socket, asio::buffer(img_data));
        std::cout << "send left image end" << std::endl;

        cv::imencode(".png", right_img, img_data);
        std::cout << "send right image begin" << std::endl;
        size = img_data.size();
        asio::write(socket, asio::buffer(&size, sizeof(size)));
        asio::write(socket, asio::buffer(img_data));
        std::cout << "send right image end" << std::endl;


        asio::read(socket, asio::buffer(&size, sizeof(size)));
        std::cout << "image size: " << size << std::endl;

        std::vector<char> result_image_data(size);
        asio::read(socket, asio::buffer(result_image_data));

        asio::read(socket, asio::buffer(&size, sizeof(size)));
        std::cout << "json size: " << size << std::endl;

        std::vector<char> json_data(size);
        asio::read(socket, asio::buffer(json_data));
        std::string json_string;
        json_string.assign(json_data.begin(), json_data.end());
        std::cout << json_string << std::endl;

        // 保持连接打开，直到所有请求都完成
        std::cout << "All requests processed, closing connection." << std::endl;
        socket.close();

        //写图片文件
        std::ofstream file(imagePath, std::ios::binary);
        file.write(result_image_data.data(), result_image_data.size());

        //解析json_string
        {
            Json::Value arrayData;
            Json::Reader reader;
            if (!reader.parse(json_string, arrayData, false))
            {
                //WriteLog(_T("PostScale Json fail"));
                return -1;
            }
            if (arrayData.size() <= 0)
            {
                //WriteLog(_T("wood_list empty"));
                return -1;
            }
            for (unsigned int i = 0; i < arrayData.size(); i++)
            {
                WoodAttr woodAttr = { 0 };
                woodAttr.index = wndIndex;
                woodAttr.ellipse.cx = arrayData[i]["cx_2d"].asDouble();
                woodAttr.ellipse.cy = arrayData[i]["cy_2d"].asDouble();
                woodAttr.ellipse.ab1 = arrayData[i]["rx_2d"].asDouble();
                woodAttr.ellipse.ab2 = arrayData[i]["ry_2d"].asDouble();
                woodAttr.ellipse.angel = arrayData[i]["angel"].asDouble();
                //短直径与长直接从mm转为cm
                woodAttr.diameters.d1 = arrayData[i]["r_long"].asDouble() * 0.1;
                woodAttr.diameters.d2 = arrayData[i]["r_short"].asDouble() * 0.1;
                woodAttr.diameter = woodAttr.diameters.d1 < woodAttr.diameters.d2 ? woodAttr.diameters.d1 : woodAttr.diameters.d2;
                /* 去皮公式
                径级4-8㎝，减去0.1
                径级8-10㎝，减去0.2
                径级10-16㎝，减去0.4
                径级16cm-20cm，减去0.5
                径级20cm及以上，减去0.6
                */
                if (debarked == 1)
                {
                    if (woodAttr.diameter >= 4.0 && woodAttr.diameter < 8.0)
                    {
                        woodAttr.diameter -= fDebark_4_8;
                        woodAttr.diameters.d1 -= fDebark_4_8;
                        woodAttr.diameters.d2 -= fDebark_4_8;
                    }
                    else if (woodAttr.diameter >= 8.0 && woodAttr.diameter < 10.0)
                    {
                        woodAttr.diameter -= fDebark_8_10;
                        woodAttr.diameters.d1 -= fDebark_8_10;
                        woodAttr.diameters.d2 -= fDebark_8_10;
                    }
                    else if (woodAttr.diameter >= 10.0 && woodAttr.diameter < 16.0)
                    {
                        woodAttr.diameter -= fDebark_10_16;
                        woodAttr.diameters.d1 -= fDebark_10_16;
                        woodAttr.diameters.d2 -= fDebark_10_16;
                    }
                    else if (woodAttr.diameter >= 16.0 && woodAttr.diameter < 20.0)
                    {
                        woodAttr.diameter -= fDebark_16_20;
                        woodAttr.diameters.d1 -= fDebark_16_20;
                        woodAttr.diameters.d2 -= fDebark_16_20;
                    }
                    else if (woodAttr.diameter >= 20.0)
                    {
                        woodAttr.diameter -= fDebark_20;
                        woodAttr.diameters.d1 -= fDebark_20;
                        woodAttr.diameters.d2 -= fDebark_20;
                    }
                    else
                    {

                    }
                }
                //木材横截面椭圆在3D相机坐标系下的坐标，单位mm
                woodAttr.ellipse.cx_3d = arrayData[i]["cx_3d"].asDouble();
                woodAttr.ellipse.cy_3d = arrayData[i]["cy_3d"].asDouble();
                woodAttr.ellipse.cz_3d = arrayData[i]["cz_3d"].asDouble();

                scaleWood.wood_list.push_back(woodAttr);
            }
            //WriteLog(_T("PostScale - code: %d"), code);
            return 1;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool capture_2d(const std::string& ip, const std::string& port, std::string& file_name)
{
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);

	auto endpoints = resolver.resolve(ip, port);
    try
    {
        asio::connect(socket, endpoints);
        std::cout << file_name << ": connect cam succeed" << std::endl;
        send_request(socket, "capture_2d");
        receive_image(socket, file_name);
        socket.close();
    }
    catch (const std::exception&)
    {
        return false;
    }
	
    return true;
}