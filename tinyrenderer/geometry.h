#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>
#include <cassert>


template<size_t DimRow, size_t DimCol, typename T> class mat;
//=============重构 通用vec结构===============
template <size_t DIM, typename T> struct vec
{
private:
	T data[DIM];
public:
	vec()
	{
		for (int i = DIM; i--; data[i] = T());
	}
	T& operator[](const size_t i)
	{
		assert(i < DIM);
		return data[i];
	}
	const T& operator[](const size_t i) const
	{
		assert(i < DIM);
		return data[i];
	}
};
//特化2维向量
template <typename T> struct vec<2, T>
{
	vec() :x(T()), y(T()) {}
	vec(T X, T Y) :x(X), y(Y) {}

	//不同type拷贝构造函数，特化实现在cpp中
	template <class U> vec<2, T>(const vec<2, U>& v);

	T& operator[](const size_t i)
	{
		assert(i < 2); return i <= 0 ? x : y;
	}
	const T& operator[](const size_t i) const
	{
		assert(i < 2); return i <= 0 ? x : y;
	}

	T x, y;
};

//特化3维向量
template <typename T> struct vec<3, T>
{
	vec() :x(T()), y(T()), z(T()) {}
	vec(T X, T Y, T Z) :x(X), y(Y), z(Z) {}

	//不同type拷贝构造函数，特化实现在cpp中
	template <class U> vec<3, T>(const vec<3, U>& v);

	T& operator[](const size_t i)
	{
		assert(i < 3);
		if (i <= 0) return x;
		else if (i == 1) return y;
		else return z;
	}
	const T& operator[](const size_t i) const
	{
		assert(i < 3);
		if (i <= 0) return x;
		else if (i == 1) return y;
		else return z;
	}
	float norm()
	{
		return std::sqrtf(x * x + y * y + z * z);
	}
	vec<3, T>& normalize(T len = 1)
	{
		*this = (*this) * (len / this->norm());
		return *this;
	}

	T x, y, z;
};

//矩阵算数运算符重载

template <size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs) //加法
{
	for (int i = 0; i < DIM; i++)
		lhs[i] += rhs[i];
	return lhs;
}

template <size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) //减法
{
	for (int i = 0; i < DIM; i++)
		lhs[i] -= rhs[i];
	return lhs;
}

template <size_t DIM, typename T>
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs)	//点积
{
	T ret = T();
	for (int i = 0; i < DIM; i++)
		ret += lhs[i] * rhs[i];
	return ret;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator*(vec<DIM, T> lhs, const U& rhs) //数乘 注意数字rhs必须使用常引用/非引用，否则无法传入临时变量
{
	for (int i = 0; i < DIM; i++)
		lhs[i] *= rhs;
	return lhs;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs)	//数除
{
	for (int i = 0; i < DIM; i++)
		lhs[i] /= rhs;
	return lhs;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator-(vec<DIM, T> lhs, const U& rhs) //加法(数加)
{
	for (int i = 0; i < DIM; i++)
		lhs[i] -= rhs;
	return lhs;
}

//注意embed函数实例化时需显式指定模板化参数<LEN>（升维后维度）(因为C++不能通过返回值类型区分函数？)
template <size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1)//升维填充
{
	vec<LEN, T> ret;
	for (int i = 0; i < LEN; i++)
		ret[i] = (i < DIM) ? v[i] : fill;
	return ret;
}



template <size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T>& v)//降维投影
{
	vec<LEN, T> ret;
	for (int i = 0; i < LEN; i++)
		ret[i] = v[i];
	return ret;
}

//错误！！！函数模板不支持部分特化，只能全特化！！！
//4维降3维投影特化（用于中心投影变换中降维，除以1-z/c）
//template <typename T> vec<3, T> proj<3,4,T>(const vec<4, T>& v)
//{
//	vec<3, T> ret;
//	for (int i = 0; i < 3; i++)
//		ret[i] = v[i] / v[3];
//	return ret;
//}

//4维降3维投影特化（用于中心投影变换中降维，除以1-z/c）
//函数全特化后必须作为inline或者extern，否则会出现重复定义错误(已成为普通函数，无法重复包含)
template <>
inline vec<3, float> proj<3, 4, float>(const vec<4, float>& v) 
{
	vec<3, float> ret;
	for (int i = 0; i < 3; i++)
		ret[i] = v[i] / v[3];
	return ret;
}


//对三维向量实现叉乘
template<typename T> vec<3, T> cross(vec<3, T> v1, vec<3, T> v2)
{
	vec<3, T> ret;
	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;
	return ret;
}

template<size_t DIM, typename T> std::ostream& operator<<(std::ostream& os, vec<DIM, T>& v)
{
	for (int i = 0; i < DIM; i++)
		os << v[i] << " ";
	return os;
}

