#pragma once
#include "Chunk.h"
#include <vector>

class ScanRec
{
public:
	struct CameraInstrinsic
	{
		float mFocalLengthX;
		float mFocalLengthY;
		float mCenterX;
		float mCenterY;
	};
public:
	ScanRec(size_t width, size_t height, float depthScale);
	~ScanRec();

	void SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic);
	void Step(float* camExtrinsic, uint8_t* rgb, uint16_t* depth);

private:
	CameraInstrinsic mCamIntrnsic;
	struct ChunkData
	{
		Chunk Chunk;
		Vec3 Center;
	};
	std::vector<ChunkData> mChunks;

	size_t mWidth;
	size_t mHeight;
	float mDepthScale;
};