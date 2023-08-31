#ifndef __MODEL_H__
#define __MODEL_H__

#include "geometry.h"
#include "tgaimage.h"
#include <vector>

class Model
{
private:
	TGAImage diffusemap_;//��ͼ
	std::vector<Vec3f> verts_;//��������
	std::vector<Vec2f> uv_;//��������
	std::vector<Vec3f> norms_;//��������
	std::vector<std::vector<Vec3i>> faces_;//������{[v_i,vt_i,vn_i]*3}
	//���أ�model�ļ���+�ض���׺������ͼ�ļ�
	void load_texture(std::string filename, const char* suffix, TGAImage& img);

public:
	//���ļ�����
	Model(const char* filename);
	~Model();

	int nverts();//������
	int nuvs();//�������
	int nnorms();//���ߵ���
	int nfaces();//����
	Vec3f vert(int i);//��ȡ��i������
	Vec2f uv(int facei,int verti);////��ȡ��facei��ĵ�verti��������uv����
	TGAColor diffuse(Vec2f uv);//��ȡuv�����Ӧ����ͼ��ɫֵ
	
	std::vector<int> face(int idx);//��ȡ��idx����(�Ķ�������)
};

#endif // !__MODEL_H__

