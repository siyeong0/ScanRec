#include "Chunk.h"
#include "MemPool.hpp"

static MemPool<NUM_BLOCKS_IN_CHUNK * sizeof(Block*), 2048> gBlockPtrArrPool;
static MemPool<sizeof(Block), 65535> gBlockPool;

Chunk::Chunk()
	: mBlocks(nullptr)
{
	mBlocks = (Block**)gBlockPtrArrPool.Alloc();
	memset(mBlocks, 0, sizeof(Block*) * NUM_BLOCKS_IN_CHUNK);
}

Chunk::~Chunk()
{
	for (size_t i = 0; i < NUM_BLOCKS_IN_CHUNK; i++)
	{
		Block** block = &mBlocks[i];
		if (*block != nullptr)
		{
			(*block)->~Block();
			gBlockPool.Free(*block);
		}
	}
	gBlockPtrArrPool.Free(mBlocks);
}

void Chunk::AddPoint(const Vector3& center, PointData& data, uint8_t label)
{
	float cx = center.x;
	float cy = center.y;
	float cz = center.z;
	float x = data.Position.x;
	float y = data.Position.y;
	float z = data.Position.z;
	Assert(fabs(cx - x) <= HALF_CHUNK_SIZE + 1e-4f && fabs(cy - y) <= HALF_CHUNK_SIZE + 1e-4f && fabs(cz - z) <= HALF_CHUNK_SIZE + 1e-4f);

	size_t idxX = size_t((x - cx + HALF_CHUNK_SIZE) / BLOCK_SIZE);
	size_t idxY = size_t((y - cy + HALF_CHUNK_SIZE) / BLOCK_SIZE);
	size_t idxZ = size_t((z - cz + HALF_CHUNK_SIZE) / BLOCK_SIZE);

	Vector3 blockCenter;
	memcpy(&blockCenter, &center, sizeof(float) * 3);
	size_t indices[3] = { idxX, idxY, idxZ };
	centerFromIdx(&blockCenter, indices, NUM_BLOCKS_IN_SIDE, BLOCK_SIZE);

	size_t size = size_t(NUM_BLOCKS_IN_SIDE);
	Block** block = &mBlocks[idxX * size * size + idxY * size + idxZ];
	if (*block == nullptr)
	{
		*block = (Block*)gBlockPool.Alloc();
		new (*block) Block();
	}
	(*block)->AddPoint(blockCenter, data, label);
}

Block** Chunk::GetBlocks()
{
	return mBlocks;
}

void Chunk::Write(const Vector3& center)
{
	std::string ChunkPath = CHUNK_CACHE_PATH;
	ChunkPath += centerToString(center);

	std::ofstream fout;
	fout.open(ChunkPath, std::ios::out | std::ios::binary);
	// write fragments
	size_t size = size_t(NUM_FRAGS_IN_SIDE);
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
					centerFromIdx(&blockCenter, indices, NUM_BLOCKS_IN_SIDE, BLOCK_SIZE);
					(*block)->Write(blockCenter);

					(*block)->~Block();
					gBlockPool.Free(*block);
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
	size_t size = size_t(NUM_FRAGS_IN_SIDE);
	while (!fin.eof())
	{
		size_t indices[3];
		fin.read(reinterpret_cast<char*>(indices), sizeof(size_t) * 3);
		size_t x = indices[0];
		size_t y = indices[1];
		size_t z = indices[2];

		Block** block = &mBlocks[x * size * size + y * size + z];
		*block = (Block*)gBlockPool.Alloc();
		new (*block) Block();

		Vector3 blockCenter;
		memcpy(&blockCenter, &center, sizeof(float) * 3);
		centerFromIdx(&blockCenter, indices, NUM_BLOCKS_IN_SIDE, BLOCK_SIZE);
		(*block)->Read(blockCenter);
	}

	fin.close();
}

bool Chunk::Include(const Vector3& center, const Vector3& point)
{
	float dx = fabs(center.x - point.x);
	float dy = fabs(center.y - point.y);
	float dz = fabs(center.z - point.z);

	return (dx <= HALF_CHUNK_SIZE && dy <= HALF_CHUNK_SIZE && dz <= HALF_CHUNK_SIZE);
}

BoundingBox Chunk::GetBoundingBox(const Vector3& center)
{
	return BoundingBox(center, Vector3(HALF_CHUNK_SIZE, HALF_CHUNK_SIZE, HALF_CHUNK_SIZE));
}