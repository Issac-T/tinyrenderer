#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include <vector>

class Model
{
private:
	std::vector<Vec3f> verts_;//顶点数据
	std::vector<Vec3f> textures_;//纹理数据
	std::vector<std::vector<int>> faces_;//面顶点数据
	std::vector<std::vector<int>> faces_texture_;//面纹理数据

public:
	//从文件构造
	Model(const char* filename);
	~Model();

	int nverts();//顶点数
	int ntextures();//纹理点数
	int nfaces();//面数
	Vec3f vert(int i);//获取第i个顶点
	Vec3f texture(int i);//获取第i个顶点纹理坐标
	
	std::vector<int> face(int idx);//获取第idx个面(的顶点索引)
	std::vector<int> face_texture(int idx);//获取第idx个面的纹理索引
};

#endif // !__MODEL_H__

