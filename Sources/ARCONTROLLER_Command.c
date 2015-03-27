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
 * @file ARController_Command.c
 * @brief ARCONTROLLER_Command allow to  !!!!! TODO.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <libuthash/uthash.h>
#include <libuthash/utlist.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_DICTIONARY_Key.h>
#include <libARController/ARCONTROLLER_Command.h>
#include "ARCONTROLLER_Command.h"

/*************************
 * Private header
 *************************/

//TODO add commentary !!!!!!!!!!!!!
int ARCONTROLLER_COMMAND_ElementCompare(ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *a, ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *b);

/*************************
 * Implementation
 *************************/
 
ARCONTROLLER_Command_t *ARCONTROLLER_COMMAND_New (eARCONTROLLER_DICTIONARY_KEY commandKey, eARCONTROLLER_ERROR *error)
{
    // -- Create a new Command --
    
    //local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Command_t *command =  NULL;
    
    // check parameters
    //if (commandKey == NULL)
    //{
    //    localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    //}
    // No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Command Controller
        command = malloc (sizeof (ARCONTROLLER_Command_t));
        if (command != NULL)
        {
            // Initialize to default values
            command->commandKey = commandKey;
            command->callbacks = NULL;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    // delete the Network Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_COMMAND_Delete (&command);
    }
    // No else: skipped by an error 

    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return command;
}

void ARCONTROLLER_COMMAND_Delete (ARCONTROLLER_Command_t **command)
{
    // -- Delete the command Controller --
    
    // local declarations
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *element = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementTmp = NULL;

    if (command != NULL)
    {
        if ((*command) != NULL)
        {
            if ((*command)->callbacks)
            {
                /* delete each element, use the safe iterator */
                //DL_FOREACH_SAFE ((*command)->callbacks, element, elementTmp)
                //{
                //    DL_DELETE ((*command)->callbacks, element);
                //}
                ARCONTROLLER_COMMAND_DeleteCallbackArray(&((*command)->callbacks));
                
            }
            
            free (*command);
            (*command) = NULL;
        }
    }
}

