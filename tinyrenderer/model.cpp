#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char* filename)
{
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
			Vec3f vt;
			for (int i = 0; i < 3; i++)
				iss >> vt.raw[i];
			textures_.push_back(vt);
		}
		else if (!line.compare(0, 2, "f "))
		{
			//面数据
			iss >> trash;
			std::vector<int> f;
			std::vector<int> f_texture;
			int itrash, v_idx,t_idx;
			while (iss >> v_idx >> trash >> t_idx >> trash >> itrash)//过滤掉法线坐标
			{
				v_idx--;//obj文件索引从1开始
				t_idx--;
				f.push_back(v_idx);
				f_texture.push_back(t_idx);
			}
			faces_.push_back(f);
			faces_texture_.push_back(f_texture);
		}
	}
	std::cout<<"# v# "<<verts_.size()<<" f# "<<faces_.size()<<std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
	return verts_.size();
}

int Model::ntextures()
{
	return textures_.size();
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

Vec3f Model::texture(int i)
{
	if (i < ntextures() && i >= 0)
		return textures_[i];

	return Vec3f();
}

std::vector<int> Model::face(int idx)
{
	if (idx < nfaces() && idx >= 0)
		return faces_[idx];

	return std::vector<int>();
}

std::vector<int> Model::face_texture(int idx)
{

	if (idx < nfaces() && idx >= 0)
		return faces_texture_[idx];

	return std::vector<int>();
}
