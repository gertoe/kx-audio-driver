// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#ifndef DEFS_H_
#define DEFS_H_

//#if defined(i386) || defined(I386) || defined(IX86) || defined(__I386__) || defined(_IX86) || defined(_M_IX86) || defined(AMD64) || defined(__x86_64__) || defined(__i386__)
#include "i386.h"
//#elif defined(__PPC__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__powerpc__)
#include "PPC.h"
//#elif defined(__ARM__) || defined(__arm__) || defined(_ARCH_ARM) || defined(_ARCH_ARM64) || defined(__aarch64e__) || defined(__arm) || defined(__arm64e__) || defined(__aarch64__)
#include "ARM.h"
//#else
#if !defined(ARCH_DETECTED)
    #error "Unknown processor architecture"
#endif

#if defined(SYSTEM_IO) && !(defined(__x86_64__) || defined(__i386__) || defined(X86))
typedef volatile uintptr_t io_port_t; //volatile char*
#else
typedef word io_port_t;
#endif

__int64 correctEndianess64(const __int64 number);
dword correctEndianess32(const dword number);
word  correctEndianess16(const word  number);

//void writeLE64(__int64* addr, const __int64 data);
void writeLE32(dword* addr, const dword data);
void writeLE16(word* addr, const word data);

//__int64 readLE64(const __int64* addr);
dword readLE32(const dword* addr);
word readLE16(const word* addr);

static inline char itoax_s(dword v)
//hex string conversion
{
    if(v>=10)
    {
        return ((char)v-10)+'a';
    }
    else
        return (char)v+'0';
}

static inline void itoax(char *str, const uintptr_t vall, const byte len)
{
    dword val = (dword)vall;// & 0xffff;
    
    dword mask = 0xf << 4 * (len - 1);
    dword shift = 4 * (len - 1);
    
    for(byte i = 0; i < len; i++){
        *str++=itoax_s((val&mask)>>shift);
        shift -= 4;
        mask = mask >> 4;
    }
    
    /*
     *str=itoax_s((val&0xf000)>>12); str++;
     *str=itoax_s((val&0xf00)>>8); str++;
     *str=itoax_s((val&0xf0)>>4); str++;
     *str=itoax_s((val&0xf)); str++;
     */
    
}

#if defined(_MSC_VER)

   #if !defined(_W64)
    #if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
     #define _W64 __w64
    #else
     #define _W64
    #endif
   #endif

   #ifndef _INTPTR_T_DEFINED
    #ifdef  _WIN64
     typedef __int64             intptr_t;
    #else
     typedef _W64 int            intptr_t;
    #endif
    #define _INTPTR_T_DEFINED
   #endif

#elif defined(__GNUC__)

 //#define __int64  long long

 typedef long			__darwin_intptr_t;
 #ifndef _INTPTR_T
 #define _INTPTR_T
 typedef __darwin_intptr_t	intptr_t;
 #endif

#else
 #error "Unknown compiler"
#endif

#ifndef NULL
 #define NULL 0
#endif

#endif
