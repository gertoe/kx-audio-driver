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


#include <IOKit/audio/IOAudioDevice.h>
#include <IOKit/audio/IOAudioEngine.h>
#include <IOKit/audio/IOAudioControl.h>
#include <IOKit/audio/IOAudioLevelControl.h>
#include <IOKit/audio/IOAudioToggleControl.h>
#include <IOKit/audio/IOAudioDefines.h>
#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOFilterInterruptEventSource.h>

#include "driver/kx.h"

#include "AudioDevice.h"
#include "AudioEngine.h"

#undef debug
#include "cedebug.h"

#define super IOAudioDevice

#undef kx_lock_acquire
#undef kx_lock_release

__int64 correctEndianess64(const __int64 number){
    return OSSwapHostToLittleInt64(number);
}

dword correctEndianess32(const dword number){
    return OSSwapHostToLittleInt32(number);
}

word  correctEndianess16(const word  number){
    return OSSwapHostToLittleInt16(number);
}

void writeLE32(dword* addr, const dword data){
	OSWriteLittleInt32(addr, 0, data);
}
void writeLE16(word* addr, const word data){
	OSWriteLittleInt16(addr, 0, data);
}

dword readLE32(const dword* addr){
	return (dword)OSReadLittleInt32( (volatile void *) addr, 0);
}

word readLE16(const word* addr){
	return (word)OSReadLittleInt16( (volatile void *) addr, 0);
}


#if defined(SYSTEM_IO)
#if !(defined(__i386__) || defined(__x86_64__) || defined(X86)) && (defined(PPC) || defined(ARM))

dword inpd_System(const io_port_t port, const word displacement){
    
    const word disp = displacement & 0xFFFF;
    
    register dword value = OSReadLittleInt32( (volatile void *) port, disp);
    OSSynchronizeIO();
    
    return value;
}

word inpw_System(const io_port_t port, const word displacement){
    
    const word disp = displacement & 0xFFFF;
    
    register word value = OSReadLittleInt16( (volatile void *)port, disp);
    OSSynchronizeIO();
    
    return value;
}

byte inp_System(const io_port_t port, const word displacement){
    
    const word disp = displacement & 0xFFFF;
    
    register byte value = ((volatile byte*)port)[disp];
    OSSynchronizeIO();
    
    return value;
}

void outpd_System(io_port_t port, const word displacement, const dword value){
    
    const word disp = displacement & 0xFFFF;
    
    OSWriteLittleInt32( (volatile void *)port, disp, value);
    OSSynchronizeIO();
}

void outpw_System(io_port_t port, const word displacement, const word value){
    
    const word disp = displacement & 0xFFFF;
    
    OSWriteLittleInt16( (volatile void *)port, disp, value);
    OSSynchronizeIO();
}

void outp_System(io_port_t port, const word displacement, const byte value){
    
    const word disp = displacement & 0xFFFF;
    
    ((volatile byte*)port)[disp] = value;
    OSSynchronizeIO();
}

#elif defined(__i386__) || defined(__x86_64__) || defined(X86)

dword inpd_System(const io_port_t port, const word displacement){
    dword value = 0;
    
    __asm__ volatile("inl %w1, %0" : "=a" (value) : "Nd" (port + displacement));
    
    return value;
}

word inpw_System(const io_port_t port, const word displacement){
    word value = 0;
    
    __asm__ volatile("inw %w1, %0" : "=a" (value) : "Nd" (port + displacement));
    
    return value;
}

byte inp_System(const io_port_t port, const word displacement){
    byte value = 0;
    
    __asm__ volatile("inb %w1, %b0" : "=a" (value) : "Nd" (port + displacement));
    
    return value;
}

void outpd_System(io_port_t port, const word displacement, const dword value){
    __asm__ volatile("outl %0, %w1" : : "a" (value), "Nd" (port + displacement));
}

void outpw_System(io_port_t port, const word displacement, const word value){
    __asm__ volatile("outw %0, %w1" : : "a" (value), "Nd" (port + displacement));
}

void outp_System(io_port_t port, const word displacement, const byte value){
    __asm__ volatile("outb %0, %w1" : : "a" (value), "Nd" (port + displacement));
}

#else

#error "Missing I/O implementation!!"

#endif
#endif

void kXAudioDevice::malloc_func(int len,void **b,int where)
{
	int *mem=(int *)IOMalloc(len+sizeof(int));
	if(mem)
	{
		*mem=len;
		mem++;
		*b=mem;
		
		memset(*b,0,len);
	}
	else
		*b=NULL;
}

void kXAudioDevice::free_func(void *buff)
{
	int *mem=(int *)buff;
	if(mem)
	{
		mem--;
		IOFree(mem,*mem);
	}
}

int kXAudioDevice::lmem_alloc_func(int len,void **lm,kx_cpu_cache_type_t cache_type)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	
	if(lm)
		*lm=NULL;
	
	return -1;
}

int kXAudioDevice::lmem_free_func(void **lm)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	return -1;
}

void * kXAudioDevice::lmem_get_addr_func(void **lm,int offset,__int64 *physical)
{
	IOLog("kXAudioDevice[%p]::lmem_alloc_func: NOT SUPPORTED (FIXME)\n",this);
	return NULL;
}

