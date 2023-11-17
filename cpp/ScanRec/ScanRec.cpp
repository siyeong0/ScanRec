#include "ScanRec.h"
#include "MemPool.hpp"

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
		Chunk* ptr = chunkData.Chunk;
		ptr->~Chunk();
		Free(ptr);
	}
	Free(mUVBuffer);
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
	float aspectRatio = mWidth / mHeight;
	mScannerCam.SetAspectRatio(aspectRatio);
	float fovY = 2.f * atanf(mHeight / 2.f / fy); // fovY / 2 == tan(h/2 / fy)
	float fovYDeg = fovY * 180.f / _PI;	// radian to degree
	mScannerCam.SetFovY(fovYDeg);

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
	// Set scanner position and orientation from camera extrinsic
	mScannerCam.SetPosition(currPosition);
	Matrix orientation = camExtrinsic;
	memset(&orientation._41, 0, sizeof(float) * 3);	// Set translation factors to 0
	mScannerCam.SetOrientationFromMatrix(orientation);
	mScannerCam.Update(0.f);

	// Debug info
	auto& camPos = mScannerCam.GetPosition();
	std::cout << "Curr Position : " << camPos.x << ", " << camPos.y << ", " << camPos.z << std::endl;

	// Chunk managing
	std::vector<size_t> visibleChunkIdxs;
	for (size_t i = 0; i < mChunkDatas.size(); i++)
	{
		ChunkData& chunkData = mChunkDatas[i];
		Chunk* chunk = chunkData.Chunk;
		const Vector3& center = chunkData.Center;
		if (mScannerCam.IsVisible(Chunk::GetBoundingBox(center)))
		{
			if (chunk == nullptr)
			{
				chunk->Read(center);
			}
			chunkData.RecentStep = mStepCount;

			visibleChunkIdxs.push_back(i);
		}

		size_t stepOffset = mStepCount - chunkData.RecentStep;
		Vector3 chunkCenter = chunkData.Center;
		float dist = (currPosition - chunkCenter).Length();
		if (stepOffset > 32 && dist > mFarDepth * 2.f)
		{
			chunk->Write(chunkData.Center);
			chunkData.Chunk = nullptr;
		}
	}

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
				auto getChunkCenter = [](float v) -> float { return roundf(v / CHUNK_SIZE) * HALF_CHUNK_SIZE; };
				Vector3 chunkCenter(getChunkCenter(pos.x), getChunkCenter(pos.y), getChunkCenter(pos.z));

				// Debug
				for (auto& chunkData : mChunkDatas)
				{
					Assert(chunkData.Center != chunkCenter);
				}

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

	for (auto& chunkData : mChunkDatas)
	{
		Chunk* chunk = chunkData.Chunk;
		Block** blocks = chunk->GetBlocks();
		for (size_t bidx = 0; bidx < NUM_BLOCKS_IN_CHUNK; bidx++)
		{
			Block* currBlock = blocks[bidx];
			if (currBlock == nullptr)
			{
				continue;
			}

			Fragment** frags = currBlock->GetFrags();
			for (size_t fidx = 0; fidx < NUM_FRAGS_IN_BLOCK; fidx++)
			{
				Fragment* currFrag = frags[fidx];
				if (currFrag == nullptr)
				{
					continue;
				}

				void* pcd = currFrag->GetPcd();
				float* points = Fragment::GetPointPtr(pcd);
				uint8_t* colors = Fragment::GetColorPtr(pcd);
				for (size_t i = 0; i < POINTS_PER_FRAG; i++)
				{
					if (points[i * 3 + 0] == PCD_EMPTY_VAL)
					{
						break;
					}
					out << points[i * 3 + 0] << ", "
						<< points[i * 3 + 1] << ", "
						<< points[i * 3 + 2] << ", "
						<< int(colors[i * 3 + 0]) << ", "
						<< int(colors[i * 3 + 1]) << ", "
						<< int(colors[i * 3 + 2]) << "\n";
					out.flush();
				}
			}
		}
	}

	out.close();
}