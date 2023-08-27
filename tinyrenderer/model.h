#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

class Model
{
private:
	std::vector<Vec3f> verts_;//��������
	std::vector<Vec3f> textures_;//��������
	std::vector<std::vector<int>> faces_;//�涥������
	std::vector<std::vector<int>> faces_texture_;//����������

public:
	//���ļ�����
	Model(const char* filename);
	~Model();

	int nverts();//������
	int ntextures();//�������
	int nfaces();//����
	Vec3f vert(int i);//��ȡ��i������
	Vec3f texture(int i);//��ȡ��i��������������
	
	std::vector<int> face(int idx);//��ȡ��idx����(�Ķ�������)
	std::vector<int> face_texture(int idx);//��ȡ��idx�������������
};

#endif // !__MODEL_H__

