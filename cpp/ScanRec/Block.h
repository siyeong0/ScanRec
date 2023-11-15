#pragma once
#include "Common.h"
#include "Fragment.h"

class Block
{
public:
	Block();
	~Block();

	void AddPoint(const Vector3& center, PointData& data, uint8_t label);

	void Write(const Vector3& center);
	void Read(const Vector3& center);

	Fragment** GetFrags();
private:
	Fragment** mFrags;
};