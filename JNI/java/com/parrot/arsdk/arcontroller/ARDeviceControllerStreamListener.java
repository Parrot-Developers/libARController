package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARNativeData;

public interface ARDeviceControllerStreamListener
{
    public void onFrameReceived (ARDeviceController deviceController, ARNativeData frame);

    public void onFrameTimeout (ARDeviceController deviceController);
}

