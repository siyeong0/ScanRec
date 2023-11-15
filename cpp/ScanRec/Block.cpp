#include "Block.h"
#include <cmath>
#include <string>
#include "Common.h"
#include "MemPool.hpp"

static MemPool<NUM_FRAGS_IN_BLOCK * sizeof(Fragment*)> gFragPtrArrMem;
static MemPool<sizeof(Fragment), 65535> gFragMem;

Block::Block()
	: mFrags(nullptr)
{
	mFrags = (Fragment**)gFragPtrArrMem.Alloc();
	memset(mFrags, 0, sizeof(Fragment*) * NUM_FRAGS_IN_BLOCK);
}
Block::~Block()
{
	for (size_t i = 0; i < NUM_FRAGS_IN_BLOCK; i++)
	{
		Fragment** frag = &mFrags[i];
		if (*frag != nullptr)
		{
			(*frag)->~Fragment();
			gFragMem.Free(*frag);
		}
	}
	gFragPtrArrMem.Free(mFrags);
}

void Block::AddPoint(const Vector3& center, PointData& data, uint8_t label)
{
	float cx = center.x;
	float cy = center.y;
	float cz = center.z;
	float x = data.X;
	float y = data.Y;
	float z = data.Z;
	Assert(fabs(cx - x) <= HALF_BLOCK_SIZE + 1e-4f && fabs(cy - y) <= HALF_BLOCK_SIZE + 1e-4f && fabs(cz - z) <= HALF_BLOCK_SIZE + 1e-4f);

	size_t idxX = size_t((x - cx + HALF_BLOCK_SIZE) / FRAGMENT_SIZE);
	size_t idxY = size_t((y - cy + HALF_BLOCK_SIZE) / FRAGMENT_SIZE);
	size_t idxZ = size_t((z - cz + HALF_BLOCK_SIZE) / FRAGMENT_SIZE);

	size_t size = size_t(NUM_FRAGS_IN_SIDE);
	Fragment** frag = &mFrags[idxX * size * size + idxY * size + idxZ];
	if (*frag == nullptr)
	{
		*frag = (Fragment*)gFragMem.Alloc();
		new (*frag) Fragment();
	}
	(*frag)->AddPoint(data, label);
}


void Block::Write(const Vector3& center)
{
	std::string blockPath = BLOCK_CACHE_PATH;
	blockPath += centerToString(center);

	std::ofstream fout;
	fout.open(blockPath, std::ios::out | std::ios::binary);
	// write fragments
	size_t size = size_t(NUM_FRAGS_IN_SIDE);
	for (size_t x = 0; x < size; x++)
	{
		for (size_t y = 0; y < size; y++)
		{
			for (size_t z = 0; z < size; z++)
			{
				Fragment** frag = &mFrags[x * size * size + y * size + z];
				if (*frag != nullptr)
				{
					fout.write(reinterpret_cast<const char*>(&x), sizeof(size_t));
					fout.write(reinterpret_cast<const char*>(&y), sizeof(size_t));
					fout.write(reinterpret_cast<const char*>(&z), sizeof(size_t));

					Vector3 fragCenter;
					memcpy(&fragCenter, &center, sizeof(float) * 3);
					size_t indices[3] = { x, y, z };
					centerFromIdx(&fragCenter, indices, NUM_FRAGS_IN_SIDE, FRAGMENT_SIZE);
					(*frag)->Write(fragCenter);

					(*frag)->~Fragment();
					gFragMem.Free(*frag);
					*frag = nullptr;
				}
			}
		}
	}

	fout.close();
}

void Block::Read(const Vector3& center)
{
	std::string blockPath = BLOCK_CACHE_PATH;
	blockPath += centerToString(center);

	std::ifstream fin;
	fin.open(blockPath, std::ios::in | std::ios::binary);
	// read fragments
	size_t size = size_t(NUM_FRAGS_IN_SIDE);
	while (!fin.eof())
	{
		size_t indices[3];
		fin.read(reinterpret_cast<char*>(indices), sizeof(size_t) * 3);
		size_t x = indices[0];
		size_t y = indices[1];
		size_t z = indices[2];

		Fragment** frag = &mFrags[x * size * size + y * size + z];
		*frag = (Fragment*)gFragMem.Alloc();
		new (*frag) Fragment();

		Vector3 fragCenter;
		memcpy(&fragCenter, &center, sizeof(float) * 3);
		centerFromIdx(&fragCenter, indices, NUM_FRAGS_IN_SIDE, FRAGMENT_SIZE);
		(*frag)->Read(fragCenter);
	}

	fin.close();
}

Fragment** Block::GetFrags()
{
	return mFrags;
}