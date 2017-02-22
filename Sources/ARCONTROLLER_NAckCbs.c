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
	uint8_t cmd_version;
	uint8_t used;               /**< Orientation has been set by user. */
};

/* ARDrone3 CameraOrientation */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	return ARCONTROLLER_OK;
}

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	return ARCONTROLLER_OK;
}

void ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationParameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters->data == NULL))
		return;

	data = feature->privatePart->CameraOrientationV2Parameters->data;
	data->sending_count = 0;
	data->used = 1;

	if (data->cmd_version > 1) {
		/* Copy values in command CameraOrientation v2 */
		feature->privatePart->CameraOrientationV2Parameters->tilt = (float)
				feature->privatePart->CameraOrientationParameters->tilt;
		feature->privatePart->CameraOrientationV2Parameters->pan = (float)
				feature->privatePart->CameraOrientationParameters->pan;
	}
}

uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters->data == NULL))
		return 0;

	data = feature->privatePart->CameraOrientationV2Parameters->data;

	if (data->cmd_version != 1)
		return 0;

	if ((data->used) && (data->sending_count < 10)) {
		data->sending_count++;
		return 1;
	}

	return 0;
}

/* ARDrone3 CameraOrientationV2 */
static void cameraStateDefaultOrientationCb (eARCONTROLLER_DICTIONARY_KEY commandKey,
		ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
{
	struct ARDrone3CameraOrientationData *data = customData;

	if (data == NULL)
		return;

	switch(commandKey) {
	case ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATION:
		if (data->cmd_version < 1)
			data->cmd_version = 1;
		break;
	case ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATIONV2:
		if (data->cmd_version < 2)
			data->cmd_version = 2;
		break;
	default:
		break;
	}
}

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2Init(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;
	eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = calloc(1, sizeof(*data));
	if (data == NULL)
		return ARCONTROLLER_ERROR_ALLOC;

	feature->privatePart->CameraOrientationV2Parameters->data = data;

	/* Set DefaultCameraOrientation callbacks to know which orientation we should send */
	error = ARCONTROLLER_FEATURE_ARDrone3_AddCallback (feature,
			ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATION,
			&cameraStateDefaultOrientationCb, data);
	if (error != ARCONTROLLER_OK)
		return error;

	error = ARCONTROLLER_FEATURE_ARDrone3_AddCallback (feature,
			ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATIONV2,
			&cameraStateDefaultOrientationCb, data);
	if (error != ARCONTROLLER_OK)
		return error;

	return ARCONTROLLER_OK;
}
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2DeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = feature->privatePart->CameraOrientationV2Parameters->data;

	/* Remove CameraOrientation callbacks */
	ARCONTROLLER_FEATURE_ARDrone3_RemoveCallback (feature,
			ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATION,
			&cameraStateDefaultOrientationCb, data);
	ARCONTROLLER_FEATURE_ARDrone3_RemoveCallback (feature,
			ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATIONV2,
			&cameraStateDefaultOrientationCb, data);

	free(feature->privatePart->CameraOrientationV2Parameters->data);
	feature->privatePart->CameraOrientationV2Parameters->data = NULL;

	return ARCONTROLLER_OK;
}

void ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2Changed(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL) ||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationParameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters->data == NULL))
		return;

	data = feature->privatePart->CameraOrientationV2Parameters->data;
	data->sending_count = 0;
	data->used = 1;

	if (data->cmd_version < 2) {
		/* Copy values in command CameraOrientation v1 */
		feature->privatePart->CameraOrientationParameters->tilt = (int8_t)
				feature->privatePart->CameraOrientationV2Parameters->tilt;
		feature->privatePart->CameraOrientationParameters->pan = (int8_t)
				feature->privatePart->CameraOrientationV2Parameters->pan;
	}
}
uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2MustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraOrientationData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters == NULL) ||
			(feature->privatePart->CameraOrientationV2Parameters->data == NULL))
		return 0;

	data = feature->privatePart->CameraOrientationV2Parameters->data;

	if (data->cmd_version != 2)
		return 0;

	if ((data->used) && (data->sending_count < 10)) {
		data->sending_count++;
		return 1;
	}

	return 0;
}


/* ARDrone3 CameraVelocity */
struct ARDrone3CameraVelocityData {
	uint8_t val_is_null;
	uint32_t sending_count;
};

