#pragma once

struct Matrix{
public:
	Matrix() = default;
	Matrix(const Matrix& other) = default;

	constexpr Matrix(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44) {

		__m128 firstLine{ _mm_load_ss(&_11) };
		__m128 secondLine{ _mm_load_ss(&_21) };
		__m128 thirdLine{ _mm_load_ss(&_31) };
		__m128 fourthLine{ _mm_load_ss(&_41) };

		_mm_store_ss(_mat[0], firstLine);
		_mm_store_ss(_mat[1], secondLine);
		_mm_store_ss(_mat[2], thirdLine);
		_mm_store_ss(_mat[3], fourthLine);
	}

	Matrix operator*(const Matrix& other);
	Matrix& operator=(const Matrix& other) = default;

#if defined(_MSC_VER)
	__forceinline
#else
	inline
#endif
	static Matrix GetIdentityMatrix() {
		return 	{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	alignas(16) float _mat[4][4]{};
};


