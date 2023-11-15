#pragma once
#include <cstdint>
#include <iostream>
#include <numeric>
#include "Common.h"
#include "LinkedList.hpp"

enum
{
	POINTS_PER_FRAG = 24,
	COLOR_OFFSET_IN_FLOAT = POINTS_PER_FRAG * 3,
	BYTES_PER_POINT = 15
};
const float PCD_EMPTY_VAL = 9e+20f;
const float PCD_MIN_DIST = FRAGMENT_SIZE / powf(POINTS_PER_FRAG, 1.f / 3.f);

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

	void Write(const Vector3& center);
	void Read(const Vector3& center);
public:
	static float* GetPointPtr(void* pcdPtr);
	static uint8_t* GetColorPtr(void* pcdPtr);
private:
	void addLabel(uint8_t label, size_t pointIdx);
private:
	LinkedList<LabelCount> mLabelCountList;
	void* mPcd;
};