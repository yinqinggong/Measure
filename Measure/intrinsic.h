#ifndef INTRINSIC_H
#define INTRINSIC_H
#include <string>
#include <opencv2/opencv.hpp>

// load camera's intrinsics
// all matrix is float64
int load_intrinsic(
    const std::string& intrinsic_path, // intrinsic file path
    cv::Mat &r_mat,   // rotation matrix of right camera to left camear
    cv::Mat &t_vect); // translation vector of right camera to left camera
#endif