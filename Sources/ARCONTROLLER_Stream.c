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
 * @file ARNETWORK_Stream.c
 * @brief ARCONTROLLER_Stream allow to operate ARStream for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <json/json.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Socket.h>
#include <libARStream/ARStream.h>
#include <libARNetwork/ARNetwork.h>
#include <libARDiscovery/ARDISCOVERY_Error.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <libARController/ARCONTROLLER_Network.h>
#include <libARController/ARCONTROLLER_Frame.h>
#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_StreamQueue.h>

#include <libARController/ARCONTROLLER_Stream.h>

#include "ARCONTROLLER_Stream.h"

/*************************
 * Private header
 *************************/

//TODO add !!!!!!!!!!!!!
int ARCONTROLLER_Stream_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id);

void* ARCONTROLLER_Stream_ReaderThreadRun (void *data);

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Stream Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Stream_t *streamController =  NULL;
    
    // Check parameters
    if (networkConfiguration == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Network Controller
        streamController = malloc (sizeof (ARCONTROLLER_Stream_t));
        if (streamController != NULL)
        {
            // Initialize to default values
            streamController->networkConfiguration = networkConfiguration;
            
            streamController->fragmentSize = ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_SIZE;
            streamController->maxNumberOfFragement = ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER;
            streamController->maxAckInterval = ARSTREAM_READER_MAX_ACK_INTERVAL_DEFAULT;
            streamController->dataThread = NULL;
            streamController->ackThread = NULL;
            streamController->readerThread = NULL;
            streamController->isRunning = 0;
            streamController->framePool = NULL;
            streamController->readyQueue = NULL;
            streamController->receiveFrameCallback = NULL;
            streamController->timeoutFrameCallback = NULL;
            streamController->receiveFrameCustomData = NULL;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // create the frame pool
        streamController->framePool = ARCONTROLLER_StreamPool_New (ARCONTROLLER_STREAMPOOL_DEFAULT_SIZE, &localError);
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // create the frame queue
        streamController->readyQueue = ARCONTROLLER_StreamQueue_New (1, &localError);
    }

    // delete the Network Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_Stream_Delete (&streamController);
    }
    // No else: skipped by an error 

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return streamController;
}

