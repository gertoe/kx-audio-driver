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

#include "AudioDevice.h"
#include "AudioEngine.h"
#include "Utility.h"
#include "emu.h"

#define DBGCLASS "kXAudioEngine"

#undef debug
#include "cedebug.h"

#define super IOAudioEngine
OSDefineMetaClassAndStructors(kXAudioEngine, IOAudioEngine)

// ----- Configuration ------

bool kXAudioEngine::init(kx_hw *hw_)
{
    
    debug("kXAudioEngine[%p]::init\n", this);
    
    UInt8 defMapping[MAPPING_NUM_CHANNELS] = {
        //2,3,4,5,6,7,8,9 - kX:  front, rear, center+lfe, back
        //1,2,3,4,5,6,7,8 - OSX: front, center+lfe, rear, back
        2,3,6,7,4,5,8,9
    };
    
    for (int i=0; i<MAPPING_NUM_CHANNELS; i++){
        mapping[i] = 0;
    }
    
    bool result = false;
    bool def    = false;
    
    hw = hw_;
    
    dump_addr();
    
    
    if (!super::init(NULL))
    {
        goto Done;
    }
    
    hw->initialized|=KX_ENGINE_INITED;
    
    setIndex(hw->actualPort);
    
    //it's better to keep this as is, because the initialization code probably assumes it with this value, after the initialization you are free to change smapling rate
    sampling_rate=48000;
    //known safe default value
    custom_sampling_rate = sampling_rate;
    
    result = true;
    
    bzero(customMapping, KXBootArgValueLength);
    bzero(customSampleRate, KXBootArgValueLength);
    bzero(customMultiplyer, KXBootArgValueLength);
    
    //enable some experiemntal stuff, no warraties here, be very carefoul, the customMapping array is used as temporary storage for calling this function
    
	#if !defined(USE_TIGER_IPC)
    if (PE_parse_boot_argn("-kxtest", customMapping, KXBootArgValueLength)){
        debug(DBGCLASS"[%p]::init: WARNING Experimental features enabled using the -kxtest boot arg, you are doing this at your own risk, this stuff may harm your computer, YOU HAVE BEEN WARNED!!\n",this);
        
        //TODO: Add test stuff here
        
    }
	#endif
    
    //cleanup
    bzero(customMapping, KXBootArgValueLength);
    
    //when you need a custom output mapping for whatever reason, here it is, it's supported via boot arg, the equivalent thing via mac's midi configuration app is quite buggy
    //This is for you Hurrain, because you told me you have a broken jack on your sound card
    
	#if !defined(USE_TIGER_IPC)
    if (PE_parse_boot_argn("_kxcl", customMapping, MAPPING_NUM_CHANNELS)) {
        debug(DBGCLASS"[%p]::init: custom output layout specified with the _kxcl boot arg \n",this);
        
        for (int i=0; i<MAPPING_NUM_CHANNELS; i++){
            
            if (!inRange(customMapping[i], '2', '9')){
                
                debug("kXAudioEngine[%p]::init: !!! remapping channel [%d] using illegal value [%c] !!!\n",this, i, customMapping[i]);
                
                def = true;
                break;
            }
            
            mapping[i] = customMapping[i] - '0';
            
            for (int l = 0; l<MAPPING_NUM_CHANNELS; l++){
                if (l != i && mapping[i] == mapping[l]){
                    debug("kXAudioEngine[%p]::init: remapping channel [%d] using duplicated value [%d], the default mapping will be used\n",this,i, mapping[i]);
                    def = true;
                    break;
                }
            }
            
            if (def){
                break;
            }
            
            debug("kXAudioEngine[%p]::init: remapping channel [%d] using custom mapping [%d]\n",this,i, mapping[i]);
        }
    }else{
        def = true;
    }
	#else
	def = true;
	#endif
    
    if (def) {
        debug("kXAudioEngine[%p]::init: Using default mapping value\n",this);
        for (int j=0; j<MAPPING_NUM_CHANNELS; j++){
            mapping[j] = defMapping[j];
        }
    }
    
	#if !defined(USE_TIGER_IPC)
     //this boot arg is used to enter custom sampling rates values for consumer cards which just uses a pitch adjustment trick and so they can basically use any sampling rate not higher than 192khz and not lower than 1khz
    if (PE_parse_boot_argn("_kxcsr", customSampleRate, KXBootArgValueLength)){
        debug(DBGCLASS"[%p]::init: custom sampling rate specified with the _kxcsr boot arg\n",this);
        
        custom_sampling_rate = stringToNumber(customSampleRate);
        
        //i mean who needs a smapling rate lower than 1k? i don't even know if the i/o kit allows for sampling rates this low - ITzTravelInTime
        if (!inRange(custom_sampling_rate, KX_MIN_RATE, KX_MAX_RATE)){
            debug("kXAudioEngine[%p]::init: new custom sampling rate \"%s\" is out of the supported range [%i to %i] or uses illegal characters\n",this, customSampleRate, KX_MIN_RATE, KX_MAX_RATE);
            custom_sampling_rate = sampling_rate;
        }else{
            debug("kXAudioEngine[%p]::init: new custom sampling rate: %u\n",this, (unsigned int)custom_sampling_rate);
        }
        
    }
	#endif
    
    if(hw->is_10k2)
        bps=32;
    else
        bps=16;
    
    n_channels=8; // should be <= MAX_CHANNELS_
    
    // Use 4 times the buffer size to prevent some sort of underrun on
    // Mavericks (related to Timer Coalescing?) causing playback crackle
    // until streams are restart (e.g. stop/start playback)
    //    n_frames = (int)(4 * (hw->mtr_buffer.size * 8 / bps / n_channels)); //with hardcoded 4x multiplyer
    //    n_frames = (int)(hw->mtr_buffer.size * 8 / bps / n_channels);       //without hardcoded multiplyer
    
    //the buffer size is n_channels*n_frames*bps/8
    
    //about the multiplyer, it turns out some users have problems with it and some other users have problems without it, so it's better to have it changable via boot arg, and so users can just use whatever value in the supported range to try to fix their issues and all without having to touch the code
    
    //hw->mtr_buffer is already allocated here, so why allocating it again later?
    n_frames = (int)((hw->mtr_buffer.size * 8 / bps / n_channels));
    
	#if !defined(USE_TIGER_IPC)
    if (PE_parse_boot_argn("_kxcfm", customMultiplyer, KXBootArgValueLength)){
           
        debug(DBGCLASS"[%p]::init: custom n_frames multiplyer specified with the _kxcfm boot arg\n",this);
        
        unsigned int mul = (unsigned int)stringToNumber(customMultiplyer);
        
        //limited to 128x for "safety" reasons, 128x is already an insane value
        if (inRange(mul, 1, 128)){
            debug(DBGCLASS"[%p]::init: custom n_frames multiplyer value is %u\n",this, (unsigned int)mul);
            
            n_frames *= ((int)mul);
        }else{
            debug(DBGCLASS"[%p]::init: custom n_frames multiplyer value \"%s\" uses illegal chracters or it's out of the supported range [1 to 128]\n",this, customMultiplyer);
        }
    }
    #endif
	
    debug("kXAudioEngine[%p]::init - n_frames=%d\n", this, n_frames);
    
    is_running=0;
    
    bzero(out_streams, sizeof(out_streams));
    bzero(in_streams, sizeof(in_streams));
    
    // configure kX to ASIO-style I/O
    if(hw)
    {
        hw->asio_notification_krnl.pb_buf=-1;
        hw->asio_notification_krnl.rec_buf=-1;
        hw->asio_notification_krnl.toggle=-1;
        hw->asio_notification_krnl.asio_method=KXASIO_METHOD_SEND_EVENT; // notify via deferred, but catch up directly in the OSX IRQ handler
        hw->asio_notification_krnl.n_voice=-1;
        hw->asio_notification_krnl.semi_buff=0;
        hw->asio_notification_krnl.active=0;
        hw->asio_notification_krnl.cur_pos=0;
        hw->asio_notification_krnl.kevent=this;
        
        hw->cur_asio_in_bps=bps;
    }
    
Done:
    
    return result;
}

