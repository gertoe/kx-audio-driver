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

//#include <IOKit/audio/IOAudioEngine.h>
#include "AudioDevice.h"

#ifndef _KXAUDIOENGINE_H
#define _KXAUDIOENGINE_H

#define MAPPING_NUM_CHANNELS 8

#ifndef KX_INTERNAL
	#error invalid configuration
#endif

class kXAudioEngine : public IOAudioEngine
{
    friend class kXAudioDevice;
    
public:
    OSDeclareDefaultStructors(kXAudioEngine)
    
private:
    kx_hw	*hw;
	int		n_frames;
	int		bps;
    
	UInt32	sampling_rate;
    
    UInt32 custom_sampling_rate;
    
	UInt8	n_channels;
	
	int		is_running;
    
    //custom saplig rate and channels mapping code
    UInt8 mapping[MAPPING_NUM_CHANNELS];
    
    // wave 2/3 - front
    // wave 6/7 - center+lfe
    // wave 4/5 - rear
    // 8/9 - rear center/etc.
    
    //boot args stuff
    UInt8 customMapping[MAPPING_NUM_CHANNELS];
    char customSampleRate[KXBootArgValueLength];
    char customMultiplyer[KXBootArgValueLength];
    
    
		//kx_voice_buffer *in_buffers_[MAX_CHANNELS_];
		//kx_voice_buffer *out_buffers_[MAX_CHANNELS_];
    
	IOAudioStream *in_streams[MAX_CHANNELS_];
	IOAudioStream *out_streams[MAX_CHANNELS_];
    
    void dump_addr(void);
    
    struct memhandle *my_alloc_contiguous(size_t size);
    void my_free_contiguous(struct memhandle *desc, mach_vm_size_t size);
    
public:

    virtual bool init(kx_hw *hw_);
    virtual void free();
	virtual void free_all();
    
    virtual bool initHardware(IOService *provider);
    virtual void stop(IOService *provider);
    
    virtual IOAudioStream *createNewAudioStream(int chn, IOAudioStreamDirection direction, UInt32 sampleBufferSize);
	virtual	void freeAudioStream(int chn,IOAudioStreamDirection direction);

    virtual IOReturn performAudioEngineStart();
    virtual IOReturn performAudioEngineStop();
    
    virtual UInt32 getCurrentSampleFrame();
	
    virtual IOReturn performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate);

    virtual IOReturn clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream);
    virtual IOReturn convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream);
    
    virtual OSString* getGlobalUniqueID();
    
#if defined(ARM)
    virtual bool driverDesiresHiResSampleIntervals(void);
#endif
    
};



#endif /* _KXAUDIOENGINE_H */
