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

ARNETWORKAL_Manager_t *ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_NewARNetworkAL (eARNETWORKAL_ERROR *error);
void ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_DeleteARNetworkAL (ARNETWORKAL_Manager_t **networkAL);
ARDISCOVERY_NetworkConfiguration_t ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_GetNetworkCongifuration ();

eARDISCOVERY_ERROR ARDISCOVERY_Connection_SendJsonCallback (uint8_t *dataTx, uint32_t *dataTxSize, void *customData);
eARDISCOVERY_ERROR ARDISCOVERY_Connection_ReceiveJsonCallback (uint8_t *dataRx, uint32_t dataRxSize, char *ip, void *customData);


typedef struct
{
    char *name;
    char *type;
    int d2cPort;
    int c2dPort;
}CONNECTION_DATA_t;

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
 
ARNETWORKAL_Manager_t *ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_NewARNetworkAL (eARNETWORKAL_ERROR *error)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- NewARNetworkAL ...");
    
    eARNETWORKAL_ERROR netAlError = ARNETWORKAL_OK;
    ARNETWORKAL_Manager_t *networkAL = NULL;
    
    // discovery connection
    if (ardiscoveryConnect ())
    {
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "- ARDiscovery Connection ERROR");
        netAlError = ARNETWORKAL_ERROR;
    }
    
    if (netAlError == ARNETWORKAL_OK)
    {
        // Create the ARNetworkALManager
        networkAL = ARNETWORKAL_Manager_New(&netAlError);
    }
    
    if (netAlError == ARNETWORKAL_OK)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, " ARNETWORKAL_Manager_InitWifiNetwork ... ip:%s | c2dPort:%d | d2cPort:%d", FAKEDRONE_IP_ADDRESS, FAKEDRONE_C2D_PORT, FAKEDRONE_D2C_PORT);
        
        netAlError = ARNETWORKAL_Manager_InitWifiNetwork (networkAL, FAKEDRONE_IP_ADDRESS, FAKEDRONE_C2D_PORT, FAKEDRONE_D2C_PORT, 1);
    }
    
    // return error
    if (error != NULL)
    {
        *error = netAlError;
    }
    
    // delete networkAL if an error occured
    if ((netAlError != ARNETWORKAL_OK) && (networkAL != NULL))
    {
        ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_DeleteARNetworkAL (&networkAL);
    }
    
    return networkAL;
}

void ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_DeleteARNetworkAL (ARNETWORKAL_Manager_t **networkAL)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- DeleteARNetworkAL ...");
    
    if (networkAL != NULL)
    {
        if ((*networkAL) != NULL)
        {
            ARNETWORKAL_Manager_Unlock((*networkAL));
        
            ARNETWORKAL_Manager_CloseWifiNetwork((*networkAL));
            ARNETWORKAL_Manager_Delete(networkAL);
        }
    }
}

