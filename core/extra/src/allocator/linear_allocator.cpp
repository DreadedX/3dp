#include "extra/allocator/linear_allocator.h"

LinearAllocator::LinearAllocator(size_t size, void *start, const char *name)
	: Allocator(size, start, name), _current_pos(start) {

		assert(size > 0);
	}

LinearAllocator::~LinearAllocator() {

	_current_pos = nullptr;
}

void *LinearAllocator::allocate(size_t size, uint8_t alignment) {

	assert (size != 0);

	uint8_t adjustment = pointer_math::alignForwardAdjustment(_current_pos, alignment);

	if(_used_memory + adjustment + size > _size) {

		printf("Allocator ran out of memory\n");

		return nullptr;
	}

	uintptr_t aligned_address = (uintptr_t)_current_pos + adjustment;

	_current_pos = (void*)(aligned_address + size);

	_used_memory += size + adjustment;
	_num_allocations++;

	return (void*)aligned_address;
}

void LinearAllocator::deallocate(void*) {

	assert(false && "Use clear() instead");
}

void LinearAllocator::clear() {

	_num_allocations = 0;
	_used_memory     = 0;

	_current_pos = _start;
}
