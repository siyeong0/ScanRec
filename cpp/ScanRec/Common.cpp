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

std::string centerToString(float center[])
{
	std::string path = 
		std::to_string(center[0])
		+ std::to_string(center[1])
		+ std::to_string(center[2]);
	return path;
}

void centerFromIdx(float outCenter[], size_t indices[], float numInSide, float size)
{
	outCenter[0] += (float(indices[0]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
	outCenter[1] += (float(indices[1]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
	outCenter[2] += (float(indices[2]) - numInSide * 0.5f + float(int(numInSide + 1) % 2) * 0.5f) * size;
}

void idxFromCenter(size_t outIndices[], float center[], float numInSide, float size)
{
	outIndices[0] = center[0] / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
	outIndices[1] = center[1] / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
	outIndices[2] = center[2] / size, +numInSide * 0.5f - float(int(numInSide + 1) % 2) * 0.5f;
}