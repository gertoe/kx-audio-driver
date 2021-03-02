//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
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


#include "AudioDevice.h"
#include "AudioEngine.h"

#if defined(KX_LIBLESS)

static const float clipMax = 1.0f;
static const float clipMin = -1.0f;

/*
static const float _INT_MIN  = 2147483648.0f;
static const float _INT_MAX  = 2147483647.0f;
static const float INT_MINDIV = (1 / INT_MIN);
static const float INT_MAXDIV = (1 / INT_MAX);
*/
 
static const float clipPosMul16 = 32767.0f;
static const float clipNegMul16 = 32768.0f;

static const float clipPosMulDiv16 = 1 / clipPosMul16;
static const float clipNegMulDiv16 = 1 / clipNegMul16;

static const float clipPosMul32 = 2147483648.0f;
static const float clipNegMul32 = 2147483647.0f;

static const float clipPosMulDiv32 = 1 / clipPosMul32;
static const float clipNegMulDiv32 = 1 / clipNegMul32;

#ifdef __BIG_ENDIAN__
//#if 0
//#warning Experimental code

static UInt32 littleToBig32(const UInt32 num){
    
    UInt32 res = 0;
    
    res |= (num & 0xff000000) >> 24;
    res |= (num & 0x00ff0000) >> 8 ;
    res |= (num & 0x0000ff00) << 8 ;
    res |= (num & 0x000000ff) << 24;
    
    return res;
    
}

#define correctEndian32(num) (littleToBig32(num))


static UInt16 littleToBig16(const UInt16 num){
    
    UInt16 res = 0;
    
    res |= (num & 0xff00) >> 8 ;
    res |= (num & 0x00ff) << 8 ;
    
    return res;
    
}

#define correctEndian16(num) (littleToBig16(num))

#else

#define correctEndian32(num) (num)
#define correctEndian16(num) (num)

#endif

#else

#include "IOAudioBlitterLib.h"

#endif

#undef debug
#include "cedebug.h"

#ifndef KX_INTERNAL
	#error invalid configuration
#endif

IOReturn kXAudioEngine::clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
	// just some checks
	#ifdef DEBUGGING
	int i;
	int n_voice=-1;
	int sz=-1;
	for(i=0;i<KX_NUMBER_OF_VOICES;i++)
	{
		if(hw->voicetable[i].buffer.addr==sampleBuf)
		{
			sz=hw->voicetable[i].buffer.size;
			n_voice=i;
			break;
		}
	}
	if(i==KX_NUMBER_OF_VOICES)
	{
		debug("!! kXAudioEngine[%p]::clipOutputSamples: invalid buffer [%08x]\n",this,sampleBuf);
		return 0;
	}
	if(firstSampleFrame+numSampleFrames>n_frames || (firstSampleFrame+numSampleFrames)*bps/8>sz)
	{
		debug("!! kXAudioEngine[%p]::clipOutputSamples: invalid offset [buf: %d, %08x first: %d num: %d [%04x, %04x]\n",this,i,
			  sampleBuf,
			  firstSampleFrame,numSampleFrames,
			  firstSampleFrame*1*bps/8,
			  (firstSampleFrame+numSampleFrames)*1*bps/8);
		return 0;
	}
	#endif
	
    #if !defined(KX_LIBLESS)
	int frsamples=firstSampleFrame * streamFormat->fNumChannels;
    UInt8 *outputBuf = &(((UInt8 *)sampleBuf)[frsamples * streamFormat->fBitWidth / 8]);
	float *fMixBuf = ((float *)mixBuf) + frsamples;
    
    // Calculate the number of actual samples to convert
    int num_samples = numSampleFrames * streamFormat->fNumChannels;

	
	if(bps==32)
		Float32ToNativeInt32_X86(fMixBuf, (SInt32 *)outputBuf, num_samples);
	else
		if(bps==16)
			Float32ToNativeInt16_X86(fMixBuf, (SInt16 *)outputBuf, num_samples);
    
    #else
    
    UInt32 sampleIndex, maxSampleIndex;
    float *floatMixBuf;
    float inSample;
    
    // Start by casting the void * mix and sample buffers to the appropriate types - float * for the mix buffer
    // and SInt32 * for the sample buffer (because our sample hardware uses signed 32-bit samples)
    floatMixBuf = (float *)mixBuf;
    
    // We calculate the maximum sample index we are going to clip and convert
    // This is an index into the entire sample and mix buffers
    maxSampleIndex = (firstSampleFrame + numSampleFrames) * streamFormat->fNumChannels;
    
    if (bps==32){
        SInt32 *outputSInt32Buf = (SInt32 *)sampleBuf;
        
        // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
        for (sampleIndex = (firstSampleFrame * streamFormat->fNumChannels); sampleIndex < maxSampleIndex; sampleIndex++){
            // Fetch the floating point mix sample
            inSample = floatMixBuf[sampleIndex];
            
            // Clip that sample to a range of -1.0 to 1.0
            // A softer clipping operation could be done here
            if (inSample > clipMax)
            {
                inSample = clipMax;
            }
            else if (inSample < clipMin)
            {
                inSample = clipMin;
            }
            
            // Scale the -1.0 to 1.0 range to the appropriate scale for signed 32-bit samples and then
            // convert to SInt32 and store in the hardware sample buffer
            
            
            
            if (inSample >= 0)
            {
                outputSInt32Buf[sampleIndex] = (SInt32)correctEndian32(inSample * clipPosMul32);
            }
            else
            {
                outputSInt32Buf[sampleIndex] = (SInt32)correctEndian32(inSample * clipNegMul32);
            }
        }
        
    }else if (bps ==16){
        SInt16 *outputSInt16Buf = (SInt16 *)sampleBuf;
        
        // Loop through the mix/sample buffers one sample at a time and perform the clip and conversion operations
        for (sampleIndex = (firstSampleFrame * streamFormat->fNumChannels); sampleIndex < maxSampleIndex; sampleIndex++){
            // Fetch the floating point mix sample
            inSample = floatMixBuf[sampleIndex];
            
            // Clip that sample to a range of -1.0 to 1.0
            // A softer clipping operation could be done here
            
            if (inSample > clipMax) {
                inSample = clipMax;
            } else if (inSample < clipMin) {
                inSample = clipMin;
            }
            
            // Scale the -1.0 to 1.0 range to the appropriate scale for signed 16-bit samples and then
            // convert to SInt16 and store in the hardware sample buffer
            
            //outputSInt16Buf[sampleIndex] = ((SInt16) (inSample * ((inSample >= 0) ? clipPosMul : clipNegMul)));
            //outputSInt16Buf[sampleIndex] = (SInt16)correctEndian16((UInt16)outputSInt16Buf[sampleIndex]);
            
            if (inSample >= 0)
            {
                outputSInt16Buf[sampleIndex] = (SInt16)correctEndian16(inSample * clipPosMul16);
            }
            else
            {
                outputSInt16Buf[sampleIndex] = (SInt16)correctEndian16(inSample * clipNegMul16);
            }
            
            /*
             if (inSample >= 0) {
             outputSInt16Buf[sampleIndex] = (SInt16) (inSample * clipPosMul);
             } else {
             outputSInt16Buf[sampleIndex] = (SInt16) (inSample * clipNegMul);
             }
             */
        }
        
    }
