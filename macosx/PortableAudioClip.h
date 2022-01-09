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

#ifndef _0kx_driver_VectorAudioClip_h
#define _0kx_driver_VectorAudioClip_h

#include "defs.h"
#include "AudioEngine.h"

#include <IOKit/IOLib.h>
//#include <TargetConditionals.h>
#include <libkern/OSTypes.h>
#include <libkern/OSByteOrder.h>

static const float clipMax = 1.0f;
static const float clipMin = -1.0f;

static const float clipPosMul8 = 127.0f;
static const float clipNegMul8 = 128.0f;

static const float clipPosMulDiv8 = 1 / clipPosMul8;
static const float clipNegMulDiv8 = 1 / clipNegMul8;

static const float clipPosMul16 = 32767.0f;
static const float clipNegMul16 = 32768.0f;

static const float clipPosMulDiv16 = 1 / clipPosMul16;
static const float clipNegMulDiv16 = 1 / clipNegMul16;

static const float clipPosMul24 = 8388607.0f;
static const float clipNegMul24 = 8388608.0f;

static const float clipPosMulDiv24 = 1 / clipPosMul24;
static const float clipNegMulDiv24 = 1 / clipNegMul24;

static const float clipPosMul32 = 2147483647.0f;
static const float clipNegMul32 = 2147483648.0f;

static const float clipPosMulDiv32 = 1 / clipPosMul32;
static const float clipNegMulDiv32 = 1 / clipNegMul32;


static inline SInt32 FloatToInt32(const double val){

#if !defined(PPC)
    static const double maxInt32 = 2147483648.0;	// 1 << 31
	static const double round = 128.0;
	static const double max32 = maxInt32 - 1.0 - round;
#endif
    
#if defined(PPC)
	SInt32 i;
	union {
		double	d;
		UInt32	i[2];
	} u;
    
	// fctiw rounds, doesn't truncate towards zero like fctiwz
	__asm__ ("fctiw %0, %1" 
			 /* outputs:  */ : "=f" (u.d) 
			 /* inputs:   */ : "f" (val));
    
	i = u.i[1];
	return i;
	
#elif defined(X86)
	
	if (val >= max32) return 0x7FFFFFFF;
	return (SInt32)val;
    
#else
    
    static const double min32 = -2147483648.0;
    
	if (val >= max32) return 0x7FFFFFFF;
	else if (val <= min32) return 0x80000000;
	return (SInt32)val;
    
#endif
    
}

static inline double fabs(const double val){
    
#if defined(PPC)
	double d;
    
	__asm__ ("fabs %0, %1" 
			 /* outputs:  */ : "=f" (d) 
			 /* inputs:   */ : "f" (val));
    
	return d;
#else
    return val * ((val > 0) - (val < 0));
#endif
    
}

static inline double fmax(const double a, const double b){
	return (a + b + fabs(a - b)) / 2;
}

static inline double fmin(const double a, const double b){
	return (a + b - fabs(a - b)) / 2;
}

static inline double clamp(const double val){
	return fmin(fmax(val, clipMin), clipMax); 
}

#define Float32ToSInt16_optimized Float32ToSInt16_portable
#define Float32ToSInt32_optimized Float32ToSInt32_portable
#define Float32ToSInt16Aligned32_optimized Float32ToSInt16Aligned32_portable

static void Float32ToSInt32_portable( const float* floatMixBuf, SInt32* destBuf, const UInt32 end, const UInt32 start){
    register float inSample;
    register UInt32 sampleIndex;
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = clamp(floatMixBuf[sampleIndex]);
				
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 32-bit samples and then
        // convert to SInt32 and store in the hardware sample buffer
		destBuf[sampleIndex] = (SInt32)correctEndianess32( FloatToInt32( inSample * clipPosMul32 + 128 ) ); //more lightweight
		
		
		//destBuf[sampleIndex] = (SInt32)correctEndianess32( FloatToInt32( inSample * ( (clipPosMul32 * (inSample > 0)) + (clipNegMul32  * (inSample < 0)) ) ) );  //experimental more accurate variant
    }
    
}

static void Float32ToSInt16Aligned32_portable( const float* floatMixBuf, SInt32* destBuf, const UInt32 end, const UInt32 start){
    register float inSample;
    register UInt32 sampleIndex;
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = clamp(floatMixBuf[sampleIndex]);
        
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 32-bit samples and then
        // convert to SInt32 and store in the hardware sample buffer
		
        destBuf[sampleIndex] = (SInt32)correctEndianess32( ((SInt16)(inSample * clipPosMul16)) << 16 ); //no artifacts 16 bit aprrox.
    }
    
}

static void Float32ToSInt16_portable( const float* floatMixBuf, SInt16* destBuf, const UInt32 end, const UInt32 start){
    
    register float inSample;
    register UInt32 sampleIndex;
    
    // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
    for (sampleIndex = start; sampleIndex < end; sampleIndex++){
        // Fetch the floating point mix sample
        inSample = clamp(floatMixBuf[sampleIndex]);
        
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 32-bit samples and then
        // convert to SInt32 and store in the hardware sample buffer
        destBuf[sampleIndex] = (SInt16)correctEndianess16((UInt16)(inSample * clipPosMul16));
    }
}

#endif
