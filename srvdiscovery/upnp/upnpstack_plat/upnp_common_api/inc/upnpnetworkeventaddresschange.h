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
* Description:  CUpnpNetworkEventAddressChange 
*
*/

#ifndef C_UPNPNETWORKEVENTADDRESSCHANGE_H
#define C_UPNPNETWORKEVENTADDRESSCHANGE_H

// Includes
#include <upnpnetworkeventbase.h>
#include <in_sock.h>

/**
 * Class encapsulating AddressChange network event
 * @since S60 5.0
 **/
class CUpnpNetworkEventAddressChange : public CUpnpNetworkEventBase 
    {
public:
    
    /**
     * Constructor
     * @since S60 5.0 
     * @param address
     */
    CUpnpNetworkEventAddressChange( TInetAddr aAddress );

    /**
     * Getter for address
     * @since S60 5.0 
     * @return address
     */
    IMPORT_C TInetAddr Address();
    
private:  // Data

    // address
    TInetAddr iInetAddr;

    };

#endif // C_UPNPNETWORKEVENTADDRESSCHANGE_H

// End of file

