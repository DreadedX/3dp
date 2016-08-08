#include "extra/allocator/stack_allocator.h"

StackAllocator::StackAllocator(size_t size, void* start, const char *name) 
	: Allocator(size, start, name), _current_pos(start) {

	assert(size > 0);

	#if _DEBUG
	_prev_position    = nullptr;
	#endif
}

StackAllocator::~StackAllocator() {

	#if _DEBUG
	_prev_position      = nullptr;
	#endif

	_current_pos   = nullptr;
}

void *StackAllocator::allocate(size_t size, uint8_t alignment) {

	assert(size != 0);

	uint8_t adjustment = pointer_math::alignForwardAdjustmentWithHeader(_current_pos, alignment, sizeof(AllocationHeader));

	if (_used_memory + adjustment + size > _size) {

		printf("Allocator ran out of memory!\n");

		return nullptr;
	}

	void *aligned_adress = pointer_math::add(_current_pos, adjustment);

	AllocationHeader *header = (AllocationHeader*)(pointer_math::subtract(aligned_adress, sizeof(AllocationHeader)));

	header->adjustment = adjustment;

#ifndef NDEBUG
	header->prev_address = _prev_position;

	_prev_position = aligned_adress;
#endif

	_current_pos = pointer_math::add(aligned_adress, size);

	_used_memory += size + adjustment;
	_num_allocations++;

	return aligned_adress;
}

void StackAllocator::deallocate(void *p) {

	assert(p == _prev_position);

	AllocationHeader *header = (AllocationHeader*)(pointer_math::subtract(p, sizeof(AllocationHeader)));

	_used_memory -= (uintptr_t)_current_pos - (uintptr_t)p + header->adjustment;

	_current_pos = pointer_math::subtract(p, header->adjustment);

#ifndef NDEBUG
	_prev_position = header->prev_address;
#endif

	_num_allocations--;
}
