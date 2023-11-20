#include "ScanRec.h"
#include "MemPool.hpp"
#include <opencv2/opencv.hpp>
#include <io.h>

ScanRec::ScanRec(size_t width, size_t height, float farDepth)
	: mStepCount(0)
	, mWidth(width)
	, mHeight(height)
	, mFarDepth(farDepth)
	, mChunkDatas()
	, mSize(0)
	, mUVBuffer(nullptr)
{
	ChunkData baseChunkData;
	baseChunkData.Chunk = (Chunk*)Alloc(sizeof(Chunk));
	new (baseChunkData.Chunk) Chunk();
	baseChunkData.Center = Vector3(0, 0, 0);
	baseChunkData.RecentStep = mStepCount;
	mChunkDatas.push_back(baseChunkData);

	mUVBuffer = (Vector2*)Alloc(sizeof(Vector2) * width * height);
}

ScanRec::~ScanRec()
{
	std::cout << "Number of Chunks : " << mChunkDatas.size() << std::endl;
	for (auto& chunkData : mChunkDatas)
	{
		if (chunkData.Chunk == nullptr)
		{
			continue;
		}
		chunkData.Chunk->~Chunk();
		Free(chunkData.Chunk);
		chunkData.Chunk = nullptr;
	}
	Free(mUVBuffer);
	mUVBuffer = nullptr;
}

