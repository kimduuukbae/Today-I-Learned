#include "stdafx.h"
#include "Matrix.h"

Matrix::Matrix(const Matrix& other){

}

Matrix Matrix::operator*(const Matrix& other){
	
#if defined(AVX_INTRINSICS)

#elif defined(SSE_INTRINSICS)

#else

#endif

}

