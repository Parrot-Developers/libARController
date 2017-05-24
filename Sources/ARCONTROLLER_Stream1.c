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
 * @file ARCONTROLLER_Stream1.c
 * @brief ARCONTROLLER_Stream1 allow to operate ARStream1 for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <json-c/json.h>
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

#include "ARCONTROLLER_AudioHeader.h"
#include "ARCONTROLLER_Stream.h"

#include <libARController/ARCONTROLLER_Stream1.h>

#include "ARCONTROLLER_Stream1.h"

/*************************
 * Private header
 *************************/

#define ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE 4

#define ARCONTROLLER_STREAM1_SAMPLERATE_8000 8000
#define ARCONTROLLER_STREAM1_SAMPLERATE_11025 11025

int ARCONTROLLER_Stream1_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id);
int ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(ARCONTROLLER_Frame_t *frame, uint8_t **spsBuffer, int *spsSize, uint8_t **ppsBuffer, int *ppsSize);

void* ARCONTROLLER_Stream1_ReaderThreadRun (void *data);

static void ARCONTROLLER_Stream1_ReadH264Frame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame);
static void ARCONTROLLER_Stream1_ReadMJPEGFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame);
static void ARCONTROLLER_Stream1_ReadPcm16leFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame);
static void ARCONTROLLER_Stream1_ReadDefaultFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame);

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Stream1_t *ARCONTROLLER_Stream1_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, eARCONTROLLER_STREAM_CODEC_TYPE codecType, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Stream1 Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Stream1_t *stream1Controller =  NULL;
    
    // Check parameters
    if (networkConfiguration == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Network Controller
        stream1Controller = malloc (sizeof (ARCONTROLLER_Stream1_t));
        if (stream1Controller != NULL)
        {
            // Initialize to default values
            stream1Controller->networkConfiguration = networkConfiguration;
            
            stream1Controller->fragmentSize = ARCONTROLLER_STREAM1_DEFAULT_VIDEO_FRAGMENT_SIZE;
            stream1Controller->maxNumberOfFragment = ARCONTROLLER_STREAM1_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER;
            stream1Controller->maxAckInterval = ARSTREAM_READER_MAX_ACK_INTERVAL_DEFAULT;
            stream1Controller->dataThread = NULL;
            stream1Controller->ackThread = NULL;
            stream1Controller->readerThread = NULL;
            stream1Controller->isRunning = 0;
            stream1Controller->framePool = NULL;
            stream1Controller->readyQueue = NULL;
            stream1Controller->receiveFrameCallback = NULL;
            stream1Controller->timeoutFrameCallback = NULL;
            //stream1Controller->codecType = codecType;
            //stream1Controller->isMP4Compliant = 0;
            stream1Controller->callbackCustomData = NULL;
            stream1Controller->decoderConfigCalled = 0;

            stream1Controller->codec.type = codecType;
            memset(&stream1Controller->codec.parameters, 0, sizeof(stream1Controller->codec.parameters));
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // create the frame pool
        stream1Controller->framePool = ARCONTROLLER_StreamPool_New (ARCONTROLLER_STREAMPOOL_DEFAULT_SIZE, &localError);
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // create the frame queue
        stream1Controller->readyQueue = ARCONTROLLER_StreamQueue_New (1, &localError);
    }

    // delete the Stream1 Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_Stream1_Delete (&stream1Controller);
    }
    // No else: skipped by an error 

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return stream1Controller;
}

