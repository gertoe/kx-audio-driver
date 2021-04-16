//
//  PPC.h
//  10kx driver
//
//  Created by Pietro Caruso on 26/02/21.
//  Copyright © 2021 kX Project. All rights reserved.
//

#ifndef PPC_h
#define PPC_h

// Basic kX-related type definitions
// these are architecture-dependent:

typedef unsigned long  dword;
typedef unsigned short word;
typedef unsigned char  byte;

#if defined(__GNUC__)
    typedef long long __int64;
#endif

#ifdef KX_INTERNAL

#define SYSTEM_IO
#define OLD_ALLOC

#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE     512//4096
#endif


#endif /* PPC_h */
