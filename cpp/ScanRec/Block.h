#pragma once
#include "Fragment.h"

class Block
{
public:
	static float BlockSize;
	static float FragmentSize;
	static float NumFragsInSide;
	static size_t NumFragsInBlock;
	static float HalfBlockSize;
	static void Config(size_t blockSizeInMeter, size_t fragSizeInMeter);
public:
	Block();
	~Block();

	void AddPoint(float* center, float* data, uint8_t label);

	void Write(std::string blockPath);
	void Read(std::string blockPath);
public:
//private:
	Fragment** mFrags;
};