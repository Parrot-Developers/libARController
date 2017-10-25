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
 * @file ARCONTROLLER_Stream2.h
 * @brief ARCONTROLLER_Stream2 allow to operate ARStream for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM2_PRIVATE_H_
#define _ARCONTROLLER_STREAM2_PRIVATE_H_

#include <json-c/json.h>
#include <libARSAL/ARSAL_Socket.h>
#include <libARSAL/ARSAL_Thread.h>
#include <libARController/ARCONTROLLER_Error.h>
#include <libARStream2/arstream2_stream_receiver.h>
#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_Stream.h>
#include <libARController/ARCONTROLLER_Stream2.h>

struct mux_ctx;

#define ARCONTROLLER_STREAM2_TAG "ARCONTROLLER_Stream2"

#define ARCONTROLLER_STREAM2_CLIENT_STREAM_PORT 55004
#define ARCONTROLLER_STREAM2_CLIENT_CONTROL_PORT 55005

#define ARCONTROLLER_STREAM2_AU_SIZE 1048576
#define ARCONTROLLER_STREAM2_AU_ARRAY_SIZE 5
#define ARCONTROLLER_STREAM2_IP_SIZE 20

#define ARCONTROLLER_STREAM2_MAX_RESYNC_ERROR 10

/**
 * @brief Stream controller allow to operate ARStream for receive a stream.
 */
struct ARCONTROLLER_Stream2_t
{
    int isRunning; /**< 0 if the stream is stopped ; otherwide the stream is running */
    
    ARSTREAM2_StreamReceiver_Handle streamReceiverHandle;
    
    ARSAL_Thread_t networkThread; /**< Stream2 StreamReceiver network thread */
    ARSAL_Thread_t appOutputThread; /**< Stream2 StreamReceiver app output thread */
    
    char serverAddress[ARCONTROLLER_STREAM2_IP_SIZE]; /**< Server address */
    struct mux_ctx *mux;
    int clientStreamPort; /**< Client stream port */
    int clientStreamFd; /**< Client stream fd */
    int clientControlPort; /**< Client control port */
    int clientControlFd; /**< Client control fd */
    int serverStreamPort; /**< Server stream port */
    int serverControlPort; /**< Server control port */
    int maxPacketSize; /**< Maximum network packet size in bytes */
    int qos_level;
    int ardiscoveryProductType;
    
    int errorCount;
    int replaceStartCodesWithNaluSize; /**< if 1, replace the NAL units start code with the NALU size */
    
    ARCONTROLLER_StreamPool_t *framePool; /**< pool of frame */
    
    void *callbackData;
    ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback;
    ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback;
};

#endif /* _ARCONTROLLER_STREAM2_PRIVATE_H_ */
