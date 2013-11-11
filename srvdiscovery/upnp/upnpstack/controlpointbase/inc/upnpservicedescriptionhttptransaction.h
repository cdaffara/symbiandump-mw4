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
* Description:  Declaration of CUpnpServiceDescriptionHttpTransaction
*
*/


#ifndef __UPNPSERVICEDESCRIPTIONHTTPTRANSACTION_H__
#define __UPNPSERVICEDESCRIPTIONHTTPTRANSACTION_H__

#include "upnphttptransaction.h"

class CUpnpControlPoint;

/**
 * Http transation to get service description message.
 */
NONSHARABLE_CLASS ( CUpnpServiceDescriptionHttpTransaction ) : public CUpnpHttpTransaction
{
public:
    static CUpnpServiceDescriptionHttpTransaction* NewLC(
        CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint );
    virtual ~CUpnpServiceDescriptionHttpTransaction();

private:    //from CUpnpHttpTransaction
    /**
     * Callback called when response comes.
     * Processes response by passing it to control point.
     */
    virtual void ProcessResponseL();

private:
    CUpnpServiceDescriptionHttpTransaction(
        CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint );

private:
    CUpnpControlPoint& iControlPoint;
};

#endif /*__UPNPSERVICEDESCRIPTIONHTTPTRANSACTION_H__*/

//  End of File

