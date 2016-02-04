#!/usr/bin/env python

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

MYDIR=os.path.abspath(os.path.dirname(sys.argv[0]))
if '' == MYDIR:
    MYDIR=os.getcwd()

sys.path.append('%(MYDIR)s/../../ARBuildUtils/Utils/Python' % locals())

from ARFuncs import *
from ARCommandsParser import *

MODULE_ARCONTROLLER='ARCONTROLLER'
MODULE_NETWORK=MODULE_ARCONTROLLER+'_NETWORK'
MODULE_FEATURE=MODULE_ARCONTROLLER+'_FEATURE'
MODULE_DEVICE=MODULE_ARCONTROLLER+'_DEVICE'
MODULE_DICTIONARY=MODULE_ARCONTROLLER+'_DICTIONARY'
MODULE_ARCOMMANDS='ARCOMMANDS'

##################################
# Functions Utils                #
##################################

#Name of the output OBJC file containing device controller category
FEATURE_CONTROLLER_EVENTS_SUBSTR = ['Event']
FEATURE_CONTROLLER_STATES_SUBSTR = ['State']
FEATURE_CONTROLLER_LISTS_SUBSTR = ['List']

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
    return ARTypeName (module, feature.name, 'Send' + ARCapitalize(cmd.formattedName()))
    
def sendingFunctionName(module, feature, cmd):
    return ARFunctionName (module, feature.name, 'Send' + ARCapitalize(cmd.formattedName()))
    
def sendingFunction (cmd):
    return 'send'+ ARCapitalize(cmd.formattedName())

def nativeSendingFunction (cmd):
    return 'nativeSend' + ARCapitalize(cmd.formattedName())
    
