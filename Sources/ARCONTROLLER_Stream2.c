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
 * @file ARCONTROLLER_Stream2.c
 * @brief ARCONTROLLER_Stream2 allow to operate Stream 2 for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <json-c/json.h>
#include <libARSAL/ARSAL_Print.h>
#include <libARSAL/ARSAL_Socket.h>
#include <libARSAL/ARSAL_Thread.h>
#include <libARDiscovery/ARDISCOVERY_Error.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <libARStream2/arstream2_stream_receiver.h>
#include <libARController/ARCONTROLLER_Error.h>

#include <libARController/ARCONTROLLER_StreamPool.h>
#include <libARController/ARCONTROLLER_Stream.h>

#include "ARCONTROLLER_Stream2.h"
#include <libARController/ARCONTROLLER_Stream2.h>
#if defined BUILD_LIBMUX
#include <libmux.h>
#endif

#define DEFAULT_VIDEO_STREAM_PORT 55004
#define DEFAULT_VIDEO_CONTROL_PORT 55005

/*************************
 * Private header
 *************************/

static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_StartStream (ARCONTROLLER_Stream2_t *stream2Controller);
static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_StopStream (ARCONTROLLER_Stream2_t *stream2Controller);
static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_RestartStream (ARCONTROLLER_Stream2_t *stream2Controller);
static eARSTREAM2_ERROR ARCONTROLLER_Stream2_SpsPpsCallback(uint8_t *spsBuffer, int spsSize, uint8_t *ppsBuffer, int ppsSize, void *userPtr);
static eARSTREAM2_ERROR ARCONTROLLER_Stream2_GetAuBufferCallback(uint8_t **auBuffer, int *auBufferSize, void **auBufferUserPtr, void *userPtr);
static eARSTREAM2_ERROR ARCONTROLLER_Stream2_AuReadyCallback(uint8_t *auBuffer, int auSize, ARSTREAM2_StreamReceiver_AuReadyCallbackTimestamps_t *auTimestamps, eARSTREAM2_STREAM_RECEIVER_AU_SYNC_TYPE auSyncType, ARSTREAM2_StreamReceiver_AuReadyCallbackMetadata_t *auMetadata, void *auBufferUserPtr, void *userPtr);
static void *ARCONTROLLER_Stream2_RestartRun (void *data);

#define ARCONTROLLER_STREAM2_TAG "ARCONTROLLER_Stream2"

/*************************
 * Implementation
 *************************/

static int ARCONTROLLER_Stream2_Open_Socket(const char *name, int defaultPort, int *sockfd, int *port)
{
    int fd, ret;
    socklen_t addrlen;
    struct sockaddr_in addr;
    int yes;

    fd = ARSAL_Socket_Create (AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        goto error;

    ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    if (ret < 0)
        goto error;

    /*  try to bind to the default port */
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (defaultPort);
    ret = ARSAL_Socket_Bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        if (errno != EADDRINUSE) {
            ret = errno;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG,
                        "bind fd=%d, addr='0.0.0.0', port=%d: error='%s'", fd, defaultPort, strerror(ret));
            goto error;
        }

        ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM2_TAG,
                    "bind fd=%d, addr='0.0.0.0', port=%d: error='%s'. Will try a random port.", fd, defaultPort,
                    strerror(ret));

        /*  bind to a OS-assigned random port */
        addr.sin_port = htons (0);
        ret = ARSAL_Socket_Bind(fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            ret = errno;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG,
                        "bind fd=%d, addr='0.0.0.0', port=0: error='%s'", fd, strerror(ret));
            goto error;
        }
    }

    /* get selected port */
    addrlen = sizeof(addr);
    ret = ARSAL_Socket_Getsockname(fd, (struct sockaddr *)&addr, &addrlen);
    if (ret < 0) {
        ret = errno;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "getsockname fd=%d, error='%s'", fd, strerror(ret));
        goto error;
    }

    yes = 1;
    ret = ARSAL_Socket_Setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (ret < 0) {
        ret = errno;
        ARSAL_PRINT(ARSAL_PRINT_WARNING, ARCONTROLLER_STREAM2_TAG, "Failed to set socket option SO_REUSEADDR: error=%d (%s)", ret, strerror(ret));
    }

    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_STREAM2_TAG, "udp local port %s: %d", name, htons(addr.sin_port));
    *port = htons(addr.sin_port);
    *sockfd = fd;
    return 0;
