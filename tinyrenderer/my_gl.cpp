#include "my_gl.h"


//全局使用的变换矩阵
Matrix ModelView;  //模型坐标系->相机坐标系
Matrix Viewport;   //相机坐标系->屏幕坐标系
Matrix Projection; //中心投影变换

void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer)
{
	Vec3f verts[3] = { proj<3>(pts[0]),proj<3>(pts[1]),proj<3>(pts[2]) };
	//计算包围盒
	Vec2i min, max;
	Vec2i pts2i[3] = { Vec2i(verts[0].x,verts[0].y),Vec2i(verts[1].x,verts[1].y) ,Vec2i(verts[2].x,verts[2].y) }; //在此处已取整
	get_boundingbox(pts2i, min, max, image.get_width(), image.get_height());
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
				//计算当前点深度(利用重心坐标)(顶点Z已在viewport变换中缩放至[0-255]范围)
				float z = verts[0].z * cord.x + verts[1].z * cord.y + verts[2].z * cord.z;
				if (z > zbuffer.get(x, y).raw[0])//深度测试成功
				{
					//渲染该点色值
					TGAColor color;
					bool discard = shader.fragment(cord, color);
					if (!discard) //在fragment中可判定是否丢弃该点光栅化
					{
						zbuffer.set(x, y, TGAColor(z, 1));//更新深度缓冲
						image.set(x, y, color);
					}
				}
			}
		}
	}
}





//-------以下为Shader前期课程练习代码-------

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor red2 = TGAColor(255, 0, 0, 100);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

//按y降序
template<class T>
void sort2Vec2_y(vec<2, T>& v0, vec<2, T>& v1)
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
void sort2Vec2_x(vec<2, T>& v0, vec<2, T>& v1)
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


void triangle_simple(Vec2i v0, Vec2i v1, Vec2i v2, const TGAColor& color, TGAImage& img)
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
	max.x = std::min(width - 1, max.x);
	max.y = std::min(height - 1, max.y);
}

void get_boundingbox(Vec2i* pts, Vec2i& min, Vec2i& max, int width, int height)
{
	get_boundingbox(pts[0], pts[1], pts[2], min, max);
	//画布边框约束
	min.x = std::max(0, min.x);
	min.y = std::max(0, min.y);
	max.x = std::min(width - 1, max.x);
	max.y = std::min(height - 1, max.y);
}

Vec3f barycentric(Vec2i* pts, Vec2i& p)
{
	return barycentric(pts[0], pts[1], pts[2], p);
}

Vec3f barycentric(Vec2i& v0, Vec2i& v1, Vec2i& v2, Vec2i& p)
{
	Vec3f A(v1.x - v0.x, v2.x - v0.x, v0.x - p.x);
	Vec3f B(v1.y - v0.y, v2.y - v0.y, v0.y - p.y);
	Vec3f C = cross(A, B); //叉乘得到[u,v,1]*k
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
	Vec3f n = cross(v2, v1);
	n.normalize();
	return n;
}

void rasterize_line(Vec2i p0, Vec2i p1, const TGAColor& color, TGAImage& img, float* yBuffer)
{
	if (p1.x < p0.x) std::swap(p1, p0);
	float y_step = (float)(p1.y - p0.y) / (float)(p1.x - p0.x);
	float y = p0.y;
	for (int x = p0.x; x <= p1.x; x++)
	{
		//深度测试
		if (y > yBuffer[x])
		{
			yBuffer[x] = y;
			img.set(x, 0, color);
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
					img.set(x, y, color);
				}
			}
		}
	}
}

void rasterize_triangle_texture(Vec3f* pts, float* zBuffer, int f_idx, Model& model, TGAImage& img, float intensity)
{
	//预获取顶点uv
	Vec2f uvs[3] = { model.uv(f_idx,0),model.uv(f_idx,1),model.uv(f_idx,2) };//顶点uv
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
					Vec2f pix_uv = uvs[0] * cord.x + uvs[1] * cord.y + uvs[2] * cord.z;//计算插值贴图坐标
					TGAColor color = model.diffuse(pix_uv);
					color = color * intensity;

					img.set(x, y, color);
				}
			}
		}
	}
}

Vec3f central_projection(const Vec3f& pts_in, const Vec3f& camera_pos)
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

Matrix camera_matrix(Vec3f& camera_pos)
{
	Matrix t1 = Matrix::identity();	//相对相机平移(仅XY平面)
	Matrix t2 = Matrix::identity();	//t1的逆
	Matrix p1 = Matrix::identity();	//关于原点的中心投影
	t1[0][3] = -camera_pos[0];
	t1[1][3] = -camera_pos[1];

	t2[0][3] = camera_pos[0];
	t2[1][3] = camera_pos[1];

	p1[3][2] = -1.f / camera_pos[2]; //r = -1/c

	return t2 * p1 * t1;
}

Matrix trans_scale_matrix(int img_width, int img_height, Vec3f scale3)
{
	Matrix ret = Matrix::identity();
	//平移
	ret[0][3] = img_width / 2.f;
	ret[1][3] = img_height / 2.f;
	//缩放(主对角线元素)
	ret[0][0] = scale3.x;
	ret[1][1] = scale3.y;
	ret[2][2] = scale3.z;
	return ret;
}

Matrix trans_scale_matrix(int img_width, int img_height, float scale)
{
	return trans_scale_matrix(img_width, img_height, Vec3f(scale, scale, scale));
}

Matrix trans_scale_matrix_1(Vec3f move, float scale)
{
	return trans_scale_matrix_1(move, Vec3f(scale, scale, scale));
}

Matrix trans_scale_matrix_1(Vec3f move, Vec3f scale3)
{
	Matrix ret = Matrix::identity();
	//平移
	ret[0][3] = move.x;
	ret[1][3] = move.y;
	ret[2][3] = move.z;
	//缩放(对角线元素)
	ret[0][0] = scale3.x;
	ret[1][1] = scale3.y;
	ret[2][2] = scale3.z;
	return ret;
}

Matrix u_lookat(Vec3f& eye_pos, Vec3f& center, Vec3f& up)
{
	Vec3f cz = eye_pos - center;//视线方向为Z轴
	Vec3f cy = up;
	Vec3f cx = cross(cy, cz);
	cy = cross(cz, cx);//生成X轴后要对y重新叉乘得到，因为输入up可能不与视线z正交
	cz.normalize();
	cy.normalize();
	cx.normalize();
	Matrix R = Matrix::identity();//变基(旋转)
	Matrix T = Matrix::identity();//原点平移
	for (int i = 0; i < 3; i++)
	{
		R[i][0] = cx[i];
		R[i][1] = cy[i];
		R[i][2] = cz[i];

		T[i][3] = -center[i];
	}
	R = R.transpose();//转置为逆矩阵
	Matrix ret = R * T;
	return ret;
}

Matrix camera_matrix(float camera_z)
{
	Matrix P = Matrix::identity();
	P[3][2] = -1.f / camera_z; //r = -1/c
	return P;
}

Matrix viewport_matrix(int x, int y, int w, int h)
{
	const float depth = 255;//z轴（深度）缩放至0-255范围，以便绘制深度图
	Matrix ret = Matrix::identity();
	//缩放
	ret[0][0] = w / 2.f;
	ret[1][1] = h / 2.f;
	ret[2][2] = depth / 2.f;
	//平移
	ret[0][3] = x + w / 2.f;
	ret[1][3] = y + h / 2.f;
	ret[2][3] = depth / 2.f;
	return ret;
}

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img)
{
	bool steep = false;
	if (abs(x1 - x0) < abs(y1 - y0))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
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

IShader::~IShader()
{
}
