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
#include <libARDiscovery/ARDISCOVERY_Error.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <libARController/ARCONTROLLER_Stream.h>

#include <libARController/ARCONTROLLER_Stream.h>

#include "ARCONTROLLER_Stream.h"

/*************************
 * Private header
 *************************/

//TODO add !!!!!!!!!!!!!
int ARCONTROLLER_Stream_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id);

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_New (ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_New ....");
        
    // -- Create a new Stream Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    eARDISCOVERY_ERROR dicoveryError = ARDISCOVERY_OK;
    ARCONTROLLER_Stream_t *streamController =  NULL;
    
    // check parameters
    if (discoveryDevice == NULL)
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
            streamController->discoveryDevice = NULL;
            streamController->fragmentSize = ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_SIZE;
            streamController->maxNumberOfFragement = ARCONTROLLER_STREAM_DEFAULT_VIDEO_FRAGMENT_MAXIMUM_NUMBER;
            streamController->maxAckInterval = ARSTREAM_READER_MAX_ACK_INTERVAL_DEFAULT;
            streamController->dataThread = NULL;
            streamController->ackThread = NULL;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "Copy the device ....");
        
        // Copy the device
        streamController->discoveryDevice = ARDISCOVERY_Device_NewByCopy (discoveryDevice, &dicoveryError);
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_DEVICE_COPY;
        }
    }
    
    // Get networkConfiguration of the device
    if (localError == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, " Get networkConfiguration of the device ...");
        
        dicoveryError = ARDISCOVERY_Device_InitNetworkCongifuration (streamController->discoveryDevice, &(streamController->networkConfiguration));
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_DEVICE_GET_NETWORK_CONFIG;
        }
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "aaaaa streamController->networkConfiguration.controllerToDeviceARStreamAck %d ....", streamController->networkConfiguration.controllerToDeviceARStreamAck);
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "aaaaa streamController->networkConfiguration.deviceToControllerARStreamData %d ....", streamController->networkConfiguration.deviceToControllerARStreamData);
        
    }
    // No else: skipped by an error
    
    // Check if it is a wifi device
    if ((localError == ARCONTROLLER_OK) && 
        (ARDISCOVERY_getProductService (streamController->discoveryDevice->productID) == ARDISCOVERY_PRODUCT_NSNETSERVICE))
    {
        // Add callbacks for the connection json part
        dicoveryError = ARDISCOVERY_Device_WifiAddConnectionCallbacks (streamController->discoveryDevice, ARCONTROLLER_Stream_SendJsonCallback, ARCONTROLLER_Stream_ReceiveJsonCallback, streamController);
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_DEVICE_JSON_CALLBACK;
        }
    }

    // delete the Network Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_Stream_Delete (&streamController);
    }
    // No else: skipped by an error 


    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "localError %d ....", localError);
        

    // return the error
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
    
    // local declarations
    int bufferIndex = 0;

    if (streamController != NULL)
    {
        if ((*streamController) != NULL)
        {
            
            if ((*streamController)->dataThread != NULL)
            {
                ARSAL_Thread_Join((*streamController)->dataThread, NULL);
                ARSAL_Thread_Destroy(&((*streamController)->dataThread));
                (*streamController)->dataThread = NULL;
            }
            
            if ((*streamController)->ackThread != NULL)
            {
                ARSAL_Thread_Join((*streamController)->ackThread, NULL);
                ARSAL_Thread_Destroy(&((*streamController)->ackThread));
                (*streamController)->ackThread = NULL;
            }
            
            ARDISCOVERY_Device_Delete (&((*streamController)->discoveryDevice));
            
            free (*streamController);
            (*streamController) = NULL;
        }
    }
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream_SendJsonCallback (json_object *jsonObj, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_SendJsonCallback ....");
    // -- Connection callback to receive the Json --
    
    // local declarations
    ARCONTROLLER_Stream_t *streamController = (ARCONTROLLER_Stream_t *)customData;
    ARDISCOVERY_DEVICE_WIFI_t *specificWifiParam = NULL;
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    json_object *valueJsonObj = NULL;
    
    // Check parameters
    if ((jsonObj == NULL) ||
        (streamController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream_ReceiveJsonCallback (json_object *jsonObj, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_ReceiveJsonCallback ....");
    
    // -- Connection callback to receive the Json --
    
    // local declarations
    ARCONTROLLER_Stream_t *streamController = (ARCONTROLLER_Stream_t *)customData;
    ARDISCOVERY_DEVICE_WIFI_t *specificWifiParam = NULL;
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
            ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamController->fragmentSize : %d ....", streamController->fragmentSize);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_FRAGMENT_MAXIMUM_NUMBER_KEY);
        if (valueJsonObj != NULL)
        {
            streamController->maxNumberOfFragement = json_object_get_int(valueJsonObj);
            ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamController->maxNumberOfFragement : %d ....", streamController->maxNumberOfFragement);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY
        valueJsonObj = json_object_object_get (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM_MAX_ACK_INTERVAL_KEY);
        if (valueJsonObj != NULL)
        {
            streamController->maxAckInterval = json_object_get_int(valueJsonObj);
            ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamController->maxAckInterval : %d ....", streamController->maxAckInterval);
        }
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // Initialization of the stream buffers
        controllerError = ARCONTROLLER_Stream_InitStreamBuffers (streamController);
        
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM_TAG, "Error occurred durring Initialization of the stream buffers : %s ", ARCONTROLLER_Error_ToString (controllerError));
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream_InitStreamBuffers (ARCONTROLLER_Stream_t *streamController)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "ARCONTROLLER_Stream_InitStreamBuffers ....");
    
    // -- Connection callback to receive the Json --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int streamAckBufferIndex = -1;
    int streamDataBufferIndex = -1;
    
    // Check parameters
    if ((streamController == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamController->networkConfiguration.controllerToDeviceARStreamAck %d ....", streamController->networkConfiguration.controllerToDeviceARStreamAck);
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamController->networkConfiguration.deviceToControllerARStreamData %d ....", streamController->networkConfiguration.deviceToControllerARStreamData);
        
        streamAckBufferIndex = ARCONTROLLER_Stream_IdToIndex (streamController->networkConfiguration.controllerToDeviceParams, streamController->networkConfiguration.numberOfControllerToDeviceParam, streamController->networkConfiguration.controllerToDeviceARStreamAck);
        streamDataBufferIndex = ARCONTROLLER_Stream_IdToIndex (streamController->networkConfiguration.deviceToControllerParams, streamController->networkConfiguration.numberOfDeviceToControllerParam, streamController->networkConfiguration.deviceToControllerARStreamData);
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamAckBufferIndex %d ....", streamAckBufferIndex);
        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "streamDataBufferIndex %d ....", streamDataBufferIndex);
        
        if ((streamAckBufferIndex != -1) &&
           (streamDataBufferIndex != -1))
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM_TAG, "efkjgnhsdkfjghsqdkfjghs ....");
            
            ARSTREAM_Reader_InitStreamAckBuffer (&(streamController->networkConfiguration.controllerToDeviceParams[streamAckBufferIndex]), streamController->networkConfiguration.controllerToDeviceARStreamAck);
            ARSTREAM_Reader_InitStreamDataBuffer (&(streamController->networkConfiguration.deviceToControllerParams[streamDataBufferIndex]), streamController->networkConfiguration.deviceToControllerARStreamData, streamController->fragmentSize, streamController->maxNumberOfFragement);
        }
        //NO ELSE ; device has not streaming
    }
    
    return error;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

int ARCONTROLLER_Stream_IdToIndex (ARNETWORK_IOBufferParam_t *parameters, int numberOfParameters, int id)
{
    // -- Get parameter buffer from id --
    
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
    return index;
}

