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
 * @file ARCONTROLLER_Frame.c
 * @brief ARCONTROLLER_Frame
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>
#include <stdint.h>

#include <libARSAL/ARSAL_Print.h>
#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Frame.h>

#include "ARCONTROLLER_Frame.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/

ARCONTROLLER_Frame_t *ARCONTROLLER_Frame_New (eARCONTROLLER_ERROR *error)
{
    return ARCONTROLLER_Frame_NewWithCapacity (ARCONTROLLER_FRAME_DEFAULT_CAPACITY, error);
}

ARCONTROLLER_Frame_t *ARCONTROLLER_Frame_NewWithCapacity (uint32_t defaultCapacity, eARCONTROLLER_ERROR *error)
{
    // -- Create a New Frame --

    //Local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame =  NULL;
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the frame
        frame = malloc (sizeof (ARCONTROLLER_Frame_t));
        if (frame != NULL)
        {
            // Initialize to default values
            frame->data = NULL;
            frame->capacity = 0;
            frame->used = 0;
            frame->missed = 0;
            frame->width = 0;
            frame->height = 0;
            frame->timestamp = 0;
            frame->isIFrame = 0;
            frame->available = 1;
            frame->base = NULL;
            frame->metadata = NULL;
            frame->metadataSize = 0;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if ((localError == ARCONTROLLER_OK) && (defaultCapacity > 0))
    {
        //alloc data
        frame->base = malloc (sizeof (uint8_t) * defaultCapacity);
        if (frame->base != NULL)
        {
            frame->data = frame->base;
            frame->capacity = defaultCapacity;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: Error is not returned 

    return frame;
}

void ARCONTROLLER_Frame_Delete (ARCONTROLLER_Frame_t **frame)
{
    // -- Delete a Frame --

    if (frame != NULL)
    {
        if ((*frame) != NULL)
        {
            // Free data
            if ((*frame)->base != NULL)
            {
                free ((*frame)->base);
                (*frame)->base = NULL;
                (*frame)->data = NULL;
            }
            
            free (*frame);
            (*frame) = NULL;
        }
    }
}

int ARCONTROLLER_Frame_ensureCapacityIsAtLeast (ARCONTROLLER_Frame_t *frame, uint32_t minimumCapacity, eARCONTROLLER_ERROR *error)
{
    // -- Ensure Capacity Is At Least minimumCapacity --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    int res = 0;
    uint8_t *newData = NULL;
    
    // Check parameters
    if (frame == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        if (frame->capacity >= minimumCapacity)
        {
            res = 1;
        }
        else
        {
            // Realloc data
            newData = realloc (frame->base, minimumCapacity);
            if (newData != NULL)
            {
                frame->base = newData;
                frame->data = frame->base;
                frame->capacity = minimumCapacity;
                res = 1;
            }
            else
            {
                localError = ARCONTROLLER_ERROR_ALLOC;
            }
        }
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return res;
}

eARCONTROLLER_ERROR ARCONTROLLER_Frame_SetFree (ARCONTROLLER_Frame_t *frame)
{
    // -- Set a Frame as Free --

    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if (frame == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        frame->used = 0;
        frame->missed = 0;
        frame->width = 0;
        frame->height = 0;
        frame->timestamp = 0;
        frame->isIFrame = 0;
        frame->available = 1;
        frame->metadata = NULL;
        frame->metadataSize = 0;
    }

    return error;
}

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

