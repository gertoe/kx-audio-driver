//
//  PPC.h
//  10kx driver
//
//  Created by Pietro Caruso on 26/02/21.
//  Copyright © 2021 kX Project.
//

#ifndef PPC_h
#define PPC_h

// Basic kX-related type definitions
// these are architecture-dependent:

typedef unsigned long  dword;
typedef unsigned short word;
typedef unsigned char  byte;

#define PPC

#if defined(__GNUC__)
    typedef long long __int64;
#endif

typedef unsigned long uintptr_t;

#ifdef KX_INTERNAL

#define SYSTEM_IO
//#define OLD_ALLOC

#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE     4096
#endif


#endif /* PPC_h */
