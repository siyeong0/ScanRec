#pragma once
#include "Block.h"

class Chunk
{
public:
	Chunk();
	~Chunk();

	void AddPoint(float* center, PointData& data, uint8_t label);

	void Write(float* center);
	void Read(float* center);
private:
	Block** mBlocks;
};