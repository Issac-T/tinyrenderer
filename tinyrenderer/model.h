#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

class Model
{
private:
	std::vector<Vec3f> verts_;//顶点数据
	std::vector<std::vector<int>> faces_;//面数据

public:
	//从文件构造
	Model(const char* filename);
	~Model();

	int nverts();//顶点数
	int nfaces();//面数
	Vec3f vert(int i);//获取第i个顶点
	std::vector<int> face(int idx);//获取第idx个面
};

#endif // !__MODEL_H__

