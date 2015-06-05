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
 * @file ARCONTROLLER_JNI_Device.c
 * @brief libARController JNI device c file.
 **/

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <jni.h>
#include <stdlib.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARDiscovery/ARDISCOVERY_Device.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Device.h>

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define ARCONTROLLER_JNIDEVICE_TAG "JNIControllerDevice"

/**
 * @brief 
 */
typedef struct
{
    ARCONTROLLER_Device_t *nativeDeviceController; /**< native Device Controller*/
    jobject jDeviceController; /**< java DeviceController*/
}ARCONTROLLER_JNIDeviceController_t;

static JavaVM *ARCONTROLLER_JNIDEVICE_VM; /**< reference to the java virtual machine */

static jmethodID ARCONTROLLER_JNIDEVICE_METHOD_ON_STATE_CHANGED;
static jmethodID ARCONTROLLER_JNIDEVICE_METHOD_ON_COMMAND_RECEIVED;
static jmethodID ARCONTROLLER_JNIDEVICE_METHOD_DID_RECEIVED_FRAME_CALLBACK;
static jmethodID ARCONTROLLER_JNIDEVICE_METHOD_TIMEOUT_FRAME_CALLBACK;

/*****************************************
 *
 *             private header:
 *
 *****************************************/
ARCONTROLLER_JNIDeviceController_t *ARCONTROLLER_JNI_Device_NewJNIDeviceController (JNIEnv *env, jobject thizz, ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error);
void ARCONTROLLER_JNI_Device_DeleteJNIDeviceController (JNIEnv *env, ARCONTROLLER_JNIDeviceController_t **jniDeviceController);

void ARCONTROLLER_JNI_Device_StateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData);
void ARCONTROLLER_JNI_Device_CommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData);

void ARCONTROLLER_JNI_Device_DidReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData);
void ARCONTROLLER_JNI_Device_TimeoutFrameCallback (void *customData);

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

static JavaVM* ARCONTROLLER_JNIDEVICE_VM = NULL; /** reference to the java virtual machine */

/**
 * @brief save the reference to the java virtual machine
 * @note this function is automatically call on the JNI startup
 * @param[in] VM reference to the java virtual machine
 * @param[in] reserved data reserved
 * @return JNI version
 */
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *VM, void *reserved)
{
    ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARCONTROLLER_JNIDEVICE_TAG, "Library has been loaded");

    /* Saving the reference to the java virtual machine */
    ARCONTROLLER_JNIDEVICE_VM = VM;

    /* Return the JNI version */
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeStaticInit (JNIEnv *env, jclass class)
{    
    // local declarations
    jclass jARDeviceControllerCls = NULL;
    
    // get ARDeviceController
    jARDeviceControllerCls = (*env)->FindClass(env, "com/parrot/arsdk/arcontroller/ARDeviceController");
    
    ARCONTROLLER_JNIDEVICE_METHOD_ON_STATE_CHANGED = (*env)->GetMethodID (env, jARDeviceControllerCls, "onStateChanged", "(II)V");
    ARCONTROLLER_JNIDEVICE_METHOD_DID_RECEIVED_FRAME_CALLBACK = (*env)->GetMethodID (env, jARDeviceControllerCls, "didReceiveFrameCallback", "(JIIII)V");
    ARCONTROLLER_JNIDEVICE_METHOD_TIMEOUT_FRAME_CALLBACK = (*env)->GetMethodID (env, jARDeviceControllerCls, "timeoutFrameCallback", "()V");    
    ARCONTROLLER_JNIDEVICE_METHOD_ON_COMMAND_RECEIVED = (*env)->GetMethodID (env, jARDeviceControllerCls, "onCommandReceived", "(IJ)V");    
    
    // cleanup
    (*env)->DeleteLocalRef (env, jARDeviceControllerCls);
}

/**
 * @brief Create and initialize a new device
 * @param env reference to the java environment
 * @param thizz reference to the object calling this function
 * @return new jni device object
 */
JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeNew (JNIEnv *env, jobject thizz, jlong jDevice)
{
    // -- Create the Device --

    // local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    //ARCONTROLLER_Device_t *deviceController = NULL;
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = NULL;
    ARDISCOVERY_Device_t *device = (ARDISCOVERY_Device_t*) (intptr_t) jDevice;

    jclass exceptionCls = NULL;
    jmethodID exceptionMethodInit = NULL;
    jthrowable exception = NULL;

    // allocate the JNI Device Controller
    jniDeviceController = ARCONTROLLER_JNI_Device_NewJNIDeviceController (env, thizz, device, &error);
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Device_AddStateChangedCallback (jniDeviceController->nativeDeviceController, ARCONTROLLER_JNI_Device_StateChanged, jniDeviceController);
    }

    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Device_AddCommandReceivedCallback (jniDeviceController->nativeDeviceController, ARCONTROLLER_JNI_Device_CommandReceived, jniDeviceController);
    }
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Device_SetVideoReceiveCallback (jniDeviceController->nativeDeviceController, ARCONTROLLER_JNI_Device_DidReceiveFrameCallback, ARCONTROLLER_JNI_Device_TimeoutFrameCallback , jniDeviceController);
    }

    if (error != ARCONTROLLER_OK)
    {
        // Delete the JNI device controller
        ARCONTROLLER_JNI_Device_DeleteJNIDeviceController (env, &jniDeviceController);
        
        // throw the exception
        exceptionCls = (*env)->FindClass(env, "com/parrot/arsdk/controller/ARControllerException");
        exceptionMethodInit = (*env)->GetMethodID(env, exceptionCls, "<init>", "(I)V");
        exception = (*env)->NewObject(env, exceptionCls, exceptionMethodInit, error);
        (*env)->Throw(env, exception);
    }

    return (long) jniDeviceController;
}

/**
 * @brief delete the Device
 * @param env reference to the java environment
 * @param thizz reference to the object calling this function
 * @param jDevice the ARCONTROLLER_Device_t to delete
 */
JNIEXPORT void JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeDelete (JNIEnv *env, jobject thizz, jlong jDeviceController)
{
    // -- Delete the DeviceController --

    // local declarations
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) jDeviceController;

    ARCONTROLLER_JNI_Device_DeleteJNIDeviceController (env, &jniDeviceController);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeStart (JNIEnv *env, jobject thizz, jlong jDeviceController)
{
    // local declarations
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) jDeviceController;
    
    return ARCONTROLLER_Device_Start (jniDeviceController->nativeDeviceController);
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeStop (JNIEnv *env, jobject thizz, jlong jDeviceController)
{
    // local declarations
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) jDeviceController;

    return ARCONTROLLER_Device_Stop (jniDeviceController->nativeDeviceController);
}

JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARDeviceController_nativeGetFeatureARDrone3(JNIEnv *env, jobject thizz, jlong jDeviceController)
{
    // local declarations
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) jDeviceController;

    return (long) jniDeviceController->nativeDeviceController->aRDrone3;
}

/*****************************************
 *
 *             private implementation:
 *
 *****************************************/

ARCONTROLLER_JNIDeviceController_t *ARCONTROLLER_JNI_Device_NewJNIDeviceController (JNIEnv *env, jobject thizz, ARDISCOVERY_Device_t *discoveryDevice, eARCONTROLLER_ERROR *error)
{
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = malloc (sizeof (ARCONTROLLER_JNIDeviceController_t));
    
    if (jniDeviceController != NULL)
    {
        // Create a new native deviceController
        jniDeviceController->nativeDeviceController = ARCONTROLLER_Device_New (discoveryDevice, error);
        
        // create a global reference of the java object
        jniDeviceController->jDeviceController = (*env)->NewGlobalRef (env, thizz);
    }
    else if (error != NULL)
    {
        *error = ARCONTROLLER_ERROR_ALLOC;
    }
    
    return jniDeviceController;
}

