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
*      Declaration of class CHttpTcpSession.
*      
*
*/


#ifndef HTTP_TCP_SESSION_H
#define HTTP_TCP_SESSION_H

// INCLUDES

#include <e32base.h>
#include "HttpSessionBase.h"

// CLASS DECLARATION

/**
* TCP Session.
*/
NONSHARABLE_CLASS( CHttpTcpSession ): public CHttpSessionBase
    {
    public:     // Constructors and destructor.

        /**
        * Two phased constructor. Leaves on failure.
        * @return The created session.
        */      
        static CHttpTcpSession* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHttpTcpSession();

    public:     // from CHttpSessionBase

        /**
        * Connect session. Does nothing.
        * @param aStatus Completes immediately with KErrNone..
        */
        virtual void ConnectL( TRequestStatus* aStatus );

        /**
        * Disconnect session. Does nothing.
        */
        virtual void Disconnect();

    protected:    // Constructors and destructor.

        /**
        * Constructor.
        */
        CHttpTcpSession();

        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();
        
    };

#endif /* def HTTP_TCP_SESSION_H */
