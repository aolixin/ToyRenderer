#pragma once

#include <ostream>
#include <math.h>
#include <assert.h>
#include <cstdint>
#include <sstream>

// Vector ������ 
template <size_t N, typename T>
struct Vector
{
	T data[N];

	inline Vector()
	{
		for (size_t i = 0; i < N; i++)
		{
			data[i] = T();
		}
	}

	Vector(const T& val)
	{
		for (size_t i = 0; i < N; i++)
		{
			data[i] = val;
		}
	}

	inline Vector(const T* ptr)
	{
		for (size_t i = 0; i < N; i++)
		{
			data[i] = ptr[i];
		}
	}

	inline Vector(const Vector<N, T>& v)
	{
		for (size_t i = 0; i < N; i++)
		{
			data[i] = v.data[i];
		}
	}

	inline Vector(const std::initializer_list<T>& list)
	{
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			data[i] = *it;
			i++;
		}
	}

	inline const T& operator[](size_t i) const
	{
		assert(i < N);
		return data[i];
	}

	inline T& operator[](size_t i)
	{
		assert(i < N);
		return data[i];
	}

	inline void load(const T* ptr)
	{
		for (size_t i = 0; i < N; i++)
		{
			data[i] = ptr[i];
		}
	}

	inline void save(T* ptr) const
	{
		for (size_t i = 0; i < N; i++)
		{
			ptr[i] = data[i];
		}
	}
};

// ��ά Vector �ػ�
template <typename T>
struct Vector<2, T>
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

		T data[2];
	};

	inline Vector()
	{
		x = y = T();
	}

	inline explicit Vector(const T& val)
	{
		x = y = val;
	}

	inline explicit Vector(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
	}

	inline Vector(const Vector<2, T>& v)
	{
		x = v.x;
		y = v.y;
	}

	inline Vector(const std::initializer_list<T>& list)
	{
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			data[i] = *it;
			i++;
		}
	}

	inline const T& operator[](size_t i) const
	{
		assert(i < 2);
		return data[i];
	}

	inline T& operator[](size_t i)
	{
		assert(i < 2);
		return data[i];
	}

	inline void load(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
	}

	inline void save(T* ptr) const
	{
		ptr[0] = x;
		ptr[1] = y;
	}

	inline Vector<2, T> xy() const
	{
		return *this;
	}

	inline Vector<2, T> yx() const
	{
		return Vector<2, T>(y, x);
	}

	inline Vector<2, T> xx() const
	{
		return Vector<2, T>(x, x);
	}

	inline Vector<2, T> yy() const
	{
		return Vector<2, T>(y, y);
	}

	inline Vector<2, T> xy1() const
	{
		return Vector<3, T>(x, y, 1);
	}

	inline Vector<2, T> xy11() const
	{
		return Vector<4, T>(x, y, 1, 1);
	}
};

// ��ά Vector �ػ�
template <typename T>
struct Vector<3, T>
{
	union
	{
		struct
		{
			T x, y, z;
		};

		struct
		{
			T r, g, b;
		};

		struct
		{
			T u, v, w;
		};

		T data[3];
	};

	inline Vector()
	{
		x = y = z = T();
	}

	inline explicit Vector(const T& val)
	{
		x = y = z = val;
	}

	inline Vector(T x, T y, T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline explicit Vector(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
		z = ptr[2];
	}

	inline Vector(const Vector<3, T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline Vector(const std::initializer_list<T>& list)
	{
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			data[i] = *it;
			i++;
		}
	}

	inline const T& operator[](size_t i) const
	{
		assert(i < 3);
		return data[i];
	}

	inline T& operator[](size_t i)
	{
		assert(i < 3);
		return data[i];
	}

	inline void load(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
		z = ptr[2];
	}

	inline void save(T* ptr) const
	{
		ptr[0] = x;
		ptr[1] = y;
		ptr[2] = z;
	}

	inline Vector<2, T> xy() const
	{
		return Vector<2, T>(x, y);
	}

	inline Vector<3, T> xyz() const
	{
		return *this;
	}

	inline Vector<4, T> xyz1() const
	{
		return Vector<4, T>(x, y, z, 1);
	}
};


// ��ά Vector �ػ�
template <typename T>
struct Vector<4, T>
{
	union
	{
		struct
		{
			T x, y, z, w;
		};

