/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef IDWSFSECURITYHEADER_H
#define IDWSFSECURITYHEADER_H

// INCLUDES
#include "SenWsSecurityHeader.h"

// CONSTANTS
_LIT8(KSecurityDraftXmlns, "http://schemas.xmlsoap.org/ws/2003/06/secext");
_LIT8(KSecurityDraftXmlnsPrefix, "wsse");

// CLASS DECLARATION
class CIdWsfSecurityHeader : public CSenWsSecurityHeader
    {
    public: // Constructors and destructor
    
        static CIdWsfSecurityHeader* NewL();
        static CIdWsfSecurityHeader* NewL(const TDesC8& aData);
        virtual ~CIdWsfSecurityHeader();

        // New functions
        
        virtual TPtrC8 XmlNs();
        virtual TPtrC8 XmlNsPrefix();
        static HBufC8* UsernameToken8L(CSenIdentityProvider &aIdentityProvider);
        static TInt UsernameTokenL(const TDesC8& username, HBufC8*& aToken);

    protected:
    
        /**
        * C++ default constructor.
        */
        CIdWsfSecurityHeader();
    };

#endif // IDWSFSECURITYHEADER_H

// End of File