void ARCONTROLLER_JNI_Device_DeleteJNIDeviceController (JNIEnv *env, ARCONTROLLER_JNIDeviceController_t **jniDeviceController)
{
    if (jniDeviceController != NULL)
    {
        if ((*jniDeviceController) != NULL)
        {
            // Delete native deviceController
            ARCONTROLLER_Device_Delete (&((*jniDeviceController)->nativeDeviceController));
            
            // Delete global references
            (*env)->DeleteGlobalRef (env, (*jniDeviceController)->jDeviceController);
            (*jniDeviceController)->jDeviceController = NULL;
            
            free (*jniDeviceController);
            (*jniDeviceController) = NULL;
        }
    }
}

// called when the state of the device controller has changed
void ARCONTROLLER_JNI_Device_StateChanged (eARCONTROLLER_DEVICE_STATE newState, eARCONTROLLER_ERROR error, void *customData)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_JNIDEVICE_TAG, "stateChanged newState: %d .....", newState);
    
    // local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    JNIEnv* env = NULL;
    jint getEnvResult = JNI_OK;
    jint attachResult = 1;
    
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) customData;
        
    if ((jniDeviceController == NULL) ||
        (jniDeviceController->jDeviceController == NULL))
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // get the environment
        getEnvResult = (*ARCONTROLLER_JNIDEVICE_VM)->GetEnv(ARCONTROLLER_JNIDEVICE_VM, (void **) &env, JNI_VERSION_1_6);

        // if no environment then attach the thread to the virtual machine
        if (getEnvResult == JNI_EDETACHED)
        {
            ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARCONTROLLER_JNIDEVICE_TAG, "attach the thread to the virtual machine ...");
            attachResult = (*ARCONTROLLER_JNIDEVICE_VM)->AttachCurrentThread(ARCONTROLLER_JNIDEVICE_VM, &env, NULL);
        }

        if (env == NULL)
        {
            localError = ARCONTROLLER_ERROR_JNI_ENV;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // java onStateChanged callback
        (*env)->CallVoidMethod(env, jniDeviceController->jDeviceController, ARCONTROLLER_JNIDEVICE_METHOD_ON_STATE_CHANGED, newState, error);
    }
    
    // if the thread has been attached then detach the thread from the virtual machine
    if ((getEnvResult == JNI_EDETACHED) && (env != NULL))
    {
        (*ARCONTROLLER_JNIDEVICE_VM)->DetachCurrentThread(ARCONTROLLER_JNIDEVICE_VM);
    }
}

// called when a command has been received from the drone
void ARCONTROLLER_JNI_Device_CommandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary, void *customData)
{
    // local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    JNIEnv* env = NULL;
    jint getEnvResult = JNI_OK;
    jint attachResult = 1;
    
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) customData;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    if ((jniDeviceController == NULL) ||
        (jniDeviceController->jDeviceController == NULL))
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // get the environment
        getEnvResult = (*ARCONTROLLER_JNIDEVICE_VM)->GetEnv(ARCONTROLLER_JNIDEVICE_VM, (void **) &env, JNI_VERSION_1_6);

        // if no environment then attach the thread to the virtual machine
        if (getEnvResult == JNI_EDETACHED)
        {
            ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARCONTROLLER_JNIDEVICE_TAG, "attach the thread to the virtual machine ...");
            attachResult = (*ARCONTROLLER_JNIDEVICE_VM)->AttachCurrentThread(ARCONTROLLER_JNIDEVICE_VM, &env, NULL);
        }

        if (env == NULL)
        {
            localError = ARCONTROLLER_ERROR_JNI_ENV;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // java onCommandReceived callback
        (*env)->CallVoidMethod(env, jniDeviceController->jDeviceController, ARCONTROLLER_JNIDEVICE_METHOD_ON_COMMAND_RECEIVED, commandKey, (long) elementDictionary);
    }
    
    // if the thread has been attached then detach the thread from the virtual machine
    if ((getEnvResult == JNI_EDETACHED) && (env != NULL))
    {
        (*ARCONTROLLER_JNIDEVICE_VM)->DetachCurrentThread(ARCONTROLLER_JNIDEVICE_VM);
    }
}

