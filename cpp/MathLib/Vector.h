#pragma once
#include "dllAPI.h"
#include <directxtk/SimpleMath.h>

using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;

//class MATHLIB_API Vector3
//{
//public:
//	Vector3() = default;
//	~Vector3() = default;
//
//	Vector3(float _x, float _y, float _z)
//		: x(_x), y(_y), z(_z)
//	{ }
//
//	inline Vector3 operator-()
//	{
//		return Vector3(-x, -y, -z);
//	}
//
//	inline Vector3 operator+(const Vector3& vec)
//	{
//		return Vector3(x + vec.x, y + vec.y, z + vec.z);
//	}
//	inline Vector3 operator-(const Vector3& vec)
//	{
//		return Vector3(x - vec.x, y - vec.y, z - vec.z);
//	}
//	inline Vector3 operator*(const Vector3& vec)
//	{
//		return Vector3(x * vec.x, y * vec.y, z * vec.z);
//	}
//	inline Vector3 operator/(const Vector3& vec)
//	{
//		return Vector3(x / vec.x, y / vec.y, z / vec.z);
//	}
//public:
//	float x;
//	float y;
//	float z;
//};
//
//inline Vector3 operator*(float v, const Vector3& vec)
//{
//	return Vector3(v * vec.x, v * vec.y, v * vec.z);
//}
//inline Vector3 operator*(const Vector3& vec, float v)
//{
//	return Vector3(v * vec.x, v * vec.y, v * vec.z);
//}
//inline Vector3 operator/(float v, const Vector3& vec)
//{
//	return Vector3(v / vec.x, v / vec.y, v / vec.z);
//}
//inline Vector3 operator/(const Vector3& vec, float v)
//{
//	return Vector3(v / vec.x, v / vec.y, v / vec.z);
//}
//
//class MATHLIB_API Vector4
//{
//public:
//	Vector4() = default;
//	~Vector4() = default;
//
//	Vector4(float _x, float _y, float _z, float _w)
//		: x(_x), y(_y), z(_z), w(_w)
//	{ }
//
//	inline Vector4 operator-()
//	{
//		return Vector4(-x, -y, -z, -w);
//	}
//
//	inline Vector4 operator+(const Vector4& vec)
//	{
//		return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
//	}
//	inline Vector4 operator-(const Vector4& vec)
//	{
//		return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
//	}
//	inline Vector4 operator*(const Vector4& vec)
//	{
//		return Vector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
//	}
//	inline Vector4 operator/(const Vector4& vec)
//	{
//		return Vector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
//	}
//public:
//	float x;
//	float y;
//	float z;
//	float w;
//};
//
//inline Vector4 operator*(float v, const Vector4& vec)
//{
//	return Vector4(v * vec.x, v * vec.y, v * vec.z, v * vec.w);
//}
//inline Vector4 operator*(const Vector4& vec, float v)
//{
//	return Vector4(v * vec.x, v * vec.y, v * vec.z, v * vec.w);
//}
//inline Vector4 operator/(float v, const Vector4& vec)
//{
//	return Vector4(v / vec.x, v / vec.y, v / vec.z, v / vec.w);
//}
//inline Vector4 operator/(const Vector4& vec, float v)
//{
//	return Vector4(v / vec.x, v / vec.y, v / vec.z, v / vec.w);
//}
