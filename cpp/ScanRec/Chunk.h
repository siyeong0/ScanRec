#pragma once
#include "Block.h"

class Chunk
{
public:
	Chunk(const Vector3& center);
	~Chunk();

	void AddPoint(PointData& data, uint8_t label);
	const Vector3& GetCenter();
	bool Include(const Vector3& point);

	void Write(const Vector3& center);
	void Read(const Vector3& center);

	Block** GetBlocks();
	BoundingBox GetBoundingBox();
private:
	Block** mBlocks;
	const Vector3 mCenter;
};