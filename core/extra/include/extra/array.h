#ifndef ARRAY_H
#define ARRAY_H

#include "extra/typedef.h"
#include <assert.h>

/** @brief Replacement for std::vector
	@todo It is still missing some things */
template <typename T>
class Array {

	public:

		/** @brief Initialize array with default values */
		Array()
			: array_(new T[_DEFAULT_ARRAY_SIZE]()),
			  reserved_size_(_DEFAULT_ARRAY_SIZE),
			  size_(0)
		{}

		/** @brief Initialize array with custom size
			@param n - Size of the array */
		Array(int n)
			: array_(new T[n]()),
			  reserved_size_(n),
			  size_(0)
		{}

		/** @brief Initialize array with custom size and custom size to increment with if the array is full
			@param n - Size of the array
			@param increments - Size to increment array size with */
		Array(int n, int increments)
			: array_(new T[n]()),
			  reserved_size_(n),
			  size_(0),
			  incrementSize_(increments)
		{}

		/** @brief Destructor that deletes array */
		~Array() {
			delete[] array_;
		}

		/** @brief Add object to array
			@param &t Object you want to add to the array */
		void add(const T &t);

		/** @brief Resize the array
			@param n - New reserved size of the array
			@note The new reserved size cannot be smaller than the actual array size */
		void resize(uint n);

		/** @brief Get the array size
			@returns Array size */
		uint size();

		/** @brief Empty the array */
		void clear();

		/** @brief Get the object at a certain index
			@param i - Index of the object */
		T &operator[](uint i);

		typedef T *iterator;

		/** @brief Begin iterator */
		iterator begin() {
			return array_;
		}

		/** @brief End iterator */
		iterator end() {
			return array_ + size_;
		}

	private:

		const uint _DEFAULT_ARRAY_SIZE = 4;

		T *array_;
		uint reserved_size_;
		uint size_;

		uint incrementSize_ = _DEFAULT_ARRAY_SIZE;
};

#pragma GCC push_options
#pragma GCC optimize ("O3")

template<typename T>
void Array<T>::add(const T &t) {

	if (size_ == reserved_size_) {

		resize(reserved_size_ + incrementSize_);
	}

	array_[size_] = t;

	size_++;
}

template<typename T>
void Array<T>::resize(uint n) {

	assert(n >= size_ && "New size is smaller than array size, data will be lost");

	T *new_array = new T[n];
	for (uint i = 0; i < size_; i++) {

		new_array[i] = array_[i];
	}

	delete[] array_;
	array_ = new_array;
	reserved_size_ = n;
}

template<typename T>
uint Array<T>::size() {

	return size_;
}

template<typename T>
void Array<T>::clear() {

	size_ = 0;
	resize(_DEFAULT_ARRAY_SIZE);

}

template<typename T>
T &Array<T>::operator[](uint i) {

	assert(i < size_ && "Index is outside of array bounds");

	return array_[i];
}

#pragma GCC pop_options

#endif