		struct
		{
			T r, g, b, a;
		};

		T data[4];
	};


	inline Vector()
	{
		x = y = z = w = T();
	}

	inline explicit Vector(const T& val)
	{
		x = y = z = w = val;
	}

	inline explicit Vector(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
		z = ptr[2];
		w = ptr[3];
	}

	inline Vector(const Vector<4, T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	inline Vector(const std::initializer_list<T>& list)
	{
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			data[i] = *it;
			i++;
		}
	}

	inline Vector(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W)
	{
	}

	inline const T& operator[](size_t i) const
	{
		assert(i < 4);
		return data[i];
	}

	inline T& operator[](size_t i)
	{
		assert(i < 4);
		return data[i];
	}

	inline void load(const T* ptr)
	{
		x = ptr[0];
		y = ptr[1];
		z = ptr[2];
		w = ptr[3];
	}

	inline void save(T* ptr) const
	{
		ptr[0] = x;
		ptr[1] = y;
		ptr[2] = z;
		ptr[3] = w;
	}

	inline Vector<2, T> xy() const
	{
		return Vector<2, T>(x, y);
	}

	inline Vector<3, T> xyz() const
	{
		return Vector<3, T>(x, y, z);
	}

	inline Vector<4, T> xyzw() const
	{
		return Vector<4, T>(x, y, z, w);
	}
};


/*
--------------------------------------���������-----------------------------------------
*/

// -a
template <size_t N, typename T>
inline Vector<N, T> operator -(const Vector<N, T>& v)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = -v[i];
	}
	return result;
}

// a == b
template <size_t N, typename T>
inline bool operator==(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	for (size_t i = 0; i < N; i++)
	{
		if (v1[i] != v2[i])
		{
			return false;
		}
	}
	return true;
}

// a != b
template <size_t N, typename T>
inline bool operator!=(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	return !(v1 == v2);
}

// a + b
template <size_t N, typename T>
inline Vector<N, T> operator+(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = v1[i] + v2[i];
	}
	return result;
}

// a - b
template <size_t N, typename T>
inline Vector<N, T> operator-(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = v1[i] - v2[i];
	}
	return result;
}

// a * b
template <size_t N, typename T>
inline Vector<N, T> operator*(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = v1[i] * v2[i];
	}
	return result;
}

// a / b
template <size_t N, typename T>
inline Vector<N, T> operator/(const Vector<N, T>& v1, const Vector<N, T>& v2)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = v1[i] / v2[i];
	}
	return result;
}

// a * x
template <size_t N, typename T>
inline Vector<N, T> operator*(const Vector<N, T>& a, const T& x)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] * x;
	}
	return result;
}

// x * a
template <size_t N, typename T>
inline Vector<N, T> operator*(const T& x, const Vector<N, T>& a)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] * x;
	}
	return result;
}

// a / x
template <size_t N, typename T>
inline Vector<N, T> operator/(const Vector<N, T>& a, const T& x)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] / x;
	}
	return result;
}

// x / a
template <size_t N, typename T>
inline Vector<N, T> operator/(const T& x, const Vector<N, T>& a)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = x / a[i];
	}
	return result;
}

// a += b
template <size_t N, typename T>
inline Vector<N, T>& operator+=(Vector<N, T>& a, const Vector<N, T>& b)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] += b[i];
	}
	return a;
}

// a -= b
template <size_t N, typename T>
inline Vector<N, T>& operator-=(Vector<N, T>& a, const Vector<N, T>& b)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] -= b[i];
	}
	return a;
}

// a *= b
template <size_t N, typename T>
inline Vector<N, T>& operator*=(Vector<N, T>& a, const Vector<N, T>& b)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] *= b[i];
	}
	return a;
}

// a /= b
template <size_t N, typename T>
inline Vector<N, T>& operator/=(Vector<N, T>& a, const Vector<N, T>& b)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] /= b[i];
	}
	return a;
}


// a *= x
template <size_t N, typename T>
inline Vector<N, T>& operator*=(Vector<N, T>& a, const T& x)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] *= x;
	}
	return a;
}

