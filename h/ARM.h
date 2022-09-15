//
//  ARM.h
//  10kx driver
//
//  Created by ITzTravelInTime on 11/05/22.
//  Copyright © 2022 kX Project. All rights reserved.
//

#ifndef ARM_h
#define ARM_h

#if(defined(__ARM__) || defined(__arm__) || defined(_ARCH_ARM) || defined(_ARCH_ARM64) || defined(__aarch64e__) || defined(__arm) || defined(__arm64e__) || defined(__aarch64__))
#define ARM
#define ARCH_DETECTED
#endif

// Basic kX-related type definitions
// these are architecture-dependent:

#if defined(ARM)
#if defined(__GNUC__)
typedef unsigned int dword;
#else
typedef unsigned long dword;
#endif

typedef unsigned short word;
typedef unsigned char  byte;

#if defined(__GNUC__)
    typedef long long __int64;
#endif

typedef unsigned long uintptr_t;

#ifdef KX_INTERNAL

#ifndef SYSTEM_IO
    #define SYSTEM_IO
#endif
//#define OLD_ALLOC

#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE 16384
#endif

#endif
#endif /* ARM_h */