bool kXAudioEngine::initHardware(IOService *provider)
{
    bool result = false;
    IOAudioSampleRate initialSampleRate;
    IOAudioStream *audioStream;
    
    const int buffer_size = 1*n_frames*bps/8;
    
    debug("kXAudioEngine[%p]::initHardware(%p)\n", this, provider);
    
    if (!super::initHardware(provider)) {
        goto Done;
    }
    
    // Setup the initial sample rate for the audio engine
    initialSampleRate.whole = sampling_rate;
    initialSampleRate.fraction = 0;
    
    char device_name[KX_MAX_STRING];
    //strncpy(device_name,"kX ",KX_MAX_STRING);
    
    for (uint i = 0; i < KX_MAX_STRING; i++){
        device_name[i] = '\0';
    }
    
    //strncat(device_name,hw->card_name,KX_MAX_STRING);
    
    _kx_strcpy(device_name, hw->card_name, KX_MAX_STRING);
    
    setDescription(device_name);
    
    setSampleRate(&initialSampleRate);
    
    setClockDomain(); // =kIOAudioNewClockDomain
    
#if defined(X86)
    setClockIsStable(false);
#endif
    
    setIndex(hw->actualPort);
    
    // calculate kx_sample_offset
    
    //this new code fixes most of the cracks the old driver had, the commented lines are left just to keep track of what was there before, please don't touch
    if(hw->is_10k2)
    {
        setSampleOffset(40);
        
        //setSampleLatency(40+1);
        
        //setSampleOffset(28); // 28 samples
        //setSampleLatency(28+1);
        
    }
    else
    {
        setSampleOffset(32); // 32 samples
        //setSampleLatency(32+1);
    }
    
    
    
    // Allocate our input and output streams
    for(int i=0;i<n_channels;i++)
    {
        // Create an IOAudioStream for each buffer and add it to this audio engine
        audioStream = createNewAudioStream(i,kIOAudioStreamDirectionOutput, buffer_size);
        if (!audioStream) {
            goto Done;
        }
        
        addAudioStream(audioStream);
        out_streams[i]=audioStream;
    }
    
    // recording, a single buffer for 8 channels is used
    audioStream = createNewAudioStream(0,kIOAudioStreamDirectionInput, n_channels*buffer_size);
    if (!audioStream) {
        goto Done;
    }
    
    addAudioStream(audioStream);
    in_streams[0]=audioStream;
    
    dump_addr();
    
    // Set the number of sample frames in each buffer
    setNumSampleFramesPerBuffer(n_frames);
    
    setIndex(hw->actualPort);
    
    result = true;
    
Done:
    if(result==false)
        free_all();
    
    return result;
}

