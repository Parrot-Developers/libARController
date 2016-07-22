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
 * @file ARCONTROLLER_Stream1.h
 * @brief ARCONTROLLER_Stream1 allow to operate ARStream1 for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM1_PRIVATE_H_
#define _ARCONTROLLER_STREAM1_PRIVATE_H_

#include <libARSAL/ARSAL_Socket.h>
#include <libARStream/ARStream.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>

#include <libARController/ARCONTROLLER_Stream.h>

#include <libARController/ARCONTROLLER_Stream1.h>
#define ARCONTROLLER_STREAM1_TAG "ARCONTROLLER_Stream1"

#define ARCONTROLLER_STREAM1_DEFAULT_VIDEO_FRAGMENT_SIZE 1000
#define ARCONTROLLER_STREAM1_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER 128

/**
 * @brief Stream1 controller allow to operate ARStream1 for receive a stream.
 */
struct ARCONTROLLER_Stream1_t
{
    ARDISCOVERY_NetworkConfiguration_t *networkConfiguration; /**< the networkConfiguration of the device*/
    ARSTREAM_Reader_t *streamReader; /**< reader of the stream */
    int fragmentSize; /**< Size maximum of the stream fragment */
    int maxNumberOfFragment; /**< Number maximum of stream fragment */
    int maxAckInterval; /**< Maximum of acknowledge interval on the stream */
    ARSAL_Thread_t dataThread; /**< video receiver thread */
    ARSAL_Thread_t ackThread; /**< acknowledge send thread */
    ARSAL_Thread_t readerThread; /**< thread to read the ready frames */
    int isRunning; /**< 0 if the stream is stopped ; otherwide the stream is running */
    ARCONTROLLER_StreamPool_t *framePool; /**< pool of frame */
    ARCONTROLLER_StreamQueue_t *readyQueue; /**< ready frames */
    ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback; /**< callback when a frame is received */
    ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback;
    ARCONTROLLER_Stream_TimeoutFrameCallback_t timeoutFrameCallback; /**< callback when timeout */
    ARCONTROLLER_Stream_Codec_t codec;
    void *callbackCustomData;  /**< custom data to send to callbacks */
    int decoderConfigCalled;  /**< different of 0 if the decoderConfigCallback has been called; */
};

 /**
 * @brief Pop a frame from the ready frame queue.
 * @warning This is a blocking function, waits a frame push if the queue is empty. 
 * @param stream1Controller The stream controller.
 * @param[out] error Executing error.
 * @return The frame pop ; Can be null if an error occurs.
 */
ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_GetFrame (ARCONTROLLER_Stream1_t *stream1Controller, eARCONTROLLER_ERROR *error);

/**
 * @brief Try to pop a frame from the ready frame queue.
 * @note If the queue is empty, returns null, and error is equals to ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY.
 * @param stream1Controller The stream controller.
 * @param[out] error Executing error.
 * @return The frame pop ; Can be null if an error occurs.
 */
ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_TryGetFrame (ARCONTROLLER_Stream1_t *stream1Controller, eARCONTROLLER_ERROR *error);

/**
 * @brief Pop a frame from the ready frame queue, with timeout.
 * @warning This is a blocking function; If the queue is empty yet after the timeout, returns null, and error is equals to ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY.
 * @param stream1Controller The stream controller.
 * @param[out] error Executing error.
 * @return The frame pop ; Can be null if an error occurs.
 */
ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_GetFrameWithTimeout (ARCONTROLLER_Stream1_t *stream1Controller, uint32_t timeoutMs, eARCONTROLLER_ERROR *error);

/**
 * @brief Initilize Stream 1 buffers.
 * @param stream1Controller The stream controller.
 * @return Executing error.
 */
eARCONTROLLER_ERROR ARCONTROLLER_Stream1_InitStream1Buffers (ARCONTROLLER_Stream1_t *stream1Controller);

uint8_t* ARCONTROLLER_Stream1_FrameCompleteCallback (eARSTREAM_READER_CAUSE cause, uint8_t *framePointer, uint32_t frameSize, int numberOfSkippedFrames, int isFlushFrame, uint32_t *newBufferCapacity, void *custom);

#endif /* _ARCONTROLLER_STREAM1_PRIVATE_H_ */