ARDISCOVERY_NetworkConfiguration_t ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_GetNetworkCongifuration ()
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- GetNetworkCongifuration ...");
    
    
    static ARNETWORK_IOBufferParam_t c2dParams[] = {
        /* Non-acknowledged commands. */
        {
            .ID = BD_NET_CD_NONACK_ID,
            .dataType = ARNETWORKAL_FRAME_TYPE_DATA,
            .sendingWaitTimeMs = 20,
            .ackTimeoutMs = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
            .numberOfRetry = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
            .numberOfCell = 2,
            .dataCopyMaxSize = 128,
            .isOverwriting = 1,
        },
        /* Acknowledged commands. */
        {
            .ID = BD_NET_CD_ACK_ID,
            .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
            .sendingWaitTimeMs = 20,
            .ackTimeoutMs = 500,
            .numberOfRetry = 3,
            .numberOfCell = 20,
            .dataCopyMaxSize = 128,
            .isOverwriting = 0,
        },
        /* Emergency commands. */
        {
            .ID = BD_NET_CD_EMERGENCY_ID,
            .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
            .sendingWaitTimeMs = 10,
            .ackTimeoutMs = 100,
            .numberOfRetry = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
            .numberOfCell = 1,
            .dataCopyMaxSize = 128,
            .isOverwriting = 0,
        },
        ///* Video ACK (Initialized later) */
        //{
            //.ID = BD_NET_CD_VIDEO_ACK_ID,
            //.dataType = ARNETWORKAL_FRAME_TYPE_UNINITIALIZED,
            //.sendingWaitTimeMs = 0,
            //.ackTimeoutMs = 0,
            //.numberOfRetry = 0,
            //.numberOfCell = 0,
            //.dataCopyMaxSize = 0,
            //.isOverwriting = 0,
        //},
    };
    static const size_t numC2dParams = sizeof(c2dParams) / sizeof(ARNETWORK_IOBufferParam_t);

    static ARNETWORK_IOBufferParam_t d2cParams[] = {
        {
            .ID = BD_NET_DC_NAVDATA_ID,
            .dataType = ARNETWORKAL_FRAME_TYPE_DATA,
            .sendingWaitTimeMs = 20,
            .ackTimeoutMs = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
            .numberOfRetry = ARNETWORK_IOBUFFERPARAM_INFINITE_NUMBER,
            .numberOfCell = 20,
            .dataCopyMaxSize = 128,
            .isOverwriting = 0,
        },
        {
            .ID = BD_NET_DC_EVENT_ID,
            .dataType = ARNETWORKAL_FRAME_TYPE_DATA_WITH_ACK,
            .sendingWaitTimeMs = 20,
            .ackTimeoutMs = 500,
            .numberOfRetry = 3,
            .numberOfCell = 20,
            .dataCopyMaxSize = 128,
            .isOverwriting = 0,
        },
        ///* Video data (Initialized later) */
        //{
            //.ID = BD_NET_DC_VIDEO_DATA_ID,
            //.dataType = ARNETWORKAL_FRAME_TYPE_UNINITIALIZED,
            //.sendingWaitTimeMs = 0,
            //.ackTimeoutMs = 0,
            //.numberOfRetry = 0,
            //.numberOfCell = 0,
            //.dataCopyMaxSize = 0,
            //.isOverwriting = 0,
        //},
    };
    static const size_t numD2cParams = sizeof(d2cParams) / sizeof(ARNETWORK_IOBufferParam_t);

    static int commandBufferIds[] = {
        BD_NET_DC_NAVDATA_ID,
        BD_NET_DC_EVENT_ID,
    };
    static const size_t numOfCommandBufferIds = sizeof(commandBufferIds) / sizeof(int);
    
    
    
    ARDISCOVERY_NetworkConfiguration_t netConfig;
    
    netConfig.controllerToDeviceNotAckId = BD_NET_CD_NONACK_ID;
    netConfig.controllerToDeviceAckId = BD_NET_CD_ACK_ID;
    netConfig.controllerToDeviceHightPriority = BD_NET_CD_EMERGENCY_ID;
    netConfig.controllerToDeviceARStreamAck = BD_NET_CD_VIDEO_ACK_ID;
    netConfig.deviceToControllerNotAckId = BD_NET_DC_NAVDATA_ID;
    netConfig.deviceToControllerAckId = BD_NET_DC_NAVDATA_ID;
    //int deviceToControllerHightPriority = -1;
    netConfig.deviceToControllerARStreamData = BD_NET_DC_VIDEO_DATA_ID;
    
    netConfig.numberOfControllerToDeviceParam = numC2dParams;
    netConfig.controllerToDeviceParams = c2dParams;
    netConfig.numberOfDeviceToControllerParam = numD2cParams;
    netConfig.deviceToControllerParams = d2cParams;
    
    netConfig.bleNotificationIDs = NULL;
    netConfig.pingDelayMs = 0;
    
    netConfig.numberOfDeviceToControllerCommandsBufferIds = numOfCommandBufferIds;
    netConfig.deviceToControllerCommandsBufferIds = commandBufferIds;
    
    return  netConfig;
}

int ardiscoveryConnect ()
{
    int failed = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- ARDiscovery Connection");
    
    CONNECTION_DATA_t *connectionData = NULL;
    ARDISCOVERY_Connection_ConnectionData_t *discoveryData = NULL;
    
    if (!failed)
    {
        connectionData = malloc(sizeof(CONNECTION_DATA_t));
        if (connectionData != NULL)
        {
            connectionData->name = TAG;
            connectionData->type = TAG;
            connectionData->d2cPort = FAKEDRONE_D2C_PORT;
            connectionData->c2dPort = FAKEDRONE_C2D_PORT; //deviceManager->c2dPort = 0; // should be read from json
        }
        else
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
        discoveryData = ARDISCOVERY_Connection_New (ARDISCOVERY_Connection_SendJsonCallback, ARDISCOVERY_Connection_ReceiveJsonCallback, connectionData, &err);
        if (discoveryData == NULL || err != ARDISCOVERY_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Error while creating discoveryData : %s", ARDISCOVERY_Error_ToString(err));
            failed = 1;
        }
    }
    
    if (!failed)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, " ControllerConnection  ... ip:%s | port:%d", FAKEDRONE_IP_ADDRESS, FAKEDRONE_DISCOVERY_PORT);
        
        eARDISCOVERY_ERROR err = ARDISCOVERY_Connection_ControllerConnection(discoveryData, FAKEDRONE_DISCOVERY_PORT, FAKEDRONE_IP_ADDRESS);
        if (err != ARDISCOVERY_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "Error while opening discovery connection : %s", ARDISCOVERY_Error_ToString(err));
            failed = 1;
        }
    }
    
    // Cleanup
    ARDISCOVERY_Connection_Delete(&discoveryData);
    
    if (connectionData != NULL)
    {
        free (connectionData);
    }
    
    return failed;
}