def setNAckFunctionType( feature, cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return ARTypeName (MODULE_FEATURE, feature.name, 'Set' + ARCapitalize(cmd.formattedName()) + argPart)
    
def setNAckFunctionName( feature, cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return ARFunctionName (MODULE_FEATURE, feature.name, 'Set' + ARCapitalize(cmd.formattedName()) + argPart)

def setNAckFunction (cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'set' + ARCapitalize(cmd.formattedName()) + argPart
    
def sendNAckFunctionName (feature, cmd):
    return ARFunctionName (MODULE_ARCONTROLLER, feature.name, 'Send' + ARCapitalize(cmd.formattedName()) +'Struct')
    
def structNAckName(cmd):
    return ARCapitalize(cmd.formattedName()) + 'Parameters'
    
def structNAckType(feature, cmd):
    return ARTypeName (MODULE_ARCONTROLLER, feature.name, ARCapitalize(cmd.formattedName()) + 'Parameters')
    
def defineNotificationDef():
    return AREnumName(MODULE_ARCONTROLLER,  'DICTIONARY', 'KEY');

def defineNotification(feature, cmd=None, arg=None):
    cmdPart = ''
    if cmd :
        cmdPart = '_' + cmd.formattedName(underscore=True)
    argPart = ''
    if arg :
        argPart = '_' + arg.name
    return AREnumValue(MODULE_ARCONTROLLER,  'DICTIONARY', 'KEY', feature.name + cmdPart + argPart);
    
def nativeGetNotificationVal(feature, cmd=None, arg=None):
    cmdPart = ''
    if cmd :
        cmdPart = cmd.formattedName()
    argPart = ''
    if arg :
        argPart = '' + arg.name.replace("_", "");
    return 'nativeStaticGetKey'+ARCapitalize(feature.name) + ARCapitalize(cmdPart) + ARCapitalize(argPart);

def arcommandsSetDecode(feature, cmd):
    return 'ARCOMMANDS_Decoder_Set' + ARCapitalize(feature.name) + ARCapitalize(cmd.formattedName()) + 'Callback'

def decodeCallback(feature, cmd):
    return ARFunctionName (MODULE_FEATURE, feature.name, ARCapitalize(cmd.formattedName()) + 'Callback')
    
def discoveryProduct (productName):
    return 'ARDISCOVERY_PRODUCT_' + productName.upper ();
    
def javaFeatureClassName (feature):
    return 'ARFeature'+ ARCapitalize(feature.name)
    
def javaFeatureName (feature):
    return 'feature'+ ARCapitalize(feature.name)
    
def nativeFeatureName (feature):
    return 'nativeFeature'+ ARCapitalize(feature.name)
    
def nativeGetFeature (feature):
    return 'nativeGetFeature'+ ARCapitalize(feature.name)
    
def javaSetNAckFunction(cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'set' + ARCapitalize(cmd.formattedName()) + argPart
    
def nativeSetNAckFunction (cmd, arg=None):
    argPart = ''
    if arg:
        argPart = ARCapitalize(arg.name)
    return 'nativeSet' + ARCapitalize(cmd.formattedName()) + argPart

XMLTYPES = ['u8',       'i8',
            'u16',      'i16',
            'u32',      'i32',
            'u64',      'i64',
            'float',    'double',
            'string']
# Equivalent C types
CTYPES   = ['uint8_t',  'int8_t',
            'uint16_t', 'int16_t',
            'uint32_t', 'int32_t',
            'uint64_t', 'int64_t',
            'float',    'double',
            'char *']

OBJCTYPES = ['UnsignedChar',  'Char',
            'UnsignedShort', 'Short',
            'UnsignedInt', 'Int',
            'UnsignedLong', 'Long',
            'Float',    'Double',
            '']
            
# Equivalent JAVA Types
# No unsigned types in java, so use signed types everywhere
JAVATYPES = ['byte',    'byte',
             'short',   'short',
             'int',     'int',
             'long',    'long',
             'float',   'double',
             'String']
# Equivalent JNI Signatures
JAVASIG   = ['B',        'B',
             'S',        'S',
             'I',        'I',
             'J',        'J',
             'F',        'D',
             'Ljava/lang/String;']
# Equivalent JNI types
JNITYPES  = ['jbyte',    'jbyte',
             'jshort',   'jshort',
             'jint',     'jint',
             'jlong',    'jlong',
             'jfloat',   'jdouble',
             'jstring']

# Equivalent format types
FORMATTYPES  = ['"%"PRIu8',    '"%"PRIi8',
             '"%"PRIu16',   '"%"PRIi16',
             '"%"PRIu32',     '"%"PRIi32',
             '"%"PRIu64',    '"%"PRIi64',
             '"%f"',   '"%f"',
             '"%s"']

def xmlToC (module, proj, cmd, arg):
    if isinstance(arg.type, AREnum):
        return AREnumName(module, proj.name, arg.type.name)
    if isinstance(arg.type, ARBitfield):
        xmlIndex = XMLTYPES.index (arg.type.type)
    else:
        xmlIndex = XMLTYPES.index (arg.type)
    return CTYPES [xmlIndex]

def xmlToObjC (proj, cmd, arg):
    if isinstance(arg.type, AREnum):
        return '[NSNumber numberWithInt:' + arg.name + ']'
    if 'string' == arg.type:
        return '[NSString stringWithCString:' + arg.name + ' encoding:NSUTF8StringEncoding]'

    if isinstance(arg.type, ARBitfield):
        xmlIndex = XMLTYPES.index (arg.type.type)
    else:
        xmlIndex = XMLTYPES.index (arg.type)
    return '[NSNumber numberWith' + OBJCTYPES [xmlIndex] + ':' + arg.name + ']'

def xmlToJava (module, proj, cmd, arg):
    if isinstance(arg.type, AREnum):
        return ARJavaEnumType(module, proj.name, arg.type.name)
    if isinstance(arg.type, ARBitfield):
        xmlIndex = XMLTYPES.index (arg.type.type)
    else:
        xmlIndex = XMLTYPES.index (arg.type)
    return JAVATYPES [xmlIndex]
    
def xmlToFormat (arg):
    if isinstance(arg.type, AREnum):
        return '"%d"';
    if isinstance(arg.type, ARBitfield):
        xmlIndex = XMLTYPES.index (arg.type.type)
    else:
        xmlIndex = XMLTYPES.index (arg.type)
    return FORMATTYPES [xmlIndex]
    
def xmlToJNI (arg):
    if isinstance(arg.type, AREnum):
        return 'jint';
    if isinstance(arg.type, ARBitfield):
        xmlIndex = XMLTYPES.index (arg.type.type)
    else:
        xmlIndex = XMLTYPES.index (arg.type)
    return JNITYPES [xmlIndex]


class ARControllerDevice:
    "Represents a ARController_device, used to generate the device controller associated"
    def __init__(self, name, product):
        self.name     = name
        self.product  = product
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
    

def parseDeviceControllerXml (xmlDeviceController, allFeatures):
    "Parses DeviceController tag"
    
    deviceController = ARControllerDevice (xmlDeviceController.attributes["name"].nodeValue, xmlDeviceController.attributes["product"].nodeValue);
    
    xmlFeatures = xmlDeviceController.getElementsByTagName ('feature')
    # check xmlFeatures
    if len (xmlFeatures) == 0:
        ARPrint ('the device Controller:'+deviceController.name+' must have at least one feature')
        EXIT (1)
    
    for cmdFeature in xmlFeatures:
        featureName = cmdFeature.attributes["name"].nodeValue
        
        # check if the feature exists
        if [feature for feature in allFeatures if feature.name == featureName]:
            deviceController.features.append(featureName)
        else:
            ARPrint ('in the device Controller:'+deviceController.name+' the feature:'+featureName+'does not exists')
            EXIT (1)
    
    return deviceController

def parseDeviceControllersXml (fileName, allFeatures):
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
        
        deviceController = parseDeviceControllerXml (xmlDeviceController, allFeatures)
        if deviceController:
            deviceControllers.append(deviceController)
            ARPrint ('append:'+deviceController.name)
    
    return deviceControllers

