// tinyrenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor red2 = TGAColor(255, 0, 0, 100);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

using std::abs;

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img);
void line(const Vec2i& v0, const Vec2i& v1, const TGAColor& color, TGAImage& img);
void mesh(Model& model, const TGAColor& color, TGAImage& img, int scale);
//光栅化填充三角形
void triangle(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//三角形边界
void triangle_bound(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img);
//获取包围盒
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max);
//计算点p关于三角形v0,v1,v2的重心坐标
Vec3f barycentric(Vec2i* pts, Vec2i& p);
Vec3f barycentric(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& p);
//使用重心坐标填充三角形
void triangle_barycentric(Vec2i* pts, const TGAColor& color, TGAImage& img);


void draw_line_test();
void draw_mesh_test();
void draw_triangle_test();


int main()
{
	Model model("obj/african_head.obj");//读取obj文件
	//根据obj面数据绘制所有三角面网格线
	int width = 2000;
	int height = 2000;
	int scale = width / 2;
	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i < model.nfaces(); i++)
	{
		//该面的三个顶点
		Vec3f pts[3] = { model.vert(model.face(i)[0]), model.vert(model.face(i)[1]), model.vert(model.face(i)[2]) };
		//只提取xy投影
		Vec2f pts2[3] = { Vec2f(pts[0].x, pts[0].y), Vec2f(pts[1].x, pts[1].y), Vec2f(pts[2].x, pts[2].y) };
		//缩放平移
		Vec2i pts3[3];
		for (int j = 0; j < 3; j++)
		{
			pts3[j] = Vec2i(pts2[j].x * scale + width / 2, pts2[j].y * scale + height / 2);
		}
		//绘制三角面(随机生成RGB颜色)
		triangle_barycentric(pts3, TGAColor(rand() % 255, rand() % 255, rand() % 255), image);
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle.tga");
}

//按y降序
template<class T>
void sort2Vec2_y(Vec2<T>& v0, Vec2<T>& v1)
{
	if (v1.y > v0.y)
	{
		Vec2i temp = v0;
		v0 = v1;
		v1 = temp;
	}
}
//按x升序
template<class T>
void sort2Vec2_x(Vec2<T>& v0, Vec2<T>& v1)
{
	if (v0.x > v1.x)
	{
		Vec2i temp = v0;
		v0 = v1;
		v1 = temp;
	}
}
//限定用y分割插值线条，生成中间点(v0.y>=v1.y)
void generateLinePointsY(const Vec2i& v0, const Vec2i& v1, std::vector<Vec2i>& ret_points)
{
	//水平线特殊处理
	if (v0.y == v1.y)
	{
		ret_points.push_back(v0);
		ret_points.push_back(v1);
		return;
	}

	float dx = (float)(v1.x - v0.x) / (v0.y - v1.y);
	int xp, yp;
	for (yp = v0.y; yp >= v1.y; yp--)
	{
		xp = v0.x + dx * (v0.y - yp);
		ret_points.push_back(Vec2i(xp, yp));
	}
}

//绘制水平线
void line_horizontal(int y, int x0, int x1, const TGAColor& color, TGAImage& img)
{
	if (x0 > x1)
	{
		int temp = x0;
		x0 = x1;
		x1 = temp;
	}
	for (int x = x0; x <= x1; x++)
	{
		img.set(x, y, color);
	}
}


void triangle(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img)
{
	//按y降序
	Vec2i tempV;
	sort2Vec2_y(v0, v1);
	sort2Vec2_y(v0, v2);
	sort2Vec2_y(v1, v2);

	//v0-v2直接形成一侧边界，另一侧为v0-v1,v1-v2两边一起构成
	//计算X扫描边界点
	std::vector<Vec2i> bound1, bound2;
	generateLinePointsY(v0, v2, bound1);
	generateLinePointsY(v0, v1, bound2);
	bound2.pop_back();//去除重复顶点v1
	generateLinePointsY(v1, v2, bound2);
	//检查
	if (bound1.size() != bound2.size())
	{
		std::cout << "bound1.size()!=bound2.size()";
		return;
	}

	//在bound1和bound2中水平扫描填充
	for (int i = 0; i < bound1.size(); i++)
	{
		line_horizontal(bound1[i].y, bound1[i].x, bound2[i].x, color, img);
	}

	//绘制边界点debug
	for (int i = 0; i < bound1.size(); i++)
	{
		img.set(bound1[i].x, bound1[i].y, red);
		img.set(bound2[i].x, bound2[i].y, red);
	}
}

void triangle_bound(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img)
{
	line(v0, v1, color, img);
	line(v1, v2, color, img);
	line(v2, v0, color, img);
}

void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max)
{
	min.x = std::min(v0.x, std::min(v1.x, v2.x));
	min.y = std::min(v0.y, std::min(v1.y, v2.y));
	max.x = std::max(v0.x, std::max(v1.x, v2.x));
	max.y = std::max(v0.y, std::max(v1.y, v2.y));
}

