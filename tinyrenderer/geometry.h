#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>

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
	//将ostream声明为友元函数，使得可以直接使用cout<<v输出向量 (friend函数/类可直接访问本类的私有成员)
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

#endif // !__GEOMETRY_H__

