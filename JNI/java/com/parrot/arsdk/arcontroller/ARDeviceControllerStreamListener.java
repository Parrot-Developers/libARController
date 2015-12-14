package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARNativeData;

public interface ARDeviceControllerStreamListener
{
    public void onConfigDecoder (ARDeviceController deviceController, ARControllerCodec codec);
    
    public void onFrameReceived (ARDeviceController deviceController, ARFrame frame);

    public void onFrameTimeout (ARDeviceController deviceController);
}
