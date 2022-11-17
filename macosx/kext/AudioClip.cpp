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
 
#include <IOKit/IOLib.h>
#include "AudioEngine.h"
 
#if !defined(KX_LIBLESS)
	#if defined(PPC)
		#include "PCMBlitterLibPPC.h"
	#elif defined(X86)
		#include "IOAudioBlitterLib.h"
		#include "IOAudioBlitterLibDispatch.h"
	#else
		#define KX_LIBLESS 1
	#endif
#endif

#if defined(KX_LIBLESS)

#include "PortableAudioClip.h"

#endif

#undef debug
#include "cedebug.h"

#ifndef KX_INTERNAL
	#error invalid configuration
#endif

IOReturn kXAudioEngine::clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
	//debug("kXAudioEngine[%p]::clipOutputSamples\n",this);
	
	// just some checks
	#if defined(DEBUGGING)// && !defined(KX_LIBLESS)
    
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
    
	if(i>=KX_NUMBER_OF_VOICES)
	{
		IOLog("!! kXAudioEngine[%p]::clipOutputSamples: invalid buffer [%p]\n",this,sampleBuf);
		return 0;
	}
    
	if(firstSampleFrame+numSampleFrames>n_frames || ((firstSampleFrame+numSampleFrames)*bps/8>sz))
	{
		IOLog("!! kXAudioEngine[%p]::clipOutputSamples: invalid offset [buf: %d, %p first: %x num: %x [%x, %x]\n",this,i,
			  sampleBuf,
			  firstSampleFrame,numSampleFrames,
			  firstSampleFrame*1*bps/8,
			  (firstSampleFrame+numSampleFrames)*1*bps/8);
		return 0;
	}
    
	#endif
	
#if !defined(KX_LIBLESS)
    
	UInt32 frsamples=firstSampleFrame * streamFormat->fNumChannels;
    UInt8 *outputBuf = &(((UInt8 *)sampleBuf)[frsamples * streamFormat->fBitWidth / 8]);
	float *fMixBuf = ((float *)mixBuf) + frsamples;
    
    // Calculate the number of actual samples to convert
    int num_samples = numSampleFrames * streamFormat->fNumChannels;

	#if !defined(PPC)
    if(bps==32){
		//Float32ToNativeInt32_X86(fMixBuf, (SInt32 *)outputBuf, num_samples);
        IOAF_Float32ToLEInt32(fMixBuf, (SInt32*)outputBuf, num_samples);
    }else if(bps==16){
        //Float32ToNativeInt16_X86(fMixBuf, (SInt16 *)outputBuf, num_samples);
        IOAF_Float32ToLEInt16(fMixBuf, (SInt16 *)outputBuf, num_samples);
    }
    #else
	if(bps==32){
		Float32ToSwapInt32(fMixBuf, (SInt32*)outputBuf, num_samples);
    }else if(bps==16){
		Float32ToSwapInt16(fMixBuf, (SInt16*)outputBuf, num_samples);
    }
	#endif
    
