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








#ifndef IDWSF_CORRELATION_HEADER_H
#define IDWSF_CORRELATION_HEADER_H

// INCLUDES
#include <SenXmlElement.h>

// CONSTANTS
_LIT8(KCorrelationName, "Correlation");
_LIT8(KCorrelationQName, "sb:Correlation");

// CLASS DECLARATION  // we could use internal element here (to optimize mem)
class CIdWsfCorrelationHeader :  public CSenXmlElement
    {
    public: // Constructors and destructors
        
        static CIdWsfCorrelationHeader* NewL(
                                const TDesC8& aRefToMessageId = KNullDesC8);
        
        static CIdWsfCorrelationHeader* NewL(const RAttributeArray& aAttributes);

        virtual ~CIdWsfCorrelationHeader();

        // New functions
        
        virtual TPtrC8 MessageId();
        virtual TPtrC8 RefToMessageId();
        virtual TPtrC8 Timestamp();
        virtual void SetCorrelationL(const TDesC8& aRefToMessageId);

    protected: // Functions from base classes
    
        // From CBaseElement
        virtual void WriteAttrsToL(RWriteStream& aWriteStream);

    private:
        
        /**
        * C++ default constructor.
        */
        CIdWsfCorrelationHeader();
        
        /**
        * By default Symbian 2nd phase constructors are private.
        */
        void ConstructL(const TDesC8& aRefToMessageId);
        void ConstructL(const RAttributeArray& aAttributes);

    private: // Data
        HBufC8* ipMessageId;         // Owned
        HBufC8* ipRefToMessageId;    // Owned
        HBufC8* ipTimestamp;         // Owned
    };

#endif // IDWSF_CORRELATION_HEADER_H

// End of File
