#ifndef FREE_LIST_ALLOCATOR_H
#define FREE_LIST_ALLOCATOR_H

#include "extra/allocator/allocator.h"

class FreeListAllocator : public Allocator {

	public:

		FreeListAllocator(size_t size, void *start);
		~FreeListAllocator();

		void *allocate(size_t size, uint8_t alignment) override;
		void deallocate(void *p) override;

	private:

		struct AllocationHeader {

			size_t size;
			uint8_t adjustment;
		};

		struct FreeBlock {

			size_t size;
			FreeBlock *next;
		};

		FreeListAllocator(const FreeListAllocator&);
		FreeListAllocator &operator=(const FreeListAllocator&);

		FreeBlock  *_free_blocks;
};

#endif

