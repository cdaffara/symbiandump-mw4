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
* Description:  MUpnpNetworkEventObserver
*
*/

#ifndef M_UPNPNETWORKEVENTOBSERVER_H
#define M_UPNPNETWORKEVENTOBSERVER_H

// Includes
#include <e32cmn.h>
#include <upnpnetworkeventbase.h>
#include <upnpnetworkeventaddresschange.h>


/**
 * Observer for network events
 * @since S60 5.0
 **/
class MUpnpNetworkEventObserver
    {
public:
    
    /**
     * Inform about netowrk event
     * @since S60 5.0 
     * @param network event
     */
    virtual void NetworkEvent( CUpnpNetworkEventBase* aEvent ) = 0;

    };

#endif // M_UPNPCONNECTIONMANAGERNETWORKEVENTOBSERVER_H

// End of file