error:
    if (fd >= 0)
        ARSAL_Socket_Close(fd);

    return -1;
}

ARCONTROLLER_Stream2_t *ARCONTROLLER_Stream2_New (ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Stream 2 Controller --

    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Stream2_t *stream2Controller =  NULL;
    int ret;
    
    // Check parameters
    if (discoveryDevice == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Stream 2 Controller
        stream2Controller = calloc (1, sizeof (ARCONTROLLER_Stream2_t));
        if (stream2Controller != NULL)
        {
            stream2Controller->isRunning = 0;
            
            stream2Controller->serverAddress[0] = '\0';
            if (discoveryDevice->networkType == ARDISCOVERY_NETWORK_TYPE_NET) {
                ARDISCOVERY_DEVICE_WifiGetIpAddress (discoveryDevice, stream2Controller->serverAddress, ARCONTROLLER_STREAM2_IP_SIZE);
                stream2Controller->mux = NULL;
            } else if (discoveryDevice->networkType == ARDISCOVERY_NETWORK_TYPE_USBMUX) {
#if defined BUILD_LIBMUX
                ARDISCOVERY_Device_UsbGetMux(discoveryDevice, &stream2Controller->mux);
                mux_ref(stream2Controller->mux);
#else
                localError = ARCONTROLLER_ERROR_NOT_IMPLEMENTED;
#endif
            }
            
            ret = ARCONTROLLER_Stream2_Open_Socket("stream", DEFAULT_VIDEO_STREAM_PORT,
                                                   &stream2Controller->clientStreamFd,
                                                   &stream2Controller->clientStreamPort);
            if (ret < 0)
                localError = ARCONTROLLER_ERROR_INIT_NETWORK_CONFIG;

            ret = ARCONTROLLER_Stream2_Open_Socket("control", DEFAULT_VIDEO_CONTROL_PORT,
                                                   &stream2Controller->clientControlFd,
                                                   &stream2Controller->clientControlPort);
            if (ret < 0)
                localError = ARCONTROLLER_ERROR_INIT_NETWORK_CONFIG;

            stream2Controller->serverStreamPort = 0;
            stream2Controller->serverControlPort = 0;
            stream2Controller->maxPacketSize = 0;
            stream2Controller->qos_level = 0;
            
            stream2Controller->errorCount = 0;
            stream2Controller->replaceStartCodesWithNaluSize = 0;
            stream2Controller->ardiscoveryProductType = discoveryDevice->productID;
            
            stream2Controller->callbackData = NULL;
            stream2Controller->decoderConfigCallback = NULL;
            stream2Controller->receiveFrameCallback = NULL;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    // delete the Network Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_Stream2_Delete (&stream2Controller);
    }
    // No else: skipped by an error 

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return stream2Controller;
}

void ARCONTROLLER_Stream2_Delete (ARCONTROLLER_Stream2_t **stream2Controller)
{
    // -- Delete the Stream 2 Controller --
    
    if (stream2Controller != NULL)
    {
        if ((*stream2Controller) != NULL)
        {
            ARCONTROLLER_Stream2_Stop (*stream2Controller);

#if defined BUILD_LIBMUX
            if ((*stream2Controller)->mux)
                mux_unref((*stream2Controller)->mux);
#endif
            if ((*stream2Controller)->clientStreamFd >= 0)
                ARSAL_Socket_Close((*stream2Controller)->clientStreamFd);

            if ((*stream2Controller)->clientControlFd >= 0)
                ARSAL_Socket_Close((*stream2Controller)->clientControlFd);

            free (*stream2Controller);
            (*stream2Controller) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream2_Start (ARCONTROLLER_Stream2_t *stream2Controller)
{
    // -- Start to read the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (!stream2Controller->isRunning))
    {
        stream2Controller->isRunning = 1;
        
        if (ARCONTROLLER_Stream2_IsInitilized(stream2Controller) != 0)
        {
            // create the frame pool
            stream2Controller->framePool = ARCONTROLLER_StreamPool_New (ARCONTROLLER_STREAM2_AU_ARRAY_SIZE, &error);
        }
        else
        {
            error = ARCONTROLLER_ERROR_INIT_STREAM;
        }
        
        if (error == ARCONTROLLER_OK)
        {
            error = ARCONTROLLER_Stream2_StartStream (stream2Controller);
        }
        
        if (error != ARCONTROLLER_OK)
        {
            ARCONTROLLER_Stream2_Stop (stream2Controller);
        }
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream2_Stop (ARCONTROLLER_Stream2_t *stream2Controller)
{
    // -- Stop to read the stream --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if ((error == ARCONTROLLER_OK) && (stream2Controller->isRunning))
    {
        stream2Controller->isRunning = 0;
        
        ARCONTROLLER_Stream2_StopStream (stream2Controller);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream2_SetCallbacks(ARCONTROLLER_Stream2_t *stream2Controller, ARCONTROLLER_Stream_DecoderConfigCallback_t decoderConfigCallback, ARCONTROLLER_Stream_DidReceiveFrameCallback_t receiveFrameCallback, void *customData)
{
    // -- Set Stream2 Callbacks --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARCONTROLLER_OK)
    {
        stream2Controller->callbackData = customData;
        stream2Controller->decoderConfigCallback = decoderConfigCallback;
        stream2Controller->receiveFrameCallback = receiveFrameCallback;
    }
    
    return error;
}

int ARCONTROLLER_Stream2_IsRunning (ARCONTROLLER_Stream2_t *stream2Controller, eARCONTROLLER_ERROR *error)
{
    // -- Get is running --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    int isRunning = 0;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        isRunning = stream2Controller->isRunning;
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: Error is not returned 
    
    return isRunning;
}

eARCONTROLLER_ERROR ARCONTROLLER_Stream2_SetMP4Compliant (ARCONTROLLER_Stream2_t *stream2Controller, int isMP4Compliant)
{
    // -- Set stream compliant with the mp4 format. --
    
    // local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARCONTROLLER_OK)
    {
        stream2Controller->replaceStartCodesWithNaluSize = isMP4Compliant;
    }
    
    return error;
}

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

eARDISCOVERY_ERROR ARCONTROLLER_Stream2_OnSendJson (ARCONTROLLER_Stream2_t *stream2Controller, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    
    json_object *valueJsonObj = NULL;
    
    // Check parameters
    if ((jsonObj == NULL) ||
        (stream2Controller == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // add ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_CLIENT_STREAM_PORT_KEY
        valueJsonObj = json_object_new_int (stream2Controller->clientStreamPort);
        json_object_object_add (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_CLIENT_STREAM_PORT_KEY, valueJsonObj);
        
        // add ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_CLIENT_CONTROL_PORT_KEY
        valueJsonObj = json_object_new_int (stream2Controller->clientControlPort);
        json_object_object_add (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_CLIENT_CONTROL_PORT_KEY, valueJsonObj);

        // add ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SUPPORTED_METADATA_VERSION_KEY
        valueJsonObj = json_object_new_int (1);
        json_object_object_add (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SUPPORTED_METADATA_VERSION_KEY, valueJsonObj);
    }
    
    return error;
}

eARDISCOVERY_ERROR ARCONTROLLER_Stream2_OnReceiveJson (ARCONTROLLER_Stream2_t *stream2Controller, json_object *jsonObj)
{
    // -- Connection callback to receive the Json --
    
    // local declarations
    eARDISCOVERY_ERROR error = ARDISCOVERY_OK;
    json_object *valueJsonObj = NULL;
    json_bool res;
    
    if ((jsonObj == NULL) ||
        (stream2Controller == NULL))
    {
        error = ARDISCOVERY_ERROR_BAD_PARAMETER;
    }
    
    if (error == ARDISCOVERY_OK)
    {
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_STREAM_PORT_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_STREAM_PORT_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream2Controller->serverStreamPort = json_object_get_int(valueJsonObj);
        }
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_CONTROL_PORT_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_CONTROL_PORT_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream2Controller->serverControlPort = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_MAX_PACKET_SIZE_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_MAX_PACKET_SIZE_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream2Controller->maxPacketSize = json_object_get_int(valueJsonObj);
        }
        
        // get ARDISCOVERY_CONNECTION_JSON_QOS_MODE_KEY
        res = json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_QOS_MODE_KEY, &valueJsonObj);
        if (res && valueJsonObj != NULL)
        {
            stream2Controller->qos_level = json_object_get_int(valueJsonObj);
        }
    }
    
    return error;
}

uint8_t ARCONTROLLER_Stream2_JsonContainsStream2Param(json_object *jsonObj)
{
    // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_STREAM_PORT_KEY
    json_object *serverStreamPortJsonObj = NULL;
    json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_STREAM_PORT_KEY, &serverStreamPortJsonObj);

    // get ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_CONTROL_PORT_KEY
    json_object *serverControlPortJsonObj = NULL;
    json_object_object_get_ex (jsonObj, ARDISCOVERY_CONNECTION_JSON_ARSTREAM2_SERVER_CONTROL_PORT_KEY, &serverControlPortJsonObj);

    return ((serverStreamPortJsonObj != NULL) && (serverControlPortJsonObj != NULL));
}

int ARCONTROLLER_Stream2_IsInitilized (ARCONTROLLER_Stream2_t *stream2Controller)
{
    // Checks if the stream 2 manager is initialized.
    return ((stream2Controller->serverStreamPort != 0) && (stream2Controller->serverControlPort != 0));
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_StartStream (ARCONTROLLER_Stream2_t *stream2Controller)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARSTREAM2_StreamReceiver_Config_t config;
    ARSTREAM2_StreamReceiver_NetConfig_t net_config;
    ARSTREAM2_StreamReceiver_MuxConfig_t mux_config;
    eARSTREAM2_ERROR stream2Error = ARSTREAM2_OK;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        memset(&config, 0, sizeof(ARSTREAM2_StreamReceiver_Config_t));
        memset(&net_config, 0, sizeof(ARSTREAM2_StreamReceiver_NetConfig_t));
        memset(&mux_config, 0, sizeof(ARSTREAM2_StreamReceiver_MuxConfig_t));

        config.maxPacketSize = stream2Controller->maxPacketSize;
        config.generateReceiverReports = 1;
        config.waitForSync = 1;
        config.outputIncompleteAu = 0;
        config.filterOutSpsPps = 1;
        config.filterOutSei = 1;
        config.replaceStartCodesWithNaluSize = stream2Controller->replaceStartCodesWithNaluSize;
        config.generateSkippedPSlices = 1;
        config.generateFirstGrayIFrame = 1;
        config.ardiscoveryProductType = stream2Controller->ardiscoveryProductType;
        config.canonicalName = "DEFAULT_SDK_CONTROLLER";

        if (stream2Controller->mux) {
            mux_config.mux = stream2Controller->mux;
            stream2Error = ARSTREAM2_StreamReceiver_Init(&(stream2Controller->streamReceiverHandle), &config, NULL, &mux_config);
        } else {
            net_config.serverAddr = stream2Controller->serverAddress; //TODO get from discovery device 
            net_config.mcastAddr = NULL;
            net_config.mcastIfaceAddr = NULL;
            net_config.serverStreamPort = stream2Controller->serverStreamPort;
            net_config.serverControlPort = stream2Controller->serverControlPort;
            net_config.clientStreamPort = stream2Controller->clientStreamPort;
            net_config.clientControlPort = stream2Controller->clientControlPort;

            if (stream2Controller->clientStreamFd >= 0) {
                ARSAL_Socket_Close(stream2Controller->clientStreamFd);
                stream2Controller->clientStreamFd = -1;
            }

            if (stream2Controller->clientControlFd >= 0) {
                ARSAL_Socket_Close(stream2Controller->clientControlFd);
                stream2Controller->clientControlFd = -1;
            }

            if (stream2Controller->qos_level == 1)
            {
                net_config.classSelector = ARSAL_SOCKET_CLASS_SELECTOR_CS4;
            }
            else
            {
                net_config.classSelector = ARSAL_SOCKET_CLASS_SELECTOR_UNSPECIFIED;
            }
            stream2Error = ARSTREAM2_StreamReceiver_Init(&(stream2Controller->streamReceiverHandle), &config, &net_config, NULL);
        }

        if (stream2Error != ARSTREAM2_OK)
        {
            error = ARCONTROLLER_ERROR_INIT_STREAM;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "Error ARSTREAM2_StreamReceiver_Init : %d", stream2Error);
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Thread_Create(&(stream2Controller->networkThread), ARSTREAM2_StreamReceiver_RunNetworkThread, stream2Controller->streamReceiverHandle) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "Creation of network thread failed.");
            error = ARCONTROLLER_ERROR_INIT_THREAD;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        if (ARSAL_Thread_Create(&(stream2Controller->appOutputThread), ARSTREAM2_StreamReceiver_RunAppOutputThread, stream2Controller->streamReceiverHandle) != 0)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "Creation of app output thread failed.");
            error = ARCONTROLLER_ERROR_INIT_THREAD;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        stream2Error = ARSTREAM2_StreamReceiver_StartAppOutput (stream2Controller->streamReceiverHandle, ARCONTROLLER_Stream2_SpsPpsCallback, stream2Controller, ARCONTROLLER_Stream2_GetAuBufferCallback, stream2Controller, ARCONTROLLER_Stream2_AuReadyCallback, stream2Controller);
        
        if (stream2Error != ARSTREAM2_OK)
        {
            error = ARCONTROLLER_ERROR_INIT_STREAM;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "Error ARSTREAM2_StreamReceiver_StartAppOutput : %d", stream2Error);
        }
    }
    
    return error;
}


static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_StopStream (ARCONTROLLER_Stream2_t *stream2Controller)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        eARSTREAM2_ERROR stream2Error = ARSTREAM2_StreamReceiver_Stop(stream2Controller->streamReceiverHandle);
        
        if (stream2Controller->networkThread != NULL)
        {
            ARSAL_Thread_Join(stream2Controller->networkThread, NULL);
            ARSAL_Thread_Destroy(&(stream2Controller->networkThread));
            stream2Controller->networkThread = NULL;
        }
        
        if (stream2Controller->appOutputThread != NULL)
        {
            ARSAL_Thread_Join(stream2Controller->appOutputThread, NULL);
            ARSAL_Thread_Destroy(&(stream2Controller->appOutputThread));
            stream2Controller->appOutputThread = NULL;
        }
        
        stream2Error = ARSTREAM2_StreamReceiver_Free(&(stream2Controller->streamReceiverHandle));
        
    }

    return error;
}