void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max)
{
	get_boundingbox(pts[0], pts[1], pts[2], min, max);
}

Vec3f barycentric(Vec2i* pts, Vec2i& p)
{
	return barycentric(pts[0], pts[1], pts[2], p);
}

Vec3f barycentric(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& p)
{
	Vec3f A(v1.x - v0.x, v2.x - v0.x, v0.x - p.x);
	Vec3f B(v1.y - v0.y, v2.y - v0.y, v0.y - p.y);
	Vec3f C = A ^ B; //叉乘得到[u,v,1]*k
	if (abs(C.z) < 1) return Vec3f(-1, 1, 1);//避免C.z为0情况（C.z是int，abs小于1即为0）

	C = C * (1 / C.z);//转为[u,v,1]
	return Vec3f(1 - C.x - C.y, C.x, C.y);	//重心坐标[1-u-v,u,v]
}

void triangle_barycentric(Vec2i* pts, const TGAColor& color, TGAImage& img)
{
	//获取包围盒
	Vec2i min, max;
	get_boundingbox(pts, min, max);
	//遍历包围盒内的点
	for (int i = min.x; i <= max.x; i++)
	{
		for (int j = min.y; j <= max.y; j++)
		{
			Vec2i p(i, j);
			Vec3f cord = barycentric(pts, p);
			if (cord.x < 0 || cord.y < 0 || cord.z < 0) //点在三角形外
				continue;
			else
				img.set(i, j, color);
		}
	}
}

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img)
{
	bool steep = false;
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	if (abs(x1 - x0) < abs(y1 - y0))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	float step = abs((y1 - y0)) / (float)(x1 - x0);
	float error = 0;
	int y = y0;
	int dy = (y1 - y0) > 0 ? 1 : -1;
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
			img.set(y, x, color);
		else
			img.set(x, y, color);
		error += step;
		if (error > 0.5)//四舍五入，(y-0.5,y+0.5]之间的点都是y
		{
			y += dy;
			error -= 1;
		}
	}
}

void line(const Vec2i& v0, const Vec2i& v1, const TGAColor& color, TGAImage& img)
{
	line(v0.x, v0.y, v1.x, v1.y, color, img);
}


void mesh(Model& model, const TGAColor& color, TGAImage& img, int scale)
{
	int nf = model.nfaces();
	//原点移至img中心
	int x0 = img.get_width() / 2;
	int y0 = img.get_height() / 2;
	//绘制每个面
	for (int i = 0; i < nf; i++)
	{
		std::vector<int> cur_face = model.face(i);
		Vec3f v1, v2;
		//依次连接该面的每条边
		for (int j = 1; j < cur_face.size(); j++)
		{
			v1 = model.vert(cur_face[j - 1]) * scale;
			v2 = model.vert(cur_face[j]) * scale;
			line(v1.x + x0, v1.y + y0, v2.x + x0, v2.y + y0, color, img);
		}
		v1 = model.vert(cur_face[cur_face.size() - 1]) * scale;
		v2 = model.vert(cur_face[0]) * scale;
		line(v1.x + x0, v1.y + y0, v2.x + x0, v2.y + y0, color, img);
	}
	return;
}


void draw_line_test()
{
	TGAImage img(200, 100, TGAImage::RGB);
	img.set(52, 41, red);
	img.set(52, 42, red2);
	img.set(20, 20, white);
	line(10, 10, 88, 55, blue, img);
	line(50, 68, 185, 20, red, img);
	line(13, 20, 80, 40, white, img);
	line(20, 13, 40, 80, red, img);
	line(80, 40, 13, 20, red, img);
	img.write_tga_file("tga_image.tga", 0);
	img.write_tga_file("tga_image_rle.tga", 1);

	TGAImage img2(200, 100, 24);
	img2.read_tga_file("tga_image.tga");
	TGAColor color = img2.get(52, 41);
	std::cout << "Hello World!\n";
}

void draw_mesh_test()
{
	Model model("obj/african_head.obj");//读取obj文件
	//根据obj面数据绘制所有三角面网格线
	TGAImage img(2000, 2000, TGAImage::RGB);
	mesh(model, white, img, 1000);
	img.flip_vertically();
	img.write_tga_file("obj_mesh.tga", 0);
}

void draw_triangle_test()
{
	TGAImage image(500, 500, TGAImage::RGB);
	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	triangle(t0[0], t0[1], t0[2], red, image);
	//triangle(t1[0], t1[1], t1[2], white, image);
	triangle_barycentric(t1, white, image);
	triangle(t2[0], t2[1], t2[2], green, image);
	triangle_bound(t2[0], t2[1], t2[2], blue, image);
	image.flip_vertically();
	image.write_tga_file("triangle_fill2.tga");
}
