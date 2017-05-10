'''
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
'''

import sys
import os
import re
import arsdkparser

from xml.dom.minidom import parseString

MYDIR=os.path.abspath(os.path.dirname(__file__))
PACKAGES_DIR=os.path.realpath(os.path.join(MYDIR, "../.."))
sys.path.append('%(PACKAGES_DIR)s/ARSDKBuildUtils/Utils/Python' % locals())
sys.path.append('%(PACKAGES_DIR)s/libARCommands/Tools' % locals())

from ARFuncs import *
from arsdkparser import *
from libARCommandsgen import *

MODULE_ARCONTROLLER='ARCONTROLLER'
MODULE_NETWORK=MODULE_ARCONTROLLER+'_NETWORK'
MODULE_FEATURE=MODULE_ARCONTROLLER+'_FEATURE'
MODULE_DEVICE=MODULE_ARCONTROLLER+'_DEVICE'
MODULE_DICTIONARY=MODULE_ARCONTROLLER+'_DICTIONARY'
MODULE_ARCOMMANDS='ARCommands'

##################################
# Functions Utils                #
##################################

#Name of the output OBJC file containing device controller category
FEATURE_CONTROLLER_EVENTS_SUBSTR = ['Event']
FEATURE_CONTROLLER_STATES_SUBSTR = ['State']
FEATURE_CONTROLLER_LISTS_SUBSTR = ['List']

# Equivalent format types
FORMATTYPES  = ['"%"PRIu8',    '"%"PRIi8',
             '"%"PRIu16',   '"%"PRIi16',
             '"%"PRIu32',     '"%"PRIi32',
             '"%"PRIu64',    '"%"PRIi64',
             '"%f"',   '"%f"',
             '"%s"']

def isEvent(cl):
    found = False
    for clname in FEATURE_CONTROLLER_EVENTS_SUBSTR:
        if clname.lower() in cl.name.lower():
            found = True
    return found

def isState(cl):
    found = False
    for clname in FEATURE_CONTROLLER_STATES_SUBSTR:
        if clname.lower() in cl.name.lower():
            found = True
    return found

def sendingFunctionType(module, feature, cmd):
    return ARTypeName (module, get_ftr_old_name(feature), 'Send' + ARCapitalize(format_cmd_name(cmd)))

def sendingFunctionName(module, feature, cmd):
    return ARFunctionName (module, get_ftr_old_name(feature), 'Send' + ARCapitalize(format_cmd_name(cmd)))

def sendingFunction (cmd):
    return 'send'+ ARCapitalize(format_cmd_name(cmd))

def nativeSendingFunction (cmd):
    return 'nativeSend' + ARCapitalize(format_cmd_name(cmd))

