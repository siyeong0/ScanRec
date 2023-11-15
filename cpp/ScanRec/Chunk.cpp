#include "Chunk.h"

Chunk::Chunk(const Vector3& center)
	: mBlocks(nullptr)
	, mCenter(center)
{
	mBlocks = new Block*[NumBlocksInChunk];
	memset(mBlocks, 0, sizeof(Block*) * NumBlocksInChunk);
}

Chunk::~Chunk()
{
	for (size_t i = 0; i < NumBlocksInChunk; i++)
	{
		Block** block = &mBlocks[i];
		if (*block != nullptr)
		{
			delete* block;
			*block = nullptr;
		}
	}
	delete[] mBlocks;
}

void Chunk::AddPoint(PointData& data, uint8_t label)
{
	float cx = mCenter.x;
	float cy = mCenter.y;
	float cz = mCenter.z;
	float x = data.X;
	float y = data.Y;
	float z = data.Z;
	Assert(fabs(cx - x) <= HalfChunkkSize + 1e-4f && fabs(cy - y) <= HalfChunkkSize + 1e-4f && fabs(cz - z) <= HalfChunkkSize + 1e-4f);

	size_t idxX = size_t((x - cx + HalfChunkkSize) / BlockSize);
	size_t idxY = size_t((y - cy + HalfChunkkSize) / BlockSize);
	size_t idxZ = size_t((z - cz + HalfChunkkSize) / BlockSize);

	Vector3 blockCenter;
	memcpy(&blockCenter, &mCenter, sizeof(float) * 3);
	size_t indices[3] = { idxX, idxY, idxZ };
	centerFromIdx(&blockCenter, indices, NumBlocksInSide, BlockSize);

	size_t size = size_t(NumBlocksInSide);
	Block** block = &mBlocks[idxX * size * size + idxY * size + idxZ];
	if (*block == nullptr)
	{
		*block = new Block;
	}
	(*block)->AddPoint(blockCenter, data, label);
}

const Vector3& Chunk::GetCenter()
{
	return mCenter;
}

bool Chunk::Include(const Vector3& point)
{
	float dx = fabs(mCenter.x - point.x);
	float dy = fabs(mCenter.y - point.y);
	float dz = fabs(mCenter.z - point.z);

	return (dx <= HalfChunkkSize && dy <= HalfChunkkSize && dz <= HalfChunkkSize);
}

void Chunk::Write(const Vector3& center)
{
	std::string ChunkPath = CHUNK_CACHE_PATH;
	ChunkPath += centerToString(center);

	std::ofstream fout;
	fout.open(ChunkPath, std::ios::out | std::ios::binary);
	// write fragments
	size_t size = size_t(NumFragsInSide);
	for (size_t x = 0; x < size; x++)
	{
		for (size_t y = 0; y < size; y++)
		{
			for (size_t z = 0; z < size; z++)
			{
				Block** block = &mBlocks[x * size * size + y * size + z];
				if (*block != nullptr)
				{
					fout.write(reinterpret_cast<const char*>(&x), sizeof(size_t));
					fout.write(reinterpret_cast<const char*>(&y), sizeof(size_t));
					fout.write(reinterpret_cast<const char*>(&z), sizeof(size_t));

					Vector3 blockCenter;
					memcpy(&blockCenter, &center, sizeof(float) * 3);
					size_t indices[3] = { x, y, z };
					centerFromIdx(&blockCenter, indices, NumBlocksInSide, BlockSize);
					(*block)->Write(blockCenter);

					delete *block;
					*block = nullptr;
				}
			}
		}
	}

	fout.close();
}

void Chunk::Read(const Vector3& center)
{
	std::string ChunkPath = CHUNK_CACHE_PATH;
	ChunkPath += centerToString(center);

	std::ifstream fin;
	fin.open(ChunkPath, std::ios::in | std::ios::binary);
	// read fragments
	size_t size = size_t(NumFragsInSide);
	while (!fin.eof())
	{
		size_t indices[3];
		fin.read(reinterpret_cast<char*>(indices), sizeof(size_t) * 3);
		size_t x = indices[0];
		size_t y = indices[1];
		size_t z = indices[2];

		Block** block = &mBlocks[x * size * size + y * size + z];
		*block = new Block;

		Vector3 blockCenter;
		memcpy(&blockCenter, &center, sizeof(float) * 3);
		centerFromIdx(&blockCenter, indices, NumBlocksInSide, BlockSize);
		(*block)->Read(blockCenter);
	}

	fin.close();
}

Block** Chunk::GetBlocks()
{
	return mBlocks;
}