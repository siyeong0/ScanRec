#pragma once
#include "Chunk.h"
#include <vector>

class ScanRec
{
public:
	struct CameraInstrinsic
	{
		Vector2 FocalLength;
		Vector2 Center;
	};
public:
	ScanRec(size_t width, size_t height, float depthScale);
	~ScanRec();

	void SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic);
	void Step(Matrix& camExtrinsic, RGB* rgb, uint16_t* depth);

private:
	CameraInstrinsic mCamIntrnsic;
	Chunk* mChunk;

	size_t mWidth;
	size_t mHeight;
	float mDepthScale;

	Vector2* mUVBuffer;
};