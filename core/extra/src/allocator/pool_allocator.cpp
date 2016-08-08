#include "extra/allocator/pool_allocator.h"

PoolAllocator::PoolAllocator(size_t object_size, uint8_t object_alignment, size_t size, void* start, const char *name) 
	: Allocator(size, start, name), _object_size(object_size), _object_alignment(object_alignment) {

		assert(object_size >= sizeof(void*));

		uint8_t adjustment = pointer_math::alignForwardAdjustment(start, object_alignment);
		
		_free_list = (void**)pointer_math::add(start, adjustment);

		size_t num_objects = (size-adjustment)/object_size;

		void **p = _free_list;

		for(size_t i = 0; i < num_objects-1; i++) {

			*p = pointer_math::add(p, object_size);
			p = (void**) *p;
		}

		*p = nullptr;
}

PoolAllocator::~PoolAllocator() {

	_free_list = nullptr;
}

void *PoolAllocator::allocate(size_t size, uint8_t alignment) {

	assert(size == _object_size && alignment == _object_alignment);

	if (_free_list == nullptr) {

		printf("This should not happen I am pretty sure");

		return nullptr;
	}

	void *p = _free_list;

	_free_list = (void**)(*_free_list);

	_used_memory += size;
	_num_allocations++;

	return p;
}

void PoolAllocator::deallocate(void *p) {

	*((void**)p) = _free_list;

	_free_list = (void**)p;

	_used_memory -= _object_size;
	_num_allocations--;
}