void ARCONTROLLER_Stream1_Delete (ARCONTROLLER_Stream1_t **stream1Controller)
{
    // -- Delete the Stream1 Controller --

    if (stream1Controller != NULL)
    {
        if ((*stream1Controller) != NULL)
        {
            ARCONTROLLER_Stream1_Stop (*stream1Controller);

            // Delete the Frame Pool
            ARCONTROLLER_StreamPool_Delete (&((*stream1Controller)->framePool));

            // Delete the Frame Queue
            ARCONTROLLER_StreamQueue_Delete (&((*stream1Controller)->readyQueue));
            
            free (*stream1Controller);
            (*stream1Controller) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream1_Start (ARCONTROLLER_Stream1_t *stream1Controller, ARNETWORK_Manager_t *networkManager)
{
    // -- Start to read the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARSTREAM_ERROR streamError = ARSTREAM_OK;
    ARCONTROLLER_Frame_t *firstFrame = NULL;
    int d2cStreamData = -1;
    int c2dStreamAck = -1;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (!stream1Controller->isRunning))
    {
        switch (stream1Controller->codec.type)
        {
            case ARCONTROLLER_STREAM_CODEC_TYPE_H264:
            case ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG:
                    d2cStreamData = stream1Controller->networkConfiguration->deviceToControllerARStreamData;
                    c2dStreamAck = stream1Controller->networkConfiguration->controllerToDeviceARStreamAck;
                    break;
            case ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE:
                    d2cStreamData = stream1Controller->networkConfiguration->deviceToControllerARStreamAudioData;
                    c2dStreamAck = stream1Controller->networkConfiguration->controllerToDeviceARStreamAudioAck;
                break;
            default:
                break;
        }

        stream1Controller->isRunning = 1;

        firstFrame = ARCONTROLLER_StreamPool_GetNextFreeFrame (stream1Controller->framePool, &error);
        
        if (error == ARCONTROLLER_OK)
        {
            stream1Controller->streamReader = ARSTREAM_Reader_New (networkManager, d2cStreamData, c2dStreamAck, ARCONTROLLER_Stream1_FrameCompleteCallback, firstFrame->data, firstFrame->capacity, stream1Controller->fragmentSize, stream1Controller->maxAckInterval, stream1Controller, &streamError);
        
            if (streamError != ARSTREAM_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "Error while creating streamReader : %s ", ARSTREAM_Error_ToString (streamError));
                error = ARCONTROLLER_ERROR_INIT_STREAM;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create (&(stream1Controller->dataThread), ARSTREAM_Reader_RunDataThread, stream1Controller->streamReader) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "Creation of Data thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create(&(stream1Controller->ackThread), ARSTREAM_Reader_RunAckThread, stream1Controller->streamReader) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "Creation of Ack thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create(&(stream1Controller->readerThread), ARCONTROLLER_Stream1_ReaderThreadRun, stream1Controller) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "Creation of reader thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }
        
        if (error != ARCONTROLLER_OK)
        {
            ARCONTROLLER_Stream1_Stop (stream1Controller);
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream1_Stop (ARCONTROLLER_Stream1_t *stream1Controller)
{
    // -- Stop to read the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (stream1Controller->isRunning))
    {
        stream1Controller->isRunning = 0;
        
        ARSTREAM_Reader_StopReader (stream1Controller->streamReader);
        
        if (stream1Controller->dataThread != NULL)
        {
            ARSAL_Thread_Join(stream1Controller->dataThread, NULL);
            ARSAL_Thread_Destroy(&(stream1Controller->dataThread));
            stream1Controller->dataThread = NULL;
        }
        
        if (stream1Controller->ackThread != NULL)
        {
            ARSAL_Thread_Join(stream1Controller->ackThread, NULL);
            ARSAL_Thread_Destroy(&(stream1Controller->ackThread));
            stream1Controller->ackThread = NULL;
        }
        
        if (stream1Controller->readerThread != NULL)
        {
            ARSAL_Thread_Join(stream1Controller->readerThread, NULL);
            ARSAL_Thread_Destroy(&(stream1Controller->readerThread));
            stream1Controller->readerThread = NULL;
        }
        
        ARSTREAM_Reader_Delete (&(stream1Controller->streamReader));

        // Force to re-call the configure callback
        stream1Controller->decoderConfigCalled = 0;
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream1_SetCallbacks (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback, ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback, ARCONTROLLER_Stream_TimeoutFrameCallback_t timeoutFrameCallback, void *customData)
{
    // -- Set Receive Frame Callbacks --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        stream1Controller->receiveFrameCallback = receiveFrameCallback;
        stream1Controller->timeoutFrameCallback = timeoutFrameCallback;
        stream1Controller->decoderConfigCallback = decoderConfigCallback;
        stream1Controller->callbackCustomData = customData;
    }
    
    return error;
}

int ARCONTROLLER_Stream1_IsRunning (ARCONTROLLER_Stream1_t *stream1Controller, eARCONTROLLER_ERROR *error)
{
    // -- Get is running --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    int isRunning = 0;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        isRunning = stream1Controller->isRunning;
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: Error is not returned 
    
    return isRunning;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream1_SetMP4Compliant (ARCONTROLLER_Stream1_t *stream1Controller, int isMP4Compliant)
{
    // -- Set stream compliant with the mp4 format. --

    // local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;

    if ((stream1Controller == NULL) ||
        (stream1Controller->codec.type != ARCONTROLLER_STREAM_CODEC_TYPE_H264))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }

    if (error == ARCONTROLLER_OK)
    {
        stream1Controller->codec.parameters.h264parameters.isMP4Compliant = isMP4Compliant;
    }

    return error;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_GetFrame (ARCONTROLLER_Stream1_t *stream1Controller, eARCONTROLLER_ERROR *error)
{
    // -- Get Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((localError == ARCONTROLLER_OK) && (stream1Controller->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_Pop (stream1Controller->readyQueue, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_TryGetFrame (ARCONTROLLER_Stream1_t *stream1Controller, eARCONTROLLER_ERROR *error)
{
    // -- Try to Get a Frame --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((localError == ARCONTROLLER_OK) && (stream1Controller->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_TryPop (stream1Controller->readyQueue, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Stream1_GetFrameWithTimeout (ARCONTROLLER_Stream1_t *stream1Controller, uint32_t timeoutMs, eARCONTROLLER_ERROR *error)
{
    // -- Get a Frame with timeout --
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((localError == ARCONTROLLER_OK) && (stream1Controller->isRunning))
    {
        frame = ARCONTROLLER_StreamQueue_PopWithTimeout (stream1Controller->readyQueue, timeoutMs, &localError);
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return frame;
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream1_OnReceiveJson (ARCONTROLLER_Stream1_t *stream1Controller, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    eARCONTROLLER_ERROR controllerError = ARCONTROLLER_OK;

    json_object *valueJsonObj = NULL;
    json_bool res;
    
    if ((jsonObj == NULL) ||
        (stream1Controller == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }

    if (error == ARDISCOVERY_OK)
    {
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream1Controller->fragmentSize = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream1Controller->maxNumberOfFragment = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream1Controller->maxAckInterval = json_object_get_int(valueJsonObj);
        }
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // Initialization of the stream buffers
        controllerError = ARCONTROLLER_Stream1_InitStream1Buffers (stream1Controller);
        
        if (controllerError != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "Error occurred durring Initialization of the stream buffers : %s ", ARCONTROLLER_Error_ToString (controllerError));
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream1_InitStream1Buffers (ARCONTROLLER_Stream1_t *stream1Controller)
{
    // -- Initilize Stream 1 buffers --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int streamAckBufferIndex = -1;
    int streamDataBufferIndex = -1;
    int streamAckBufferId = -1;
    int streamDataBufferId = -1;
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
    switch (stream1Controller->codec.type)
    {
        case ARCONTROLLER_STREAM_CODEC_TYPE_H264:
        case ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG:
            streamAckBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->controllerToDeviceParams, stream1Controller->networkConfiguration->numberOfControllerToDeviceParam, stream1Controller->networkConfiguration->controllerToDeviceARStreamAck);
            streamDataBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->deviceToControllerParams, stream1Controller->networkConfiguration->numberOfDeviceToControllerParam, stream1Controller->networkConfiguration->deviceToControllerARStreamData);
            streamAckBufferId = stream1Controller->networkConfiguration->controllerToDeviceARStreamAck;
            streamDataBufferId = stream1Controller->networkConfiguration->deviceToControllerARStreamData;
            break;
        case ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE:
            streamAckBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->controllerToDeviceParams, stream1Controller->networkConfiguration->numberOfControllerToDeviceParam, stream1Controller->networkConfiguration->controllerToDeviceARStreamAudioAck);
            streamDataBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->deviceToControllerParams, stream1Controller->networkConfiguration->numberOfDeviceToControllerParam, stream1Controller->networkConfiguration->deviceToControllerARStreamAudioData);
            streamAckBufferId = stream1Controller->networkConfiguration->controllerToDeviceARStreamAudioAck;
            streamDataBufferId = stream1Controller->networkConfiguration->deviceToControllerARStreamAudioData;
            break;
        default:
            break;
    }

        if ((streamAckBufferIndex != -1) &&
           (streamDataBufferIndex != -1))
        {
            ARSTREAM_Reader_InitStreamAckBuffer (&(stream1Controller->networkConfiguration->controllerToDeviceParams[streamAckBufferIndex]), streamAckBufferId);
            ARSTREAM_Reader_InitStreamDataBuffer (&(stream1Controller->networkConfiguration->deviceToControllerParams[streamDataBufferIndex]), streamDataBufferId, stream1Controller->fragmentSize, stream1Controller->maxNumberOfFragment);
        }
        //NO ELSE ; device has not streaming
    }
    
    return error;
}

uint8_t* ARCONTROLLER_Stream1_FrameCompleteCallback (eARSTREAM_READER_CAUSE cause, uint8_t *frameData, uint32_t frameSize, int numberOfSkippedFrames, int isFlushFrame, uint32_t *newBufferCapacity, void *custom)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM1_TAG, "ARCONTROLLER_Stream1_FrameCompleteCallback ....");
    
    // -- Reader Callback for ARStream1 --
    static ARCONTROLLER_Frame_t *oldFrame;
    ARCONTROLLER_Stream1_t *stream1Controller = (ARCONTROLLER_Stream1_t *)custom;
    ARCONTROLLER_StreamPool_t *pool = NULL;
    ARCONTROLLER_StreamQueue_t *queue = NULL;
    ARCONTROLLER_Frame_t *frame = NULL;
    uint8_t *resFrameData = NULL;
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if ((stream1Controller == NULL) ||
        (stream1Controller->framePool == NULL) ||
        (stream1Controller->readyQueue == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        pool = stream1Controller->framePool;
        queue = stream1Controller->readyQueue;
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
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "cause %d not known", cause);
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
        
        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM1_TAG, "cause : %d | error: %s", cause, ARCONTROLLER_Error_ToString (error));
    }

    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM1_TAG, "ARCONTROLLER_Stream1_FrameCompleteCallback end resFrameData : %d....", resFrameData);

    return resFrameData;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

int ARCONTROLLER_Stream1_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id)
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

void* ARCONTROLLER_Stream1_ReaderThreadRun (void *data)
{
    // -- Manage the reception of the Video --

    // local declarations
    ARCONTROLLER_Stream1_t *stream1Controller = data;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;

    // Check parameters
    if (stream1Controller != NULL)
    {
        while (stream1Controller->isRunning)
        {
            frame = ARCONTROLLER_Stream1_GetFrameWithTimeout (stream1Controller, 500, &error);
            
            if (frame != NULL)
            {
                switch (stream1Controller->codec.type)
                {
                    case ARCONTROLLER_STREAM_CODEC_TYPE_H264:
                        ARCONTROLLER_Stream1_ReadH264Frame (stream1Controller, frame);
                        break;

                    case ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG:
                        ARCONTROLLER_Stream1_ReadMJPEGFrame (stream1Controller, frame);
                        break;

                    case ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE:
                        ARCONTROLLER_Stream1_ReadPcm16leFrame (stream1Controller, frame);
                        break;

                    default:
                        ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "codec %d not known", stream1Controller->codec.type);
                        ARCONTROLLER_Stream1_ReadDefaultFrame (stream1Controller, frame);
                        break;
                }

                ARCONTROLLER_Frame_SetFree (frame);
            }
            else
            {
                //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM1_TAG, " timeout no frame error:%d .....", error);
                
                if (stream1Controller->timeoutFrameCallback != NULL)
                {
                    stream1Controller->timeoutFrameCallback (stream1Controller->callbackCustomData);
                }
                // NO ELSE ; no callback registered
            }
        }
    }

    return NULL;
}

static void ARCONTROLLER_Stream1_ReadH264Frame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARCONTROLLER_ERROR callbackError = ARCONTROLLER_OK;

    uint8_t *spsBuffer = NULL;
    int spsSize = 0;
    uint8_t *ppsBuffer = NULL;
    int ppsSize = 0;

    if (frame->isIFrame)
    {
        error = ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(frame, &spsBuffer, &spsSize, &ppsBuffer, &ppsSize);

        if (error == ARCONTROLLER_OK)
        {
            //Remove sps/pps of the frame data
            frame->data = frame->data + spsSize + ppsSize;
            frame->used = frame->used - spsSize - ppsSize;

            //Set Codec
            stream1Controller->codec.parameters.h264parameters.spsBuffer = spsBuffer;
            stream1Controller->codec.parameters.h264parameters.spsSize = spsSize;
            stream1Controller->codec.parameters.h264parameters.ppsBuffer = ppsBuffer;
            stream1Controller->codec.parameters.h264parameters.ppsSize = ppsSize;

            //Configuration decoder callback
            if ((!stream1Controller->decoderConfigCalled) && (stream1Controller->decoderConfigCallback != NULL))
            {
                stream1Controller->decoderConfigCallback (stream1Controller->codec, stream1Controller->callbackCustomData);
                stream1Controller->decoderConfigCalled = 1;
            }
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "sps pps not found.");
        }
    }
    // NO ELSE ; no callback registered

    if ((error == ARCONTROLLER_OK) && (stream1Controller->decoderConfigCalled == 1))
    {
        //reformat H264 for mp4 format
        if (stream1Controller->codec.parameters.h264parameters.isMP4Compliant)
        {
            // replace nalu header by nalu size
            uint32_t naluSize = htonl (frame->used - ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE);
            memcpy (frame->data, &naluSize, sizeof (uint32_t));
        }

        if (stream1Controller->receiveFrameCallback != NULL)
        {
            callbackError = stream1Controller->receiveFrameCallback (frame, stream1Controller->callbackCustomData);
            if (callbackError != ARCONTROLLER_OK)
            {
                //Recall decoderConfigCallback
                stream1Controller->decoderConfigCalled = 0;
            }
        }
        // NO ELSE ; no callback registered
    }
    // NO ELSE ; drop the frame
}

static void ARCONTROLLER_Stream1_ReadMJPEGFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame)
{
    ARCONTROLLER_Stream_Codec_t codec;
    eARCONTROLLER_ERROR callbackError = ARCONTROLLER_OK;

    //Set Codec
    codec.type = ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG;

    //Callback
    if ((!stream1Controller->decoderConfigCalled) && (stream1Controller->decoderConfigCallback != NULL))
    {
        stream1Controller->decoderConfigCallback (codec, stream1Controller->callbackCustomData);
        stream1Controller->decoderConfigCalled = 1;
    }

    if (stream1Controller->receiveFrameCallback != NULL)
    {
        callbackError = stream1Controller->receiveFrameCallback (frame, stream1Controller->callbackCustomData);
        if (callbackError != ARCONTROLLER_OK)
        {
            //Recall decoderConfigCallback
            stream1Controller->decoderConfigCalled = 0;
        }
    }
    // NO ELSE ; no callback registered
}

static int getSampleRate(ARCONTROLLER_Frame_t *frame, eARCONTROLLER_ERROR *error)
{
    int sampleFormat = 0;
    int sampleShift = 0;
    int sampleRate = 0;
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;

    // Check parameters
    if ((frame == NULL) ||
        (frame->data == NULL) ||
        (frame->used <= 10))
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing

    if (localError == ARCONTROLLER_OK)
    {
        sampleFormat = (frame->data[9] << 8) + frame->data[10];

        if (((sampleFormat >> ARCONTROLLER_AUDIO_HEADER_FMT_BASE_RATE_SHIFT) & 0x1) == 1)
        {
            sampleRate = ARCONTROLLER_STREAM1_SAMPLERATE_11025;
        }
        else
        {
            sampleRate = ARCONTROLLER_STREAM1_SAMPLERATE_8000;
        }
        sampleShift = (sampleFormat >> ARCONTROLLER_AUDIO_HEADER_FMT_RATE_SHIFT_SHIFT) & 0x3;
        sampleRate <<= sampleShift;
    }

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned

    return sampleRate;
}

static void ARCONTROLLER_Stream1_ReadPcm16leFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame)
{

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARCONTROLLER_ERROR callbackError = ARCONTROLLER_OK;
    int sampleRate = 0;

    sampleRate = getSampleRate(frame, &error);

    if (error == ARCONTROLLER_OK)
    {
        //Remove header of the frame data
        frame->data = frame->data + ARCONTROLLER_AUDIO_HEADER_SIZE;
        frame->used = frame->used - ARCONTROLLER_AUDIO_HEADER_SIZE;

        //Callback if not called or sample rate has changed
        if (((!stream1Controller->decoderConfigCalled) ||
            (sampleRate != stream1Controller->codec.parameters.pcm16leParameters.sampleRate)) &&
            (stream1Controller->decoderConfigCallback != NULL))
        {
            //Set Codec
            stream1Controller->codec.type = ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE;
            stream1Controller->codec.parameters.pcm16leParameters.sampleRate = sampleRate;
            stream1Controller->codec.parameters.pcm16leParameters.channel = ARCONTROLLER_STREAM_AUDIO_CHANNEL_MONO;

            stream1Controller->decoderConfigCallback (stream1Controller->codec, stream1Controller->callbackCustomData);
            stream1Controller->decoderConfigCalled = 1;
        }

        if ((stream1Controller->receiveFrameCallback != NULL) &&
            stream1Controller->decoderConfigCalled)
        {
            callbackError = stream1Controller->receiveFrameCallback (frame, stream1Controller->callbackCustomData);
            if (callbackError != ARCONTROLLER_OK)
            {
                //Recall decoderConfigCallback
                stream1Controller->decoderConfigCalled = 0;
            }
        }
        // NO ELSE ; no callback registered
    }
}

