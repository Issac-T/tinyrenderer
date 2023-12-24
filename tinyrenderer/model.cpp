#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
	//ƴ��������ͼ�ļ���
	size_t pos = filename.find_last_of(".");
	std::string img_name = filename.substr(0, pos) + std::string(suffix);
	//������ͼ�ļ�
	if (img.read_tga_file(img_name.c_str()))
		std::cout << "load texture file " << img_name << " success" << "[" << img.get_width() << "*" << img.get_height() << "]" << std::endl;
	else
		std::cerr << "load texture file " << img_name << " failed" << std::endl;
}

Model::Model(const char* filename)
{
	//����.obj�ļ�����
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (!in.is_open())
	{
		std::cerr << "Cannot open " << filename << std::endl;
		return;
	}
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v "))//compareΪ0���������ִ���ͬ
		{
			//��������
			iss >> trash;//��������ǰ��"v "��ǩ
			Vec3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vt "))//compareΪ0���������ִ���ͬ
		{
			//��������
			iss >> trash >> trash;
			Vec2f vt;
			for (int i = 0; i < 2; i++)	//uvֻȡǰ����ֵ��������0����
				iss >> vt[i];
			uv_.push_back(vt);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			//��������
			iss >> trash >> trash;
			Vec3f vn;
			for (int i = 0; i < 3; i++)
				iss >> vn[i];
			norms_.push_back(vn);
		}
		else if (!line.compare(0, 2, "f "))
		{
			//������
			iss >> trash;
			std::vector<Vec3i> f;
			Vec3i vtn_i;

			while (iss >> vtn_i[0] >> trash >> vtn_i[1] >> trash >> vtn_i[2])
			{
				for (int i = 0; i < 3; i++) vtn_i[i]--;//obj�ļ�������1��ʼ
				f.push_back(vtn_i);
			}
			faces_.push_back(f);
		}
	}
	std::cout << "# vertex# " << verts_.size() << " texture# " << uv_.size() << " norms# " << norms_.size() << " faces# " << faces_.size() << std::endl;

	//����ģ�Ͷ�Ӧ.tga��ͼ�ļ�
	load_texture(filename, "_diffuse.tga", diffusemap_);
	diffusemap_.flip_vertically();
	//����ģ�Ͷ�Ӧ_nm.tga��ͼ�ļ�
	load_texture(filename, "_nm.tga", normalmap_);
	normalmap_.flip_vertically();
	//����ģ�Ͷ�Ӧ_nm_tangent��ͼ�ļ����пռ䷨����ͼ��
	load_texture(filename, "_nm_tangent.tga", normalmap_tan_);
	normalmap_tan_.flip_vertically();

	//����ģ�Ͷ�Ӧ_spec.tga��ͼ�ļ�
	load_texture(filename, "_spec.tga", specmap_); //�Ҷ�ͼ bpp=8;
	specmap_.flip_vertically();
}

Model::~Model()
{
}

int Model::nverts()
{
	return verts_.size();
}

int Model::nuvs()
{
	return uv_.size();
}

int Model::nnorms()
{
	return norms_.size();
}

int Model::nfaces()
{
	return faces_.size();
}

Vec3f Model::vert(int i)
{
	if (i < nverts() && i >= 0)
		return verts_[i];

	return Vec3f();
}

Vec2f Model::uv(int facei, int verti)
{
	Vec2f ret;
	int uv_i = faces_[facei][verti][1];	//�ö����Ӧuv����
	ret = uv_[uv_i];
	ret[0] *= diffusemap_.get_width();
	ret[1] *= diffusemap_.get_height();

	return ret;
}

Vec3f Model::norm(int facei, int verti)
{
	int i = faces_[facei][verti][2];
	return norms_[i];
}

Vec3f Model::normal(Vec2f uv)
{
	TGAColor c = normalmap_.get(uv[0], uv[1]);
	//RGBתΪxyz��������
	Vec3f ret(c.r, c.g, c.b);
	return (ret - 128.f).normalize();
}

//�пռ䷨��
Vec3f Model::normal_tan(Vec2f uv)
{
	TGAColor c = normalmap_tan_.get(uv[0], uv[1]);
	//RGBתΪxyz��������
	Vec3f ret(c.r, c.g, c.b);
	return (ret - 128.f)/ 128.f;
}

TGAColor Model::diffuse(Vec2f uv)
{
	return diffusemap_.get(uv[0], uv[1]);
}

float Model::specular(Vec2f uv)
{
	return specmap_.get(uv.x,uv.y).raw[0]/255.f*10.0f; //������0~10
}

std::vector<int> Model::face(int idx)
{
	std::vector<int> vi;
	if (idx < nfaces() && idx >= 0)
	{
		for (int i = 0; i < 3; i++) vi.push_back(faces_[idx][i][0]);
	}
	return vi;
}
