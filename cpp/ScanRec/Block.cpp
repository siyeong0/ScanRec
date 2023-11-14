#include "Block.h"
#include <cmath>
#include <string>
#include "Common.h"

//void Block::Config(size_t blockSizeInMeter, size_t fragSizeInMeter)
//{
//	BlockSize = float(blockSizeInMeter);
//	FragmentSize = float(fragSizeInMeter);
//	NumFragsInSide = BlockSize / FragmentSize;
//	NumFragsInBlock = size_t(powf(floor(NumFragsInSide), 3.0));
//	HalfBlockSize = BlockSize / 2.0f;
//}

Block::Block()
	: mFrags(nullptr)
{
	mFrags = new Fragment*[NumFragsInBlock];
	memset(mFrags, 0, sizeof(Fragment*) * NumFragsInBlock);
}
Block::~Block()
{
	for (size_t i = 0; i < NumFragsInBlock; i++)
	{
		Fragment** frag = &mFrags[i];
		if (*frag != nullptr)
		{
			delete* frag;
			*frag = nullptr;
		}
	}
	delete[] mFrags;
}

void Block::AddPoint(const Vector3& center, PointData& data, uint8_t label)
{
	float cx = center.x;
	float cy = center.y;
	float cz = center.z;
	float x = data.X;
	float y = data.Y;
	float z = data.Z;
	Assert(fabs(cx - x) <= HalfBlockSize && fabs(cy - y) <= HalfBlockSize && fabs(cz - z) <= HalfBlockSize);

	size_t idxX = size_t((x - cx + HalfBlockSize) / FragmentSize);
	size_t idxY = size_t((y - cy + HalfBlockSize) / FragmentSize);
	size_t idxZ = size_t((z - cz + HalfBlockSize) / FragmentSize);

	size_t size = size_t(NumFragsInSide);
	Fragment** frag = &mFrags[idxX * size * size + idxY * size + idxZ];
	if (*frag == nullptr)
	{
		*frag = new Fragment();
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
	size_t size = size_t(NumFragsInSide);
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
					centerFromIdx(&fragCenter, indices, NumFragsInSide, FragmentSize);
					(*frag)->Write(fragCenter);

					delete* frag;
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
	size_t size = size_t(NumFragsInSide);
	while (!fin.eof())
	{
		size_t indices[3];
		fin.read(reinterpret_cast<char*>(indices), sizeof(size_t) * 3);
		size_t x = indices[0];
		size_t y = indices[1];
		size_t z = indices[2];

		Fragment** frag = &mFrags[x * size * size + y * size + z];
		*frag = new Fragment;

		Vector3 fragCenter;
		memcpy(&fragCenter, &center, sizeof(float) * 3);
		centerFromIdx(&fragCenter, indices, NumFragsInSide, FragmentSize);
		(*frag)->Read(fragCenter);
	}

	fin.close();
}