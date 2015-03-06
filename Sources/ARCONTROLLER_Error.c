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
/*
 * GENERATED FILE
 *  Do not modify this file, it will be erased during the next configure run 
 */

/**
 * @file ARCONTROLLER_Error.c
 * @brief ToString function for eARCONTROLLER_ERROR enum
 */

#include <libARController/ARCONTROLLER_Error.h>

char *ARCONTROLLER_Error_ToString (eARCONTROLLER_ERROR error)
{
    switch (error)
    {
    case ARCONTROLLER_OK:
        return "No error";
        break;
    case ARCONTROLLER_ERROR:
        return "Unknown generic error";
        break;
    case ARCONTROLLER_ERROR_ALLOC:
        return "Memory allocation error";
        break;
    case ARCONTROLLER_ERROR_BAD_PARAMETER:
        return "Bad parameters";
        break;
    case ARCONTROLLER_ERROR_MUTEX:
        return "Mutex lock or unlock error";
        break;
    case ARCONTROLLER_ERROR_INIT_MUTEX:
        return "Mutex initialization error";
        break;
    case ARCONTROLLER_ERROR_STATE:
        return "Bad state of the Network Controller to call this function";
        break;
    case ARCONTROLLER_ERROR_INIT_THREAD:
        return "Thread initialization error";
        break;
    case ARCONTROLLER_ERROR_INIT_NETWORKAL:
        return "Error during the getting of the ARNetWorkAL from the device";
        break;
    case ARCONTROLLER_ERROR_INIT_NETWORK_CONFIG:
        return "Error during the getting of the ARNetWork Configuration from the device";
        break;
    case ARCONTROLLER_ERROR_NOT_SENT:
        return "Error data not sent";
        break;
    default:
        return "Unknown value";
        break;
    }
    return "Unknown value";
}
