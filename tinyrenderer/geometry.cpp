#include "geometry.h"
#include <cassert>

template<> template<> vec<3, float>::vec(const vec<3, int>& v) : x(v.x), y(v.y), z(v.z) {}
template<> template<> vec<2, float>::vec(const vec<2, int>& v) : x(v.x), y(v.y) {}
//float转int向上取整
template<> template<> vec<3, int>::vec(const vec<3, float>& v) : x(v.x + 0.5f), y(v.y + 0.5f), z(v.z + 0.5f) {}
template<> template<> vec<2, int>::vec(const vec<2, float>& v) : x(v.x + 0.5f), y(v.y + 0.5f) {}


/*
Matrix::Matrix(int r, int c): rows(r),cols(c)
{
	using std::vector;
	m = vector<vector<float>>(r, vector<float>(c, 0.f));
}

inline int Matrix::nrows()
{
	return rows;
}

inline int Matrix::ncols()
{
	return cols;
}

Matrix Matrix::identity(int dimensions)
{
	Matrix mi(dimensions, dimensions);
	for (int i = 0; i < dimensions; i++)
		mi[i][i] = 1;
	return mi;
}

std::vector<float>& Matrix::operator[](const int i)
{
	assert(i >= 0 && i < rows);//越界保护
	return m[i];
}

Matrix Matrix::operator*(const Matrix& a)
{
	assert(cols == a.rows);	//断言符合相乘条件
	Matrix ret(rows, a.cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < a.cols; j++)
		{
			//计算第i行第j列元素值
			for (int k = 0; k < cols; k++)
				ret[i][j] += m[i][k] * a.m[k][j];
		}
	}
	return ret;
}

Matrix Matrix::transpose()
{
	Matrix mt(cols, rows);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			mt[j][i] = m[i][j];
	}
	return mt;
}

Matrix Matrix::inverse()
{
	//待完成
	return Matrix();
}

Matrix v2m(Vec3f v)
{
	Matrix ret(4, 1);
	ret[0][0] = v[0];
	ret[1][0] = v[1];
	ret[2][0] = v[2];
	ret[3][0] = 1.f;
	return ret;
}

Vec3f m2v(Matrix m)
{
	assert(m.nrows() >= 3);
	float coff = m[3][0]; //注意第4维归一化，重投影至原三维空间，否则中心投影出错
	return Vec3f(m[0][0]/ coff, m[1][0]/ coff, m[2][0]/ coff);
}

std::ostream& operator<<(std::ostream& s, Matrix& m)
{
	for (int i = 0; i < m.rows; i++)
	{
		for (int j = 0; j < m.cols; j++)
		{
			s << m[i][j] << "\t";
		}
		s << "\n";
	}
	return s;
}

*/