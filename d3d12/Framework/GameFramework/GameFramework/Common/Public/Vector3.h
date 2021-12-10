#pragma once

struct Vector3 {
public:
	Vector3() = default;
	Vector3(const Vector3& other) = default;

	Vector3(float x, float y, float z);

	void Normalize();
	bool IsNormalized() const;

	float GetSize() const;

	// static functions
	static float DotProduct(const Vector3& lhs, const Vector3& rhs);
	static Vector3 CrossProduct(const Vector3& lhs, const Vector3& rhs);

	Vector3 operator+(const Vector3& rhs);
	class Basis {
	public:
		static Vector3 GetOriginVector();
		static Vector3 GetUpVector();
		static Vector3 GetForwardVector();
		static Vector3 GetRightVector();
	};

	float _v3[3]{}
};

