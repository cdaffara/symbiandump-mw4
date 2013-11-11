/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  CUpnpFileSender declaration
 *
 */

#ifndef UPNPFILESENDER_H
#define UPNPFILESENDER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "httptransferobserver.h"

class CHttpUploader;
class CUpnpHttpMessage;
class CUpnpControlPoint;

// CLASS DECLARATION

/**
 *  CUpnpFileSender
 * 
 */
NONSHARABLE_CLASS( CUpnpFileSender ) : public CBase, public MHttpTransferObserver
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpFileSender();

    /**
     * Two-phased constructor.
     */
    static CUpnpFileSender* NewL( CUpnpControlPoint& aCPToNotifyOfSendingResult );

    /**
     * Two-phased constructor.
     */
    static CUpnpFileSender* NewLC( CUpnpControlPoint& aCPToNotifyOfSendingResult );

    /**
     * Send particular file by http post
     * Message should have post method and outfilenama set, and proper destination
     * address set. Response will be notified to control point object
     */
    void SendL( CUpnpHttpMessage& aMessageToSend );
    
private:
    //from MHttpTransferObserver
    
    virtual void TransferProgress( TAny* aKey,
                                   TInt aBytes, 
                                   TInt aTotalBytes );

    virtual void ReadyForTransferL( TAny* aKey );

    virtual void TransferCompleted( TAny* aKey, TInt aStatus );    
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpFileSender( CUpnpControlPoint& aCPToNotifyOfSendingResult );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:

    // control point, not owned
    CUpnpControlPoint& iControlPoint;
    
    CHttpUploader* iUploader;
    
    };

#endif // UPNPFILESENDER_H
