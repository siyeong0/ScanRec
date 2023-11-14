#pragma once
#include "dllAPI.h"

#include "Constant.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Collision.h"
#include "Plane.h"

constexpr float _PI = 3.141592653589f;
constexpr float _2PI = 2.f * _PI;
constexpr float _EULER = 2.7182818284f;

extern "C" MATHLIB_API bool ComputeTangentFrame(
	const uint32_t * indices, size_t numFacees,
	Vector3 * positions, Vector3 * normals, Vector2 * texcoords,
	size_t numVertices, Vector3 * tangents, Vector3 * biTangents);