eARDISCOVERY_ERROR ARDISCOVERY_Connection_SendJsonCallback (uint8_t *dataTx, uint32_t *dataTxSize, void *customData)
{
    CONNECTION_DATA_t *connectionData = (CONNECTION_DATA_t *)customData;
    eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
    
    if ((dataTx != NULL) && (dataTxSize != NULL) && (connectionData != NULL))
    {
        *dataTxSize = sprintf((char *)dataTx, "{ \"%s\": %d,\n \"%s\": \"%s\",\n \"%s\": \"%s\" }",
        ARDISCOVERY_CONNECTION_JSON_D2CPORT_KEY, connectionData->d2cPort,
        ARDISCOVERY_CONNECTION_JSON_CONTROLLER_NAME_KEY, connectionData->name,
        ARDISCOVERY_CONNECTION_JSON_CONTROLLER_TYPE_KEY, connectionData->type) + 1;
    }
    else
    {
        err = ARDISCOVERY_ERROR;
    }
    
    return err;
}

eARDISCOVERY_ERROR ARDISCOVERY_Connection_ReceiveJsonCallback (uint8_t *dataRx, uint32_t dataRxSize, char *ip, void *customData)
{
    CONNECTION_DATA_t *connectionData = (CONNECTION_DATA_t *)customData;
    eARDISCOVERY_ERROR err = ARDISCOVERY_OK;
    
    if ((dataRx != NULL) && (dataRxSize != 0) && (connectionData != NULL))
    {
        char *json = malloc(dataRxSize + 1);
        strncpy(json, (char *)dataRx, dataRxSize);
        json[dataRxSize] = '\0';
        
        //ARSAL_PRINT(ARSAL_PRINT_DEBUG, TAG, "    - ReceiveJson:%s ", json);
        
        //normally c2dPort should be read from the json here.
        
        free(json);
    }
    else
    {
        err = ARDISCOVERY_ERROR;
    }
    
    return err;
}

/**
 *  TESTS
 */

int ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_basicTest ()
{
    int failed = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- Basic Test:");
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "- MUST BE CONNNECTED TO AN ARDONE 3:");
    
    //init device for fakeDrone
    ARDISCOVERY_DiscoveryDevice_t device;
    
    device.name = "fakeDrone";
    device.nameLength = sizeof(device.name);
    device.productID = ARDISCOVERY_PRODUCT_ARDRONE;
    //ARDISCOVERY_DISCOVERYDEVICE_NetworkDevice_t device;
    device.newNetworkAL = &ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_NewARNetworkAL;
    device.deleteNetworkAL = &ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_DeleteARNetworkAL;
    device.getNetworkCongifuration = &ARCONTROLLER_TESTBENCH_NetworkControllerAutoTest_GetNetworkCongifuration;
    device.customData = NULL;
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    u_int8_t cmdBuffer[128];
    int32_t cmdSize = 0;
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_New ...");
    ARCONTROLLER_Network_t *networkController = ARCONTROLLER_Network_New (device, &error);
    
    if ((error != ARCONTROLLER_OK) || (networkController == NULL))
    {
        failed += 1;
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARDISCOVERY_Error_ToString(error));
        }
        
        if (error != ARCONTROLLER_OK)
        {
            ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "networkController is NULL");
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
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARDISCOVERY_Error_ToString(error));
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
    
    if (networkController != NULL)
    {
        ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_Stop ...");
        error = ARCONTROLLER_Network_Stop (networkController);
        
        if (error != ARCONTROLLER_OK)
        {
            failed += 1;
            if (error != ARCONTROLLER_OK)
            {
                ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "error: %s", ARDISCOVERY_Error_ToString(error));
            }
        }
    }
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, TAG, "    - ARCONTROLLER_Network_Delete ...");
    ARCONTROLLER_Network_Delete (&networkController);
    
    if (networkController != NULL)
    {
        failed += 1;
        ARSAL_PRINT(ARSAL_PRINT_ERROR, TAG, "networkController not NULL");
    }
    
    return failed;
} 
