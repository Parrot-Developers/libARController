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
//#include "../../Includes/libARController/ARController.h"
#include "ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest.h"

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define TAG "ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest"
#define FAKEDRONE_IP_ADDRESS "192.168.42.1"
#define FAKEDRONE_DISCOVERY_PORT 44444
#define FAKEDRONE_C2D_PORT 54321 // should be read from Json
#define FAKEDRONE_D2C_PORT 43210

#define BD_NET_CD_NONACK_ID 10
#define BD_NET_CD_ACK_ID 11
#define BD_NET_CD_EMERGENCY_ID 12
#define BD_NET_CD_VIDEO_ACK_ID 13
#define BD_NET_DC_NAVDATA_ID 127
#define BD_NET_DC_EVENT_ID 126
#define BD_NET_DC_VIDEO_DATA_ID 125

/*****************************************
 *
 *             private header:
 *
 *****************************************/

//ARNETWORKAL_Manager_t *ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_NewARNetworkAL (ARDISCOVERY_Device_t *device, eARNETWORKAL_ERROR *error);
//void ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_DeleteARNetworkAL (ARDISCOVERY_Device_t *device, ARNETWORKAL_Manager_t **networkAL);
//void ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_InitNetworkCongifuration (ARDISCOVERY_Device_t *device, ARDISCOVERY_NetworkConfiguration_t *networkConfiguration);

//eARDISCOVERY_ERROR ARDISCOVERY_Connection_SendJsonCallback (uint8_t *dataTx, uint32_t *dataTxSize, void *customData);
//eARDISCOVERY_ERROR ARDISCOVERY_Connection_ReceiveJsonCallback (uint8_t *dataRx, uint32_t dataRxSize, char *ip, void *customData);

//typedef struct
//{
    //char *name;
    //char *type;
    //int d2cPort;
    //int c2dPort;
//}CONNECTION_DATA_t;

int ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_basicTest ();

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

int ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest ()
{
    int nbError = 0;
    
    ARSAL_PRINT (ARSAL_PRINT_INFO, TAG, "-- Start --");
    
    // basic test
    nbError += ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_basicTest();
    
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

int ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_basicTest ()
{
    int failed = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- Basic Test:");
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- MUST BE CONNNECTED TO AN ARDONE 3:");
    
    //init device for fakeDrone
    ARDISCOVERY_Device_t *device = NULL;
    eARDISCOVERY_ERROR errorDiscovery = ARDISCOVERY_OK;
    
    ARCONTROLLER_Network_t *networkController = NULL;
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    u_int8_t cmdBuffer[128];
    int32_t cmdSize = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_New ...");
    
    device = ARDISCOVERY_Device_New (&errorDiscovery);
    if ((errorDiscovery != ARDISCOVERY_OK) || (device == NULL))
    {
        failed += 1;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "device : %p", device);
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :", ARDISCOVERY_Error_ToString(errorDiscovery));
    }
    
    if (errorDiscovery == ARDISCOVERY_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi ...");
        errorDiscovery = ARDISCOVERY_Device_InitWifi (device, ARDISCOVERY_PRODUCT_ARDRONE, "toto", FAKEDRONE_IP_ADDRESS, FAKEDRONE_DISCOVERY_PORT);
        
        if (errorDiscovery != ARDISCOVERY_OK)
        {
            failed += 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Discovery error :", ARDISCOVERY_Error_ToString(errorDiscovery));
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_InitWifi succeed");
        }
    }
    
    if (errorDiscovery == ARDISCOVERY_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_New ...");
        //networkController = ARCONTROLLER_Network_New (device, &error);
        networkController = ARCONTROLLER_Network_New (device, NULL, NULL, NULL, NULL, &error);
    
        if ((error != ARCONTROLLER_OK) || (networkController == NULL))
        {
            failed += 1;
            if (error != ARCONTROLLER_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARCONTROLLER_Error_ToString(error));
            }
            
            if (error != ARCONTROLLER_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "networkController is NULL");
            }
        }
    }
    
    if (error == ARCONTROLLER_OK) 
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCOMMANDS_Generator_GenerateCommomSettingsAllSetting ...");
        // generate allSetting command
        
        eARCOMMANDS_GENERATOR_ERROR cmdError = ARCOMMANDS_Generator_GenerateCommonSettingsAllSettings (cmdBuffer, sizeof(cmdBuffer), &cmdSize);
        if (cmdError != ARCOMMANDS_GENERATOR_OK)
        {
            failed += 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "    - Generating of the commands AllSetting failed");
        }
    }
    
    if (failed == 0) 
    {
        // Send allSetting command
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - sending allsetting ...");
        
        eARNETWORK_ERROR netError = ARNETWORK_OK;
        
        error = ARCONTROLLER_Network_SendData (networkController, cmdBuffer, cmdSize, ARCONTROLLER_NETWORK_SENDING_DATA_TYPE_ACK, ARNETWORK_MANAGER_CALLBACK_RETURN_DATA_POP, &netError);
    
        if ((error != ARCONTROLLER_OK) || (netError != ARNETWORK_OK))
        {
            failed += 1;
            
            if (error != ARCONTROLLER_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARCONTROLLER_Error_ToString(error));
            }
            
            if (netError != ARNETWORK_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "netError: %s", ARNETWORK_Error_ToString(netError));
            }
            
        }
        else
        {
            ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - wait ...");
            sleep (1);
        }
    }
    
    //if (networkController != NULL)
    //{
        //ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_Stop ...");
        //error = ARCONTROLLER_Network_Stop (networkController);
        
        //if (error != ARCONTROLLER_OK)
        //{
            //failed += 1;
            //if (error != ARCONTROLLER_OK)
            //{
                //ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARDISCOVERY_Error_ToString(error));
            //}
        //}
    //}
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_Delete ...");
    ARCONTROLLER_Network_Delete (&networkController);
    
    if (networkController != NULL)
    {
        failed += 1;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "networkController not NULL");
    }
    
    if (device != NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARDISCOVERY_Device_Delete ...");
        ARDISCOVERY_Device_Delete (&device);
        
        if (device != NULL)
        {
            failed += 1;
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "device : %p", device);
        }
    }
    
    
    
    return failed;
} 
