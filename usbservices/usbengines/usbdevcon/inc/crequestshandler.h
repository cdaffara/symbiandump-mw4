/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles requests
*
*/


#ifndef CREQUESTSHANDLER_H
#define CREQUESTSHANDLER_H

#include <e32base.h>

class CUsbCMHandler;

/**
 *  Requests handler
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CRequestsHandler : public CBase
    {

public:

     /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @param aLdd Link to LDD services
     * @param aUsbWatcher Link to USb Watcher services
     * @param aUsbManager Link to USB Manager services
     * @return Constructed instance
     */ 
    static CRequestsHandler* NewL(RDevUsbcClient& aLdd, RUsbWatcher& aUsbWatcher, RUsb& aUsbManager);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CRequestsHandler();
    
    /**
     * Handler
     *
     * @since S60 v.5.0
     * @param aSetupPacket Request to be handled
     * @param aUsbManager Link to UsbManager services
     */
    TInt Handle(const RBuf8& aSetupPacket, RBuf8& aData);
    
private:
    
    /**
     * Default construction
     *
     * @since S60 v.5.0
     * @param aLdd Link to LDD services
     * @param aUsbWatcher Link to USB Watcher services
     * @param aUsbManager Link to USB Manager services
     */
    CRequestsHandler(RDevUsbcClient& aLdd, RUsbWatcher& aUsbWatcher, RUsb& aUsbManager);
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */ 
    void ConstructL();

private: // data
    
    /**
     * Request handlers
     * Own.  
     */
    RPointerArray<CUsbCMHandler> iHandlers;
    
    /**
     * LDD
     * Not own.  
     */
    RDevUsbcClient& iLdd;
    
    /**
     * USB Watcher
     * Not own.  
     */
    RUsbWatcher& iUsbWatcher;
    
    /**
     * Usb manager
     * Not own
     */
    RUsb& iUsbManager;
        
    };

#endif // CREQUESTSHANDLER_H