//============行列式计算==============
//行列式等于它任意一行(列)的各元素与其对应的代数式余子式乘积之和
template<size_t DIM, typename T>
static T mat_det(const mat<DIM, DIM, T>& src)
{
	T ret = 0;
	for (size_t i = 0; i < DIM; i++)
		ret += src[0][i] * src.cofactor(0, i);
	return ret;
}

template<typename T>
static T mat_det(const mat<1, 1, T>& src)	//非类型模板参数递归必须使用特化作为终止条件
{
	return src[0][0];
}


//=============重构 通用mat矩阵类===============
template<size_t DimRow, size_t DimCol, typename T>
class mat
{
	vec<DimCol, T> rows[DimRow];
public:
	mat() {}
	~mat() {}

	vec<DimCol, T>& operator[](size_t idx) //取行向量
	{
		assert(idx < DimRow);
		return rows[idx];
	}
	const vec<DimCol, T>& operator[](size_t idx)const //取行向量（常量版本）
	{
		assert(idx < DimRow);
		return rows[idx];
	}

	vec<DimRow, T> col(size_t idx) const//取列向量
	{
		assert(idx < DimCol);
		vec<DimRow, T> ret;
		for (int i = 0; i < DimRow; i++) ret[i] = rows[i][idx];
		return ret;
	}

	void set_col(size_t idx, vec<DimRow, T> v)//设置列向量
	{
		assert(idx < DimCol);
		for (int i = 0; i < DimRow; i++)  rows[i][idx] = v[i];
	}

	static mat<DimRow, DimCol, T> identity()//生成单位阵
	{
		mat<DimRow, DimCol, T> ret;
		for (size_t i = DimRow; i--;)
		{
			for (size_t j = DimCol; j--;)
				ret[i][j] = (i == j);
		}
		return ret;
	}

	T det() const//行列式计算
	{
		////行列式等于它任意一行(列)的各元素与其对应的代数式余子式乘积之和
		//assert(DimCol == DimRow);

		//if (DimRow == 1) return rows[0][0];
		//T ret = 0;
		//for (size_t i = 0; i < DimRow; i++)
		//	ret += rows[0][i] * this->cofactor(0, i);
		//return T
		return mat_det(*this);
	}

	mat<DimRow - 1, DimCol - 1, T> get_minor(size_t row, size_t col) const //第ij元素余子式矩阵
	{
		mat<DimRow - 1, DimCol - 1, T> ret;
		for (size_t i = DimRow-1; i--;)
		{
			for (size_t j = DimCol-1; j--;)
				ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1];
		}
		return ret;
	}

	T cofactor(size_t row, size_t col) const//求代数余子式
	{
		int c = ((row + col) % 2) == 0 ? 1 : -1;
		return c * get_minor(row, col).det();
	}

	mat<DimRow, DimCol, T> adjugate() const //求伴随矩阵
	{
		mat<DimRow, DimCol, T> ret;
		for (size_t i = DimRow; i--;)	//注意第一次判断时即会执行i--，故i初值实际为Dim-1
		{
			for (size_t j = DimCol; j--;)
			{
				ret[j][i] = this->cofactor(i, j); //注意此处i,j顺序，伴随矩阵是转置
			}
		}
		return ret;
	}

	mat<DimRow, DimCol, T> invert() const //求逆矩阵
	{
		mat<DimRow, DimCol, T> ret = this->adjugate();//伴随矩阵
		T tmpdet = rows[0] * ret.col(0);//求得本阶行列式

		return ret / tmpdet;
	}

	mat<DimRow, DimCol, T> transpose() const //求转置矩阵
	{
		mat<DimRow, DimCol, T> ret;
		for (size_t i = DimRow; i--;)
		{
			for (size_t j = DimCol; j--;)
			{
				ret[i][j] = rows[j][i];
			}
		}
		return ret;

	}
};

//////////////////////////////////////////////////////////////////////////
//运算符

//矩阵左乘列向量
template<size_t DIMRows, size_t DIMCols, typename T>
vec<DIMRows, T> operator* (const mat<DIMRows, DIMCols, T>& lhs, const vec<DIMCols, T> rhs)
{
	vec<DIMRows, T> ret;
	for (size_t i = DIMRows; i--;)
	{
		ret[i] = lhs[i] * rhs;
	}
	return ret;
}

//矩阵相乘
template<size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator* (const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs)
{
	mat<R1, C2, T> ret;
	for (size_t i = R1; i--;)
	{
		for (size_t j = C2; j--;)
			ret[i][j] = lhs[i] * rhs.col(j);
	}
	return ret;
}

//矩阵数乘
template<size_t DIMRows, size_t DIMCols, typename T>
mat<DIMRows, DIMCols, T> operator* (const mat<DIMRows, DIMCols, T>& lhs, const T num)
{
	mat<DIMRows, DIMCols, T> ret;
	for (size_t i = DIMRows; i--;)
	{
		for (size_t j = DIMCols; j--;)
			ret[i][j] = lhs[i][j] * num;
	}
	return ret;
}

