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
* Description:  Declaration of CUpnpSoapActionHttpTransaction
*
*/


#ifndef C__CUPNPSOAPACTIONHTTPTRANSACTION_H__
#define C__CUPNPSOAPACTIONHTTPTRANSACTION_H__

#include "upnphttptransaction.h"

class CUpnpControlPoint;

/**
 * Http transation to get service description message.
 */
NONSHARABLE_CLASS ( CUpnpSoapActionHttpTransaction ) : public CUpnpHttpTransaction
{
public:
    static CUpnpSoapActionHttpTransaction* NewLC(
        CUpnpHttpMessage* aRequest, TInt aActionId, CUpnpControlPoint& aControlPoint );
    
    virtual ~CUpnpSoapActionHttpTransaction();


private:    //from CUpnpHttpTransaction
    /**
     * Callback called when response comes.
     * Processes response by passing it to control point.
     */
    virtual void ProcessResponseL();

private:
    CUpnpSoapActionHttpTransaction(
        CUpnpHttpMessage* aRequest, TInt aActionId, CUpnpControlPoint& aControlPoint );

private:
    
    TInt iActionId; //owned
    
    CUpnpControlPoint& iControlPoint; //not owned
    
};

#endif /*C__CUPNPSOAPACTIONHTTPTRANSACTION_H__*/

//  End of File