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
* Description:  Declaration of CUpnpServiceSubscriptionHttpTransaction
*
*/

#ifndef UPNPSERVICESUBSCRIPTIONHTTPTRANSACTION_H
#define UPNPSERVICESUBSCRIPTIONHTTPTRANSACTION_H

// INCLUDES
#include "upnphttptransaction.h"

// CLASS DECLARATION
class CUpnpControlPoint;
/**
 *  CUpnpServiceSubscriptionHttpTransaction
 * 
 */

NONSHARABLE_CLASS( CUpnpServiceSubscriptionHttpTransaction ) : public CUpnpHttpTransaction
    {
public:
    // Constructors and destructor

    /**
    * Destructor.
    */
    ~CUpnpServiceSubscriptionHttpTransaction();

    /**
    * Two-phased constructor.
    */
    static CUpnpServiceSubscriptionHttpTransaction* NewLC( CUpnpHttpMessage* aRequest,
                                                           CUpnpControlPoint& aControlPoint );

private:    //from CUpnpHttpTransaction
    /**
    * Callback called when response comes.
    * Processes response by passing it to control point.
    */
    virtual void ProcessResponseL();

private:

    /**
    * Constructor for performing 1st stage construction
    */
    CUpnpServiceSubscriptionHttpTransaction( CUpnpHttpMessage* aRequest,
                                             CUpnpControlPoint& aControlPoint );

private:
     //control point, not owned
    CUpnpControlPoint& iControlPoint;

    //SessionId of message that added service to control point's subscribed services
    TInt iServiceSessionId;
    };

#endif // UPNPSERVICESUBSCRIPTIONHTTPTRANSACTION_H
