/*
    Copyright (C) 2016 Parrot SA

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
 * @file ARCONTROLLER_StreamSender.h
 * @brief ARCONTROLLER_StreamSender allow to operate ARStream for send a stream.
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM_SENDER_PRIVATE_H_
#define _ARCONTROLLER_STREAM_SENDER_PRIVATE_H_

#include <libARStream/ARStream.h>
#include <libARNetwork/ARNetwork.h>
#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_StreamSender.h>

#include "ARCONTROLLER_AudioHeader.h"

#define ARCONTROLLER_STREAM_SENDER_TAG "ARCONTROLLER_StreamSender"

/**
 * @brief Stream controller allow to operate ARStream for receive a stream.
 */
struct ARCONTROLLER_StreamSender_t
{
    ARDISCOVERY_NetworkConfiguration_t *networkConfiguration; /**< the networkConfiguration of the device*/
    ARSTREAM_Sender_t *streamSender; /**< sender of the stream */
    int isRunning; /**< 0 if the stream is stopped ; otherwide the stream is running */
    int fragmentSize; /**< Size maximum of the stream fragment */
    int maxFragmentSize;
    int maxNumberOfFragment; /**< Number maximum of stream fragment */
    int maxAckInterval; /**< Maximum of acknowledge interval on the stream */
    ARCONTROLLER_StreamPool_t *framePool; /**< pool of frame */
    ARSAL_Thread_t dataThread; /**< sender thread */
    ARSAL_Thread_t ackThread; /**< acknowledge receiver thread */
    /* audio stream part */
    ARCONTROLLER_AudioHeader_t audioHeader;
};

#endif /* _ARCONTROLLER_STREAM_SENDER_PRIVATE_H_ */
