#pragma once
#include <vector>
#include "Common.h"

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
		allocNewBucket();
	}

	~MemPool()
	{
		for (auto& bucket : mBuckets)
		{
			free(bucket.Ptr);
			free(bucket.IdxTable);
		}
	}

	void* Alloc()
	{
		for (int i = mBuckets.size() - 1; i >= 0; i--)
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
		// all buckets are full, allocate new bucket
		Bucket& bucket = allocNewBucket();
		void* retPtr = bucket.Ptr;
		bucket.Idx++;
		return retPtr;
	}

	void Free(void* ptr)
	{
		const size_t STRIDE = typeSize * bucketSize;
		const size_t ptrVal = reinterpret_cast<size_t>(ptr);
		for (int i = mBuckets.size() - 1; i >= 0; i--)
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
				// swap
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
		bucket.Ptr = malloc(typeSize * bucketSize);
		bucket.Idx = 0;
		bucket.IdxTable = (uint16_t*)malloc(sizeof(uint16_t) * bucketSize);
		for (size_t i = 0; i < bucketSize; i++)
		{
			bucket.IdxTable[i] = i;
		}
		mBuckets.push_back(bucket);

		return mBuckets[mBuckets.size() - 1];
	}
public:
	std::vector<Bucket> mBuckets;

};