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
* Description:  CUpnpHttpServerTransactionDescription declaration.
*
*/

#ifndef UPNPHTTPFILESERVETRANSACTION_H_
#define UPNPHTTPFILESERVETRANSACTION_H_

// INCLUDES
#include "upnphttpservertransaction.h"

// FORWARD DECLARATION
class CUpnpHttpServerTransactionHandler;
class TInetAddr;

/**
 *  Transaction serving desciptions and icons. 
 *  
 */
NONSHARABLE_CLASS( CUpnpHttpServerTransactionDescription ): public CUpnpHttpServerTransaction
    {
public:
    /**
     * 2 phase constructor
     */
    static CUpnpHttpServerTransactionDescription* NewL( 
    		CUpnpHttpServerTransactionHandler& aClientContext, const TInetAddr& aSender, const TDesC8& aUri );
        
    ~CUpnpHttpServerTransactionDescription();
    
    /**
     * Base class callback implementation
     */        
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent );  



protected:

    void ConstructL( const TInetAddr& aSender, const TDesC8& aSenderUri );
	
    CUpnpHttpServerTransactionDescription( CUpnpHttpServerTransactionHandler& aClientContext );
    
private:
    void DoCallbackL( TUpnpHttpServerEvent aEvent );
    
    TInt DecodeUri();

private:
    // Sender Uri.
    HBufC8* iSenderUri;
    // Sender address.
    TInetAddr iSender;
    
    
    RBuf8 iDecodedUri;
    
    CUpnpHttpServerTransactionHandler& iClientContext;
        
    };

#endif /* UPNPHTTPFILESERVETRANSACTION_H_ */
