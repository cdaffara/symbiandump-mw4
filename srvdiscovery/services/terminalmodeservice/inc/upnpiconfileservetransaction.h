/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CUpnpIconFileServeTransaction class declaration
*
*/

#ifndef __UPNPICONFILESERVETRANSACTION_H_
#define __UPNPICONFILESERVETRANSACTION_H_

#include <upnphttpservertransaction.h>

/**
* Class derives from CUpnpHttpServerTransaction base class. It sets the actual filepath 
* which have to be served for http-get request
*/
class CUpnpIconFileServeTransaction: public CUpnpHttpServerTransaction
    {
public:
    ~CUpnpIconFileServeTransaction();  
    
    static CUpnpIconFileServeTransaction* NewL( const TDesC& aOutFilePath, RFs& aIconSession );

public:
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent );
    
protected:
    CUpnpIconFileServeTransaction( RFs& aIconSession );
    void ConstructL( const TDesC& aOutFilePath  );    
    
private:
    void DoCallbackL( TUpnpHttpServerEvent aEvent ); 
       
private:
    RBuf   iOutFilePath;
    RFs&   iIconFileSession ;
    };

#endif  // __UPNPICONFILESERVETRANSACTION_H__