#else
    
    UInt32 startSampleIndex, maxSampleIndex;
    
    maxSampleIndex = (firstSampleFrame + numSampleFrames) * streamFormat->fNumChannels;
    startSampleIndex = (firstSampleFrame * streamFormat->fNumChannels);
    
	
    if (bps==32){
	
	//IOLog("detectted bit depth: %u\n", streamFormat->fBitDepth);
	
	switch(streamFormat->fBitDepth){
		case 8:
			//IOLog("Defaulting to 8 bit playback routines\n");
			//Float32ToSInt8Aligned32_optimized( (const float *)mixBuf, (SInt32 *)sampleBuf, maxSampleIndex, startSampleIndex); //playback noise
			//break;
		case 16:
			//IOLog("Defaulting to 16 bit playback routines\n");
			Float32ToSInt16Aligned32_optimized( (const float *)mixBuf, (SInt32 *)sampleBuf, maxSampleIndex, startSampleIndex);
			break;
		//case 24:
			//IOLog("Defaulting to 24 bit playback routines\n");
			//Float32ToSInt24Unpacked_optimized( (const float *)mixBuf, (SInt32 *)sampleBuf, maxSampleIndex, startSampleIndex); //occasional artifacts
			//break;
		//case 32:
		default:
			//IOLog("Defaulting to 32 bit playback routines\n");
			Float32ToSInt32_optimized( (const float *)mixBuf, (SInt32 *)sampleBuf, maxSampleIndex, startSampleIndex);
			break;
	}
		
	//Float32ToSInt32_optimized( (const float *)mixBuf, (SInt32 *)sampleBuf, maxSampleIndex, startSampleIndex);
		
    }else if (bps ==16){
		//if (streamFormat->fBitDepth == 16){
			Float32ToSInt16_optimized( (const float *)mixBuf, (SInt16*)sampleBuf, maxSampleIndex, startSampleIndex);
		//}else{
		//	Float32ToSInt8Aligned16_optimized( (const float *)mixBuf, (SInt16*)sampleBuf, maxSampleIndex, startSampleIndex);
		//}
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
    
	#if !defined(PPC)
    if(bps==32){
		NativeInt32ToFloat32_X86((SInt32 *)inputBuf,(Float32 *)destBuf,num_samples);
        //IOAF_LEInt32ToFloat32((SInt32 *)inputBuf,(Float32 *)destBuf,num_samples);
    }else if(bps==16){
        NativeInt16ToFloat32_X86((SInt16 *)inputBuf,(Float32 *)destBuf,num_samples);
        //IOAF_LEInt16ToFloat32((SInt16 *)inputBuf,(Float32 *)destBuf,num_samples);
    }
	#else
	if(bps==32){
		SwapInt32ToFloat32((SInt32 *)inputBuf,(float *)destBuf,num_samples, 32);
    }else if(bps==16){
        SwapInt16ToFloat32((SInt16 *)inputBuf,(float *)destBuf,num_samples, 16);
    }
	#endif
#else
    
    UInt32 numSamplesLeft;
    float *floatDestBuf;
    
    // Calculate the number of actual samples to convert
    numSamplesLeft = numSampleFrames * streamFormat->fNumChannels;
    
    // Start by casting the destination buffer to a float *
    floatDestBuf = (float *)destBuf;
    
    if(bps==32){
        SInt32 *inputBuf;
        float inputSample;
    
        // Determine the starting point for our input conversion
        inputBuf = &(((SInt32 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels]);
    
        // Loop through each sample and scale and convert them
        for (UInt32 i = 0; i < numSamplesLeft; i++) {
            // Fetch the SInt32 input sample
            inputSample = (float)((SInt32)correctEndianess32(*inputBuf));
        
            // Scale that sample to a range of -1.0 to 1.0, convert to float and store in the destination buffer
            // at the proper location
            *floatDestBuf = clamp(inputSample * ( (clipPosMulDiv32 * (inputSample > 0)) + (clipNegMulDiv32  * (inputSample < 0)) ) );
			
            // Move on to the next sample
            ++inputBuf;
            ++floatDestBuf;
        }
        
    }else if(bps==16){
        SInt16 *inputBuf;
        float inputSample;
        
        // Determine the starting point for our input conversion
        inputBuf = &(((SInt16 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels]);
        
        // Loop through each sample and scale and convert them
        for (UInt32 i = 0; i < numSamplesLeft; i++) {
            
            // Fetch the SInt16 input sample
            inputSample = (float)((SInt16)correctEndianess16(*inputBuf));
            
            // Scale that sample to a range of -1.0 to 1.0, convert to float and store in the destination buffer
            // at the proper location
                
            *floatDestBuf = clamp(inputSample * ( (clipPosMulDiv16 * (inputSample > 0)) + (clipNegMulDiv16 * (inputSample < 0)) ) );
            
            // Move on to the next sample
            ++inputBuf;
            ++floatDestBuf;
        }
    }

    
	#endif
    return kIOReturnSuccess;
}
