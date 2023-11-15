#include "Fragment.h"
#include <memory>
#include <fstream>
#include "Common.h"

Fragment::Fragment()
	: mLabelCountList(LinkedList<LabelCount>())
	, mPcd(nullptr)
{
	void* t = nullptr;
	t = malloc(POINTS_PER_FRAG * BYTES_PER_POINT);
	mPcd = t;
	float* fPcd = reinterpret_cast<float*>(mPcd);
	std::fill(fPcd, fPcd + POINTS_PER_FRAG * 3, PCD_EMPTY_VAL);
}

Fragment::~Fragment()
{
	if (mPcd == nullptr)
	{
		return;
	}
	free(mPcd);
}

bool Fragment::AddPoint(PointData& data, uint8_t label)
{
	float* fPcd = GetPointPtr(mPcd);
	uint8_t* uPcd = GetColorPtr(mPcd);
	// find empty memory
	for (int i = 0; i < POINTS_PER_FRAG; i++)
	{
		float* pointPtr = &fPcd[i * 3];
		uint8_t* colPtr = &uPcd[i * 3];

		if (pointPtr[0] == PCD_EMPTY_VAL)
		{
			memcpy(pointPtr, &data, sizeof(float) * 3);
			memcpy(colPtr, &(data.R), sizeof(uint8_t) * 3);
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
	//  full
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
	// write pcd
	fout.write(reinterpret_cast<const char*>(mPcd), POINTS_PER_FRAG * BYTES_PER_POINT);
	delete[] mPcd;
	mPcd = nullptr;
	// write label list
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
	// read pcd
	mPcd = reinterpret_cast<void*>(new uint8_t[POINTS_PER_FRAG * BYTES_PER_POINT]);
	fin.read(reinterpret_cast<char*>(mPcd), POINTS_PER_FRAG * BYTES_PER_POINT);
	// read label list
	mLabelCountList.Read(fin);

	fin.close();
}

float* Fragment::GetPointPtr(void* pcdPtr)
{
	return reinterpret_cast<float*>(pcdPtr);
}

uint8_t* Fragment::GetColorPtr(void* pcdPtr)
{
	return reinterpret_cast<uint8_t*>(GetPointPtr(pcdPtr) + COLOR_OFFSET_IN_FLOAT);
}