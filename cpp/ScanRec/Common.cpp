#include "Common.h"

std::string FRAGMENT_CACHE_PATH = std::string("../cache/fragment/");
std::string BLOCK_CACHE_PATH = std::string("../cache/block/");
std::string CHUNK_CACHE_PATH = std::string("../cache/chunk/");

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