// a /= x
template <size_t N, typename T>
inline Vector<N, T>& operator/=(Vector<N, T>& a, const T& x)
{
	for (size_t i = 0; i < N; i++)
	{
		a[i] /= x;
	}
	return a;
}


/*
------------------------------ʸ������
*/

// ά��ת��
template <size_t N1, size_t N2, typename T>
inline Vector<N1, T> vector_convert(const Vector<N2, T>& a, T fill = 1)
{
	Vector<N1, T> result;
	for (size_t i = 0; i < N1; i++)
	{
		result[i] = i < N2 ? a[i] : fill;
	}
	return result;
}

// |a| ^ 2
template <size_t N, typename T>
inline T vector_length_square(const Vector<N, T>& a)
{
	T result = 0;
	for (size_t i = 0; i < N; i++)
	{
		result += a[i] * a[i];
	}
	return result;
}

// |a|
template <size_t N, typename T>
inline T vector_length(const Vector<N, T>& a)
{
	return sqrt(vector_length_square(a));
}

// |a|
template <size_t N>
inline float vector_length(const Vector<N, float>& a)
{
	return sqrtf(vector_length_square(a));
}

// a / |a|
template <size_t N, typename T>
inline Vector<N, T> vector_normalize(const Vector<N, T>& a)
{
	return a / vector_length(a);
}

// ʸ�����
template <size_t N, typename T>
inline T vector_dot(const Vector<N, T>& a, const Vector<N, T>& b)
{
	T result = 0;
	for (size_t i = 0; i < N; i++)
	{
		result += a[i] * b[i];
	}
	return result;
}

// ��ά�������
template <typename T>
inline T vector_cross(const Vector<2, T>& a, const Vector<2, T>& b)
{
	return a.x * b.y - a.y * b.x;
}

// ��ά�������
template <typename T>
inline Vector<3, T> vector_cross(const Vector<3, T>& a, const Vector<3, T>& b)
{
	return Vector<3, T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

// ��άʸ����ˣ�ǰ��ά��ˣ���һλ����
template <typename T>
inline Vector<4, T> vector_cross(const Vector<4, T>& a, const Vector<4, T>& b)
{
	return Vector<4, T>(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
		a.w);
}

// a + (b - a) * t
template <size_t N, typename T>
inline Vector<N, T> vector_lerp(const Vector<N, T>& a, const Vector<N, T>& b, T t)
{
	return a + (b - a) * t;
}

// ����Ԫ��ȡ���ֵ
template <size_t N, typename T>
inline Vector<N, T> vector_max(const Vector<N, T>& a, const Vector<N, T>& b)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] > b[i] ? a[i] : b[i];
	}
	return result;
}

// ����Ԫ��ȡ��Сֵ
template <size_t N, typename T>
inline Vector<N, T> vector_min(const Vector<N, T>& a, const Vector<N, T>& b)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] < b[i] ? a[i] : b[i];
	}
	return result;
}

// ��ʸ����ֵ������ minx/maxx ��Χ��
template <size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T>& a, const Vector<N, T>& minx, const Vector<N, T>& maxx)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] < minx[i] ? minx[i] : (a[i] > maxx[i] ? maxx[i] : a[i]);
	}
	return result;
}

// ʸ��ֵԪ�ط�Χ�ü�
template <size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T>& a, const T& minx = 0, const T& maxx = 1)
{
	Vector<N, T> result;
	for (size_t i = 0; i < N; i++)
	{
		result[i] = a[i] < minx ? minx : (a[i] > maxx ? maxx : a[i]);
	}
	return result;
}

// ������ı���
template <size_t N, typename T>
inline std::ostream& operator<<(std::ostream& os, const Vector<N, T>& v)
{
	os << "[";
	for (size_t i = 0; i < N; i++)
	{
		os << v[i];
		if (i != N - 1)
		{
			os << ",";
		}
	}
	os << "]";
	return os;
}

// ������ַ���
template <size_t N, typename T>
inline std::string vector_repr(const Vector<N, T>& v)
{
	std::stringstream ss;
	ss << v;
	return ss.str();
}

//---------------------------------------------------------------------
// ��ѧ�⣺����
//---------------------------------------------------------------------

template <size_t ROW, size_t COL, typename T>
struct Matrix
{
	T data[ROW][COL];

