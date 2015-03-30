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
 * @file main.h
 * @brief libARNetwork TestBench automatic
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libARSAL/ARSAL_Print.h>
#include <libARDiscovery/ARDISCOVERY_Error.h>
#include <libARDiscovery/ARDISCOVERY_Device.h>
#include <libARDiscovery/ARDISCOVERY_Connection.h>
#include <libARCommands/ARCommands.h>
#include <libARController/ARController.h>

#include "ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest.h"

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define TAG "ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest"

#define TEST_BEBOP 0
#define TEST_JS 1


#define DEVICE_TYPE TEST_BEBOP
//#define DEVICE_TYPE TEST_JS

#define FAKEDRONE_IP_ADDRESS "192.168.42.1"
#define FAKEDRONE_DISCOVERY_PORT 44444

#define JS_IP_ADDRESS "192.168.2.1"
#define JS_DISCOVERY_PORT 44444

/*****************************************
 *
 *             private header:
 *
 *****************************************/

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_basicTest ();

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_commandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, void *customData);

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, void *customData);

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_DidReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData);

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StatusChangedCallback (eARCONTROLLER_DEVICE_STATE newState, void *customData);

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest ()
{
    int nbError = 0;
    
    ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "-- Start --");
    
    // basic test
    nbError += ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_basicTest();
    
    if (nbError == 0)
    {
        ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, " | Success |");
    }
    else
    {
        ARSAL_PRINT (ARSAL_PRINT_ERROR, TAG, " | %d Errors are occured |", nbError);
    }
    
    ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "-- End --");
    
    return nbError;
}

   

/*****************************************
 *
 *             private implementation:
 *
 ****************************************/

/**
 *  TESTS
 */

int streamEnableReceived = 0;
int cmdReceived = 0;

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_basicTest ()
{
    int failed = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- Basic Test:");
#if DEVICE_TYPE == TEST_BEBOP
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- MUST BE CONNNECTED TO AN ARDONE 3:");
#elif DEVICE_TYPE == TEST_JS
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- MUST BE CONNNECTED TO A JumpingSumo:");
#endif
    
    ARDISCOVERY_Device_t *device = NULL;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Device_t *deviceController = NULL;
    
    pid_t child = 0;
    char videoOutFileName[] ="/home/mmaitre/Documents/ARDrone/SDK3/git/ARSDKBuildUtils/Targets/Unix/Build/TestBench/Unix/video_fifo";
    FILE *videoOut = NULL;
    
    failed += ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_initDiscoveryDevice (&device);
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARCONTROLLER_Device_New ... ");
        deviceController = ARCONTROLLER_Device_New (device, &error);
        
        if ((error != ARCONTROLLER_OK) || (deviceController == NULL))
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (device != NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARDISCOVERY_Device_Delete ... ");
        ARDISCOVERY_Device_Delete (&device);
        
        if (device != NULL)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error device is not NULL ");
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- open  videoOut ... ");
        
        
        //int mkfifoRes = mkfifo(videoOutFileName, S_IFIFO|0666);
        //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- mkfifoRes %d ", mkfifoRes);
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- start mplayer ... ");
        
        // fork the process to launch ffplay
        if ((child = fork()) == 0)
        {
            //execlp("mplayer", "mplayer", videoOutFileName, NULL);
            execlp("mplayer", "mplayer", "video_fifo", "lirc=no", NULL);
            //execlp("mplayer", videoOutFileName, NULL);
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Missing mplayer, you will not see the video. Please install mplayer.");
            return -1;
        }
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- fopen %s ", videoOutFileName);
        videoOut = fopen(videoOutFileName, "w");
        //videoOut = open(videoOutFileName, "w");
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "-   videoOut %p ... ", videoOut);
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- set Video callback ... ");
        
        error = ARCONTROLLER_Device_SetVideoReceiveCallback (deviceController, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_DidReceiveFrameCallback, NULL , videoOut);
    
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- add callback for command received ... ");
        
        error = ARCONTROLLER_Device_AddCommandRecievedCallback (deviceController, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_commandReceived, deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- add callback for satus changed ... ");
        
        error = ARCONTROLLER_Device_AddStatusChangedCallback (deviceController, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StatusChangedCallback, deviceController);

        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARCONTROLLER_Devcie_Start ... ");
        
        error = ARCONTROLLER_Device_Start (deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- add callback for StreamingVideoEnable ... ");

#if DEVICE_TYPE == TEST_BEBOP
        error = ARCONTROLLER_FEATURE_ARDrone3_AddCallback (deviceController->aRDrone3, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable, deviceController);
#elif DEVICE_TYPE == TEST_JS
        error = ARCONTROLLER_FEATURE_JumpingSumo_AddCallback (deviceController->jumpingSumo, ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable, deviceController);
#endif
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }

    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- send StreamingVideoEnable ... ");
        
        streamEnableReceived = 0;
#if DEVICE_TYPE == TEST_BEBOP
        error = deviceController->aRDrone3->sendMediaStreamingVideoEnable (deviceController->aRDrone3, 1);
#elif DEVICE_TYPE == TEST_JS
        error = deviceController->jumpingSumo->sendMediaStreamingVideoEnable (deviceController->jumpingSumo, 1);
#endif
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- wait ... ");
            sleep (5);
            
            if (streamEnableReceived == 0)
            {
                failed++;
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "last cmd StreamingVideoEnable not received");
            }
            
            if (cmdReceived == 0)
            {
                failed++;
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "last cmd not received with commandReceivedCallback");
            }
        }
    }
    
    if (failed == 0)
    {
        
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- wait for video ... ");
        sleep (50);
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- remove callback for StreamingVideoEnable ... ");
#if DEVICE_TYPE == TEST_BEBOP
        error = ARCONTROLLER_FEATURE_ARDrone3_RemoveCallback (deviceController->aRDrone3, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable, deviceController);
#elif DEVICE_TYPE == TEST_JS
        error = ARCONTROLLER_FEATURE_JumpingSumo_RemoveCallback (deviceController->jumpingSumo, ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable, deviceController);
#endif
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }

    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- remove callback for command received ... ");
        
        error = ARCONTROLLER_Device_RemoveCommandRecievedCallback (deviceController, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_commandReceived, deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARCONTROLLER_Devcie_Stop ... ");
        
        error = ARCONTROLLER_Device_Stop (deviceController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- remove callback for satus changed ... ");
        
        error = ARCONTROLLER_Device_RemoveStatusChangedCallback (deviceController, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StatusChangedCallback, deviceController);

        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
    }
    
    
    if (deviceController != NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARCONTROLLER_Device_Delete ... ");
        ARCONTROLLER_Device_Delete (&deviceController);
        
        if (deviceController != NULL)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error deviceController is not NULL ");
        }
    }
    
    if (videoOut != NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- close videoOut ... ");
        fflush (videoOut);
        fclose (videoOut);
        videoOut = NULL;
    }
    
    if (child > 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- stop mplayer  ... ");
        kill(child, SIGKILL);
    }
    
    return failed;
} 

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_initDiscoveryDevice (ARDISCOVERY_Device_t **device)
{
    int failed = 0;
    
    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- init discovey device  ... ");
    
    *device = ARDISCOVERY_Device_New (&errorDiscovery);
    if ((errorDiscovery != ARDISCOVERY_OK) || (device == NULL))
    {
        failed += 1;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "device : %p", device);
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
    }
    
    if (errorDiscovery == ARDISCOVERY_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi ...");
#if DEVICE_TYPE == TEST_BEBOP
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "ARDISCOVERY_PRODUCT_ARDRONE .....................");
        errorDiscovery = ARDISCOVERY_Device_InitWifi ((*device), ARDISCOVERY_PRODUCT_ARDRONE, "Bebop", FAKEDRONE_IP_ADDRESS, FAKEDRONE_DISCOVERY_PORT);
