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
 * @file ARNETWORK_Network.c
 * @brief ARCONTROLLER_Network allow to operate an ARNETWORK_Manager for send and receive commands.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Socket.h>
#include <libARSAL/ARSAL_Mutex.h>
#include <libARNetwork/ARNETWORK_Manager.h>
#include <libARDiscovery/ARDISCOVERY_NetworkConfiguration.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <libARCommands/ARCommands.h>
#include <libARController/ARCONTROLLER_Stream.h>
#include <libARController/ARCONTROLLER_Network.h>

#include "ARCONTROLLER_Stream.h"
#include "ARCONTROLLER_Network.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Network_t *ARCONTROLLER_Network_New (ARDISCOVERY_Device_t *discoveryDevice, ARCONTROLLER_Network_DisconnectionCallback_t disconnectionCallback, ARDISCOVERY_Device_ConnectionJsonCallback_t sendJsonCallback, ARDISCOVERY_Device_ConnectionJsonCallback_t receiveJsonCallback, void *customData, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Network Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Network_t *networkController =  NULL;
    eARDISCOVERY_ERROR dicoveryError = ARDISCOVERY_OK;
    
    // Check parameters
    if (discoveryDevice == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Network Controller
        networkController = malloc (sizeof (ARCONTROLLER_Network_t));
        if (networkController != NULL)
        {
            // Initialize to default values
            networkController->discoveryDevice = NULL;
            networkController->networkALManager = NULL;
            networkController->networkManager = NULL;
            networkController->rxThread = NULL;
            networkController->txThread = NULL;
            networkController->readerThreads = NULL;
            networkController->readerThreadsData = NULL;
            networkController->state = ARCONTROLLER_NETWORK_STATE_RUNNING;
            
            //video part
            networkController->hasVideo = 0;
            networkController->videoController = NULL;
            
            //Connection callback
            networkController->sendJsonCallback = sendJsonCallback;
            networkController->receiveJsonCallback = receiveJsonCallback;
            networkController->disconnectionCallback = disconnectionCallback;
            networkController->callbacksCustomData = customData;
            
            // init networkConfiguration
            networkController->networkConfig.controllerToDeviceNotAckId = -1;
            networkController->networkConfig.controllerToDeviceAckId = -1;
            networkController->networkConfig.controllerToDeviceHightPriority = -1;
            networkController->networkConfig.controllerToDeviceARStreamAck = -1;
            networkController->networkConfig.deviceToControllerNotAckId = -1;
            networkController->networkConfig.deviceToControllerAckId = -1;
            //networkController->networkConfig.deviceToControllerHightPriority = -1;
            networkController->networkConfig.deviceToControllerARStreamData = -1;
            networkController->networkConfig.numberOfControllerToDeviceParam = 0;
            networkController->networkConfig.controllerToDeviceParams = NULL;
            networkController->networkConfig.numberOfDeviceToControllerParam  = 0;
            networkController->networkConfig.deviceToControllerParams = NULL;
            networkController->networkConfig.pingDelayMs =-1;
            networkController->networkConfig.numberOfDeviceToControllerCommandsBufferIds = 0;
            networkController->networkConfig.deviceToControllerCommandsBufferIds = NULL;
            
            /* Create the mutex/condition */
            if ( (localError == ARCONTROLLER_OK) &&
                 (ARSAL_Mutex_Init (&(networkController->mutex)) != 0))
            {
                localError = ARCONTROLLER_ERROR_INIT_MUTEX;
            }
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // Copy the device
        eARDISCOVERY_ERROR dicoveryError = ARDISCOVERY_OK;
        
        networkController->discoveryDevice = ARDISCOVERY_Device_NewByCopy (discoveryDevice, &dicoveryError);
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_DEVICE_COPY;
        }
    }
    
    // Check if it is a wifi device
    if ((localError == ARCONTROLLER_OK) && 
        (ARDISCOVERY_getProductService (networkController->discoveryDevice->productID) == ARDISCOVERY_PRODUCT_NSNETSERVICE))
    {
        // Add callbacks for the connection json part
        dicoveryError = ARDISCOVERY_Device_WifiAddConnectionCallbacks (networkController->discoveryDevice, ARCONTROLLER_Network_OnSendJson, ARCONTROLLER_Network_OnReceiveJson, networkController);
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_DEVICE_JSON_CALLBACK;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // Initialize the network Configuration
        eARDISCOVERY_ERROR dicoveryError = ARDISCOVERY_OK;
        dicoveryError = ARDISCOVERY_Device_InitNetworkConfiguration (networkController->discoveryDevice, &(networkController->networkConfig));
        if (dicoveryError != ARDISCOVERY_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_NETWORK_CONFIG;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // Check if the device has video
        if (networkController->networkConfig.deviceToControllerARStreamData != -1)
        {
            networkController->hasVideo = 1;
            networkController->videoController = ARCONTROLLER_Stream_New (&(networkController->networkConfig), networkController->discoveryDevice, &localError);
        }
        //NO else ; device has not video
    }
    // No else: skipped by an error
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the NetworkAL
        eARDISCOVERY_ERROR dicoveryError = ARDISCOVERY_OK;
        eARNETWORKAL_ERROR netALError = ARNETWORKAL_OK;
        networkController->networkALManager = ARDISCOVERY_Device_NewARNetworkAL (networkController->discoveryDevice, &dicoveryError, &netALError);
        if (dicoveryError != ARDISCOVERY_OK)
        {
            if (netALError != ARNETWORKAL_OK)
            {
                ARSAL_PRINT (ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "error: %s", ARNETWORKAL_Error_ToString (netALError));
            }
            
            localError = ARCONTROLLER_ERROR_INIT_ARNETWORKAL_MANAGER;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the ARNetworkManager.
        eARNETWORK_ERROR netError = ARNETWORK_OK;
        
        networkController->networkManager = ARNETWORK_Manager_New (networkController->networkALManager, networkController->networkConfig.numberOfControllerToDeviceParam, networkController->networkConfig.controllerToDeviceParams, networkController->networkConfig.numberOfDeviceToControllerParam, networkController->networkConfig.deviceToControllerParams, networkController->networkConfig.pingDelayMs, ARCONTROLLER_Network_OnDisconnectNetwork, networkController, &netError);
        if (netError != ARNETWORK_OK)
        {
            localError = ARCONTROLLER_ERROR_INIT_ARNETWORK_MANAGER;
        }
    }

    if (localError == ARCONTROLLER_OK)
    {
        // Create the Network receiver and transmitter Threads
        localError = ARCONTROLLER_Network_CreateNetworkThreads (networkController);
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the reader Threads
        localError = ARCONTROLLER_Network_CreateReaderThreads (networkController);
    }
    
    // delete the Network Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "error: %s", ARCONTROLLER_Error_ToString (localError));
        ARCONTROLLER_Network_Delete (&networkController);
    }
    // No else: skipped by an error 

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return networkController;
}

