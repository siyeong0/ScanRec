#pragma once
#include <string>
#include <fstream>
#include <cassert>

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

struct Vec3
{
	float X;
	float Y;
	float Z;
};

struct Mat3x3
{
	float _00;
	float _01;
	float _02;

	float _10;
	float _11;
	float _12;

	float _20;
	float _21;
	float _22;
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

std::string centerToString(float center[]);
void centerFromIdx(float outCenter[], size_t indices[], float numInSide, float size);
void idxFromCenter(size_t outIndices[], float center[], float numInSide, float size);