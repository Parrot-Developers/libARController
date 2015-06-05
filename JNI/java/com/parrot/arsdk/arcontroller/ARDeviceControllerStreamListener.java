package com.parrot.arsdk.arcontroller;

public interface ARDeviceControllerStreamListener
{
    public void onFrameReceived (ARDeviceController deviceController, ARFrame frame);

    public void onFrameTimeout (ARDeviceController deviceController);
}
