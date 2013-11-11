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


#ifndef UPNPSSDPHANDLER_H
#define UPNPSSDPHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <in_sock.h>

// CLASS DECLARATION
class CUpnpDeviceLibraryElement;
class MUpnpSsdpServerObserver;
class RSocketServ;

/**
 *  CUpnpSsdpHandlerBase is a base class for 
 *  classes which handle SSDP server states.
 */
class CUpnpSsdpHandlerBase : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpSsdpHandlerBase();

    /**
     * Method for ssdp startup 
     * aSocketServ - reference to socket server session
     */
    virtual void StartL( RSocketServ& aSocketServ ) = 0;
     
    /**
     * Method for ssdp immidiate stop
     */    
    virtual void Stop() = 0;
    
     /**
     * Method for address change event
     */    
    virtual void AddressChangeL( TInetAddr& aAddress  ) = 0;
        
    /**
     * Method for ssdp m-search
     * aSearchString - The SSDP search string, that is placed to the 
     *                 ST header of the SSDP message.
     * aMX - list of MX times for m-search messages
     */    
    virtual void SsdpSearchL( const TDesC8& aSearchString, TDesC8& aMX ) = 0;
    
    /**
     * Method for adverising local device
     */    
    virtual void AdvertiseDeviceL( TInt aLive,  CUpnpDeviceLibraryElement& aElement ) = 0;
  
    /**
     * Method checks if ssdp is started
     */    
    virtual TBool IsStarted() = 0;

protected:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpSsdpHandlerBase( MUpnpSsdpServerObserver& aObserver );

protected:

    // Observer pointer 
    MUpnpSsdpServerObserver& iSsdpServerObserver; 
    
    };

#endif // UPNPSSDPHANDLER_H
