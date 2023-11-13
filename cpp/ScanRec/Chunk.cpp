#include "Chunk.h"

Chunk::Chunk()
	: mBlocks(nullptr)
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

void Chunk::AddPoint(float* center, PointData& data, uint8_t label)
{
	float cx = center[0];
	float cy = center[1];
	float cz = center[2];
	float x = data.X;
	float y = data.Y;
	float z = data.Z;
	Assert(fabs(cx - x) <= HalfChunkkSize && fabs(cy - y) <= HalfChunkkSize && fabs(cz - z) <= HalfChunkkSize);

	size_t idxX = size_t((x - cx + HalfChunkkSize) / BlockSize);
	size_t idxY = size_t((y - cy + HalfChunkkSize) / BlockSize);
	size_t idxZ = size_t((z - cz + HalfChunkkSize) / BlockSize);

	float blockCenter[3];
	memcpy(blockCenter, center, sizeof(float) * 3);
	size_t indices[3] = { idxX, idxY, idxZ };
	centerFromIdx(blockCenter, indices, NumBlocksInSide, BlockSize);

	size_t size = size_t(NumBlocksInSide);
	Block** block = &mBlocks[idxX * size * size + idxY * size + idxZ];
	if (*block == nullptr)
	{
		*block = new Block;
	}
	(*block)->AddPoint(blockCenter, data, label);
}

void Chunk::Write(float* center)
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

					float blockCenter[3];
					memcpy(blockCenter, center, sizeof(float) * 3);
					size_t indices[3] = { x, y, z };
					centerFromIdx(blockCenter, indices, NumBlocksInSide, BlockSize);
					(*block)->Write(blockCenter);

					delete *block;
					*block = nullptr;
				}
			}
		}
	}

	fout.close();
}

void Chunk::Read(float* center)
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

		float blockCenter[3];
		memcpy(blockCenter, center, sizeof(float) * 3);
		centerFromIdx(blockCenter, indices, NumBlocksInSide, BlockSize);
		(*block)->Read(blockCenter);
	}

	fin.close();
}