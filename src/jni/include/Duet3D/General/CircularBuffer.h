/*
 * CircularBuffer.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_INCLUDE_DUET3D_GENERAL_CIRCULARBUFFER_HPP_
#define JNI_INCLUDE_DUET3D_GENERAL_CIRCULARBUFFER_HPP_

#include "Debug.h"
#include <cstddef>

template <typename T, size_t Size>
class CircularBuffer
{
  public:
	CircularBuffer() : head_(0), tail_(0), full_(false) {}

	void Push(const T& item)
	{
		buffer_[head_] = item; // Copy the item to the buffer
		IncrementIndex(head_);
		if (Full())
		{
			IncrementIndex(tail_);
		}
		full_ = head_ == tail_;
	}

	bool Pop()
	{
		// Handle underflow
		if (Empty())
		{
			return false;
		}

		full_ = false;
		IncrementIndex(tail_);
		return true;
	}

	bool Pop(T& ref)
	{
		// Handle underflow
		if (Empty())
		{
			return false;
		}

		ref = buffer_[tail_]; // Copy the item to the reference
		full_ = false;
		IncrementIndex(tail_);
		return true;
	}

	bool Empty() const { return !Full() && (head_ == tail_); }

	bool Full() const { return full_; }

	void Reset()
	{
		head_ = 0;
		tail_ = 0;
		full_ = false;
	}

	size_t GetFilled() const
	{
		if (Full())
			return Size;
		if (head_ < tail_)
			return (head_ + Size) - tail_;
		return (head_ - tail_);
	}

	size_t GetSize() const { return Size; }

	size_t GetTail() const { return tail_; }
	size_t GetHead() const { return head_; }
	const T& GetItem(const size_t index) const
	{
		if (index >= GetFilled())
		{
			warn("Accessing out of bounds index %d, filled=%d", index, GetFilled());
		}
		size_t i = (tail_ + index) % Size;
		return buffer_[i];
	}

  private:
	size_t NextIndex(size_t index) { return (index + 1) % Size; }
	size_t PreviousIndex(size_t index) { return (index - 1) % Size; }
	size_t IncrementIndex(size_t& index)
	{
		index = (index + 1) % Size;
		return index;
	}

	T buffer_[Size];
	size_t head_;
	size_t tail_;
	bool full_;
};

#endif /* JNI_INCLUDE_DUET3D_GENERAL_CIRCULARBUFFER_HPP_ */
