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
		if (!line.compare(0, 2, "v "))//compareΪ0���������ִ���ͬ
		{
			//��������
			iss>>trash;//>>�����ո�ֹͣ���������������ǰ��"v "��ǩ
			Vec3f v;
			for(int i=0;i<3;i++)
				iss >> v.raw[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 2, "f "))
		{
			//������
			iss >> trash;
			std::vector<int> f;
			int itrash, idx;
			while (iss >> idx >> trash >> itrash >> trash >> itrash)//���˵����ߺ���������
			{
				idx--;//obj�ļ�������1��ʼ
				f.push_back(idx);
			}
			faces_.push_back(f);
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

std::vector<int> Model::face(int idx)
{
	if (idx < nfaces() && idx >= 0)
		return faces_[idx];

	return std::vector<int>();
}