int kXAudioDevice::pci_alloc(struct memhandle *h, kx_cpu_cache_type_t cache_type)
{

#if defined(OLD_ALLOC)

	#warning "Using old dma memory allocation method"

    IOPhysicalAddress physical;
    h->addr=IOMallocContiguous((vm_size_t)h->size,PAGE_SIZE,&physical);
    h->dma_handle = (dword)physical;
    
    if (!(h->addr) || !(h->dma_handle))
        return -1;
#else

    //h->addr=IOMallocContiguous(h->size,PAGE_SIZE,&phys_addr);
    mach_vm_address_t mask = kx_allocation_mask; //0x000000007FFFFFFFULL & ~(PAGE_SIZE - 1);
    
    h->desc = IOBufferMemoryDescriptor::inTaskWithPhysicalMask(
                                                               kernel_task,
                                                               kIODirectionInOut | kIOMemoryPhysicallyContiguous,
                                                               h->size,
                                                               mask);
    
    if (!h->desc)
        return -1;
    
    h->desc->prepare();
    
    h->addr =              h->desc->getBytesNoCopy    ();
    h->dma_handle = (dword)h->desc->getPhysicalAddress();
#endif
    
    //buffer cleaning
    bzero((byte*)h->addr, h->size); 
    
    return 0;
}

void kXAudioDevice::pci_free(struct memhandle *h)
{
#if defined(OLD_ALLOC)
	
	#warning "Using old dma memory allocation method"

    IOFreeContiguous(h->addr,h->size);
#else
	h->desc->release();
#endif
	memset(h, 0, sizeof(*h));
}


void kXAudioDevice::send_message(int len,const void *b)
{
	// fixme !!
	debug("kXAudioDevice[%p]::send_message: notification message\n",this);
}

void kXAudioDevice::notify_func(void *data,int what)
{
	if(what==LLA_NOTIFY_TIMER)
	{
		debug("!! kXAudioDevice::notify: timer notification [should not happen!]\n");
	}
	else
	if(what==LLA_NOTIFY_EVENT)
	{
		#if 0
		// this is no longer necessary
		// event from ASIO ('buffer switch')
		// move this code to kXAudioDevice::interruptFilter and remove KXASIO_METHOD_SEND_EVENT
		if(data)
		{
			kXAudioEngine *that=(kXAudioEngine *)data;
			
			if(that->hw && that->hw->asio_notification_krnl.pb_buf==0)
			{
				// cur_pos is ~0 samples
				if(that->hw->asio_notification_krnl.cur_pos<=10)
					that->takeTimeStamp();
				else
					debug("!! kXAudioDevice::notify: invalid current position [%x]\n",that->hw->asio_notification_krnl.cur_pos);
			}
		}
		#else
		debug("!! kXAudioDevice::notify: invalid notification event\n");
		#endif
	}
	else
		debug("!! kXAudioDevice::notify: invalid notification event [%d]\n",what);
}

void kXAudioDevice::sync(sync_data*s)
{
	// this should call kx_sync(s); synchronized with any ISR/IRQ handlers
	// FIXME !!
	kx_sync(s);
}

int kXAudioDevice::debug_func(int where,const char *__format, ... )
{
	if(where<=0) return 0; // ignore this log
	
	char string[256];
	
	const char *subop="??";
	switch(where)
	{
		case  DERR: subop="ERROR"; break;
		case  DLIB: subop="HAL"; break;
	}
	
	va_list ap;
	va_start(ap, __format);
	vsnprintf(string,sizeof(string),__format,ap);
	IOLog("kXAudioDevice [%s] %s",subop,string);
	va_end(ap);
	
	return 0;
}

void kXAudioDevice::get_physical(kx_voice_buffer *buff,const dword offset, dword *physical)
{
	//*physical=(__int64)(buff->physical+offset);
    (*physical)=(buff->physical+offset);
}

void kXAudioDevice::save_fpu_state(kx_fpu_state *state)
{
	// fixme !!
}

void kXAudioDevice::rest_fpu_state(kx_fpu_state *state)
{
	// fixme !!
}

#undef kx_spin_lock_init
KX_API(void,kx_spin_lock_init(kx_hw *hw,spinlock_t *lock,const char *name))
{
	lock->lock=IORecursiveLockAlloc();
	lock->name=name;
	
	lock->file=NULL;
	lock->line=-1;
	lock->kx_lock=0;
}

#undef kx_lock_acquire
KX_API(void,kx_lock_acquire(kx_hw *hw, spinlock_t *lock, unsigned long *,const char *file,int line))
{
	IORecursiveLockLock(lock->lock);
	lock->kx_lock++;
	lock->file=file;
	lock->line=line;
}

#undef kx_lock_release
KX_API(void,kx_lock_release(kx_hw *hw, spinlock_t *lock, unsigned long *,const char *file,int line))
{
	lock->kx_lock--;
	lock->file=file;
	lock->line=line;
	IORecursiveLockUnlock(lock->lock);
}

double kx_log2(register double _x_)
{
	// FIXME
	return 0.0;
}

double kx_log10(register double _x_)
{
	// FIXME
	return 0.0;
}

double kx_pow2(register double _y_)
{
	// fixme !!
	return 0.0;
}

double kx_pow10(register double y)
{
	// fixme !!
	return 0.0;
}

double kx_sqrt(register double y)
{
	// FIXME !!
	return 0.0;
}
