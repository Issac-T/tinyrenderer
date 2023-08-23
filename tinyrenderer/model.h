#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

class Model
{
private:
	std::vector<Vec3f> verts_;//��������
	std::vector<std::vector<int>> faces_;//������

public:
	//���ļ�����
	Model(const char* filename);
	~Model();

	int nverts();//������
	int nfaces();//����
	Vec3f vert(int i);//��ȡ��i������
	std::vector<int> face(int idx);//��ȡ��idx����
};

#endif // !__MODEL_H__

