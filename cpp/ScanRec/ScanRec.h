#pragma once
#include "Chunk.h"
#include <vector>
#include "Camera.h"

class ScanRec
{
public:
	struct CameraInstrinsic
	{
		Vector2 FocalLength;
		Vector2 Center;
	};
public:
	ScanRec(size_t width, size_t height, float farDepth);
	~ScanRec();

	void SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic);
	void Step(Matrix& camExtrinsic, RGB* rgb, uint16_t* depth);

	void Save(std::string filename);
private:
	CameraInstrinsic mCamIntrnsic;
	std::vector<Chunk*> mChunks;
	size_t mSize;

	size_t mWidth;
	size_t mHeight;
	float mFarDepth;

	Vector2* mUVBuffer;

	Camera mScannerCam;
};