#include <iostream>
#include "Chunk.h"
#include <random>
#include <opencv2/opencv.hpp>
#include <DirectXMath.h>
#include "MathLib.h"
#include "ScanRec.h"
#include <io.h>
#include "MemPool.hpp"

void TestPcdGeneration();
void TestMemPool();

int main(void)
{
	auto q = Quaternion::CreateFromYawPitchRoll(0.4f, 1.3f, 0.2f);
	auto vec = q.ToEuler();
	TestPcdGeneration();
	// TestMemPool();
	return 0;
}

void TestPcdGeneration()
{
	std::string basePath = "../../resources/2023-11-09-17-09-09/";
	ScanRec scanRec(256, 144, 8);
	Matrix prevExtrinsic;
	size_t count = 0;
	for (int i = 1014; i < 1700; i += 1)
	{
		std::string imagePath = basePath + "image/" + std::to_string(i) + ".jpg";
		std::string depthPath = basePath + "depth/" + std::to_string(i) + ".png";
		std::string cameraPath = basePath + "camera/" + std::to_string(i) + ".bin";
		if (_access(imagePath.c_str(), 0) == -1 || _access(depthPath.c_str(), 0) == -1 || _access(cameraPath.c_str(), 0) == -1)
		{
			continue;
		}
		// read rgb
		cv::Mat img = cv::imread(imagePath);
		cv::imshow("test", img);
		cv::waitKey(1);
		cv::Mat rgb;
		cv::resize(img, rgb, cv::Size(256, 144));
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
		// read depth
		auto depth = cv::imread(depthPath, 2);
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
		// read camera info
		std::ifstream camFile;
		camFile.open(cameraPath, std::ios_base::in | std::ios_base::binary);
		float camIntrinsic[12] = { 0 };
		camFile.read(reinterpret_cast<char*>(camIntrinsic), 12 * sizeof(float));
		Matrix camExtrinsic;
		camFile.read(reinterpret_cast<char*>(&camExtrinsic), 16 * sizeof(float));

		// calculate distance between prev extrinsic and curr extrinsic
		Vector3 prevTranslation(reinterpret_cast<float*>(&prevExtrinsic._41));
		Vector3 currTranslation(reinterpret_cast<float*>(&camExtrinsic._41));
		Matrix prevRotMat = prevExtrinsic;
		memset(&prevRotMat._41, 0, sizeof(float) * 3);
		Matrix currRotMat = camExtrinsic;
		memset(&currRotMat._41, 0, sizeof(float) * 3);

		float transDist = (currTranslation - prevTranslation).Length();
		float rotDist = (currRotMat * prevRotMat.Transpose()).ToEuler().Length();
		if (transDist + rotDist < 0.6)
		{
			continue;
		}
		std::cout << "=======================" << std::endl;
		std::cout << count << " : frame number" << i << std::endl;
		std::cout << "T : " << transDist << std::endl;
		std::cout << "R : " << rotDist << std::endl;
		std::cout << "=======================" << std::endl;

		ScanRec::CameraInstrinsic camIn;
		camIn.FocalLength.x = camIntrinsic[0] / 5.f;
		camIn.FocalLength.y = camIntrinsic[5] / 5.f;
		camIn.Center.x = camIntrinsic[8] / 5.f;
		camIn.Center.y = camIntrinsic[9] / 5.f;
		scanRec.SetCameraIntrinsics(camIn);

		scanRec.Step(camExtrinsic, rgbData, depthData);

		prevExtrinsic = camExtrinsic;
		count++;
	}
	scanRec.Save("pcd.txt");
}

void TestMemPool()
{
	struct Foo
	{
		float x;
		float y;

		uint16_t a;
		uint8_t b;
		uint8_t c;
		uint8_t d;
	};
	MemPool<sizeof(Foo)> mp;
	std::vector<Foo*> foos;
	for (size_t i = 0; i < 25092; i++)
	{
		foos.push_back(reinterpret_cast<Foo*>(mp.Alloc()));
	}
	
	auto getAllocCount = [&]() -> size_t
	{
		size_t allocCount = 0;
		for (auto& bucket : mp.mBuckets)
		{
			allocCount += bucket.Idx;
		}
		return allocCount;
	};

	mp.Free(foos[12]);
	auto t = mp.Alloc();
	mp.Free(foos[177]);
	mp.Free(foos[1311]);
	auto t1 = mp.Alloc();
	auto t2 = mp.Alloc();
	auto t3 = mp.Alloc();
	mp.Free(foos[11]);
	mp.Free(foos[23333]);
	mp.Free(foos[133]);
	mp.Free(foos[3131]);
	mp.Free(foos[4]);
	mp.Free(foos[1111]);
	auto t4 = mp.Alloc();
	auto t5 = mp.Alloc();
	auto t6 = mp.Alloc();
	auto t7 = mp.Alloc();
	auto t8 = mp.Alloc();
	auto t9 = mp.Alloc();

	return;
}