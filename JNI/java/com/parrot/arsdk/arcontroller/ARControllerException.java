package com.parrot.arsdk.arcontroller;

public class ARControllerException extends Exception 
{
    private ARCONTROLLER_ERROR_ENUM error;
    
    public ARControllerException()
    {
        error = ARCONTROLLER_ERROR_ENUM.ARCONTROLLER_ERROR;
    }
    
    public ARControllerException (ARCONTROLLER_ERROR_ENUM error) 
    {
        this.error = error;
    }
    
    public ARControllerException (int error) 
    {
        this.error = ARCONTROLLER_ERROR_ENUM.getFromValue(error);
    }
    
    public ARCONTROLLER_ERROR_ENUM getError()
    {
        return error;
    }
    
    public void setError(ARCONTROLLER_ERROR_ENUM error)
    {
        this.error = error;
    }
    
}

