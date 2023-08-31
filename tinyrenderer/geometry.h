#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>

//ģ�廯����������֧��ͨ�õĺ�������ֵ����������
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
	inline Vec2<T> operator+(const Vec2<T>& V) const //ֻ������Ϊconst�ĳ�Ա�������ܱ�const�������
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
	//��ostream����Ϊ��Ԫ������ʹ�ÿ���ֱ��ʹ��cout<<v������� (friend����/���ֱ�ӷ��ʱ����˽�г�Ա)
	friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <class T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v)
{
	s << "(" << v.u << "," << v.v << ")" << std::endl;
	return s;
}

//ģ�廯����������֧��ͨ�õĺ�������ֵ����������
template <class T>
struct Vec3
{
	union
	{
		struct
		{
			T x, y, z;//��������ʽ
		};
		struct
		{
			T ivert, iuv, inorm;//���㡢�������߹���������ʽ
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
	inline Vec3<T> operator+(const Vec3<T>& V) const //ֻ������Ϊconst�ĳ�Ա�������ܱ�const�������
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
	//�������
	inline T operator*(const Vec3<T>& V) const
	{
		return (x * V.x + y * V.y + z * V.z);
	}
	//�������
	inline Vec3<T> operator^(const Vec3<T>& V) const
	{
		return Vec3<T>(y * V.z - z * V.y, z * V.x - x * V.z, x * V.y - y * V.x);
	}
	//��������ģ��(���׷���)
	float norm() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}
	//��һ��(lΪ��ѡ�����ģ����Ĭ��1��Ϊ�顰1����)
	Vec3<T>& normalize(T len = 1)	//Ϊʲô�������ã�
	{
		float n = norm();
		*this = (*this) * (len / n);
		return *this;
	}
	//��ostream����Ϊ��Ԫ������ʹ�ÿ���ֱ��ʹ��cout<<v������� (friend����/���ֱ�ӷ��ʱ����˽�г�Ա)
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

