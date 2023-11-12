#include "Common.h"

std::string FRAGMENT_CACHE_PATH = std::string("../cache/fragment/");
std::string BLOCK_CACHE_PATH = std::string("../cache/block/");;

std::string centerToString(float center[])
{
	std::string path = 
		std::to_string(center[0])
		+ std::to_string(center[1])
		+ std::to_string(center[2]);
	return path;
}