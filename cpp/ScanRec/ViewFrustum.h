#pragma once
#include "MathLib.h"

class ViewFrustum
{
public:
	ViewFrustum();
	ViewFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ);
	~ViewFrustum();

	void Initialize(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	void Update(const Vector3& position, const Matrix& orientation);
	void Update(const Vector3& position, const Quaternion& orientation);
	bool Intersects(const BoundingBox& box) const;

private:
	BoundingFrustum mBaseFrustum;
	BoundingFrustum mViewFrustum;
};

