//
//  ARM.h
//  10kx driver
//
//  Created by ITzTravelInTime on 11/05/22.
//  Copyright © 2022 kX Project. All rights reserved.
//

#ifndef ARM_h
#define ARM_h

// Basic kX-related type definitions
// these are architecture-dependent:

typedef unsigned long  dword;
typedef unsigned short word;
typedef unsigned char  byte;

#define ARM

#if defined(__GNUC__)
    typedef long long __int64;
#endif

#ifdef KX_INTERNAL

#define SYSTEM_IO
//#define OLD_ALLOC

#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE 16384
#endif

#endif /* ARM_h */
