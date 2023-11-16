#pragma once
#include "MathLib.h"

class Camera
{
public:
	enum class eDirection { Forward, Backward, Left, Right, Up, Down };

public:
	Camera();
	Camera(Vector3 pos, Vector3 up, float yaw, float pitch);
	~Camera();

	void Update(float dt);
	bool IsVisible(const BoundingBox& boundingBox);
	void SetOrientationFromQuat(const Quaternion& quat);
	void SetOrientationFromMatrix(const Matrix& mat);

	Matrix ProjMatrix() const;
	Matrix ViewMatrix() const;
	const Vector3& GetPosition() const;
	float GetYaw() const;
	float GetPitch() const;
	float GetSpeed() const;
	float GetMouseSensitivity() const;
	float GetFovY() const;
	float GetAspectRatio() const;
	float GetNearZ() const;
	float GetFarZ() const;

	void SetPosition(Vector3& position);
	void SetPosition(Vector3&& position);
	void SetSpeed(float value);
	void SetMouseSensitivity(float value);
	void SetFovY(float value);
	void SetAspectRatio(float value);
	void SetNearZ(float value);
	void SetFarZ(float value);

	void Move(eDirection dir, float delta);
	void OnMouseMove(float mouseNdcX, float mouseNdcY, bool constrainPitch = true);
	void OnMouseScroll(float yOffset);

private:
	void updateDirections();

private:
	const Vector3 mWorldUp = Vector3(0, 1, 0);

	Vector3 mPosition;
	Vector3 mFrontDir;
	Vector3 mUpDir;
	Vector3 mRightDir;

	float mYaw = 0.f;		// Rad
	float mPitch = 0.f;		// Rad

	float mSpeed = 5.f;
	float mMouseSensitivity = 0.5f;
	float mFovY = 45.f;		// Degree
	float mAspectRatio = 16.f / 9.f;
	float mNearZ = 0.01f;
	float mFarZ = 1000.f;

	// Frustum Culling
	BoundingFrustum mViewFrustum;
};