void ARCONTROLLER_Stream_Delete (ARCONTROLLER_Stream_t **streamController)
{
    // -- Delete the Stream Controller --

    if (streamController != NULL)
    {
        if ((*streamController) != NULL)
        {
            ARCONTROLLER_Stream_Stop (*streamController);

            // Create the Frame Pool
            ARCONTROLLER_StreamPool_Delete (&((*streamController)->framePool));

            // Create the Frame Queue
            ARCONTROLLER_StreamQueue_Delete (&((*streamController)->readyQueue));
            
            free (*streamController);
            (*streamController) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_Start (ARCONTROLLER_Stream_t *streamController, ARNETWORK_Manager_t *networkManager)
{
    // -- Start to read the stream --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARSTREAM_ERROR streamError = ARSTREAM_OK;
    ARCONTROLLER_Frame_t *firstFrame = NULL;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (!streamController->isRunning))
    {
        streamController->isRunning = 1;
        firstFrame = ARCONTROLLER_StreamPool_GetNextFreeFrame (streamController->framePool, &error);
        
        if (error == ARCONTROLLER_OK)
        {
            streamController->streamReader = ARSTREAM_Reader_New (networkManager, streamController->networkConfiguration->deviceToControllerARStreamData, streamController->networkConfiguration->controllerToDeviceARStreamAck, ARCONTROLLER_Stream_FrameCompleteCallback, firstFrame->data, firstFrame->capacity, streamController->fragmentSize, streamController->maxAckInterval, streamController, &streamError);
        
            if (streamError != ARSTREAM_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Error while creating streamReader : %s ", ARSTREAM_Error_ToString (streamError));
                error = ARCONTROLLER_ERROR_INIT_STREAM;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create (&(streamController->dataThread), ARSTREAM_Reader_RunDataThread, streamController->streamReader) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Creation of Data thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create(&(streamController->ackThread), ARSTREAM_Reader_RunAckThread, streamController->streamReader) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Creation of Ack thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create(&(streamController->readerThread), ARCONTROLLER_Stream_ReaderThreadRun, streamController) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Creation of reader thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error != ARCONTROLLER_OK)
        {
            ARCONTROLLER_Stream_Stop (streamController);
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_Stop (ARCONTROLLER_Stream_t *streamController)
{
    // -- Stop to read the stream --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (streamController->isRunning))
    {
        streamController->isRunning = 0;
        
        ARSTREAM_Reader_StopReader (streamController->streamReader);
        
        if (streamController->dataThread != NULL)
        {
            ARSAL_Thread_Join(streamController->dataThread, NULL);
            ARSAL_Thread_Destroy(&(streamController->dataThread));
            streamController->dataThread = NULL;
        }
        
        if (streamController->ackThread != NULL)
        {
            ARSAL_Thread_Join(streamController->ackThread, NULL);
            ARSAL_Thread_Destroy(&(streamController->ackThread));
            streamController->ackThread = NULL;
        }
        
        if (streamController->readerThread != NULL)
        {
            ARSAL_Thread_Join(streamController->readerThread, NULL);
            ARSAL_Thread_Destroy(&(streamController->readerThread));
            streamController->readerThread = NULL;
        }
        
        ARSTREAM_Reader_Delete (&(streamController->streamReader));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_SetReceiveFrameCallback (ARCONTROLLER_Stream_t *streamController, ARNETWORKAL_Stream_DidReceiveFrameCallback_t receiveFrameCallback, ARNETWORKAL_Stream_TimeoutFrameCallback_t timeoutFrameCallback, void *customData)
{
    // -- Stop to read the stream --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        streamController->receiveFrameCallback = receiveFrameCallback;
        streamController->timeoutFrameCallback = timeoutFrameCallback;
        streamController->receiveFrameCustomData = customData;
    }
    
    return error;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream_GetFrame (ARCONTROLLER_Stream_t *streamController, eARCONTROLLER_ERROR *error)
{
    // -- Get Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (streamController == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (streamController->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_Pop (streamController->readyQueue, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream_TryGetFrame (ARCONTROLLER_Stream_t *streamController, eARCONTROLLER_ERROR *error)
{
    // -- Try to Get a Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (streamController == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (streamController->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_TryPop (streamController->readyQueue, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream_GetFrameWithTimeout (ARCONTROLLER_Stream_t *streamController, uint32_t timeoutMs, eARCONTROLLER_ERROR *error)
{
    // -- Get a Frame with timeout --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (streamController == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((localError == ARCONTROLLER_OK) && (streamController->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_PopWithTimeout (streamController->readyQueue, timeoutMs, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

eARDISCOVERY_ERROR ARCONTROLLER_Stream_OnSendJson (ARCONTROLLER_Stream_t *streamController, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    //json_object *valueJsonObj = NULL;
    
    // Check parameters
    if ((jsonObj == NULL) ||
        (streamController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream_OnReceiveJson (ARCONTROLLER_Stream_t *streamController, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    eARCONTROLLER_ERROR controllerError = ARCONTROLLER_OK;
    
    json_object *valueJsonObj = NULL;
    
    if ((jsonObj == NULL) ||
        (streamController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY);
        if (valueJsonObj != NULL)
        {
            streamController->fragmentSize = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY);
        if (valueJsonObj != NULL)
        {
            streamController->maxNumberOfFragement = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY);
        if (valueJsonObj != NULL)
        {
            streamController->maxAckInterval = json_object_get_int(valueJsonObj);
        }
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // Initialization of the stream buffers
        controllerError = ARCONTROLLER_Stream_InitStreamBuffers (streamController);
        
        if (controllerError != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Error occurred durring Initialization of the stream buffers : %s ", ARCONTROLLER_Error_ToString (controllerError));
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_InitStreamBuffers (ARCONTROLLER_Stream_t *streamController)
{
    // -- Connection callback to receive the Json --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int streamAckBufferIndex = -1;
    int streamDataBufferIndex = -1;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        streamAckBufferIndex = ARCONTROLLER_Stream_IdToIndex (streamController->networkConfiguration->controllerToDeviceParams, streamController->networkConfiguration->numberOfControllerToDeviceParam, streamController->networkConfiguration->controllerToDeviceARStreamAck);
        streamDataBufferIndex = ARCONTROLLER_Stream_IdToIndex (streamController->networkConfiguration->deviceToControllerParams, streamController->networkConfiguration->numberOfDeviceToControllerParam, streamController->networkConfiguration->deviceToControllerARStreamData);
                
        if ((streamAckBufferIndex != -1) &&
           (streamDataBufferIndex != -1))
        {
            ARSTREAM_Reader_InitStreamAckBuffer (&(streamController->networkConfiguration->controllerToDeviceParams[streamAckBufferIndex]), streamController->networkConfiguration->controllerToDeviceARStreamAck);
            ARSTREAM_Reader_InitStreamDataBuffer (&(streamController->networkConfiguration->deviceToControllerParams[streamDataBufferIndex]), streamController->networkConfiguration->deviceToControllerARStreamData, streamController->fragmentSize, streamController->maxNumberOfFragement);
        }
        //NO ELSE ; device has not streaming
    }
    
    return error;
}

uint8_t* ARCONTROLLER_Stream_FrameCompleteCallback (eARSTREAM_READER_CAUSE cause, uint8_t *frameData, uint32_t frameSize, int numberOfSkippedFrames, int isFlushFrame, uint32_t *newBufferCapacity, void *custom)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_FrameCompleteCallback ....");
    
    // -- Reader Callback for ARStream --
    static ARCONTROLLER_Frame_t *oldFrame;
    ARCONTROLLER_Stream_t *streamController = (ARCONTROLLER_Stream_t *)custom;
    ARCONTROLLER_StreamPool_t *pool = NULL;
    ARCONTROLLER_StreamQueue_t *queue = NULL;
    ARCONTROLLER_Frame_t *frame = NULL;
    uint8_t *resFrameData = NULL;
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if ((streamController == NULL) ||
        (streamController->framePool == NULL) ||
        (streamController->readyQueue == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        pool = streamController->framePool;
        queue = streamController->readyQueue;
    }
    
    if (error == ARCONTROLLER_OK)
    {
        switch (cause)
        {
            case ARSTREAM_READER_CAUSE_FRAME_COMPLETE:
                frame = ARCONTROLLER_StreamPool_GetFrameFromData (pool, frameData, &error);
                
                if (error == ARCONTROLLER_OK)
                {
                    frame->isIFrame = (isFlushFrame == 1) ? 1 : 0;
                    frame->used = frameSize;
                    frame->missed = numberOfSkippedFrames;
                    ARCONTROLLER_StreamQueue_Push (queue, frame);
                    
                    frame = ARCONTROLLER_StreamPool_GetNextFreeFrame (pool, &error);
                }
                
                break;
                
            case ARSTREAM_READER_CAUSE_FRAME_TOO_SMALL:
                if (frameData != NULL)
                {
                    frame = ARCONTROLLER_StreamPool_GetFrameFromData (pool, frameData, &error);
                }
                
                oldFrame = frame;
                frame = ARCONTROLLER_StreamPool_GetNextFreeFrame (pool, &error);
                
                if (error == ARCONTROLLER_OK)
                {
                    ARCONTROLLER_Frame_ensureCapacityIsAtLeast (frame, *newBufferCapacity, &error);
                }
                
                break;
                
            case ARSTREAM_READER_CAUSE_COPY_COMPLETE:
                error = ARCONTROLLER_Frame_SetFree (oldFrame);
                oldFrame = NULL;
                
                if (error == ARCONTROLLER_OK)
                {
                    frame = ARCONTROLLER_StreamPool_GetFrameFromData (pool, frameData, &error);
                }
                break;
                
            case ARSTREAM_READER_CAUSE_CANCEL:
                frame = ARCONTROLLER_StreamPool_GetFrameFromData (pool, frameData, &error);
                if (error == ARCONTROLLER_OK)
                {
                    error = ARCONTROLLER_Frame_SetFree (frame);
                }
                break;
                
            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "cause %d not known", cause);
                error = ARCONTROLLER_ERROR;
                break;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        *newBufferCapacity = frame->capacity;
        resFrameData = frame->data;
    }
    else
    {
        //Error occured
        *newBufferCapacity = 0;
        resFrameData = NULL;
        
        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "cause : %d | error: %s", cause, ARCONTROLLER_Error_ToString (error));
    }

    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_FrameCompleteCallback end resFrameData : %d....", resFrameData);

    return resFrameData;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

int ARCONTROLLER_Stream_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id)
{
    // -- Get parameter buffer from ID --
    
    int index = 0;
    int indexOfId = -1;
    
    // Check parameters
    if (parameters != NULL)
    {
        for (index = 0 ; index < numberOfParameters ; index++)
        {
            if (parameters[index].ID == id)
            {
                indexOfId = index;
                break; // indexOfId found ; break on the loop
            }
        }
    }
    return indexOfId;
}

void* ARCONTROLLER_Stream_ReaderThreadRun (void *data)
{
    // -- Manage the reception of the Video -- 
    
    // local declarations 
    ARCONTROLLER_Stream_t *streamController = data;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (streamController != NULL)
    {
        while (streamController->isRunning)
        {
            frame = ARCONTROLLER_Stream_GetFrameWithTimeout (streamController, 500, &error);
            
            if (frame != NULL)
            {
                if (streamController->receiveFrameCallback != NULL)
                {
                    streamController->receiveFrameCallback (frame, streamController->receiveFrameCustomData);
                }
                // NO ELSE ; no callback registered
                
                ARCONTROLLER_Frame_SetFree (frame);
            }
            else
            {
                //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, " timeout no frame error:%d .....", error);
                
                if (streamController->timeoutFrameCallback != NULL)
                {
                    streamController->timeoutFrameCallback (streamController->receiveFrameCustomData);
                }
                // NO ELSE ; no callback registered
            }
        }
    }
        
    return NULL;
}
