package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.ardiscovery.ARDISCOVERY_PRODUCT_ENUM;

public interface ARDeviceControllerListener
{
    /**
     * Called when deviceController state is changed
    **/
    public void onStateChanged (ARDeviceController deviceController, ARCONTROLLER_DEVICE_STATE_ENUM newState, ARCONTROLLER_ERROR_ENUM error);
    
    public void onExtensionStateChanged (ARDeviceController deviceController, ARCONTROLLER_DEVICE_STATE_ENUM newState, ARDISCOVERY_PRODUCT_ENUM product, String name, ARCONTROLLER_ERROR_ENUM error);

    public void onCommandReceived (ARDeviceController deviceController, ARCONTROLLER_DICTIONARY_KEY_ENUM commandKey, ARControllerDictionary elementDictionary);
}