void kXAudioEngine::free()
{
    debug("kXAudioEngine[%p]::free()\n", this);
    
    free_all();
    
    super::free();
    
    hw=NULL;
}

void kXAudioEngine::stop(IOService *provider)
{
    debug("kXAudioEngine[%p]::stop(%p) - %s\n", this, provider, is_running?"[still running!]":"");
    
    // Add code to shut down hardware (beyond what is needed to simply stop the audio engine)
    // There may be nothing needed here
    free_all();
    
    super::stop(provider);
}

void kXAudioEngine::free_all()
{
    debug("kXAudioEngine[%p]::free_all() - %s\n",this,is_running?"[still running!]":"");
    if(is_running)
        stopAudioEngine();
    
    // free buffers
    setNumSampleFramesPerBuffer(0);
    
    for(int i=0;i<n_channels;i++)
    {
        if(out_streams[i])
        {
            out_streams[i]->setSampleBuffer (NULL, 0);
            out_streams[i]->release();
            out_streams[i]=NULL;
        }
        if(in_streams[i])
        {
            in_streams[i]->setSampleBuffer (NULL, 0);
            in_streams[i]->release();
            in_streams[i]=NULL;
        }
        
        if(hw)
        {
            freeAudioStream(i,kIOAudioStreamDirectionInput);
            freeAudioStream(i,kIOAudioStreamDirectionOutput);
        }
    }
    
    if(hw)
    {
        if((hw->initialized&KX_ENGINE_INITED) && !(hw->initialized&KX_DEVICE_INITED))
        {
            debug("kXAudioEngine[%p]::free_all() - close iKX interface\n",this);
            kx_close(&hw);
            hw=0;
        }
        else
        {
            debug("kXAudioEngine[%p]::free_all() - dereference iKX interface\n",this);
            hw->initialized&=(~KX_ENGINE_INITED);
            hw=NULL;
        }
    }
    else
        debug("kXAudioEngine[%p]::free_all() - iKX interface already closed\n",this);
}

struct memhandle *kXAudioEngine::my_alloc_contiguous(size_t size)
{
    if(size<PAGE_SIZE)
        size=PAGE_SIZE;
    
#ifdef DEBUGGING
    size += 2 * PAGE_SIZE;
#endif
    
    struct memhandle* mem = new struct memhandle;
    bzero((void*)mem, sizeof(*mem));
    mem->size = size;
    
#if defined(OLD_ALLOC)
    
    //old allocation method, deprecated and with a lot of drawbacks, the new one makes sure we are allocating memory in the 32 bit address space and that the buffer is contiguous
    
    IOPhysicalAddress phy = NULL;
    mem->addr = IOMallocContiguous(size, PAGE_SIZE, &phy);
    mem->dma_handle = (dword)phy;
    
    if (!mem->addr || !mem->dma_handle){
        debug("kXAudioEngine[%p]::my_alloc_contiguous() - allocation failed\n",this);
        delete mem;
        return NULL;
    }

    
#else
	mach_vm_address_t mask = kx_allocation_mask;//0x000000007FFFFFFFULL & ~(PAGE_SIZE - 1);
    
    IOBufferMemoryDescriptor *desc =
    IOBufferMemoryDescriptor::inTaskWithPhysicalMask(
                                                     kernel_task,
                                                     kIODirectionInOut | kIOMemoryPhysicallyContiguous,
                                                     size,
                                                     mask);
    
    if(desc){
        desc->prepare();
        
        IOPhysicalAddress pa = desc->getPhysicalAddress();
        mem->desc = desc;
        
        if (pa & (~mask))
            debug("kXAudioEngine[%p]::my_alloc_contiguous() - memory misaligned or beyond 2GB limit (%p)\n", this, (void *)pa);
        
        mem->dma_handle = (dword)pa; //pa is the address of the memory buffer, which must be in the 32 bit address space limit for the card to work
        mem->addr = desc->getBytesNoCopy();
        
    }else{
        debug("kXAudioEngine[%p]::my_alloc_contiguous() - allocation failed\n",this);
        delete mem;
        return NULL;
    }
#endif
    
    debug("kXAudioEngine[%p]::my_alloc_contiguos() - Allocation success. Virtual address [%p], Physical address [%p]\n", (this), (mem->addr), (void*)(IOPhysicalAddress)(mem->dma_handle));
    
