#ifndef __MY_GL_H__
#define __MY_GL_H__

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

struct IShader
{
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nthvert) = 0; //从模型取顶点（直接转换至屏幕坐标）
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;//着色
};
//三角形光栅化
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);


//-------以下为Shader前期课程练习代码-------
extern const TGAColor white;
extern const TGAColor red;
extern const TGAColor red2;
extern const TGAColor green;
extern const TGAColor blue;

using std::abs;

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img);
void line(const Vec2i& v0, const Vec2i& v1, const TGAColor& color, TGAImage& img);
void mesh(Model& model, const TGAColor& color, TGAImage& img, int scale);
//光栅化填充三角形
void triangle_simple(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//三角形边界
void triangle_bound(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//获取包围盒
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max);
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max, int width, int height);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max, int width, int height);//补充画布宽高进行约束
//计算点p关于三角形v0,v1,v2的重心坐标
Vec3f barycentric(Vec2i* pts, Vec2i& p);
Vec3f barycentric(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& p);
//使用重心坐标填充三角形
void triangle_barycentric(Vec2i* pts, const TGAColor& color, TGAImage& img);
//计算面法向(不区分正反)
Vec3f get_face_normal(Vec3f* pts);
//带深度测试的光栅化（二维投影至一位线条版本）
void rasterize_line(Vec2i p0, Vec2i p1, const TGAColor& color, TGAImage& img, float* yBuffer);
//带深度测试的三角面光栅化
void rasterize_triangle(Vec3f* pts, float* zBuffer, const TGAColor& color, TGAImage& img);
//带深度测试的三角面光栅化(带纹理+亮度)
void rasterize_triangle_texture(Vec3f* pts, float* zBuffer, int f_idx, Model& model, TGAImage& img, float intensity = 1);
//对点做中心投影（透视投影）
Vec3f central_projection(const Vec3f& pts_in, const Vec3f& camera_pos);
//生成相机投影矩阵
Matrix camera_matrix(Vec3f& camera_pos);
//生成初始平移-缩放矩阵(一般平移至画布中心、缩放至与画布相似大小)
Matrix trans_scale_matrix(int img_width, int img_height, Vec3f scale3);
Matrix trans_scale_matrix(int img_width, int img_height, float scale = 1.f);
//生成平移 - 缩放矩阵(与画布无关)-注意为先缩放再平移
Matrix trans_scale_matrix_1(Vec3f move, float scale = 1.f);
Matrix trans_scale_matrix_1(Vec3f move, Vec3f scale3);
//生成相机投影矩阵(带有空间方向)
Matrix u_lookat(Vec3f& eye_pos, Vec3f& center, Vec3f& up);
//生成相机投影矩阵(仅做根据z轴做中心投影，不处理任何相对平移)
Matrix camera_matrix(float camera_z);
//生成ViewPort矩阵（将[-1~1]立方缩放至[0 0 0]-[w,h,d]范围内）
Matrix viewport_matrix(int x, int y, int w, int h);

#endif // !__MY_GL_H__

