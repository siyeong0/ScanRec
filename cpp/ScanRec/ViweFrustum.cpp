#include "ViewFrustum.h"

ViewFrustum::ViewFrustum()
{

}

ViewFrustum::ViewFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	Initialize(fovAngleY, aspectRatio, nearZ, farZ);
}

ViewFrustum::~ViewFrustum()
{

}

void ViewFrustum::Initialize(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	mBaseFrustum = BoundingFrustum(CreatePerspectiveFovRH(fovAngleY, aspectRatio, nearZ, farZ));
}

void ViewFrustum::Update(const Vector3& position, const Matrix& orientation)
{
	mViewFrustum = mBaseFrustum;
	mViewFrustum.Origin = position;
	mViewFrustum.Orientation = orientation;
}

bool ViewFrustum::Intersects(const BoundingBox& box) const
{
	return mViewFrustum.Intersects(box);
}