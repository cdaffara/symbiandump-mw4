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

#ifndef CUPNPCPBINITIALEVENTRETRY_H
#define CUPNPCPBINITIALEVENTRETRY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CUpnpGenaMessage;
class CUpnpCpbInitialEventRetryHandler;


/**
 *  CUpnpCpbInitialEventRetry provides timeout for Gena message 
 * 
 */
NONSHARABLE_CLASS( CUpnpCpbInitialEventRetry ) : public CTimer
    {
public:
    // Constructors and destructor
    // Ownership of aMessage is passed through
    static CUpnpCpbInitialEventRetry* CUpnpCpbInitialEventRetry::NewLC(
            CUpnpGenaMessage* aMessage,
            CUpnpCpbInitialEventRetryHandler& iOwner );

    ~CUpnpCpbInitialEventRetry();
    
    // CTimer callback implementation
    void RunL();

    CUpnpGenaMessage& Message();
    
private:    
    CUpnpCpbInitialEventRetry( CUpnpGenaMessage* aMessage,
                CUpnpCpbInitialEventRetryHandler& iOwner );
    void ConstrucL();

private:
    // owned
    CUpnpGenaMessage* iMessage;
    CUpnpCpbInitialEventRetryHandler& iOwner;
    
    };

#endif // CUPNPCPBINITIALEVENTRETRY_H
