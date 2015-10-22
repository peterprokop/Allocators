//
//  allocators.hpp
//  Allocators
//
//  Created by Peter Prokop on 22/10/15.
//  Copyright © 2015 Peter Prokop. All rights reserved.
//

#ifndef allocators_hpp
#define allocators_hpp

#include <stdio.h>
#include <iostream>

#define ALIGN_SIZE sizeof(double)

struct AllocatorBlock {
    void* ptr;
    size_t length;
};

template <class Primary, class Fallback>
class FallbackAllocator
: private Primary
, private Fallback {
public:
    AllocatorBlock allocate(size_t);
    void deallocate(AllocatorBlock);
    bool owns(AllocatorBlock);
};


size_t roundToAligned(size_t n);

/**
 Stack allocator
 */
template <size_t s> class StackAllocator {
    
public:
    char d_[s];
    char* p_;
    
    StackAllocator() : p_(d_) {}
    
    AllocatorBlock allocate(size_t n) {
        auto n1 = roundToAligned(n);
        if (n1 > d_ + s - p_) {
            return {nullptr, 0};
        }
        AllocatorBlock result = {p_, n};
        p_ += n1;
        return result;
    }
    
    void deallocate(AllocatorBlock b) {
        if ((char*)b.ptr + roundToAligned(b.length) == p_) {
            p_ = (char*)b.ptr;
        }
    }
    
    bool owns(AllocatorBlock b) {
        return b.ptr >= d_ && b.ptr < d_ + s;
    }
    
    void deallocateAll() {
        p_ = d_;
    }
};

/**
 Freelist
 */

template <class A, size_t s> class Freelist {
    A parent_;
    struct Node { Node* next; };
    Node* root_ = nullptr;
public:
    AllocatorBlock allocate(size_t n) {
        if (n == s && root_) {
            AllocatorBlock b = { root_, n };
            root_ = root_->next;
            return b;
        } else {
            return parent_.allocate(n);
        }
    }
    
    bool owns(AllocatorBlock b) {
        return b.length == s || parent_.owns(b);
    }
    
    void deallocate(AllocatorBlock b) {
        if (b.length != s) {
            parent_.deallocate(b);
            return;
        } else {
            auto p = (Node*)b.ptr;
            p->next = root_;
            root_ = p;
        }
    }
    
    /**
     TODO:
     - Add tolerance: min...max
     - Allocate in batches
     - Add upper bound: no more than top elements
     */
    
};

/**
 Mallocator
 */

class Mallocator {
public:
    AllocatorBlock allocate(size_t n) {
        void* ptr = malloc(n);
        return {ptr, n};
    }
    
    void deallocate(AllocatorBlock b) {
        free(b.ptr);
    }
};

/**
 Bucketizer
 */
template <class Allocator,
size_t min,
size_t max,
size_t step>
class Bucketizer {
    Allocator parent_;
    
public:
    AllocatorBlock allocate(size_t n) {
        if (n > max) {
            return {nullptr, 0};
        } if ( (n - min) % step == 0) {
            return parent_.allocate(n);
        } else {
            auto delta = (n - min)/step + 1;
            return parent_.allocate(min + delta * step);
        }
    }
    
    bool owns(AllocatorBlock b) {
        return (b.length >= min && b.length <= max && (b.length - min) % step == 0) ||
        parent_.owns(b);
    }
    
    void deallocate(AllocatorBlock b) {
        parent_.deallocate(b);
    }
};

#endif /* allocators_hpp */
