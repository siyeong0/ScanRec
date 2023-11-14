#pragma once
#include <string>
#include <fstream>
#include <cassert>

#include "MathLib.h"

extern std::string FRAGMENT_CACHE_PATH;
extern std::string BLOCK_CACHE_PATH;
extern std::string CHUNK_CACHE_PATH;

extern float FragmentSize;
extern float BlockSize;
extern float ChunkSize;
extern float NumFragsInSide;
extern float NumBlocksInSide;
extern size_t NumFragsInBlock;
extern size_t NumBlocksInChunk;
extern float HalfFragtSize;
extern float HalfBlockSize;
extern float HalfChunkkSize;

struct RGB
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct PointData
{
	float X;
	float Y;
	float Z;
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

inline void _Assert(bool e)
{
	if (!e)
	{
		_CrtDbgBreak();
	}
}

# ifdef DEBUG
# define Assert(E) _Assert(E)
# else
# define Assert(E) __assume(E)
#endif

std::string centerToString(const Vector3& center);
void centerFromIdx(Vector3* outCenter, size_t indices[], float numInSide, float size);
void idxFromCenter(size_t outIndices[], const Vector3& center, float numInSide, float size);