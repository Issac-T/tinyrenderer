#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
	//拼接生成贴图文件名
	size_t pos = filename.find_last_of(".");
	std::string img_name = filename.substr(0, pos) + std::string(suffix);
	//加载贴图文件
	if (img.read_tga_file(img_name.c_str()))
		std::cout << "load texture file " << img_name << " success"<<"["<<img.get_width()<<"*"<<img.get_height()<<"]" << std::endl;
	else
		std::cerr << "load texture file " << img_name << " failed" << std::endl;
}

Model::Model(const char* filename)
{
	//加载.obj文件数据
	std::ifstream in;
	in.open(filename,std::ifstream::in);
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
		if (!line.compare(0, 2, "v "))//compare为0代表两个字串相同
		{
			//顶点坐标
			iss>>trash;//坐标数据前的"v "标签
			Vec3f v;
			for(int i=0;i<3;i++)
				iss >> v.raw[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vt "))//compare为0代表两个字串相同
		{
			//纹理坐标
			iss >> trash>> trash;
			Vec2f vt;
			for (int i = 0; i < 2; i++)	//uv只取前两个值，第三个0忽略
				iss >> vt.raw[i];
			uv_.push_back(vt);
		}
		else if (!line.compare(0, 3, "vn "))
		{
			//法线数据
			iss >> trash >> trash;
			Vec3f vn;
			for (int i = 0; i < 3; i++)
				iss >> vn.raw[i];
			norms_.push_back(vn);
		}
		else if (!line.compare(0, 2, "f "))
		{
			//面数据
			iss >> trash;
			std::vector<Vec3i> f;
			Vec3i vtn_i;

			while (iss >> vtn_i.raw[0] >> trash >> vtn_i.raw[1] >> trash >> vtn_i.raw[2])
			{
				for (int i = 0; i < 3; i++) vtn_i.raw[i]--;//obj文件索引从1开始
				f.push_back(vtn_i);
			}
			faces_.push_back(f);
		}
	}
	std::cout<<"# vertex# "<<verts_.size() << " texture# " << uv_.size() << " norms# " << norms_.size() <<" faces# "<<faces_.size()<<std::endl;
	
	//加载模型对应.tga贴图文件
	load_texture(filename, "_diffuse.tga", diffusemap_);
	diffusemap_.flip_vertically();
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
	int uv_i = faces_[facei][verti][1];	//该顶点对应uv索引
	ret = uv_[uv_i];
	ret.x *= diffusemap_.get_width();
	ret.y *= diffusemap_.get_height();

	return ret;
}

TGAColor Model::diffuse(Vec2f uv)
{
	return diffusemap_.get(uv.u,uv.v);
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
