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
import arsdkparser

MYDIR=os.path.abspath(os.path.dirname(__file__))
LIBARCONTROLLER_DIR=os.path.realpath(os.path.join(MYDIR, ".."))
PACKAGES_DIR=os.path.realpath(os.path.join(MYDIR, "../.."))

sys.path.append('%(PACKAGES_DIR)s/ARSDKBuildUtils/Utils/Python' % locals())

from ARFuncs import *
from generateFeatureControllers import *
from generateDeviceControllers import *
from generateDictionaryKeyEnum import *
from arsdkparser import *

genDebug = True

MY_GEN_DIR = LIBARCONTROLLER_DIR+'/gen/'

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

def generate_ctrls(ctx):
    createDir(INC_DIR)
    createDir(JNI_C_DIR)
    createDir(JNI_JAVA_DIR)
    createDir(SRC_DIR)

    #################################
    # 1ST PART :                    #
    #################################
    # Read XML file to local arrays #
    # of commands / classes         #
    #################################

    # generate Feature Controllers
    generateFeatureControllers (ctx, SRC_DIR, INC_DIR)
    generateFeatureControllersJNI (ctx, JNI_C_DIR, JNI_JAVA_DIR);

    # generate Device Controllers
    generateDeviceControllers (ctx, SRC_DIR, INC_DIR)
    generateControllersJNI (ctx, JNI_C_DIR, JNI_JAVA_DIR)

    # generate DictionaryKeyEnum
    generateDictionaryKeyEnum (ctx, SRC_DIR, INC_DIR)

#===============================================================================
#===============================================================================
def list_files(ctx, outdir):
    # Print device controllers generated files
    list_files_deviceCtrls (ctx, SRC_DIR, INC_DIR, JNI_C_DIR, JNI_JAVA_DIR)

    # Print device dictionary key generated files
    list_files_dict_key (ctx, SRC_DIR, INC_DIR)

    # Print features controllers generated files
    list_files_ftr_ctrls (ctx, SRC_DIR, INC_DIR, JNI_C_DIR, JNI_JAVA_DIR)
    

#===============================================================================
#===============================================================================
def generate_files(ctx, outdir):
    # Remove old generation
    os.system('rm -rf '+LIBARCONTROLLER_DIR+'/gen/*')
    
    # Generation
    generate_ctrls(ctx)
    PREBUILD_ACTION = PACKAGES_DIR+'/ARSDKBuildUtils/Utils/Python/ARSDK_PrebuildActions.py'
    os.system('python '+PREBUILD_ACTION+' --lib libARController --root '+LIBARCONTROLLER_DIR)
