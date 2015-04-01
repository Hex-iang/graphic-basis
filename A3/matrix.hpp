#pragma once 
#include "vector.hpp"
#include <math.h>
#include <ostream>

template<typename T>
class Matrix
{
public:
	T m[4][4];

	Matrix()
	{
		// create a identity matrix by default
		m[0][0] = T(1.0); m[0][1] = T(0.0); m[0][2] = T(0.0); m[0][3] = T(0.0);
		m[1][0] = T(0.0); m[1][1] = T(1.0); m[1][2] = T(0.0); m[1][3] = T(0.0);
		m[2][0] = T(0.0); m[2][1] = T(0.0); m[2][2] = T(1.0); m[2][3] = T(0.0);
		m[3][0] = T(0.0); m[3][1] = T(0.0); m[3][2] = T(0.0); m[3][3] = T(1.0);
	}
	Matrix(
		const T &c00, const T &c01, const T &c02, const T &c03,
		const T &c10, const T &c11, const T &c12, const T &c13,
		const T &c20, const T &c21, const T &c22, const T &c23,
		const T &c30, const T &c31, const T &c32, const T &c33)
	{
		m[0][0] = c00; m[0][1] = c01; m[0][2] = c02; m[0][3] = c03;
		m[1][0] = c10; m[1][1] = c11; m[1][2] = c12; m[1][3] = c13;
		m[2][0] = c20; m[2][1] = c21; m[2][2] = c22; m[2][3] = c23;
		m[3][0] = c30; m[3][1] = c31; m[3][2] = c32; m[3][3] = c33;
	}

	const T* operator [] (size_t i) const { return &m[i][0]; }
	T* operator [] (size_t i) { return &m[i][0]; }

	// transform a point in homogeneous form (see OpenExr lib)
	template<typename S>
	Vec3<S> multVecMatrix(const Vec3<S> &src) const
	{
		S x = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0] + m[3][0];
		S y = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1] + m[3][1];
		S z = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2] + m[3][2];
		S w = src.x * m[0][3] + src.y * m[1][3] + src.z * m[2][3] + m[3][3];

		return Vec3<S>( x / w, y / w, z / w);
	}
	// we are just interested in rotations. Use the 3x3 sub-matrix
	template<typename S>
	Vec3<S> multDirMatrix(const Vec3<S> &src) const
	{
		S d1 = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0];
		S d2 = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1];
		S d3 = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2];

		return Vec3<S>(d1, d2, d3);
	}
	friend std::ostream & operator << (std::ostream &os, const Matrix<T> &m)
	{
		os << "[";
		os.precision(7);
		const T *c = &m[0][0];
		for (size_t i = 0; i < 16; ++i) {
			os << *(c++) << ((i==15) ? "]" : " ");
		}
		return os;
	}
	Matrix operator * (const Matrix &right) const
	{
		Matrix r;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				r.m[i][j] = 
					m[i][0] * right.m[0][j] +
					m[i][1] * right.m[1][j] +
					m[i][2] * right.m[2][j] +
					m[i][3] * right.m[3][j];
		return r;
	}
	Matrix inverse() const;
	Matrix transpose() const
	{
		Matrix t;
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				t[i][j] = m[j][i];
			}
		}
		return t;
	}

	static Matrix<T> scale(const T &sx, const T &sy, const T &sz)
	{
		// creat a scale matrix
		return Matrix<T>(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
	}

	const Matrix<T> & setTranslation(const Vec3<T> &t)
	{
		m[0][0] = 1; 		m[0][1] = 0; 		m[0][2] = 0; 		m[0][3] = 0;
		m[1][0] = 0; 		m[1][1] = 1; 		m[1][2] = 0; 		m[1][3] = 0;
		m[2][0] = 0; 		m[2][1] = 0; 		m[2][2] = 1;	 	m[2][3] = 0;
		m[3][0] = t.x; 	m[3][1] = t.y; 	m[3][2] = t.z; 	m[3][3] = 1;

		return *this;
	}
};

template<typename T>
Matrix<T> Matrix<T>::inverse() const
{
	int i, j, k;
	Matrix<T> s;
	Matrix<T> t (*this);

	// Forward elimination
	for (i = 0; i < 3 ; i++) {
		int pivot = i;

		T pivotsize = t[i][i];

		if (pivotsize < 0)
			pivotsize = -pivotsize;

		for (j = i + 1; j < 4; j++) {
			T tmp = t[j][i];

			if (tmp < 0)
				tmp = -tmp;

			if (tmp > pivotsize) {
				pivot = j;
				pivotsize = tmp;
			}
		}

		if (pivotsize == 0) {
			return Matrix();
		}

		if (pivot != i) {
			for (j = 0; j < 4; j++) {
				T tmp;

				tmp = t[i][j];
				t[i][j] = t[pivot][j];
				t[pivot][j] = tmp;

				tmp = s[i][j];
				s[i][j] = s[pivot][j];
				s[pivot][j] = tmp;
			}
		}

		for (j = i + 1; j < 4; j++) {
			T f = t[j][i] / t[i][i];

			for (k = 0; k < 4; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	// Backward substitution
	for (i = 3; i >= 0; --i) {
		T f;

		if ((f = t[i][i]) == 0) {
			//if (singExc)
			//	throw ::Imath::SingMatrixExc ("Cannot invert singular matrix.");

			return Matrix();
		}

		for (j = 0; j < 4; j++) {
			t[i][j] /= f;
			s[i][j] /= f;
		}

		for (j = 0; j < i; j++) {
			f = t[j][i];

			for (k = 0; k < 4; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	return s;
}

typedef Matrix<float> Mat4;

