/*
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
* Description:  MUpnpHttpServerTransactionCreator declaration.
*
*/

#ifndef UPNPHTTPSERVERTRANSACTIONCREATOR_H_
#define UPNPHTTPSERVERTRANSACTIONCREATOR_H_

// FORWARD DECLARATIONS

class CUpnpHttpServerTransaction;
class TDesC8;
class TInetAddr;

// CLASS DESCRIPTION

/**
 * MUpnpHttpServerTransactionCreator is an interface 
 * for creation of CUpnpHttpServerTransaction based class instance.
 * 
 * This method is invoked whenever http request related to file transfer
 * arrives to http server. 
 *
 * @lib dlnawebserver.lib
 * @since S60 5.1
 */
class MUpnpHttpServerTransactionCreator
    {
public:
    /**
     * Create a new transaction object.
     * The object ownership is transferred to an invoker.
     * 
     * @since S60 5.1
     * @param aMethod Http message method ( GET, HEAD or POST )
     * @param aUri Http message request uri
     * @param aSernder IP address of the Http message sender
     * @param aTrans Returns newly created transaction object 
     */
    virtual void NewTransactionL( const TDesC8& aMethod,
                                  const TDesC8& aUri,
                                  const TInetAddr& aSender,
                                  CUpnpHttpServerTransaction*& aTrans ) = 0;
    };

#endif /* UPNPHTTPSERVERTRANSACTIONCREATOR_H_ */
