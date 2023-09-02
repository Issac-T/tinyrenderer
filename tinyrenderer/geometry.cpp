#include "geometry.h"
#include <cassert>

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

