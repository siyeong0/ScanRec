#include "Fragment.h"
#include <memory>
#include <fstream>

Fragment::Fragment()
	: mLabelCountList(LinkedList<LabelCount>())
	, mPcd(nullptr)
{
	mPcd = new float[POINTS_PER_FRAG * PCD_OFFSET];
	std::fill(mPcd, mPcd + POINTS_PER_FRAG * PCD_OFFSET, PCD_EMPTY_VAL);
}

Fragment::~Fragment()
{
	if (mPcd == nullptr)
	{
		return;
	}
	delete[] mPcd;
}

bool Fragment::AddPoint(float* data, uint8_t label, std::string fragPath)
{
	// already full
	if (mPcd == nullptr)
	{
		return false;
	}
	// find empty memory
	for (int i = 0; i < POINTS_PER_FRAG; i++)
	{
		float* pcdPtr = &mPcd[i * PCD_OFFSET];
		// add data
		if (pcdPtr[0] == PCD_EMPTY_VAL)
		{
			memcpy(pcdPtr, data, sizeof(float) * PCD_OFFSET);
			addLabel(label, i);
			// now full
			if (i == POINTS_PER_FRAG - 1)
			{
				Write(fragPath);
			}

			Write(fragPath);
			Read(fragPath);

			return true;
		}
	}
	// TODO: assert(false)
	return false;
}

//uint8_t Fragment::GetLabel()
//{
//	uint32_t maxCount = 0;
//	uint8_t maxLabel = 0;
//
//	size_t idx = 0;
//	while (mLabelCountList[idx] != nullptr)
//	{
//		uint8_t label = mLabelCountList[idx]->Data.Label;
//		uint32_t countBitMask = mLabelCountList[idx]->Data.Count;
//		uint32_t count;
//		for (count = 0; countBitMask != 0; count++)
//		{ 
//			countBitMask &= (countBitMask - 1);
//		}
//		if (count >= maxCount)
//		{
//			maxCount = count;
//			maxLabel = label;
//		}
//		idx++;
//	}
//
//	return maxLabel;
//}

const LinkedList<Fragment::LabelCount>& Fragment::GetLabelCountList(std::string fragPath)
{
	if (mLabelCountList.mRoot == nullptr)
	{
		Read(fragPath);
	}
	return mLabelCountList;
}

float* Fragment::GetPcd(std::string fragPath)
{
	if (mPcd == nullptr)
	{
		Read(fragPath);
	}
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

void Fragment::Write(std::string fragPath)
{
	std::ofstream fout;
	fout.open(fragPath, std::ios::out | std::ios::binary);
	// write pcd
	fout.write(reinterpret_cast<const char*>(mPcd), POINTS_PER_FRAG * PCD_OFFSET * sizeof(float));
	delete[] mPcd;
	mPcd = nullptr;
	// write label list
	mLabelCountList.Write(fout);
	mLabelCountList.Free();

	fout.close();
}

void Fragment::Read(std::string fragPath)
{
	std::ifstream fin;
	fin.open(fragPath, std::ios::in | std::ios::binary);
	// read pcd
	mPcd = new float[POINTS_PER_FRAG * PCD_OFFSET];
	fin.read(reinterpret_cast<char*>(mPcd), POINTS_PER_FRAG * PCD_OFFSET * sizeof(float));
	// read label list
	mLabelCountList.Read(fin);

	fin.close();
}