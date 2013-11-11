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
* Description:  
*
*/



#ifndef CUPNPSSDPHANDLERUP_H
#define CUPNPSSDPHANDLERUP_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "upnpssdphandlerbase.h"
#include "upnpmsearchtimer.h"



// CLASS DECLARATION
class CUpnpDeviceLibrary;
class CUpnpSsdpServer;
class CUpnpNotifyTimer;


// CONST DECLARATION
const TInt KMXTimeBuffer = 10;


// CLASS DECLARATION

/**
 *  CUpnpSsdpHandlerUp handles the state when ssdp servers   
 *  are up and running.
 */
class CUpnpSsdpHandlerUp : public CUpnpSsdpHandlerBase, 
                           public MUpnpNotifyTimerObserver
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpSsdpHandlerUp();

    /**
     * Two-phased constructor.
     */
    static CUpnpSsdpHandlerUp* NewL( MUpnpSsdpServerObserver& aSsdpServerObserver );

    /**
     * Starts two ssdp servers - unicast and multicast 
     * aSocketServ - reference to socket server session
     */
    virtual void StartL( RSocketServ& aSocketServ );
        
        
    /**
     * Stops two ssdp servers 
     */
    virtual void Stop();
        
    /**
     * Starts sending ssdp m-search messages
     * aSearchString - The SSDP search string, that is placed to the 
     *                 ST header of the SSDP message.
     * aMX - list of MX times for messages
     */            
    virtual void SsdpSearchL( const TDesC8& aSearchString, TDesC8& aMX );
        
    /**
     * Stops two ssdp servers 
     */
    virtual void AdvertiseDeviceL( TInt aLive,  CUpnpDeviceLibraryElement& aElement );
    
    /**
     * Informs if start method was performed
     */
    virtual TBool IsStarted();   
    
     /**
     * Method for address change event
     */    
    virtual void AddressChangeL( TInetAddr& aAddress );
     
    /**
     * Increments control point session count
     */    
    TInt AddLocalControlPoint( );
    
    /**
     * Decrements control point session count
     */    
    TInt RemoveLocalControlPoint( );
    
private: // from MUpnpNotifyTimerObserver
    
    void TimerEventL( CUpnpNotifyTimer* aTimer );
        
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpSsdpHandlerUp( MUpnpSsdpServerObserver& aSsdpServerObserver );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
        
    void ClearTimers();        

private:
    
    // Unicast SSDP server instance 
    CUpnpSsdpServer* iSsdpServer;

    // Multicast SSDP server instance 
    CUpnpSsdpServer* iMulticastSsdpServer;

	// timers array        
    RPointerArray<CUpnpMSearchTimer> iTimers;
    
    // Local control points counter 
    TInt iCPCounter;
    
    };

#endif // CUPNPSSDPHANDLERUP_H
