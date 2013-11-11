/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Declaration of class CHttpSessionBase.
*      
*
*/


#ifndef HTTP_SESSION_BASE_H
#define HTTP_SESSION_BASE_H

// INCLUDES

#include <e32base.h>
#include <http.h>

// CLASS DECLARATION

/**
* HTTP Session base class. Abstraction of the two derived class of sessions,
* WAP and TCP.
*/
NONSHARABLE_CLASS( CHttpSessionBase ): public CBase
    {
    public:     // Constructors and destructor.

        /**
        * Destructor.
        */
        virtual ~CHttpSessionBase();

    public:     // new methods

        /**
        * Connect session.
        * @param aStatus Completes when done.
        */
        virtual void ConnectL( TRequestStatus* aStatus ) = 0;

        /**
        * Disconnect session. Safe to call if not connected. Use this to
        * cancel an outstanding ConnectL.
        */
        virtual void Disconnect() = 0;

        /**
        * Get underlying HTTP session.
        */
        inline RHTTPSession& Sess();

    protected:    // Constructors and destructor.

        /**
        * Constructor.
        */
        CHttpSessionBase();

    protected:  // data

        RHTTPSession iSess;    ///< HTTP Session handle.

    };

#include "HttpSessionBase.inl"

#endif /* def HTTP_SESSION_BASE_H */