	inline Matrix()
	{
		for (size_t i = 0; i < ROW; i++)
		{
			for (size_t j = 0; j < COL; j++)
			{
				data[i][j] = T();
			}
		}
	}

	inline explicit Matrix(const T& val)
	{
		for (size_t i = 0; i < ROW; i++)
		{
			for (size_t j = 0; j < COL; j++)
			{
				data[i][j] = val;
			}
		}
	}

	inline Matrix(const Matrix<ROW, COL, T>& src)
	{
		for (size_t i = 0; i < ROW; i++)
		{
			for (size_t j = 0; j < COL; j++)
			{
				data[i][j] = src.data[i][j];
			}
		}
	}

	inline Matrix(const std::initializer_list<T>& list)
	{
		size_t i = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			data[i / COL][i % COL] = *it;
			i++;
		}
	}


	inline Matrix(const std::initializer_list<Vector<COL, T>>& u)
	{
		auto it = u.begin();
		for (size_t i = 0; i < ROW; i++) SetRow(i, *it++);
	}

	inline const T* operator[](size_t row) const
	{
		assert(row < ROW);
		return data[row];
	}

	inline T* operator[](size_t row)
	{
		assert(row < ROW);
		return data[row];
	}


	// ȡһ��
	inline Vector<COL, T> Row(size_t row) const
	{
		assert(row < ROW);
		Vector<COL, T> a;
		for (size_t i = 0; i < COL; i++) a[i] = data[row][i];
		return a;
	}

	// ȡһ��
	inline Vector<ROW, T> Col(size_t col) const
	{
		assert(col < COL);
		Vector<ROW, T> a;
		for (size_t i = 0; i < ROW; i++) a[i] = data[i][col];
		return a;
	}

	// ����һ��
	inline void SetRow(size_t row, const Vector<COL, T>& a)
	{
		assert(row < ROW);
		for (size_t i = 0; i < COL; i++) data[row][i] = a[i];
	}

	// ����һ��
	inline void SetCol(size_t col, const Vector<ROW, T>& a)
	{
		assert(col < COL);
		for (size_t i = 0; i < ROW; i++) data[i][col] = a[i];
	}


	// ȡ��ɾ��ĳ�к�ĳ�е��Ӿ�����ʽ
	inline Matrix<ROW - 1, COL - 1, T> GetMinor(size_t row, size_t col) const
	{
		Matrix<ROW - 1, COL - 1, T> ret;
		for (size_t r = 0; r < ROW - 1; r++)
		{
			for (size_t c = 0; c < COL - 1; c++)
			{
				ret.data[r][c] = data[r < row ? r : r + 1][c < col ? c : c + 1];
			}
		}
		return ret;
	}

	// ȡ��ת�þ���
	inline Matrix<COL, ROW, T> Transpose() const
	{
		Matrix<COL, ROW, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.data[c][r] = data[r][c];
		}
		return ret;
	}

	// ȡ�� 0 ����
	inline static Matrix<ROW, COL, T> GetZero()
	{
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.data[r][c] = 0;
		}
		return ret;
	}

	// ȡ�õ�λ����
	inline static Matrix<ROW, COL, T> GetIdentity()
	{
		Matrix<ROW, COL, T> ret;
		for (size_t r = 0; r < ROW; r++)
		{
			for (size_t c = 0; c < COL; c++)
				ret.data[r][c] = (r == c) ? 1 : 0;
		}
		return ret;
	}
};


/*
--------------------------------��������------------------------------
*/

// ==
template <size_t ROW, size_t COL, typename T>
inline bool operator==(const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b)
{
	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < COL; j++)
		{
			if (a[i][j] != b[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

// !=
template <size_t ROW, size_t COL, typename T>
inline bool operator!=(const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b)
{
	return !(a == b);
}

// -a
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T>& a)
{
	Matrix<ROW, COL, T> result;
	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < COL; j++)
		{
			result[i][j] = -a[i][j];
		}
	}
	return result;
}

// a + b
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator+(const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b)
{
	Matrix<ROW, COL, T> result;
	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < COL; j++)
		{
			result[i][j] = a[i][j] + b[i][j];
		}
	}
	return result;
}

