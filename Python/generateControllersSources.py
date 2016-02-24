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

sys.path.append('%(MYDIR)s/../../ARSDKBuildUtils/Utils/Python' % locals())
sys.path.append('%(MYDIR)s/../../libARCommands/tools' % locals())

from ARFuncs import *
from ARCommandsParser import *
from generateFeatureControllers import *
from generateDeviceControllers import *
from generateDictionaryKeyEnum import *
from arsdkparser import *

# Default project name
DEFAULT_FEATURE_NAME='common'

noGen = False
genDebug = True
features = [DEFAULT_FEATURE_NAME]
args = sys.argv
args.pop (0)
libARCommandsDir=MYDIR+'/../../libARCommands/'
outputDir='.'

MY_GEN_DIR = MYDIR + '/../gen/'

#Relative path of SOURCE dir
SRC_DIR = MY_GEN_DIR + '/Sources/'

#Relative path of INCLUDES dir
INC_DIR = MY_GEN_DIR + '/Includes/libARController/'

#Relative path of INCLUDES dir
JNI_DIR = MY_GEN_DIR + '/JNI/'
JNI_C_DIR = JNI_DIR + '/c/'
JNI_JAVA_DIR = JNI_DIR + '/java/com/parrot/arsdk/arcontroller/'

# Create array of generated files (so we can cleanup only our files)
GENERATED_FILES = []
GENERATED_FILES.append (INC_DIR + 'ARCONTROLLER_Device.h')
GENERATED_FILES.append (SRC_DIR + 'ARCONTROLLER_Device.h')
GENERATED_FILES.append (SRC_DIR + 'ARCONTROLLER_Device.c')
GENERATED_FILES.append (JNI_C_DIR + 'ARCONTROLLER_JNI_Device.c')
GENERATED_FILES.append (JNI_JAVA_DIR + 'ARDeviceController.java')

GENERATED_FILES.append (INC_DIR + 'ARCONTROLLER_Feature.h')
GENERATED_FILES.append (SRC_DIR + 'ARCONTROLLER_Feature.h')
GENERATED_FILES.append (SRC_DIR + 'ARCONTROLLER_Feature.c')
GENERATED_FILES.append (JNI_C_DIR + 'ARCONTROLLER_JNI_Feature*.c')
GENERATED_FILES.append (JNI_JAVA_DIR + 'ARFeature*.java')

GENERATED_FILES.append (INC_DIR + 'ARCONTROLLER_DICTIONARY_Key.c.h')
GENERATED_FILES.append (SRC_DIR + 'ARCONTROLLER_DICTIONARY_Key.c')

def createDir(path):
    if not os.path.exists(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))

createDir(INC_DIR)
createDir(JNI_C_DIR)
createDir(JNI_JAVA_DIR)
createDir(SRC_DIR)

while len(args) > 0:
    a = args.pop (0)
    
    #################################
    # If "-fname" is passed as an   #
    # argument, just output the     #
    # name of the generated files   #
    #################################
    if a == "-fname":
        for fil in GENERATED_FILES:
            ARPrint (fil + ' ', True)
        ARPrint ('')
        EXIT (0)
        
    #################################
    # If "-dname" is passed as an   #
    # argument, just output the     #
    # name of the generated dirs    #
    #################################
    elif a == "-dname":
        EXIT (0)
    
    ####################################
    # If "-libARCommandsDir" is passed #
    # as an argument, we get Xml files #
    # in this directory otherwise '.'  #
    ####################################
    if a == "-libARCommandsDir":
        val = args.pop(0)
        libARCommandsDir=val
    ####################################
    # If "-outputDir" is passed        #
    # as an argument, we put output    #
    # filesin this directory,          #
    # otherwise '.'                    #
    ####################################
    #elif a == "-outputDir":
    #    val = args.pop(0)
    #    outputDir=val
    #################################
    # If "-nogen" is passed as an   #
    # argument, don't generate any  #
    # file                          #
    #################################
    elif a == "-nogen":
        noGen=True
    #################################
    # If -projectname is specified, #
    # use its value to set the      #
    # project name instead of the   #
    # default one.                  #
    #################################
    elif a == "-feature":
        featuresList = args.pop(0)
        for feature in featuresList.split(','):
            features.append (feature)
    else:
        ARPrint ('Invalid parameter ' + a)
        EXIT(1)

