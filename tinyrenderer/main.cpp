// tinyrenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "model.h"
#include "drawtest.h"

Model* model = NULL;
const int width = 2000;
const int height = 2000;

Vec3f light_dir(3, 3, 3);
Vec3f camera_pos(0, 0, 3);
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
			face_intensity = face_norm * (light_dir.normalize());
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

//仅以灰度渲染
struct  GouraudShader0 : public IShader
{
	Vec3f varying_intensity;//3个顶点光照强度
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标

	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec3f vert_normal = model->norm(iface, nvert);
		varying_intensity[nvert] = std::max(0.f, vert_normal * light_dir.normalize());//注意避免负值(<=0都为全黑)

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

//亮度离散为6个梯度渲染
struct GouraudShader1 : public GouraudShader0
{
	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		float intensity = varying_intensity * bar;//根据顶点光强线性插值计算渲染点光强
		int temp = int(intensity * 3);
		float intensity_staged = (temp / 3.f);//光强分段[0~1]范围内等分六段
		color = TGAColor(255, 255, 0) * intensity_staged;
		return false;
	}
};

//加入材质渲染
struct  GouraudShader : public IShader
{

	Vec3f varying_intensity;//3个顶点光照强度
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标

	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec3f vert_normal = model->norm(iface, nvert);
		varying_intensity[nvert] = std::max(0.f, vert_normal * light_dir.normalize());//注意避免负值(<=0都为全黑)
		varying_uv.set_col(nvert, model->uv(iface, nvert));

		Vec4f gl_Vertex = embed<4>(model->vert(model->face(iface)[nvert]));
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		float intensity = varying_intensity * bar;//根据顶点光强线性插值计算渲染点光强
		Vec2f uv = varying_uv * bar;
		color = model->diffuse(uv) * intensity;
		return false;
	}

};


//加入法线贴图渲染
struct  GouraudShaderNormalTexture : public IShader
{
	Vec3f varying_intensity;//3个顶点光照强度
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标

	Vec3f uniform_eyedir;//视线方向

	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec3f vert_normal = model->norm(iface, nvert);
		varying_intensity[nvert] = std::max(0.f, vert_normal * light_dir.normalize());//注意避免负值(<=0都为全黑)
		varying_uv.set_col(nvert, model->uv(iface, nvert));

		Vec4f gl_Vertex = embed<4>(model->vert(model->face(iface)[nvert]));
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		Vec2f uv = varying_uv * bar;
		//取该点法线(来自法线贴图)
		Vec3f norm = model->normal(uv);
		//计算漫反射光强
		float intensity = std::max(0.f, norm * light_dir.normalize());
		//取该点高光强度
		float spec = model->specular(uv);
		//计算高光反射强度
		Vec3f reflect_dir = norm * 2 * (norm * light_dir.normalize()) - light_dir.normalize();
		float spec_intensity = std::max<float>(0, std::pow((reflect_dir * uniform_eyedir), spec));
		//计算颜色
		TGAColor color_diff = model->diffuse(uv);
		color.bytespp = 3;
		for (int i = 0; i < 3; i++)
			color.raw[i] = std::min<float>(255, 15 + color_diff.raw[i] * (intensity + 0.6f * spec_intensity));
		return false;
	}

};

//加入法线贴图渲染(切空间法线)
struct  GouraudShaderNormalTexture_tan : public IShader
{
	Vec3f varying_intensity;//3个顶点光照强度
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标
	Vec3f varying_du, varying_dv;//
	mat<3, 3, float> tan_A, tan_Ar; //切空间法线矩阵
	mat<3, 3, float> varying_vn;//3个顶点法线
	Vec3f verts[3];//3个顶点法线
	mat<3, 3, float> varying_tanBasis;//3个切空间基

	Vec3f uniform_eyedir;//视线方向

