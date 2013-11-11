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
* Description:  Declares the CUpnpHttpTransaction class
*
*/


#ifndef __UPNPHTTPTRANSACTION_H__
#define __UPNPHTTPTRANSACTION_H__

#include <e32base.h>

class CUpnpHttpMessage;

/**
 * Abstract base class for every upnp transaction.
 * Transaction encapsulates request and response message.
 * Client constructs instance of transaction with request, and sends it
 * by client engine. When response comes engine writes it to response message
 * and transaction can process it by implementing ProcessResponseL method.
 */
class CUpnpHttpTransaction : public CBase
    {
    public:
        IMPORT_C ~CUpnpHttpTransaction();

        /*
         * Returns request message of transaction.
         *
         * @return request message of transaction
         */
        IMPORT_C CUpnpHttpMessage* Request();

        /*
         * Returns response message of transaction.
         *
         * @return response message of transaction
         */
        IMPORT_C CUpnpHttpMessage* Response();

        /*
         * Pure virtual method that let child classes to process
         * response in their own specific way.
         */
        virtual void ProcessResponseL() = 0;

        /*
         * Creates ok response for the request
         *
         * @param aResponseBody response body to set in response message
         */
        void CreateOkResponseL( const TDesC8& aResponseBody );

        /*
         * Creates response for the request
         *
         */      
        void CreateFaultResponseL( const TDesC8& aResponseBody, TInt aStatus, TInt aError );

    protected:
        IMPORT_C CUpnpHttpTransaction( CUpnpHttpMessage* aRequest );
        IMPORT_C void BaseConstructL();

    private://owned
        CUpnpHttpMessage* iHttpRequest;
        CUpnpHttpMessage* iHttpResponse;
    };

#endif /*__UPNPHTTPTRANSACTION_H__*/

//  End of File

