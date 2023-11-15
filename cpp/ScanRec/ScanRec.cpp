#include "ScanRec.h"

ScanRec::ScanRec(size_t width, size_t height, float depthScale)
	: mWidth(width)
	, mHeight(height)
	, mDepthScale(depthScale)
	, mChunks()
	, mSize(0)
	, mUVBuffer(nullptr)
{
	mChunks.push_back(new Chunk(Vector3(0,0,0)));
	mUVBuffer = new Vector2[width * height];
}

ScanRec::~ScanRec()
{
	for (auto& ptr : mChunks)
	{
		delete ptr;
	}
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
			Vector4 hgPoint;
			float z = float(depth[h * mWidth + w]) / 8191.875f;
			hgPoint.x = mUVBuffer[h * mWidth + w].x * z;
			hgPoint.y = -mUVBuffer[h * mWidth + w].y * z;
			hgPoint.z = -z;
			hgPoint.w = 1.f;
			hgPoint = Vector4::Transform(hgPoint, camExtrinsic);
			memcpy(&data, &hgPoint, sizeof(float) * 3);
			// color
			memcpy(&(data.R), &rgb[h * mWidth + w], sizeof(uint8_t) * 3);
			
			bool bChunkExist = false;
			for (auto& chunk : mChunks)
			{
				if (chunk->Include(Vector3(data.X, data.Y, data.Z)))
				{
					chunk->AddPoint(data, 3);
					bChunkExist = true;
					break;
				}
			}
			if (!bChunkExist)
			{
				auto getChunkCenter = [](float v) -> float { return roundf(v / ChunkSize) * HalfChunkkSize; };
				Vector3 chunkCenter(getChunkCenter(data.X), getChunkCenter(data.Y), getChunkCenter(data.Z));
				Chunk* newChunk = new Chunk(chunkCenter);
				newChunk->AddPoint(data, 3);
				mChunks.push_back(newChunk);
			}
		}
	}
}

void ScanRec::Save(std::string filename)
{
	std::ofstream out;
	out.open(std::string("../../") + filename, std::ios_base::out);

	for (auto& chunk : mChunks)
	{
		Block** blocks = chunk->GetBlocks();
		for (size_t bidx = 0; bidx < NumBlocksInChunk; bidx++)
		{
			Block* currBlock = blocks[bidx];
			if (currBlock == nullptr)
			{
				continue;
			}

			Fragment** frags = currBlock->GetFrags();
			for (size_t fidx = 0; fidx < NumFragsInBlock; fidx++)
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