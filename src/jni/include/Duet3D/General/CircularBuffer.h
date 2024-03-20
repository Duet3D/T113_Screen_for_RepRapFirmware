/*
 * CircularBuffer.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_INCLUDE_DUET3D_GENERAL_CIRCULARBUFFER_HPP_
#define JNI_INCLUDE_DUET3D_GENERAL_CIRCULARBUFFER_HPP_

#include <cstddef>

template <typename T, size_t Size>
class CircularBuffer {
public:
    CircularBuffer() : head_(0), tail_(0), full_(false) {}

    void Push(const T& item) {
        buffer_[head_] = item;
        if (full_) {
            IncrementIndex(tail_);
        }
        if (head_ == Size - 1)
            full_ = true;
        IncrementIndex(head_);
    }

    bool Pop(T& ref) {
        if (Empty()) {
            // Handle underflow
            return false;
        }

        ref = buffer_[tail_];
        tail_ = (tail_ + 1) % Size;
        full_ = false;
        return true;
    }

    bool Empty() const {
        return !full_ && (head_ == tail_);
    }

    bool Full() const {
        return full_;
    }

    void Reset()
    {
    	full_ = false;
    	head_ = 0;
    	tail_ = 0;
    }

    size_t GetSize() const {
        return Size;
    }
    
    size_t GetTail() const { return tail_; }
    size_t GetHead() const { return head_; }
    const T& GetItem(const size_t index) const
    {
    	size_t i = (tail_ + index) % Size;
    	return buffer_[i];
    }

private:
    size_t NextIndex(size_t index) { return (index + 1) % Size; }
    size_t PreviousIndex(size_t index) { return (index - 1) % Size; }
    size_t IncrementIndex(size_t &index)
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
