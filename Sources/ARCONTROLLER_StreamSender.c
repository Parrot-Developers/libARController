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
 * @file ARCONTROLLER_StreamSender.c
 * @brief ARCONTROLLER_StreamSender allow to operate ARStream for send a stream.
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Socket.h>
#include <libARStream/ARStream.h>
#include <libARNetwork/ARNetwork.h>
#include <libARDiscovery/ARDiscovery.h>
#include <libARController/ARCONTROLLER_Network.h>
#include <libARController/ARCONTROLLER_Frame.h>
#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_StreamQueue.h>
#include <libARController/ARCONTROLLER_StreamSender.h>
#include "ARCONTROLLER_AudioHeader.h"
#include "ARCONTROLLER_StreamSender.h"

/*************************
 * Private header
 *************************/

#define ARCONTROLLER_STREAM_SENDER_QUEUE_SIZE 24
#define ARCONTROLLER_STREAM_SENDER_NB_FRAMES_IN_POOL 40

static void ARCONTROLLER_StreamSender_FrameUpdateCallback (eARSTREAM_SENDER_STATUS status, uint8_t *framePointer, uint32_t frameSize, void *custom);
static int ARCONTROLLER_StreamSender_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id);

static void setAudioHeader(ARCONTROLLER_Frame_t *frame, ARCONTROLLER_AudioHeader_t *header);

/*************************
 * Implementation
 *************************/

