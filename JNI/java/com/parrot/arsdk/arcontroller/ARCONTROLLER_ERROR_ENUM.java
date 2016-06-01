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

package com.parrot.arsdk.arcontroller;

import java.util.HashMap;

/**
 * Java copy of the eARCONTROLLER_ERROR enum
 */
public enum ARCONTROLLER_ERROR_ENUM {
   /** Dummy value for all unknown cases */
    eARCONTROLLER_ERROR_UNKNOWN_ENUM_VALUE (Integer.MIN_VALUE, "Dummy value for all unknown cases"),
   /** No error */
    ARCONTROLLER_OK (0, "No error"),
   /** Unknown generic error */
    ARCONTROLLER_ERROR (-1000, "Unknown generic error"),
   /** Memory allocation error */
    ARCONTROLLER_ERROR_ALLOC (-999, "Memory allocation error"),
   /** Bad parameters */
    ARCONTROLLER_ERROR_BAD_PARAMETER (-998, "Bad parameters"),
   /** Mutex lock or unlock error */
    ARCONTROLLER_ERROR_MUTEX (-997, "Mutex lock or unlock error"),
   /** Mutex initialization error */
    ARCONTROLLER_ERROR_INIT_MUTEX (-996, "Mutex initialization error"),
   /** Bad state of the Network Controller to call this function */
    ARCONTROLLER_ERROR_STATE (-995, "Bad state of the Network Controller to call this function"),
   /** Buffer is too small */
    ARCONTROLLER_ERROR_BUFFER_SIZE (-994, "Buffer is too small"),
   /** Error of initialization */
    ARCONTROLLER_ERROR_INIT (-2000, "Error of initialization"),
   /** Thread initialization error */
    ARCONTROLLER_ERROR_INIT_THREAD (-1999, "Thread initialization error"),
   /** Error during the getting of the ARNETWORKAL_Manager from the device */
    ARCONTROLLER_ERROR_INIT_ARNETWORKAL_MANAGER (-1998, "Error during the getting of the ARNETWORKAL_Manager from the device"),
   /** Error Initialization of the ARNETWORK_Manager */
    ARCONTROLLER_ERROR_INIT_ARNETWORK_MANAGER (-1997, "Error Initialization of the ARNETWORK_Manager"),
   /** Error during the getting of the ARNetWork Configuration from the device */
    ARCONTROLLER_ERROR_INIT_NETWORK_CONFIG (-1996, "Error during the getting of the ARNetWork Configuration from the device"),
   /** Error during the copy of the device */
    ARCONTROLLER_ERROR_INIT_DEVICE_COPY (-1995, "Error during the copy of the device"),
   /** Error during the get of the networkConfiguration from the device */
    ARCONTROLLER_ERROR_INIT_DEVICE_GET_NETWORK_CONFIG (-1994, "Error during the get of the networkConfiguration from the device"),
   /** Error during the add of json callback to the device */
    ARCONTROLLER_ERROR_INIT_DEVICE_JSON_CALLBACK (-1993, "Error during the add of json callback to the device"),
   /** Error during the get of the current date */
    ARCONTROLLER_ERROR_INIT_GET_DATE (-1992, "Error during the get of the current date"),
   /** Error during the get of the current time */
    ARCONTROLLER_ERROR_INIT_GET_TIME (-1991, "Error during the get of the current time"),
   /** Error during the initialization of a stream */
    ARCONTROLLER_ERROR_INIT_STREAM (-1990, "Error during the initialization of a stream"),
   /** Error during the initialization of a semaphore */
    ARCONTROLLER_ERROR_INIT_SEMAPHORE (-1989, "Error during the initialization of a semaphore"),
   /** Error data not sent */
    ARCONTROLLER_ERROR_NOT_SENT (-1988, "Error data not sent"),
   /** Error the device has no video */
    ARCONTROLLER_ERROR_NO_VIDEO (-1987, "Error the device has no video"),
   /** No element saved for this command */
    ARCONTROLLER_ERROR_NO_ELEMENT (-1986, "No element saved for this command"),
   /** No argument saved for this command */
    ARCONTROLLER_ERROR_NO_ARGUMENTS (-1985, "No argument saved for this command"),
   /** start canceled */
    ARCONTROLLER_ERROR_CANCELED (-1984, "start canceled"),
   /** Error of command generating */
    ARCONTROLLER_ERROR_COMMAND_GENERATING (-1983, "Error of command generating"),
   /** Error of command generating */
    ARCONTROLLER_ERROR_COMMAND_CALLBACK (-3000, "Error of command generating"),
   /** the command callback is already registered */
    ARCONTROLLER_ERROR_COMMAND_CALLBACK_ALREADY_REGISTERED (-2999, "the command callback is already registered"),
   /** the command callback is not registred */
    ARCONTROLLER_ERROR_COMMAND_CALLBACK_NOT_REGISTERED (-2998, "the command callback is not registred"),
   /** Generic stream pool error */
    ARCONTROLLER_ERROR_STREAMPOOL (-4000, "Generic stream pool error"),
   /** no frame found */
    ARCONTROLLER_ERROR_STREAMPOOL_FRAME_NOT_FOUND (-3999, "no frame found"),
   /** Generic stream queue error */
    ARCONTROLLER_ERROR_STREAMQUEUE (-5000, "Generic stream queue error"),
   /** Error stream queue empty */
    ARCONTROLLER_ERROR_STREAMQUEUE_EMPTY (-4999, "Error stream queue empty"),
   /** Generic JNI error */
    ARCONTROLLER_ERROR_JNI (-6000, "Generic JNI error"),
   /** Error of JNI environment */
    ARCONTROLLER_ERROR_JNI_ENV (-5999, "Error of JNI environment"),
   /** Native part not initialized */
    ARCONTROLLER_ERROR_JNI_INIT (-5998, "Native part not initialized"),
   /** Generic extension related error */
    ARCONTROLLER_ERROR_EXTENSION (-7000, "Generic extension related error"),
   /** Product not valid to be an extension */
    ARCONTROLLER_ERROR_EXTENSION_PRODUCT_NOT_VALID (-6999, "Product not valid to be an extension"),
   /** Generic stream error */
    ARCONTROLLER_ERROR_STREAM (-8000, "Generic stream error"),
   /** Stream re-synchronization required */
    ARCONTROLLER_ERROR_STREAM_RESYNC_REQUIRED (-7999, "Stream re-synchronization required");

    private final int value;
    private final String comment;
    static HashMap<Integer, ARCONTROLLER_ERROR_ENUM> valuesList;

    ARCONTROLLER_ERROR_ENUM (int value) {
        this.value = value;
        this.comment = null;
    }

    ARCONTROLLER_ERROR_ENUM (int value, String comment) {
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
     * Gets the ARCONTROLLER_ERROR_ENUM instance from a C enum value
     * @param value C value of the enum
     * @return The ARCONTROLLER_ERROR_ENUM instance, or null if the C enum value was not valid
     */
    public static ARCONTROLLER_ERROR_ENUM getFromValue (int value) {
        if (null == valuesList) {
            ARCONTROLLER_ERROR_ENUM [] valuesArray = ARCONTROLLER_ERROR_ENUM.values ();
            valuesList = new HashMap<Integer, ARCONTROLLER_ERROR_ENUM> (valuesArray.length);
            for (ARCONTROLLER_ERROR_ENUM entry : valuesArray) {
                valuesList.put (entry.getValue (), entry);
            }
        }
        ARCONTROLLER_ERROR_ENUM retVal = valuesList.get (value);
        if (retVal == null) {
            retVal = eARCONTROLLER_ERROR_UNKNOWN_ENUM_VALUE;
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