    bzero(mem->addr, mem->size);
    
    return mem;
    
}

void kXAudioEngine::my_free_contiguous(struct memhandle *desc, mach_vm_size_t size)
{
#ifdef DEBUGGING
    if(size<PAGE_SIZE)
        size=PAGE_SIZE;
    
    //    addr=((UInt8 *)addr)-PAGE_SIZE;
    UInt8 *buff = (UInt8 *)desc->getBytesNoCopy();
    
    // check
    for(int i=0;i<PAGE_SIZE;i++)
    {
        if(buff[i]!=0x11)
        {
            debug("!! kXAudioEngine[%p]::my_free_contiguous: buffer was damaged before! offset: %d\n",this,i);
            break;
        }
        if(buff[i+PAGE_SIZE+size]!=0x22)
        {
            debug("!! kXAudioEngine[%p]::my_free_contiguous: buffer was damaged after! offset: %d\n",this,i);
            break;
        }
    }
#endif
    
#if !defined(OLD_ALLOC)
    //this is enought to free the memory buffer
    desc->desc->release();
#else
    IOFreeContiguous(desc->addr, desc->size);
#endif
}

void kXAudioEngine::freeAudioStream(int chn,IOAudioStreamDirection direction)
{
    
    
    if(direction==kIOAudioStreamDirectionOutput)
    {
        struct memhandle mem;
		
        for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
        {
            
            if(hw->voicetable[i].asio_id==this && hw->voicetable[i].asio_channel==(dword)chn)
            {
                kx_wave_close(hw,i);
                
                hw->voicetable[i].asio_id=0;
                hw->voicetable[i].asio_channel=0xffffffff;
                
                hw->voicetable[i].asio_mdl=0;
                hw->voicetable[i].asio_kernel_addr=0;
                hw->voicetable[i].asio_user_addr=0;
                
                bzero(((byte*)&mem), sizeof(mem));
                
                mem.addr       = hw->voicetable[i].buffer.addr;
                mem.dma_handle = hw->voicetable[i].buffer.physical;
                mem.size       = hw->voicetable[i].buffer.size;
                
#if !defined(OLD_ALLOC)
                if(!hw->voicetable[i].buffer.desc){
                    continue;
                }
                mem.desc = hw->voicetable[i].buffer.desc;
#endif
				
                if(mem.addr && mem.size!=0)
                {
                    //my_free_contiguous(hw->voicetable[i].buffer.addr,hw->voicetable[i].buffer.size);
                    my_free_contiguous(&mem, mem.size);
                    bzero(((byte*)&(hw->voicetable[i].buffer)), sizeof(hw->voicetable[i].buffer));
                }
            }
        }
    }
    else
    {
        if(direction==kIOAudioStreamDirectionInput)
        {
            
#if !defined(OLD_ALLOC)
            if (!hw->mtr_buffer.desc){
                return;
            }
#endif
            
            if (hw->mtr_buffer.addr && hw->mtr_buffer.size != 0)
            {
                my_free_contiguous(&hw->mtr_buffer, hw->mtr_buffer.size);
                bzero(&(hw->mtr_buffer), sizeof(hw->mtr_buffer));
            }
        }
    }
}

