#ifndef __UNSUAL_API_H__
#define __UNSUAL_API_H__

#include <iostream>
#include <string>
#include <vector>

typedef struct defWoodEllipse
{
	float cx;
	float cy; //cx, cy���������Բ�����ĵ�XY���꣬Ϊ��������ϵ
	float ab1;
	float ab2;//ab1,ab2���������Բ���̰��᳤�ȣ�Ϊ��������ϵ
	float angel;//��Բ����ǣ��ԽǶ�Ϊ��λ
	//ǰ����Щ��Ϣ��������ҳ�ϻ�����Բ��������ʶ��ͼ��֮��
	float lx1;
	float ly1;
	float lx2;
	float ly2; //lx1, ly1, lx2, ly2����Բ��ֱ���������˵��XY���꣬Ϊ�������ϵ������Ϊ��λ
	float sx1;
	float sy1;
	float sx2;
	float sy2;//sx1,sy1,sx2,sy2����Բ��ֱ���������˵��XY���꣬Ϊ�������ϵ������Ϊ��λ
}WoodEllipse;


typedef struct defDiameters
{
	float d1;
	float d2; //[d1, d2] ����ֱ���볤ֱ������
}Diameters;

typedef struct defWood
{
	WoodEllipse ellipse;// ľ�ĺ������Բ��Ϊ������ʽ��
	Diameters diameters;//ľ�Ķ�ֱ���볤ֱ���ĳ��ȣ�������Ϊ��λ��Ϊ������ʽ��
	float diameter;//��ֱ�����ֱ���Ľ�Сֵ��������Ϊ��λ
	float volumn; //ľ�ĵ��������ʱ������Ϊ0���û�����ľ�ĳ���֮��ǰ����ҳ�����ÿ��ľ�ĵ������������ֶ�
}Wood;

typedef struct defScaleWood
{
	int id; //��ֵ, ���μ�������ID��ʹ�õ���ʱ���
	std::vector<Wood> wood_list;//�б�, ľ����Ϣ�б�������ʶ�����ÿ��ľ�ĵ���Ϣ
	std::string img;//�ַ���,Base64�����jpg�ļ�����У������֮���ͼ��

}ScaleWood;

int PostPreview(std::string& url);
int PostPhoto(std::string& limg);
int PostScale(ScaleWood& scalewood);

#endif