static void ARCONTROLLER_Stream1_ReadDefaultFrame (ARCONTROLLER_Stream1_t *stream1Controller, ARCONTROLLER_Frame_t *frame)
{
    eARCONTROLLER_ERROR callbackError = ARCONTROLLER_OK;

    if (stream1Controller->receiveFrameCallback != NULL)
    {
        callbackError = stream1Controller->receiveFrameCallback (frame, stream1Controller->callbackCustomData);
        if (callbackError != ARCONTROLLER_OK)
        {
            //Recall decoderConfigCallback
            stream1Controller->decoderConfigCalled = 0;
        }
    }
    // NO ELSE ; no callback registered
}

int ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(ARCONTROLLER_Frame_t *frame, uint8_t **spsBuffer, int *spsSize, uint8_t **ppsBuffer, int *ppsSize)
{
    // -- Get sps and pps from Iframe --

    size_t searchIndex = 0;
    int found = 0;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;

    if (frame->used < ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE ||
        0 != frame->data[0] ||
        0 != frame->data[1] ||
        0 != frame->data[2] ||
        1 != frame->data[3])
    {
        ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "bad frame.");
        error = ARCONTROLLER_ERROR;
    }

    if (error == ARCONTROLLER_OK)
    {
        // we'll need to search the "00 00 00 01" pattern to find each header size
        // Search start at index 4 to avoid finding the SPS "00 00 00 01" tag
        for (searchIndex = ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex <= frame->used - ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex++)
        {
            if (0 == frame->data[searchIndex] &&
                0 == frame->data[searchIndex+1] &&
                0 == frame->data[searchIndex+2] &&
                1 == frame->data[searchIndex+3])
            {
                *spsBuffer = frame->data;
                *spsSize = searchIndex;
                found = 1;
                break;  // PPS header found
            }
        }

        if (found == 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "sps not Found.");
            error = ARCONTROLLER_ERROR;
        }
    }

    if (error == ARCONTROLLER_OK)
    {
        found = 0;

        // Search start at index 4 to avoid finding the PSS "00 00 00 01" tag
        for (searchIndex = (*spsSize) + ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex <= frame->used - ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex ++)
        {
            if (0 == frame->data[searchIndex  ] &&
                0 == frame->data[searchIndex+1] &&
                0 == frame->data[searchIndex+2] &&
                1 == frame->data[searchIndex+3])
            {
                *ppsBuffer = (*spsBuffer) + (*spsSize);
                *ppsSize = searchIndex - (*spsSize);
                found = 1;
                break;  // frame header found
            }
        }

        if (found == 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "pps not Found.");
            error = ARCONTROLLER_ERROR;
        }
    }

    return error;
}
