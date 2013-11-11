/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Subscribes for IP events from ipeventnotifier module
*
*/




#ifndef C_UPNPIPEVENTNOTIFIER_H
#define C_UPNPIPEVENTNOTIFIER_H

#include <e32base.h>
#include <e32property.h>
#include <in_sock.h>

/**
 *  Observer class for getting information about IP events
 *
 *  @since S60 v3.1
 */
class MUpnpIPEventObserver
    {
public:
    /**
     * This function will be invoke if IPEvent occurs (IPAddressReady)
     */
    virtual void IPEventOccursL() = 0;
    };


/**
 *  Class responsible for subscribe for IP event from ipeventnotifier module.
 *  We register to get notification about IPEvent::EIPReady 
 *  for specific interface index 
 *  (it has to be specific during constrution of this class).
 *  To see more detail infromation please check:
 *  \src\common\generic\networking\ipeventnotifier
 *
 *  @since S60 v3.1
 */
class CUpnpIPEventNotifier : public CActive
    {
public:

    /**
     * Create a CUpnpIPEventNotifier object using two phase construction,
     * and return a pointer to the created object
     * @param aInterfaceIndex Interface index
     * @result pointer to new event notifier
     */
    static CUpnpIPEventNotifier* NewL( TInt aInterfaceIndex );

    /**
     * Destroy the object and release all memory objects
     */
    virtual ~CUpnpIPEventNotifier();      

    /**
     * Subscribes for IP events
     * @param observer for notification
     */
    void Subscribe( MUpnpIPEventObserver* aObserver );

private:

    /**
     * Simply constructor
     */
    CUpnpIPEventNotifier();

    /**
     * 2nd constructor
     * @param interface index
     */
    void ConstructL( TInt aInterfaceIndex );

private:  // from CActive

    /**
     * Invoke when asynchronous request will be completed
     * @since S60 S60 v3.1
     */
    void RunL();

    /**
     * Invoke when asynchronous RunL leaves
     */
    TInt RunError( TInt aErr );
    
    /**
     * Invoke for canceling asynchronous request
     * @since S60 S60 v3.1
     */    
    void DoCancel();
    
private: 

    /* @var iProperty Property containing current IP address */
    RProperty iProperty;

    /* @var iObserver Observer which should be informed in case of changes */
    MUpnpIPEventObserver* iObserver;

    };


#endif //  C_UPNPIPEVENTNOTIFIER_H
