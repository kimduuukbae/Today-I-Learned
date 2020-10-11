#include <iostream>

struct Vector2D {
	float x, y;
	auto length() -> float const {
		return std::sqrtf((x * x) + (y * y));
	}
};

struct Vector3D : Vector2D {
	float z;
	auto length() -> float const {
		return std::sqrtf((x * x) + (y * y) + (z * z));
	}
};

struct FastVector3D {
	int x, y, z;
	auto length() -> int const {
		return 2;
	}
};

template <typename derived, typename base>
concept isBase = std::is_base_of_v< std::remove_reference_t<base>, 
	std::remove_reference_t<derived>>;

template <typename vector>
concept A = 
isBase<vector, Vector2D> && requires (vector& vec) {
	{ vec.length() };
};

template <typename vector>
requires A<vector> && isBase<vector, Vector2D>
constexpr auto normalize(vector& vec) -> float {
	return vec.length();
}

int main() {
	Vector3D v2{ 1.0f, 2.0f, 3.0f };
	std::cout << normalize<Vector3D>(v2) << std::endl;
	FastVector3D v3{ 1, 2, 3 };
	std::cout << normalize<FastVector3D>(v3) << std::endl;	//error
}