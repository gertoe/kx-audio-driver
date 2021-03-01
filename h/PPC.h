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

typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

#if defined(__GNUC__)
    typedef long long __int64;
#endif

#ifdef KX_INTERNAL
 #ifndef CONIO_USAGE

#if defined(__GNUC__) // _MSC_VER?


//TODO: try to implement this stuff for PPC
extern __inline__ dword inpd(word port)
{
    unsigned long datum = 0;
    //__asm__ volatile("inl %w1, %0" : "=a" (datum) : "Nd" (port));
    return(datum);
}

extern __inline__ word inpw(word port)
{
    unsigned short datum = 0;
    //__asm__ volatile("inw %w1, %w0" : "=a" (datum) : "Nd" (port));
    return(datum);
}

extern __inline__ byte inp(word port)
{
    unsigned char datum = 0;
    //__asm__ volatile("inb %w1, %b0" : "=a" (datum) : "Nd" (port));
    return(datum);
}

extern __inline__ void outpd(word port,
                             unsigned long value)
{
    // note: used to be 'dword', but this conflicts with x64 build in 10.6/10.7
    //__asm__ volatile("outl %0, %w1" : : "a" (value), "Nd" (port));
}

extern __inline__ void outpw(word port,
                             word value)
{
    //__asm__ volatile("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

extern __inline__ void outp(word port,
                            byte value)
{
    //__asm__ volatile("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

#endif // __GNUC__
#endif // CONIO_USAGE
#endif // KX_INTERNAL

#ifndef PAGE_SIZE
 #define PAGE_SIZE     4096
#endif


#endif /* PPC_h */
