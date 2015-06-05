package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.ardiscovery.ARDiscoveryDevice;

import com.parrot.arsdk.arsal.ARNativeData;

import java.util.List;
import java.util.ArrayList;

public class ARDeviceController
{
    private static String TAG = "ARDeviceController";
    
    private static native void nativeStaticInit ();
    
    private native long nativeNew(long jdevice) throws ARControllerException;
    private native void nativeDelete(long jARDeviceController);
    
    private native int nativeStart (long jDeviceController);
    private native int nativeStop (long jDeviceController);
    
    private native long nativeGetFeatureARDrone3(long jDeviceController);
    private native int nativeSendPilotingTakeOffTest (long jDeviceController);

    private long jniDeviceController;
    private boolean initOk;
    
    private List<ARDeviceControllerListener> listeners;
    private List<ARDeviceControllerStreamListener> streamlisteners;
    ARFeatureARDrone3 featureARDrone3;
    
    static
    {
        nativeStaticInit();
    }

    /**
     * Constructor
     */
    public ARDeviceController (ARDiscoveryDevice device) throws ARControllerException
    {
        initOk = false;
        
        if (device != null)
        {
            jniDeviceController = nativeNew(device.getNativeDevice());
        }
        
        if (jniDeviceController != 0)
        {
            listeners = new ArrayList<ARDeviceControllerListener>();
            streamlisteners = new ArrayList<ARDeviceControllerStreamListener>();
            initOk = true;
            
            long nativeFeatureARDrone3 = nativeGetFeatureARDrone3 (jniDeviceController);
            if (nativeFeatureARDrone3 != 0)
            {
                featureARDrone3 = new ARFeatureARDrone3(nativeFeatureARDrone3);
            }
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
                nativeDelete(jniDeviceController);
                jniDeviceController = 0;
                initOk = false;
                
                if(featureARDrone3 != null)
                {
                    featureARDrone3.dispose();
                    featureARDrone3 = null;
                }
            }
            else
            {
                error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_ERROR_JNI_INIT;
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
    
    public ARCONTROLLER_ERROR_ENUM start ()
    {
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeStart(jniDeviceController);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
            else
            {
                error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_ERROR_JNI_INIT;
            }
        }
        
        return error;
    }
    
    public ARCONTROLLER_ERROR_ENUM stop ()
    {
        ARSALPrint.d(TAG,"stop ...");
        ARSALPrint.wtf(TAG,"stop ... jniDeviceController: " + jniDeviceController);
        
        ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
        synchronized (this)
        {
            if(initOk == true)
            {
                int nativeError = nativeStop(jniDeviceController);
                error = ARCONTROLLER_ERROR_ENUM.getFromValue(nativeError);
            }
            else
            {
                error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_ERROR_JNI_INIT;
            }
        }
        
        return error;
    }
    
    public ARFeatureARDrone3 getFeatureARDrone3 ()
    {
        return featureARDrone3;
    }
    
    public synchronized void addListener (ARDeviceControllerListener listener)
    {
       if (! listeners.contains(listener))
       {
           listeners.add (listener);
       }
    }

    public synchronized void removeListener (ARDeviceControllerListener listener)
    {
        listeners.remove (listener);
    }
    
    public synchronized void addStreamListener (ARDeviceControllerStreamListener listener)
    {
       if (! streamlisteners.contains(listener))
       {
           streamlisteners.add (listener);
       }
    }

    public synchronized void removeStreamListener (ARDeviceControllerStreamListener listener)
    {
        streamlisteners.remove (listener);
    }
    
    private void onStateChanged (int newState, int error)
    {
        for (ARDeviceControllerListener l : listeners)
        {
            l.onStateChanged (this, ARCONTROLLER_DEVICE_STATE_ENUM.getFromValue(newState), ARCONTROLLER_ERROR_ENUM.getFromValue(error));
        }
    }
    
    private void onCommandReceived (int nativeCommandKey, long elementDictionary)
    {
        ARCONTROLLER_DICTIONARY_KEY_ENUM commandKey = ARCONTROLLER_DICTIONARY_KEY_ENUM.getFromValue(nativeCommandKey);
        
        ARControllerDictionary dictionary = new ARControllerDictionary(elementDictionary);
        
        for (ARDeviceControllerListener l : listeners)
        {
            l.onCommandReceived (this, ARCONTROLLER_DICTIONARY_KEY_ENUM.getFromValue(nativeCommandKey), dictionary);
        }
    }
    
    private void  didReceiveFrameCallback (long data, int dataSize)
    {
        //ARSALPrint.d(TAG,"didReceiveFrameCallback ... data: " + data + " dataSize:" + dataSize);
        
        ARNativeData frame = new ARNativeData (data, dataSize);
        
        //ARSALPrint.d(TAG,"frame : " + frame);
        
        for (ARDeviceControllerStreamListener l : streamlisteners)
        {
            l.onFrameReceived (this, frame);
        }
    }

    private void  timeoutFrameCallback ()
    {
        //ARSALPrint.d(TAG,"timeoutFrameCallback ... ");
        
        for (ARDeviceControllerStreamListener l : streamlisteners)
        {
            l.onFrameTimeout (this);
        }
    }
}
