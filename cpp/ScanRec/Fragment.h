#pragma once
#include <cstdint>
#include <iostream>
#include <numeric>
#include "LinkedList.hpp"

const float PCD_EMPTY_VAL = std::numeric_limits<float>::max();
const size_t PCD_OFFSET = 9;	// 3(points) + 3(normals) + 3(colors)
const size_t BYTES_PER_POINT = 9 * sizeof(float);

class Fragment
{
public:
	enum
	{
		POINTS_PER_FRAG = 16
	};
	struct LabelCount
	{
		uint32_t Label : 8;
		uint32_t Count : 24;
	};
public:
	Fragment();
	~Fragment();

	bool AddPoint(float* data, uint8_t label, std::string fragPath);

	const LinkedList<LabelCount>& GetLabelCountList(std::string fragPath);
	float* GetPcd(std::string fragPath);

	void Write(std::string fragPath);
	void Read(std::string fragPath);
private:
	void addLabel(uint8_t label, size_t pointIdx);
private:
	LinkedList<LabelCount> mLabelCountList;
	float* mPcd;
};