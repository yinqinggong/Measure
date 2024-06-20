#ifndef __UNSUAL_API_H__
#define __UNSUAL_API_H__

#include <iostream>
#include <string>
#include <vector>

typedef struct defWoodEllipse
{
	double cx;
	double cy; //cx, cy���������Բ�����ĵ�XY���꣬Ϊ��������ϵ
	double ab1;
	double ab2;//ab1,ab2���������Բ���̰��᳤�ȣ�Ϊ��������ϵ
	double angel;//��Բ����ǣ��ԽǶ�Ϊ��λ
	//ǰ����Щ��Ϣ��������ҳ�ϻ�����Բ��������ʶ��ͼ��֮��
	double lx1;
	double ly1;
	double lx2;
	double ly2; //lx1, ly1, lx2, ly2����Բ��ֱ���������˵��XY���꣬Ϊ�������ϵ������Ϊ��λ
	double sx1;
	double sy1;
	double sx2;
	double sy2;//sx1,sy1,sx2,sy2����Բ��ֱ���������˵��XY���꣬Ϊ�������ϵ������Ϊ��λ
}WoodEllipse;


typedef struct defDiameters
{
	double d1;
	double d2; //[d1, d2] ����ֱ���볤ֱ������
}Diameters;

typedef struct defWoodAttr
{
	WoodEllipse ellipse;// ľ�ĺ������Բ��Ϊ������ʽ��
	Diameters diameters;//ľ�Ķ�ֱ���볤ֱ���ĳ��ȣ�������Ϊ��λ��Ϊ������ʽ��
	double diameter;//��ֱ�����ֱ���Ľ�Сֵ��������Ϊ��λ
	double volumn; //ľ�ĵ��������ʱ������Ϊ0���û�����ľ�ĳ���֮��ǰ����ҳ�����ÿ��ľ�ĵ������������ֶ�
}WoodAttr;

typedef struct defScaleWood
{
	unsigned int id; //��ֵ, ���μ�������ID��ʹ�õ���ʱ���
	std::vector<WoodAttr> wood_list;//�б�, ľ����Ϣ�б�������ʶ�����ÿ��ľ�ĵ���Ϣ
	std::string img;//�ַ���,Base64�����jpg�ļ�����У������֮���ͼ��

}ScaleWood;

int PostPreview(std::string& url);
int PostPhoto(std::string& limg);
int PostScale(ScaleWood& scalewood);

#endif