IOAudioStream *kXAudioEngine::createNewAudioStream(int chn, IOAudioStreamDirection direction, UInt32 sampleBufferSize)
{
    IOAudioStream *audioStream = new IOAudioStream;
    
    if (audioStream){
        
        static UInt32 streamIndex = 0;
        
        if (!audioStream->initWithAudioEngine(this, direction, 1)){
            
            audioStream->release();
            
        } else {
            IOAudioSampleRate rate;
            
            IOAudioStreamFormat format = {
                1,                                              // num channels
                kIOAudioStreamSampleFormatLinearPCM,            // sample format
                kIOAudioStreamNumericRepresentationSignedInt,   // numeric format
                bps,                                            // bit depth
                bps,                                            // bit width
                kIOAudioStreamAlignmentHighByte,                // high byte aligned - unused because bit depth == bit width
                kIOAudioStreamByteOrderLittleEndian,            // little endian
                true,                                           // format is mixable
                hw->actualPort + streamIndex++                    // driver-defined tag
            };
            
            //buffer allocation
            struct memhandle* mem = my_alloc_contiguous(sampleBufferSize);
            
            if(!mem)
                return NULL;
            
            // allocate memory:
            if(direction==kIOAudioStreamDirectionOutput)
            {
                //buffer object creation and cleaning
                kx_voice_buffer buffer;
                bzero(((byte*)&buffer), sizeof(buffer));
                
                buffer.physical = mem->dma_handle;
                buffer.addr = mem->addr;
                
#if !defined(OLD_ALLOC)
                buffer.desc = mem->desc;
                //basically the buffer is not allocated if this happens
                if (!buffer.desc)
                    return NULL;
#endif
                
                //finishes to set up the buffer object
                buffer.size=(dword)mem->size;
                buffer.that=this;
                buffer.notify=-n_frames;
                
                int need_notifier=VOICE_OPEN_NOTIMER;
                if(chn==0) // first voice?
                    need_notifier|=VOICE_OPEN_NOTIFY; // half/full buffer
                
                //This mapping has been replaced by a more flexible system which allows for customization using boot args, kepth this old code here for back record stuff
                /*int mapping[]=
                 { //2,3,4,5,6,7,8,9 - kX:  front, rear, center+lfe, back
                 //1,2,3,4,5,6,7,8 - OSX: front, center+lfe, rear, back
                 2,3,6,7,4,5,8,9 };
                 // wave 2/3 - front
                 // wave 6/7 - center+lfe
                 // wave 4/5 - rear
                 // 8/9 - rear center/etc.
                 */
                
                int i=kx_allocate_multichannel(hw,bps,sampling_rate,need_notifier,&buffer,DEF_ASIO_ROUTING+mapping[chn]); // start with 2/3
                
                if(i>=0)
                {
                    hw->voicetable[i].asio_id=this;
                    hw->voicetable[i].asio_mdl=0;
                    hw->voicetable[i].asio_user_addr=0;
                    hw->voicetable[i].asio_kernel_addr=0;
                    hw->voicetable[i].asio_channel=chn;
                    
                    if(chn==0) // first?
                    {
                        hw->asio_notification_krnl.n_voice=i;
                        hw->asio_notification_krnl.semi_buff=(hw->voicetable[i].param.endloop-hw->voicetable[i].param.startloop)/2;
                    }
                }
            }
            else if(direction==kIOAudioStreamDirectionInput)
            {
                format.fNumChannels = n_channels; // sets input to 8 channels
                
                /*
				 #if !defined(OLD_ALLOC)
				 hw->mtr_buffer.desc = buffer.desc;
				 #endif
				 hw->mtr_buffer.addr = buffer.addr; //sets the address in the driver code
				 hw->mtr_buffer.size = buffer.size; //sets the sice in the driver code
				 hw->mtr_buffer.dma_handle = buffer.physical; //sets the address for the dma functions/memory access from hardware
				 */
                
                memcpy(&hw->mtr_buffer, mem, sizeof(hw->mtr_buffer));
                
                //kx_writeptr(hw,FXIDX,0,0);
                
                //debug("createNewAudioStream FXBA:\n");
                //kx_writeptr_prof(hw, FXBA, 0, hw->mtr_buffer.dma_handle);
                kx_writeptr(hw, FXBA, 0, hw->mtr_buffer.dma_handle); //sets the appropriate dma reg in the card
                
                dword ch = (1 << (format.fNumChannels * 2)) - 1;    // 24bit needs 2 physical channels
                debug("createNewAudioStream FXWCH/FXWC_K1: %x\n", (unsigned int)ch);
                
                if(hw->is_10k2)
                {
                    kx_writeptr(hw,FXWCL, 0, 0);
                    kx_writeptr(hw,FXWCH, 0, ch);
                }
                else
                    kx_writeptr(hw,FXWC_K1, 0, ch << 16);
            }
            
            debug("kXAudioEngine[%p] Initializing sampling rates\n",this);
            
            // As part of creating a new IOAudioStream, its sample buffer needs to be set
            // It will automatically create a mix buffer should it be needed
            if((mem->addr) && (mem->size))
				audioStream->setSampleBuffer((mem->addr), (UInt32)(mem->size));
            
            //we need to add our supported sample rates now
            rate.fraction = 0;
			
			{
				
				const bool isInput = (direction==kIOAudioStreamDirectionInput);
				
                //the order of this array is important, don't touch it
                const UInt32 supportedFreqs[] = {48000, 44100, 192000, 176400, 96000, 88200, 22050, 24000, 11025, 12000, 6000, 7000, 8000, 9600, 16000, 18900, 32000, 37800, 44056, 49716, 64000, custom_sampling_rate, sampling_rate};
                UInt8 length = isInput ? 1 : (sizeof(supportedFreqs) / sizeof(*supportedFreqs));
                
                if (hw->is_edsp){
                    debug("kXAudioEngine[%p] The current sound card in an E-MU e-dsp or similar (using the same architecture), using hardware support for sampling rathes rather than the software resampler\n",this);
                    
                    if (isInput)
                        length = 2;
                    else
                        length = 11;
                    
                }else{
                    //support for custom sampling rates for the software resampler
                    if (custom_sampling_rate != sampling_rate)
                        debug("kXAudioEngine[%p] Custom sampling rate: %u\n",this, (unsigned int)custom_sampling_rate);
                }
                
                format.fBitWidth = bps;
                
				{
					UInt8 d = isInput ? bps : 8;
					
					for (; d <= bps; d += 8){
						format.fBitDepth = d;
						for (UInt8 i = 0; i < length; i++){
							rate.whole = supportedFreqs[i];
							audioStream->addAvailableFormat(&format, &rate, &rate);
							//debug("kXAudioEngine[%p] Added sampling rate: %u at %u bits. is input? %u \n",this, rate.whole, format.fBitDepth, isInput);
						}
					}
					
				}
                
			}
            // Finally, the IOAudioStream's current format needs to be indicated
            audioStream->setFormat(&format);
            
            delete mem;
        }
    }
    
    return audioStream;
}