#elif DEVICE_TYPE == TEST_JS
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "ARDISCOVERY_PRODUCT_JS .....................");
        errorDiscovery = ARDISCOVERY_Device_InitWifi ((*device), ARDISCOVERY_PRODUCT_JS, "Js", JS_IP_ADDRESS, JS_DISCOVERY_PORT);
#endif
        
        if (errorDiscovery != ARDISCOVERY_OK)
        {
            failed += 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :%s", ARDISCOVERY_Error_ToString(errorDiscovery));
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - (*device)->produitID = %d", (*device)->productID );
            
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi succeed");
        }
    }
    
    return failed;
}

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_commandReceived (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, void *customData)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_commandReceived ........");
    
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    commandKey %d", commandKey);
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    argumentDictionary %p", argumentDictionary);
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    customData %p", customData);
    
    ARCONTROLLER_Device_t *deviceController = customData;
    
    if (deviceController != NULL)
    {
        cmdReceived = 1;
    }
    
}

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StreamEnable ........");
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    commandKey %d", commandKey);
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    argumentDictionary %p", argumentDictionary);
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    customData %p", customData);
    
    ARCONTROLLER_Device_t *deviceController = customData;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    
    ARCONTROLLER_DICTIONARY_ARG_t *streamEnableArgs = ARCONTROLLER_Device_GetCommandArguments (deviceController, commandKey, &error);
    
    if (deviceController != NULL)
    {
        if (error == ARCONTROLLER_OK)
        {
            if (streamEnableArgs != NULL)
            {
#if DEVICE_TYPE == TEST_BEBOP
                HASH_FIND_STR (streamEnableArgs, ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED_ENABLED, arg);
#elif DEVICE_TYPE == TEST_JS
                HASH_FIND_STR (streamEnableArgs, ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED_ENABLED, arg);
#endif
                
                if (arg != NULL)
                {
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "arg->valueType %d",arg->valueType);
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "arg->value %d",arg->value.U8);
                    streamEnableReceived = 1;
                }
                else
                {
                    ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "arg is NULL");
                }
            }
            else
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "streamEnableArgs is NULL");
            }
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Error : %s ", ARCONTROLLER_Error_ToString(error));
        }
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "customData is NULL ");
    }
}

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_DidReceiveFrameCallback (ARCONTROLLER_Frame_t *frame, void *customData)
{
    //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_DidReceiveFrameCallback ........");
    
    FILE *videoOut = customData;
    
    if (videoOut != NULL)
    {
        if (frame != NULL)
        {
            //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - frame->width:%d frame->height:%d ",frame->width, frame->height);
            //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - frame->data:%p frame->used:%d",frame->data, frame->used);
            //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - frame->isIFrame:%d",frame->isIFrame);
            //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - frame->missed:%d",frame->missed);
            
            //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - fwrite frame->data:%p frame->used:%d",frame->data, frame->used);
            fwrite(frame->data, frame->used, 1, videoOut);
            //fflush (videoOut);
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "frame is NULL.");
        }
    }
    else
    {
        ARSAL_PRINT(ARSAL_PRINT_WARNING, TAG, "videoOut is NULL.");
    }
}

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StatusChangedCallback (eARCONTROLLER_DEVICE_STATE newState, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_StatusChangedCallback newState:%d........", newState);
}
