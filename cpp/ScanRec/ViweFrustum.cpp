#include "ViewFrustum.h"
#include <DirectXMath.h>

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
	// BoundingFrustum::CreateFromMatrix(mBaseFrustum, DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearZ, farZ), true);
	BoundingFrustum::CreateFromMatrix(mBaseFrustum, DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearZ, farZ));
	auto temp = mBaseFrustum.Near;
	mBaseFrustum.Near = mBaseFrustum.Far;
	mBaseFrustum.Far = temp;
}

void ViewFrustum::Update(const Vector3& position, const Matrix& orientation)
{
	mViewFrustum = mBaseFrustum;
	mViewFrustum.Origin = position;
	mViewFrustum.Orientation = Quaternion::CreateFromRotationMatrix(orientation);
}

void ViewFrustum::Update(const Vector3& position, const Quaternion& orientation)
{
	mViewFrustum = mBaseFrustum;
	mViewFrustum.Origin = position;
	mViewFrustum.Orientation = orientation;
}

bool ViewFrustum::Intersects(const BoundingBox& box)
{
	return mViewFrustum.Intersects(box);
}