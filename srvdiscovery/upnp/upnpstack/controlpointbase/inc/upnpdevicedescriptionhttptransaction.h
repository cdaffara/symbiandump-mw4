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
* Description:  Declaration of CUpnpDeviceDescriptionHttpTransaction
*
*/

#ifndef __UPNPDEVICEDESCRIPTIONHTTPTRANSACTION_H__
#define __UPNPDEVICEDESCRIPTIONHTTPTRANSACTION_H__

#include "upnphttptransaction.h"

class CUpnpControlPoint;

/**
 * Http transation to get device description message.
 */
NONSHARABLE_CLASS ( CUpnpDeviceDescriptionHttpTransaction ) : public CUpnpHttpTransaction
{
public:
    static CUpnpDeviceDescriptionHttpTransaction* NewLC( CUpnpHttpMessage* aRequest,
                                                         CUpnpControlPoint& aControlPoint );

    virtual ~CUpnpDeviceDescriptionHttpTransaction();

private:    //from CUpnpHttpTransaction
    /**
     * Callback called when response comes.
     * Processes response by passing it to control point.
     */
    virtual void ProcessResponseL();

private:
    CUpnpDeviceDescriptionHttpTransaction(
        CUpnpHttpMessage* aRequest, CUpnpControlPoint& aControlPoint );

private:
    /**
     * Reference to control point - for callback purposes.
     */
    CUpnpControlPoint& iControlPoint;

};

#endif /*__UPNPDEVICEDESCRIPTIONHTTPTRANSACTION_H__*/

//  End of File

