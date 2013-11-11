/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Takes control over the EP0 vendor specific messages, and process requests
*
*/


#ifndef CUSBDEVCON_H
#define CUSBDEVCON_H

#include <e32std.h>
#include <e32base.h>
#include <d32usbc.h>
#include <usbman.h>
#include <usbwatcher.h>

class CUsbStateWatcher;     // watches device's USB states
class CStateMachine;        // state-machine
class CRequestsHandler;     // contains actual requests handlers

static const TUint KSetupPacketLength = 8; // 8 bytes, always
static const TUint KInactiveTimeForShutDown = 10000000; // microseconds

/**
 *  USB Device Controller
 *  Contains all needed subclasses, to 
 *  take control over EP0 vendor specific messages
 *  and processing requests
 *
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CUsbDevCon : public CActive  
    {

public:
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @return Constructed instance
     */ 
    static CUsbDevCon* NewL();
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @return Constructed instance
     */ 
    static CUsbDevCon* NewLC();
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CUsbDevCon();
    
    /**
     * Performs action needed by specific usb state
     *
     * @since S60 v.5.0
     * @param aUsbState USB state
     */
    void ActAccordinglyToUsbStateL(TUsbcDeviceState aUsbState);
    
private:

    /**
     * Default construction
     *
     * @since S60 v.5.0
     */
    CUsbDevCon();
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     */
    void ConstructL();
    
    /**
     * Take needed actions on starting services
     *
     * @since S60 v.5.0
     */
    void StartL();
    
    /**
     * Take needed actions on stopping services
     *
     * @since S60 v.5.0
     */
    void StopL();
    
    // from CActive
    
    /**
     * From CActive
     *
     */
    void RunL();
    
    /**
     * From CActive
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
     * USB state watcher
     * Own
     */
    CUsbStateWatcher* iUsbStateWatcher;

    /**
     * CStateMachine
     * Own
     */
    CStateMachine* iStateMachine;
    
    /**
     * Requests handler 
     * Own  
     */
    CRequestsHandler* iRequestsHandler;
    
    /**
     * USB client
     * Need to take control over EP0 
     * Also reference to it will be provided to handlers
     */
    RDevUsbcClient iLdd;
    
    /**
     * USB Watcher
     * Need to handle some requests (like GetPersonalities)
     * Reference to it will be provided to handlers
     */
    RUsbWatcher iUsbWatcher;
    
    /**
     * USB Manager
     * Need to handle some requests (like SetPersonality)
     * Reference to it will be provided to handlers
     */
    RUsb iUsbManager;
    
    /**
     * Previous USB state 
     */
    TInt iPrevUsbState;
    
    /**
     * Shuts down if USB inactive for specified period of time
     */
    RTimer iShutdownTimer;

    };

#endif // USBDEVCON_H
