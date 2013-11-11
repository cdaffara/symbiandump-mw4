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
* Description:  CUpnpNetworkEventBase 
*
*/

#ifndef C_UPNPNETWORKEVENTBASE_H
#define C_UPNPNETWORKEVENTBASE_H

// Includes
#include <e32base.h>
#include <upnpconnmanagereventtypes.h>


/**
 * Base class for network events
 * @since S60 5.0
 **/
class CUpnpNetworkEventBase : public CBase
    {
public:
    
    /**
     * Constructor
     * @since S60 5.0 
     * @param error
     */
    CUpnpNetworkEventBase( TInt aError );
    
    /**
     * Constructor
     * @since S60 5.0 
     * @param network event type
     */
    CUpnpNetworkEventBase( TUpnpCmSubRsp aType );
    
    /**
     * Getter for network event type
     * @since S60 5.0 
     * @return Type
     */
    IMPORT_C TUpnpCmSubRsp Type();
    
    /**
     * Getter for network event error
     * @since S60 5.0 
     * @return error
     */
    IMPORT_C TInt SubscriberError(); 
    
protected:  // Data

    // Network event type
    TUpnpCmSubRsp iType;

    // Subscription error
    TInt iSubscriberError;

    };

#endif // C_UPNPNETWORKEVENTBASE_H

// End of file