// a - b
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T>& a, const Matrix<ROW, COL, T>& b)
{
	Matrix<ROW, COL, T> result;
	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < COL; j++)
		{
			result[i][j] = a[i][j] - b[i][j];
		}
	}
	return result;
}

// a * b
template <size_t ROW, size_t COL, size_t NEWCOL, typename T>
inline Matrix<ROW, NEWCOL, T> operator*(const Matrix<ROW, COL, T>& a, const Matrix<COL, NEWCOL, T>& b)
{
	Matrix<ROW, NEWCOL, T> result;
	for (size_t i = 0; i < ROW; i++)
	{
		for (size_t j = 0; j < NEWCOL; j++)
		{
			result[i][j] = vector_dot(a.Row(i), b.Col(j));
		}
	}
	return result;
}

// a * x
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator *(const Matrix<ROW, COL, T>& a, T x)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.m[j][i] = a.m[j][i] * x;
		}
	}
	return out;
}

// x * a
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator *(T x, const Matrix<ROW, COL, T>& a)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.m[j][i] = a.m[j][i] * x;
		}
	}
	return out;
}

// a / x
template <size_t ROW, size_t COL, typename T>
inline Matrix<ROW, COL, T> operator /(const Matrix<ROW, COL, T>& a, T x)
{
	Matrix<ROW, COL, T> out;
	for (size_t j = 0; j < ROW; j++)
	{
		for (size_t i = 0; i < COL; i++)
		{
			out.data[j][i] = a.data[j][i] / x;
		}
	}
	return out;
}


// vecx * a
template <size_t ROW, size_t COL, typename T>
inline Vector<COL, T> operator *(const Vector<ROW, T>& x, const Matrix<ROW, COL, T>& a)
{
	Vector<COL, T> b;
	for (size_t i = 0; i < COL; i++)
		b[i] = vector_dot(x, a.Col(i));
	return b;
}

// a * vecx
template <size_t ROW, size_t COL, typename T>
inline Vector<ROW, T> operator *(const Matrix<ROW, COL, T>& a, const Vector<COL, T>& x)
{
	Vector<ROW, T> b;
	for (size_t i = 0; i < ROW; i++)
		b[i] = vector_dot(x, a.Row(i));
	return b;
}

/*
------------------------------����ʽ����ѧ��--------------------------
*/

// ����ʽ��ֵ��һ��
template <typename T>
inline T matrix_det(const Matrix<1, 1, T>& m)
{
	return m[0][0];
}

// ����ʽ��ֵ������
template <typename T>
inline T matrix_det(const Matrix<2, 2, T>& m)
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

// ����ʽ��ֵ���������ʽ������һ��ͬ���ǵ�����ʽ������
template <size_t N, typename T>
inline T matrix_det(const Matrix<N, N, T>& m)
{
	T sum = 0;
	for (size_t i = 0; i < N; i++) sum += m[0][i] * matrix_cofactor(m, 0, i);
	return sum;
}

// ����ʽ��һ��
template <typename T>
inline T matrix_cofactor(const Matrix<1, 1, T>& m, size_t row, size_t col)
{
	return 0;
}

// �������ʽ����ɾ���ض����е���ʽ������ʽֵ
template <size_t N, typename T>
inline T matrix_cofactor(const Matrix<N, N, T>& m, size_t row, size_t col)
{
	return matrix_det(m.GetMinor(row, col)) * (((row + col) % 2) ? -1 : 1);
}

// ������󣺼�����ʽ�����ת��
template <size_t N, typename T>
inline Matrix<N, N, T> matrix_adjoint(const Matrix<N, N, T>& m)
{
	Matrix<N, N, T> ret;
	for (size_t j = 0; j < N; j++)
	{
		for (size_t i = 0; i < N; i++) ret[j][i] = matrix_cofactor(m, i, j);
	}
	return ret;
}

// �������ʹ�ð�������������ʽ��ֵ�õ�
template <size_t N, typename T>
inline Matrix<N, N, T> matrix_invert(const Matrix<N, N, T>& m)
{
	Matrix<N, N, T> ret = matrix_adjoint(m);
	T det = vector_dot(m.Row(0), ret.Col(0));
	return ret / det;
}

