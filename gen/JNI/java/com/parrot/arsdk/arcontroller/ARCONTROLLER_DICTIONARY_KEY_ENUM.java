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
/*
 * GENERATED FILE
 *  Do not modify this file, it will be erased during the next configure run 
 */

package com.parrot.arsdk.arcontroller;

import java.util.HashMap;

/**
 * Java copy of the eARCONTROLLER_DICTIONARY_KEY enum
 */
public enum ARCONTROLLER_DICTIONARY_KEY_ENUM {
   /** Dummy value for all unknown cases */
    eARCONTROLLER_DICTIONARY_KEY_UNKNOWN_ENUM_VALUE (Integer.MIN_VALUE, "Dummy value for all unknown cases"),
   /** Key used to define the feature <code>Generic</code> */
    ARCONTROLLER_DICTIONARY_KEY_GENERIC (0, "Key used to define the feature <code>Generic</code>"),
   /** Key used to define the feature <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE (1, "Key used to define the feature <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingStateFlatTrimChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSTATE_FLATTRIMCHANGED (2, "Key used to define the event <code>PilotingStateFlatTrimChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSTATE_FLYINGSTATECHANGED (3, "Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSTATE_ALERTSTATECHANGED (4, "Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingStateAutoTakeOffModeChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSTATE_AUTOTAKEOFFMODECHANGED (5, "Key used to define the event <code>PilotingStateAutoTakeOffModeChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingStateFlyingModeChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSTATE_FLYINGMODECHANGED (6, "Key used to define the event <code>PilotingStateFlyingModeChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_MEDIARECORDSTATE_PICTURESTATECHANGED (7, "Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_MEDIARECORDSTATE_PICTURESTATECHANGEDV2 (8, "Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_MEDIARECORDEVENT_PICTUREEVENTCHANGED (9, "Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMaxAltitudeChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSETTINGSSTATE_MAXALTITUDECHANGED (10, "Key used to define the event <code>PilotingSettingsStateMaxAltitudeChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMaxTiltChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_PILOTINGSETTINGSSTATE_MAXTILTCHANGED (11, "Key used to define the event <code>PilotingSettingsStateMaxTiltChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxVerticalSpeedChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SPEEDSETTINGSSTATE_MAXVERTICALSPEEDCHANGED (12, "Key used to define the event <code>SpeedSettingsStateMaxVerticalSpeedChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxRotationSpeedChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SPEEDSETTINGSSTATE_MAXROTATIONSPEEDCHANGED (13, "Key used to define the event <code>SpeedSettingsStateMaxRotationSpeedChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SpeedSettingsStateWheelsChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SPEEDSETTINGSSTATE_WHEELSCHANGED (14, "Key used to define the event <code>SpeedSettingsStateWheelsChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxHorizontalSpeedChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SPEEDSETTINGSSTATE_MAXHORIZONTALSPEEDCHANGED (15, "Key used to define the event <code>SpeedSettingsStateMaxHorizontalSpeedChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SettingsStateProductMotorsVersionChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SETTINGSSTATE_PRODUCTMOTORSVERSIONCHANGED (16, "Key used to define the event <code>SettingsStateProductMotorsVersionChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SettingsStateProductInertialVersionChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SETTINGSSTATE_PRODUCTINERTIALVERSIONCHANGED (17, "Key used to define the event <code>SettingsStateProductInertialVersionChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>SettingsStateCutOutModeChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_SETTINGSSTATE_CUTOUTMODECHANGED (18, "Key used to define the event <code>SettingsStateCutOutModeChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the event <code>FloodControlStateFloodControlChanged</code> in project <code>MiniDrone</code> */
    ARCONTROLLER_DICTIONARY_KEY_MINIDRONE_FLOODCONTROLSTATE_FLOODCONTROLCHANGED (19, "Key used to define the event <code>FloodControlStateFloodControlChanged</code> in project <code>MiniDrone</code>"),
   /** Key used to define the feature <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO (20, "Key used to define the feature <code>JumpingSumo</code>"),
   /** Key used to define the event <code>PilotingStatePostureChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_PILOTINGSTATE_POSTURECHANGED (21, "Key used to define the event <code>PilotingStatePostureChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_PILOTINGSTATE_ALERTSTATECHANGED (22, "Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>PilotingStateSpeedChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_PILOTINGSTATE_SPEEDCHANGED (23, "Key used to define the event <code>PilotingStateSpeedChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_PILOTINGSTATE_FLYINGSTATECHANGED (24, "Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>AnimationsStateJumpLoadChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ANIMATIONSSTATE_JUMPLOADCHANGED (25, "Key used to define the event <code>AnimationsStateJumpLoadChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>AnimationsStateJumpTypeChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ANIMATIONSSTATE_JUMPTYPECHANGED (26, "Key used to define the event <code>AnimationsStateJumpTypeChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>AnimationsStateJumpMotorProblemChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ANIMATIONSSTATE_JUMPMOTORPROBLEMCHANGED (27, "Key used to define the event <code>AnimationsStateJumpMotorProblemChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>SettingsStateProductGPSVersionChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_SETTINGSSTATE_PRODUCTGPSVERSIONCHANGED (28, "Key used to define the event <code>SettingsStateProductGPSVersionChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDSTATE_PICTURESTATECHANGED (29, "Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordStateVideoStateChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDSTATE_VIDEOSTATECHANGED (30, "Key used to define the event <code>MediaRecordStateVideoStateChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDSTATE_PICTURESTATECHANGEDV2 (31, "Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordStateVideoStateChangedV2</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDSTATE_VIDEOSTATECHANGEDV2 (32, "Key used to define the event <code>MediaRecordStateVideoStateChangedV2</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDEVENT_PICTUREEVENTCHANGED (33, "Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaRecordEventVideoEventChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIARECORDEVENT_VIDEOEVENTCHANGED (34, "Key used to define the event <code>MediaRecordEventVideoEventChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkSettingsStateWifiSelectionChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSETTINGSSTATE_WIFISELECTIONCHANGED (35, "Key used to define the event <code>NetworkSettingsStateWifiSelectionChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkStateWifiScanListChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSTATE_WIFISCANLISTCHANGED (36, "Key used to define the event <code>NetworkStateWifiScanListChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkStateAllWifiScanChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSTATE_ALLWIFISCANCHANGED (37, "Key used to define the event <code>NetworkStateAllWifiScanChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkStateWifiAuthChannelListChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSTATE_WIFIAUTHCHANNELLISTCHANGED (38, "Key used to define the event <code>NetworkStateWifiAuthChannelListChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkStateAllWifiAuthChannelChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSTATE_ALLWIFIAUTHCHANNELCHANGED (39, "Key used to define the event <code>NetworkStateAllWifiAuthChannelChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>NetworkStateLinkQualityChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_NETWORKSTATE_LINKQUALITYCHANGED (40, "Key used to define the event <code>NetworkStateLinkQualityChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>AudioSettingsStateMasterVolumeChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_AUDIOSETTINGSSTATE_MASTERVOLUMECHANGED (41, "Key used to define the event <code>AudioSettingsStateMasterVolumeChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>AudioSettingsStateThemeChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_AUDIOSETTINGSSTATE_THEMECHANGED (42, "Key used to define the event <code>AudioSettingsStateThemeChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>RoadPlanStateScriptMetadataListChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ROADPLANSTATE_SCRIPTMETADATALISTCHANGED (43, "Key used to define the event <code>RoadPlanStateScriptMetadataListChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>RoadPlanStateAllScriptsMetadataChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ROADPLANSTATE_ALLSCRIPTSMETADATACHANGED (44, "Key used to define the event <code>RoadPlanStateAllScriptsMetadataChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>RoadPlanStateScriptUploadChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ROADPLANSTATE_SCRIPTUPLOADCHANGED (45, "Key used to define the event <code>RoadPlanStateScriptUploadChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>RoadPlanStateScriptDeleteChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ROADPLANSTATE_SCRIPTDELETECHANGED (46, "Key used to define the event <code>RoadPlanStateScriptDeleteChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>RoadPlanStatePlayScriptChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_ROADPLANSTATE_PLAYSCRIPTCHANGED (47, "Key used to define the event <code>RoadPlanStatePlayScriptChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>SpeedSettingsStateOutdoorChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_SPEEDSETTINGSSTATE_OUTDOORCHANGED (48, "Key used to define the event <code>SpeedSettingsStateOutdoorChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>MediaStreamingStateVideoEnableChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED (49, "Key used to define the event <code>MediaStreamingStateVideoEnableChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the event <code>VideoSettingsStateAutorecordChanged</code> in project <code>JumpingSumo</code> */
    ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_VIDEOSETTINGSSTATE_AUTORECORDCHANGED (50, "Key used to define the event <code>VideoSettingsStateAutorecordChanged</code> in project <code>JumpingSumo</code>"),
   /** Key used to define the feature <code>CommonDebug</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMONDEBUG (51, "Key used to define the feature <code>CommonDebug</code>"),
   /** Key used to define the event <code>StatsEventSendPacket</code> in project <code>CommonDebug</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMONDEBUG_STATSEVENT_SENDPACKET (52, "Key used to define the event <code>StatsEventSendPacket</code> in project <code>CommonDebug</code>"),
   /** Key used to define the event <code>DebugSettingsStateInfo</code> in project <code>CommonDebug</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMONDEBUG_DEBUGSETTINGSSTATE_INFO (53, "Key used to define the event <code>DebugSettingsStateInfo</code> in project <code>CommonDebug</code>"),
   /** Key used to define the event <code>DebugSettingsStateListChanged</code> in project <code>CommonDebug</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMONDEBUG_DEBUGSETTINGSSTATE_LISTCHANGED (54, "Key used to define the event <code>DebugSettingsStateListChanged</code> in project <code>CommonDebug</code>"),
   /** Key used to define the feature <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER (55, "Key used to define the feature <code>SkyController</code>"),
   /** Key used to define the event <code>WifiStateWifiList</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_WIFISTATE_WIFILIST (56, "Key used to define the event <code>WifiStateWifiList</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>WifiStateConnexionChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_WIFISTATE_CONNEXIONCHANGED (57, "Key used to define the event <code>WifiStateConnexionChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>WifiStateWifiAuthChannelListChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_WIFISTATE_WIFIAUTHCHANNELLISTCHANGED (58, "Key used to define the event <code>WifiStateWifiAuthChannelListChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>WifiStateAllWifiAuthChannelChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_WIFISTATE_ALLWIFIAUTHCHANNELCHANGED (59, "Key used to define the event <code>WifiStateAllWifiAuthChannelChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>WifiStateWifiSignalChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_WIFISTATE_WIFISIGNALCHANGED (60, "Key used to define the event <code>WifiStateWifiSignalChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>DeviceStateDeviceList</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_DEVICESTATE_DEVICELIST (61, "Key used to define the event <code>DeviceStateDeviceList</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>DeviceStateConnexionChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_DEVICESTATE_CONNEXIONCHANGED (62, "Key used to define the event <code>DeviceStateConnexionChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SettingsStateAllSettingsChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SETTINGSSTATE_ALLSETTINGSCHANGED (63, "Key used to define the event <code>SettingsStateAllSettingsChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SettingsStateResetChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SETTINGSSTATE_RESETCHANGED (64, "Key used to define the event <code>SettingsStateResetChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SettingsStateProductSerialChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SETTINGSSTATE_PRODUCTSERIALCHANGED (65, "Key used to define the event <code>SettingsStateProductSerialChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SettingsStateProductVariantChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SETTINGSSTATE_PRODUCTVARIANTCHANGED (66, "Key used to define the event <code>SettingsStateProductVariantChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>CommonStateAllStatesChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_COMMONSTATE_ALLSTATESCHANGED (67, "Key used to define the event <code>CommonStateAllStatesChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SkyControllerStateBatteryChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SKYCONTROLLERSTATE_BATTERYCHANGED (68, "Key used to define the event <code>SkyControllerStateBatteryChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SkyControllerStateGpsFixChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SKYCONTROLLERSTATE_GPSFIXCHANGED (69, "Key used to define the event <code>SkyControllerStateGpsFixChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>SkyControllerStateGpsPositionChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_SKYCONTROLLERSTATE_GPSPOSITIONCHANGED (70, "Key used to define the event <code>SkyControllerStateGpsPositionChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AccessPointSettingsStateAccessPointSSIDChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_ACCESSPOINTSETTINGSSTATE_ACCESSPOINTSSIDCHANGED (71, "Key used to define the event <code>AccessPointSettingsStateAccessPointSSIDChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AccessPointSettingsStateAccessPointChannelChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_ACCESSPOINTSETTINGSSTATE_ACCESSPOINTCHANNELCHANGED (72, "Key used to define the event <code>AccessPointSettingsStateAccessPointChannelChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AccessPointSettingsStateWifiSelectionChanged</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_ACCESSPOINTSETTINGSSTATE_WIFISELECTIONCHANGED (73, "Key used to define the event <code>AccessPointSettingsStateWifiSelectionChanged</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>GamepadInfosStateGamepadControl</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_GAMEPADINFOSSTATE_GAMEPADCONTROL (74, "Key used to define the event <code>GamepadInfosStateGamepadControl</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>GamepadInfosStateAllGamepadControlsSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_GAMEPADINFOSSTATE_ALLGAMEPADCONTROLSSENT (75, "Key used to define the event <code>GamepadInfosStateAllGamepadControlsSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>ButtonMappingsStateCurrentButtonMappings</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_BUTTONMAPPINGSSTATE_CURRENTBUTTONMAPPINGS (76, "Key used to define the event <code>ButtonMappingsStateCurrentButtonMappings</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>ButtonMappingsStateAllCurrentButtonMappingsSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_BUTTONMAPPINGSSTATE_ALLCURRENTBUTTONMAPPINGSSENT (77, "Key used to define the event <code>ButtonMappingsStateAllCurrentButtonMappingsSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>ButtonMappingsStateAvailableButtonMappings</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_BUTTONMAPPINGSSTATE_AVAILABLEBUTTONMAPPINGS (78, "Key used to define the event <code>ButtonMappingsStateAvailableButtonMappings</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>ButtonMappingsStateAllAvailableButtonsMappingsSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_BUTTONMAPPINGSSTATE_ALLAVAILABLEBUTTONSMAPPINGSSENT (79, "Key used to define the event <code>ButtonMappingsStateAllAvailableButtonsMappingsSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisMappingsStateCurrentAxisMappings</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISMAPPINGSSTATE_CURRENTAXISMAPPINGS (80, "Key used to define the event <code>AxisMappingsStateCurrentAxisMappings</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisMappingsStateAllCurrentAxisMappingsSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISMAPPINGSSTATE_ALLCURRENTAXISMAPPINGSSENT (81, "Key used to define the event <code>AxisMappingsStateAllCurrentAxisMappingsSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisMappingsStateAvailableAxisMappings</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISMAPPINGSSTATE_AVAILABLEAXISMAPPINGS (82, "Key used to define the event <code>AxisMappingsStateAvailableAxisMappings</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisMappingsStateAllAvailableAxisMappingsSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISMAPPINGSSTATE_ALLAVAILABLEAXISMAPPINGSSENT (83, "Key used to define the event <code>AxisMappingsStateAllAvailableAxisMappingsSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisFiltersStateCurrentAxisFilters</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISFILTERSSTATE_CURRENTAXISFILTERS (84, "Key used to define the event <code>AxisFiltersStateCurrentAxisFilters</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisFiltersStateAllCurrentFiltersSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISFILTERSSTATE_ALLCURRENTFILTERSSENT (85, "Key used to define the event <code>AxisFiltersStateAllCurrentFiltersSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisFiltersStatePresetAxisFilters</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISFILTERSSTATE_PRESETAXISFILTERS (86, "Key used to define the event <code>AxisFiltersStatePresetAxisFilters</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>AxisFiltersStateAllPresetFiltersSent</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_AXISFILTERSSTATE_ALLPRESETFILTERSSENT (87, "Key used to define the event <code>AxisFiltersStateAllPresetFiltersSent</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>CoPilotingStatePilotingSource</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_COPILOTINGSTATE_PILOTINGSOURCE (88, "Key used to define the event <code>CoPilotingStatePilotingSource</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationState</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_CALIBRATIONSTATE_MAGNETOCALIBRATIONSTATE (89, "Key used to define the event <code>CalibrationStateMagnetoCalibrationState</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationQualityUpdatesState</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_CALIBRATIONSTATE_MAGNETOCALIBRATIONQUALITYUPDATESSTATE (90, "Key used to define the event <code>CalibrationStateMagnetoCalibrationQualityUpdatesState</code> in project <code>SkyController</code>"),
   /** Key used to define the event <code>ButtonEventsSettings</code> in project <code>SkyController</code> */
    ARCONTROLLER_DICTIONARY_KEY_SKYCONTROLLER_BUTTONEVENTS_SETTINGS (91, "Key used to define the event <code>ButtonEventsSettings</code> in project <code>SkyController</code>"),
   /** Key used to define the feature <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON (92, "Key used to define the feature <code>Common</code>"),
   /** Key used to define the event <code>NetworkEventDisconnection</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_NETWORKEVENT_DISCONNECTION (93, "Key used to define the event <code>NetworkEventDisconnection</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateAllSettingsChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_ALLSETTINGSCHANGED (94, "Key used to define the event <code>SettingsStateAllSettingsChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateResetChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_RESETCHANGED (95, "Key used to define the event <code>SettingsStateResetChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateProductNameChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_PRODUCTNAMECHANGED (96, "Key used to define the event <code>SettingsStateProductNameChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateProductVersionChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_PRODUCTVERSIONCHANGED (97, "Key used to define the event <code>SettingsStateProductVersionChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateProductSerialHighChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_PRODUCTSERIALHIGHCHANGED (98, "Key used to define the event <code>SettingsStateProductSerialHighChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateProductSerialLowChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_PRODUCTSERIALLOWCHANGED (99, "Key used to define the event <code>SettingsStateProductSerialLowChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateCountryChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_COUNTRYCHANGED (100, "Key used to define the event <code>SettingsStateCountryChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>SettingsStateAutoCountryChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_SETTINGSSTATE_AUTOCOUNTRYCHANGED (101, "Key used to define the event <code>SettingsStateAutoCountryChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateAllStatesChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_ALLSTATESCHANGED (102, "Key used to define the event <code>CommonStateAllStatesChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateBatteryStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_BATTERYSTATECHANGED (103, "Key used to define the event <code>CommonStateBatteryStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateMassStorageStateListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_MASSSTORAGESTATELISTCHANGED (104, "Key used to define the event <code>CommonStateMassStorageStateListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateMassStorageInfoStateListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_MASSSTORAGEINFOSTATELISTCHANGED (105, "Key used to define the event <code>CommonStateMassStorageInfoStateListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateCurrentDateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_CURRENTDATECHANGED (106, "Key used to define the event <code>CommonStateCurrentDateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateCurrentTimeChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_CURRENTTIMECHANGED (107, "Key used to define the event <code>CommonStateCurrentTimeChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateMassStorageInfoRemainingListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_MASSSTORAGEINFOREMAININGLISTCHANGED (108, "Key used to define the event <code>CommonStateMassStorageInfoRemainingListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateWifiSignalChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_WIFISIGNALCHANGED (109, "Key used to define the event <code>CommonStateWifiSignalChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateSensorsStatesListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_SENSORSSTATESLISTCHANGED (110, "Key used to define the event <code>CommonStateSensorsStatesListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateProductModel</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_PRODUCTMODEL (111, "Key used to define the event <code>CommonStateProductModel</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CommonStateCountryListKnown</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_COMMONSTATE_COUNTRYLISTKNOWN (112, "Key used to define the event <code>CommonStateCountryListKnown</code> in project <code>Common</code>"),
   /** Key used to define the event <code>OverHeatStateOverHeatChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_OVERHEATSTATE_OVERHEATCHANGED (113, "Key used to define the event <code>OverHeatStateOverHeatChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>OverHeatStateOverHeatRegulationChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_OVERHEATSTATE_OVERHEATREGULATIONCHANGED (114, "Key used to define the event <code>OverHeatStateOverHeatRegulationChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>WifiSettingsStateOutdoorSettingsChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_WIFISETTINGSSTATE_OUTDOORSETTINGSCHANGED (115, "Key used to define the event <code>WifiSettingsStateOutdoorSettingsChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>MavlinkStateMavlinkFilePlayingStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKFILEPLAYINGSTATECHANGED (116, "Key used to define the event <code>MavlinkStateMavlinkFilePlayingStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>MavlinkStateMavlinkPlayErrorStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_MAVLINKSTATE_MAVLINKPLAYERRORSTATECHANGED (117, "Key used to define the event <code>MavlinkStateMavlinkPlayErrorStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CALIBRATIONSTATE_MAGNETOCALIBRATIONSTATECHANGED (118, "Key used to define the event <code>CalibrationStateMagnetoCalibrationStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationRequiredState</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CALIBRATIONSTATE_MAGNETOCALIBRATIONREQUIREDSTATE (119, "Key used to define the event <code>CalibrationStateMagnetoCalibrationRequiredState</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationAxisToCalibrateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CALIBRATIONSTATE_MAGNETOCALIBRATIONAXISTOCALIBRATECHANGED (120, "Key used to define the event <code>CalibrationStateMagnetoCalibrationAxisToCalibrateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CalibrationStateMagnetoCalibrationStartedChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CALIBRATIONSTATE_MAGNETOCALIBRATIONSTARTEDCHANGED (121, "Key used to define the event <code>CalibrationStateMagnetoCalibrationStartedChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>CameraSettingsStateCameraSettingsChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CAMERASETTINGSSTATE_CAMERASETTINGSCHANGED (122, "Key used to define the event <code>CameraSettingsStateCameraSettingsChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>FlightPlanStateAvailabilityStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_AVAILABILITYSTATECHANGED (123, "Key used to define the event <code>FlightPlanStateAvailabilityStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>FlightPlanStateComponentStateListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANSTATE_COMPONENTSTATELISTCHANGED (124, "Key used to define the event <code>FlightPlanStateComponentStateListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>FlightPlanEventStartingErrorEvent</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANEVENT_STARTINGERROREVENT (125, "Key used to define the event <code>FlightPlanEventStartingErrorEvent</code> in project <code>Common</code>"),
   /** Key used to define the event <code>FlightPlanEventSpeedBridleEvent</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_FLIGHTPLANEVENT_SPEEDBRIDLEEVENT (126, "Key used to define the event <code>FlightPlanEventSpeedBridleEvent</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ARLibsVersionsStateControllerLibARCommandsVersion</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ARLIBSVERSIONSSTATE_CONTROLLERLIBARCOMMANDSVERSION (127, "Key used to define the event <code>ARLibsVersionsStateControllerLibARCommandsVersion</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ARLibsVersionsStateSkyControllerLibARCommandsVersion</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ARLIBSVERSIONSSTATE_SKYCONTROLLERLIBARCOMMANDSVERSION (128, "Key used to define the event <code>ARLibsVersionsStateSkyControllerLibARCommandsVersion</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ARLibsVersionsStateDeviceLibARCommandsVersion</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ARLIBSVERSIONSSTATE_DEVICELIBARCOMMANDSVERSION (129, "Key used to define the event <code>ARLibsVersionsStateDeviceLibARCommandsVersion</code> in project <code>Common</code>"),
   /** Key used to define the event <code>AudioStateAudioStreamingRunning</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_AUDIOSTATE_AUDIOSTREAMINGRUNNING (130, "Key used to define the event <code>AudioStateAudioStreamingRunning</code> in project <code>Common</code>"),
   /** Key used to define the event <code>HeadlightsStateIntensityChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_HEADLIGHTSSTATE_INTENSITYCHANGED (131, "Key used to define the event <code>HeadlightsStateIntensityChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>AnimationsStateList</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ANIMATIONSSTATE_LIST (132, "Key used to define the event <code>AnimationsStateList</code> in project <code>Common</code>"),
   /** Key used to define the event <code>AccessoryStateSupportedAccessoriesListChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ACCESSORYSTATE_SUPPORTEDACCESSORIESLISTCHANGED (133, "Key used to define the event <code>AccessoryStateSupportedAccessoriesListChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>AccessoryStateAccessoryConfigChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ACCESSORYSTATE_ACCESSORYCONFIGCHANGED (134, "Key used to define the event <code>AccessoryStateAccessoryConfigChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>AccessoryStateAccessoryConfigModificationEnabled</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_ACCESSORYSTATE_ACCESSORYCONFIGMODIFICATIONENABLED (135, "Key used to define the event <code>AccessoryStateAccessoryConfigModificationEnabled</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ChargerStateMaxChargeRateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CHARGERSTATE_MAXCHARGERATECHANGED (136, "Key used to define the event <code>ChargerStateMaxChargeRateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ChargerStateCurrentChargeStateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CHARGERSTATE_CURRENTCHARGESTATECHANGED (137, "Key used to define the event <code>ChargerStateCurrentChargeStateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ChargerStateLastChargeRateChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CHARGERSTATE_LASTCHARGERATECHANGED (138, "Key used to define the event <code>ChargerStateLastChargeRateChanged</code> in project <code>Common</code>"),
   /** Key used to define the event <code>ChargerStateChargingInfo</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_CHARGERSTATE_CHARGINGINFO (139, "Key used to define the event <code>ChargerStateChargingInfo</code> in project <code>Common</code>"),
   /** Key used to define the event <code>RunStateRunIdChanged</code> in project <code>Common</code> */
    ARCONTROLLER_DICTIONARY_KEY_COMMON_RUNSTATE_RUNIDCHANGED (140, "Key used to define the event <code>RunStateRunIdChanged</code> in project <code>Common</code>"),
   /** Key used to define the feature <code>Pro</code> */
    ARCONTROLLER_DICTIONARY_KEY_PRO (141, "Key used to define the feature <code>Pro</code>"),
   /** Key used to define the event <code>ProStateSupportedFeatures</code> in project <code>Pro</code> */
    ARCONTROLLER_DICTIONARY_KEY_PRO_PROSTATE_SUPPORTEDFEATURES (142, "Key used to define the event <code>ProStateSupportedFeatures</code> in project <code>Pro</code>"),
   /** Key used to define the event <code>ProStateFeaturesActivated</code> in project <code>Pro</code> */
    ARCONTROLLER_DICTIONARY_KEY_PRO_PROSTATE_FEATURESACTIVATED (143, "Key used to define the event <code>ProStateFeaturesActivated</code> in project <code>Pro</code>"),
   /** Key used to define the event <code>ProEventChallengeEvent</code> in project <code>Pro</code> */
    ARCONTROLLER_DICTIONARY_KEY_PRO_PROEVENT_CHALLENGEEVENT (144, "Key used to define the event <code>ProEventChallengeEvent</code> in project <code>Pro</code>"),
   /** Key used to define the feature <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3 (145, "Key used to define the feature <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDSTATE_PICTURESTATECHANGED (146, "Key used to define the event <code>MediaRecordStatePictureStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordStateVideoStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDSTATE_VIDEOSTATECHANGED (147, "Key used to define the event <code>MediaRecordStateVideoStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDSTATE_PICTURESTATECHANGEDV2 (148, "Key used to define the event <code>MediaRecordStatePictureStateChangedV2</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordStateVideoStateChangedV2</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDSTATE_VIDEOSTATECHANGEDV2 (149, "Key used to define the event <code>MediaRecordStateVideoStateChangedV2</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDEVENT_PICTUREEVENTCHANGED (150, "Key used to define the event <code>MediaRecordEventPictureEventChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaRecordEventVideoEventChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIARECORDEVENT_VIDEOEVENTCHANGED (151, "Key used to define the event <code>MediaRecordEventVideoEventChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateFlatTrimChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLATTRIMCHANGED (152, "Key used to define the event <code>PilotingStateFlatTrimChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_FLYINGSTATECHANGED (153, "Key used to define the event <code>PilotingStateFlyingStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALERTSTATECHANGED (154, "Key used to define the event <code>PilotingStateAlertStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateNavigateHomeStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_NAVIGATEHOMESTATECHANGED (155, "Key used to define the event <code>PilotingStateNavigateHomeStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStatePositionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_POSITIONCHANGED (156, "Key used to define the event <code>PilotingStatePositionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateSpeedChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_SPEEDCHANGED (157, "Key used to define the event <code>PilotingStateSpeedChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateAttitudeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ATTITUDECHANGED (158, "Key used to define the event <code>PilotingStateAttitudeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateAutoTakeOffModeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_AUTOTAKEOFFMODECHANGED (159, "Key used to define the event <code>PilotingStateAutoTakeOffModeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingStateAltitudeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSTATE_ALTITUDECHANGED (160, "Key used to define the event <code>PilotingStateAltitudeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingEventMoveByEnd</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGEVENT_MOVEBYEND (161, "Key used to define the event <code>PilotingEventMoveByEnd</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkStateWifiScanListChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSTATE_WIFISCANLISTCHANGED (162, "Key used to define the event <code>NetworkStateWifiScanListChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkStateAllWifiScanChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSTATE_ALLWIFISCANCHANGED (163, "Key used to define the event <code>NetworkStateAllWifiScanChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkStateWifiAuthChannelListChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSTATE_WIFIAUTHCHANNELLISTCHANGED (164, "Key used to define the event <code>NetworkStateWifiAuthChannelListChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkStateAllWifiAuthChannelChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSTATE_ALLWIFIAUTHCHANNELCHANGED (165, "Key used to define the event <code>NetworkStateAllWifiAuthChannelChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMaxAltitudeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_MAXALTITUDECHANGED (166, "Key used to define the event <code>PilotingSettingsStateMaxAltitudeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMaxTiltChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_MAXTILTCHANGED (167, "Key used to define the event <code>PilotingSettingsStateMaxTiltChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAbsolutControlChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_ABSOLUTCONTROLCHANGED (168, "Key used to define the event <code>PilotingSettingsStateAbsolutControlChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMaxDistanceChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_MAXDISTANCECHANGED (169, "Key used to define the event <code>PilotingSettingsStateMaxDistanceChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateNoFlyOverMaxDistanceChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_NOFLYOVERMAXDISTANCECHANGED (170, "Key used to define the event <code>PilotingSettingsStateNoFlyOverMaxDistanceChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxHorizontalSpeed</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_AUTONOMOUSFLIGHTMAXHORIZONTALSPEED (171, "Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxHorizontalSpeed</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxVerticalSpeed</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_AUTONOMOUSFLIGHTMAXVERTICALSPEED (172, "Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxVerticalSpeed</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxHorizontalAcceleration</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_AUTONOMOUSFLIGHTMAXHORIZONTALACCELERATION (173, "Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxHorizontalAcceleration</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxVerticalAcceleration</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_AUTONOMOUSFLIGHTMAXVERTICALACCELERATION (174, "Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxVerticalAcceleration</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxRotationSpeed</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_AUTONOMOUSFLIGHTMAXROTATIONSPEED (175, "Key used to define the event <code>PilotingSettingsStateAutonomousFlightMaxRotationSpeed</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateBankedTurnChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_BANKEDTURNCHANGED (176, "Key used to define the event <code>PilotingSettingsStateBankedTurnChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateMinAltitudeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_MINALTITUDECHANGED (177, "Key used to define the event <code>PilotingSettingsStateMinAltitudeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateCirclingDirectionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_CIRCLINGDIRECTIONCHANGED (178, "Key used to define the event <code>PilotingSettingsStateCirclingDirectionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateCirclingRadiusChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_CIRCLINGRADIUSCHANGED (179, "Key used to define the event <code>PilotingSettingsStateCirclingRadiusChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateCirclingAltitudeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_CIRCLINGALTITUDECHANGED (180, "Key used to define the event <code>PilotingSettingsStateCirclingAltitudeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStatePitchModeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_PITCHMODECHANGED (181, "Key used to define the event <code>PilotingSettingsStatePitchModeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PilotingSettingsStateLandingModeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PILOTINGSETTINGSSTATE_LANDINGMODECHANGED (182, "Key used to define the event <code>PilotingSettingsStateLandingModeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxVerticalSpeedChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SPEEDSETTINGSSTATE_MAXVERTICALSPEEDCHANGED (183, "Key used to define the event <code>SpeedSettingsStateMaxVerticalSpeedChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxRotationSpeedChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SPEEDSETTINGSSTATE_MAXROTATIONSPEEDCHANGED (184, "Key used to define the event <code>SpeedSettingsStateMaxRotationSpeedChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SpeedSettingsStateHullProtectionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SPEEDSETTINGSSTATE_HULLPROTECTIONCHANGED (185, "Key used to define the event <code>SpeedSettingsStateHullProtectionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SpeedSettingsStateOutdoorChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SPEEDSETTINGSSTATE_OUTDOORCHANGED (186, "Key used to define the event <code>SpeedSettingsStateOutdoorChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SpeedSettingsStateMaxPitchRollRotationSpeedChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SPEEDSETTINGSSTATE_MAXPITCHROLLROTATIONSPEEDCHANGED (187, "Key used to define the event <code>SpeedSettingsStateMaxPitchRollRotationSpeedChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkSettingsStateWifiSelectionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSETTINGSSTATE_WIFISELECTIONCHANGED (188, "Key used to define the event <code>NetworkSettingsStateWifiSelectionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkSettingsStateWifiSecurityChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSETTINGSSTATE_WIFISECURITYCHANGED (189, "Key used to define the event <code>NetworkSettingsStateWifiSecurityChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>NetworkSettingsStateWifiSecurity</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_NETWORKSETTINGSSTATE_WIFISECURITY (190, "Key used to define the event <code>NetworkSettingsStateWifiSecurity</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateProductMotorVersionListChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_PRODUCTMOTORVERSIONLISTCHANGED (191, "Key used to define the event <code>SettingsStateProductMotorVersionListChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateProductGPSVersionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_PRODUCTGPSVERSIONCHANGED (192, "Key used to define the event <code>SettingsStateProductGPSVersionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateMotorErrorStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_MOTORERRORSTATECHANGED (193, "Key used to define the event <code>SettingsStateMotorErrorStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateMotorSoftwareVersionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_MOTORSOFTWAREVERSIONCHANGED (194, "Key used to define the event <code>SettingsStateMotorSoftwareVersionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateMotorFlightsStatusChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_MOTORFLIGHTSSTATUSCHANGED (195, "Key used to define the event <code>SettingsStateMotorFlightsStatusChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateMotorErrorLastErrorChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_MOTORERRORLASTERRORCHANGED (196, "Key used to define the event <code>SettingsStateMotorErrorLastErrorChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>SettingsStateP7ID</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_SETTINGSSTATE_P7ID (197, "Key used to define the event <code>SettingsStateP7ID</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStatePictureFormatChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_PICTUREFORMATCHANGED (198, "Key used to define the event <code>PictureSettingsStatePictureFormatChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateAutoWhiteBalanceChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_AUTOWHITEBALANCECHANGED (199, "Key used to define the event <code>PictureSettingsStateAutoWhiteBalanceChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateExpositionChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_EXPOSITIONCHANGED (200, "Key used to define the event <code>PictureSettingsStateExpositionChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateSaturationChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_SATURATIONCHANGED (201, "Key used to define the event <code>PictureSettingsStateSaturationChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateTimelapseChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_TIMELAPSECHANGED (202, "Key used to define the event <code>PictureSettingsStateTimelapseChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateVideoAutorecordChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_VIDEOAUTORECORDCHANGED (203, "Key used to define the event <code>PictureSettingsStateVideoAutorecordChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PictureSettingsStateVideoStabilizationModeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PICTURESETTINGSSTATE_VIDEOSTABILIZATIONMODECHANGED (204, "Key used to define the event <code>PictureSettingsStateVideoStabilizationModeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>MediaStreamingStateVideoEnableChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_MEDIASTREAMINGSTATE_VIDEOENABLECHANGED (205, "Key used to define the event <code>MediaStreamingStateVideoEnableChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateHomeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_HOMECHANGED (206, "Key used to define the event <code>GPSSettingsStateHomeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateResetHomeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_RESETHOMECHANGED (207, "Key used to define the event <code>GPSSettingsStateResetHomeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateGPSFixStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_GPSFIXSTATECHANGED (208, "Key used to define the event <code>GPSSettingsStateGPSFixStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateGPSUpdateStateChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_GPSUPDATESTATECHANGED (209, "Key used to define the event <code>GPSSettingsStateGPSUpdateStateChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateHomeTypeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_HOMETYPECHANGED (210, "Key used to define the event <code>GPSSettingsStateHomeTypeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSSettingsStateReturnHomeDelayChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSETTINGSSTATE_RETURNHOMEDELAYCHANGED (211, "Key used to define the event <code>GPSSettingsStateReturnHomeDelayChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>CameraStateOrientation</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_ORIENTATION (212, "Key used to define the event <code>CameraStateOrientation</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>CameraStateDefaultCameraOrientation</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_CAMERASTATE_DEFAULTCAMERAORIENTATION (213, "Key used to define the event <code>CameraStateDefaultCameraOrientation</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>AntiflickeringStateElectricFrequencyChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_ANTIFLICKERINGSTATE_ELECTRICFREQUENCYCHANGED (214, "Key used to define the event <code>AntiflickeringStateElectricFrequencyChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>AntiflickeringStateModeChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_ANTIFLICKERINGSTATE_MODECHANGED (215, "Key used to define the event <code>AntiflickeringStateModeChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSStateNumberOfSatelliteChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSTATE_NUMBEROFSATELLITECHANGED (216, "Key used to define the event <code>GPSStateNumberOfSatelliteChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSStateHomeTypeAvailabilityChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSTATE_HOMETYPEAVAILABILITYCHANGED (217, "Key used to define the event <code>GPSStateHomeTypeAvailabilityChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>GPSStateHomeTypeChosenChanged</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_GPSSTATE_HOMETYPECHOSENCHANGED (218, "Key used to define the event <code>GPSStateHomeTypeChosenChanged</code> in project <code>ARDrone3</code>"),
   /** Key used to define the event <code>PROStateFeatures</code> in project <code>ARDrone3</code> */
    ARCONTROLLER_DICTIONARY_KEY_ARDRONE3_PROSTATE_FEATURES (219, "Key used to define the event <code>PROStateFeatures</code> in project <code>ARDrone3</code>"),
   /** Key used to define the feature <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME (220, "Key used to define the feature <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>Availability</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_AVAILABILITY (221, "Key used to define the event <code>Availability</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>Run</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_RUN (222, "Key used to define the event <code>Run</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>GeographicConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_GEOGRAPHICCONFIGCHANGED (223, "Key used to define the event <code>GeographicConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>RelativeConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_RELATIVECONFIGCHANGED (224, "Key used to define the event <code>RelativeConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>AnimRun</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_ANIMRUN (225, "Key used to define the event <code>AnimRun</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>SpiralAnimConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_SPIRALANIMCONFIGCHANGED (226, "Key used to define the event <code>SpiralAnimConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>SwingAnimConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_SWINGANIMCONFIGCHANGED (227, "Key used to define the event <code>SwingAnimConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>BoomerangAnimConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_BOOMERANGANIMCONFIGCHANGED (228, "Key used to define the event <code>BoomerangAnimConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>CandleAnimConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_CANDLEANIMCONFIGCHANGED (229, "Key used to define the event <code>CandleAnimConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>DollySlideAnimConfigChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_DOLLYSLIDEANIMCONFIGCHANGED (230, "Key used to define the event <code>DollySlideAnimConfigChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the event <code>UserFramingPositionChanged</code> in project <code>UnknownFeature_1</code> */
    ARCONTROLLER_DICTIONARY_KEY_FOLLOW_ME_USERFRAMINGPOSITIONCHANGED (231, "Key used to define the event <code>UserFramingPositionChanged</code> in project <code>UnknownFeature_1</code>"),
   /** Key used to define the feature <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI (232, "Key used to define the feature <code>Wifi</code>"),
   /** Key used to define the event <code>ScannedItem</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_SCANNEDITEM (233, "Key used to define the event <code>ScannedItem</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>AuthorizedChannel</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_AUTHORIZEDCHANNEL (234, "Key used to define the event <code>AuthorizedChannel</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>ApChannelChanged</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_APCHANNELCHANGED (235, "Key used to define the event <code>ApChannelChanged</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>SecurityChanged</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_SECURITYCHANGED (236, "Key used to define the event <code>SecurityChanged</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>CountryChanged</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_COUNTRYCHANGED (237, "Key used to define the event <code>CountryChanged</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>EnvironementChanged</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_ENVIRONEMENTCHANGED (238, "Key used to define the event <code>EnvironementChanged</code> in project <code>Wifi</code>"),
   /** Key used to define the event <code>RssiChanged</code> in project <code>Wifi</code> */
    ARCONTROLLER_DICTIONARY_KEY_WIFI_RSSICHANGED (239, "Key used to define the event <code>RssiChanged</code> in project <code>Wifi</code>"),
   /** Unused, iterator maximum value */
    ARCONTROLLER_DICTIONARY_DICTIONARY_KEY_MAX (240, "Unused, iterator maximum value");

