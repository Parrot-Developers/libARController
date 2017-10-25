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
 * @file ARNETWORK_Network.h
 * @brief ARCONTROLLER_Network allow to operate an ARNETWORK_Manager for send and receive commands.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_NETWORK_PRIVATE_H_
#define _ARCONTROLLER_NETWORK_PRIVATE_H_

#include <libARSAL/ARSAL_Socket.h>
#include <libARSAL/ARSAL_Mutex.h>
#include <libARCommands/ARCOMMANDS_Decoder.h>

#include "ARCONTROLLER_StreamSender.h"

#define ARCONTROLLER_NETWORK_TAG "ARCONTROLLER_Network"
#define ARCONTROLLER_NETWORK_READING_TIMEOUT_MS 1000

typedef struct ARCONTROLLER_NETWORK_THREAD_DATA_t ARCONTROLLER_NETWORK_THREAD_DATA_t;

/**
 * @brief Network controller allow to operate an ARNETWORK_Manager for send and receive commands.
 */
struct ARCONTROLLER_Network_t
{
    ARCOMMANDS_Decoder_t *decoder;
    ARDISCOVERY_Device_t *discoveryDevice; /**< The device to drive */
    ARDISCOVERY_NetworkConfiguration_t networkConfig; /**< The network configuration ; get from the device */
    ARNETWORKAL_Manager_t *networkALManager; /**< The networkALManager ; get from the device  */
    ARNETWORK_Manager_t *networkManager; /**< The networkManager */
    ARSAL_Thread_t rxThread; /**< Receiver thread of the networkManager */
    ARSAL_Thread_t txThread; /**< Transmitter thread of the networkManager */
    ARSAL_Thread_t *readerThreads; /**< Reader threads for all buffers of receiving */
    ARCONTROLLER_NETWORK_THREAD_DATA_t *readerThreadsData; /**< Data for all reader threads*/
    ARSAL_Mutex_t mutex; /**< Mutex for multithreading */
    eARCONTROLLER_NETWORK_STATE state; /**< State of the networkController*/

    //video part
    int hasVideo; /**< 0 if the device has not Video stream ; otherwise 1 */
    ARCONTROLLER_Stream_t *videoController; /**< Manage input video stream  */

    //audio part
    int hasAudio; /**< 0 if the device has not Audio stream ; otherwise 1 */
    ARCONTROLLER_Stream_t *audioController; /**< Manage input audio stream  */

    //output audio part
    int hasOutputAudio; /**< 0 if the device has not output Audio stream ; otherwise 1 */
    ARCONTROLLER_StreamSender_t *audioOutputController; /**< Manage output audio stream  */

    //Connection callback
    ARDISCOVERY_Device_ConnectionJsonCallback_t sendJsonCallback; /**< Callback of json sending part of the connection.*/
    ARDISCOVERY_Device_ConnectionJsonCallback_t receiveJsonCallback; /**< Callback of json reception part of the connection.*/
    ARCONTROLLER_Network_DisconnectionCallback_t disconnectionCallback; /**< Callback of disconnection */
    void *callbacksCustomData; /**< data to set as argument to the callbacks */
};

struct ARCONTROLLER_NETWORK_THREAD_DATA_t
{
    ARCONTROLLER_Network_t *networkController; /**< Network Controller owning the reader thread */
    int readerBufferId; /**< ID to read by the reader thread */
};

/**
 * @brief Create the Network receiver and transmitter Threads //TODO !!!!!!!!!!!!!!!!!
 */
eARCONTROLLER_ERROR ARCONTROLLER_Network_CreateNetworkThreads (ARCONTROLLER_Network_t *networkController);

/**
 * @brief Stop the Network receiver and transmitter Threads //TODO !!!!!!!!!!!!!!!!!
 */
eARCONTROLLER_ERROR ARCONTROLLER_Network_StopNetworkThreads (ARCONTROLLER_Network_t *networkController);

/**
 * @brief Create the reader Threads //TODO !!!!!!!!!!!!!!!!!
 */
eARCONTROLLER_ERROR ARCONTROLLER_Network_CreateReaderThreads (ARCONTROLLER_Network_t *networkController);

/**
 * @brief Stop the reader Threads //TODO !!!!!!!!!!!!!!!!!
 */
eARCONTROLLER_ERROR ARCONTROLLER_Network_StopReaderThreads (ARCONTROLLER_Network_t *networkController);

/**
 * @brief Read and decode one input network buffer
 * @warning This function must be called in its own thread.
 * @post Before join the thread calling this function, ARCONTROLLER_Network_Stop() must be called.
 * @param data thread data of type ARCONTROLLER_NETWORK_THREAD_DATA_t*
 * @return NULL
 * @see ARCONTROLLER_Network_Stop()
 */
void *ARCONTROLLER_Network_ReaderRun (void *data);

//TODO !!!!!!!!!!!!!!!!!
void ARCONTROLLER_Network_OnDisconnectNetwork (ARNETWORK_Manager_t *manager, ARNETWORKAL_Manager_t *alManager, void *customData);

//TODO !!!!!!!!!!!!!!!!!
eARNETWORK_MANAGER_CALLBACK_RETURN ARCONTROLLER_Network_SendingCallback(int buffer_id, uint8_t *data, void *custom, eARNETWORK_MANAGER_CALLBACK_STATUS cause);

//TODO !!!!!!!!!!!!!!!!!
eARDISCOVERY_ERROR ARCONTROLLER_Network_OnSendJson (json_object *jsonObj, void *customData);

//TODO !!!!!!!!!!!!!!!!!
eARDISCOVERY_ERROR ARCONTROLLER_Network_OnReceiveJson (json_object *jsonObj, void *customData);

#endif /* _ARCONTROLLER_NETWORK_PRIVATE_H_ */
