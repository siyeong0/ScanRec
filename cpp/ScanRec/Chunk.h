#pragma once
#include "Block.h"

class Chunk
{
public:
	Chunk();
	~Chunk();

	void AddPoint(const Vector3& center, PointData& data, uint8_t label);

	Block** GetBlocks();

	void Write(const Vector3& center);
	void Read(const Vector3& center);

	static bool Include(const Vector3& center, const Vector3& point);
	static BoundingBox GetBoundingBox(const Vector3& center);
private:
	Block** mBlocks;
};