#endif
    
    return kIOReturnSuccess;
}

IOReturn kXAudioEngine::convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
	#ifdef DEBUGGING
	 // just some checks
	int i;
	int sz=-1;
	for(i=0;i<KX_NUMBER_OF_VOICES;i++)
	{
		if(hw->voicetable[i].buffer.addr==sampleBuf)
		{
			sz=hw->voicetable[i].buffer.size;
			break;
		}
	}
	if(i==KX_NUMBER_OF_VOICES)
	{
		debug("!! kXAudioEngine[%p]::convertInputSamples: invalid buffer [%08x] audio_stream: %08x stream_format: %08x\n",this,sampleBuf,audioStream,streamFormat);
		return 0;
	}
	
	if(firstSampleFrame+numSampleFrames>n_frames || (firstSampleFrame+numSampleFrames)*bps/8>sz)
	{
		debug("!! kXAudioEngine[%p]::convertInputSamples: invalid offset [%08x first: %d num: %d [%04x, %04x]\n",this,
			  sampleBuf,
			  firstSampleFrame,numSampleFrames,
			  firstSampleFrame*1*bps/8,
			  (firstSampleFrame+numSampleFrames)*1*bps/8);
		return 0;
	}
	#endif
	

#if !defined(KX_LIBLESS)
    
    int num_samples = numSampleFrames * streamFormat->fNumChannels;
    UInt8 *inputBuf = &(((UInt8 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels * streamFormat->fBitWidth / 8]);
    
	if(bps==32)
		NativeInt32ToFloat32_Portable((SInt32 *)inputBuf,(Float32 *)destBuf,num_samples);
	else
		if(bps==16)
			NativeInt16ToFloat32_Portable((SInt16 *)inputBuf,(Float32 *)destBuf,num_samples);
#else
    
    UInt32 numSamplesLeft;
    float *floatDestBuf;
    
    // Calculate the number of actual samples to convert
    numSamplesLeft = numSampleFrames * streamFormat->fNumChannels;
    
    // Start by casting the destination buffer to a float *
    floatDestBuf = (float *)destBuf;
    
    if(bps==32){
        SInt32 *inputBuf;
        SInt32 inputSample;
    
    
        // Determine the starting point for our input conversion
        inputBuf = &(((SInt32 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels]);
    
        // Loop through each sample and scale and convert them
        for (UInt32 i = 0; i < numSamplesLeft; i++) {
            // Fetch the SInt32 input sample
            inputSample = *inputBuf;
        
            // Scale that sample to a range of -1.0 to 1.0, convert to float and store in the destination buffer
            // at the proper location
            if (inputSample >= 0) {
                *floatDestBuf = (SInt32)correctEndian32((UInt32)(inputSample * clipPosMulDiv32));
            } else {
                *floatDestBuf = (SInt32)correctEndian32((UInt32)(inputSample * clipNegMulDiv32));
            }
        
            // Move on to the next sample
            ++inputBuf;
            ++floatDestBuf;
        }
        
    }else if(bps==16){
        SInt16 *inputBuf;
        SInt16 inputSample;
        
        // Determine the starting point for our input conversion
        inputBuf = &(((SInt16 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels]);
        
        // Loop through each sample and scale and convert them
        for (UInt32 i = 0; i < numSamplesLeft; i++) {
            
            // Fetch the SInt16 input sample
            inputSample = *inputBuf;
            
            // Scale that sample to a range of -1.0 to 1.0, convert to float and store in the destination buffer
            // at the proper location
            if (inputSample >= 0) {
                (*floatDestBuf) = (SInt16)correctEndian16(inputSample * clipPosMulDiv16);// / 32767.0;
            } else {
                (*floatDestBuf) = (SInt16)correctEndian16(inputSample * clipNegMulDiv16);// / 32768.0;
            }
            
            // Move on to the next sample
            ++inputBuf;
            ++floatDestBuf;
        }
    }

    
	#endif
    return kIOReturnSuccess;
}
