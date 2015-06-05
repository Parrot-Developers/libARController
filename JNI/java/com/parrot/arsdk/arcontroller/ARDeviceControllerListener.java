package com.parrot.arsdk.arcontroller;

public interface ARDeviceControllerListener
{
    /**
     * Called when deviceController state is changed
    **/
    public void onStateChanged (ARDeviceController deviceController, ARCONTROLLER_DEVICE_STATE_ENUM newState, ARCONTROLLER_ERROR_ENUM error);

    public void onCommandReceived (ARDeviceController deviceController, ARCONTROLLER_DICTIONARY_KEY_ENUM commandKey, ARControllerDictionary elementDictionary);
}