IOReturn kXAudioEngine::performAudioEngineStart()
{
    // debug("kXAudioEngine[%p]::performAudioEngineStart() - %d\n", this,is_running);
    
    // When performAudioEngineStart() gets called, the audio engine should be started from the beginning
    // of the sample buffer.  Because it is starting on the first sample, a new timestamp is needed
    // to indicate when that sample is being read from/written to.  The function takeTimeStamp()
    // is provided to do that automatically with the current time.
    // By default takeTimeStamp() will increment the current loop count in addition to taking the current
    // timestamp.  Since we are starting a new audio engine run, and not looping, we don't want the loop count
    // to be incremented.  To accomplish that, false is passed to takeTimeStamp().
    
    dword low=0,high=0;
    int first=-1;
    
    for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
    {
        if(hw->voicetable[i].usage&VOICE_USAGE_ASIO && hw->voicetable[i].asio_id==this)
        {
            if(i>=32)
                high|=(1<<(i-32));
            else
                low|=(1<<i);
            if(first==-1)
                first=i;
        }
    }
    
    if(first != -1)
    {
        // zero sample position, too
        for(int i=0;i<32;i++)
        {
            if(low&(1<<i))
                kx_wave_set_position(hw,i,0);
            if(high&(1<<i))
                kx_wave_set_position(hw,i+32,0);
        }
        
        // debug("kXAudioEngine[%p]::performAudioEngineStart: start audio [%d; %x %x]\n",this,first,(unsigned)low,(unsigned)high);
        kx_wave_start_multiple(hw,first,low,high);
    }
    
    // recording
    //BTW it loooks fancy
    dword sz;
    switch(hw->mtr_buffer.size)
    {
        case 384: sz=ADCBS_BUFSIZE_384; break;
        case 448: sz=ADCBS_BUFSIZE_448; break;
        case 512: sz=ADCBS_BUFSIZE_512; break;
        case 640: sz=ADCBS_BUFSIZE_640; break;
        case 768: sz=ADCBS_BUFSIZE_768; break;
        case 896: sz=ADCBS_BUFSIZE_896; break;
        case 1024: sz=ADCBS_BUFSIZE_1024; break;
        case 1280: sz=ADCBS_BUFSIZE_1280; break;
        case 1536: sz=ADCBS_BUFSIZE_1536; break;
        case 1792: sz=ADCBS_BUFSIZE_1792; break;
        case 2048: sz=ADCBS_BUFSIZE_2048; break;
        case 2560: sz=ADCBS_BUFSIZE_2560; break;
        case 3072: sz=ADCBS_BUFSIZE_3072; break;
        case 3584: sz=ADCBS_BUFSIZE_3584; break;
        case 4096: sz=ADCBS_BUFSIZE_4096; break;
        case 5120: sz=ADCBS_BUFSIZE_5120; break;
        case 6144: sz=ADCBS_BUFSIZE_6144; break;
        case 7168: sz=ADCBS_BUFSIZE_7168; break;
        case 8192: sz=ADCBS_BUFSIZE_8192; break;
        case 10240: sz=ADCBS_BUFSIZE_10240; break;
        case 12288: sz=ADCBS_BUFSIZE_12288; break;
        case 14366: sz=ADCBS_BUFSIZE_14366; break;
        case 16384: sz=ADCBS_BUFSIZE_16384; break;
        case 20480: sz=ADCBS_BUFSIZE_20480; break;
        case 24576: sz=ADCBS_BUFSIZE_24576; break;
        case 28672: sz=ADCBS_BUFSIZE_28672; break;
        case 32768: sz=ADCBS_BUFSIZE_32768; break;
        case 40960: sz=ADCBS_BUFSIZE_40960; break;
        case 49152: sz=ADCBS_BUFSIZE_49152; break;
        case 57344: sz=ADCBS_BUFSIZE_57344; break;
        default:
        case 65536: sz=ADCBS_BUFSIZE_65536; break;
    }
    
    //debug("performAudioEngineStart FXBS:\n");
    //kx_writeptr_prof(hw, FXBS, 0, sz);
    kx_writeptr(hw, FXBS, 0, sz);
    
    hw->asio_notification_krnl.active=1;
    
    is_running=1;
    
    takeTimeStamp(false);
    
    return kIOReturnSuccess;
}

