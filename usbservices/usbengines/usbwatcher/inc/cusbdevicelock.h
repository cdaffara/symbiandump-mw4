/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Header file for class CUsbDeviceLock.
*
*/


#ifndef CUSBDEVICELOCK_H
#define CUSBDEVICELOCK_H

class CUsbWatcher;

/**
 *  Class for device lock notification
 *
 *  @lib euser.lib usbman.lib SysUtil.lib
 *  @since Series 60 2.?
 */
NONSHARABLE_CLASS( CUsbDeviceLock ) : public CActive
    {

public:

    static CUsbDeviceLock* NewL( CUsbWatcher& aOwner );

    virtual ~CUsbDeviceLock();

public:
    /**
     * Subscribes device lock change event.
     *
     * @since Series 60 3.0
     */    
    void Subscribe();
    
    /**
     * Current state of device lock.
     *
     * @since Series 60 3.0
     * @return When set (ETrue) device is locked. When unset (EFalse) device is unlocked.
     */
    TBool Locked();
    
public: // from base class CActive
    
    /**
     * From CActive.
     * This method is never called in this implementation.
     *
     * @since Series 60 3.0
     */
    TInt RunError( TInt /*aError*/ );

    /**
     * From CActive.
     * This method is called when device lock state has changed. 
     * New state is given to owner class.
     *
     * @since Series 60 3.0
     */
    void RunL();

    /**
     * From CActive.
     * Will cancel outstanding request if called.
     *
     * @since Series 60 3.0
     */
    void DoCancel();
      
private:

    /**
     * C++ constructor.
     */
    CUsbDeviceLock( CUsbWatcher& aOwner );

    CUsbDeviceLock();

    void ConstructL();
      

private:
    /**
     * Handle to P&S key
     */
    RProperty  iProperty;
    
    /**
     * Owner of this class
     */
    CUsbWatcher& iOwner;
};

#endif // CUSBDEVICELOCK_H

// End of file

