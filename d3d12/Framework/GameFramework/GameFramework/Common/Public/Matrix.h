#pragma once

struct Matrix{
public:

	//TODO : SIMD Ãß°¡
	Matrix() = default;
	Matrix(const Matrix& other) = default;

	constexpr Matrix(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44) {

		_mat[0][0] = _11; _mat[0][1] = _12; _mat[0][2] = _13; _mat[0][3] = _14;
		_mat[1][0] = _21; _mat[1][1] = _22; _mat[1][2] = _23; _mat[1][3] = _24;
		_mat[2][0] = _31; _mat[2][1] = _32; _mat[2][2] = _33; _mat[2][3] = _34;
		_mat[3][0] = _41; _mat[3][1] = _42; _mat[3][2] = _43; _mat[3][3] = _44;
	}

	Matrix operator*(const Matrix& other);
	Matrix& operator=(const Matrix& other) = default;

	static Matrix GetIdentityMatrix() {
		return 	{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	float _mat[4][4]{};
};