static eARCONTROLLER_ERROR ARCONTROLLER_Stream2_RestartStream (ARCONTROLLER_Stream2_t *stream2Controller)
{
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
        
    // Check parameters
    if (stream2Controller == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Stream2_StopStream (stream2Controller);
    }
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Stream2_StartStream (stream2Controller);
    }
    
    return error;
}

eARSTREAM2_ERROR ARCONTROLLER_Stream2_SpsPpsCallback(uint8_t *spsBuffer, int spsSize, uint8_t *ppsBuffer, int ppsSize, void *userPtr)
{
    ARCONTROLLER_Stream2_t *stream2Controller = (ARCONTROLLER_Stream2_t *)userPtr;
    
    
    if(stream2Controller->decoderConfigCallback != NULL)
    {
        ARCONTROLLER_Stream_Codec_t codec;
        codec.type = ARCONTROLLER_STREAM_CODEC_TYPE_H264;
        codec.parameters.h264parameters.spsBuffer = spsBuffer;
        codec.parameters.h264parameters.spsSize = spsSize;
        codec.parameters.h264parameters.ppsBuffer = ppsBuffer;
        codec.parameters.h264parameters.ppsSize = ppsSize;
        codec.parameters.h264parameters.isMP4Compliant = stream2Controller->replaceStartCodesWithNaluSize;
        
        stream2Controller->decoderConfigCallback(codec, stream2Controller->callbackData);
    }
    
    return ARSTREAM2_OK;
}

