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
 * @file ARCONTROLLER_NAckCbs.c
 * @brief Callback for the algorithmic to send non acknowledged commands.
 */

#include <stdlib.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <ARCONTROLLER_Feature.h>
#include "ARCONTROLLER_NAckCbs.h"

/*************************
 * Private header
 *************************/

/*************************
 * Implementation
 *************************/

struct ARDrone3CameraOrientationData {
	uint32_t sending_count;
};

/* ARDrone3 CameraOrientation */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;
	if ((feature == NULL)||
		(feature->privatePart == NULL) ||
		(feature->privatePart->CameraOrientationParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = calloc(1, sizeof(*data));
	if (data == NULL)
		return ARCONTROLLER_ERROR_ALLOC;

	feature->privatePart->CameraOrientationParameters->data = data;

	return ARCONTROLLER_OK;
}

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	if ((feature == NULL)||
		(feature->privatePart == NULL) ||
		(feature->privatePart->CameraOrientationParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	free(feature->privatePart->CameraOrientationParameters->data);
	feature->privatePart->CameraOrientationParameters->data = NULL;

	return ARCONTROLLER_OK;
}

void ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
		(feature->privatePart == NULL) ||
		(feature->privatePart->CameraOrientationParameters == NULL) ||
		(feature->privatePart->CameraOrientationParameters->data == NULL))
		return;

	data = feature->privatePart->CameraOrientationParameters->data;
	data->sending_count = 0;
}

uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
		(feature->privatePart == NULL) ||
		(feature->privatePart->CameraOrientationParameters == NULL) ||
		(feature->privatePart->CameraOrientationParameters->data == NULL))
		return 0;

	data = feature->privatePart->CameraOrientationParameters->data;

	if (data->sending_count < 10) {
		data->sending_count++;
		return 1;
	}

	return 0;
}
