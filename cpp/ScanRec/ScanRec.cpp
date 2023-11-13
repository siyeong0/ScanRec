#include "ScanRec.h"

ScanRec::ScanRec(size_t width, size_t height, float depthScale)
	: mWidth(width)
	, mHeight(height)
	, mDepthScale(depthScale)
	, mUVBuffer(nullptr)
{
	mUVBuffer = new Vec2[width * height];
}

ScanRec::~ScanRec()
{
	delete[] mUVBuffer;
}

void ScanRec::SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic)
{
	mCamIntrnsic = camIntrinsic;

	float cx = mCamIntrnsic.mCenterX;
	float cy = mCamIntrnsic.mCenterY;
	float invFx = 1.f / mCamIntrnsic.mFocalLengthX;
	float invFy = 1.f / mCamIntrnsic.mFocalLengthY;
	for (size_t h = 0; h < mHeight; h++)
	{
		for (size_t w = 0; w < mWidth; w++)
		{
			Vec2& uv = mUVBuffer[h * mWidth + w];
			uv.X = (w - cx) * invFx;
			uv.Y = (h - cy) * invFy;
		}
	}
}

void ScanRec::Step(float* camExtrinsic, RGB* rgb, uint16_t* depth)
{
	Vec3 position;
	memcpy(&position, &camExtrinsic[12], sizeof(float) * 3);
	Mat3x3 rotMat;
	memcpy(&(rotMat._00), &camExtrinsic[0], sizeof(float) * 3);
	memcpy(&(rotMat._10), &camExtrinsic[4], sizeof(float) * 3);
	memcpy(&(rotMat._20), &camExtrinsic[8], sizeof(float) * 3);

	PointData* out = new PointData[mWidth * mHeight];

	for (size_t h = 0; h < mHeight; h++)
	{
		for (size_t w = 0; w < mWidth; w++)
		{
			PointData data;
			// point
			float point[3];
			float z = float(depth[h * mWidth + w]);
			point[0] = mUVBuffer[h * mWidth + w].X * z;
			point[1] = mUVBuffer[h * mWidth + w].Y * z;
			point[2] = z;
			memcpy(&data, point, sizeof(float) * 3);
			// color
			memcpy(&(data.R), &rgb[h * mWidth + w], sizeof(uint8_t) * 3);

			// add to chunk

			// debug
			out[h * mWidth + w] = data;
		}
	}

	// write
}