//矩阵数除
template<size_t DIMRows, size_t DIMCols, typename T>
mat<DIMRows, DIMCols, T> operator/ (const mat<DIMRows, DIMCols, T>& lhs, const T num)
{
	mat<DIMRows, DIMCols, T> ret;
	for (size_t i = DIMRows; i--;)
	{
		for (size_t j = DIMCols; j--;)
			ret[i][j] = lhs[i][j] / num;
	}
	return ret;
}

//矩阵打印输出
template<size_t DIMRows, size_t DIMCols, typename T>
std::ostream& operator<<(std::ostream& os, mat<DIMRows, DIMCols, T>& m)
{
	for (size_t i = 0; i < DIMRows; i++)
		os << m[i] << std::endl;
	return os;
}

//////////////////////////////////////////////////////////////////////////
typedef vec<2, float> Vec2f;
typedef vec<3, float> Vec3f;
typedef vec<4, float> Vec4f;
typedef vec<2, int > Vec2i;
typedef vec<3, int> Vec3i;
typedef mat<4, 4, float> Matrix;


/*

//模板化两点向量，支持通用的含有两个值的向量类型
template <class T>
struct Vec2
{
	union
	{
		struct
		{
			T x, y;
		};
		struct
		{
			T u, v;
		};
		T raw[2];
	};

	Vec2() :u(0), v(0) {}
	Vec2(T _u, T _v) :u(_u), v(_v) {}
	T& operator[](const int i)
	{
		if (i <= 0) return x;
		else return y;
	}
	inline Vec2<T> operator+(const Vec2<T>& V) const //只有修饰为const的成员函数才能被const对象调用
	{
		return Vec2<T>(u + V.u, v + V.v);
	}
	inline Vec2<T> operator-(const Vec2<T>& V) const
	{
		return Vec2<T>(u - V.u, v - V.v);
	}
	inline Vec2<T> operator*(float f) const
	{
		return Vec2<T>(u * f, v * f);
	}
	//将ostream声明为友元函数，使得可以直接使用cout<<v输出向量 (friend函数/类可直接访问本类的私有成员)
	friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <class T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v)
{
	s << "(" << v.u << "," << v.v << ")" << std::endl;
	return s;
}

//模板化三点向量，支持通用的含有三个值的向量类型
template <class T>
struct Vec3
{
	union
	{
		struct
		{
			T x, y, z;//三坐标形式
		};
		struct
		{
			T ivert, iuv, inorm;//顶点、纹理、法线关联索引形式
		};
		T raw[3];
	};

	Vec3() :x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) {}
	T& operator[](const int i)
	{
		if (i <= 0) return x;
		if (i == 1) return y;
		else return z;
	}
	inline Vec3<T> operator+(const Vec3<T>& V) const //只有修饰为const的成员函数才能被const对象调用
	{
		return Vec3<T>(x + V.x, y + V.y, z + V.z);
	}
	inline Vec3<T> operator-(const Vec3<T>& V) const
	{
		return Vec3<T>(x - V.x, y - V.y, z - V.z);
	}
	inline Vec3<T> operator*(float f) const
	{
		return Vec3<T>(x * f, y * f, z * f);
	}
	//向量点乘
	inline T operator*(const Vec3<T>& V) const
	{
		return (x * V.x + y * V.y + z * V.z);
	}
	//向量叉乘
	inline Vec3<T> operator^(const Vec3<T>& V) const
	{
		return Vec3<T>(y * V.z - z * V.y, z * V.x - x * V.z, x * V.y - y * V.x);
	}
	//计算向量模长(二阶范数)
	float norm() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}
	//归一化(l为可选处理后模长，默认1即为归“1”化)
	Vec3<T>& normalize(T len = 1)	//为什么返回引用？
	{
		float n = norm();
		*this = (*this) * (len / n);
		return *this;
	}
	//将ostream <<声明为友元函数，使得可以直接使用cout<<v输出向量 (friend函数/类可直接访问本类的私有成员)
	friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
};

template <class T> std::ostream& operator<<(std::ostream& s, Vec3<T>& v)
{
	s << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
	return s;
}

typedef Vec2<float> Vec2f;
typedef Vec2<int>	Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>	Vec3i;


const int DEFAULT_MATRIX_SIZE = 4;

class Matrix
{
	std::vector<std::vector<float>> m;
	int rows, cols;
public:
	Matrix(int r = DEFAULT_MATRIX_SIZE, int c = DEFAULT_MATRIX_SIZE);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);//生成单位阵

	std::vector<float>& operator[](const int i);//重载[]取出行向量，方便使用M[i][j]定位元素
	Matrix operator*(const Matrix& a);
	Matrix transpose();//转置
	Matrix inverse();//逆矩阵

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};




//点坐标转为矩阵格式-以便矩阵相乘进行坐标变换
Matrix v2m(Vec3f v);
//矩阵格式点坐标转为Vec3f
Vec3f m2v(Matrix m);
*/

#endif // !__GEOMETRY_H__

