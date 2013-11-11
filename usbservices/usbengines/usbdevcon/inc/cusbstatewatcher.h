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
* Description:  Watches USB states
*
*/


#ifndef CUSBSTATEWATCHER_H
#define CUSBSTATEWATCHER_H

#include <e32std.h>
#include <d32usbc.h>

#include "cusbdevcon.h"

/**
 *  Usb State watcher
 *  Reports Usb state changes to CUsbDevCon
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CUsbStateWatcher : public CActive
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @param aObserver Gets notifications when USB state changed
     * @param aLdd Link to LDD services
     * @return Constructed instance
     */ 
    static CUsbStateWatcher* NewL(CUsbDevCon& aObserver, RDevUsbcClient& aLdd);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CUsbStateWatcher();
    
    /**
     * Activates USB states watching
     *
     * @since S60 v.5.0
     */
    void Activate();
        
private:
    
    /**
     * Default construction
     *
     * @since S60 v.5.0
     * @param aObserver Gets notifications when USB state changed
     * @param aLdd Link to LDD services
     */
    CUsbStateWatcher(CUsbDevCon& aObserver, RDevUsbcClient& aLdd);
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */
    void ConstructL();
    
    // from CActive
    
    /**
     * From CActive.
     *
     */
    void RunL();
    
    /**
     * From CActive.
     *
     */ 
    void DoCancel();
    
     /**
     * From CActive.
     *
     */ 
     TInt RunError( TInt /*aError*/ );

private: // data
    
    /**
     * Device state
     */
    TUint iState;
        
    /**
     * Observer
     * Not own.  
     */
    CUsbDevCon& iObserver;
    
    /**
     * LDD
     * Not own.  
     */
    RDevUsbcClient& iLdd;
    
    };

#endif // CUSBSTATEWATCHER_H
