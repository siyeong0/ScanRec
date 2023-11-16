#pragma once
#include <vector>
#include "Common.h"
#include "Memory.h"

using namespace mem;

template<size_t typeSize, size_t bucketSize=1024>
class MemPool
{
private:
	struct Bucket
	{
		void* Ptr;
		uint16_t Idx;
		uint16_t* IdxTable;
	};
public:
	MemPool()
	{
		static_assert(bucketSize < 65536);
		static_assert(typeSize * bucketSize < 1e+9);
		allocNewBucket();
	}

	~MemPool()
	{
		std::cout << "Bucket<" << typeSize << ", " << bucketSize << "> size : " << mBuckets.size() << std::endl;
		for (auto& bucket : mBuckets)
		{
			mem::Free(bucket.Ptr);
			mem::Free(bucket.IdxTable);
		}
	}

	void* Alloc()
	{
		for (int i = (int)mBuckets.size() - 1; i >= 0; i--)
		{
			Bucket& currBucket = mBuckets[i];
			if (currBucket.Idx == bucketSize)
			{
				continue;
			}

			char* bytePtr = reinterpret_cast<char*>(currBucket.Ptr);
			void* retPtr = reinterpret_cast<void*>(bytePtr + currBucket.IdxTable[currBucket.Idx] * typeSize);
			currBucket.Idx++;
			return retPtr;
		}
		// All buckets are full, allocate new bucket
		Bucket& bucket = allocNewBucket();
		void* retPtr = bucket.Ptr;
		bucket.Idx++;
		return retPtr;
	}

	void Free(void* ptr)
	{
		const size_t STRIDE = typeSize * bucketSize;
		const size_t ptrVal = reinterpret_cast<size_t>(ptr);
		for (int i = (int)mBuckets.size() - 1; i >= 0; i--)
		{
			Bucket& currBucket = mBuckets[i];
			const size_t currPtrVal = reinterpret_cast<size_t>(currBucket.Ptr);
			if (ptrVal >= currPtrVal && ptrVal < currPtrVal + STRIDE)
			{
				uint16_t targetIdx = uint16_t((ptrVal - currPtrVal) / typeSize);
				size_t j = 0;
				while (currBucket.IdxTable[j] != targetIdx)
				{
					j++;
				}
				currBucket.Idx--;
				uint16_t& idxTarget = currBucket.IdxTable[j];
				uint16_t& idxSource = currBucket.IdxTable[currBucket.Idx];
				// Swap
				uint16_t temp = idxTarget;
				idxTarget = idxSource;
				idxSource = temp;

				return;
			}
		}
		Assert(false);
	}

private:
	Bucket& allocNewBucket()
	{
		Bucket bucket;
		bucket.Ptr = mem::Alloc(typeSize * bucketSize);
		bucket.Idx = 0;
		bucket.IdxTable = (uint16_t*)mem::Alloc(sizeof(uint16_t) * bucketSize);
		for (uint16_t i = 0; i < bucketSize; i++)
		{
			bucket.IdxTable[i] = i;
		}
		mBuckets.push_back(bucket);

		return mBuckets[mBuckets.size() - 1];
	}
public:
	std::vector<Bucket> mBuckets;

};