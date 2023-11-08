// tinyrenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "model.h"
#include "drawtest.h"

Model* model = NULL;
const int width = 800;
const int height = 800;

Vec3f light_dir(1, 1, 1);
Vec3f camera_pos(1, 1, 3);
Vec3f camera_up(0, 1, 0);
Vec3f camera_center(0, 0, 0);

struct  SimpleShader : public IShader
{
	float face_intensity;//3个顶点光照强度
	Vec3f face_norm;
	Vec3f verts[3];
	virtual Vec4f vertex(int iface, int nvert)
	{
		verts[nvert] = model->vert(model->face(iface)[nvert]);
		if (nvert == 2)
		{
			face_norm = cross((verts[1] - verts[0]), (verts[2] - verts[0])).normalize();
			face_intensity = face_norm*(light_dir.normalize());
		}

		Vec4f gl_Vertex = embed<4>(verts[nvert]);
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		color = TGAColor(255, 255, 255) * std::max(0.f, face_intensity);//三角面统一使用面法线确定着色强度
		return false;
	}

};

struct  GouraudShader : public IShader
{
	Vec3f varying_intensity;//3个顶点光照强度

	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec3f vert_normal = model->norm(iface, nvert);
		varying_intensity[nvert] = std::max(0.f,vert_normal * light_dir.normalize());//注意避免负值(<=0都为全黑)
		
		Vec4f gl_Vertex = embed<4>(model->vert(model->face(iface)[nvert]));
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		float intensity = varying_intensity * bar;//根据顶点光强线性插值计算渲染点光强
		color = TGAColor(255, 255, 255) * intensity;
		return false;
	}

};

int main()
{
	//draw_move_camera_test();//相机移动测试

	//初始化变换矩阵
	ModelView = u_lookat(camera_pos, camera_center, camera_up);
	Projection = camera_matrix(camera_pos.z - camera_center.z);
	Viewport = viewport_matrix(0, 0, width, height);
	//加载模型
	model = new Model("obj/african_head.obj");
	//初始化画布
	TGAImage image(width, height, TGAImage::RGB);
	//初始化深度缓存
	TGAImage zbuff(width, height, TGAImage::GRAYSCALE);
	for (int i = width; i--;)
	{
		for (int j = height; j--; )
		{
			zbuff.set(i, j, TGAColor(0,1));
		}
	}

	//初始化着色器
	SimpleShader shader;
	//进行绘制(遍历三角面)
	for (int i = 0; i < model->nfaces(); i++)
	{
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++)
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zbuff);
	}
	image.flip_vertically();
	image.write_tga_file("SimpleShaderTest1.tga");
	zbuff.flip_vertically();
	zbuff.write_tga_file("SimpleShaderTest1_zbuff.tga");

}

