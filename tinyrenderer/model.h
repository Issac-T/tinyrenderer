#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include "tgaimage.h"
#include <vector>

class Model
{
private:
	TGAImage diffusemap_;//材质贴图
	TGAImage normalmap_;//法线贴图
	TGAImage normalmap_tan_;//切空间法线贴图
	TGAImage specmap_;//高光贴图
	std::vector<Vec3f> verts_;//顶点数据
	std::vector<Vec2f> uv_;//纹理数据
	std::vector<Vec3f> norms_;//法线数据
	std::vector<std::vector<Vec3i>> faces_;//面数据{[v_i,vt_i,vn_i]*3}
	//加载（model文件名+特定后缀）的贴图文件
	void load_texture(std::string filename, const char* suffix, TGAImage& img);

public:
	//从文件构造
	Model(const char* filename);
	~Model();

	int nverts();//顶点数
	int nuvs();//纹理点数
	int nnorms();//法线点数
	int nfaces();//面数
	Vec3f vert(int i);//获取第i个顶点
	Vec2f uv(int facei, int verti);////获取第facei面的第verti个纹理点的uv坐标
	Vec3f norm(int facei, int verti);//获取第facei面的第verti个法线点
	Vec3f normal(Vec2f uv);//获取uv坐标对应的法线值(法线贴图)
	Vec3f normal_tan(Vec2f uv);//获取uv坐标对应的法线值(切空间法线贴图)
	TGAColor diffuse(Vec2f uv);//获取uv坐标对应的漫反射贴图颜色值
	float specular(Vec2f uv);//获取uv坐标对应的高光贴图颜色值

	std::vector<int> face(int idx);//获取第idx个面(的顶点索引)
};

#endif // !__MODEL_H__

