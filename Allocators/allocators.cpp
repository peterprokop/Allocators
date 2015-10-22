//
//  allocators.cpp
//  Allocators
//
//  Created by Peter Prokop on 22/10/15.
//  Copyright © 2015 Peter Prokop. All rights reserved.
//

#include "allocators.hpp"

template <class P, class F>
AllocatorBlock FallbackAllocator<P, F>::allocate(size_t n) {
    AllocatorBlock r = P::allocate(n);
    if (!r.ptr) {
        r = F::allocate(n);
    }
    return r;
}

template <class P, class F>
void FallbackAllocator<P, F>::deallocate(AllocatorBlock b) {
    if (P::owns(b)) {
        P::deallocate(b);
    } else {
        F::deallocate(b);
    }
    
}

template <class P, class F>
bool FallbackAllocator<P, F>::owns(AllocatorBlock b) {
    return P::owns(b) || F::owns(b);
}

size_t roundToAligned(size_t n) {
    auto floor = (n / ALIGN_SIZE) * ALIGN_SIZE;
    
    if (floor == n) {
        return floor;
    } else {
        return floor + ALIGN_SIZE;
    }
}