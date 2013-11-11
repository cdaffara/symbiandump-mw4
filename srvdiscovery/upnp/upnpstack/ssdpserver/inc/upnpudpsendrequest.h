/** @file
 * Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  CUpnpUdpSendRequest is udp message sender
 *
 */

#ifndef C_CUPNPUDPSERVERREQUEST_H
#define C_CUPNPUDPSERVERREQUEST_H

// INCLUDE FILES
#include <e32std.h>

// CLASS DECLARATION
/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 *
 *  @since Series60 2.0
 */
NONSHARABLE_CLASS(CUpnpUdpSendRequest) : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUpnpUdpSendRequest* NewL( TDesC8& aBuffer, TInetAddr& anAddr );

    /**
     * C++ default constructor.
     */
    CUpnpUdpSendRequest( const TInetAddr& anAddr );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const TDesC8& aBuffer );

    /**
     * Destructor.
     */
    ~CUpnpUdpSendRequest();

public:

    HBufC8* iBuffer;
    TInetAddr iAddr;

    };

#endif  //C_CUPNPUDPSERVERREQUEST_H
// End of File
