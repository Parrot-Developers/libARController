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
#define FAKEDRONE_IP_ADDRESS "192.168.42.1"
#define FAKEDRONE_DISCOVERY_PORT 44444

/*****************************************
 *
 *             private header:
 *
 *****************************************/

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_basicTest ();

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_AllStateChangedCallback (int commandKey, ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentDictionary, void *customData);

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

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_basicTest ()
{
    int failed = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- Basic Test:");
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- MUST BE CONNNECTED TO AN ARDONE 3:");
    
    ARDISCOVERY_Device_t *device = NULL;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Device_t *deviceController = NULL;
    int cmdReceived = 0;
    
    failed += ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_initDiscoveryDevice (&device);
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARCONTROLLER_Device_New ... ");
        deviceController = ARCONTROLLER_Device_New (device, &error);
        
        if ((error != ARCONTROLLER_OK) || (deviceController == NULL))
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :", ARCONTROLLER_Error_ToString(error));
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
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- add callback for CommonAllStates ... ");
        
        error = ARCONTROLLER_FEATURE_Common_AddCallback (deviceController->common, ARCONTROLLER_FEATURE_COMMON_DICTIONARY_KEY_COMMONSTATE_ALLSTATESCHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_AllStateChangedCallback, &cmdReceived);
    
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :", ARCONTROLLER_Error_ToString(error));
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
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- sendCommonAllStates ... ");
        
        cmdReceived = 0;
        error = deviceController->common->sendCommonAllStates (deviceController->common);
        
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- wait ... ");
            sleep (2);
            
            if (cmdReceived == 0)
            {
                failed++;
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "last cmd not received");
            }
        }
    }
    
    if (failed == 0)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- remove callback for CommonAllStates ... ");
        
        error = ARCONTROLLER_FEATURE_Common_RemoveCallback (deviceController->common, ARCONTROLLER_FEATURE_COMMON_DICTIONARY_KEY_COMMONSTATE_ALLSTATESCHANGED, ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_AllStateChangedCallback, &cmdReceived);
    
        if (error != ARCONTROLLER_OK)
        {
            failed++;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- error :%s", ARCONTROLLER_Error_ToString(error));
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
    
    return failed;
} 

int ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_initDiscoveryDevice (ARDISCOVERY_Device_t **device)
{
    int failed = 0;
    
    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- init discovey device ... ");
    
    *device = ARDISCOVERY_Device_New (&errorDiscovery);
    if ((errorDiscovery != ARDISCOVERY_OK) || (device == NULL))
    {
        failed += 1;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "device : %p", device);
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :", ARDISCOVERY_Error_ToString(errorDiscovery));
    }
    
    if (errorDiscovery == ARDISCOVERY_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi ...");
        errorDiscovery = ARDISCOVERY_Device_InitWifi ((*device), ARDISCOVERY_PRODUCT_ARDRONE, "toto", FAKEDRONE_IP_ADDRESS, FAKEDRONE_DISCOVERY_PORT);
        
        if (errorDiscovery != ARDISCOVERY_OK)
        {
            failed += 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :", ARDISCOVERY_Error_ToString(errorDiscovery));
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - (*device)->produitID = %d", (*device)->productID );
            
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi succeed");
        }
    }
    
    return failed;
}

void ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_AllStateChangedCallback (int commandKey, ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentDictionary, void *customData)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_TESTBENCH_DeviceControllerAutoTest_AllStateChangedCallback ........");
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    commandKey %d", commandKey);
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    argumentDictionary %p", argumentDictionary);
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    customData %p", customData);
    
    int *cmdReceived = customData;
    
    if (cmdReceived != NULL)
    {
        *cmdReceived = 1;
    }
    
}
