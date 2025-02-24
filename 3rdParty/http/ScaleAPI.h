#ifndef __UNSUAL_API_H__
#define __UNSUAL_API_H__

#include <iostream>
#include <string>
#include <vector>

typedef struct defWoodEllipse
{
	double cx;
	double cy; //cx, cy：横截面椭圆的中心点XY坐标，为像素坐标系
	double ab1;
	double ab2;//ab1,ab2：横截面椭圆长短半轴长度，为像素坐标系
	double angel;//椭圆的倾角，以角度为单位
	//前面这些信息用于在网页上绘制椭圆，叠加在识别图像之上
	double lx1;
	double ly1;
	double lx2;
	double ly2; //lx1, ly1, lx2, ly2：椭圆长直径的两个端点的XY坐标，为相机坐标系，以米为单位
	double sx1;
	double sy1;
	double sx2;
	double sy2;//sx1,sy1,sx2,sy2：椭圆短直径的两个端点的XY坐标，为相机坐标系，以米为单位
}WoodEllipse;


typedef struct defDiameters
{
	double d1;
	double d2; //[d1, d2] ：短直径与长直径长度
}Diameters;

typedef struct defWoodAttr
{
	WoodEllipse ellipse;// 木材横截面椭圆，为如下形式：
	Diameters diameters;//木材短直径与长直径的长度，以厘米为单位，为如下形式：
	double diameter;//长直径与短直径的较小值，以厘米为单位
	double volumn; //木材的体积，暂时先设置为0，用户设置木材长度之后，前端网页计算出每根木材的体积，填入该字段
	bool isDeleting;//额外添加的信息，标注是否正在被删除
}WoodAttr;

typedef struct defScaleWood
{
	unsigned int id; //数值, 本次检尺任务的ID，使用的是时间戳
	std::vector<WoodAttr> wood_list;//列表, 木材信息列表，包括了识别出的每根木材的信息
	std::string img;//字符串,Base64编码的jpg文件，是校正处理之后的图像
}ScaleWood;

int PostPreview(std::string& url);
int PostPhoto(std::string& limg, int& errorCode, std::string& rimg, std::string& m_camparam);
int PostScale(ScaleWood& scalewood, int& errorCode);
int PostInfer(ScaleWood& scalewood, int& errorCode, std::string& limg, std::string& rimg, std::string& m_camparam, int& w, int& h, int& c);

#endif
