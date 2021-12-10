#include "stdafx.h"
#include "Vector3.h"

Vector3::Vector3(float x, float y, float z) {
	_v3[0] = x;
	_v3[1] = y;
	_v3[2] = z;
}

Vector3 Vector3::operator+(const Vector3& rhs){
#if defined(SSE_INTRINSICS)


#elif defined(NO_INTRINSICS)
	_v3[0] += rhs._v3[0];
	_v3[1] += rhs._v3[1];
	_v3[2] += rhs._v3[2];

	return *this;
#endif
	__m128 _simdResult{};
}


void Vector3::Normalize()
{
}

bool Vector3::IsNormalized() const
{
	return false;
}

float Vector3::GetSize() const
{
	return 0.0f;
}

float Vector3::DotProduct(const Vector3& lhs, const Vector3& rhs)
{
	return 0.0f;
}

Vector3 Vector3::CrossProduct(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3();
}



Vector3 Vector3::Basis::GetOriginVector()
{
	return Vector3{ 0.0f, 0.0f, 0.0f };
}

Vector3 Vector3::Basis::GetUpVector()
{
	return Vector3{ 0.0, 1.0f, 0.0f };
}

Vector3 Vector3::Basis::GetForwardVector()
{
	return Vector3{ 0.0f, 0.0f, 1.0f };
}

Vector3 Vector3::Basis::GetRightVector()
{
	return Vector3{ 1.0f, 0.0f, 0.0f };
}
