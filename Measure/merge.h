#pragma once
#ifndef MERGE_H
#define MERGE_H
#include <opencv2/opencv.hpp>
#include <vector>


struct ScaleData
{
    cv::Point2d center_2d; // 2D椭圆的中心点 
    int rx_2d;  // 2D椭圆的X轴长度，单位为像素
    int ry_2d;  // 2D椭圆的Y轴长度，单位为像素
    float angle; // 2D椭圆的倾角，单位为度

    cv::Point3f center_3d; // 木材横截面中心点的3D坐标，单位为毫米
    float r_long; //  木材X轴的长度，单位为mm
    float r_short;//  木材Y轴的长度，单位为mm
};


// 阵列合并
// results1: 左侧相机的检尺结果，通过调用检尺接口得到
// results2: 右侧相机的检尺结果，通过调用检尺接口得到
// R: 左侧相机到右侧相机的旋转矩阵，通过标定程序得到
// t: 左侧相机到右侧相机的平移向量，单位为mm，通过标定程序得到
// flags1: 左侧相机的合并结果，标志位数组，值为1表示该木材保留，值为0表示该木材丢弃
// flags2: 右侧相机的合并结果，标志位数组，值为1表示该木材保留，值为0表示该木材丢弃
void log_scale_merge(const std::vector<ScaleData>& results1, const std::vector<ScaleData>& results2,
    const cv::Mat& R, const cv::Mat& t, 
    float x_offset, float y_offset, float z_offset, float threshold,
    std::vector<int>& flags1, std::vector<int>& flags2);


// 保存显示木材3D坐标的图像，用于调试分析
// left_img_path: 左相机图像的保存路径
// right_img_path: 右相机图像的保存路径
// left_img: 左相机经过畸变校正之后的图像
// right_img: 右相机经过畸变校正之后的图像
// results1: 左相机的检尺结果
// results2: 右相机的检尺结果
// R, t: 左相机到右相机的旋转矩阵与平移向量
void save_pos_image(const std::string& left_img_path, const std::string& right_img_path,
    cv::Mat& left_img, cv::Mat right_img,
    const std::vector<ScaleData>& results1, const std::vector<ScaleData>& results2,
    const cv::Mat& R, const cv::Mat& t);

#endif // !SCALE_MERGE_H
