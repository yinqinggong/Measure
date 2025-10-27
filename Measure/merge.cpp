#include "merge.h"
#include <string.h>
#include <fstream>
#include <ctime>
#include <string>

static const float kMAX_X_IN_LEFT = -10000.0f;
static const float kMIN_X_IN_RIGHT = 10000.0f;
static const float kX_EPS = 300.0f;


void log_scale_merge(const std::vector<ScaleData> &results1, const std::vector<ScaleData> &results2,
                    const cv::Mat &R, const cv::Mat &t, 
                    float x_offset, float y_offset, float z_offset, float x_margin, float threshold,
                    std::vector<int> &flags1, std::vector<int> &flags2)
{
    // 初始化标志位数组，保留所有木材
    flags1.resize(results1.size());
    for (size_t i = 0; i < results1.size(); i++)
        flags1[i] = 1;
    flags2.resize(results2.size());
    for (size_t i = 0; i < results2.size(); i++)
        flags2[i] = 1;

    // 对特殊情况进行判断
    if (results1.size() == 0 || results2.size() == 0)
        return;

    // 生成左相机木材的3D坐标矩阵, 3 x M
    cv::Mat pos1 = cv::Mat::zeros(3, results1.size(), CV_32FC1);
    for (size_t i = 0; i < results1.size(); i++)
    {
        pos1.at<float>(0, i) = results1[i].center_3d.x;
        pos1.at<float>(1, i) = results1[i].center_3d.y;
        pos1.at<float>(2, i) = results1[i].center_3d.z;
    }

    // 生成右相机木材的3D坐标矩阵，3 x N
    cv::Mat pos2 = cv::Mat::zeros(3, results2.size(), CV_32FC1);
    for (size_t i = 0; i < results2.size(); i++)
    {
        pos2.at<float>(0, i) = results2[i].center_3d.x;
        pos2.at<float>(1, i) = results2[i].center_3d.y;
        pos2.at<float>(2, i) = results2[i].center_3d.z;
    }
    
    // 将左相机中的木材变换到右相机的坐标系
    cv::Mat pos1_in_2 = R * pos1;
    for (int i = 0; i < pos1_in_2.cols; i++)
    {
        pos1_in_2.at<float>(0, i) = pos1_in_2.at<float>(0, i) + t.at<float>(0, 0);
        pos1_in_2.at<float>(1, i) = pos1_in_2.at<float>(1, i) + t.at<float>(1, 0);
        pos1_in_2.at<float>(2, i) = pos1_in_2.at<float>(2, i) + t.at<float>(2, 0);
    }

    // 计算左相机图像中最右侧木材的X坐标
    float x1_max = kMAX_X_IN_LEFT;
    for (int i = 0; i < pos1_in_2.cols; i++)
    {
        if (pos1_in_2.at<float>(0, i) > x1_max)
            x1_max = pos1_in_2.at<float>(0, i);
    }

    // 计算右相机图像中最左侧木材的X坐标
    float x2_min = kMIN_X_IN_RIGHT;
    for (int i = 0; i < pos2.cols; i++)
    {
        if (pos2.at<float>(0, i) < x2_min)
            x2_min = pos2.at<float>(0, i);
    }

    // for debug 2025-08-13
    //std::time_t time_stamp = std::time(0);
    //std::string log_file_name = std::to_string(time_stamp) + ".txt";
    //std::ofstream log_file(log_file_name);
    //if (!log_file)
    //{
    //    std::cout << "create log file failed" << std::endl;
    //    return;
    //}
    //log_file << "x1_max: " << std::to_string(x1_max) << std::endl;
    //log_file << "x2_min: " << std::to_string(x2_min) << std::endl;

    // 如果没有重叠的木材，直接返回
    if (x1_max < x2_min - kX_EPS)
    {
        //log_file.close();
        return;
    }
    
    std::vector<int> overlap1, overlap2;
    std::vector<int> overlap_flags2;

    // 取出左相机中的重叠木材
    for (int i = 0; i < pos1_in_2.cols; i++)
        if (pos1_in_2.at<float>(0, i) > x2_min - kX_EPS)
            overlap1.push_back(i);

    // 取出右相机中的重叠木材
    for (int i = 0; i < pos2.cols; i++)
    {
        if (pos2.at<float>(0, i) < x1_max + kX_EPS)
        {
            overlap2.push_back(i);
            overlap_flags2.push_back(0);
        }
    }

    //log_file << "number of overlaped wood in left: " << std::to_string(overlap1.size()) << std::endl;
    //log_file << "number of overlaped wood in right: " << std::to_string(overlap2.size()) << std::endl;

    // 处理所有重叠木材
    for (size_t i = 0; i < overlap1.size(); i++)
    {
        float dist, delta_x, delta_y, delta_z;

        for (size_t j = 0; j < overlap2.size(); j++)
        {
            delta_x = pos1_in_2.at<float>(0, overlap1[i]) + x_offset - pos2.at<float>(0, overlap2[j]);
            delta_y = pos1_in_2.at<float>(1, overlap1[i]) + y_offset - pos2.at<float>(1, overlap2[j]);
            delta_z = pos1_in_2.at<float>(2, overlap1[i]) + z_offset - pos2.at<float>(2, overlap2[j]);

            dist = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;


            if (dist < threshold || results2[overlap_flags2[j]].center_2d.x < x_margin)
                overlap_flags2[j] = 1;

            //log_file 
            //    << std::to_string(pos1_in_2.at<float>(0, overlap1[i]))<<" "
            //    << std::to_string(pos1_in_2.at<float>(1, overlap1[i]))<<" "
            //    << std::to_string(pos1_in_2.at<float>(2, overlap1[i]))<<",   "
            //    << std::to_string(pos2.at<float>(0, overlap2[j])) <<" "
            //    << std::to_string(pos2.at<float>(1, overlap2[j])) << " "
            //    << std::to_string(pos2.at<float>(2, overlap2[j])) << ",   "
            //    << std::to_string(dist)
            //    << std::endl;
        }
    }

    
    for (size_t i = 0; i < overlap2.size(); i++)
    {
        if (overlap_flags2[i] == 1)
            flags2[overlap2[i]] = 0;
    }

   // log_file.close();
}



