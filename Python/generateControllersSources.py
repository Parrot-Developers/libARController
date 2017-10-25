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

NATIVE="native" 
JAVA="java"
JNI="jni"

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

PREBUILD_ACTION = PACKAGES_DIR+'/ARSDKBuildUtils/Utils/Python/ARSDK_PrebuildActions.py'

class Paths:
    def __init__(self, outdir):
        self.MY_GEN_DIR = outdir

        #Relative path of SOURCE dir
        self.SRC_DIR = self.MY_GEN_DIR + '/Sources/'

        #Relative path of INCLUDES dir
        self.INC_DIR = self.MY_GEN_DIR + '/Includes/libARController/'

        #Relative path of INCLUDES dir
        self.JNI_DIR = self.MY_GEN_DIR + '/JNI/'
        self.JNI_C_DIR = self.JNI_DIR + '/c/'
        self.JNI_JAVA_DIR = self.JNI_DIR + '/java/com/parrot/arsdk/arcontroller/'

        # Create array of generated files (so we can cleanup only our files)
        self.GENERATED_FILES = []
        self.GENERATED_FILES.append (self.INC_DIR + 'ARCONTROLLER_Device.h')
        self.GENERATED_FILES.append (self.SRC_DIR + 'ARCONTROLLER_Device.h')
        self.GENERATED_FILES.append (self.SRC_DIR + 'ARCONTROLLER_Device.c')
        self.GENERATED_FILES.append (self.JNI_C_DIR + 'ARCONTROLLER_JNI_Device.c')
        self.GENERATED_FILES.append (self.JNI_JAVA_DIR + 'ARDeviceController.java')

        self.GENERATED_FILES.append (self.INC_DIR + 'ARCONTROLLER_Feature.h')
        self.GENERATED_FILES.append (self.SRC_DIR + 'ARCONTROLLER_Feature.h')
        self.GENERATED_FILES.append (self.SRC_DIR + 'ARCONTROLLER_Feature.c')
        self.GENERATED_FILES.append (self.JNI_C_DIR + 'ARCONTROLLER_JNI_Feature*.c')
        self.GENERATED_FILES.append (self.JNI_JAVA_DIR + 'ARFeature*.java')

        self.GENERATED_FILES.append (self.INC_DIR + 'ARCONTROLLER_DICTIONARY_Key.c.h')
        self.GENERATED_FILES.append (self.SRC_DIR + 'ARCONTROLLER_DICTIONARY_Key.c')

def createDir(path):
    if not os.path.exists(os.path.dirname(path)):
        os.makedirs(os.path.dirname(path))

def generate_ctrls(ctx, paths, extra):
    createDir(paths.INC_DIR)
    createDir(paths.JNI_C_DIR)
    createDir(paths.JNI_JAVA_DIR)
    createDir(paths.SRC_DIR)

    #################################
    # 1ST PART :                    #
    #################################
    # Read XML file to local arrays #
    # of commands / classes         #
    #################################

    # generate DictionaryKeyEnum
    if extra == NATIVE:
        # generate Feature Controllers
        generateFeatureControllers (ctx, paths.SRC_DIR, paths.INC_DIR)
        # generate Device Controllers
        generateDeviceControllers (ctx, paths.SRC_DIR, paths.INC_DIR)
        # generate DictionaryKeyEnum
        generateDictionaryKeyEnum (ctx, paths.SRC_DIR, paths.INC_DIR)
        os.system(PREBUILD_ACTION+' --lib libARController --root '+LIBARCONTROLLER_DIR+' --outdir '+paths.MY_GEN_DIR+ ' --disable-java')
        #os.system(PREBUILD_ACTION+' --lib libARController --root '+LIBARCONTROLLER_DIR+' --outdir '+paths.MY_GEN_DIR)
    elif extra == JAVA:
        # generate Feature Controllers
        generateFeatureControllersJava (ctx, paths.JNI_JAVA_DIR);
        # generate Device Controllers
        generateControllersJava (ctx, paths.JNI_JAVA_DIR)
        # generate DictionaryKeyEnum
        generateDictionaryKeyEnumJava (ctx, paths.JNI_JAVA_DIR)
    elif extra == JNI:
        # generate Feature Controllers
        generateFeatureControllersJNI (ctx, paths.JNI_C_DIR);
        # generate Device Controllers
        generateControllersJNI (ctx, paths.JNI_C_DIR)

#===============================================================================
#===============================================================================
def list_files(ctx, outdir, extra):
    paths = Paths(outdir)

    # Print features controllers generated files
    if extra == NATIVE:
        list_files_ftr_ctrls (ctx, paths.SRC_DIR, paths.INC_DIR)
    elif extra == JAVA:
        list_files_ftr_ctrls_java (ctx,paths.JNI_C_DIR, paths.JNI_JAVA_DIR)
    elif extra == JNI:
        list_files_ftr_ctrls_jni (ctx,paths.JNI_C_DIR, paths.JNI_JAVA_DIR)

    # Print device controllers generated files
    if extra == NATIVE:
        list_files_deviceCtrls (ctx, paths.SRC_DIR, paths.INC_DIR)
    elif extra == JAVA:
        list_files_deviceCtrls_java (ctx, paths.JNI_C_DIR, paths.JNI_JAVA_DIR)
    elif extra == JNI:
        list_files_deviceCtrls_jni (ctx, paths.JNI_C_DIR, paths.JNI_JAVA_DIR)

    # Print device dictionary key generated files
    if extra == NATIVE:
        list_files_dict_key (ctx, paths.SRC_DIR, paths.INC_DIR)
    elif extra == JAVA:
        list_files_dict_key_java (ctx, paths.JNI_JAVA_DIR)

    if extra == NATIVE:
        print(os.path.join(outdir, "Sources/ARCONTROLLER_Error.c"))

#===============================================================================
#===============================================================================
def generate_files(ctx, outdir, extra):
    paths = Paths(outdir)

    # Generation
    generate_ctrls(ctx, paths, extra)
