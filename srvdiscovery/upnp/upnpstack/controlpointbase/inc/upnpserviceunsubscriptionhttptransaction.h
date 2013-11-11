/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CUpnpServiceUnsubscriptionHttpTransaction
*
*/

#ifndef UPNPSERVICEUNSUBSCRIBETRANSACTION_H
#define UPNPSERVICEUNSUBSCRIBETRANSACTION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "upnphttptransaction.h"

// CLASS DECLARATION
class CUpnpControlPoint;
/**
 *  Cupnpserviceunsubscribetransaction
 * 
 */

NONSHARABLE_CLASS( CUpnpServiceUnsubscriptionHttpTransaction ) : public CUpnpHttpTransaction
    {
public:
    // Constructors and destructor

    /**
    * Destructor.
    */
    ~CUpnpServiceUnsubscriptionHttpTransaction();

    /**
    * Two-phased constructor.
    */
    static CUpnpServiceUnsubscriptionHttpTransaction* NewLC( CUpnpHttpMessage* aRequest,
                                                             CUpnpControlPoint& aControlPoint );

private:

    /**
    * Constructor for performing 1st stage construction
    */
    CUpnpServiceUnsubscriptionHttpTransaction( CUpnpHttpMessage* aRequest,
                                               CUpnpControlPoint& aControlPoint );

    /**
    * Callback called when response comes.
    * Processes response by passing it to control point.
    */
    virtual void ProcessResponseL();

private: 

	//control point, now owned
    CUpnpControlPoint& iControlPoint;
    };

#endif // UPNPSERVICEUNSUBSCRIBETRANSACTION_H
