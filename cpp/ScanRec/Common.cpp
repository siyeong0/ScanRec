#include "Common.h"

std::string FRAGMENT_CACHE_PATH = std::string("../cache/fragment/");
std::string BLOCK_CACHE_PATH = std::string("../cache/block/");
std::string CHUNK_CACHE_PATH = std::string("../cache/chunk/");

float FragmentSize		= 0.05f;
float BlockSize			= 1.0f;
float ChunkSize			= 8.0f;
float NumFragsInSide	= BlockSize / FragmentSize;
float NumBlocksInSide	= ChunkSize / BlockSize;
size_t NumFragsInBlock	= size_t(powf(floor(NumFragsInSide), 3.0));;
size_t NumBlocksInChunk = size_t(powf(floor(NumBlocksInSide), 3.0));;
float HalfFragtSize		= FragmentSize / 2.0f;
float HalfBlockSize		= BlockSize / 2.0f;
float HalfChunkkSize	= ChunkSize / 2.0f;

std::string centerToString(const Vector3& center)
{
	std::string path = 
		std::to_string(center.x)
		+ std::to_string(center.y)
		+ std::to_string(center.z);
	return path;
}

void centerFromIdx(Vector3* outCenter, size_t indices[], float numInSide, float size)
{
	outCenter->x += (float(indices[0]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
	outCenter->y += (float(indices[1]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
	outCenter->z += (float(indices[2]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
}

void idxFromCenter(size_t outIndices[], const Vector3& center, float numInSide, float size)
{
	outIndices[0] = center.x / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
	outIndices[1] = center.y / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
	outIndices[2] = center.z / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
}