IOReturn kXAudioEngine::performAudioEngineStop()
{
    dump_addr();
    
    // debug("kXAudioEngine[%p]::performAudioEngineStop() - %d\n", this,is_running);
    
    hw->asio_notification_krnl.active=0;
    
    dword low=0,high=0;
    int first=-1;
    
    for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
    {
        if(hw->voicetable[i].usage&VOICE_USAGE_ASIO && hw->voicetable[i].asio_id==this)
        {
            if(i>=32)
                high|=(1<<(i-32));
            else
                low|=(1<<i);
            if(first==-1)
                first=i;
        }
    }
    
    if(first!=-1)
    {
        // debug("kXAudioEngine[%p]::performAudioEngineStop: stop audio [%d; %x %x]\n",this,first,(unsigned)low,(unsigned)high);
        kx_wave_stop_multiple(hw,first,low,high);
        
        // zero sample position, too
        for(int i=0;i<32;i++)
        {
            if(low&(1<<i))
                kx_wave_set_position(hw,i,0);
            if(high&(1<<i))
                kx_wave_set_position(hw,i+32,0);
        }
    }
    
    // recording
    //debug("performAudioEngineStop FXBS:\n");
    //kx_writeptr_prof(hw, FXBS, 0, 0);
    kx_writeptr(hw, FXBS, 0, 0);
    
    is_running=0;
    
    return kIOReturnSuccess;
}

UInt32 kXAudioEngine::getCurrentSampleFrame()
{
    // In order for the erase process to run properly, this function must return the current location of
    // the audio engine - basically a sample counter
    // It doesn't need to be exact, but if it is inexact, it should err towards being before the current location
    // rather than after the current location.  The erase head will erase up to, but not including the sample
    // frame returned by this function.  If it is too large a value, sound data that hasn't been played will be
    // erased.
    
    if(hw && hw->asio_notification_krnl.n_voice>=0)
    {
        kx_get_asio_position(hw,1); // this simply updates hw->asio_notification_krnl->cur_pos, we ignore return code
        // cur_pos is in samples, for 24/32 streams it is in 16-bit bytes, that is, again, samples
        
        // sanity check
        dword pos=hw->asio_notification_krnl.cur_pos;
        if(pos>=(dword)n_frames)
        {
            debug("!! kXAudioEngine[%p]::getCurrentSampleFrame: invalid pos [%x, %d]\n",this,(unsigned)pos,n_frames);
            pos=0;
        }
        
        return pos; // this is in number of samples
    }
    
    debug("!! kXAudioEngine[%p]::getCurrentSampleFrame: hw = 0\n",this);
    return 0;
}

