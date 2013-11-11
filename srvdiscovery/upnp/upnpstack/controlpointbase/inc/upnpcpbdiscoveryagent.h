/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares ControlPoint Discovery class.
*
*/


#ifndef C_CUPNPCPBDISCOVERYAGENT_H
#define C_CUPNPCPBDISCOVERYAGENT_H

// INCLUDES
#include <e32base.h>
#include "upnpdiscoveryobserver.h"

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpControlPoint;
class MUpnpCpbHttpMessageController;
#ifdef RD_UPNP_REMOTE_ACCESS
class CUpnpRadaSync;
#endif

/**
 * Mediator class handling whole discovery process.
 * It gets device description and decides how it should be treated
 * and how to discorery it.
 *
 * @lib controlpointbase.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUpnpCpbDiscoveryAgent ) : public CBase
    {
    public:
    /**
     * Two-phased constructor
     * @since S60 v5.0
     */
    static CUpnpCpbDiscoveryAgent* CUpnpCpbDiscoveryAgent::NewL(
        MUpnpCpbHttpMessageController& aMessanger );

    /**
     * Destructor
     * @since S60 v5.0
     */
    ~CUpnpCpbDiscoveryAgent();

    public: // Functions from devired class

    /**
     * Make a SSDP search. Search string should be some of these strings (defined in UPnP Device Architecture)
     * @since Series60 5.0
     * @param aSearchString The search target.
     */
    void SearchL( const TDesC8& aSearchString );

    #ifdef RD_UPNP_REMOTE_ACCESS
    /*
     * Enables rada device
     * @since Series60 5.0
     */
    void EnableRadaDeviceL( TRequestStatus& aStatus );

    /*
     * Enables rada device
     * @since Series60 5.0
     * @return Rada device start status
     */
    TInt EnableRadaDeviceL();

    /*
     * Disables rada device
     * @since Series60 5.0
     */
    void DisableRadaDeviceL();

    /**********IPFinterng*****/

    void StartIPFilteringL();

    void StopIPFiltering();

    /***********************/
    #endif

    private:
    /**
     * Constructor
     * @since S60 v5.0
     */
    CUpnpCpbDiscoveryAgent( MUpnpCpbHttpMessageController& aMessanger );

    /**
     * Second phase constructor
     * @since S60 v5.0
     */
    void ConstructL();

    private:
    //not owned
    MUpnpCpbHttpMessageController& iMessanger;

    #ifdef RD_UPNP_REMOTE_ACCESS
    // RADA client
    CUpnpRadaSync* iRadaClient;
    #endif
    };

#endif // C_CUPNPCPBDISCOVERYAGENT_H
//  End of File
