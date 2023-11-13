#include "ScanRec.h"

ScanRec::ScanRec(size_t width, size_t height, float depthScale)
	: mWidth(width)
	, mHeight(height)
	, mDepthScale(depthScale)
{

}

ScanRec::~ScanRec()
{

}

void ScanRec::SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic)
{

}

void ScanRec::Step(float* camExtrinsic, uint8_t* rgb, uint16_t* depth)
{
	Vec3 position;
	memcpy(&position, &camExtrinsic[12], sizeof(float) * 3);
	Mat3x3 rotMat;
	memcpy(&(rotMat._00), &camExtrinsic[0], sizeof(float) * 3);
	memcpy(&(rotMat._10), &camExtrinsic[4], sizeof(float) * 3);
	memcpy(&(rotMat._20), &camExtrinsic[8], sizeof(float) * 3);
}