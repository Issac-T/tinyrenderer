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
	virtual Vec4f vertex(int iface, int nthvert) = 0; //��ģ��ȡ���㣨ֱ��ת������Ļ���꣩
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;//��ɫ
};
//�����ι�դ��
void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);


//-------����ΪShaderǰ�ڿγ���ϰ����-------
extern const TGAColor white;
extern const TGAColor red;
extern const TGAColor red2;
extern const TGAColor green;
extern const TGAColor blue;

using std::abs;

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img);
void line(const Vec2i& v0, const Vec2i& v1, const TGAColor& color, TGAImage& img);
void mesh(Model& model, const TGAColor& color, TGAImage& img, int scale);
//��դ�����������
void triangle_simple(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//�����α߽�
void triangle_bound(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//��ȡ��Χ��
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max);
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max, int width, int height);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max, int width, int height);//���仭����߽���Լ��
//�����p����������v0,v1,v2����������
Vec3f barycentric(Vec2i* pts, Vec2i& p);
Vec3f barycentric(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& p);
//ʹ�������������������
void triangle_barycentric(Vec2i* pts, const TGAColor& color, TGAImage& img);
//�����淨��(����������)
Vec3f get_face_normal(Vec3f* pts);
//����Ȳ��ԵĹ�դ������άͶӰ��һλ�����汾��
void rasterize_line(Vec2i p0, Vec2i p1, const TGAColor& color, TGAImage& img, float* yBuffer);
//����Ȳ��Ե��������դ��
void rasterize_triangle(Vec3f* pts, float* zBuffer, const TGAColor& color, TGAImage& img);
//����Ȳ��Ե��������դ��(������+����)
void rasterize_triangle_texture(Vec3f* pts, float* zBuffer, int f_idx, Model& model, TGAImage& img, float intensity = 1);
//�Ե�������ͶӰ��͸��ͶӰ��
Vec3f central_projection(const Vec3f& pts_in, const Vec3f& camera_pos);
//�������ͶӰ����
Matrix camera_matrix(Vec3f& camera_pos);
//���ɳ�ʼƽ��-���ž���(һ��ƽ�����������ġ��������뻭�����ƴ�С)
Matrix trans_scale_matrix(int img_width, int img_height, Vec3f scale3);
Matrix trans_scale_matrix(int img_width, int img_height, float scale = 1.f);
//����ƽ�� - ���ž���(�뻭���޹�)-ע��Ϊ��������ƽ��
Matrix trans_scale_matrix_1(Vec3f move, float scale = 1.f);
Matrix trans_scale_matrix_1(Vec3f move, Vec3f scale3);
//�������ͶӰ����(���пռ䷽��)
Matrix u_lookat(Vec3f& eye_pos, Vec3f& center, Vec3f& up);
//�������ͶӰ����(��������z��������ͶӰ���������κ����ƽ��)
Matrix camera_matrix(float camera_z);
//����ViewPort���󣨽�[-1~1]����������[0 0 0]-[w,h,d]��Χ�ڣ�
Matrix viewport_matrix(int x, int y, int w, int h);

#endif // !__MY_GL_H__

