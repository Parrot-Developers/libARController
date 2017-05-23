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
 * @file ARCONTROLLER_Stream.c
 * @brief ARCONTROLLER_Stream allow to operate ARStream for receive a stream.
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


#include <libARController/ARCONTROLLER_Stream1.h>
#include <libARController/ARCONTROLLER_Stream2.h>

#include <libARController/ARCONTROLLER_Stream.h>

#include "ARCONTROLLER_Stream.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_video_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error)
{
    // -- Create a new video Stream Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Stream_t *streamController =  NULL;
    eARCONTROLLER_STREAM_CODEC_TYPE codecType = ARCONTROLLER_STREAM_CODEC_TYPE_DEFAULT;

    // Check parameters
    if (discoveryDevice == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }

    if (localError == ARCONTROLLER_OK)
    {
	// Get video codec
	switch (discoveryDevice->productID)
	{
	    case ARDISCOVERY_PRODUCT_JS:
	    case ARDISCOVERY_PRODUCT_JS_EVO_LIGHT:
	    case ARDISCOVERY_PRODUCT_JS_EVO_RACE:
	    case ARDISCOVERY_PRODUCT_POWER_UP:
		codecType =  ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG;
		break;

	    default:
		codecType = ARCONTROLLER_STREAM_CODEC_TYPE_DEFAULT;
		break;
	}

        streamController = ARCONTROLLER_Stream_New (networkConfiguration, discoveryDevice, codecType, &localError);
    }

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned

    return streamController;
}

ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_audio_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error)
{
    // -- Create a new audio Stream Controller --

    return ARCONTROLLER_Stream_New (networkConfiguration, discoveryDevice, ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE, error);

}

ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_STREAM_CODEC_TYPE codecType, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Stream Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Stream_t *streamController =  NULL;
    
    // Check parameters
    if ((networkConfiguration == NULL) || (discoveryDevice == NULL))
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
            streamController->isRunning = 0;
            
            //stream 1
            streamController->stream1Controller = ARCONTROLLER_Stream1_New (networkConfiguration, codecType, &localError);
            
            //stream 2
            streamController->stream2Controller = ARCONTROLLER_Stream2_New (discoveryDevice, &localError);
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }

    // delete the Stream Controller if an error occurred
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

            //Delete stream 1
            ARCONTROLLER_Stream1_Delete (&((*streamController)->stream1Controller));
            
            //Delete stream 2
            ARCONTROLLER_Stream2_Delete (&((*streamController)->stream2Controller));
            
            free (*streamController);
            (*streamController) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_SetMP4Compliant (ARCONTROLLER_Stream_t *streamController, int isMP4Compliant)
{
    // -- Set stream compliant with the mp4 format. --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARCONTROLLER_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ARCONTROLLER_Stream1_SetMP4Compliant (streamController->stream1Controller, isMP4Compliant);
        ARCONTROLLER_Stream2_SetMP4Compliant (streamController->stream2Controller, isMP4Compliant);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_Start (ARCONTROLLER_Stream_t *streamController, ARNETWORK_Manager_t *networkManager)
{
    // -- Start to read the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing

    if ((error == ARCONTROLLER_OK) && (!streamController->isRunning))
    {
        streamController->isRunning = 1;
        
        if (ARCONTROLLER_Stream2_IsInitilized(streamController->stream2Controller))
        {
            error = ARCONTROLLER_Stream2_Start (streamController->stream2Controller);
        }
        else
        {
            error = ARCONTROLLER_Stream1_Start (streamController->stream1Controller, networkManager);
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
        
        if (ARCONTROLLER_Stream1_IsRunning(streamController->stream1Controller, NULL))
        {
            error = ARCONTROLLER_Stream1_Stop (streamController->stream1Controller);
        }
        
        if (ARCONTROLLER_Stream2_IsRunning(streamController->stream2Controller, NULL))
        {
            error = ARCONTROLLER_Stream2_Stop (streamController->stream2Controller);
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_SetReceiveFrameCallback (ARCONTROLLER_Stream_t *streamController, ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback, ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback, ARCONTROLLER_Stream_TimeoutFrameCallback_t timeoutFrameCallback, void *customData)
{
    // -- Set Receive Frame Callbacks --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (streamController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Stream1_SetCallbacks(streamController->stream1Controller, decoderConfigCallback, receiveFrameCallback, timeoutFrameCallback, customData);
    }
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Stream2_SetCallbacks(streamController->stream2Controller, decoderConfigCallback,  receiveFrameCallback, customData);
    }
    
    return error;
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
    
    if (error == ARDISCOVERY_OK)
    {
        //stream 2
        error = ARCONTROLLER_Stream2_OnSendJson (streamController->stream2Controller, jsonObj);
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream_OnReceiveJson (ARCONTROLLER_Stream_t *streamController, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    if ((jsonObj == NULL) ||
        (streamController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        error = ARCONTROLLER_Stream1_OnReceiveJson (streamController->stream1Controller, jsonObj);
        
        error = ARCONTROLLER_Stream2_OnReceiveJson (streamController->stream2Controller, jsonObj);
    }
    
    return error;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

