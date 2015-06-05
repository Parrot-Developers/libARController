package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.ardiscovery.ARDiscoveryDevice;

import java.util.List;
import java.util.ArrayList;

public class ARFeatureARDrone3
{
    private static String TAG = "ARFeatureARDrone3";
    
    private native int nativeSendCameraOrientation (long jFeatureARDrone3, int tilt, int pan);
    private native int nativeSendPilotingTakeOff (long jFeatureARDrone3);
    private native int nativeSendPilotingLanding (long jFeatureARDrone3);
    private native int nativeSendPilotingEmergency (long jFeatureARDrone3);
    private native int nativeSendMediaStreamingVideoEnable (long jFeatureARDrone3, int enable);

    private long jniFeatureARDrone3;
    private boolean initOk;
    
    /**
     * Constructor
     */
    public ARFeatureARDrone3 (long nativeFeatureARDrone3)
    {
        initOk = false;
        
        if (nativeFeatureARDrone3 != 0)
        {
            jniFeatureARDrone3 = nativeFeatureARDrone3;
            initOk = true;
        }
    }

    /**
     * Dispose
     */
    public void dispose()
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                jniFeatureARDrone3 = 0;
                initOk = false;
            }
        }
    }

    /**
     * Destructor
     */
    public void finalize () throws Throwable
    {
        try
        {
            dispose ();
        }
        finally
        {
            super.finalize ();
        }
    }
    
    public ARCONTROLLER_ERROR_ENUM sendCameraOrientation (int tilt, int pan)
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeSendCameraOrientation (jniFeatureARDrone3, tilt, pan);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
        }
        return error;
    }
    
    public ARCONTROLLER_ERROR_ENUM sendPilotingTakeOff ()
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeSendPilotingTakeOff (jniFeatureARDrone3);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
        }
        
        return error;
    }
    
    public ARCONTROLLER_ERROR_ENUM sendPilotingLanding ()
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeSendPilotingLanding (jniFeatureARDrone3);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
        }
        
        return error;
    }
    
    public ARCONTROLLER_ERROR_ENUM sendPilotingEmergency ()
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeSendPilotingEmergency (jniFeatureARDrone3);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
        }
        
        return error;
    }
    
    public ARCONTROLLER_ERROR_ENUM sendMediaStreamingVideoEnable (boolean enable)
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeEnable = (enable) ? 1 : 0;
                int nativeError = nativeSendMediaStreamingVideoEnable (jniFeatureARDrone3, nativeEnable);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
        }
        
        return error;
    }

}