void ARCONTROLLER_Network_Delete (ARCONTROLLER_Network_t **networkController)
{
    // -- Delete the Network Controller --

    if (networkController != NULL)
    {
        if ((*networkController) != NULL)
        {
            (*networkController)->state = ARCONTROLLER_NETWORK_STATE_STOPPED;
            
            ARSAL_Mutex_Destroy (&((*networkController)->mutex));
                        
            // Check if the device has video
            if ((*networkController)->hasVideo)
            {
                ARCONTROLLER_Stream_Delete (&((*networkController)->videoController));
            }
            //NO else ; device has not video
            
            ARCONTROLLER_Network_StopReaderThreads (*networkController); //TODO read error !!!!!!!!!!
            
            ARCONTROLLER_Network_StopNetworkThreads (*networkController); //TODO  read error  !!!!!!!!!!
            
            ARNETWORK_Manager_Delete(&((*networkController)->networkManager));
            
            ARDISCOVERY_Device_DeleteARNetworkAL ((*networkController)->discoveryDevice, &((*networkController)->networkALManager)); //TODO  read error  !!!!!!!!!!
            
            ARDISCOVERY_Device_Delete (&((*networkController)->discoveryDevice));
            
            free (*networkController);
            (*networkController) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_Pause (ARCONTROLLER_Network_t *networkController)
{
    // -- Pauses the Network Controller --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        switch (networkController->state)
        {
            case ARCONTROLLER_NETWORK_STATE_RUNNING:
                networkController->state = ARCONTROLLER_NETWORK_STATE_PAUSE;
                break;
                
            case ARCONTROLLER_NETWORK_STATE_PAUSE:
            case ARCONTROLLER_NETWORK_STATE_STOPPED:
                ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_NETWORK_TAG, "Nothing to do ; Network controller state : %d ", networkController->state);
                break;
            
            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "State : %d not known", networkController->state);
                break;
        }
        
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_Resume (ARCONTROLLER_Network_t *networkController)
{
    // -- Resumes the Network Controller --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        switch (networkController->state)
        {
            case ARCONTROLLER_NETWORK_STATE_PAUSE:
                networkController->state = ARCONTROLLER_NETWORK_STATE_RUNNING;
                break;
                
            case ARCONTROLLER_NETWORK_STATE_RUNNING:
                ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_NETWORK_TAG, "Nothing to do ; Network controller state : %d ", networkController->state);
                break;
                
            case ARCONTROLLER_NETWORK_STATE_STOPPED:
                error = ARCONTROLLER_ERROR_STATE;
                break;
            
            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "State : %d not known", networkController->state);
                break;
        }
        
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_SetVideoReceiveCallback (ARCONTROLLER_Network_t *networkController, ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback, ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback, ARCONTROLLER_Stream_TimeoutFrameCallback_t timeoutFrameCallback, void *customData)
{
    // -- Set Video Receive Callback --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int locked = 0;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
        else
        {
            locked = 1;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if (networkController->videoController != NULL)
        {
            error = ARCONTROLLER_Stream_SetReceiveFrameCallback (networkController->videoController, decoderConfigCallback, receiveFrameCallback, timeoutFrameCallback, customData);
        }
        else
        {
            error = ARCONTROLLER_ERROR_NO_VIDEO;
        }
    }
        
    if (locked)
    {
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_StartVideoStream (ARCONTROLLER_Network_t *networkController)
{
    // -- Start Video stream --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int locked = 0;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
        else
        {
            locked = 1;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // Check if the device has video
        if (networkController->hasVideo)
        {
            error = ARCONTROLLER_Stream_Start (networkController->videoController, networkController->networkManager);
        }
        //NO else ; device has not video
    }
    // No else: skipped by an error
    
    if (locked)
    {
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_StopVideoStream (ARCONTROLLER_Network_t *networkController)
{
    // -- Stop Video stream --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int locked = 0;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
        else
        {
            locked = 1;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // Check if the device has video
        if (networkController->hasVideo)
        {
            error = ARCONTROLLER_Stream_Stop (networkController->videoController);
        }
        //NO else ; device has not video
    }
    // No else: skipped by an error
    
    if (locked)
    {
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_SetVideoStreamMP4Compliant (ARCONTROLLER_Network_t *networkController, int isMP4Compliant)
{
    // -- Set video stream compliant with the mp4 format --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    int locked = 0;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARCONTROLLER_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Mutex_Lock (&(networkController->mutex)) != 0)
        {
            error = ARCONTROLLER_ERROR_MUTEX;
        }
        else
        {
            locked = 1;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // Check if the device has video
        if (networkController->hasVideo)
        {
            error = ARCONTROLLER_Stream_SetMP4Compliant (networkController->videoController, isMP4Compliant);
        }
        //NO else ; device has not video
    }
    // No else: skipped by an error
    
    if (locked)
    {
        ARSAL_Mutex_Unlock (&(networkController->mutex));
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_SendData (ARCONTROLLER_Network_t *networkController, void *data, int dataSize, eARCONTROLLER_NETWORK_SENDING_DATA_TYPE dataType, eARNETWORK_MANAGER_CALLBACK_RETURN timeoutPolicy, eARNETWORK_ERROR *netError)
{
    // -- Send data through the Network --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    eARNETWORK_ERROR localNetError = ARNETWORK_OK;
    ARCONTROLLER_NETWORK_SendingConfiguration_t *sendingConfig = NULL;
    int bufferID = -1; 
    
    // Check parameters
    if ((networkController == NULL) ||
        (data == NULL) || (dataSize <= 0) ||
        (dataType == ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_MAX))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        sendingConfig = malloc(sizeof(ARCONTROLLER_NETWORK_SendingConfiguration_t));
        if (sendingConfig != NULL)
        {
            sendingConfig->timeoutPolicy = timeoutPolicy;
            sendingConfig->customData = NULL;
        }
        else
        {
            error = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // find bufferID to send
        switch (dataType)
        {
            case ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_NOT_ACK:
                bufferID = networkController->networkConfig.controllerToDeviceNotAckId;
                break;
                
            case ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_ACK:
                bufferID = networkController->networkConfig.controllerToDeviceAckId;
                break;
                
            case ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_HIGH_PRIORITY:
                bufferID = networkController->networkConfig.controllerToDeviceHightPriority;
                break;
                
            case ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_STREAM:
                bufferID = networkController->networkConfig.controllerToDeviceARStreamAck;
                break;
                
            case ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_MAX:
                error = ARCONTROLLER_ERROR;
                break;
            
            default:
                ARSAL_PRINT (ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "DataType : %d not known", dataType);
                error = ARCONTROLLER_ERROR;
                break;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        localNetError = ARNETWORK_Manager_SendData(networkController->networkManager, bufferID, data, dataSize, sendingConfig, &(ARCONTROLLER_Network_SendingCallback), 1);
        
        if (localNetError != ARNETWORK_OK)
        {
            error = ARCONTROLLER_ERROR_NOT_SENT;
        }
    }
    
    // Return netError
    if(netError != NULL)
    {
        *netError = localNetError;
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Network_OnSendJson (json_object *jsonObj, void *customData)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    ARCONTROLLER_Network_t *networkController = (ARCONTROLLER_Network_t *)customData;
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    // Check parameters
    if ((jsonObj == NULL) ||
        (networkController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        if (networkController->videoController != NULL)
        {
            error = ARCONTROLLER_Stream_OnSendJson (networkController->videoController, jsonObj);
        }
    }
    
    if (error == ARDISCOVERY_OK)
    {
        if (networkController->sendJsonCallback != NULL)
        {
            error = networkController->sendJsonCallback (jsonObj, networkController->callbacksCustomData);
        }
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Network_OnReceiveJson (json_object *jsonObj, void *customData)
{
    // -- Connection callback to receive the Json --
    
    // Local declarations
    ARCONTROLLER_Network_t *networkController = (ARCONTROLLER_Network_t *)customData;
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    if ((jsonObj == NULL) ||
        (networkController == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        if (networkController->videoController != NULL)
        {
            error = ARCONTROLLER_Stream_OnReceiveJson (networkController->videoController, jsonObj);
        }
    }
    
    if (error == ARDISCOVERY_OK)
    {
        if (networkController->receiveJsonCallback != NULL)
        {
            error = networkController->receiveJsonCallback ( jsonObj, networkController->callbacksCustomData);
        }
    }
    
    return error;
}

 /*************************
 * Private Implementation
 *************************/

eARCONTROLLER_ERROR ARCONTROLLER_Network_CreateNetworkThreads (ARCONTROLLER_Network_t *networkController)
{
    // -- Create the Network receiver and transmitter Threads --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Thread_Create(&(networkController->rxThread), ARNETWORK_Manager_ReceivingThreadRun, networkController->networkManager) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Creation of Rx thread failed.");
            error = ARCONTROLLER_ERROR_INIT_THREAD;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Thread_Create(&(networkController->txThread), ARNETWORK_Manager_SendingThreadRun, networkController->networkManager) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Creation of Tx thread failed.");
            error = ARCONTROLLER_ERROR_INIT_THREAD;
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_StopNetworkThreads (ARCONTROLLER_Network_t *networkController)
{
    // -- Stop the Network receiver and transmitter Threads --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ARNETWORK_Manager_Stop(networkController->networkManager);
        if (networkController->rxThread != NULL)
        {
            ARSAL_Thread_Join(networkController->rxThread, NULL);
            ARSAL_Thread_Destroy(&(networkController->rxThread));
            networkController->rxThread = NULL;
        }
        
        if (networkController->txThread != NULL)
        {
            ARSAL_Thread_Join(networkController->txThread, NULL);
            ARSAL_Thread_Destroy(&(networkController->txThread));
            networkController->txThread = NULL;
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_CreateReaderThreads (ARCONTROLLER_Network_t *networkController)
{
    // -- Create the reader Threads --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // allocate reader thread array.
        networkController->readerThreads = calloc(networkController->networkConfig.numberOfDeviceToControllerCommandsBufferIds, sizeof(ARSAL_Thread_t));
        
        if (networkController->readerThreads == NULL)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Allocation of reader threads failed.");
            error = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // allocate reader thread data array.
        networkController->readerThreadsData = calloc(networkController->networkConfig.numberOfDeviceToControllerCommandsBufferIds, sizeof(ARCONTROLLER_NETWORK_THREAD_DATA_t));
        
        if (networkController->readerThreadsData == NULL)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Allocation of reader threads data failed.");
            error = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // Create and start reader threads.
        int readerThreadIndex = 0;
        for (readerThreadIndex = 0 ; readerThreadIndex < networkController->networkConfig.numberOfDeviceToControllerCommandsBufferIds ; readerThreadIndex++)
        {
            // initialize reader thread data
            networkController->readerThreadsData[readerThreadIndex].networkController = networkController;
            networkController->readerThreadsData[readerThreadIndex].readerBufferId = networkController->networkConfig.deviceToControllerCommandsBufferIds[readerThreadIndex];
            
            if (ARSAL_Thread_Create(&(networkController->readerThreads[readerThreadIndex]), ARCONTROLLER_Network_ReaderRun, &(networkController->readerThreadsData[readerThreadIndex])) != 0)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Creation of reader thread failed.");
                error = ARCONTROLLER_ERROR_INIT_THREAD;
                break; //Stop the loop of the creation of the reader threads
            }
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Network_StopReaderThreads (ARCONTROLLER_Network_t *networkController)
{
    // -- Stop the reader Threads --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (networkController == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        if (networkController->readerThreads != NULL)
        {
            // Stop reader Threads
            int readerThreadIndex = 0;
            for (readerThreadIndex = 0 ; readerThreadIndex < networkController->networkConfig.numberOfDeviceToControllerCommandsBufferIds ; readerThreadIndex++)
            {
                if (networkController->readerThreads[readerThreadIndex] != NULL)
                {
                    ARSAL_Thread_Join (networkController->readerThreads[readerThreadIndex], NULL);
                    ARSAL_Thread_Destroy (&(networkController->readerThreads[readerThreadIndex]));
                    networkController->readerThreads[readerThreadIndex] = NULL;
                }
            }
            
            // free reader thread array
            free (networkController->readerThreads);
            networkController->readerThreads = NULL;
        }
        
        if (networkController->readerThreadsData != NULL)
        {
            // free reader thread data array
            free (networkController->readerThreadsData);
            networkController->readerThreadsData = NULL;
        }
    }
    
    return error;
}

void *ARCONTROLLER_Network_ReaderRun (void *data)
{
    // -- Read and decode one input network buffer --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    ARCONTROLLER_Network_t *networkController = NULL;
    int readerBufferId = -1;
    
    // Allocate some space for incoming data.
    const size_t maxLength = 128 * 1024;
    void *readData = malloc (maxLength);
    if (readData == NULL)
    {
        error = ARCONTROLLER_ERROR_ALLOC;
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if (data != NULL)
        {
            //get thread data
            networkController = ((ARCONTROLLER_NETWORK_THREAD_DATA_t *) data)->networkController;
            readerBufferId = ((ARCONTROLLER_NETWORK_THREAD_DATA_t *) data)->readerBufferId;
            
            if (networkController == NULL)
            {
                error = ARCONTROLLER_ERROR_BAD_PARAMETER;
            }
        }
        else
        {
            error = ARCONTROLLER_ERROR_BAD_PARAMETER;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        while (networkController->state != ARCONTROLLER_NETWORK_STATE_STOPPED)
        {
            if (networkController->state == ARCONTROLLER_NETWORK_STATE_RUNNING)
            {
                eARNETWORK_ERROR netError = ARNETWORK_OK;
                int length = 0;
                int skip = 0;
                
                //read data
                netError = ARNETWORK_Manager_ReadDataWithTimeout (networkController->networkManager, readerBufferId, readData, maxLength, &length, ARCONTROLLER_NETWORK_READING_TIMEOUT_MS);
                if (netError != ARNETWORK_OK)
                {
                    if (netError != ARNETWORK_ERROR_BUFFER_EMPTY)
                    {
                        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "ARNETWORK_Manager_ReadDataWithTimeout () failed : %s", ARNETWORK_Error_ToString(netError));
                    }
                    skip = 1;
                }
                
                if (!skip)
                {
                    // Forward data to the CommandsManager
                    eARCOMMANDS_DECODER_ERROR cmdError = ARCOMMANDS_DECODER_OK;
                    cmdError = ARCOMMANDS_Decoder_DecodeBuffer ((uint8_t *)readData, length);
                    if ((cmdError != ARCOMMANDS_DECODER_OK) && (cmdError != ARCOMMANDS_DECODER_ERROR_NO_CALLBACK))
                    {
                        char msg[128];
                        ARCOMMANDS_Decoder_DescribeBuffer ((uint8_t *)readData, length, msg, sizeof(msg));
                        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "ARCOMMANDS_Decoder_DecodeBuffer () failed : %d %s", cmdError, msg);
                    }
                }
                //NO ELSE ; no data read
            }
            else
            {
                //sleep
                sleep (1); //TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! replace by signal 
            }
        }
    }
    
    // Print Error
    if (error != ARCONTROLLER_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "error: %s", ARCONTROLLER_Error_ToString (error));
    }
    
    // cleanup
    if (readData != NULL)
    {
        free (readData);
        readData = NULL;
    }
    
    return NULL;
}

void ARCONTROLLER_Network_OnDisconnectNetwork (ARNETWORK_Manager_t *manager, ARNETWORKAL_Manager_t *alManager, void *customData)
{
    ARCONTROLLER_Network_t *networkController = (ARCONTROLLER_Network_t *)customData;
    
    if ((networkController != NULL) && (networkController->disconnectionCallback != NULL))
    {
        networkController->disconnectionCallback (networkController->callbacksCustomData);
    }
}

eARNETWORK_MANAGER_CALLBACK_RETURN ARCONTROLLER_Network_SendingCallback(int buffer_id, uint8_t *data, void *custom, eARNETWORK_MANAGER_CALLBACK_STATUS cause)
{
    eARNETWORK_MANAGER_CALLBACK_RETURN retval = ARNETWORK_MANAGER_CALLBACK_RETURN_DEFAULT;

    ARCONTROLLER_NETWORK_SendingConfiguration_t *sendingConfig = ((ARCONTROLLER_NETWORK_SendingConfiguration_t *)custom);

    switch (cause)
    {
        case ARNETWORK_MANAGER_CALLBACK_STATUS_SENT:
        case ARNETWORK_MANAGER_CALLBACK_STATUS_ACK_RECEIVED:
        case ARNETWORK_MANAGER_CALLBACK_STATUS_CANCEL:
            //call customCallback
            break;
        
        case ARNETWORK_MANAGER_CALLBACK_STATUS_TIMEOUT:
            if (sendingConfig != NULL)
            {
                retval = sendingConfig->timeoutPolicy;
                //retval = call customCallback
            }
            else
            {
                retval = ARNETWORK_MANAGER_CALLBACK_RETURN_DATA_POP;
            }
            break;
            
        case ARNETWORK_MANAGER_CALLBACK_STATUS_DONE:
            // End of the sending process : free the customData
            if (sendingConfig != NULL)
            {
                free (sendingConfig);
                sendingConfig = NULL;
            }
            break;
            
        case ARNETWORK_MANAGER_CALLBACK_STATUS_FREE:
            free(data);
            data = NULL;
            break;
            
        default:
            ARSAL_PRINT (ARSAL_PRINT_ERROR, ARCONTROLLER_NETWORK_TAG, "Cause: %d not known", cause);
            break;
    }
    
    return retval;
}
