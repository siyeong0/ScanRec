#pragma once
#include <cstdlib>

namespace mem
{
	inline void* Alloc(size_t size)
	{
		return malloc(size);
	}

	inline void Free(void* ptr)
	{
		free(ptr);
	}
}