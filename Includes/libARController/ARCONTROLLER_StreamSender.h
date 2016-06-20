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
 * @file ARCONTROLLER_StreamSender.h
 * @brief ARCONTROLLER_StreamSender allow to operate ARStream for sender a stream.
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM_SENDER_H_
#define _ARCONTROLLER_STREAM_SENDER_H_

#include <libARStream/ARStream.h>
#include <libARNetwork/ARNetwork.h>
#include <libARDiscovery/ARDiscovery.h>
#include <libARController/ARCONTROLLER_Error.h>

/**
 * @brief Stream controller allow to operate ARStream for send a stream.
 */
typedef struct ARCONTROLLER_StreamSender_t ARCONTROLLER_StreamSender_t;

/**
 * @brief Create a new audio output Stream Controller.
 * @warning This function allocate memory.
 * @post ARCONTROLLER_Stream_Delete() must be called to delete the Stream Controller and free the memory allocated.
 * @param[in] networkConfiguration The configuration of the network carrying the stream.
 * @param[out] error Executing error.
 * @return The new Stream Controller.
 * @see ARCONTROLLER_StreamSender_Delete.
 */
ARCONTROLLER_StreamSender_t *ARCONTROLLER_StreamSender_New (ARDISCOVERY_NetworkConfiguration_t *networkConfiguration, eARCONTROLLER_ERROR *error);

/**
 * @brief Delete the audio output Stream Controller.
 * @warning This function free memory.
 * @param streamController The Stream Controller to delete.
 * @see ARCONTROLLER_StreamSender_New().
 */
void ARCONTROLLER_StreamSender_Delete (ARCONTROLLER_StreamSender_t **streamController);

/**
 * @brief Start the audio output Stream Controller.
 * @post ARCONTROLLER_Stream_Stop() must be called to stop the Stream Controller.
 * @param streamController The stream controller.
 * @return Executing error.
 * @see ARCONTROLLER_StreamSender_Stop.
 */
eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_Start (ARCONTROLLER_StreamSender_t *streamController, ARNETWORK_Manager_t *networkManager);

/**
 * @brief Stop the audio output Stream Controller.
 * @param streamController The stream controller.
 * @return Executing error.
 * @see ARCONTROLLER_StreamSender_Start.
 */
eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_Stop (ARCONTROLLER_StreamSender_t *streamController);

/**
 * @brief Send audio stream frame through the network.
 * @param streamController The stream controller. ; must be not NULL.
 * @param[in] data The data to send.
 * @param[in] dataSize The data size.
 * @return Executing error.
 */
eARCONTROLLER_ERROR ARCONTROLLER_StreamSender_SendAudioFrame(ARCONTROLLER_StreamSender_t *streamController, uint8_t *data, int dataSize);

/**
 * @brief Callback to read a json part durring the connection.
 * @param streamController The stream controller.
 * @param jsonObj connection json.
 * @return Executing error.
 */
eARDISCOVERY_ERROR ARCONTROLLER_StreamSender_OnReceiveJson (ARCONTROLLER_StreamSender_t *streamController, json_object *jsonObj);

#endif /* _ARCONTROLLER_STREAM_SENDER_H_ */
