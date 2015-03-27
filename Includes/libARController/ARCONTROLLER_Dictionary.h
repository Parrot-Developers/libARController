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
 * @file ARNETWORK_Command.h
 * @brief Dictionary controller allow to !!!!! TODO.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */

#ifndef _ARCONTROLLER_DICTIONARY_H_
#define _ARCONTROLLER_DICTIONARY_H_

#include <libuthash/uthash.h>
#include <libuthash/utlist.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_DICTIONARY_Key.h>

/**
 * @brief .
 */
typedef enum  
{
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING,
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM, /**< enumeration relative to the commands. must be read as I32 type. */
     
    ARCONTROLLER_DICTIONARY_VALUE_TYPE_MAX,
}eARCONTROLLER_DICTIONARY_VALUE_TYPE;

/**
 * @brief .
 */
typedef union 
{
    uint8_t U8;
    int8_t I8;
    uint16_t U16;
    int16_t I16;
    uint32_t U32;
    int32_t I32;
    uint64_t U64;
    int64_t I64;
    float Float;
    double Double;
    char * String;
}ARCONTROLLER_DICTIONARY_VALUE_t;

/**
 * @brief Dictionary element to storing the commands arguments coming from the device.
 */
typedef struct 
{
    const char *argument; /**< Key associates to the argument.*/
    ARCONTROLLER_DICTIONARY_VALUE_t value; /**< Value associates to the key ; value of the argument*/
    eARCONTROLLER_DICTIONARY_VALUE_TYPE valueType; /**< Type of the value*/
    UT_hash_handle hh; /**< makes this structure hashable */
}ARCONTROLLER_DICTIONARY_ARG_t;

/**
 * @brief Dictionary element to storing the commands coming from the device.
 */
typedef struct 
{
    eARCONTROLLER_DICTIONARY_KEY command; /**< Key associates to the command */
    ARCONTROLLER_DICTIONARY_ARG_t *arguments; /**< Arguments of the command coming from the device. */
    UT_hash_handle hh; /**< makes this structure hashable */
}ARCONTROLLER_DICTIONARY_COMMANDS_t;

/**
 * @brief . // TODO !!!!!!!!!!!!!!!!!!!
 * 
 * @param[in] customData customDate set by the register
 */
typedef void (*ARCONTROLLER_DICTIONARY_CALLBACK_t) (eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, void *customData); // TODO int -> ARCommands Big enum

/**
 * @brief 
 */
typedef struct ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t;

/**
 * @brief !!!! TODO
 */
typedef struct ARCONTROLLER_Dictionary_t ARCONTROLLER_Dictionary_t;

/**
 * @brief Create a new Command Controller
 * @warning This function allocate memory
 * @post ARCONTROLLER_Network_New() must be called to delete the Network Controller and free the memory allocated.
 * @param[in] commandKey Key of the new command.
 * @param[out] error executing error.
 * @return the new dictionary
 * @see ARCONTROLLER_Dictionary_New
 */
ARCONTROLLER_Dictionary_t *ARCONTROLLER_Dictionary_New (eARCONTROLLER_DICTIONARY_KEY commandKey, eARCONTROLLER_ERROR *error);

/**
 * @brief Delete the Command Controller
 * @warning This function free memory
 * @param command The Command controller to delete
 * @see ARCONTROLLER_Dictionary_New()
 */
void ARCONTROLLER_Dictionary_Delete (ARCONTROLLER_Dictionary_t **command);

/**
 * @brief Add a callback to use when the command is received
 * @param feature The feature controller receiving the command.
 * @param[in] callback the callback to add.
 * @param[out] error executing error.
 * @param[in] customData custom data given as parameter to the callback.
 */
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddCallback (ARCONTROLLER_Dictionary_t *command, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

/**
 * @brief Remove a callback used when the command is received
 * @param feature The feature controller receiving the command.
 * @param[in] callback the callback to remove.
 * @param[out] error executing error.
 */
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveCallback (ARCONTROLLER_Dictionary_t *command, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

//TODO add commentary !!!!!!!!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddDictionaryElement (ARCONTROLLER_Dictionary_t **dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

//TODO add commentary !!!!!!!!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveDictionaryElement (ARCONTROLLER_Dictionary_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

//TODO add commentary !!!!!!!!!!!!!!!!
void ARCONTROLLER_Dictionary_DeleteDictionary (ARCONTROLLER_Dictionary_t **dictionary);

//TODO add commentary !!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_Notify (ARCONTROLLER_Dictionary_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary);

//TODO add commentary !!!!!!!!
void ARCONTROLLER_Dictionary_DeleteCallbackArray (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackArray);

//TODO add commentary !!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddCallbackInArray (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackArray, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

//TODO add commentary !!!!!!!!
eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveCallbackFromArray (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackArray, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData);

void ARCONTROLLER_DICTIONARY_NotifyAllCallbackInArray (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackArray, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary);

//not checked
ARCONTROLLER_DICTIONARY_ARG_t *ARCONTROLLER_DICTIONARY_ArgumentsCopy (ARCONTROLLER_DICTIONARY_ARG_t *argumentDictionary, eARCONTROLLER_ERROR *error);


#endif /* _ARCONTROLLER_DICTIONARY_H_ */