	virtual Vec4f vertex(int iface, int nvert)
	{
		varying_vn.set_col(nvert, model->norm(iface, nvert));
		//varying_intensity[nvert] = std::max(0.f, vert_norm[nvert] * light_dir.normalize());//注意避免负值(<=0都为全黑)
		varying_uv.set_col(nvert, model->uv(iface, nvert));
		verts[nvert] = model->vert(model->face(iface)[nvert]);

		if (nvert == 2)
		{
			tan_A[0] = verts[1] - verts[0];
			tan_A[1] = verts[2] - verts[0];
			varying_du = Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
			varying_dv = Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
		}
		Vec4f gl_Vertex = embed<4>(verts[nvert]);
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		Vec2f uv = varying_uv * bar;
		//计算切空间基
		tan_A[2] = (varying_vn * bar).normalize();
		tan_Ar = (tan_A).invert();
		varying_tanBasis.set_col(0, (tan_Ar * varying_du).normalize());
		varying_tanBasis.set_col(1, (tan_Ar * varying_dv).normalize());
		varying_tanBasis.set_col(2, tan_A[2]);
		//取该点法线(来自切线法线贴图)
		Vec3f n_dis = model->normal_tan(uv);
		Vec3f norm = (varying_tanBasis * n_dis).normalize();
		//计算漫反射光强
		float intensity = std::max(0.f, norm * light_dir.normalize());
		//取该点高光强度
		float spec = model->specular(uv);
		//计算高光反射强度
		Vec3f reflect_dir = norm * 2 * (norm * light_dir.normalize()) - light_dir.normalize();
		float spec_intensity = std::max<float>(0, std::pow((reflect_dir * uniform_eyedir), spec));
		//计算颜色
		TGAColor color_diff = model->diffuse(uv);
		color.bytespp = 3;
		for (int i = 0; i < 3; i++)
			color.raw[i] = std::min<float>(255, 5 + color_diff.raw[i] * (intensity + 0.6f * spec_intensity));
		return false;
	}

};

//网格渲染器
struct  MeshShader : public IShader
{
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标
	Vec3f verts[3];//3个顶点法线

	Vec3f uniform_eyedir;//视线方向

	virtual Vec4f vertex(int iface, int nvert)
	{
		//varying_intensity[nvert] = std::max(0.f, vert_norm[nvert] * light_dir.normalize());//注意避免负值(<=0都为全黑)
		varying_uv.set_col(nvert, model->uv(iface, nvert));
		verts[nvert] = model->vert(model->face(iface)[nvert]);

		Vec4f gl_Vertex = embed<4>(verts[nvert]);
		return Viewport * Projection * ModelView * gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		float thre = 0.07f;
		if (abs(bar[0]) < thre || abs(bar[1]) < thre || abs(bar[2]) < thre)
		{
			Vec2f uv = varying_uv * bar;
			TGAColor color_diff = model->diffuse(uv);
			color = color_diff;
			//color = TGAColor(255, 255, 255);
			return 0;
		}
		else
			return 1;
	}

};


//深度渲染器
struct  DepthShader : public IShader
{
	mat<3, 3, float> varying_verts;//3个画布空间顶点
	Matrix MPV;//模型-投影-视图矩阵

	DepthShader(Matrix matrix_mpv)
	{
		MPV = matrix_mpv;
	}

	virtual Vec4f vertex(int iface, int nvert)
	{
		Vec3f vert = model->vert(model->face(iface)[nvert]);
		Vec4f gl_Vertex = embed<4>(vert);
		gl_Vertex = MPV * gl_Vertex;
		varying_verts.set_col(nvert, proj<3>(gl_Vertex));

		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		Vec3f frag_pos = varying_verts * bar;
		color = TGAColor(frag_pos.z, frag_pos.z, frag_pos.z);
		return 0;
	}

};

//带阴影渲染器
//深度渲染器
struct  ShadowShader : public IShader
{
	Vec3f varying_intensity;//3个顶点光照强度
	mat<2, 3, float> varying_uv;//3个顶点纹理坐标
	Vec3f varying_du, varying_dv;//
	mat<3, 3, float> tan_A, tan_Ar; //切空间法线矩阵
	mat<3, 3, float> varying_vn;//3个顶点法线
	Vec3f verts[3];//3个顶点法线
	mat<3, 3, float> varying_tanBasis;//3个切空间基

	Matrix MVP;//模型-投影-视图矩阵
	Matrix shadow_mvp;//阴影深度图映射矩阵
	TGAImage* shadow_map;//阴影深度图

	Vec3f uniform_eyedir;//视线方向

	ShadowShader(Matrix matrix_mvp, Matrix matrix_shadow_mvp, TGAImage* image_shadow)
	{
		MVP = matrix_mvp;
		shadow_mvp = matrix_shadow_mvp;
		shadow_map = image_shadow;
	}

