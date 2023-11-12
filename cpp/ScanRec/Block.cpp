#include "Block.h"
#include <cmath>
#include <string>
#include "Common.h"

float Block::BlockSize = 1.0f;
float Block::FragmentSize = 0.05f;
float Block::NumFragsInSide = Block::BlockSize / Block::FragmentSize;
size_t Block::NumFragsInBlock = size_t(powf(floor(Block::NumFragsInSide), 3.0));
float Block::HalfBlockSize = Block::BlockSize / 2.0f;

void Block::Config(size_t blockSizeInMeter, size_t fragSizeInMeter)
{
	BlockSize = float(blockSizeInMeter);
	FragmentSize = float(fragSizeInMeter);
	NumFragsInSide = BlockSize / FragmentSize;
	NumFragsInBlock = size_t(powf(floor(NumFragsInSide), 3.0));
	HalfBlockSize = BlockSize / 2.0f;
}

Block::Block()
	: mFrags(nullptr)
{
	mFrags = new Fragment*[NumFragsInBlock];
	memset(mFrags, 0, sizeof(Fragment*) * NumFragsInBlock);
}
Block::~Block()
{
	delete[] mFrags;
}

void Block::AddPoint(float* center, float* data, uint8_t label)
{
	float cx = center[0];
	float cy = center[1];
	float cz = center[2];
	float x = data[0];
	float y = data[1];
	float z = data[2];
	size_t idxX = size_t((x - cx + HalfBlockSize) / FragmentSize);
	size_t idxY = size_t((y - cy + HalfBlockSize) / FragmentSize);
	size_t idxZ = size_t((z - cz + HalfBlockSize) / FragmentSize);

	size_t size = size_t(NumFragsInSide);
	Fragment** frag = &mFrags[idxX * size * size + idxY * size + idxZ];
	if (*frag == nullptr)
	{
		*frag = new Fragment;
	}
	std::string basePath = FRAGMENT_CACHE_PATH;
	(*frag)->AddPoint(data, label, basePath+ centerToString(center));
}