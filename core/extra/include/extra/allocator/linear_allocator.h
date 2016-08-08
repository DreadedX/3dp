#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include "extra/allocator.h"

class LinearAllocator : public Allocator {

	public:

		LinearAllocator(size_t size, void* start, const char *name);
		~LinearAllocator();

		void *allocate(size_t size, uint8_t alignment) override;
		void deallocate(void *p) override;

		void clear();

	private:

		LinearAllocator(const LinearAllocator&);
		LinearAllocator &operator=(const LinearAllocator&);

		void *_current_pos;
};

#endif

