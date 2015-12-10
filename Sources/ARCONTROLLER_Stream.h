/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
/**
 * @file ARCONTROLLER_Stream.h
 * @brief ARCONTROLLER_Stream allow to operate ARStream for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM_PRIVATE_H_
#define _ARCONTROLLER_STREAM_PRIVATE_H_

#include <libARSAL/ARSAL_Socket.h>
#include <libARStream/ARStream.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>

#include <libARController/ARCONTROLLER_Stream1.h>
#include <libARController/ARCONTROLLER_Stream2.h>

#include <libARController/ARCONTROLLER_Stream.h>

#define ARCONTROLLER_STREAM_TAG "ARCONTROLLER_Stream"

#define ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_SIZE 1000
#define ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER 128

/**
 * @brief Stream controller allow to operate ARStream for receive a stream.
 */
struct ARCONTROLLER_Stream_t
{
    int isRunning; /**< 0 if the stream is stopped ; otherwide the stream is running */
    
    //stream 1
    ARCONTROLLER_Stream1_t *stream1Controller;
    
    //stream 2
    ARCONTROLLER_Stream2_t *stream2Controller;
    
};

uint8_t* ARCONTROLLER_Stream_FrameCompleteCallback (eARSTREAM_READER_CAUSE cause, uint8_t *framePointer, uint32_t frameSize, int numberOfSkippedFrames, int isFlushFrame, uint32_t *newBufferCapacity, void *custom);

#endif /* _ARCONTROLLER_STREAM_PRIVATE_H_ */