ARPrint ('libARCommandsDir = ' + libARCommandsDir)
#ARPrint ('outputDir = ' + outputDir)


#################################
# Read configure.ac             #
#################################

#configureAcFile = open (libARCommandsDir + '/Build/configure.ac', 'rb')
#AC_INIT_LINE=configureAcFile.readline ()
#while (not AC_INIT_LINE.startswith ('AC_INIT')) and ('' != AC_INIT_LINE):
#    AC_INIT_LINE=configureAcFile.readline ()
#if '' == AC_INIT_LINE:
#    ARPrint ('Unable to read from configure.ac file !')
#    EXIT (1)
#
#AC_ARGS=re.findall(r'\[[^]]*\]', AC_INIT_LINE)
#LIB_NAME=AC_ARGS[0].replace ('[', '').replace (']', '')
#LIB_MODULE=LIB_NAME.replace ('lib', '')
#LIB_VERSION=AC_ARGS[1].replace ('[', '').replace (']', '')


#################################
# 1ST PART :                    #
#################################
# Read XML file to local arrays #
# of commands / classes         #
#################################


# Get xml file
xmlDir = libARCommandsDir + '/Xml/'
listDir = os.listdir(xmlDir)
xmlFiles = [f for f in listDir if f.endswith('.xml')] # check if in features
xmlFiles.remove('generic.xml')
xmlFiles = sorted(xmlFiles, key=str)

# Parse all xml files
ctx = ArParserCtx()
for xml in ['generic.xml'] + xmlFiles:
    xmlPath = xmlDir+'/'+xml
    parse_xml(ctx, xmlPath)

if noGen: # called with "-nogen"
    ARPrint ('Commands parsed:')
    for ftr in allFeatures:
        ARPrint ('Feature ' + get_ftr_old_name(ftr))
        ARPrint ('/*')
        ARPrint (' * ' + ftr.doc.replace('\n', '\n * '))
        ARPrint (' */')
        for enum in ftr.enums:
            ARPrint (' --> enum:' + enum.name)
            ARPrint ('     /* ')
            ARPrint ('      * ' + enum.doc.replace('\n', '\n      * '))
            ARPrint ('      */')
            for eVal in enum.values:
                ARPrint ('     --> ' + eVal.name + 'val:' + str(eVal.value))
                ARPrint ('     /* ')
                ARPrint ('      * ' + eVal.doc.replace('\n', '\n      * '))
                ARPrint ('      */ ')
        for msg in ftr.cmds + ftr.evts:
            if isinstance (msg, ArCmd):
                ARPrint (' --> cmd:' + msg.name)
            else:
                ARPrint (' --> evt:' + msg.name)
            ARPrint ('     buffer:  ' + ArCmdBufferType.TO_STRING[msg.bufferType])
            ARPrint ('     timeout: ' + ArCmdTimeoutPolicy.TO_STRING[msg.timeoutPolicy])
            ARPrint ('     list:    ' + ArCmdListType.TO_STRING[msg.listType])
            ARPrint ('     content: ' + ArCmdContent.TO_STRING[msg.content])
            ARPrint ('     /* ')
            ARPrint ('      * ' + msg.doc.replace('\n', '\n      * '))
            ARPrint ('      */')
            for arg in msg.args:
                if isinstance (arg.argType, ArEnum):
                    ARPrint ('     (' + arg.argType.name + ' ' + arg.name + ')')
                elif isinstance (arg.argType, ArBitfield):
                    ARPrint ('     (bitfield:' + ArArgType.TO_STRING[arg.argType.btfType] + ':' + arg.argType.enum.name + ' ' + arg.name + ')')
                else:
                    ARPrint ('     (' + ArArgType.TO_STRING[arg.argType] + ' ' + arg.name + ')')
                ARPrint ('     /* ')
                ARPrint ('      * ' + arg.doc.replace('\n', '\n      * '))
                ARPrint ('      */')
    EXIT (0)

# generate Feature Controllers
generateFeatureControllers (ctx, SRC_DIR, INC_DIR)
generateFeatureControllersJNI (ctx, JNI_C_DIR, JNI_JAVA_DIR);

# generate Device Controllers
generateDeviceControllers (ctx, SRC_DIR, INC_DIR)
generateControllersJNI (ctx, JNI_C_DIR, JNI_JAVA_DIR)

# generate DictionaryKeyEnum
generateDictionaryKeyEnum (ctx, SRC_DIR, INC_DIR)
