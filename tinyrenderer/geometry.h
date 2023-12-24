#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>
#include <cassert>


template<size_t DimRow, size_t DimCol, typename T> class mat;
//=============�ع� ͨ��vec�ṹ===============
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
//�ػ�2ά����
template <typename T> struct vec<2, T>
{
	vec() :x(T()), y(T()) {}
	vec(T X, T Y) :x(X), y(Y) {}

	//��ͬtype�������캯�����ػ�ʵ����cpp��
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

//�ػ�3ά����
template <typename T> struct vec<3, T>
{
	vec() :x(T()), y(T()), z(T()) {}
	vec(T X, T Y, T Z) :x(X), y(Y), z(Z) {}

	//��ͬtype�������캯�����ػ�ʵ����cpp��
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

//�����������������

template <size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs) //�ӷ�
{
	for (int i = 0; i < DIM; i++)
		lhs[i] += rhs[i];
	return lhs;
}

template <size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) //����
{
	for (int i = 0; i < DIM; i++)
		lhs[i] -= rhs[i];
	return lhs;
}

template <size_t DIM, typename T>
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs)	//���
{
	T ret = T();
	for (int i = 0; i < DIM; i++)
		ret += lhs[i] * rhs[i];
	return ret;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator*(vec<DIM, T> lhs, const U& rhs) //���� ע������rhs����ʹ�ó�����/�����ã������޷�������ʱ����
{
	for (int i = 0; i < DIM; i++)
		lhs[i] *= rhs;
	return lhs;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs)	//����
{
	for (int i = 0; i < DIM; i++)
		lhs[i] /= rhs;
	return lhs;
}

template <size_t DIM, typename T, typename U>
vec<DIM, T> operator-(vec<DIM, T> lhs, const U& rhs) //�ӷ�(����)
{
	for (int i = 0; i < DIM; i++)
		lhs[i] -= rhs;
	return lhs;
}

//ע��embed����ʵ����ʱ����ʽָ��ģ�廯����<LEN>����ά��ά�ȣ�(��ΪC++����ͨ������ֵ�������ֺ�����)
template <size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1)//��ά���
{
	vec<LEN, T> ret;
	for (int i = 0; i < LEN; i++)
		ret[i] = (i < DIM) ? v[i] : fill;
	return ret;
}



template <size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T>& v)//��άͶӰ
{
	vec<LEN, T> ret;
	for (int i = 0; i < LEN; i++)
		ret[i] = v[i];
	return ret;
}

//���󣡣�������ģ�岻֧�ֲ����ػ���ֻ��ȫ�ػ�������
//4ά��3άͶӰ�ػ�����������ͶӰ�任�н�ά������1-z/c��
//template <typename T> vec<3, T> proj<3,4,T>(const vec<4, T>& v)
//{
//	vec<3, T> ret;
//	for (int i = 0; i < 3; i++)
//		ret[i] = v[i] / v[3];
//	return ret;
//}

//4ά��3άͶӰ�ػ�����������ͶӰ�任�н�ά������1-z/c��
//����ȫ�ػ��������Ϊinline����extern�����������ظ��������(�ѳ�Ϊ��ͨ�������޷��ظ�����)
template <>
inline vec<3, float> proj<3, 4, float>(const vec<4, float>& v) 
{
	vec<3, float> ret;
	for (int i = 0; i < 3; i++)
		ret[i] = v[i] / v[3];
	return ret;
}


//����ά����ʵ�ֲ��
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

//============����ʽ����==============
//����ʽ����������һ��(��)�ĸ�Ԫ�������Ӧ�Ĵ���ʽ����ʽ�˻�֮��
template<size_t DIM, typename T>
static T mat_det(const mat<DIM, DIM, T>& src)
{
	T ret = 0;
	for (size_t i = 0; i < DIM; i++)
		ret += src[0][i] * src.cofactor(0, i);
	return ret;
}

template<typename T>
static T mat_det(const mat<1, 1, T>& src)	//������ģ������ݹ����ʹ���ػ���Ϊ��ֹ����
{
	return src[0][0];
}


