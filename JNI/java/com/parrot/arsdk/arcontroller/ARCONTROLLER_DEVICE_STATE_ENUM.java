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
 * Java copy of the eARCONTROLLER_DEVICE_STATE enum
 */
public enum ARCONTROLLER_DEVICE_STATE_ENUM {
   /** Dummy value for all unknown cases */
    eARCONTROLLER_DEVICE_STATE_UNKNOWN_ENUM_VALUE (Integer.MIN_VALUE, "Dummy value for all unknown cases"),
   /** device controller is stopped */
    ARCONTROLLER_DEVICE_STATE_STOPPED (0, "device controller is stopped"),
   /** device controller is starting */
    ARCONTROLLER_DEVICE_STATE_STARTING (1, "device controller is starting"),
   /** device controller is running */
    ARCONTROLLER_DEVICE_STATE_RUNNING (2, "device controller is running"),
   /** device controller is paused */
    ARCONTROLLER_DEVICE_STATE_PAUSED (3, "device controller is paused"),
   /** device controller is stopping */
    ARCONTROLLER_DEVICE_STATE_STOPPING (4, "device controller is stopping"),
   /** Max of the enumeration */
    ARCONTROLLER_DEVICE_STATE_MAX (5, "Max of the enumeration");

    private final int value;
    private final String comment;
    static HashMap<Integer, ARCONTROLLER_DEVICE_STATE_ENUM> valuesList;

    ARCONTROLLER_DEVICE_STATE_ENUM (int value) {
        this.value = value;
        this.comment = null;
    }

    ARCONTROLLER_DEVICE_STATE_ENUM (int value, String comment) {
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
     * Gets the ARCONTROLLER_DEVICE_STATE_ENUM instance from a C enum value
     * @param value C value of the enum
     * @return The ARCONTROLLER_DEVICE_STATE_ENUM instance, or null if the C enum value was not valid
     */
    public static ARCONTROLLER_DEVICE_STATE_ENUM getFromValue (int value) {
        if (null == valuesList) {
            ARCONTROLLER_DEVICE_STATE_ENUM [] valuesArray = ARCONTROLLER_DEVICE_STATE_ENUM.values ();
            valuesList = new HashMap<Integer, ARCONTROLLER_DEVICE_STATE_ENUM> (valuesArray.length);
            for (ARCONTROLLER_DEVICE_STATE_ENUM entry : valuesArray) {
                valuesList.put (entry.getValue (), entry);
            }
        }
        ARCONTROLLER_DEVICE_STATE_ENUM retVal = valuesList.get (value);
        if (retVal == null) {
            retVal = eARCONTROLLER_DEVICE_STATE_UNKNOWN_ENUM_VALUE;
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
