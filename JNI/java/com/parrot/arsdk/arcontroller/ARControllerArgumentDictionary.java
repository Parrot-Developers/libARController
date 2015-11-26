package com.parrot.arsdk.arcontroller;

import android.support.annotation.NonNull;

import com.parrot.arsdk.arcontroller.ARCONTROLLER_ERROR_ENUM;
import com.parrot.arsdk.arcontroller.ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM;
import com.parrot.arsdk.arsal.ARSALPrint;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

public class ARControllerArgumentDictionary<V> implements Map<String, V> /*Parcelable, Cloneable*/
{
    private static String TAG = "ARControllerArgumentDictionary";
    
    private static native void nativeStaticInit ();

    private native int nativeGetSize (long jniDictionary);
    //private native long nativeGetElement (long jniDictionary, String key);
    
    private native long nativeGetArg (long jniDictionary, String key);
    private native int nativeGetArgType  (long jniArg);
    private native int nativeGetArgValueInt (long jniArg);
    private native long nativeGetArgValueLong (long jniArg);
    private native double nativeGetArgValueDouble (long jniArg);
    private native String nativeGetArgValueString (long jniArg);

    private long jniDictionary;
    private boolean initOk;
    
    /**
     * Constructor
     */
    public ARControllerArgumentDictionary (long nativeDictionary)
    {
        initOk = false;
        
        if (nativeDictionary != 0)
        {
            jniDictionary = nativeDictionary;
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
                jniDictionary = 0;
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

    @Override
    public void clear()
    {
        synchronized (this)
        {
            if (initOk == true)
            {
                //nativeClear();
            }
        }
    }

    @Override
    public boolean containsKey(Object key)
    {
        boolean ret = false;

        synchronized (this)
        {
            if ((initOk == true) && (key instanceof String))
            {
                String stringKey = (String) key;
                ret = (nativeGetArg(jniDictionary, stringKey) != 0);
            }
            // No Else; The dictionary does not contain the key
        }

        return ret;
    }

    @Override
    public boolean containsValue(Object value)
    {
        int nativeReturn = 0;

        synchronized (this)
        {
            if ((initOk == true) && (value instanceof String))
            {
                //nativeReturn = nativeContainsValue();
            }
        }

        return (nativeReturn != 0);
    }

    @NonNull
    @Override
    public Set<Entry<String, V>> entrySet()
    {
        return null;
    }

    @Override
    public V get(Object key)
    {
        V ret = null;
        synchronized (this)
        {
            if ((initOk == true) && (key instanceof String))
            {
                String stringKey = (String) key;
                
                long nativeArg = nativeGetArg (jniDictionary, stringKey);
                if (nativeArg != 0)
                {
                    int nativeType = nativeGetArgType (nativeArg);
                    
                    switch (ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM.getFromValue(nativeType))
                    {
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U8:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I8:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U16:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I16:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U32:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I32:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM:
                            ret = (V) new Integer(nativeGetArgValueInt (nativeArg));
                            break;
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_U64:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_I64:
                            ret = (V) new Long(nativeGetArgValueLong (nativeArg));
                            break;
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_FLOAT:
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_DOUBLE:
                            ret = (V) new Double(nativeGetArgValueDouble (nativeArg));
                            break;
                        
                        case ARCONTROLLER_DICTIONARY_VALUE_TYPE_STRING:
                            ret = (V) nativeGetArgValueString (nativeArg);
                            break;
                            
                        default:
                            ARSALPrint.e(TAG,"valueType "+ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM.getFromValue(nativeType)+" not known");
                            break;
                    }
                }
            }
        }

        return ret;
    }

    @Override
    public boolean isEmpty()
    {
        boolean ret = true;
        synchronized (this)
        {
            if (initOk == true)
            {
                ret = (nativeGetSize(jniDictionary) == 0);
            }
        }

        return ret;
    }

    @NonNull
    @Override
    public Set<String> keySet()
    {
        return null;
    }

    @Override
    public V put(String key, V value)
    {
        return null;
    }

    @Override
    public void putAll(Map<? extends String, ? extends V> map)
    {

    }

    @Override
    public V remove(Object key)
    {
        return null;
    }

    @Override
    public int size()
    {
        int size = 0;
        synchronized (this)
        {
            if (initOk == true)
            {
                size = nativeGetSize(jniDictionary);
            }
        }

        return size;
    }

    @NonNull
    @Override
    public Collection<V> values()
    {
        return null;
    }
}
