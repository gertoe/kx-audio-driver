//
//  File created by Pietro Caruso.
//
//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//    Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
//  https://github.com/kxproject/ (previously www.kxproject.com)

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

#ifndef _10kx_driver_VectorAudioClip_h
#define _10kx_driver_VectorAudioClip_h

#include "defs.h"
#include "AudioEngine.h"

#include <IOKit/IOLib.h>
//#include <TargetConditionals.h>
#include <libkern/OSTypes.h>
#include <libkern/OSByteOrder.h>

#define NO_ASM

static const double clipMax = 1.0f;
static const double clipMin = -1.0f;

static const double clipPosMul8 = 127.0f;
static const double clipNegMul8 = 128.0f;

static const double clipPosMulDiv8 = 1 / clipPosMul8;
static const double clipNegMulDiv8 = 1 / clipNegMul8;

static const double clipPosMul16 = 32767.0f;
static const double clipNegMul16 = 32768.0f;

static const double clipPosMulDiv16 = 1 / clipPosMul16;
static const double clipNegMulDiv16 = 1 / clipNegMul16;

static const double clipPosMul24 = 8388607.0f;
static const double clipNegMul24 = 8388608.0f;

static const double clipPosMulDiv24 = 1 / clipPosMul24;
static const double clipNegMulDiv24 = 1 / clipNegMul24;

static const double clipPosMul32 = 2147483647.0f;
static const double clipNegMul32 = 2147483648.0f;

static const double clipPosMulDiv32 = 1 / clipPosMul32;
static const double clipNegMulDiv32 = 1 / clipNegMul32;

static inline SInt32 FloatToInt32(const register double val){
    
#if !defined(PPC) || defined(NO_ASM)
    static const double maxInt32 = 2147483648.0;    // 1 << 31
    static const double round = 128.0;
    static const double max32 = maxInt32 - 1.0 - round;
#endif
    
#if defined(PPC) && !defined(NO_ASM)
    register SInt32 i;
    register union {
        double    d;
        UInt32    i[2];
    } u;
    
    // fctiw rounds, doesn't truncate towards zero like fctiwz
    __asm__ ("fctiw %0, %1"
             /* outputs:  */ : "=f" (u.d)
             /* inputs:   */ : "f" (val));
    
    i = u.i[1];
    return i;
    
#elif defined(X86)
    
     if (val >= max32) return 0x7FFFFFFF;
     return (SInt32)val; //the compiler handles it better on x86, hence the missing check compared to the generic version
    
    
    //return ((val >= max32) * 0x7FFFFFFF) + ((val < max32) * ((SInt32)val)); //branchless, so the cpu is happy
#else
    //Generic slow version
    static const double min32 = -2147483648.0;
    
    
     if (val >= max32) return 0x7FFFFFFF;
     else if (val <= min32) return 0x80000000;
     return (SInt32)val;
     
    //return ((val >= max32) * 0x7FFFFFFF) + ((val <= min32) * 0x80000000) + ((val < max32 && val > min32) * (SInt32)val);
    
#endif
    
}

static inline double fabs(const register double val){
	
#if defined(PPC) && !defined(NO_ASM)
    double d;
    
    __asm__ volatile ("fabs %0, %1"
					  : "=f" (d)
					  : "f" (val));
    
    return d;
#else
    return val * ((val > 0) - (val < 0));
#endif
    
}

static inline double fmax(const register double a, const register double b){
    return (a + b + fabs(a - b)) / 2;
}

static inline double fmin(const register double a, const register double b){
    return (a + b - fabs(a - b)) / 2;
}

static inline double clamp(const register double val){
    return fmin(fmax(val, clipMin), clipMax);
}


static void Float32ToSInt32_portable( const float* floatMixBuf, SInt32* destBuf, const UInt32 end, const UInt32 start){
    register double inSample;
    register UInt32 sampleIndex;
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = (clamp(floatMixBuf[sampleIndex]) * clipPosMul32) + 128.0f;
                
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 32-bit samples and then
        // convert to SInt32 and store in the hardware sample buffer
        //destBuf[sampleIndex] = (SInt32)correctEndianess32( FloatToInt32( inSample ) ); //more lightweight
        
        //writeLE32((UInt32*)&(destBuf[sampleIndex]), FloatToInt32( inSample ));
        OSWriteLittleInt32(destBuf, sampleIndex * sizeof(*destBuf), FloatToInt32(inSample)); //better performance since we avoid using the shifter this way
    }
    
}

static void Float32ToSInt16Aligned32_portable( const float* floatMixBuf, SInt32* destBuf, const UInt32 end, const UInt32 start){
    register double inSample;
    register UInt32 sampleIndex;
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = (clamp(floatMixBuf[sampleIndex]) * clipPosMul16);
        
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 16-bit samples and then
        // convert to SInt16, then shifts it up by 16 bits (to match the 32 bit range for the buffer) and store in the hardware sample buffer
        
        //destBuf[sampleIndex] = (SInt32)correctEndianess32( ((SInt16)inSample) << 16 ); //no artifacts 16 bit aprrox.
        
        
        //writeLE32((UInt32*)&(destBuf[sampleIndex]), ((SInt16)inSample) << 16 );
        OSWriteLittleInt32(destBuf, sampleIndex * sizeof(*destBuf), ((SInt16)inSample) << 16);
    }
    
}

static void Float32ToSInt16_portable( const float* floatMixBuf, SInt16* destBuf, const UInt32 end, const UInt32 start){
    
    register double inSample;
    register UInt32 sampleIndex;
    //register UInt16* bufferPointer = (UInt16*)&(destBuf[start]);
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = (clamp(floatMixBuf[sampleIndex]) * clipPosMul16);
        
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 16-bit samples and then
        // convert to SInt16 and store in the hardware sample buffer
        //destBuf[sampleIndex] = (SInt16)correctEndianess16((UInt16)(inSample));
        
        //writeLE16((UInt16*)&(destBuf[sampleIndex]), ((SInt16)inSample));
        OSWriteLittleInt16(destBuf, sampleIndex * sizeof(*destBuf), (SInt16)inSample);
    }
}

#define Float32ToSInt16_optimized Float32ToSInt16_portable
#define Float32ToSInt32_optimized Float32ToSInt32_portable
#define Float32ToSInt16Aligned32_optimized Float32ToSInt16Aligned32_portable

#endif
