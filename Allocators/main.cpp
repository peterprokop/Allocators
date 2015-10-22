//
//  main.cpp
//  Allocators
//
//  Created by Peter Prokop on 14/10/15.
//  Copyright © 2015 Peter Prokop. All rights reserved.
//

#include <iostream>
#include "allocators.hpp"


using Stackalloc = StackAllocator<16384>;
using Flist = Freelist<Stackalloc, sizeof(int)>;
using FlistOnMalloc = Freelist<Mallocator, sizeof(int)>;
using BucketizerOnStack = Bucketizer<Stackalloc, 1, 128, 16>;

#define TEST_ITERATION_COUNT 100

void profileStackAllocVSMalloc() {
    clock_t start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            int* ptr = (int*) malloc(sizeof(int));
            free(ptr);
        }
    }
    
    clock_t end = clock() ;
    double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileStackAllocVSMalloc malloc %f \n", elapsed_time);
    
    Stackalloc a;
    
    start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            auto b = a.allocate(sizeof(int));
            a.deallocate(b);
        }
    }
    
    end = clock() ;
    elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileStackAllocVSMalloc StackAllocator %f \n", elapsed_time);
}

void profileFlistVSMalloc() {
    clock_t start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            int* ptr = (int*) malloc(sizeof(int));
            free(ptr);
        }
    }
    
    clock_t end = clock() ;
    double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileFlistVSMalloc malloc %f \n", elapsed_time);
    
    Flist a;
    
    start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            auto b = a.allocate(sizeof(int));
            a.deallocate(b);
        }
    }
    
    end = clock() ;
    elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileFlistVSMalloc Flist %f \n", elapsed_time);
}

void profileBucketizerOnStackVSMalloc() {
    clock_t start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            int* ptr = (int*) malloc(sizeof(int));
            free(ptr);
        }
    }
    
    clock_t end = clock() ;
    double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileBucketizerOnStackVSMalloc malloc %f \n", elapsed_time);
    
    BucketizerOnStack a;
    
    start = clock() ;
    
    for (int j = 0; j < TEST_ITERATION_COUNT; ++j) {
        for (int i = 0; i < 128; ++i) {
            auto b = a.allocate(sizeof(int));
            a.deallocate(b);
        }
    }
    
    end = clock() ;
    elapsed_time = (end-start)/(double)CLOCKS_PER_SEC/TEST_ITERATION_COUNT;
    
    printf("profileBucketizerOnStackVSMalloc BucketizerOnStack %f \n", elapsed_time);
}

int main(int argc, const char * argv[]) {
    Stackalloc a;
    auto b = a.allocate(128 * sizeof(int));
    int* p = static_cast<int *>(b.ptr);
    for (int i = 0; i < 128; ++i) {
        p[i] = i;
    }
    printf("===\n");
    for (int i = 0; i < 128; ++i) {
        printf("%i\n", p[i]);
    }
    
    a.deallocate(b);
    
    
    // Test - alloc & dealloc
    AllocatorBlock pointers[128];
    for (int i = 0; i < 128; ++i) {
        auto b = a.allocate(sizeof(int));
        pointers[i] = b;
        printf("pointer: %p\n", a.p_);
    }
    printf("Dealloc:\n");
    for (int i = 127; i >= 0; --i) {
        a.deallocate(pointers[i]);
        printf("pointer: %p\n", a.p_);
    }
    
    // Test - alloc & dealloc
    FlistOnMalloc fom;
    AllocatorBlock bonsPointers[16];
    for (int i = 0; i < 16; ++i) {
        auto b = fom.allocate(i * sizeof(int));
        bonsPointers[i] = b;
        printf("pointer: %p\n", b.ptr);
    }
    printf("Dealloc:\n");
    for (int i = 15; i >= 0; --i) {
        fom.deallocate(bonsPointers[i]);
    }
    
    profileStackAllocVSMalloc();
    profileFlistVSMalloc();
    profileBucketizerOnStackVSMalloc();
    
    return 0;
}


