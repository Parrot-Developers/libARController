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
 * @file ARController_Dictionary.c
 * @brief ARCONTROLLER_Dictionary allow to  !!!!! TODO.
 * @date 02/03/2015
 * @author maxime.maitre@parrot.com
 */
 
#include <stdlib.h>

#include <uthash/uthash.h>
#include <uthash/utlist.h>

#include <libARSAL/ARSAL_Print.h>

#include <libARController/ARCONTROLLER_Error.h>
#include <libARController/ARCONTROLLER_DICTIONARY_Key.h>
#include <libARController/ARCONTROLLER_Dictionary.h>
#include "ARCONTROLLER_Dictionary.h"

/*************************
 * Private header
 *************************/

/**
 * @brief Compare two list elements
 * @param a The first element
 * @param b The second element
 * @return 0 if the two elements are equal ; otherwise 1.
 */
int ARCONTROLLER_DICTIONARY_ElementCompare(ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *a, ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *b);

/*************************
 * Implementation
 *************************/
 
ARCONTROLLER_Dictionary_t *ARCONTROLLER_Dictionary_New (eARCONTROLLER_DICTIONARY_KEY commandKey, eARCONTROLLER_ERROR *error)
{
    // -- Create a new dictionary --
    
    // Local declarations
    eARCONTROLLER_ERROR localError = ARCONTROLLER_OK;
    ARCONTROLLER_Dictionary_t *dictionary =  NULL;
    
    if (localError == ARCONTROLLER_OK)
    {
        // Create the Command Controller
        dictionary = malloc (sizeof (ARCONTROLLER_Dictionary_t));
        if (dictionary != NULL)
        {
            // Initialize to default values
            dictionary->commandKey = commandKey;
            dictionary->callbacks = NULL;
        }
        else
        {
            localError = ARCONTROLLER_ERROR_ALLOC;
        }
    }
    
    // Delete the dictionary Controller if an error occurred
    if (localError != ARCONTROLLER_OK)
    {
        ARCONTROLLER_Dictionary_Delete (&dictionary);
    }
    // No else: skipped by an error 

    // Return the error
    if (error != NULL)
    {
        *error = localError;
    }
    // No else: error is not returned 

    return dictionary;
}

void ARCONTROLLER_Dictionary_Delete (ARCONTROLLER_Dictionary_t **dictionary)
{
    // -- Delete the dictionary Controller --
    
    // Local declarations

    if (dictionary != NULL)
    {
        if ((*dictionary) != NULL)
        {
            if ((*dictionary)->callbacks)
            {
                // Delete each element, use the safe iterator
                ARCONTROLLER_Dictionary_DeleteCallbackList(&((*dictionary)->callbacks));
                
            }
            
            free (*dictionary);
            (*dictionary) = NULL;
        }
    }
}

