#include "scale_merge.h"
#include "LogFile.h"
#include <tchar.h>

static const float kMAX_X_IN_LEFT = -10000.0f;
static const float kMIN_X_IN_RIGHT = 10000.0f;
static const float kX_EPS = 300.0f;
static const float kPOS_EPS = 32400.0f;


void log_scale_merge(const std::vector<ScaleData>& results1, const std::vector<ScaleData>& results2,
    const cv::Mat& R, const cv::Mat& t, std::vector<int>& flags1, std::vector<int>& flags2)
{
    // 初始化结果标志位数组，全部置为1，表示木材保留
    flags1.resize(results1.size());
    for (size_t i = 0; i < results1.size(); i++)
        flags1[i] = 1;
    flags2.resize(results2.size());
    for (size_t i = 0; i < results2.size(); i++)
        flags2[i] = 1;

    // 对特殊情况进行判断
    if (results1.size() == 0 || results2.size() == 0)
        return;


    // 将检尺结果的中心点坐标数据填入矩阵中,按行排布
    cv::Mat pos1 = cv::Mat::zeros(results1.size(), 3, CV_32FC1);
    for (size_t i = 0; i < results1.size(); i++)
    {
        pos1.at<float>(i, 0) = results1[i].center_3d.x;
        pos1.at<float>(i, 1) = results1[i].center_3d.y;
        pos1.at<float>(i, 2) = results1[i].center_3d.z;
    }

    cv::Mat pos2 = cv::Mat::zeros(results2.size(), 3, CV_32FC1);
    for (size_t i = 0; i < results2.size(); i++)
    {
        pos2.at<float>(i, 0) = results2[i].center_3d.x;
        pos2.at<float>(i, 1) = results2[i].center_3d.y;
        pos2.at<float>(i, 2) = results2[i].center_3d.z;
    }
   
    // 将左侧相机的结果变换到右侧相机的坐标系中
    cv::Mat temp1 = R * pos1.t();
    cv::Mat temp2 = cv::Mat::zeros(temp1.rows, temp1.cols,CV_32F);
    for (int i = 0; i < temp1.rows; i++)
        for (int j = 0; j < temp1.cols; j++)
            temp2.at<float>(i, j) = temp1.at<float>(i, j) + t.at<float>(i, 0);
    cv::Mat pos1_in_2 = temp2.t();

    // 寻找左侧相机中最右边的木材，即X坐标最大的
    float x1_max = kMAX_X_IN_LEFT;
    for (int i = 0; i < pos1_in_2.rows; i++)
    {
        if (pos1_in_2.at<float>(i, 0) > x1_max)
            x1_max = pos1_in_2.at<float>(i, 0);
    }

    // 寻找右侧相机中最左边的木材，即X坐标最小的
    float x2_min = kMIN_X_IN_RIGHT;
    for (int i = 0; i < pos2.rows; i++)
    {
        if (pos2.at<float>(i, 0) < x2_min)
            x2_min = pos2.at<float>(i, 0);
    }

    // 如果二者没有交集，直接返回
    if (x1_max < x2_min)
        return;


    // 查找左右相机有交叠的木材
    std::vector<int> overlap1, overlap2;
    std::vector<int> overlap_flags2;

    // 左侧相机中有交叠的木材
    for (int i = 0; i < pos1_in_2.rows; i++)
        if (pos1_in_2.at<float>(i, 0) > x2_min - kX_EPS)
            overlap1.push_back(i);

    // 右侧相机中有交叠的木材
    for (int i = 0; i < pos2.rows; i++)
    {
        if (pos2.at<float>(i, 0) < x1_max + kX_EPS)
        {
            overlap2.push_back(i);
            overlap_flags2.push_back(0);
        }
    }

    // 处理所有交叠木材
    for (size_t i = 0; i < overlap1.size(); i++)
    {
        float dist, delta_x, delta_y, delta_z;

        for (size_t j = 0; j < overlap2.size(); j++)
        {
            delta_x = pos1_in_2.at<float>(overlap1[i], 0) - pos2.at<float>(overlap2[j], 0);
            delta_y = pos1_in_2.at<float>(overlap1[i], 1) - pos2.at<float>(overlap2[j], 1);
            delta_z = pos1_in_2.at<float>(overlap1[i], 2) - pos2.at<float>(overlap2[j], 2);

            // 计算左右相机中两个木材之间的距离
            dist = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;

            // 如果距离小于阈值，则被认为是同一根木材，保留左侧相机的结果，丢弃右侧相机的结果
            if (dist < kPOS_EPS)
            {
                overlap_flags2[j] = 1;
            }
        }
    }

    // 去除右侧相机中的重复的木材
    for (size_t i = 0; i < overlap2.size(); i++)
    {
        if (overlap_flags2[i] == 1)
            flags2[overlap2[i]] = 0;
    }
}