IOReturn kXAudioEngine::performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate)
{
    debug("kXAudioEngine[%p]::peformFormatChange(%p, %p, %p)\n", this, audioStream, newFormat, newSampleRate);
    
    //WARNING: This function assumes newSampleRate->whole is a positive number despite being a signed type
    
    if (newSampleRate)
    {
        if(!hw->is_edsp){
            
            
            //not a real sampling rate support, but rather a pitch adjustment to sound right, this can work for the professional cards as well, but there is a better implementation for professional cards using the hana fpga, so it just uses that (BTW how does the creative windows driver for some cards support 192 khz?)
            //if (audioStream->direction == kIOAudioStreamDirectionOutput){
				if(newSampleRate->whole == 192000){
					for(int i=0; i<n_channels;i++)
					{
						hw->voicetable[i].param.pitch_target  = 0xffff;
					}
				}else{
					for(int i=0; i<n_channels;i++)
					{
						hw->voicetable[i].param.initial_pitch =(word)  kx_srToPitch(kx_sr_coeff(hw,newSampleRate->whole) >> 8);
						hw->voicetable[i].param.pitch_target = kx_samplerate_to_linearpitch(kx_sr_coeff(hw,newSampleRate->whole));
						hw->voicetable[i].sampling_rate = newSampleRate->whole;
					}
				}
			//}else{
				
			//}
            
            //TODO: input sample rates switching
            
        }else{
            
            dword config_val = kx_readfn0(hw,HCFG_K1);
            
            dword clockVal = EMU_HANA_WCLOCK_INT_48K;
            dword clockChip = EMU_HANA_DEFCLOCK_48K;
            dword led = EMU_HANA_DOCK_LEDS_2_44K;
            
            dword pitch_target_seed = 48000; //kx_samplerate_to_linearpitch(kx_sr_coeff(hw,48000));
            dword card_frequency = 48000;
            
            switch (newSampleRate->whole) {
                    
                case 6000:
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    
                    pitch_target_seed = 6000;
                    break;
                case 11025:
                    clockVal = EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X;
                    clockChip = EMU_HANA_DEFCLOCK_44_1K;
                    
                    pitch_target_seed = 12000;
                    card_frequency = 44100;
                    break;
                case 12000:
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    
                    pitch_target_seed = 12000;
                    break;
                case 22050:
                    clockVal = EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X;
                    clockChip = EMU_HANA_DEFCLOCK_44_1K;
                    
                    pitch_target_seed = 24000;
                    card_frequency = 44100;
                    break;
                case 24000:
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    
                    pitch_target_seed = 24000;
                    break;
                case 44100:
                    clockVal = EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X;
                    clockChip = EMU_HANA_DEFCLOCK_44_1K;
                    
                    card_frequency = 44100;
                    break;
                case 48000:
                    led = EMU_HANA_DOCK_LEDS_2_48K;
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    break;
                case 88200:
                    
                    clockVal = EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X;
                    clockChip = EMU_HANA_DEFCLOCK_44_1K;
                    
                    led = EMU_HANA_DOCK_LEDS_2_96K;
                    
                    pitch_target_seed = 96000;
                    card_frequency = 44100;
                    
                    break;
                case 96000:
                    led = EMU_HANA_DOCK_LEDS_2_96K;
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    
                    pitch_target_seed = 96000;
                    break;
                case 176400:
                    clockVal = EMU_HANA_WCLOCK_INT_44_1K | EMU_HANA_WCLOCK_1X;
                    clockChip = EMU_HANA_DEFCLOCK_44_1K;
                    
                    led = EMU_HANA_DOCK_LEDS_2_192K;
                    
                    pitch_target_seed = 192000;
                    card_frequency = 44100;
                    
                    break;
                case 192000:
                    led = EMU_HANA_DOCK_LEDS_2_192K;
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    
                    pitch_target_seed = 192000;
                    break;
                default:
                    debug("\t Internal Error - unknown sample rate selected falling back to 48khz.\n");
                    clockVal |= EMU_HANA_WCLOCK_1X;
                    led = EMU_HANA_DOCK_LEDS_2_48K;
                    break;
            }
            
            kx_set_hw_parameter(hw,KX_HW_AC3_PASSTHROUGH, card_frequency == 48000);
            
            //mute the fpga
            kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_MUTE);
            
            //turn off the leds
            kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2, EMU_HANA_DOCK_LEDS_2_44K | EMU_HANA_DOCK_LEDS_2_EXT);
            kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2, EMU_HANA_DOCK_LEDS_2_48K | EMU_HANA_DOCK_LEDS_2_EXT);
            kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2, EMU_HANA_DOCK_LEDS_2_96K | EMU_HANA_DOCK_LEDS_2_EXT);
            kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2, EMU_HANA_DOCK_LEDS_2_192K | EMU_HANA_DOCK_LEDS_2_EXT);
            
            IOSleep(100);
            
            //set defclock and wclock
            kx_writefpga(hw,EMU_HANA_DEFCLOCK, clockChip);
            kx_writefpga(hw,EMU_HANA_WCLOCK, clockVal);
            
            const dword pitch_target = pitch_target_seed == 192000 ? 0xffff : kx_samplerate_to_linearpitch(kx_sr_coeff(hw, pitch_target_seed));
            
            for(int i=0; i<n_channels;i++){
                hw->voicetable[i].param.pitch_target = pitch_target;
                hw->voicetable[i].sampling_rate = pitch_target_seed;
            }
            
            //unmute and update leds
            IOSleep(200);
            kx_writefpga(hw,EMU_HANA_DOCK_LEDS_2, led | EMU_HANA_DOCK_LEDS_2_LOCK);
            kx_writefpga(hw,EMU_HANA_UNMUTE,EMU_UNMUTE);
            
            if (card_frequency != 48000){
                config_val |= HCFG_44K_K2;
            }else{
                config_val &= ( ~HCFG_44K_K2 );
            }
            
            kx_writefn0(hw,HCFG_K1,config_val);
            
            hw->card_frequency = card_frequency;
        }
    }
    
    //dump_addr();
    
    return kIOReturnSuccess;
}


void kXAudioEngine::dump_addr(void)
{
    
    //debug dumping and stuff, nice if you don't need it and you just want some extra text to look at while you are booting
    
#if 1
    IOLog("== functions: init: %p initHardware: %p free: %p stop: %p free_all: %p alloc: %p free: %p freeAS: %p\n"
          "== functions: create: %p perfStart: %p perfStop: %p getCW: %p fmt: %p\n"
          "== functions: clip: %p clip2: %p\n",
          OSMemberFunctionCast(void *,this,&kXAudioEngine::init),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::initHardware),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::free),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::stop),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::free_all),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::my_free_contiguous),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::my_alloc_contiguous),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::freeAudioStream),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::createNewAudioStream),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::performAudioEngineStart),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::performAudioEngineStop),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::getCurrentSampleFrame),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::performFormatChange),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::clipOutputSamples),
          OSMemberFunctionCast(void *,this,&kXAudioEngine::convertInputSamples)
          );
    
    //debug stuff
    /*
    debug("\n\n== n_frames stuff\n");
    debug("== n_frames %i\n", n_frames);
    debug("== customMultiplyer %s\n", customMultiplyer);
    debug("== customMultiplyer interpeted %u\n", stringToNumber(customMultiplyer));
    
    debug("\n\n== sampling rate stuff\n");
    debug("== custom_sampling_rate %i\n", custom_sampling_rate);
    debug("== customSampleRate %s\n", customSampleRate);
    debug("== customSampleRate interpreted %u\n", stringToNumber(customSampleRate));
    
    debug("\n\n== mapping stuff\n");
    debug("== customMapping %s\n", customMapping);
    debug("== Mapping %s\n", mapping);
    
    debug("\n\n");
    */
     
#endif
}

#if defined(ARM)
bool kXAudioEngine::driverDesiresHiResSampleIntervals(){
    return TRUE; //to be changed probably, idk
}
#endif