void ScanRec::SetCameraIntrinsics(const CameraInstrinsic& camIntrinsic)
{
	mCamIntrnsic = camIntrinsic;

	float cx = mCamIntrnsic.Center.x;
	float cy = mCamIntrnsic.Center.y;
	float fx = mCamIntrnsic.FocalLength.x;
	float fy = mCamIntrnsic.FocalLength.y;
	float invFx = 1.f / fx;
	float invFy = 1.f / fy;

	// Create view frustum
	float aspectRatio = float(mWidth) / mHeight;
	float fovY = 2.f * atanf(mHeight / 2.f / fy); // fovY / 2 == tan(h/2 / fy)
	mScannerFrustum.Initialize(fovY, aspectRatio, 0.01f, mFarDepth);

	// Fill UV buffer
	// world coord x = (w - cx) * depth / focalLength
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
	Vector3 currPosition = camExtrinsic.Translation();
	Matrix orientation = camExtrinsic;
	memset(&orientation._41, 0, sizeof(float) * 3);	// Set translation factors to 0
	// Set scanner position and orientation from camera extrinsic
	mScannerFrustum.Update(currPosition, orientation);

	// Chunk managing
	std::vector<size_t> visibleChunkIdxs;
	for (size_t i = 0; i < mChunkDatas.size(); i++)
	{
		ChunkData& chunkData = mChunkDatas[i];
		Chunk* chunk = chunkData.Chunk;
		// const Vector3& center = chunkData.Center;
		Vector3 center = chunkData.Center;

		size_t stepOffset = mStepCount - chunkData.RecentStep;
		float dist = (Chunk::GetCenter(currPosition) - center).Length();
		if (int(stepOffset) > 4 && dist > mFarDepth * 2.0f)
		{
			if (chunk != nullptr)
			{
				std::ofstream out;
				std::string path = CHUNK_CACHE_PATH;
				out.open(path + centerToString(center), std::ios_base::out);
				Assert(out.is_open());

				Chunk::Write(chunk, out);
				(chunk)->~Chunk();
				chunkData.Chunk = nullptr;

				out.close();
			}
		}

		if (mScannerFrustum.Intersects(Chunk::GetBoundingBox(center)))
		{
			if (chunkData.Chunk == nullptr)
			{
				std::ifstream in;
				std::string path = CHUNK_CACHE_PATH;
				in.open(path + centerToString(center), std::ios_base::in);
				Assert(in.is_open());

				chunkData.Chunk = (Chunk*)Alloc(sizeof(Chunk));
				new (chunkData.Chunk) Chunk();
				Chunk::Read(chunkData.Chunk, center, in);
				
				in.close();
			}
			chunkData.RecentStep = mStepCount;

			visibleChunkIdxs.push_back(i);
		}
	}
	// Visualize
	cv::Mat mat(512, 512, CV_8UC3, cv::Scalar(255,255,255));
	auto cvt = [](float x) -> int { return int((x + CHUNK_SIZE * 2.5) / (CHUNK_SIZE * 5) * 512.f); };
	cv::circle(mat, cv::Point(cvt(currPosition.x), cvt(currPosition.z)),
		10, cv::Scalar(255, 0, 0), 5, -1);
	Vector3 dir = Vector3::Transform(Vector3(0, 0, -1), orientation);
	Vector2 front(dir.x, dir.z);
	front.Normalize();
	cv::line(mat, 
		cv::Point(cvt(currPosition.x), cvt(currPosition.z)),
		cv::Point(cvt(currPosition.x + front.x * 5), cvt(currPosition.z + front.y * 5)),
		cv::Scalar(255,0,0), 2);

	for (size_t i = 0; i < mChunkDatas.size(); i++)
	{
		ChunkData& chunkData = mChunkDatas[i];
		Chunk* chunk = chunkData.Chunk;

		Vector3 c = chunkData.Center;
		cv::rectangle(mat, 
			cv::Rect(
				cv::Point(cvt(c.x - HALF_CHUNK_SIZE), cvt(c.z - HALF_CHUNK_SIZE)),
				cv::Point(cvt(c.x + HALF_CHUNK_SIZE), cvt(c.z + HALF_CHUNK_SIZE))),
			chunk != nullptr ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 0), 2, 8);
	}
	cv::imshow("x", mat);
	cv::waitKey(1);

	// Generate point cloud data
	float depthScale = 1.f / ((powf(2.f, 16.f) - 1) / mFarDepth);
	for (size_t h = 0; h < mHeight; h++)
	{
		for (size_t w = 0; w < mWidth; w++)
		{
			if (depth[h * mWidth + w] == 0)
			{
				continue;
			}
			PointData data;
			const Vector3& pos = data.Position;
			// point
			Vector4 hgPoint;
			float z = float(depth[h * mWidth + w]) * depthScale;
			hgPoint.x = mUVBuffer[h * mWidth + w].x * z;
			hgPoint.y = -mUVBuffer[h * mWidth + w].y * z;
			hgPoint.z = -z;
			hgPoint.w = 1.f;
			hgPoint = Vector4::Transform(hgPoint, camExtrinsic);
			memcpy(&data, &hgPoint, sizeof(float) * 3);
			// color
			memcpy(&(data.Color), &rgb[h * mWidth + w], sizeof(uint8_t) * 3);
			
			bool bChunkExist = false;
			for (auto& chunkIdx : visibleChunkIdxs)
			{
				ChunkData& chunkData = mChunkDatas[chunkIdx];
				Chunk* chunk = chunkData.Chunk;
				const Vector3& center = chunkData.Center;
				if (Chunk::Include(center, Vector3(pos.x, pos.y, pos.z)))
				{
					chunk->AddPoint(center, data, 3);
					bChunkExist = true;
					break;
				}
			}
			if (!bChunkExist)
			{
				Vector3 chunkCenter = Chunk::GetCenter(pos);

				// Debug
#ifdef DEBUG
				for (auto& chunkData : mChunkDatas)
				{
					Assert(chunkData.Center != chunkCenter);
				}
#endif

				ChunkData newChunkData;
				newChunkData.Chunk = (Chunk*)Alloc(sizeof(Chunk));
				new (newChunkData.Chunk) Chunk();
				newChunkData.Center = chunkCenter;
				newChunkData.Chunk->AddPoint(chunkCenter, data, 3);
				newChunkData.RecentStep = mStepCount;
				mChunkDatas.push_back(newChunkData);

				visibleChunkIdxs.push_back(mChunkDatas.size() - 1);
			}
		}
	}
	mStepCount++;
}

void ScanRec::Save(std::string filename)
{
	std::ofstream out;
	out.open(std::string("../../") + filename, std::ios_base::out);
	Assert(out.is_open());

	for (auto& chunkData : mChunkDatas)
	{
		if (chunkData.Chunk == nullptr)
		{
			std::ifstream in;
			std::string path = CHUNK_CACHE_PATH;
			in.open(path + centerToString(chunkData.Center), std::ios_base::in);
			Assert(in.is_open());

			chunkData.Chunk = (Chunk*)Alloc(sizeof(Chunk));
			new (chunkData.Chunk) Chunk();
			Chunk::Read(chunkData.Chunk, chunkData.Center, in);

			in.close();
		}
		const Chunk* chunk = chunkData.Chunk;
		const Vector3& center = chunkData.Center;
		Chunk::Write(chunk, out);
	}

	out.close();
}

void ScanRec::Load(std::string filename)
{
	std::ifstream in;
	in.open(std::string("../../") + filename, std::ios_base::out);
	Assert(in.is_open());
}