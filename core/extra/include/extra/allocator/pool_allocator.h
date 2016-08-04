#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "extra/allocator/allocator.h"

class PoolAllocator : public Allocator {

	public:

		PoolAllocator(size_t object_size, uint8_t object_alignment, size_t size, void *start, const char *name);
		~PoolAllocator();

		void *allocate(size_t size, uint8_t alignment) override;
		void deallocate(void *p) override;

	private:

		PoolAllocator(const PoolAllocator&);
		PoolAllocator &operator=(const PoolAllocator&);

		size_t _object_size;
		uint8_t _object_alignment;

		void **_free_list;
};

#endif