static eARSTREAM2_ERROR ARCONTROLLER_Stream2_GetAuBufferCallback(uint8_t **auBuffer, int *auBufferSize, void **auBufferUserPtr, void *userPtr)
{
    ARCONTROLLER_Stream2_t *stream2Controller = (ARCONTROLLER_Stream2_t *)userPtr;
    eARSTREAM2_ERROR retVal = ARSTREAM2_OK;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = ARCONTROLLER_StreamPool_GetNextFreeFrame (stream2Controller->framePool, &error);
    
    if (error == ARCONTROLLER_OK)
    {
        *auBuffer = frame->data;
        *auBufferSize = frame->capacity;
        *auBufferUserPtr = frame;
        
        frame->available = 0;
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "ARCONTROLLER_Stream2_GetAuBuffer ERROR NO BUFFER available");
        retVal = ARSTREAM2_ERROR_RESOURCE_UNAVAILABLE;
    }
    
    return retVal;
}

static eARSTREAM2_ERROR ARCONTROLLER_Stream2_AuReadyCallback(uint8_t *auBuffer, int auSize, ARSTREAM2_StreamReceiver_AuReadyCallbackTimestamps_t *auTimestamps, eARSTREAM2_STREAM_RECEIVER_AU_SYNC_TYPE auSyncType, ARSTREAM2_StreamReceiver_AuReadyCallbackMetadata_t *auMetadata, void *auBufferUserPtr, void *userPtr)
{
    ARCONTROLLER_Stream2_t *stream2Controller = (ARCONTROLLER_Stream2_t *)userPtr;
    ARCONTROLLER_Frame_t *frame = (ARCONTROLLER_Frame_t *) auBufferUserPtr;
    eARSTREAM2_ERROR retVal = ARSTREAM2_OK;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARSAL_Thread_t restartThread = NULL;
    
    //callback
    if ((frame != NULL) && (stream2Controller->receiveFrameCallback != NULL))
    {
        //set frame size
        frame->used = auSize;

        //set frame type
        frame->isIFrame = ((auSyncType == ARSTREAM2_STREAM_RECEIVER_AU_SYNC_TYPE_IDR) || (auSyncType == ARSTREAM2_STREAM_RECEIVER_AU_SYNC_TYPE_IFRAME)) ? 1 : 0;

        //set timestamp
        frame->timestamp = auTimestamps->auNtpTimestampRaw;

        //set metadata
        frame->metadata = auMetadata->auMetadata;
        frame->metadataSize = auMetadata->auMetadataSize;

        error = stream2Controller->receiveFrameCallback(frame, stream2Controller->callbackData);
        
        //Manage Error
        if (error != ARCONTROLLER_OK)
        {
            if ((error == ARCONTROLLER_ERROR_STREAM_RESYNC_REQUIRED) && (stream2Controller->errorCount < ARCONTROLLER_STREAM2_MAX_RESYNC_ERROR))
            {
                stream2Controller->errorCount++;
                retVal = ARSTREAM2_ERROR_RESYNC_REQUIRED;
            }
            else
            {
                // Restart stream2
                if (ARSAL_Thread_Create (&restartThread, ARCONTROLLER_Stream2_RestartRun, stream2Controller) != 0)
                {
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_STREAM2_TAG, "Creation of restart thread failed.");
                }
                else
                {
                    ARSAL_Thread_Destroy (&restartThread);
                    restartThread = NULL;
                }
            }
        }
        else
        {
            stream2Controller->errorCount = 0;
        }
        
        //Free the current frame
        ARCONTROLLER_Frame_SetFree(frame);
    }

    return retVal;
}

static void *ARCONTROLLER_Stream2_RestartRun (void *data)
{
    // -- Thread Run of re-start --
    
    // Local declarations
    ARCONTROLLER_Stream2_t *stream2Controller = (ARCONTROLLER_Stream2_t *)data;
    
    ARCONTROLLER_Stream2_RestartStream (stream2Controller);
    
    return NULL;
}
