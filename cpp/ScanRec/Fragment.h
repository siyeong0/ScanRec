#pragma once
#include <cstdint>
#include <iostream>
#include <numeric>
#include "Common.h"
#include "LinkedList.hpp"

enum
{
	POINTS_PER_FRAG = 16,
	COLOR_OFFSET_IN_FLOAT = 16 * 3,
	BYTES_PER_POINT = 15
};
const float PCD_EMPTY_VAL = std::numeric_limits<float>::max();

class Fragment
{
public:
	struct LabelCount
	{
		uint32_t Label : 8;
		uint32_t Count : 24;
	};
public:
	Fragment();
	~Fragment();

	bool AddPoint(PointData& data, uint8_t label);

	uint8_t GetLabel();
	const LinkedList<LabelCount>& GetLabelCountList();
	void* GetPcd();

	void Write(float* center);
	void Read(float* center);
private:
	void addLabel(uint8_t label, size_t pointIdx);
private:
	LinkedList<LabelCount> mLabelCountList;
	void* mPcd;
};