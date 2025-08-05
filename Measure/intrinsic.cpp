#include "intrinsic.h"


int load_intrinsic(const std::string& intrinsic_path,
    cv::Mat &r_mat, cv::Mat &t_vect)
{
    cv::FileStorage file_storage(intrinsic_path, cv::FileStorage::READ);
    cv::FileNode file_node;
    
    if (!file_storage.isOpened())
    {
        //std::cout<<"open file failed"<<std::endl;
        return 0;
    }

    file_node = file_storage["R"];
    if (file_node.empty())
    {
        //std::cout<<"get node R fialed"<<std::endl;
        return 0;
    }    
    //cv::Mat r_mat_64 = file_node.mat();
    //r_mat_64.convertTo(r_mat, CV_32F, 1, 0);
    r_mat = file_node.mat();

    file_node = file_storage["T"];
    if (file_node.empty())
    {
        //std::cout<<"get node T fialed"<<std::endl;
        return 0;
    }
    //cv::Mat t_vect_64 = file_node.mat();
    //t_vect_64.convertTo(t_vect, CV_32F, 1, 0);
    t_vect = file_node.mat();

    file_storage.release();
    return 1;
}