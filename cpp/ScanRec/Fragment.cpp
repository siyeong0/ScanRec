#include "Fragment.h"
#include <memory>
#include <fstream>
#include "Common.h"
#include "MemPool.hpp"

static MemPool<POINTS_PER_FRAG * BYTES_PER_POINT, 65535> gPcdPool;

Fragment::Fragment()
	: mPcd(nullptr)
{
	mPcd = gPcdPool.Alloc();
	// Initialize to empty val
	float* fPcd = reinterpret_cast<float*>(mPcd);
	std::fill(fPcd, fPcd + POINTS_PER_FRAG * 3, PCD_EMPTY_VAL);
}

Fragment::~Fragment()
{
	if (mPcd == nullptr)
	{
		return;
	}
	gPcdPool.Free(mPcd);
}

bool Fragment::AddPoint(PointData& data, uint8_t label)
{
	float* fPcd = GetPointPtr(mPcd);
	uint8_t* uPcd = GetColorPtr(mPcd);
	// Find empty memory
	for (int i = 0; i < POINTS_PER_FRAG; i++)
	{
		float* pointPtr = &fPcd[i * 3];
		uint8_t* colPtr = &uPcd[i * 3];

		if (pointPtr[0] == PCD_EMPTY_VAL)
		{
			memcpy(pointPtr, &data, sizeof(float) * 3);
			memcpy(colPtr, &(data.Color), sizeof(uint8_t) * 3);
			addLabel(label, i);

			return true;
		}
		else
		{
			Vector3 currPoint(pointPtr);
			Vector3 inputPoint(reinterpret_cast<float*>(&data));
			float dist = (currPoint - inputPoint).Length();
			if (dist < PCD_MIN_DIST)
			{
				return false;
			}
		}
	}
	// Full
	return false;
}

uint8_t Fragment::GetLabel()
{
	uint32_t maxCount = 0;
	uint8_t maxLabel = 0;

	size_t idx = 0;
	while (mLabelCountList[idx] != nullptr)
	{
		uint8_t label = mLabelCountList[idx]->Data.Label;
		uint32_t countBitMask = mLabelCountList[idx]->Data.Count;
		uint32_t count;
		for (count = 0; countBitMask != 0; count++)
		{ 
			countBitMask &= (countBitMask - 1);
		}
		if (count >= maxCount)
		{
			maxCount = count;
			maxLabel = label;
		}
		idx++;
	}

	return maxLabel;
}

const LinkedList<Fragment::LabelCount>& Fragment::GetLabelCountList()
{
	return mLabelCountList;
}

void* Fragment::GetPcd()
{
	return mPcd;
}

void Fragment::addLabel(uint8_t label, size_t pointIdx)
{
	size_t idx = 0;
	while (mLabelCountList[idx] != nullptr)
	{
		if (mLabelCountList[idx]->Data.Label == label)
		{
			mLabelCountList[idx]->Data.Count |= 1 << pointIdx;
			return;
		}
		idx++;
	}
	LabelCount lc;
	lc.Label = label;
	lc.Count = 1 << pointIdx;
	mLabelCountList.Append(lc);
}

void Fragment::Write(const Vector3& center)
{
	std::string fragPath = FRAGMENT_CACHE_PATH;
	fragPath += centerToString(center);

	std::ofstream fout;
	fout.open(fragPath, std::ios::out | std::ios::binary);
	// Write pcd
	fout.write(reinterpret_cast<const char*>(mPcd), POINTS_PER_FRAG * BYTES_PER_POINT);
	gPcdPool.Free(mPcd);
	mPcd = nullptr;
	// Write label list
	mLabelCountList.Write(fout);
	mLabelCountList.Free();

	fout.close();
}

void Fragment::Read(const Vector3& center)
{
	std::string fragPath = FRAGMENT_CACHE_PATH;
	fragPath += centerToString(center);

	std::ifstream fin;
	fin.open(fragPath, std::ios::in | std::ios::binary);
	// Read pcd
	mPcd = gPcdPool.Alloc();
	fin.read(reinterpret_cast<char*>(mPcd), POINTS_PER_FRAG * BYTES_PER_POINT);
	// Read label list
	mLabelCountList.Read(fin);

	fin.close();
}

float* Fragment::GetPointPtr(void* pcdPtr)
{
	return reinterpret_cast<float*>(pcdPtr);
}

int8_t* Fragment::GetNormalPtr(void* pcdPtr)
{
	return reinterpret_cast<int8_t*>(pcdPtr) + COLOR_OFFSET_IN_BYTE;
}

uint8_t* Fragment::GetColorPtr(void* pcdPtr)
{
	return reinterpret_cast<uint8_t*>(pcdPtr) + COLOR_OFFSET_IN_BYTE;
}