// ������ı���
template <size_t ROW, size_t COL, typename T>
inline std::ostream& operator <<(std::ostream& os, const Matrix<ROW, COL, T>& m)
{
	for (size_t r = 0; r < ROW; r++)
	{
		Vector<COL, T> row = m.Row(r);
		os << row << std::endl;
	}
	return os;
}

//---------------------------------------------------------------------
// ���ߺ���
//---------------------------------------------------------------------
template <typename T>
inline T Abs(T x) { return (x < 0) ? (-x) : x; }

template <typename T>
inline T Max(T x, T y) { return (x < y) ? y : x; }

template <typename T>
inline T Min(T x, T y) { return (x > y) ? y : x; }

template <typename T>
inline bool NearEqual(T x, T y, T error)
{
	return (Abs(x - y) < error);
}

template <typename T>
inline T Between(T xmin, T xmax, T x)
{
	return Min(Max(xmin, x), xmax);
}

// ��ȡ [0, 1] �ķ�Χ
template <typename T>
inline T Saturate(T x)
{
	return Between<T>(0, 1, x);
}

// ���ͱ���
typedef Vector<2, float> Vec2f;
typedef Vector<2, double> Vec2d;
typedef Vector<2, int> Vec2i;
typedef Vector<3, float> Vec3f;
typedef Vector<3, double> Vec3d;
typedef Vector<3, int> Vec3i;
typedef Vector<4, float> Vec4f;
typedef Vector<4, double> Vec4d;
typedef Vector<4, int> Vec4i;

typedef Matrix<4, 4, float> Mat4x4f;
typedef Matrix<3, 3, float> Mat3x3f;
typedef Matrix<4, 3, float> Mat4x3f;
typedef Matrix<3, 4, float> Mat3x4f;



//---------------------------------------------------------------------
// 3D ��ѧ����
//---------------------------------------------------------------------

// ʸ��ת������ɫ
inline static uint32_t vector_to_color(const Vec4f& color)
{
	uint32_t r = (uint32_t)Between(0, 255, (int)(color.r * 255.0f));
	uint32_t g = (uint32_t)Between(0, 255, (int)(color.g * 255.0f));
	uint32_t b = (uint32_t)Between(0, 255, (int)(color.b * 255.0f));
	uint32_t a = (uint32_t)Between(0, 255, (int)(color.a * 255.0f));
	return (r << 16) | (g << 8) | b | (a << 24);
}

// ʸ��ת��������ɫ
inline static uint32_t vector_to_color(const Vec3f& color)
{
	return vector_to_color(color.xyz1());
}

// ������ɫ��ʸ��
inline static Vec4f vector_from_color(uint32_t rgba)
{
	Vec4f out;
	out.r = (float)((rgba >> 16) & 0xff) / 255.0f;
	out.g = (float)((rgba >> 8) & 0xff) / 255.0f;
	out.b = (float)((rgba >> 0) & 0xff) / 255.0f;
	out.a = (float)((rgba >> 24) & 0xff) / 255.0f;
	return out;
}

// matrix set to zero
inline static Mat4x4f matrix_set_zero()
{
	Mat4x4f m;
	m.data[0][0] = m.data[0][1] = m.data[0][2] = m.data[0][3] = 0.0f;
	m.data[1][0] = m.data[1][1] = m.data[1][2] = m.data[1][3] = 0.0f;
	m.data[2][0] = m.data[2][1] = m.data[2][2] = m.data[2][3] = 0.0f;
	m.data[3][0] = m.data[3][1] = m.data[3][2] = m.data[3][3] = 0.0f;
	return m;
}

// set to identity
inline static Mat4x4f matrix_set_identity()
{
	Mat4x4f m;
	m.data[0][0] = m.data[1][1] = m.data[2][2] = m.data[3][3] = 1.0f;
	m.data[0][1] = m.data[0][2] = m.data[0][3] = 0.0f;
	m.data[1][0] = m.data[1][2] = m.data[1][3] = 0.0f;
	m.data[2][0] = m.data[2][1] = m.data[2][3] = 0.0f;
	m.data[3][0] = m.data[3][1] = m.data[3][2] = 0.0f;
	return m;
}

