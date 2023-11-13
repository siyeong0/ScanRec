#pragma once
#include "Common.h"
#include "Fragment.h"

class Block
{
public:
	Block();
	~Block();

	void AddPoint(float* center, PointData& data, uint8_t label);

	void Write(float* center);
	void Read(float* center);
private:
	Fragment** mFrags;
};