#include "ScanRec.h"

ScanRec::ScanRec(size_t width, size_t height, float depthScale)
	: mWidth(width)
	, mHeight(height)
	, mDepthScale(depthScale)
	, mUVBuffer(nullptr)
{
	mUVBuffer = new Vector2[width * height];
	mChunk = new Chunk(Vector3(0,0,0));
}

ScanRec::~ScanRec()
{
	delete[] mUVBuffer;
}

void ScanRec::SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic)
{
	mCamIntrnsic = camIntrinsic;

	float cx = mCamIntrnsic.Center.x;
	float cy = mCamIntrnsic.Center.y;
	float invFx = 1.f / mCamIntrnsic.FocalLength.x;
	float invFy = 1.f / mCamIntrnsic.FocalLength.y;
	for (size_t h = 0; h < mHeight; h++)
	{
		for (size_t w = 0; w < mWidth; w++)
		{
			Vector2& uv = mUVBuffer[h * mWidth + w];
			uv.x = (w - cx) * invFx;
			uv.y = (h - cy) * invFy;
		}
	}
}

void ScanRec::Step(Matrix& camExtrinsic, RGB* rgb, uint16_t* depth)
{
	std::ofstream out;
	out.open("pcd.txt", std::ios_base::out);

	for (size_t h = 0; h < mHeight; h++)
	{
		for (size_t w = 0; w < mWidth; w++)
		{
			if (depth[h * mWidth + w] == 0)
			{
				continue;
			}
			PointData data;
			// point
			float point[3];
			float z = float(depth[h * mWidth + w]) / 8191.875f;
			point[0] = mUVBuffer[h * mWidth + w].x * z;
			point[1] = mUVBuffer[h * mWidth + w].y * z;
			point[2] = z;
			memcpy(&data, point, sizeof(float) * 3);
			// color
			memcpy(&(data.R), &rgb[h * mWidth + w], sizeof(uint8_t) * 3);

			mChunk->AddPoint(data, 3);
			// add to chunk
			out << data.X << ", ";
			out << data.Y << ", ";
			out << data.Z << ", ";
			out << int(data.R) << ", ";
			out << int(data.G) << ", ";
			out << int(data.B) << ", ";
			out << std::endl;
			out.flush();
		}
	}
	out.close();
	// write
}