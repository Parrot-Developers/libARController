package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARNativeData;

public interface ARDeviceControllerStreamListener
{
    public void onSpsPpsReceived (ARDeviceController deviceController, ARNativeData sps, ARNativeData pps);
    
    public void onFrameReceived (ARDeviceController deviceController, ARFrame frame);

    public void onFrameTimeout (ARDeviceController deviceController);
}
