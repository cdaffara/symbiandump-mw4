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


#ifndef CUPNPSSDPHANDLERDOWN_H
#define CUPNPSSDPHANDLERDOWN_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "upnpssdphandlerbase.h"



// CLASS DECLARATION
class MUpnpSsdpServerObserver;

/**
 *  CUpnpSsdpHandlerDown handles the state when ssdp servers   
 *  are stopped. In this state most operation should do nothing 
 *  so respective methods have empty body
 */
class CUpnpSsdpHandlerDown : public CUpnpSsdpHandlerBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpSsdpHandlerDown();

    /**
     * Two-phased constructor.
     */
    static CUpnpSsdpHandlerDown* NewL( MUpnpSsdpServerObserver& aSsdpServerObserver );

    /**
     * Empty implementation
     */
    virtual void StartL( RSocketServ& aSocketServ );
          
    /**
     * Empty implementation
     */
    virtual void Stop();
             
    /**
     * Empty implementation
     */
    virtual void SsdpSearchL( const TDesC8& aSearchString, TDesC8& aMX );
        
    /**
     * Empty implementation
     */
    virtual void AdvertiseDeviceL( TInt aLive,  CUpnpDeviceLibraryElement& aElement );
    
    /**
     * Always returns true
     */    
    virtual TBool IsStarted();
    
     /**
     * Method for address change event
     */    
    virtual void AddressChangeL( TInetAddr& aAddress );
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpSsdpHandlerDown( MUpnpSsdpServerObserver& aSsdpServerObserver );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    };

#endif // CUPNPSSDPHANDLERDOWN_H
