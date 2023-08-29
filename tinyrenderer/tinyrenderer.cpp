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
void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max, int width, int height);
void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max,int width,int height);//补充画布宽高进行约束
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
void rasterize_triangle_texture(Vec3f* pts, Vec3f* pts_texture, float* zBuffer,TGAImage& img, TGAImage& texture, float intensity=0);
//对点做中心投影（透视投影）
Vec3f central_projection(const Vec3f& pts_in, const Vec3f& camera_pos);


void draw_line_test();
void draw_mesh_test();
void draw_triangle_test();
void draw_mesh_triangle_test();//obj网格三角面测试
void draw_mesh_shadow_test();//obj模型光照阴影测试(简单法向)
void draw_depth_test_line();//线段深度测试
void draw_depth_test_triangle();//三角面深度测试
void draw_mesh_texture_test();//三角面深度测试



int main()
{
	Model model("obj/african_head.obj");//读取obj文件
	TGAImage texture;
	texture.read_tga_file("obj/african_head_diffuse.tga");//加载贴图文件
	texture.flip_vertically();//贴图文件上下翻转

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	//Vec3f camera_pos(0,0,2000);//相机Z轴位置
	Vec3f camera_pos(width/2,height/2,2000);//相机Z轴位置

	//初始化zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//根据obj面数据绘制所有三角面网格线
	for (int i = 0; i < model.nfaces(); i++)
	{
		//该面的三个顶点
		std::vector<int> v_index = model.face(i);
		std::vector<int> t_index = model.face_texture(i);
		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };
		Vec3f pts_texture[3] = { model.texture(t_index[0]), model.texture(t_index[1]), model.texture(t_index[2]) };

		//加入明暗效果-根据面法向量和光照方向计算
		Vec3f fn = get_face_normal(pts);//面法向
		Vec3f light_dir(0, 0, -1);//光照方向-Z轴负方向
		float intensity = (fn * light_dir);//光照强度(向量点积)

		if (intensity > 0)	//背向光线的面不绘制
		{
			//缩放 原点平移至画布中心
			Vec3f pts_scaled[3];
			for (int j = 0; j < 3; j++)
			{
				pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
			}

			//对该三角面做透视变换
			Vec3f pts_projected[3];
			for (int k = 0; k < 3; k++)
				pts_projected[k] = central_projection(pts_scaled[k], camera_pos);

			//绘制三角面
			rasterize_triangle_texture(pts_projected, pts_texture,zBuffer,image,texture, intensity);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_texture_intensity_projection.tga");

	//todo 绘制zBuffer深度图
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

void get_boundingbox(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& min, Vec2i& max, int width, int height)
{
	get_boundingbox(v0, v1, v2, min, max);
	//画布边框约束
	min.x = std::max(0, min.x);
	min.y = std::max(0, min.y);
	max.x = std::min(width-1, max.x);
	max.y = std::min(height-1, max.y);
}

void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max, int width, int height)
{
	get_boundingbox(pts[0], pts[1], pts[2], min, max);
	//画布边框约束
	min.x = std::max(0, min.x);
	min.y = std::max(0, min.y);
	max.x = std::min(width-1, max.x);
	max.y = std::min(height-1, max.y);
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

Vec3f get_face_normal(Vec3f* pts)
{
	Vec3f v1 = pts[1] - pts[0];
	Vec3f v2 = pts[2] - pts[0];
	Vec3f n = v2 ^ v1;
	n.normalize();
	return n;
}

void rasterize_line(Vec2i p0, Vec2i p1, const TGAColor& color, TGAImage& img, float* yBuffer)
{
	if (p1.x < p0.x) std::swap(p1, p0);
	float y_step= (float)(p1.y - p0.y) / (float)(p1.x - p0.x);
	float y = p0.y;
	for (int x = p0.x; x <= p1.x; x++)
	{
		//深度测试
		if (y > yBuffer[x])
		{
			yBuffer[x] = y;
			img.set(x,0, color);
		}
		y += y_step;
	}
}

void rasterize_triangle(Vec3f* pts, float* zBuffer, const TGAColor& color, TGAImage& img)
{
	//计算包围盒
	Vec2i min, max;
	Vec2i pts2i[3] = { Vec2i(pts[0].x,pts[0].y),Vec2i(pts[1].x,pts[1].y) ,Vec2i(pts[2].x,pts[2].y) }; //在此处已取整
	get_boundingbox(pts2i, min, max, img.get_width(), img.get_height());
	//遍历包围盒内的点
	for (int x = min.x; x <= max.x; x++)
	{
		for (int y = min.y; y <= max.y; y++)
		{
			//计算重心坐标
			Vec2i P=Vec2i(x,y);
			Vec3f cord = barycentric(pts2i, P);
			//三角形内部判断
			if (cord.x < 0 || cord.y < 0 || cord.z < 0)
				continue;
			else
			{
				//计算当前点深度(利用重心坐标)
				float z = pts[0].z * cord.x + pts[1].z * cord.y + pts[2].z * cord.z;
				if (z > zBuffer[x + y * img.get_width()])//深度测试成功
				{
					zBuffer[x + y * img.get_width()]= z;//更新深度缓冲
					img.set(x, y, color);
				}
			}
		}
	}
}

void rasterize_triangle_texture(Vec3f* pts, Vec3f* pts_texture, float* zBuffer, TGAImage& img, TGAImage& texture,float intensity)
{
	//计算包围盒
	Vec2i min, max;
	Vec2i pts2i[3] = { Vec2i(pts[0].x,pts[0].y),Vec2i(pts[1].x,pts[1].y) ,Vec2i(pts[2].x,pts[2].y) }; //在此处已取整
	get_boundingbox(pts2i, min, max,img.get_width(),img.get_height());
	//遍历包围盒内的点
	for (int x = min.x; x <= max.x; x++)
	{
		for (int y = min.y; y <= max.y; y++)
		{
			//计算重心坐标
			Vec2i P = Vec2i(x, y);
			Vec3f cord = barycentric(pts2i, P);
			//三角形内部判断
			if (cord.x < 0 || cord.y < 0 || cord.z < 0)
				continue;
			else
			{
				//计算当前点深度(利用重心坐标)
				float z = pts[0].z * cord.x + pts[1].z * cord.y + pts[2].z * cord.z;
				if (z > zBuffer[x + y * img.get_width()])//深度测试成功
				{
					zBuffer[x + y * img.get_width()] = z;//更新深度缓冲
					//获取对应贴图颜色
					Vec3f texture_cord = pts_texture[0] * cord.x + pts_texture[1] * cord.y + pts_texture[2] * cord.z;//计算插值贴图坐标
					int t_x = texture_cord.x * texture.get_width();//注意贴图坐标需要乘贴图文件宽高
					int t_y = texture_cord.y * texture.get_height();
					TGAColor color = texture.get(t_x, t_y);
					for (int i = 0; i < 3; i++) 
						color.raw[i] = color.raw[i] * intensity;//调整亮度intensity=[0~1]

					img.set(x, y, color);
				}
			}
		}
	}
}

Vec3f central_projection(const Vec3f &pts_in, const Vec3f &camera_pos)
{
	if (camera_pos.z == 0) return Vec3f();
	float r = 1 - pts_in.z / camera_pos.z;
	if (r == 0) return Vec3f();

	Vec3f pts_trans(pts_in.x - camera_pos.x, pts_in.y - camera_pos.y, pts_in.z);//相对相机坐标平移(仅x,y平面)
	pts_trans = pts_trans * (1 / r);//透视变换
	pts_trans.x += camera_pos.x;//平移回原画布位置
	pts_trans.y += camera_pos.y;//平移回原画布位置

	return pts_trans;
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

void draw_mesh_triangle_test()
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

void draw_mesh_shadow_test()
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
		//加入明暗效果-根据面法向量和光照方向计算
		Vec3f fn = get_face_normal(pts);//面法向
		Vec3f light_dir(0, 0, -1);//光照方向-Z轴负方向
		float intensity = (fn * light_dir);//光照强度(向量点积)
		if (intensity > 0)	//背向光线的面不绘制
		{
			//绘制三角面
			//triangle_barycentric(pts3, TGAColor(intensity*255, intensity*255, intensity*255), image);
			triangle_barycentric(pts3, TGAColor(rand() % 255, rand() % 255, rand() % 255), image);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_visible.tga");

}

void draw_depth_test_line()
{
	const int width = 800, height = 1;
	TGAImage image(width, height, TGAImage::RGB);
	float yBuffer[width];//深度缓冲
	//memset(yBuffer, -1, sizeof(yBuffer));

	rasterize_line(Vec2i(20, 34), Vec2i(744, 400), red, image, yBuffer);
	rasterize_line(Vec2i(120, 434), Vec2i(444, 400), green, image, yBuffer);
	rasterize_line(Vec2i(330, 463), Vec2i(594, 200), blue, image, yBuffer);

	image.flip_vertically();
	image.write_tga_file("z_buffer_test_line.tga");
}

void draw_depth_test_triangle()
{


	Model model("obj/african_head.obj");//读取obj文件

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);
	//初始化zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//根据obj面数据绘制所有三角面网格线
	for (int i = 0; i < model.nfaces(); i++)
	{
		//该面的三个顶点
		Vec3f pts[3] = { model.vert(model.face(i)[0]), model.vert(model.face(i)[1]), model.vert(model.face(i)[2]) };
		//缩放平移
		Vec3f pts_scaled[3];
		for (int j = 0; j < 3; j++)
		{
			pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
		}
		//加入明暗效果-根据面法向量和光照方向计算
		Vec3f fn = get_face_normal(pts);//面法向
		Vec3f light_dir(0, 0, -1);//光照方向-Z轴负方向
		float intensity = (fn * light_dir);//光照强度(向量点积)
		if (intensity > 0)	//背向光线的面不绘制
		{
			//绘制三角面
			rasterize_triangle(pts_scaled, zBuffer, TGAColor(intensity * 255, intensity * 255, intensity * 255), image);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_visible_zBuffer.tga");

	//todo 绘制zBuffer深度图
}

void draw_mesh_texture_test()
{
	Model model("obj/african_head.obj");//读取obj文件
	TGAImage texture;
	texture.read_tga_file("obj/african_head_diffuse.tga");//加载贴图文件
	texture.flip_vertically();//贴图文件上下翻转

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	//初始化zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//根据obj面数据绘制所有三角面网格线
	for (int i = 0; i < model.nfaces(); i++)
	{
		//该面的三个顶点
		std::vector<int> v_index = model.face(i);
		std::vector<int> t_index = model.face_texture(i);
		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };
		Vec3f pts_texture[3] = { model.texture(t_index[0]), model.texture(t_index[1]), model.texture(t_index[2]) };

		//缩放平移
		Vec3f pts_scaled[3];
		for (int j = 0; j < 3; j++)
		{
			pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
		}
		//加入明暗效果-根据面法向量和光照方向计算
		Vec3f fn = get_face_normal(pts);//面法向
		Vec3f light_dir(0, 0, -1);//光照方向-Z轴负方向
		float intensity = (fn * light_dir);//光照强度(向量点积)
		if (intensity > 0)	//背向光线的面不绘制
		{
			//绘制三角面
			//rasterize_triangle_texture(pts_scaled, pts_texture,zBuffer,image,texture);
			rasterize_triangle_texture(pts_scaled, pts_texture, zBuffer, image, texture, intensity);
		}
	}

	image.flip_vertically();
	//image.write_tga_file("mesh_triangle_texture.tga");
	image.write_tga_file("mesh_triangle_texture_intensity.tga");

	//todo 绘制zBuffer深度图
}
