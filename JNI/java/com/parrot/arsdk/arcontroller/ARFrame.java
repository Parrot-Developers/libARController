package com.parrot.arsdk.arcontroller;

import com.parrot.arsdk.arsal.ARNativeData;

public class ARFrame extends ARNativeData
{
    private final static int ARFRAME_DEFAULT_CAPACITY = 60000;

    private int missed;
    private boolean isIFrame;
    private boolean available;
    
    public ARFrame ()
    {
        super(ARFRAME_DEFAULT_CAPACITY);
        initialize ();
    }
    
    public ARFrame (int defaultCapacity)
    {
        super(defaultCapacity);
        initialize ();
    }
    
    public ARFrame (long data, int dataCapacity, int dataSize, boolean isIFrame, int missed)
    {
        super(data, dataCapacity);
        initialize ();
        setUsedSize(dataSize);
        this.missed = missed;
        this.isIFrame = isIFrame;
    }
    
    private void initialize ()
    {
        this.missed = 0;
        this.isIFrame = false;
        this.available = true;
    }
    
    public int getMissed()
    {
        return missed;
    }

    public void setMissed (int missed)
    {
        this.missed = missed;
    }

    public boolean isIFrame()
    {
        return isIFrame;
    }

    public void setIFrame (boolean isIFrame)
    {
        this.isIFrame = isIFrame;
    }

    public boolean isAvailable()
    {
        return available;
    }

    public void setAvailable (boolean available)
    {
        this.available = available;
    }
}
