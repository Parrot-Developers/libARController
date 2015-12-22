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

#include "ARCONTROLLER_Stream.h"

#include <libARController/ARCONTROLLER_Stream1.h>

#include "ARCONTROLLER_Stream1.h"

/*************************
 * Private header
 *************************/

#define ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE 4

int ARCONTROLLER_Stream1_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id);
void ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(ARCONTROLLER_Frame_t *frame, uint8_t **spsBuffer, int *spsSize, uint8_t **ppsBuffer, int *ppsSize);

void* ARCONTROLLER_Stream1_ReaderThreadRun (void *data);
int ARCONTROLLER_Stream1_useStream1V2 (ARCONTROLLER_Stream1_t *stream1Controller);

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
            stream1Controller->maxNumberOfFragement = ARCONTROLLER_STREAM1_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER;
            stream1Controller->maxAckInterval = ARSTREAM_READER_MAX_ACK_INTERVAL_DEFAULT;
            stream1Controller->dataThread = NULL;
            stream1Controller->ackThread = NULL;
            stream1Controller->readerThread = NULL;
            stream1Controller->isRunning = 0;
            stream1Controller->framePool = NULL;
            stream1Controller->readyQueue = NULL;
            stream1Controller->receiveFrameCallback = NULL;
            stream1Controller->timeoutFrameCallback = NULL;
            stream1Controller->codecType = codecType;
            stream1Controller->isMP4Compliant = 0;
            stream1Controller->callbackCustomData = NULL;
            stream1Controller->decoderConfigCalled = 0;
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
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (!stream1Controller->isRunning))
    {
        stream1Controller->isRunning = 1;

        firstFrame = ARCONTROLLER_StreamPool_GetNextFreeFrame (stream1Controller->framePool, &error);
        
        if (error == ARCONTROLLER_OK)
        {
            stream1Controller->streamReader = ARSTREAM_Reader_New (networkManager, stream1Controller->networkConfiguration->deviceToControllerARStreamData, stream1Controller->networkConfiguration->controllerToDeviceARStreamAck, ARCONTROLLER_Stream1_FrameCompleteCallback, firstFrame->data, firstFrame->capacity, stream1Controller->fragmentSize, stream1Controller->maxAckInterval, stream1Controller, &streamError);
        
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
    
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARCONTROLLER_OK)
    {
        stream1Controller->isMP4Compliant = isMP4Compliant;
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
    
    if ((error == ARCONTROLLER_OK) && (stream1Controller->isRunning))
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
    
    if ((error == ARCONTROLLER_OK) && (stream1Controller->isRunning))
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
    
    if ((jsonObj == NULL) ||
        (stream1Controller == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }

    if (error == ARDISCOVERY_OK)
    {
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY);
        if (valueJsonObj != NULL)
        {
            stream1Controller->fragmentSize = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY);
        if (valueJsonObj != NULL)
        {
            stream1Controller->maxNumberOfFragement = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY);
        if (valueJsonObj != NULL)
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
    
    // Check parameters
    if (stream1Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        streamAckBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->controllerToDeviceParams, stream1Controller->networkConfiguration->numberOfControllerToDeviceParam, stream1Controller->networkConfiguration->controllerToDeviceARStreamAck);
        streamDataBufferIndex = ARCONTROLLER_Stream1_IdToIndex (stream1Controller->networkConfiguration->deviceToControllerParams, stream1Controller->networkConfiguration->numberOfDeviceToControllerParam, stream1Controller->networkConfiguration->deviceToControllerARStreamData);
                
        if ((streamAckBufferIndex != -1) &&
           (streamDataBufferIndex != -1))
        {
            ARSTREAM_Reader_InitStreamAckBuffer (&(stream1Controller->networkConfiguration->controllerToDeviceParams[streamAckBufferIndex]), stream1Controller->networkConfiguration->controllerToDeviceARStreamAck);
            ARSTREAM_Reader_InitStreamDataBuffer (&(stream1Controller->networkConfiguration->deviceToControllerParams[streamDataBufferIndex]), stream1Controller->networkConfiguration->deviceToControllerARStreamData, stream1Controller->fragmentSize, stream1Controller->maxNumberOfFragement);
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
    ARCONTROLLER_Stream_Codec_t codec;
    eARCONTROLLER_ERROR callbackError = ARCONTROLLER_OK;
    
    uint8_t *spsBuffer;
    int spsSize;
    uint8_t *ppsBuffer;
    int ppsSize;
    
    // Check parameters
    if (stream1Controller != NULL)
    {
        while (stream1Controller->isRunning)
        {
            frame = ARCONTROLLER_Stream1_GetFrameWithTimeout (stream1Controller, 500, &error);
            
            if (frame != NULL)
            {
                
                switch (stream1Controller->codecType)
                {
                    case ARCONTROLLER_STREAM_CODEC_TYPE_H264:
                        
                        if (frame->isIFrame)
                        {
                            ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(frame, &spsBuffer, &spsSize, &ppsBuffer, &ppsSize);
                            
                            //Remove sps/pps of the frame data
                            frame->data = frame->data + spsSize + ppsSize;
                            frame->used = frame->used - spsSize - ppsSize;
                            
                            //Set Codec
                            codec.type = ARCONTROLLER_STREAM_CODEC_TYPE_H264;
                            codec.parameters.h264parameters.spsBuffer = spsBuffer;
                            codec.parameters.h264parameters.spsSize = spsSize;
                            codec.parameters.h264parameters.ppsBuffer = ppsBuffer;
                            codec.parameters.h264parameters.ppsSize = ppsSize;
                            codec.parameters.h264parameters.isMP4Compliant = stream1Controller->isMP4Compliant;
                            
                            //Configuration decoder callback 
                            if ((!stream1Controller->decoderConfigCalled) && (stream1Controller->decoderConfigCallback != NULL))
                            {
                                stream1Controller->decoderConfigCallback (codec, stream1Controller->callbackCustomData);
                                stream1Controller->decoderConfigCalled = 1;
                            }
                        }
                        // NO ELSE ; no callback registered
                        
                        //reformat H264 for mp4 format
                        if (stream1Controller->isMP4Compliant)
                        {
                            // replace nalu header by nalu size
                            uint32_t naluSize = htonl (frame->used - ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE);
                            memcpy (frame->data, &naluSize, sizeof (uint32_t));
                        }
                        
                        break;
                        
                    case ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG:
                        
                        //Set Codec
                        codec.type = ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG;
                        
                        //Callback 
                        if ((!stream1Controller->decoderConfigCalled) && (stream1Controller->decoderConfigCallback != NULL))
                        {
                            stream1Controller->decoderConfigCallback (codec, stream1Controller->callbackCustomData);
                            stream1Controller->decoderConfigCalled = 1;
                        }
                        break;
                    
                    default:
                        ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM1_TAG, "codec %d not known", stream1Controller->codecType);
                        break;
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

void ARCONTROLLER_Stream1_GetSpsPpsFromIFrame(ARCONTROLLER_Frame_t *frame, uint8_t **spsBuffer, int *spsSize, uint8_t **ppsBuffer, int *ppsSize)
{
    // -- Get sps and pps from Iframe --
    
    int searchIndex = 0;

    // we'll need to search the "00 00 00 01" pattern to find each header size
    // Search start at index 4 to avoid finding the SPS "00 00 00 01" tag
    for (searchIndex = ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex <= frame->used - ARCONTROLLER_STREAM1_H264_NAL_HEADER_SIZE; searchIndex ++)
    {
        if (0 == frame->data[searchIndex] &&
                0 == frame->data[searchIndex+1] &&
                0 == frame->data[searchIndex+2] &&
                1 == frame->data[searchIndex+3])
        {
            *spsBuffer = frame->data;
            *spsSize = searchIndex;
            break;  // PPS header found
        }
    }

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
            break;  // frame header found
        }
    }
}
