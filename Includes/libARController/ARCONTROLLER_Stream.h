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
 * @file ARNETWORK_Stream.h
 * @brief ARCONTROLLER_Stream allow to operate ARStream for receive a stream.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_STREAM_H_
#define _ARCONTROLLER_STREAM_H_

#include <json/json.h>
#include <libARSAL/ARSAL_Thread.h>
#include <libARController/ARCONTROLLER_Error.h>
#include <libARDiscovery/ARDISCOVERY_Error.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>

/**
 * @brief Stream controller allow to operate ARStream for receive a stream.
 */
typedef struct ARCONTROLLER_Stream_t ARCONTROLLER_Stream_t;

// TODO ADD !!!!!!!!!
ARCONTROLLER_Stream_t *ARCONTROLLER_Stream_New (ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error);

// TODO ADD !!!!!!!!!
void ARCONTROLLER_Stream_Delete (ARCONTROLLER_Stream_t **streamController);


// TODO ADD !!!!!!!!!
eARDISCOVERY_ERROR ARCONTROLLER_Stream_SendJsonCallback (json_object *jsonObj, void *customData);

// TODO ADD !!!!!!!!!
eARDISCOVERY_ERROR ARCONTROLLER_Stream_ReceiveJsonCallback (json_object *jsonObj, void *customData);

#endif /* _ARCONTROLLER_STREAM_H_ */