void save_pos_image(const std::string& left_img_path, const std::string& right_img_path,
    cv::Mat& left_img, cv::Mat right_img,
    const std::vector<ScaleData>& results1, const std::vector<ScaleData>& results2,
    const cv::Mat& R, const cv::Mat& t)
{
    if (results1.size() != 0)
    {
        cv::Mat pos1 = cv::Mat::zeros(3, results1.size(), CV_32FC1);
        for (size_t i = 0; i < results1.size(); i++)
        {
            pos1.at<float>(0, i) = results1[i].center_3d.x;
            pos1.at<float>(1, i) = results1[i].center_3d.y;
            pos1.at<float>(2, i) = results1[i].center_3d.z;
        }

        cv::Mat pos1_in_2 = R * pos1;
        for (int i = 0; i < pos1_in_2.cols; i++)
        {
            pos1_in_2.at<float>(0, i) = pos1_in_2.at<float>(0, i) + t.at<float>(0, 0);
            pos1_in_2.at<float>(1, i) = pos1_in_2.at<float>(1, i) + t.at<float>(1, 0);
            pos1_in_2.at<float>(2, i) = pos1_in_2.at<float>(2, i) + t.at<float>(2, 0);
        }

        char pos_info[64];
        for (size_t i = 0; i < results1.size(); i++)
        {
            sprintf_s(pos_info, "%.1f", pos1_in_2.at<float>(0, i));
            cv::putText(left_img, pos_info,
                cv::Point(static_cast<int>(results1[i].center_2d.x - 40), static_cast<int>(results1[i].center_2d.y - 30)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);

            sprintf_s(pos_info, "%.1f", pos1_in_2.at<float>(1, i));
            cv::putText(left_img, pos_info,
                cv::Point(static_cast<int>(results1[i].center_2d.x - 40), static_cast<int>(results1[i].center_2d.y)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);

            sprintf_s(pos_info, "%.1f", pos1_in_2.at<float>(2, i));
            cv::putText(left_img, pos_info,
                cv::Point(static_cast<int>(results1[i].center_2d.x - 40), static_cast<int>(results1[i].center_2d.y + 30)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);
        }
    }
    cv::imwrite(left_img_path, left_img);

    if (results2.size() != 0)
    {
        char pos_info[64];
        for (size_t i = 0; i < results2.size(); i++)
        {
            sprintf_s(pos_info, "%.1f", results2[i].center_3d.x);
            cv::putText(right_img, pos_info,
                cv::Point(static_cast<int>(results2[i].center_2d.x - 40), static_cast<int>(results2[i].center_2d.y - 30)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);

            sprintf_s(pos_info, "%.1f", results2[i].center_3d.y);
            cv::putText(right_img, pos_info,
                cv::Point(static_cast<int>(results2[i].center_2d.x - 40), static_cast<int>(results2[i].center_2d.y)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);

            sprintf_s(pos_info, "%.1f", results2[i].center_3d.z);
            cv::putText(right_img, pos_info,
                cv::Point(static_cast<int>(results2[i].center_2d.x - 40), static_cast<int>(results2[i].center_2d.y + 30)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7f, cv::Scalar(0, 0, 255), 1);
        }

    }
    cv::imwrite(right_img_path, right_img);
}