//=============�ع� ͨ��mat������===============
template<size_t DimRow, size_t DimCol, typename T>
class mat
{
	vec<DimCol, T> rows[DimRow];
public:
	mat() {}
	~mat() {}

	vec<DimCol, T>& operator[](size_t idx) //ȡ������
	{
		assert(idx < DimRow);
		return rows[idx];
	}
	const vec<DimCol, T>& operator[](size_t idx)const //ȡ�������������汾��
	{
		assert(idx < DimRow);
		return rows[idx];
	}

	vec<DimRow, T> col(size_t idx) const//ȡ������
	{
		assert(idx < DimCol);
		vec<DimRow, T> ret;
		for (int i = 0; i < DimRow; i++) ret[i] = rows[i][idx];
		return ret;
	}

	void set_col(size_t idx, vec<DimRow, T> v)//����������
	{
		assert(idx < DimCol);
		for (int i = 0; i < DimRow; i++)  rows[i][idx] = v[i];
	}

	static mat<DimRow, DimCol, T> identity()//���ɵ�λ��
	{
		mat<DimRow, DimCol, T> ret;
		for (size_t i = DimRow; i--;)
		{
			for (size_t j = DimCol; j--;)
				ret[i][j] = (i == j);
		}
		return ret;
	}

	T det() const//����ʽ����
	{
		////����ʽ����������һ��(��)�ĸ�Ԫ�������Ӧ�Ĵ���ʽ����ʽ�˻�֮��
		//assert(DimCol == DimRow);

		//if (DimRow == 1) return rows[0][0];
		//T ret = 0;
		//for (size_t i = 0; i < DimRow; i++)
		//	ret += rows[0][i] * this->cofactor(0, i);
		//return T
		return mat_det(*this);
	}

	mat<DimRow - 1, DimCol - 1, T> get_minor(size_t row, size_t col) const //��ijԪ������ʽ����
	{
		mat<DimRow - 1, DimCol - 1, T> ret;
		for (size_t i = DimRow-1; i--;)
		{
			for (size_t j = DimCol-1; j--;)
				ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1];
		}
		return ret;
	}

	T cofactor(size_t row, size_t col) const//���������ʽ
	{
		int c = ((row + col) % 2) == 0 ? 1 : -1;
		return c * get_minor(row, col).det();
	}

	mat<DimRow, DimCol, T> adjugate() const //��������
	{
		mat<DimRow, DimCol, T> ret;
		for (size_t i = DimRow; i--;)	//ע���һ���ж�ʱ����ִ��i--����i��ֵʵ��ΪDim-1
		{
			for (size_t j = DimCol; j--;)
			{
				ret[j][i] = this->cofactor(i, j); //ע��˴�i,j˳�򣬰��������ת��
			}
		}
		return ret;
	}

	mat<DimRow, DimCol, T> invert() const //�������
	{
		mat<DimRow, DimCol, T> ret = this->adjugate();//�������
		T tmpdet = rows[0] * ret.col(0);//��ñ�������ʽ

		return ret / tmpdet;
	}

	mat<DimRow, DimCol, T> transpose() const //��ת�þ���
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
//�����

//�������������
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

//�������
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

//��������
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

//��������
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

//�����ӡ���
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
	//��ostream <<����Ϊ��Ԫ������ʹ�ÿ���ֱ��ʹ��cout<<v������� (friend����/���ֱ�ӷ��ʱ����˽�г�Ա)
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

	static Matrix identity(int dimensions);//���ɵ�λ��

	std::vector<float>& operator[](const int i);//����[]ȡ��������������ʹ��M[i][j]��λԪ��
	Matrix operator*(const Matrix& a);
	Matrix transpose();//ת��
	Matrix inverse();//�����

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};




//������תΪ�����ʽ-�Ա������˽�������任
Matrix v2m(Vec3f v);
//�����ʽ������תΪVec3f
Vec3f m2v(Matrix m);
*/

#endif // !__GEOMETRY_H__