ARCONTROLLER_StreamSender_t *ARCONTROLLER_StreamSender_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Stream sender Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_StreamSender_t *streamController =  NULL;
    
    // Check parameters
    if (networkConfiguration == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Network Controller
        streamController = malloc (sizeof (ARCONTROLLER_StreamSender_t));
        if (streamController != NULL)
        {
            // Initialize to default values
            streamController->networkConfiguration = networkConfiguration;
            streamController->streamSender = NULL;
            streamController->isRunning = 0;
            streamController->fragmentSize = 1000;
            streamController->maxFragmentSize = 65500;
            streamController->maxNumberOfFragment = 1;
            streamController->maxAckInterval = 0;
            streamController->framePool = NULL;
            /* audio stream part*/
            memset(&streamController->audioHeader, 0, ARCONTROLLER_AUDIO_HEADER_SIZE);
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }

    if (localError == ARCONTROLLER_OK)
    {
        // create the frame pool
        streamController->framePool = ARCONTROLLER_StreamPool_New (ARCONTROLLER_STREAM_SENDER_NB_FRAMES_IN_POOL, &localError);
    }

    // delete the Stream Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_StreamSender_Delete (&streamController);
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

void ARCONTROLLER_StreamSender_Delete (ARCONTROLLER_StreamSender_t **streamController)
{
    // -- Delete the Stream sender Controller --

    if (streamController != NULL)
    {
        if ((*streamController) != NULL)
        {
            ARCONTROLLER_StreamSender_Stop (*streamController);

            // Delete the Frame Pool
            ARCONTROLLER_StreamPool_Delete (&((*streamController)->framePool));

            free (*streamController);
            (*streamController) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_Start (ARCONTROLLER_StreamSender_t *streamController, ARNETWORK_Manager_t *networkManager)
{
    // -- Start to send the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARSTREAM_ERROR streamError = ARSTREAM_OK;
    int  c2dStreamData = -1;
    int d2CStreamAck = -1;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing

    if ((error == ARCONTROLLER_OK) && (!streamController->isRunning))
    {
        streamController->isRunning = 1;
        
        c2dStreamData = streamController->networkConfiguration->controllerToDeviceARStreamAudioData;
        d2CStreamAck = streamController->networkConfiguration->deviceToControllerARStreamAudioAck;

        streamController->streamSender = ARSTREAM_Sender_New (networkManager, c2dStreamData, d2CStreamAck, ARCONTROLLER_StreamSender_FrameUpdateCallback, ARCONTROLLER_STREAM_SENDER_QUEUE_SIZE, streamController->maxFragmentSize, streamController->maxNumberOfFragment, streamController, &streamError);
        if (streamError != ARSTREAM_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_SENDER_TAG, "ARSTREAM_Sender_New error = %d", streamError);
            error = ARCONTROLLER_ERROR;
        }

        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create (&(streamController->dataThread), ARSTREAM_Sender_RunDataThread, streamController->streamSender) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_SENDER_TAG, "Creation of Data thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }

        if (error == ARCONTROLLER_OK)
        {
            if (ARSAL_Thread_Create(&(streamController->ackThread), ARSTREAM_Sender_RunAckThread, streamController->streamSender) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_SENDER_TAG, "Creation of Ack thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
            }
        }

        /* Initialize header template*/
        if (error == ARCONTROLLER_OK)
        {
            streamController->audioHeader.timestamp = 0;
            streamController->audioHeader.sampleFormat = ARCONTROLLER_AUDIO_CODEC_PCM16LE << ARCONTROLLER_AUDIO_HEADER_FMT_CODEC_SHIFT;
            streamController->audioHeader.sampleFormat |= ARCONTROLLER_AUDIO_HEADER_FMT_MONO << ARCONTROLLER_AUDIO_HEADER_FMT_CHANNELS_SHIFT;
            streamController->audioHeader.sampleFormat |= ARCONTROLLER_AUDIO_HEADER_FMT_8000_HZ << ARCONTROLLER_AUDIO_HEADER_FMT_BASE_RATE_SHIFT;
            streamController->audioHeader.sampleFormat |= ARCONTROLLER_AUDIO_HEADER_FMT_RATE_X1 << ARCONTROLLER_AUDIO_HEADER_FMT_RATE_SHIFT_SHIFT;
            streamController->audioHeader.codecData1 = 0;
            streamController->audioHeader.codecData2 = 0;
        }

        if (error != ARCONTROLLER_OK)
        {
            ARCONTROLLER_StreamSender_Stop (streamController);
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_Stop (ARCONTROLLER_StreamSender_t *streamController)
{
    // -- Stop to send the stream --

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
        ARSTREAM_Sender_StopSender(streamController->streamSender);

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
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_SendAudioFrame(ARCONTROLLER_StreamSender_t *streamController, uint8_t *data, int dataSize)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *localFrame = NULL;
    eARSTREAM_ERROR streamError = ARSTREAM_OK;

    // Check parameters
    if ((streamController == NULL) || (streamController->framePool == NULL) ||
            (data == NULL) || (dataSize > ARCONTROLLER_AUDIO_DATA_SIZE))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }

    if (error == ARCONTROLLER_OK)
    {
        streamController->audioHeader.timestamp += ARCONTROLLER_AUDIO_DATA_SIZE;

        localFrame = ARCONTROLLER_StreamPool_GetNextFreeFrame (streamController->framePool, &error);

        if(localFrame == NULL)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_SENDER_TAG, "sendNewFrame no more buffer");
        }
    }

    if (error == ARCONTROLLER_OK)
    {
        ARCONTROLLER_Frame_ensureCapacityIsAtLeast (localFrame, ARCONTROLLER_AUDIO_FRAME_SIZE, &error);
    }

    if (error == ARCONTROLLER_OK)
    {
	setAudioHeader(localFrame, &streamController->audioHeader);

        memcpy(localFrame->data+ARCONTROLLER_AUDIO_HEADER_SIZE, data, dataSize);
        localFrame->used = ARCONTROLLER_AUDIO_HEADER_SIZE + dataSize;

        streamError = ARSTREAM_Sender_SendNewFrame (streamController->streamSender, localFrame->data, localFrame->used, 0, NULL);
        if (streamError != ARSTREAM_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_SENDER_TAG, "sendNewFrame error : %d", streamError);
            ARCONTROLLER_Frame_SetFree (localFrame);
            error = ARCONTROLLER_ERROR;
        }
    }

    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_StreamSender_OnReceiveJson (ARCONTROLLER_StreamSender_t *streamController, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --

    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    int streamAckBufferIndex = -1;
    int streamDataBufferIndex = -1;
    int streamAckBufferId = -1;
    int streamDataBufferId = -1;

    json_object *valueJsonObj = NULL;
    json_bool res;

    if ((jsonObj == NULL) ||
        (streamController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }

    if (error == ARDISCOVERY_OK)
    {

        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_SIZE_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            streamController->fragmentSize = json_object_get_int(valueJsonObj);
        }

        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            streamController->maxNumberOfFragment = json_object_get_int(valueJsonObj);
        }

        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            streamController->maxAckInterval = json_object_get_int(valueJsonObj);
        }
    }

    if (error == ARDISCOVERY_OK)
    {
        streamAckBufferIndex = ARCONTROLLER_StreamSender_IdToIndex (streamController->networkConfiguration->deviceToControllerParams, streamController->networkConfiguration->numberOfDeviceToControllerParam, streamController->networkConfiguration->deviceToControllerARStreamAudioAck);
        streamDataBufferIndex = ARCONTROLLER_StreamSender_IdToIndex (streamController->networkConfiguration->controllerToDeviceParams, streamController->networkConfiguration->numberOfControllerToDeviceParam, streamController->networkConfiguration->controllerToDeviceARStreamAudioData);
        streamAckBufferId = streamController->networkConfiguration->deviceToControllerARStreamAudioAck;
        streamDataBufferId = streamController->networkConfiguration->controllerToDeviceARStreamAudioData;

        // Initialization of the stream buffers
        ARSTREAM_Sender_InitStreamAckBuffer (&(streamController->networkConfiguration->deviceToControllerParams[streamAckBufferIndex]), streamAckBufferId);
        ARSTREAM_Sender_InitStreamDataBuffer (&(streamController->networkConfiguration->controllerToDeviceParams[streamDataBufferIndex]), streamDataBufferId, streamController->fragmentSize, streamController->maxNumberOfFragment);
    }

    return error;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/


/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

void ARCONTROLLER_StreamSender_FrameUpdateCallback (eARSTREAM_SENDER_STATUS status, uint8_t *framePointer, uint32_t frameSize, void *custom)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    ARCONTROLLER_StreamSender_t *streamController = custom;

    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }

    if (error == ARCONTROLLER_OK)
    {
        frame = ARCONTROLLER_StreamPool_GetFrameFromData (streamController->framePool, framePointer, &error);
    }

    if (error == ARCONTROLLER_OK)
    {
        switch (status)
        {
            case ARSTREAM_SENDER_STATUS_FRAME_SENT:
            case ARSTREAM_SENDER_STATUS_FRAME_CANCEL:
            error = ARCONTROLLER_Frame_SetFree (frame);
            break;
            default:
            //Do nothing
            break;
        }
    }
}

int ARCONTROLLER_StreamSender_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id)
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

void setAudioHeader(ARCONTROLLER_Frame_t *frame, ARCONTROLLER_AudioHeader_t *header)
{
    memcpy(&frame->data[0], header, ARCONTROLLER_AUDIO_HEADER_SIZE);
}