#define ARCONTROLLER_ARDRONE3_CAMERA_VELOCITY_SEND_MAX_COUNT 10

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraVelocityData *data = NULL;
	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraVelocityParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = calloc(1, sizeof(*data));
	if (data == NULL)
		return ARCONTROLLER_ERROR_ALLOC;

    data->val_is_null = 1;
    data->sending_count = ARCONTROLLER_ARDRONE3_CAMERA_VELOCITY_SEND_MAX_COUNT;
	feature->privatePart->CameraVelocityParameters->data = data;

	return ARCONTROLLER_OK;
}
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraVelocityParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	free(feature->privatePart->CameraVelocityParameters->data);
	feature->privatePart->CameraVelocityParameters->data = NULL;

	return ARCONTROLLER_OK;
}
void ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraVelocityData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraVelocityParameters == NULL) ||
			(feature->privatePart->CameraVelocityParameters->data == NULL))
		return;

	data = feature->privatePart->CameraVelocityParameters->data;
	if ((feature->privatePart->CameraVelocityParameters->tilt == 0) &&
			(feature->privatePart->CameraVelocityParameters->pan == 0)) {
		data->val_is_null = 1;
		data->sending_count = 0;
	} else {
		data->val_is_null = 0;
	}
}
uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	struct ARDrone3CameraVelocityData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->CameraVelocityParameters == NULL) ||
			(feature->privatePart->CameraVelocityParameters->data == NULL))
		return 0;

	data = feature->privatePart->CameraVelocityParameters->data;

	if (!data->val_is_null)
		return 1;

	if (data->sending_count < ARCONTROLLER_ARDRONE3_CAMERA_VELOCITY_SEND_MAX_COUNT) {
		data->sending_count++;
		return 1;
	}

	return 0;
}

/* ControllerInfo gps */
struct ControllerInfoGpsData {
	uint8_t val_is_new;
};

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoGpsInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoGpsData *data = NULL;
	if ((feature == NULL) ||
			(feature->privatePart == NULL) ||
			(feature->privatePart->GpsParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = calloc(1, sizeof(*data));
	if (data == NULL)
		return ARCONTROLLER_ERROR_ALLOC;

	feature->privatePart->GpsParameters->data = data;

	return ARCONTROLLER_OK;
}

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoGpsDeInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	if ((feature == NULL) ||
			(feature->privatePart == NULL) ||
			(feature->privatePart->GpsParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	free(feature->privatePart->GpsParameters->data);
	feature->privatePart->GpsParameters->data = NULL;

	return ARCONTROLLER_OK;
}

void ARCONTROLLER_NAckCbs_ControllerInfoGpsChanged(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoGpsData *data = NULL;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->GpsParameters == NULL) ||
			(feature->privatePart->GpsParameters->data == NULL))
		return;

	data = feature->privatePart->GpsParameters->data;
	data->val_is_new = 1;
}

uint8_t ARCONTROLLER_NAckCbs_ControllerInfoGpsMustBeSent(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoGpsData *data = NULL;
	int must_be_sent = 0;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->GpsParameters == NULL) ||
			(feature->privatePart->GpsParameters->data == NULL))
		return 0;

	data = feature->privatePart->GpsParameters->data;
	must_be_sent = data->val_is_new;

	data->val_is_new = 0;
	return must_be_sent;
}

/* ControllerInfo barometer */
struct ControllerInfoBarometerData {
	uint8_t val_is_new;
};

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoBarometerInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoBarometerData *data = NULL;
	if ((feature == NULL) ||
			(feature->privatePart == NULL) ||
			(feature->privatePart->BarometerParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	data = calloc(1, sizeof(*data));
	if (data == NULL)
		return ARCONTROLLER_ERROR_ALLOC;

	feature->privatePart->BarometerParameters->data = data;

	return ARCONTROLLER_OK;
}

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoBarometerDeInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	if ((feature == NULL) ||
			(feature->privatePart == NULL) ||
			(feature->privatePart->BarometerParameters == NULL))
		return ARCONTROLLER_ERROR_BAD_PARAMETER;

	free(feature->privatePart->BarometerParameters->data);
	feature->privatePart->BarometerParameters->data = NULL;

	return ARCONTROLLER_OK;
}

void ARCONTROLLER_NAckCbs_ControllerInfoBarometerChanged(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoBarometerData *data = NULL;
	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->BarometerParameters == NULL) ||
			(feature->privatePart->BarometerParameters->data == NULL))
		return;

	data = feature->privatePart->BarometerParameters->data;
	data->val_is_new = 1;
}

uint8_t ARCONTROLLER_NAckCbs_ControllerInfoBarometerMustBeSent(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature) {
	struct ControllerInfoBarometerData *data = NULL;
	int must_be_sent = 0;

	if ((feature == NULL)||
			(feature->privatePart == NULL) ||
			(feature->privatePart->BarometerParameters == NULL) ||
			(feature->privatePart->BarometerParameters->data == NULL))
		return 0;

	data = feature->privatePart->BarometerParameters->data;
	must_be_sent = data->val_is_new;

	data->val_is_new = 0;
	return must_be_sent;
}