    private final int value;
    private final String comment;
    static HashMap<Integer, ARCONTROLLER_DICTIONARY_KEY_ENUM> valuesList;

    ARCONTROLLER_DICTIONARY_KEY_ENUM (int value) {
        this.value = value;
        this.comment = null;
    }

    ARCONTROLLER_DICTIONARY_KEY_ENUM (int value, String comment) {
        this.value = value;
        this.comment = comment;
    }

    /**
     * Gets the int value of the enum
     * @return int value of the enum
     */
    public int getValue () {
        return value;
    }

    /**
     * Gets the ARCONTROLLER_DICTIONARY_KEY_ENUM instance from a C enum value
     * @param value C value of the enum
     * @return The ARCONTROLLER_DICTIONARY_KEY_ENUM instance, or null if the C enum value was not valid
     */
    public static ARCONTROLLER_DICTIONARY_KEY_ENUM getFromValue (int value) {
        if (null == valuesList) {
            ARCONTROLLER_DICTIONARY_KEY_ENUM [] valuesArray = ARCONTROLLER_DICTIONARY_KEY_ENUM.values ();
            valuesList = new HashMap<Integer, ARCONTROLLER_DICTIONARY_KEY_ENUM> (valuesArray.length);
            for (ARCONTROLLER_DICTIONARY_KEY_ENUM entry : valuesArray) {
                valuesList.put (entry.getValue (), entry);
            }
        }
        ARCONTROLLER_DICTIONARY_KEY_ENUM retVal = valuesList.get (value);
        if (retVal == null) {
            retVal = eARCONTROLLER_DICTIONARY_KEY_UNKNOWN_ENUM_VALUE;
        }
        return retVal;    }

    /**
     * Returns the enum comment as a description string
     * @return The enum description
     */
    public String toString () {
        if (this.comment != null) {
            return this.comment;
        }
        return super.toString ();
    }
}