void ARCONTROLLER_COMMAND_DeleteCallbackArray (ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t **callbackArray)
{
    // -- Delete all callback in array --
    
    // local declarations
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *element = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementTmp = NULL;

    /* delete each element, use the safe iterator */
    DL_FOREACH_SAFE ((*callbackArray), element, elementTmp)
    {
        DL_DELETE ((*callbackArray), element);
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_AddCallback (ARCONTROLLER_Command_t *command, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add a callback to use when the command is received --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *newElement = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t likeElement;
    
    // check parameters
    if ((command == NULL) || (callback == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // check if the callback is not already registered
        //DL_SEARCH_SCALAR (command->callbacks, elementFind, callback, callback);//TODO sup !!!!
        
        // Element to find
        likeElement.callback = callback;
        likeElement.customData = customData;
        
        DL_SEARCH (command->callbacks, elementFind, &likeElement, ARCONTROLLER_COMMAND_ElementCompare);
        
        if (elementFind != NULL)
        {
            error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_ALREADY_REGISTERED;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // add the callback
        //newElement = malloc (sizeof(ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t));
        //if (newElement != NULL)
        //{
            //newElement->callback = callback;
            //newElement->customData = customData;
            //DL_APPEND (command->callbacks, newElement);
        //}
        //else
        //{
            //error = ARCONTROLLER_ERROR_ALLOC;
        //}
        error = ARCONTROLLER_COMMAND_AddCallbackInArray (&(command->callbacks), callback, customData);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_AddCallbackInArray (ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t **callbackArray, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add callback in array --
    
    // local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *newElement = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t likeElement;

    // add the callback
    newElement = malloc (sizeof(ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t));
    if (newElement != NULL)
    {
        newElement->callback = callback;
        newElement->customData = customData;
        DL_APPEND ((*callbackArray), newElement);
    }
    else
    {
        error = ARCONTROLLER_ERROR_ALLOC;
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_RemoveCallback (ARCONTROLLER_Command_t *command, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove a callback to use when the command  is received --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t likeElement;
    
    // check parameters
    if ((command == NULL) || (callback == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        ////DL_SEARCH_SCALAR (command->callbacks, elementFind, callback, callback);//TODO sup !!!!!!!!
        
        //// Element to find
        //likeElement.callback = callback;
        //likeElement.customData = customData;
        
        //DL_SEARCH (command->callbacks, elementFind, &likeElement, ARCONTROLLER_COMMAND_ElementCompare);
        //if (elementFind != NULL)
        //{
             //DL_DELETE (command->callbacks, elementFind);
        //}
        //else
        //{
            //error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED;
        //}
        error = ARCONTROLLER_COMMAND_RemoveCallbackFromArray (&(command->callbacks), callback, customData);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_RemoveCallbackFromArray (ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t **callbackArray, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove callback from array --
    
    // local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t likeElement;

    // Element to find
    likeElement.callback = callback;
    likeElement.customData = customData;
    
    DL_SEARCH ((*callbackArray), elementFind, &likeElement, ARCONTROLLER_COMMAND_ElementCompare);
    if (elementFind != NULL)
    {
         DL_DELETE ((*callbackArray), elementFind);
    }
    else
    {
        error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED;
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_AddDictionaryElement (ARCONTROLLER_Command_t **dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add a command dictionary element --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Command_t *dictElement = NULL;
    
    // check parameters
    if (dictionary == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // Find if the element already exist
        HASH_FIND_INT (*dictionary, &commandKey, dictElement);
        if (dictElement == NULL)
        {
            dictElement = ARCONTROLLER_COMMAND_New (commandKey, &error);
            
            if (error == ARCONTROLLER_OK)
            {
                HASH_ADD_INT (*dictionary, commandKey, dictElement);
            }
        }
        // NO ELSE ; command already exist
    }
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_COMMAND_AddCallback (dictElement, callback, customData);
    }
    
    ARSAL_PRINT(ARSAL_PRINT_INFO, ARCONTROLLER_COMMAND_TAG, " error : %d ; dictionary: %p ... ", error, *dictionary);
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_RemoveDictionaryElement (ARCONTROLLER_Command_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_FEATURE_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove a command dictionary element --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Command_t *dictElement = NULL;
    
    // check parameters
    if (dictionary == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // Find if the element already exist
        HASH_FIND_INT (dictionary, &commandKey, dictElement);
        if (dictElement != NULL)
        {
            error = ARCONTROLLER_COMMAND_RemoveCallback (dictElement, callback, customData);
        }
        else
        {
            error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED;
        }
    }
    
    return error;
}

void ARCONTROLLER_COMMAND_DeleteDictionary (ARCONTROLLER_Command_t **dictionary)
{
    // -- Delete a command dictionary --
    
    ARCONTROLLER_Command_t *dictElement = NULL;
    ARCONTROLLER_Command_t *dictTmp = NULL;
    
    if (dictionary != NULL)
    {
        if ((*dictionary) != NULL)
        {
            // Free the hash table contents
            HASH_ITER(hh, (*dictionary), dictElement, dictTmp)
            {
                /* for each element of the commands dictionary */
                
                HASH_DEL((*dictionary), dictElement);
                ARCONTROLLER_COMMAND_Delete (&dictElement);
            }
            
            free (*dictionary);
            (*dictionary) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_COMMAND_Notify (ARCONTROLLER_Command_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentDictionary)
{
    // -- Notify all listeners --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    ARCONTROLLER_Command_t *commandCallbacks = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *callbackElement = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *callbackElementTmp = NULL;
    
    if (error == ARCONTROLLER_OK)
    {
        // find the command
        HASH_FIND_INT (dictionary, &commandKey, commandCallbacks);
        if (commandCallbacks != NULL)
        {
            ///* for each callback */
            //DL_FOREACH_SAFE (commandCallbacks->callbacks, callbackElement, callbackElementTmp)
            //{
                //if (callbackElement->callback != NULL)
                //{
                    //callbackElement->callback (commandKey, argumentDictionary, callbackElement->customData);
                //}
            //}
            ARCONTROLLER_COMMAND_NotifyAllCallbackInArray (&(commandCallbacks->callbacks), commandKey, argumentDictionary);
        }
        // NO Else ; no callback registered.
    }
    
    return error;
}

void ARCONTROLLER_COMMAND_NotifyAllCallbackInArray (ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t **callbackArray, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentDictionary)
{
    // -- Notify all listeners --
    
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *callbackElement = NULL;
    ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *callbackElementTmp = NULL;
    
    /* for each callback */
    DL_FOREACH_SAFE ((*callbackArray), callbackElement, callbackElementTmp)
    {
        if (callbackElement->callback != NULL)
        {
            callbackElement->callback (commandKey, argumentDictionary, callbackElement->customData);
        }
    }
}

ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *ARCONTROLLER_COMMAND_ArgumentsCopy (ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentDictionary, eARCONTROLLER_ERROR *error)
{
    // -- Argument Copy --
    
    //TODO  not checked !!!!!!!!
    
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argumentsCopy = NULL;
    ARCONTROLLER_FEATURE_DICTIONARY_ARG_t *argDictNewElement = NULL;
    
    // check parameters
    if (argumentDictionary == NULL)
    {
        localError = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (localError == ARCONTROLLER_OK)
    {
        //// New argument element
        //argDictNewElement = malloc (sizeof(ARCONTROLLER_FEATURE_DICTIONARY_ARG_t));
        //if (argDictNewElement != NULL)
        //{
            //argDictNewElement->valueType = ARCONTROLLER_FEATURE_DICTIONARY_VALUE_TYPE_ENUM;
            //argDictNewElement->argument = ARCONTROLLER_DICTIONARY_KEY_JUMPINGSUMO_PILOTINGSTATE_POSTURECHANGED_STATE;
            //argDictNewElement->value.I32 = state;
            
            //HASH_ADD_KEYPTR (hh, argumentsCopy, argDictNewElement, strlen(argDictNewElement->argument), argDictNewElement);
        //}
        //else
        //{
            //localError == ARCONTROLLER_ERROR_ALLOC;
        //}
    }
    
    // return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 
    
    return argumentsCopy;
}

 /*************************
 * Private Implementation
 *************************/

//eARCONTROLLER_ERROR *ARCONTROLLER_COMMAND_InitDictionaryElement (ARCONTROLLER_Command_t *element, char *commandName)
//{
    //// -- initialize a new Command dictionary element--
    
    ////local declarations
    //eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    //// check parameters
    //if ((element == NULL) || (commandName == NULL))
    //{
        //error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    //}
    //// No Else: the checking parameters sets localError to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    //if (error == ARCONTROLLER_OK)
    //{
        //// Initialize to default values
        //element->command = commandName;
        //element->callbacks = NULL;
    //}
    
    //// delete the Network Controller if an error occurred
    //if (error != ARCONTROLLER_OK)
    //{
        //ARCONTROLLER_COMMAND_DisposeDictionaryElement (command);
    //}
    //// No else: skipped by an error 


    //return error;
//}

//void ARCONTROLLER_COMMAND_DisposeDictionaryElement (ARCONTROLLER_Command_t *element)
//{
    //// -- dispose the command dictionary element --
    
    //// local declarations
    //ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *element = NULL;
    //ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *elementTmp = NULL;

    //if (element != NULL)
    //{
        //if (element->callbacks)
        //{
            ///* delete each element, use the safe iterator */
            //DL_FOREACH_SAFE (element->callbacks, element, elementTmp)
            //{
                //DL_DELETE (element->callbacks, element);
            //}
        //}
    //}
//}

int ARCONTROLLER_COMMAND_ElementCompare(ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *a, ARCONTROLLER_COMMAND_CALLBAK_LIST_ELEMENT_t *b)
{
    return !((a->callback == b->callback) && (a->customData == b->customData));
}
