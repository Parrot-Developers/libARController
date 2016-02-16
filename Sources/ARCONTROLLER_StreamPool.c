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
 * @file ARCONTROLLER_StreamPool.c
 * @brief ARCONTROLLER_StreamPool 
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>
#include <stdint.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Frame.h>
#include <libARController/ARCONTROLLER_StreamPool.h>

#include "ARCONTROLLER_StreamPool.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/

ARCONTROLLER_StreamPool_t *ARCONTROLLER_StreamPool_New (uint32_t capacity, eARCONTROLLER_ERROR *error)
{
    // -- Create a new streamPool --

    // Local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_StreamPool_t *streamPool =  NULL;
    size_t index = 0;
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the StreamPool
        streamPool = malloc (sizeof (ARCONTROLLER_StreamPool_t));
        if (streamPool != NULL)
        {
            // Initialize to default values
            streamPool->frames = NULL;
            streamPool->capacity = 0;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    if ((localError == ARCONTROLLER_OK) && (capacity > 0))
    {
        //Allocate the frame array
        streamPool->frames = calloc (sizeof (ARCONTROLLER_Frame_t*), capacity);
        if (streamPool->frames != NULL)
        {
            streamPool->capacity = capacity;
            for (index = 0 ; ((index < capacity) && (localError == ARCONTROLLER_OK)); index++)
            {
                streamPool->frames[index] = ARCONTROLLER_Frame_New (&localError);
            }
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    // Delete if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_StreamPool_Delete (&streamPool);
    }
    // No else: Skipped by an error
    
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: Error is not returned 

    return streamPool;
}

void ARCONTROLLER_StreamPool_Delete (ARCONTROLLER_StreamPool_t **streamPool)
{
    // -- Delete a streamPool --
    size_t index = 0;

    if (streamPool != NULL)
    {
        if ((*streamPool) != NULL)
        {
            // Free frames
            if ((*streamPool)->frames != NULL)
            {
                for (index = 0 ; index < (*streamPool)->capacity ; index++)
                {
                     ARCONTROLLER_Frame_Delete (&((*streamPool)->frames[index]));
                }
                
                free ((*streamPool)->frames);
                (*streamPool)->frames = NULL;
            }
            
            free (*streamPool);
            (*streamPool) = NULL;
        }
    }
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamPool_GetNextFreeFrame (ARCONTROLLER_StreamPool_t *streamPool, eARCONTROLLER_ERROR *error)
{
    // -- Get the next free frame --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *freeFrame = NULL;
    size_t index = 0;
    
    // Check parameters
    if (streamPool == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: The checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing.
    
    if (localError == ARCONTROLLER_OK)
    {
        for (index = 0 ; index < streamPool->capacity ; index++)
        {
            if (streamPool->frames[index]->available)
            {
                streamPool->frames[index]->available = 0;
                freeFrame = streamPool->frames[index];
                break; // The first free frame is found ; break the loop.
            }
        }
       
        if (freeFrame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMPOOL_FRAME_NOT_FOUND;
        }
    }
    
    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: Rrror is not returned 
    
    return freeFrame;
}

ARCONTROLLER_Frame_t *ARCONTROLLER_StreamPool_GetFrameFromData (ARCONTROLLER_StreamPool_t *streamPool, uint8_t *frameData, eARCONTROLLER_ERROR *error)
{
    // -- Get frame from its pointer --

    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Frame_t *frame = NULL;
    size_t index = 0;
    
    // Check parameters
    if (streamPool == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: The checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing.
    
    if (localError == ARCONTROLLER_OK)
    {
        for (index = 0 ; index < streamPool->capacity ; index++)
        {
            if (streamPool->frames[index]->data == frameData)
            {
                frame = streamPool->frames[index];
                break; // The frame is found ; break the loop
            }
        }
       
        if (frame == NULL)
        {
            localError = ARCONTROLLER_ERROR_STREAMPOOL_FRAME_NOT_FOUND;
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

/*****************************************
 *
 *             local implementation:
 *
 ****************************************/