	virtual Vec4f vertex(int iface, int nvert)
	{
		varying_vn.set_col(nvert, model->norm(iface, nvert));
		//varying_intensity[nvert] = std::max(0.f, vert_norm[nvert] * light_dir.normalize());//注意避免负值(<=0都为全黑)
		varying_uv.set_col(nvert, model->uv(iface, nvert));
		verts[nvert] = model->vert(model->face(iface)[nvert]);

		if (nvert == 2)
		{
			tan_A[0] = verts[1] - verts[0];
			tan_A[1] = verts[2] - verts[0];
			varying_du = Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
			varying_dv = Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
		}
		Vec4f gl_Vertex = embed<4>(verts[nvert]);
		gl_Vertex = MVP * gl_Vertex;
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		Vec2f uv = varying_uv * bar;
		//计算shadowmap对应
		Vec3f frag_pos = verts[0] * bar[0] + verts[1] * bar[1] + verts[2] * bar[2];
		Vec3f map_pos = proj<3>(shadow_mvp * embed<4>(frag_pos));
		float SM_depth = shadow_map->get(map_pos.x, map_pos.y).r;
		float shadow_value = 1.0;
		if (map_pos.z < SM_depth) //该点在阴影中
			shadow_value = 0.3f;

		//计算切空间基
		tan_A[2] = (varying_vn * bar).normalize();
		tan_Ar = (tan_A).invert();
		varying_tanBasis.set_col(0, (tan_Ar * varying_du).normalize());
		varying_tanBasis.set_col(1, (tan_Ar * varying_dv).normalize());
		varying_tanBasis.set_col(2, tan_A[2]);
		//取该点法线(来自切线法线贴图)
		Vec3f n_dis = model->normal_tan(uv);
		Vec3f norm = (varying_tanBasis * n_dis).normalize();
		//计算漫反射光强
		float intensity = std::max(0.f, norm * light_dir.normalize());
		//取该点高光强度
		float spec = model->specular(uv);
		//计算高光反射强度
		Vec3f reflect_dir = norm * 2 * (norm * light_dir.normalize()) - light_dir.normalize();
		float spec_intensity = std::max<float>(0, std::pow((reflect_dir * uniform_eyedir), spec));
		//计算颜色
		TGAColor color_diff = model->diffuse(uv);
		color.bytespp = 3;
		for (int i = 0; i < 3; i++)
			color.raw[i] = std::min<float>(255, 5 + shadow_value * color_diff.raw[i] * (intensity + 0.6f * spec_intensity));
		return false;
	}
};


int main()
{
	//draw_move_camera_test();//相机移动测试
	//加载模型
	model = new Model("obj/diablo3_pose.obj");
	//model.
	//初始化画布
	TGAImage shadowmap(width, height, TGAImage::RGB);
	TGAImage image(width, height, TGAImage::RGB);
	//初始化深度缓存
	TGAImage zbuff(width, height, TGAImage::GRAYSCALE);
	for (int i = width; i--;)
	{
		for (int j = height; j--; )
		{
			zbuff.set(i, j, TGAColor(0, 1));
		}
	}
	
	//1.从光源视角渲染深度图-shadowmap

	//初始化变换矩阵
	ModelView = u_lookat(light_dir, camera_center, camera_up);
	Projection = camera_matrix((camera_pos - camera_center).norm());
	Viewport = viewport_matrix(0, 0, width, height);

	//初始化着色器
	Matrix shadow_mvp = Viewport * Projection * ModelView;
	DepthShader shadowmap_shader(shadow_mvp);
	//进行绘制(遍历三角面)
	for (int i = 0; i < model->nfaces(); i++)
	{
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++)
		{
			screen_coords[j] = shadowmap_shader.vertex(i, j);
		}
		triangle(screen_coords, shadowmap_shader, shadowmap, zbuff);
	}
	shadowmap.flip_vertically();
	shadowmap.write_tga_file("ShadowShaderTest_shadowmap.tga");
	shadowmap.flip_vertically(); //反转回来，之后再进行渲染时查询shadowmap需要
	zbuff.flip_vertically();
	zbuff.write_tga_file("ShadowShaderTest_shadowmap_depth.tga");

	//2.从相机视角渲染
	//初始化变换矩阵
	ModelView = u_lookat(camera_pos, camera_center, camera_up);
	Projection = camera_matrix((camera_pos - camera_center).norm());
	Viewport = viewport_matrix(0, 0, width, height);
	//初始化深度缓存
	for (int i = width; i--;)
	{
		for (int j = height; j--; )
		{
			zbuff.set(i, j, TGAColor(0, 1));
		}
	}
	//初始化着色器
	//MeshShader shader;
	//shader.uniform_eyedir = (camera_pos - camera_center).normalize();
	Matrix camera_mvp = Viewport * Projection * ModelView;
	ShadowShader shader(camera_mvp,shadow_mvp,&shadowmap);
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
	image.write_tga_file("ShadowShaderTest.tga");
	zbuff.flip_vertically();
	zbuff.write_tga_file("ShadowShaderTest_depth.tga");

}