// ƽ�Ʊ任
inline static Mat4x4f matrix_set_translate(float x, float y, float z)
{
	Mat4x4f m = matrix_set_identity();
	m.data[3][0] = x;
	m.data[3][1] = y;
	m.data[3][2] = z;
	return m;
}

// ���ű任
inline static Mat4x4f matrix_set_scale(float x, float y, float z)
{
	Mat4x4f m = matrix_set_identity();
	m.data[0][0] = x;
	m.data[1][1] = y;
	m.data[2][2] = z;
	return m;
}

// ��Ӱ���任����eye/�ӵ�λ�ã�at/�������up/ָ���Ϸ���ʸ��
inline static Mat4x4f matrix_set_lookat(const Vec3f& eye, const Vec3f& at, const Vec3f& up)
{
	Vec3f zaxis = vector_normalize(at - eye);
	Vec3f xaxis = vector_normalize(vector_cross(zaxis, up));
	Vec3f yaxis = vector_cross(xaxis, zaxis);
	Mat4x4f m;
	m.SetCol(0, Vec4f(xaxis.x, xaxis.y, xaxis.z, -vector_dot(xaxis, eye)));
	m.SetCol(1, Vec4f(yaxis.x, yaxis.y, yaxis.z, -vector_dot(yaxis, eye)));
	m.SetCol(2, Vec4f(zaxis.x, zaxis.y, zaxis.z, -vector_dot(zaxis, eye)));
	m.SetCol(3, Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	return m;
}

// directx MatrixPerspectiveFovLH
inline static Mat4x4f matrix_set_perspective(float fovy, float aspect, float zn, float zf)
{
	// without reverse z
	//float tanHalfFovy = 1 / tan(fovy / 2);
	//Mat4x4f m = matrix_set_zero();
	//m.data[0][0] = tanHalfFovy / aspect;
	//m.data[1][1] = tanHalfFovy;
	//m.data[2][2] = zf / (zf - zn);
	//m.data[3][2] = -zn * zf / (zf - zn);
	//m.data[2][3] = 1;

	// reverse z
	float tanHalfFovy = 1 / tan(fovy / 2);
	Mat4x4f m = matrix_set_zero();
	m.data[0][0] = tanHalfFovy / aspect;
	m.data[1][1] = tanHalfFovy;
	m.data[2][2] = zn / (zn - zf);
	m.data[3][2] = -zn * zf / (zn - zf);
	m.data[2][3] = 1;
	return m;
}


inline static Mat4x4f matrix_set_ortho(float left, float right, float bottom, float top, float zn, float zf)
{
	Mat4x4f m = matrix_set_identity();
	m.data[0][0] = 2.0f / (right - left);
	m.data[1][1] = 2.0f / (top - bottom);
	m.data[2][2] = 2.0f / (zf - zn);
	m.data[3][0] = (left + right) / (left - right);
	m.data[3][1] = (bottom + top) / (bottom - top);
	m.data[3][2] = zn / (zn - zf);
	return m;
}


// ��ת�任��Χ�� (x, y, z) ʸ����ת theta �Ƕ�
inline static Mat4x4f matrix_set_rotate(float x, float y, float z, float theta)
{
	float qsin = (float)sin(theta * 0.5f);
	float qcos = (float)cos(theta * 0.5f);
	float w = qcos;
	Vec3f vec = vector_normalize(Vec3f(x, y, z));
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;
	Mat4x4f m;
	m.data[0][0] = 1 - 2 * y * y - 2 * z * z;
	m.data[1][0] = 2 * x * y - 2 * w * z;
	m.data[2][0] = 2 * x * z + 2 * w * y;
	m.data[0][1] = 2 * x * y + 2 * w * z;
	m.data[1][1] = 1 - 2 * x * x - 2 * z * z;
	m.data[2][1] = 2 * y * z - 2 * w * x;
	m.data[0][2] = 2 * x * z - 2 * w * y;
	m.data[1][2] = 2 * y * z + 2 * w * x;
	m.data[2][2] = 1 - 2 * x * x - 2 * y * y;
	m.data[0][3] = m.data[1][3] = m.data[2][3] = 0.0f;
	m.data[3][0] = m.data[3][1] = m.data[3][2] = 0.0f;
	m.data[3][3] = 1.0f;
	return m;
}
