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
 * @file ARCONTROLLER_Frame.h
 * @brief ARCONTROLLER_Frame
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_FRAME_H_
#define _ARCONTROLLER_FRAME_H_

#include <stdint.h>

/**
 * @brief 
 */
typedef struct 
{
    uint8_t *data;
    uint32_t capacity;
    uint32_t used;
    uint32_t missed;
    uint32_t width;
    uint32_t height;
    int isIFrame;
    int available;
}ARCONTROLLER_Frame_t;

ARCONTROLLER_Frame_t *ARCONTROLLER_Frame_New (eARCONTROLLER_ERROR *error);

ARCONTROLLER_Frame_t *ARCONTROLLER_Frame_NewWithCapacity (uint32_t defaultCapacity, eARCONTROLLER_ERROR *error);

void ARCONTROLLER_Frame_Delete (ARCONTROLLER_Frame_t **frame);

int ARCONTROLLER_Frame_ensureCapacityIsAtLeast (ARCONTROLLER_Frame_t *frame, uint32_t minimumCapacity, eARCONTROLLER_ERROR *error);

eARCONTROLLER_ERROR ARCONTROLLER_Frame_SetFree (ARCONTROLLER_Frame_t *frame);

#endif /* _ARCONTROLLER_FRAME_H_ */
