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
 * @file ARCONTROLLER_NAckCbs.h
 * @brief Callback for the algorithmic to send non acknowledged commands.
 */
#ifndef _ARCONTROLLER_NACK_CBS_PRIVATE_H_
#define _ARCONTROLLER_NACK_CBS_PRIVATE_H_

#include <ARCONTROLLER_Feature.h>

/* ARDrone3 PilotingPCMD */
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3PilotingPCMDInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3PilotingPCMDDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
void ARCONTROLLER_NAckCbs_ARDrone3PilotingPCMDChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
};
static inline
uint8_t ARCONTROLLER_NAckCbs_ARDrone3PilotingPCMDMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature)
{
	return 1;
};

/* ARDrone3 CameraOrientation */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

void ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

/* JumpingSumo PilotingPCMD */
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_JumpingSumoPilotingPCMDInit(
		ARCONTROLLER_FEATURE_JumpingSumo_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_JumpingSumoPilotingPCMDDeInit(
		ARCONTROLLER_FEATURE_JumpingSumo_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
void ARCONTROLLER_NAckCbs_JumpingSumoPilotingPCMDChanged(
		ARCONTROLLER_FEATURE_JumpingSumo_t *feature)
{
};
static inline
uint8_t ARCONTROLLER_NAckCbs_JumpingSumoPilotingPCMDMustBeSent(
		ARCONTROLLER_FEATURE_JumpingSumo_t *feature)
{
	return 1;
};

/* MiniDrone PilotingPCMD */
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_MiniDronePilotingPCMDInit(
		ARCONTROLLER_FEATURE_MiniDrone_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_MiniDronePilotingPCMDDeInit(
		ARCONTROLLER_FEATURE_MiniDrone_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
void ARCONTROLLER_NAckCbs_MiniDronePilotingPCMDChanged(
		ARCONTROLLER_FEATURE_MiniDrone_t *feature)
{
};
static inline
uint8_t ARCONTROLLER_NAckCbs_MiniDronePilotingPCMDMustBeSent(
		ARCONTROLLER_FEATURE_MiniDrone_t *feature)
{
	return 1;
};

/* Powerup PilotingPCMD */
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_PowerupPilotingPCMDInit(
		ARCONTROLLER_FEATURE_Powerup_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_PowerupPilotingPCMDDeInit(
		ARCONTROLLER_FEATURE_Powerup_t *feature)
{
	return ARCONTROLLER_OK;
};
static inline
void ARCONTROLLER_NAckCbs_PowerupPilotingPCMDChanged(
		ARCONTROLLER_FEATURE_Powerup_t *feature)
{
};
static inline
uint8_t ARCONTROLLER_NAckCbs_PowerupPilotingPCMDMustBeSent(
		ARCONTROLLER_FEATURE_Powerup_t *feature)
{
	return 1;
};

/* ARDrone3 CameraVelocity */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityDeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
void ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityChanged(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraVelocityMustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

/* ARDrone3 CameraOrientationV2 */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2Init(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2DeInit(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
void ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2Changed(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);
uint8_t ARCONTROLLER_NAckCbs_ARDrone3CameraOrientationV2MustBeSent(
		ARCONTROLLER_FEATURE_ARDrone3_t *feature);

/* ControllerInfo gps */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoGpsInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoGpsDeInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
void ARCONTROLLER_NAckCbs_ControllerInfoGpsChanged(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
uint8_t ARCONTROLLER_NAckCbs_ControllerInfoGpsMustBeSent(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);

/* ControllerInfo barometer */
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoBarometerInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
eARCONTROLLER_ERROR ARCONTROLLER_NAckCbs_ControllerInfoBarometerDeInit(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
void ARCONTROLLER_NAckCbs_ControllerInfoBarometerChanged(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);
uint8_t ARCONTROLLER_NAckCbs_ControllerInfoBarometerMustBeSent(
		ARCONTROLLER_FEATURE_ControllerInfo_t *feature);

#endif /* _ARCONTROLLER_NACK_CBS_PRIVATE_H_ */
