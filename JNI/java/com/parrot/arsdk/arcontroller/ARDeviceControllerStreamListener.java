package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARNativeData;

public interface ARDeviceControllerStreamListener
{
    public ARCONTROLLER_ERROR_ENUM configureDecoder (ARDeviceController deviceController, ARControllerCodec codec);
    
    public ARCONTROLLER_ERROR_ENUM onFrameReceived (ARDeviceController deviceController, ARFrame frame);

    public void onFrameTimeout (ARDeviceController deviceController);
}
