#ifndef __UNSUAL_API_H__
#define __UNSUAL_API_H__

#include <iostream>
#include <string>
#include <vector>

typedef struct defWoodEllipse
{
	float cx;
	float cy; //cx, cy：横截面椭圆的中心点XY坐标，为像素坐标系
	float ab1;
	float ab2;//ab1,ab2：横截面椭圆长短半轴长度，为像素坐标系
	float angel;//椭圆的倾角，以角度为单位
	//前面这些信息用于在网页上绘制椭圆，叠加在识别图像之上
	float lx1;
	float ly1;
	float lx2;
	float ly2; //lx1, ly1, lx2, ly2：椭圆长直径的两个端点的XY坐标，为相机坐标系，以米为单位
	float sx1;
	float sy1;
	float sx2;
	float sy2;//sx1,sy1,sx2,sy2：椭圆短直径的两个端点的XY坐标，为相机坐标系，以米为单位
}WoodEllipse;


typedef struct defDiameters
{
	float d1;
	float d2; //[d1, d2] ：短直径与长直径长度
}Diameters;

typedef struct defWood
{
	WoodEllipse ellipse;// 木材横截面椭圆，为如下形式：
	Diameters diameters;//木材短直径与长直径的长度，以厘米为单位，为如下形式：
	float diameter;//长直径与短直径的较小值，以厘米为单位
	float volumn; //木材的体积，暂时先设置为0，用户设置木材长度之后，前端网页计算出每根木材的体积，填入该字段
}Wood;

typedef struct defScaleWood
{
	int id; //数值, 本次检尺任务的ID，使用的是时间戳
	std::vector<Wood> wood_list;//列表, 木材信息列表，包括了识别出的每根木材的信息
	std::string img;//字符串,Base64编码的jpg文件，是校正处理之后的图像

}ScaleWood;

int PostPreview(std::string& url);
int PostPhoto(std::string& limg);
int PostScale(ScaleWood& scalewood);

#endif