void ARCONTROLLER_Dictionary_DeleteCallbackList (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackList)
{
    // -- Delete all callback in a list --
    
    // Local declarations
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *element = NULL;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *elementTmp = NULL;

    // Delete each element, use the safe iterator
    DL_FOREACH_SAFE ((*callbackList), element, elementTmp)
    {
        DL_DELETE ((*callbackList), element);
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddCallback (ARCONTROLLER_Dictionary_t *element, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add a callback to use when the command is received --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t likeElement;
    
    // Check parameters
    if ((element == NULL) || (callback == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: the checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // Check if the callback is not already registered
        
        // Element to find
        likeElement.callback = callback;
        likeElement.customData = customData;
        
        DL_SEARCH (element->callbacks, elementFind, &likeElement, ARCONTROLLER_DICTIONARY_ElementCompare);
        
        if (elementFind != NULL)
        {
            error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_ALREADY_REGISTERED;
        }
    }
    
    if (error == ARCONTROLLER_OK)
    {
        // Add the callback
        error = ARCONTROLLER_Dictionary_AddCallbackInList (&(element->callbacks), callback, customData);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddCallbackInList (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackList, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add callback in array --
    
    // Local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *newElement = NULL;

    // Add the callback
    newElement = malloc (sizeof(ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t));
    if (newElement != NULL)
    {
        newElement->callback = callback;
        newElement->customData = customData;
        DL_APPEND ((*callbackList), newElement);
    }
    else
    {
        error = ARCONTROLLER_ERROR_ALLOC;
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveCallback (ARCONTROLLER_Dictionary_t *element, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove a callback to use when the command is received --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    // Check parameters
    if ((element == NULL) || (callback == NULL))
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: The checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Dictionary_RemoveCallbackFromList (&(element->callbacks), callback, customData);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveCallbackFromList (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackList, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove Callback From Array --
    
    // Local declarations
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *elementFind = NULL;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t likeElement;

    // Element to find
    likeElement.callback = callback;
    likeElement.customData = customData;
    
    DL_SEARCH ((*callbackList), elementFind, &likeElement, ARCONTROLLER_DICTIONARY_ElementCompare);
    if (elementFind != NULL)
    {
         DL_DELETE ((*callbackList), elementFind);
    }
    else
    {
        error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED;
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_AddDictionaryElement (ARCONTROLLER_Dictionary_t **dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Add a Command Dictionary Element --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Dictionary_t *dictElement = NULL;
    
    // Check parameters
    if (dictionary == NULL)
    {
        return ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: The checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // Find if the element already exist
        HASH_FIND_INT (*dictionary, &commandKey, dictElement);
        if (dictElement == NULL)
        {
            dictElement = ARCONTROLLER_Dictionary_New (commandKey, &error);
            
            if (error == ARCONTROLLER_OK)
            {
                HASH_ADD_INT (*dictionary, commandKey, dictElement);
            }
        }
        // NO ELSE ; Command already exist
    }
    
    if (error == ARCONTROLLER_OK)
    {
        error = ARCONTROLLER_Dictionary_AddCallback (dictElement, callback, customData);
    }
    
    return error;
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_RemoveDictionaryElement (ARCONTROLLER_Dictionary_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_CALLBACK_t callback, void *customData)
{
    // -- Remove a Command Dictionary Element --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    ARCONTROLLER_Dictionary_t *dictElement = NULL;
    
    // Check parameters
    if (dictionary == NULL)
    {
        error = ARCONTROLLER_ERROR_BAD_PARAMETER;
    }
    // No Else: The checking parameters sets error to ARNETWORK_ERROR_BAD_PARAMETER and stop the processing
    
    if (error == ARCONTROLLER_OK)
    {
        // Find if the element already exist
        HASH_FIND_INT (dictionary, &commandKey, dictElement);
        if (dictElement != NULL)
        {
            error = ARCONTROLLER_Dictionary_RemoveCallback (dictElement, callback, customData);
        }
        else
        {
            error = ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED;
        }
    }
    
    return error;
}

void ARCONTROLLER_Dictionary_DeleteDictionary (ARCONTROLLER_Dictionary_t **dictionary)
{
    // -- Delete a Command Dictionary --
    
    ARCONTROLLER_Dictionary_t *dictElement = NULL;
    ARCONTROLLER_Dictionary_t *dictTmp = NULL;
    
    if (dictionary != NULL)
    {
        if ((*dictionary) != NULL)
        {
            // Free the hash table contents
            HASH_ITER(hh, (*dictionary), dictElement, dictTmp)
            {
                // For each element of the commands dictionary 
                                
                HASH_DEL((*dictionary), dictElement);
                ARCONTROLLER_Dictionary_Delete (&dictElement);
            }
            
            free (*dictionary);
            (*dictionary) = NULL;
        }
    }
}

eARCONTROLLER_ERROR ARCONTROLLER_Dictionary_Notify (ARCONTROLLER_Dictionary_t *dictionary, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary)
{
    // -- Notify All Listeners --
    
    eARCONTROLLER_ERROR error = ARCONTROLLER_OK;
    
    ARCONTROLLER_Dictionary_t *commandCallbacks = NULL;
    
    if (error == ARCONTROLLER_OK)
    {
        // Find the command
        HASH_FIND_INT (dictionary, &commandKey, commandCallbacks);
        if (commandCallbacks != NULL)
        {
            ARCONTROLLER_DICTIONARY_NotifyAllCallbackInList (&(commandCallbacks->callbacks), commandKey, elementDictionary);
        }
        // NO Else ; No callback registered.
    }
    
    return error;
}

void ARCONTROLLER_DICTIONARY_NotifyAllCallbackInList (ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t **callbackList, eARCONTROLLER_DICTIONARY_KEY commandKey, ARCONTROLLER_DICTIONARY_ELEMENT_t *elementDictionary)
{
    // -- Notify All Listeners --
    
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *callbackElement = NULL;
    ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *callbackElementTmp = NULL;
    
    // For each callback
    DL_FOREACH_SAFE ((*callbackList), callbackElement, callbackElementTmp)
    {
        if (callbackElement->callback != NULL)
        {
            callbackElement->callback (commandKey, elementDictionary, callbackElement->customData);
        }
    }
}

 /*************************
 * Private Implementation
 *************************/

int ARCONTROLLER_DICTIONARY_ElementCompare(ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *a, ARCONTROLLER_DICTIONARY_CALLBAK_LIST_ELEMENT_t *b)
{
    return !((a->callback == b->callback) && (a->customData == b->customData));
}
