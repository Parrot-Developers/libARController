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
 * @file ARCONTROLLER_StreamQueue.c
 * @brief ARCONTROLLER_StreamQueue 
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include <libuthash/uthash.h>
#include <libuthash/utlist.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Frame.h>
#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_StreamQueue.h>

#include "ARCONTROLLER_StreamQueue.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/
 
ARCONTROLLER_StreamQueue_t *ARCONTROLLER_StreamQueue_New (ARCONTROLLER_StreamPool_t *pool, uint32_t capacity, int flushOnIFrame, eARCONTROLLER_ERROR *error)
{
    // -- Create a new streamQueue --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_StreamQueue_t *streamQueue =  NULL;
    
    // check parameters
    if (pool == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the StreamQueue
        streamQueue = malloc (sizeof (ARCONTROLLER_StreamQueue_t));
        if (streamQueue != NULL)
        {
            // Initialize to default values
            streamQueue->pool = pool;
            
            streamQueue->frames = NULL;
            //streamQueue->frameCapacity = 0;
            //streamQueue->numberOfFrame = 0;
            
            streamQueue->flushOnIFrame = flushOnIFrame;
            streamQueue->semaphore = NULL;
            
            // Create the mutex/condition 
            if ((ARSAL_Mutex_Init (&(streamQueue->mutex)) != 0) ||
                (ARSAL_Sem_Init (&(streamQueue->semaphore), 0, 0) != 0))
            {
                localError = ARCONTROLLER_ERROR_INIT_MUTEX;
            }
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    /*
    if ((localError == ARCONTROLLER_OK) && (capacity > 0))
    {
        //allocate the array
        streamQueue->frames = calloc (sizeof (ARCONTROLLER_Frame_t*), capacity);
        if (streamQueue->frames != NULL)
        {
            streamQueue->frameCapacity = capacity;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }*/
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return streamQueue;
}

void ARCONTROLLER_StreamQueue_Delete (ARCONTROLLER_StreamQueue_t **streamQueue)
{
    // -- Delete a StreamQueue --

    if (streamQueue != NULL)
    {
        if ((*streamQueue) != NULL)
        {
            ARSAL_Mutex_Destroy (&((*streamQueue)->mutex));
            ARSAL_Sem_Destroy (&((*streamQueue)->semaphore));
            
            ARCONTROLLER_StreamQueue_Flush (*streamQueue);
            /*free (*streamQueue)->frames);
            (*streamQueue)->frames = NULL;*/
            
            free (*streamQueue);
            (*streamQueue) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamQueue_Push (ARCONTROLLER_StreamQueue_t *streamQueue, ARCONTROLLER_Frame_t *frame)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "ARCONTROLLER_StreamQueue_Push...." );
    // -- Push a Frame --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_StreamQueue_element_t *newElement = NULL;
    //int newCapacity = 0;
    int locked = 0;
    
    // Check Parameters
    if ((streamQueue == NULL) ||
        (frame == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "lock...." );
        ARSAL_Mutex_Lock (&(streamQueue->mutex));
        locked = 1;
        //TODO manage Error !!!!!!!!!!!!
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if ((streamQueue->flushOnIFrame) && (frame->isIFrame))
        {
            //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "ARCONTROLLER_StreamQueue_LocalFlush...." );
            error = ARCONTROLLER_StreamQueue_LocalFlush (streamQueue);
            //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "ARCONTROLLER_StreamQueue_LocalFlush error:%d", error);
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        newElement = malloc (sizeof(ARCONTROLLER_StreamQueue_element_t));
        if (newElement != NULL)
        {
            newElement->frame = frame;
            DL_APPEND (streamQueue->frames, newElement);
            ARSAL_Sem_Post (&(streamQueue->semaphore));
        }
        else
        {
            error = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (locked)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "unlock...." );
        ARSAL_Mutex_Unlock (&(streamQueue->mutex));
        locked = 0;
    }

    return error;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_Pop (ARCONTROLLER_StreamQueue_t *streamQueue, eARCONTROLLER_ERROR *error)
{
    // -- Pop a Frame --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // check parameters
    if (streamQueue == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "lock...." );
        ARSAL_Mutex_Lock (&(streamQueue->mutex));
        
        if (ARSAL_Sem_Wait (&(streamQueue->semaphore)))
        {
            frame = ARCONTROLLER_StreamQueue_LocalPopFrame (streamQueue);
        }
        
        if (frame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY;
        }
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "unlock...." );
        ARSAL_Mutex_Unlock (&(streamQueue->mutex));
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_TryPop (ARCONTROLLER_StreamQueue_t *streamQueue, eARCONTROLLER_ERROR *error)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "ARCONTROLLER_StreamQueue_TryPop...." );
    // -- Try to Pop a Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // check parameters
    if (streamQueue == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "lock...." );
        ARSAL_Mutex_Lock (&(streamQueue->mutex));
        
        frame = ARCONTROLLER_StreamQueue_LocalPopFrame (streamQueue);
        
        if (frame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY;
        }
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "unlock...." );
        ARSAL_Mutex_Unlock (&(streamQueue->mutex));
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_PopWithTimeout (ARCONTROLLER_StreamQueue_t *streamQueue, uint32_t timeoutMs, eARCONTROLLER_ERROR *error)
{
    // -- Pop a frame with timeout --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    struct timespec semTimeout; //TODO
    
    // check parameters
    if (streamQueue == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "lock...." );
        ARSAL_Mutex_Lock (&(streamQueue->mutex));
        
        // convert timeoutMs in timespec
        semTimeout.tv_sec = timeoutMs / 1000;
        semTimeout.tv_nsec = (timeoutMs % 1000) * 1000000;
        
        if (ARSAL_Sem_Timedwait (&(streamQueue->semaphore), &semTimeout))
        {
            frame = ARCONTROLLER_StreamQueue_LocalPopFrame (streamQueue);
        }
        
        if (frame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY;
        }
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "unlock...." );
        ARSAL_Mutex_Unlock (&(streamQueue->mutex));
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamQueue_Flush (ARCONTROLLER_StreamQueue_t *streamQueue)
{
    // -- Flush the Queue --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // check parameters
    if (streamQueue == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "lock...." );
        ARSAL_Mutex_Lock (&(streamQueue->mutex));
        
        error = ARCONTROLLER_StreamQueue_LocalFlush (streamQueue);
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_QUEUE_TAG, "unlock...." );
        ARSAL_Mutex_Unlock (&(streamQueue->mutex));
    }

    return error;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/
/*
ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_PopWithDispatchTime (ARCONTROLLER_StreamQueue_t *streamQueue, const struct timespec timeout, eARCONTROLLER_ERROR *error)
{
    // -- Pop a frame --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    int index = 0;
    
    // check parameters
    if (streamQueue == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        if (ARSAL_Sem_Timedwait (&(streamQueue->semaphore), timeout))
        {
            frame = ARCONTROLLER_StreamQueue_LocalPop (streamQueue);
        }
        
        if (frame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY;
        }
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}*/

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_LocalPopFrame (ARCONTROLLER_StreamQueue_t *streamQueue)
{
    // -- Pop a frame --

    ARCONTROLLER_Frame_t *frame = NULL;
   
    //frame = streamQueue->frames[0];
    if (streamQueue->frames != NULL)
    {
        frame = streamQueue->frames->frame;
        DL_DELETE (streamQueue->frames, streamQueue->frames);
    }
    //NO ELSE ; no frame
    
    return frame;
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamQueue_LocalFlush (ARCONTROLLER_StreamQueue_t *streamQueue)
{
    // -- Flush the Queue --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    frame = ARCONTROLLER_StreamQueue_LocalTryPop (streamQueue, &error);
    while (frame != NULL)
    {
        //TODO manage error !!!!!!!!!!!!!!
        error = ARCONTROLLER_Frame_SetFree (frame);
        frame = ARCONTROLLER_StreamQueue_LocalTryPop (streamQueue, &error);
    }
    
    if (error == ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY)
    {
        error = ARCONTROLLER_OK;
    }

    return error;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamQueue_LocalTryPop (ARCONTROLLER_StreamQueue_t *streamQueue, eARCONTROLLER_ERROR *error)
{
    // -- Try to Pop a Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
        
    if (ARSAL_Sem_Trywait (&(streamQueue->semaphore)))
    {
        frame = ARCONTROLLER_StreamQueue_LocalPopFrame (streamQueue);
    }
    
    if (frame == NULL)
    {
        localError = ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY;
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}
