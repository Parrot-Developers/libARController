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
 * @file ARCONTROLLER_JNI_Dictionary.c
 * @brief libARController JNI dictionary c file.
 **/

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <jni.h>
#include <stdlib.h>

#include <uthash/uthash.h>
#include <uthash/utlist.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_Dictionary.h>
#include <libARController/ARCONTROLLER_DICTIONARY_Key.h>

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define ARCONTROLLER_JNIDICTIONARY_TAG "JNIDictionary"

/*****************************************
 *
 *             private header:
 *
 *****************************************/

static jmethodID ARCONTROLLER_JNIDICTIONARY_METHOD_NEW_INTEGER;
static jmethodID ARCONTROLLER_JNIDICTIONARY_METHOD_NEW_DOUBLE;

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

JNIEXPORT jstring JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerDictionary_nativeStaticGetSingleKey (JNIEnv *env, jclass class)
{
    return (*env)->NewStringUTF(env, ARCONTROLLER_DICTIONARY_SINGLE_KEY);
}


JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerDictionary_nativeGetSize (JNIEnv *env, jobject thizz, jlong jniDictionary)
{
    // local declarations
    ARCONTROLLER_DICTIONARY_ELEMENT_t *nativeDictionary = (ARCONTROLLER_DICTIONARY_ELEMENT_t*) (intptr_t) jniDictionary;
    
    return HASH_COUNT (nativeDictionary);
}

JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerDictionary_nativeGetElement (JNIEnv *env, jobject thizz, jlong jniDictionary, jstring key)
{
    // local declarations
    const char *nativeKey = (*env)->GetStringUTFChars(env, key, 0);
    ARCONTROLLER_DICTIONARY_ELEMENT_t *nativeDictionary = (ARCONTROLLER_DICTIONARY_ELEMENT_t*) (intptr_t) jniDictionary;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
    
    HASH_FIND_STR (nativeDictionary, nativeKey, element);
    
    // cleanup
    (*env)->ReleaseStringUTFChars(env, key, nativeKey);
    
    return (long) element;
}

JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerDictionary_nativeGetAllElements (JNIEnv *env, jobject thizz, jlong jniDictionary)
{
    // local declarations
    ARCONTROLLER_DICTIONARY_ELEMENT_t *nativeDictionary = (ARCONTROLLER_DICTIONARY_ELEMENT_t*) (intptr_t) jniDictionary;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *element = NULL;
    ARCONTROLLER_DICTIONARY_ELEMENT_t *elementTmp = NULL;
    
    int length = HASH_COUNT (nativeDictionary);
    jlongArray jElements = (*env)->NewLongArray(env, length);
    jlong *elementsArr = (*env)->GetLongArrayElements(env, jElements, NULL);
    
    int i = 0;
    HASH_ITER (hh, nativeDictionary, element, elementTmp)
    {
        // For each element
        elementsArr[i] = (long) element;
        i++;
    }
    
    // cleanup
    (*env)->ReleaseLongArrayElements(env, jElements, elementsArr, 0);
    
    return (long) jElements;
}

JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArg (JNIEnv *env, jobject thizz, jlong jniDictionary, jstring key)
{
    // local declarations
    const char *nativeKey = (*env)->GetStringUTFChars(env, key, 0);
    ARCONTROLLER_DICTIONARY_ELEMENT_t *nativeDictionary = (ARCONTROLLER_DICTIONARY_ELEMENT_t*) (intptr_t) jniDictionary;
    ARCONTROLLER_DICTIONARY_ARG_t *arg = NULL;
    
    if (nativeDictionary != NULL)
    {
        HASH_FIND_STR (nativeDictionary->arguments, nativeKey, arg);
    }
    
    // cleanup
    (*env)->ReleaseStringUTFChars(env, key, nativeKey);
    
    return (long) arg;
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArgType (JNIEnv *env, jobject thizz, jlong jniArg)
{
    // local declarations
    ARCONTROLLER_DICTIONARY_ARG_t *nativeArg = (ARCONTROLLER_DICTIONARY_ARG_t*) (intptr_t) jniArg;
    
    return nativeArg->valueType;
}

JNIEXPORT jint JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArgValueInt (JNIEnv *env, jobject thizz, jlong jniArg)
{
    // local declarations
    ARCONTROLLER_DICTIONARY_ARG_t *nativeArg = (ARCONTROLLER_DICTIONARY_ARG_t*) (intptr_t) jniArg;
    jint value = 0;
    
    if (nativeArg != NULL)
    {
        switch (nativeArg->valueType)
        {
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8:
                value = (jint) nativeArg->value.U8;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8:
                value = (jint) nativeArg->value.I8;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16:
                value = (jint) nativeArg->value.U16;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16:
                value = (jint) nativeArg->value.I16;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32:
                value = (jint) nativeArg->value.U32;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32:
                value = (jint) nativeArg->value.I32;
                break;
            
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "Bad valueType:%d to return int", nativeArg->valueType);
                break;

            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "ValueType:%d not known", nativeArg->valueType);
                break;
        }
    }
    
    return value;
}

JNIEXPORT jlong JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArgValueLong (JNIEnv *env, jobject thizz, jlong jniArg)
{
    // local declarations
    ARCONTROLLER_DICTIONARY_ARG_t *nativeArg = (ARCONTROLLER_DICTIONARY_ARG_t*) (intptr_t) jniArg;
    jlong value = 0;

    if (nativeArg != NULL)
    {
        switch (nativeArg->valueType)
        {
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64:
                value = (jlong) nativeArg->value.U64;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64:
                value = (jlong) nativeArg->value.I64;
                break;

            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "Bad valueType:%d to return double", nativeArg->valueType);
                break;

            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "ValueType:%d not known", nativeArg->valueType);
                break;
        }
    }

    return value;
}

JNIEXPORT jdouble JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArgValueDouble (JNIEnv *env, jobject thizz, jlong jniArg)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_JNIDICTIONARY_TAG, "nativeGetArgValueDouble ...");
    
    // local declarations
    ARCONTROLLER_DICTIONARY_ARG_t *nativeArg = (ARCONTROLLER_DICTIONARY_ARG_t*) (intptr_t) jniArg;
    double value = 0.0;
    
    if (nativeArg != NULL)
    {
        switch (nativeArg->valueType)
        {
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT:
                value = (double) nativeArg->value.Float;
                break;
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE:
                value = (double) nativeArg->value.Double;
                break;
            
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "Bad valueType:%d to return long", nativeArg->valueType);
                break;

            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "ValueType:%d not known", nativeArg->valueType);
                break;
        }
    }
    
    return value;
}

JNIEXPORT jstring JNICALL
Java_com_parrot_arsdk_arcontroller_ARControllerArgumentDictionary_nativeGetArgValueString (JNIEnv *env, jobject thizz, jlong jniArg)
{
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_JNIDICTIONARY_TAG, "nativeGetArgValueString ...");
    
    // local declarations
    ARCONTROLLER_DICTIONARY_ARG_t *nativeArg = (ARCONTROLLER_DICTIONARY_ARG_t*) (intptr_t) jniArg;
    jstring value = NULL;
    
    if (nativeArg != NULL)
    {
        switch (nativeArg->valueType)
        {
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING:
                value = (*env)->NewStringUTF(env, nativeArg->value.String);
                break;
            
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT:
            case ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "Bad valueType:%d to return long", nativeArg->valueType);
                break;

            default:
                ARSAL_PRINT(ARSAL_PRINT_ERROR, ARCONTROLLER_JNIDICTIONARY_TAG, "ValueType:%d not known", nativeArg->valueType);
                break;
        }
    }
    
    return value;
}


/*****************************************
 *
 *             private implementation:
 *
 *****************************************/

