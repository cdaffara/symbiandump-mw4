/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpHttpServerSession is class that is used to handle single 
*                 connections to HttpServer
*
*/

#ifndef UPNPHTTPSERVERSESSION_H
#define UPNPHTTPSERVERSESSION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CUpnpHttpServerRuntime;
class MUpnpHttpServerObserver;
class CUpnpHttpMessage;

const TInt KUpnpHttpServerRandomPort = 0;

// CLASS DECLARATION
/**
 * CUpnpHttpServerSession is used for starting and stopping http server
 * instance on random or predefined port.
 * It contains only one default runtime which handles all incoming request URLs. 
 * @lib dlnawebserver.lib
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CUpnpHttpServerSession ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     * @param aUnusedIapId internet access point id (unused in current implementation)
     * @param aObserver observer to be notified about httpEvents
     */
    IMPORT_C static CUpnpHttpServerSession* NewL( TUint aUnusedIapId,
                                                  MUpnpHttpServerObserver& aObserver );
                                                  
    /**
     * Destructor.
     */
    ~CUpnpHttpServerSession();

    /**
     * Starts http server owned by this session.
     * 
     * @since S60 5.1
     * @param aPort port on which server starts, default value: KRandomPort
     */
    IMPORT_C void StartL( const TInt aPort = KUpnpHttpServerRandomPort );

    /**
     * Stops http server owned by this session.
     * 
     * @since S60 5.1
     */
    IMPORT_C void Stop();

    /**
     * Gets http server address (including port).
     *
     * @since S60 5.1
     * @param aAddr used to return server address
     */
    IMPORT_C void GetAddress( TInetAddr& aAddr );

    /**
     * Gets default runtime.
     * 
     * @since S60 5.1
     * @return reference to http server runtime
     */
    IMPORT_C CUpnpHttpServerRuntime& DefaultRuntime();
    
    //For internal usage only.
    static CUpnpHttpServerSession* NewLC( TUint aUnusedIapId,
                                          MUpnpHttpServerObserver& aObserver );
    IMPORT_C TSockAddr* HWAddressL();    
    IMPORT_C TInt SendMessageL( CUpnpHttpMessage* aMessage );
           
private:

    /**
     * Constructor for performing 1st stage construction.
     */
    CUpnpHttpServerSession();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL( TUint aUnusedIapId, MUpnpHttpServerObserver& aObserver );

private:    // data

    // Pointer to server runtime, owned
    CUpnpHttpServerRuntime* iServerRuntime;
    };

#endif // UPNPHTTPSERVERSESSION_H