void ARCONTROLLER_JNI_Device_DidReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_JNIDEVICE_TAG, "DidReceiveFrameCallback .....");
    
    // local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    JNIEnv* env = NULL;
    jint getEnvResult = JNI_OK;
    jint attachResult = 1;
    
    jlong data = 0;
    jint dataCapacity = 0;
    jint dataSize = 0;
    jint isIFrame = 0;
    jint missed = 0;
    
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) customData;
        
    if ((jniDeviceController == NULL) ||
        (jniDeviceController->jDeviceController == NULL))
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // get the environment
        getEnvResult = (*ARCONTROLLER_JNIDEVICE_VM)->GetEnv(ARCONTROLLER_JNIDEVICE_VM, (void **) &env, JNI_VERSION_1_6);

        // if no environment then attach the thread to the virtual machine
        if (getEnvResult == JNI_EDETACHED)
        {
            ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARCONTROLLER_JNIDEVICE_TAG, "attach the thread to the virtual machine ...");
            attachResult = (*ARCONTROLLER_JNIDEVICE_VM)->AttachCurrentThread(ARCONTROLLER_JNIDEVICE_VM, &env, NULL);
        }

        if (env == NULL)
        {
            localError = ARCONTROLLER_ERROR_JNI_ENV;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        
        if (frame != NULL)
        {
            data = frame->data;
            dataCapacity = frame->capacity;
            dataSize = frame->used;
            isIFrame = frame->isIFrame;
            missed = frame->missed;
        }
                
        // java onStateChanged callback
        (*env)->CallVoidMethod(env, jniDeviceController->jDeviceController, ARCONTROLLER_JNIDEVICE_METHOD_DID_RECEIVED_FRAME_CALLBACK, data, dataCapacity, dataSize, isIFrame, missed);
    }
    
    // if the thread has been attached then detach the thread from the virtual machine
    if ((getEnvResult == JNI_EDETACHED) && (env != NULL))
    {
        (*ARCONTROLLER_JNIDEVICE_VM)->DetachCurrentThread(ARCONTROLLER_JNIDEVICE_VM);
    }
}

void ARCONTROLLER_JNI_Device_TimeoutFrameCallback (void *customData)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_JNIDEVICE_TAG, "TimeoutFrameCallback .....");
    
    // local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    JNIEnv* env = NULL;
    jint getEnvResult = JNI_OK;
    jint attachResult = 1;
    
    ARCONTROLLER_JNIDeviceController_t *jniDeviceController = (ARCONTROLLER_JNIDeviceController_t*) (intptr_t) customData;
        
    if ((jniDeviceController == NULL) ||
        (jniDeviceController->jDeviceController == NULL))
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // get the environment
        getEnvResult = (*ARCONTROLLER_JNIDEVICE_VM)->GetEnv(ARCONTROLLER_JNIDEVICE_VM, (void **) &env, JNI_VERSION_1_6);

        // if no environment then attach the thread to the virtual machine
        if (getEnvResult == JNI_EDETACHED)
        {
            ARSAL_PRINT(ARSAL_PRINT_DEBUG, ARCONTROLLER_JNIDEVICE_TAG, "attach the thread to the virtual machine ...");
            attachResult = (*ARCONTROLLER_JNIDEVICE_VM)->AttachCurrentThread(ARCONTROLLER_JNIDEVICE_VM, &env, NULL);
        }

        if (env == NULL)
        {
            localError = ARCONTROLLER_ERROR_JNI_ENV;
        }
    }
    
    if (localError == ARCONTROLLER_OK)
    {
        // java onStateChanged callback
        (*env)->CallVoidMethod(env, jniDeviceController->jDeviceController, ARCONTROLLER_JNIDEVICE_METHOD_TIMEOUT_FRAME_CALLBACK);
    }
    
    // if the thread has been attached then detach the thread from the virtual machine
    if ((getEnvResult == JNI_EDETACHED) && (env != NULL))
    {
        (*ARCONTROLLER_JNIDEVICE_VM)->DetachCurrentThread(ARCONTROLLER_JNIDEVICE_VM);
    }
}
