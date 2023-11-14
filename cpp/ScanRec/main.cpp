#include <iostream>
#include "Chunk.h"
#include <random>
#include <opencv2/opencv.hpp>
#include <DirectXMath.h>
#include "MathLib.h"
#include "ScanRec.h"

int main(void)
{
	auto rgb = cv::imread("../0.jpg");
	uchar* rawRgb = rgb.ptr();
	RGB* rgbData = new RGB[256 * 144];
	for (int h = 0; h < 144; h++)
	{
		for (int w = 0; w < 256; w++)
		{
			int idx = (h * 256 + w) * 3;
			rgbData[h * 256 + w].B = static_cast<uint8_t>(rawRgb[idx + 0]);
			rgbData[h * 256 + w].G = static_cast<uint8_t>(rawRgb[idx + 1]);
			rgbData[h * 256 + w].R = static_cast<uint8_t>(rawRgb[idx + 2]);
		}
	}
	auto depth = cv::imread("../0.png", 2);
	uchar* rawDepth = depth.ptr();
	uint16_t* depthData = new uint16_t[256 * 144];
	for (int h = 0; h < 144; h++)
	{
		for (int w = 0; w < 256; w++)
		{
			int idx = (h * 256 + w) * 2;
			depthData[h * 256 + w] = *reinterpret_cast<uint16_t*>(&rawDepth[idx]);
		}
	}

	Vector3 center(8.0f, 0.0f, 0.0f);
	ScanRec scanRec(256,144,8);
	ScanRec::CameraInstrinsic camIn;
	camIn.FocalLength.x = 128;
	camIn.FocalLength.y = 72;
	camIn.Center.x = 128;
	camIn.Center.y = 72;
	scanRec.SetCameraIntrinsics(camIn);

	Matrix identity;
	scanRec.Step(identity, rgbData, depthData);
	return 0;
}

