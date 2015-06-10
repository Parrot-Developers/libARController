package com.parrot.arsdk.arcontroller;

import android.support.annotation.NonNull;

import com.parrot.arsdk.arcontroller.ARCONTROLLER_ERROR_ENUM;
import com.parrot.arsdk.arcontroller.ARCONTROLLER_DICTIONARY_VALUE_TYPE_ENUM;
import com.parrot.arsdk.arsal.ARSALPrint;

import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.ArrayList;
import java.util.List;

public class ARControllerDictionary implements Map<String, ARControllerArgumentDictionary<Object>>
{
    private static String TAG = "ARControllerDictionary";
    
    public static String ARCONTROLLER_DICTIONARY_SINGLE_KEY = "";
    
    private static native String nativeStaticGetSingleKey ();

    private native int nativeGetSize (long jniDictionary);
    private native long nativeGetElement (long jniDictionary, String key);
    private native long[] nativeGetAllElements (long jniDictionary);
    
    private native long nativeGetArg (long jniDictionary, String key);
    private native int nativeGetArgType  (long jniArg);
    private native int nativeGetArgValueInt (long jniArg);
    private native long nativeGetArgValueLong (long jniArg);
    private native double nativeGetArgValueDouble (long jniArg);
    private native String nativeGetArgValueString (long jniArg);

    private long jniDictionary;
    private boolean initOk;
    
    static
    {
        ARCONTROLLER_DICTIONARY_SINGLE_KEY = nativeStaticGetSingleKey();
    }
    
    /**
     * Constructor
     */
    public ARControllerDictionary (long nativeDictionary)
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
                ret = (nativeGetElement(jniDictionary, stringKey) != 0);
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
    public Set<Entry<String, ARControllerArgumentDictionary<Object>>> entrySet()
    {
        return null;
    }

    @Override
    public ARControllerArgumentDictionary<Object> get(Object key)
    {
        ARControllerArgumentDictionary<Object> ret = null;
        synchronized (this)
        {
            if ((initOk == true) && (key instanceof String))
            {
                String stringKey = (String) key;

                long nativeElement = nativeGetElement (jniDictionary, stringKey);
                ret = new ARControllerArgumentDictionary<Object> (nativeElement);
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
    public ARControllerArgumentDictionary<Object> put(String key, ARControllerArgumentDictionary<Object> value)
    {
        return null;
    }

    @Override
    public void putAll(Map<? extends String, ? extends ARControllerArgumentDictionary<Object>> map)
    {

    }

    @Override
    public ARControllerArgumentDictionary<Object> remove(Object key)
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
    public Collection<ARControllerArgumentDictionary<Object>> values()
    {
        Collection<ARControllerArgumentDictionary<Object>> elements = null;
        
        synchronized (this)
        {
            if (initOk == true)
            {
                long nativeElements[] = nativeGetAllElements (jniDictionary);
                
                if (nativeElements != null)
                {
                    elements = new ArrayList<ARControllerArgumentDictionary<Object>> (nativeElements.length);
                    
                    for (long element : nativeElements)
                    {
                        elements.add(new ARControllerArgumentDictionary<Object> (element));
                    }
                }
            }
        }
        
        return elements;
    }
}
