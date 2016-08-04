#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "extra/allocator/allocator.h"

class StackAllocator : public Allocator {

	public:

		StackAllocator(size_t size, void *start, const char *name);
		~StackAllocator();

		void *allocate(size_t size, uint8_t alignment) override;
		void deallocate(void *p) override;

	private:

		StackAllocator(const StackAllocator&);
		StackAllocator &operator=(const StackAllocator&);

		struct AllocationHeader {
#ifndef NDEBUG
			void *prev_address;
#endif
			uint8_t adjustment;
		};

#ifndef NDEBUG
		void *_prev_position;
#endif

		void *_current_pos;
};

#endif