def setNAckFunctionType( feature, cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return ARTypeName (MODULE_FEATURE, get_ftr_old_name(feature), 'Set' + ARCapitalize(format_cmd_name(cmd)) + argPart)

def setNAckFunctionName( feature, cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return ARFunctionName (MODULE_FEATURE, get_ftr_old_name(feature), 'Set' + ARCapitalize(format_cmd_name(cmd)) + argPart)

def setNAckFunction (cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'set' + ARCapitalize(format_cmd_name(cmd)) + argPart

def sendNAckFunctionName (feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, get_ftr_old_name(feature), 'Send' + ARCapitalize(format_cmd_name(cmd)) +'Struct')

def structNAckName(cmd):
    return ARCapitalize(format_cmd_name(cmd)) + 'Parameters'

def structNAckType(feature, cmd):
    return ARTypeName (MODULE_ARCONTROLLER, get_ftr_old_name(feature), ARCapitalize(format_cmd_name(cmd)) + 'Parameters')

def nAckCbInit(feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, 'NAckCbs', get_ftr_old_name(feature) + ARCapitalize(format_cmd_name(cmd))  + 'Init')

def nAckCbDeInit(feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, 'NAckCbs', get_ftr_old_name(feature) + ARCapitalize(format_cmd_name(cmd)) + 'DeInit')

def nAckCbMustBeSent(feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, 'NAckCbs', get_ftr_old_name(feature) + ARCapitalize(format_cmd_name(cmd)) + 'MustBeSent')

def nAckCbChange(feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, 'NAckCbs', get_ftr_old_name(feature) + ARCapitalize(format_cmd_name(cmd)) + 'Changed')

def defineNotificationDef():
    return AREnumName(MODULE_ARCONTROLLER,  'DICTIONARY', 'KEY');

def defineNotification(feature, cmd=None, arg=None):
    cmdPart = ''
    if cmd :
        cmdPart = '_' + format_cmd_name(cmd, underscore=True)
    argPart = ''
    if arg :
        argPart = '_' + arg.name
    return AREnumValue(MODULE_ARCONTROLLER,  'DICTIONARY', 'KEY', get_ftr_old_name(feature) + cmdPart + argPart);

def nativeGetNotificationVal(feature, cmd=None, arg=None):
    cmdPart = ''
    if cmd :
        cmdPart = format_cmd_name(cmd)
    argPart = ''
    if arg :
        argPart = '' + arg.name.replace("_", "");
    return 'nativeStaticGetKey'+ARCapitalize(get_ftr_old_name(feature)) + ARCapitalize(cmdPart) + ARCapitalize(argPart);

def arcommandsSetDecode(feature, cmd):
    return 'ARCOMMANDS_Decoder_Set' + ARCapitalize(get_ftr_old_name(feature)) + ARCapitalize(format_cmd_name(cmd)) + 'Cb'

def decodeCallback(feature, cmd):
    return ARFunctionName (MODULE_FEATURE, get_ftr_old_name(feature), ARCapitalize(format_cmd_name(cmd)) + 'Callback')

def discoveryProduct (productName):
    return 'ARDISCOVERY_PRODUCT_' + productName.upper ();

def javaFeatureClassName (feature):
    return 'ARFeature'+ ARCapitalize(get_ftr_old_name(feature))

def javaFeatureName (feature):
    return 'feature'+ ARCapitalize(get_ftr_old_name(feature))

def nativeFeatureName (feature):
    return 'nativeFeature'+ ARCapitalize(get_ftr_old_name(feature))

def nativeGetFeature (feature):
    return 'nativeGetFeature'+ ARCapitalize(get_ftr_old_name(feature))

def javaSetNAckFunction(cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'set' + ARCapitalize(format_cmd_name(cmd)) + argPart

def nativeSetNAckFunction (cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'nativeSet' + ARCapitalize(format_cmd_name(cmd)) + argPart

def xmlToFormat (arg):
    if isinstance(arg.argType, ArEnum):
        return '"%d"';
    if isinstance(arg.argType, ArBitfield):
        xmlIndex = XMLTYPES.index (arg.argType.btfType)
    else:
        xmlIndex = XMLTYPES.index (arg.argType)
    return FORMATTYPES [xmlIndex]

def format_cmd_name(msg, underscore=False):#project only
    if underscore:
        return ARCapitalize(msg.name) if msg.cls is None else ARCapitalize(msg.cls.name) + '_'+  ARCapitalize(msg.name)
    else:
        return msg.name if msg.cls is None else msg.cls.name + ARCapitalize(msg.name)

def get_arg_doc(arg):
    doc = ''
    if arg.argType in ArArgType.TO_STRING:
        doc = arg.doc
    else:
        if arg.doc:
            doc = arg.doc + '\n'

        if isinstance(arg.argType, ArEnum):
            doc = doc + arg.argType.doc
        elif isinstance(arg.argType, ArBitfield):
            doc = doc + arg.argType.enum.doc

    return doc

def get_arg_doc(arg):
    doc = ''
    if arg.argType in ArArgType.TO_STRING:
        doc = arg.doc
    else:
        if arg.doc:
            doc = arg.doc + '\n'

        if isinstance(arg.argType, ArEnum):
            doc = doc + arg.argType.doc
        elif isinstance(arg.argType, ArBitfield):
            doc = doc + arg.argType.enum.doc

    return doc

def ftr_new_to_old_name(newName):
    FROM_NEW_NAME = {'ardrone3':'ARDrone3', 'common_dbg':'commonDebug',
                        'jpsumo':'JumpingSumo', 'minidrone':'MiniDrone',
                        'skyctrl':'SkyController'}
    if newName in FROM_NEW_NAME:
        return FROM_NEW_NAME[newName]
    else:
        return newName

def get_ftr_old_name(ftr):
    return ftr_new_to_old_name(ftr.name)

class ARControllerDevice:
    "Represents a ARController_device, used to generate the device controller associated"
    def __init__(self, name, product):
        self.name     = name
        self.product  = product
        self.flags    = None
        self.comments = []
        self.features = []

        # check name
        if not self.name:
            ARPrint ('all DeviceController must have a name')
            EXIT (1)

        # check device
        if self.product:
            None #TODO !!!!!
        else:
            ARPrint ('all DeviceController must have a product')
            EXIT (1)

class ARFlags:
    "Represents all flags that can be associated to a device controller"
    def __init__(self, flags):
        self.can_be_extension = False
        self.can_have_extension = False

        if flags:
            flag_list = flags.split("|")

            for flag in flag_list:
                if flag == 'can_be_extension':
                   self.can_be_extension = True
                elif flag == 'can_have_extension':
                   self.can_have_extension = True
                else:
                   ARPrint ('Flag ' + flag + ' not know')
                   EXIT (1)

def parseDeviceControllerXml (xmlDeviceController, ctx):
    "Parses DeviceController tag"

    deviceController = ARControllerDevice (xmlDeviceController.attributes["name"].nodeValue,
        xmlDeviceController.attributes["product"].nodeValue);

    flags_str = None
    if xmlDeviceController.hasAttribute("flags"):
        flags_str = xmlDeviceController.attributes["flags"].nodeValue
    deviceController.flags = ARFlags(flags_str)

    xmlFeatures = xmlDeviceController.getElementsByTagName ('feature')
    # check xmlFeatures
    if len (xmlFeatures) == 0:
        ARPrint ('the device Controller:'+deviceController.name+' must have at least one feature')
        EXIT (1)

    for cmdFeature in xmlFeatures:
        featureName = cmdFeature.attributes["name"].nodeValue

        # check if the feature exists
        if [feature for feature in ctx.features if feature.name == featureName]:
            deviceController.features.append(featureName)
        else:
            ARPrint ('in the device Controller:'+deviceController.name+' the feature:'+featureName+' does not exists')
            EXIT (1)

    return deviceController

def parseDeviceControllersXml (fileName, ctx):
    "Parses the file containing the deviceController list"

    deviceControllers = []

    if not os.path.exists(fileName):
        ARPrint ('file ' + fileName +' not exists')
        return None
    file = open (fileName, 'r')
    data = file.read ()
    file.close ()
    xmlfile = parseString (data)

    # Check if the XMLFile only contains ONE project (not zero, nor more)
    cmdDeviceControllers = xmlfile.getElementsByTagName ('deviceControllers')
    if len (cmdDeviceControllers) != 1:
        ARPrint (fileName + ' should contain exactly ONE deviceControllers tag.')
        EXIT (1)

    xmlDeviceControllerList = cmdDeviceControllers[0].getElementsByTagName ('ARController_Device')
    for xmlDeviceController in xmlDeviceControllerList:

        deviceController = parseDeviceControllerXml (xmlDeviceController, ctx)
        if deviceController:
            deviceControllers.append(deviceController)

    return deviceControllers
