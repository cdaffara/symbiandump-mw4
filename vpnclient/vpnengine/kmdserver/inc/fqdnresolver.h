/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors the completion of FQDN resolve.
*
*/


#ifndef C_FQDNRESOLVER_H
#define C_FQDNRESOLVER_H

#include <e32base.h>
#include <es_sock.h>

// FORWARD DECLARATIONS
class CVpnConnection;

/**
 *  FQDN resolver callback interface.
 *  Callback interface which is used by CFqdnResolver object to notify
 *  about completion of FQDN address resolving.
 * 
 *  @lib internal (kmdserver.exe)
 */
class MFqdnResolverCallback
    {
public:
    /**
     * Notifies about completion of FQDN address resolving.
     * @param aStatus Completion status
     * @param aNameEntry Result of name resolution
     */
    virtual void AddressResolveCompleted( const TInt aStatus,
                                          const TNameEntry aNameEntry ) = 0;    
    };

/**
 *  FQDN address resolver.
 *  Active object provides functionality for resolving an IP address from FQDN
 *  address.
 *
 *  @lib internal (kmdserver.exe)
 */
class CFqdnResolver : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aConnection VPN connection object
     * @param aCallback Callback interface
     */
    static CFqdnResolver* NewL( CVpnConnection& aConnection,
                                MFqdnResolverCallback& aCallback );
    /**
     * Destructor.
     */
    ~CFqdnResolver();

    /**
     * Resolves IP address from FQDN address. Completion is notified via
     * MFqdnResolverCallback interface.
     * @param aFqdn FQDN address. Ownership transferred.
     */
    void ResolveAddress( HBufC* aFqdn );

private:
    
    CFqdnResolver( CVpnConnection& aConnection,
                   MFqdnResolverCallback& aCallback );

// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous resolving request.
     */    
    void RunL();
        
    /**
     * From CActive
     * Handles cancellation of asynchronous resolving request.
     */    
    void DoCancel();

private: // data
    
    /**
     * VPN connection object.
     * Not own.
     */
    CVpnConnection&         iConnection;    
    
    /**
     * FQDN address
     * Own.
     */
    HBufC*                  iFqdn;
    
    /**
     * Name entry.
     * Own.
     */
    TNameEntry              iNameEntry;

    /**
     * Callback interface.
     * Not own.
     */
    MFqdnResolverCallback&  iCallback;    
    };

#endif // C_FQDNRESOLVER_H
