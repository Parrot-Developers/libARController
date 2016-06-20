package com.parrot.arsdk.arcontroller;

import android.util.Log;

import com.parrot.arsdk.arcontroller.ARCONTROLLER_ERROR_ENUM;
import com.parrot.arsdk.arcontroller.ARCONTROLLER_STREAM_CODEC_TYPE_ENUM;
import com.parrot.arsdk.arsal.ARSALPrint;
import com.parrot.arsdk.arsal.ARNativeData;


public class ARControllerCodec
{
    private static String TAG = "ARControllerCodec";

    protected long jniCodec;
    protected boolean initOk;

    ARCONTROLLER_STREAM_CODEC_TYPE_ENUM type;

    /**
     * Constructor
     */
    public ARControllerCodec (ARCONTROLLER_STREAM_CODEC_TYPE_ENUM type)
    {
        this.type = type;
        initOk = true;
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
                jniCodec = 0;
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

    public ARCONTROLLER_STREAM_CODEC_TYPE_ENUM getType()
    {
        return type;
    }

    public H264 getAsH264()
    {
        if (type != ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_H264){
            throw new RuntimeException("Is not a ARCONTROLLER_STREAM_CODEC_TYPE_H264 codec (type = " + type + ")");
        }

        return (H264) this;
    }

    public Mjpeg getAsMJPEG()
    {
        if (type != ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG){
            throw new RuntimeException("Is not a ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG codec (type = " + type + ")");
        }

        return (Mjpeg) this;
    }

    public PCM16LE getAsPCM16LE()
    {
        if (type != ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE){
            throw new RuntimeException("Is not a ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE codec (type = " + type + ")");
        }

        return (PCM16LE) this;
    }

    public static class H264 extends ARControllerCodec
    {
        private ARNativeData sps;
        private ARNativeData pps;

        private H264(long nativeSps, int nativeSpsSize, long nativePps, int nativePpsSize)
        {
            super(ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_H264);

            sps = new ARNativeData(nativeSps, nativeSpsSize);
            sps.setUsedSize(nativeSpsSize);

            pps = new ARNativeData(nativePps, nativePpsSize);
            pps.setUsedSize(nativePpsSize);
        }

        public void dispose()
        {
            ARCONTROLLER_ERROR_ENUM error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_OK;
            synchronized (this)
            {
                if(initOk == true)
                {
                    sps.dispose();
                    sps = null;
                    pps.dispose();
                    pps = null;
                    super.dispose();
                }
            }
        }

        public ARNativeData getSps()
        {
            return sps;
        }

        public ARNativeData getPps()
        {
            return pps;
        }
    }

    public static class Mjpeg extends ARControllerCodec
    {
        public Mjpeg()
        {
            super(ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_MJPEG);
        }
    }

    public static class PCM16LE extends ARControllerCodec
    {
        private int sampleRate;
        private int channel;
        public static int CHANNEL_MONO = 0;
        public static int CHANNEL_STEREO = 1;

        public PCM16LE(int sampleRate, int channel)
        {
            super(ARCONTROLLER_STREAM_CODEC_TYPE_ENUM.ARCONTROLLER_STREAM_CODEC_TYPE_PCM16LE);
            this.sampleRate = sampleRate;
            this.channel = channel;
        }

        public int getSampleRate()
        {
            return sampleRate;
        }
        public int getChannel()
        {
            return channel;
        }
    }
}
