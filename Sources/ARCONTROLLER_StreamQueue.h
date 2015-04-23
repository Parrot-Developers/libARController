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
 * @file ARNETWORK_StreamQueue.h
 * @brief ARNETWORK_StreamQueue 
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM_QUEUE_PRIVATE_H_
#define _ARCONTROLLER_STREAM_QUEUE_PRIVATE_H_

#include <libARSAL/ARSAL_Sem.h>
#include <libARSAL/ARSAL_Mutex.h>
#include <libARController/ARCONTROLLER_Frame.h>
#include <libARController/ARCONTROLLER_StreamQueue.h>

#define ARCONTROLLER_STREAM_QUEUE_TAG "ARNETWORK_StreamQueue"

typedef struct ARCONTROLLER_StreamQueue_element_t ARCONTROLLER_StreamQueue_element_t;

struct ARCONTROLLER_StreamQueue_element_t
{
    ARCONTROLLER_Frame_t *frame; /**< Frame used to store stream data */
    
    ARCONTROLLER_StreamQueue_element_t *next; /**<  Next element ; Do Not Modify */
    ARCONTROLLER_StreamQueue_element_t *prev; /**<  Previous element ; Do Not Modify */
};

struct ARCONTROLLER_StreamQueue_t
{
    ARCONTROLLER_StreamQueue_element_t *frames; /**< Queue of frames ready. */
    int flushOnIFrame; /**< Flag to activate the flush of the frame queue when a IFrame is pushed ; '1' the flush is activated ; '0' the flush is  not activated. */
    ARSAL_Mutex_t mutex; /**< Mutex to prevent concurrent access on "frames" */
    ARSAL_Sem_t semaphore; /**< Semaphore of number of frame in the frame queue */
};

// TODO ADD !!!!!!!!!
ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_LocalTryPop (ARCONTROLLER_StreamQueue_t *streamQueue, eARCONTROLLER_ERROR *error);

// TODO ADD !!!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_StreamQueue_LocalFlush (ARCONTROLLER_StreamQueue_t *streamQueue);

// TODO ADD !!!!!!!!!
ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_LocalPopFrame (ARCONTROLLER_StreamQueue_t *streamQueue);

#endif /* _ARCONTROLLER_STREAM_QUEUE_PRIVATE_H_ */
