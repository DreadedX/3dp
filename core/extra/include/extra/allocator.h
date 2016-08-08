#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <string>
#include <cassert>

#include "extra/print.h"

class Allocator {

	public:

		Allocator(size_t size, void *start, const char *name) {

			_start           = start;
			_size            = size;
			_name            = name;

			_used_memory     = 0;
			_num_allocations = 0;
		}

		virtual ~Allocator() {

			if(_num_allocations != 0 && _used_memory != 0) {

				print_w("Memory leak detected in %s (%i object, %i bytes)", _name, _num_allocations, _used_memory);
			}

			_start = nullptr;
			_size  = 0;
		}

		virtual void *allocate(size_t size, uint8_t alignment = 4) = 0;
		virtual void deallocate(void *p) = 0;

		void *getStart() const {

			return _start;
		}

		size_t getSize() const {

			return _size;
		}

		size_t getUsedMemory() const {

			return _used_memory;
		}

		size_t getNumAllocations() const {

			return _num_allocations;
		}

	protected:

		void  *_start;
		size_t _size;
		const char *_name;

		size_t _used_memory;
		size_t _num_allocations;
};

class HeapAllocator : public Allocator {

	public:

		HeapAllocator(size_t size, void* start);
		~HeapAllocator();

		void *allocate(size_t size, uint8_t) override {

			return malloc(size);
		}

		void deallocate(void *p) override {

			free(p);
		}

		void clear();

	private:

		HeapAllocator(const HeapAllocator&);
		HeapAllocator &operator=(const HeapAllocator&);

		void *_current_pos;
};

namespace allocator {

	template <class T>
		T *make_new(Allocator &allocator) {

			return new (allocator.allocate(sizeof(T), __alignof(T))) T;
		}

	template <class T>
		T *make_new(Allocator &allocator, const T& t) {

			return new (allocator.allocate(sizeof(T), __alignof(T))) T(t);
		}

	template <class T>
		void make_delete(Allocator &allocator, T& object) {

			object.~T();
			allocator.deallocate(&object);
		}

	template <class T>
		T *make_new_array(Allocator &allocator, size_t length) {

			assert(length != 0);

			uint8_t headerSize = sizeof(size_t)/sizeof(T);

			if(sizeof(size_t)%sizeof(T) > 0) {

				headerSize += 1;
			}

			T *p = ( (T*) allocator.allocate(sizeof(T)*(length + headerSize), __alignof(T)) ) + headerSize;

			*( ((size_t*)p) - 1 ) = length;

			for(size_t i = 0; i < length; i++) {
				new (&p[i]) T;
			}

			return p;
		}

	template <class T>
		void make_delete_array(Allocator &allocator, T* array) {

			assert(array != nullptr);

			size_t length = *( (size_t*)array - 1);

			for(size_t i = 0; i < length; i++) {

				array[i].~T();
			}

			uint8_t headerSize = sizeof(size_t)/sizeof(T);

			if(sizeof(size_t)%sizeof(T) > 0) {

				headerSize += 1;
			}

			allocator.deallocate(array - headerSize);
		}
}

namespace pointer_math {

	inline void *alignForward(void *address, uint8_t alignment) {

		return (void*) ( (reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment-1)) & static_cast<uintptr_t>(~(alignment-1)) );
	}

	inline uint8_t alignForwardAdjustment(const void *address, uint8_t alignment) {

		uint8_t adjustment = alignment - ( reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment-1) );

		if (adjustment == alignment) {

			return 0;
		}

		return adjustment;
	}

	inline uint8_t alignForwardAdjustmentWithHeader(const void *address, uint8_t alignment, uint8_t headerSize) {

		uint8_t adjustment = alignForwardAdjustment(address, alignment);

		uint8_t neededSpace = headerSize;

		if (adjustment < neededSpace) {

			neededSpace -= adjustment;

			adjustment += alignment * (neededSpace / alignment);

			if (neededSpace % alignment > 0) {

				adjustment += alignment;
			}
		}

		return adjustment;
	}

	inline void *add(void *p, size_t x) {

		return (void*)( reinterpret_cast<uintptr_t>(p) + x);
	}

	inline const void *add(const void *p, size_t x) {

		return (const void*)( reinterpret_cast<uintptr_t>(p) + x);
	}

	inline void *subtract(void *p, size_t x) {

		return (void*)( reinterpret_cast<uintptr_t>(p) - x);
	}

	inline const void *subtract(const void *p, size_t x) {

		return (const void*)( reinterpret_cast<uintptr_t>(p) - x);
	}
}

#endif
