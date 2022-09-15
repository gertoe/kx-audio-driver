//
//  PPC.h
//  10kx driver
//
//  Created by Pietro Caruso on 26/02/21.
//  Copyright © 2021 kX Project.
//

#ifndef PPC_h
#define PPC_h

#if (defined(__PPC__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__powerpc__))
#define PPC
#define ARCH_DETECTED
#endif

// Basic kX-related type definitions
// these are architecture-dependent:

#if defined(PPC)
typedef unsigned long  dword;
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
 #define PAGE_SIZE     4096
#endif

#endif
#endif /* PPC_h */
