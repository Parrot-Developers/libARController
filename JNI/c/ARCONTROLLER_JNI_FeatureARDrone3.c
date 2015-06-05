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
 * @file ARCONTROLLER_JNI_FeatureARDrone3.c
 * @brief libARController JNI feature ARDrone3 c file.
 **/

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <jni.h>
#include <stdlib.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Feature.h>

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define ARCONTROLLER_JNIFEATUREARDRONE3_TAG "JNIFeatureARDrone3"

/*****************************************
 *
 *             private header:
 *
 *****************************************/


/*****************************************
 *
 *             implementation :
 *
 *****************************************/

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARFeatureARDrone3_nativeSendCameraOrientation (JNIEnv *env, jobject thizz, jlong jFeatureARDrone3, jint tilt, jint pan)
{
    // local declarations
    ARCONTROLLER_FEATURE_ARDrone3_t *nativeFeatureARDrone3 = (ARCONTROLLER_FEATURE_ARDrone3_t*) (intptr_t) jFeatureARDrone3;
    
    return nativeFeatureARDrone3->sendCameraOrientation (nativeFeatureARDrone3, (int8_t) tilt, (int8_t) pan);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARFeatureARDrone3_nativeSendPilotingTakeOff (JNIEnv *env, jobject thizz, jlong jFeatureARDrone3)
{
    // local declarations
    ARCONTROLLER_FEATURE_ARDrone3_t *nativeFeatureARDrone3 = (ARCONTROLLER_FEATURE_ARDrone3_t*) (intptr_t) jFeatureARDrone3;
    
    return nativeFeatureARDrone3->sendPilotingTakeOff (nativeFeatureARDrone3);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARFeatureARDrone3_nativeSendPilotingLanding (JNIEnv *env, jobject thizz, jlong jFeatureARDrone3)
{
    // local declarations
    ARCONTROLLER_FEATURE_ARDrone3_t *nativeFeatureARDrone3 = (ARCONTROLLER_FEATURE_ARDrone3_t*) (intptr_t) jFeatureARDrone3;
    
    return nativeFeatureARDrone3->sendPilotingLanding (nativeFeatureARDrone3);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARFeatureARDrone3_nativeSendPilotingEmergency(JNIEnv *env, jobject thizz, jlong jFeatureARDrone3)
{
    // local declarations
    ARCONTROLLER_FEATURE_ARDrone3_t *nativeFeatureARDrone3 = (ARCONTROLLER_FEATURE_ARDrone3_t*) (intptr_t) jFeatureARDrone3;
    
    return nativeFeatureARDrone3->sendPilotingEmergency (nativeFeatureARDrone3);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARFeatureARDrone3_nativeSendMediaStreamingVideoEnable (JNIEnv *env, jobject thizz, jlong jFeatureARDrone3, jint enable)
{
    // local declarations
    ARCONTROLLER_FEATURE_ARDrone3_t *nativeFeatureARDrone3 = (ARCONTROLLER_FEATURE_ARDrone3_t*) (intptr_t) jFeatureARDrone3;
    
    return nativeFeatureARDrone3->sendMediaStreamingVideoEnable (nativeFeatureARDrone3, (uint8_t)enable);
}

/*****************************************
 *
 *             private implementation:
 *
 *****************************************/

