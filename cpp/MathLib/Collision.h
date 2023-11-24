#pragma once
#include <Eigen/Dense>
#include "Vector.h"
#include "Matrix.h"

struct BoundingBox
{
	Vector3 Center;            // Center of the box.
	Vector3 Extents;           // Distance from the center to each side.

	BoundingBox() : Center(0, 0, 0), Extents(1.f, 1.f, 1.f) {}
	BoundingBox(const Vector3& center, const Vector3& extents) : Center(center), Extents(extents) {}

	BoundingBox(const BoundingBox&) = default;
	BoundingBox& operator=(const BoundingBox&) = default;
	BoundingBox(BoundingBox&&) = default;
	BoundingBox& operator=(BoundingBox&&) = default;
};

struct BoundingFrustum
{
	static const size_t CORNER_COUNT = 8;

	Vector3 Origin;            // Origin of the frustum (and projection)
	Matrix Orientation;       // Rotation matrix.

	float RightSlope;           // Positive X (X/Z)
	float LeftSlope;            // Negative X
	float TopSlope;             // Positive Y (Y/Z)
	float BottomSlope;          // Negative Y
	float Near, Far;            // Z of the near plane and far plane

	BoundingFrustum() :
		Origin(0, 0, 0), Orientation(Matrix::Identity()), RightSlope(1.f), LeftSlope(-1.f),
		TopSlope(1.f), BottomSlope(-1.f), Near(0), Far(1.f) {}
	BoundingFrustum(const Matrix& projection) { CreateFromProjection(projection); }

	BoundingFrustum(const BoundingFrustum&) = default;
	BoundingFrustum& operator=(const BoundingFrustum&) = default;
	BoundingFrustum(BoundingFrustum&&) = default;
	BoundingFrustum& operator=(BoundingFrustum&&) = default;

	void CreateFromProjection(const Matrix& projection)

	{
		// Corners of the projection frustum in homogenous space
		static Vector4 homogenousPoints[6] =
		{
			Vector4{  1.0f,  0.0f, 1.0f, 1.0f },   // right (at far plane)
			Vector4{ -1.0f,  0.0f, 1.0f, 1.0f },   // left
			Vector4{  0.0f,  1.0f, 1.0f, 1.0f },   // top
			Vector4{  0.0f, -1.0f, 1.0f, 1.0f },   // bottom

			Vector4{ 0.0f, 0.0f, 0.0f, 1.0f },     // near
			Vector4{ 0.0f, 0.0f, 1.0f, 1.0f }      // far
		};

		Matrix matInverse = projection.inverse();

		// Compute the frustum corners in world space
		Vector4 points[6];

		for (size_t i = 0; i < 6; ++i)
		{
			// Transform point.
			points[i] = matInverse * homogenousPoints[i];
		}

		Origin = Vector3(0.0f, 0.0f, 0.0f);
		Orientation = Matrix::Identity();

		// Compute the slopes
		points[0] = points[0].cwiseProduct(Vector4(1.f / points[0][2], 1.f / points[0][2], 1.f / points[0][2], 1.f / points[0][2]));
		points[1] = points[1].cwiseProduct(Vector4(1.f / points[1][2], 1.f / points[1][2], 1.f / points[1][2], 1.f / points[1][2]));
		points[2] = points[2].cwiseProduct(Vector4(1.f / points[2][2], 1.f / points[2][2], 1.f / points[2][2], 1.f / points[2][2]));
		points[3] = points[3].cwiseProduct(Vector4(1.f / points[3][2], 1.f / points[3][2], 1.f / points[3][2], 1.f / points[3][2]));

		RightSlope = points[0][0];
		LeftSlope = points[1][0];
		TopSlope = points[2][1];
		BottomSlope = points[3][1];

		// Compute near and far.
		points[4] = points[4].cwiseProduct(Vector4(1.f / points[4][3], 1.f / points[4][3], 1.f / points[4][3], 1.f / points[4][3]));
		points[5] = points[5].cwiseProduct(Vector4(1.f / points[5][3], 1.f / points[5][3], 1.f / points[5][3], 1.f / points[5][3]));

		// Left hand
		//Near = points[4][2];
		//Far = points[5][2];
		// Right hand
		Far = points[4][2];
		Near = points[5][2];
	}


	bool Intersects(const BoundingBox& box) const
	{
		// Build the frustum planes
		Vector4 planes[6];
		planes[0] = Vector4(0.0f, 0.0f, -1.0f, Near);
		planes[1] = Vector4(0.0f, 0.0f, 1.0f, -Far);
		planes[2] = Vector4(1.0f, 0.0f, -RightSlope, 0.0f);
		planes[3] = Vector4(-1.0f, 0.0f, LeftSlope, 0.0f);
		planes[4] = Vector4(0.0f, 1.0f, -TopSlope, 0.0f);
		planes[5] = Vector4(0.0f, -1.0f, BottomSlope, 0.0f);

		Vector3 points[8] =
		{
			Vector3{LeftSlope, TopSlope, Near},
			Vector3{RightSlope, TopSlope, Near},
			Vector3{LeftSlope, BottomSlope, Near},
			Vector3{RightSlope, BottomSlope, Near},
			Vector3{LeftSlope, TopSlope, Far},
			Vector3{RightSlope, TopSlope, Far},
			Vector3{LeftSlope, BottomSlope, Far},
			Vector3{RightSlope, BottomSlope, Far}
		};
		// Load the box
		Matrix3 rot;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				rot.coeffRef(i, j) = Orientation.coeffRef(i, j);
			}
		}
		const Vector3& center = rot.transpose() * (box.Center - Origin);
		const Vector3& extents = box.Extents;
		Vector3 boxMin = center - extents;
		Vector3 boxMax = center + extents;

		size_t outCount;
		// Check box outside/inside of frustum
		for (size_t i = 0; i < 6; ++i)
		{
			outCount = 0;
			outCount += size_t(planes[i].dot(Vector4(boxMin[0], boxMin[1], boxMin[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMax[0], boxMin[1], boxMin[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMin[0], boxMax[1], boxMin[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMax[0], boxMax[1], boxMin[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMin[0], boxMin[1], boxMax[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMax[0], boxMin[1], boxMax[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMin[0], boxMax[1], boxMax[2], 1.0f)) < 0.0f);
			outCount += size_t(planes[i].dot(Vector4(boxMax[0], boxMax[1], boxMax[2], 1.0f)) < 0.0f);
			if (outCount != 8)
			{
				return true;
			}
		}

		// Check frustum outside/inside box
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][0] > boxMax[0]);
		}
		if (outCount != 8) return true;
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][0] > boxMin[0]);
		}
		if (outCount != 8) return true;
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][1] > boxMax[1]);
		}
		if (outCount != 8) return true;
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][1] > boxMin[1]);
		}
		if (outCount != 8) return true;
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][2] > boxMax[2]);
		}
		if (outCount != 8) return true;
		outCount = 0;
		for (int i = 0; i < 8; i++)
		{
			outCount += size_t(points[i][2] > boxMin[2]);
		}

		return false;
	}
};