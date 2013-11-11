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








#ifndef IDWSF_MESSAGE_H
#define IDWSF_MESSAGE_H

// INCLUDES
#include "idwsfcorrelationheader.h"
#include "idwsfsiuheader.h"
#include "idwsfsoapmessage.h"

// CONSTANTS
_LIT8(KSBXmlNs,                         "urn:liberty:sb:2003-08");
_LIT8(KStatusEndpointMoved,             "EndpointMoved");
_LIT8(KStatusInappropriateCredentials,  "InappropriateCredentials");

// CLASS DECLARATION
class CIdWsfMessage : public CIdWsfSoapMessage
    {
    public: // Constructors and destructor
    
        static CIdWsfMessage* NewL(const TDesC8& aRefToMessageId = KNullDesC8);
        virtual ~CIdWsfMessage();

        // New functions
    
        /**
        * Convenience methods
        */
        virtual void SetCorrelationL(const TDesC8& aRefToMessageId);
        virtual TPtrC8 MessageId();
        virtual TPtrC8 RefToMessageId();
        virtual TPtrC8 Timestamp();

        /**
        * Retains ownership.
        */
        virtual CIdWsfSiuHeader* SiuHeader();

        /**
        * @return the value of the "code" attribute in a possible Status element.
        * The Status element is expected to be a child of the first child of the
        * S:Body. Or the child of a <detail> element of a <s:Fault>
        */
        virtual const TDesC8* StatusCodeL();

        // Functions from base classes
        
        // From CSenSoapMessage
        virtual void ParseHeaderL(const TDesC8& aNsUri,
                                  const TDesC8& aLocalName,
                                  const TDesC8& aQName,
                                  const RAttributeArray& aAttributes);



    protected:
        
        /**
        * C++ default constructor.
        */
        CIdWsfMessage();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL(const TDesC8& aRefToMessageId);

    private: // Data
        CIdWsfCorrelationHeader* ipCorrelationHeader;// Not owned (owned by header)
        CIdWsfSiuHeader* ipSiuHeader;                // Owned, but element not owned
    };

#endif // IDWSF_MESSAGE_H

// End of File

