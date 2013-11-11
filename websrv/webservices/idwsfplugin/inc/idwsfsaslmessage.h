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








#ifndef IDWSF_SASL_MESSAGE_H
#define IDWSF_SASL_MESSAGE_H

// INCLUDES
#include "msensaslmessage.h"
#include "idwsfdsqueryresponse.h"

// CONSTANTS
const TInt KStateParsingPwTransforms = 15; 

_LIT8(KSaslXmlns, "urn:liberty:sa:2004-04");
_LIT8(KSaslRequestName, "SASLRequest");
_LIT8(KSaslRequestQName, "sa:SASLRequest");
_LIT8(KSaslResponseName, "SASLResponse");
_LIT8(KSaslResponseQName, "sa:SASLRequest");

// FORWARD DECLARATIONS
class CSenPasswordTransform;

// CLASS DECLARATION
class CIdWsfSaslMessage :  public CIdWsfDsQueryResponse,
                           public MSenSaslMessage
    {
    public: // Constructors and destructor
        
        static CIdWsfSaslMessage* NewL(MSenCoreServiceManager& aServiceManager);
        static CIdWsfSaslMessage* NewL( MSenCoreServiceManager& aServiceManager,
                                        const TDesC8& aXmlns,
                                        const TDesC8& aLocalName,
                                        const TDesC8& aQName     );
        virtual ~CIdWsfSaslMessage();

        // New functions
        
        /**
        *  Accepts KNullDesC.
        */
        virtual TInt ConstructInitRequestFromL(const TDesC8& aAllMechanismNames,
                                               CSenIdentityProvider& aAccount);
        virtual TInt ConstructRequestFromL(const TDesC8& aMech);
        virtual TInt ConstructRequestFromL(const TDesC8& aMech,
                                           const TDesC8& aData);

        TBool IsContinue();
        TBool IsAbort();

        /**
        * @return KNullDesC if not set.
        */
        TPtrC8 Mechanism();

        /**
        * @return KNullDesC if not set.
        */
        TPtrC8 ParameterName();

        /**
        * @return KNullDesC if not set.
        */
        TPtrC8 Data();

        /**
        * @return array of PasswordTransform ECOM plugins
        * which may be empty if none have been parsed.
        */
        RPointerArray<CSenPasswordTransform> Transforms();

    private:
    
        /**
        * C++ default constructor.
        */
        CIdWsfSaslMessage(MSenCoreServiceManager& aServiceManager);
        
        /**
        * By default Symbian 2nd phase constructors is private.
        */
        void ConstructL();
        void ConstructL(const TDesC8& aXmlns,
                        const TDesC8& aLocalName,
                        const TDesC8& aQName);
        
        // New functions
        
        /**
        * helper
        * @return NULL if list is empty
        */
        CSenPasswordTransform* LastParsedPasswordTransform();

        RFileLogger* Log() const;
        
        // Functions from base classes
        
        // From CBaseFragment
        virtual void StartElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName,
                                   const RAttributeArray& aAttributes);
        
        virtual void EndElementL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName);


    protected: // Data
    
        // to access base64, encoding/decoding
        MSenCoreServiceManager& iServiceManager;

    private: // Data
        HBufC8* ipMechanism;         // Owned
        HBufC8* ipParameterName;     // Owned
        RPointerArray<CSenPasswordTransform> iTransforms; // NOT owned
    };

#endif // IDWSF_SASL_MESSAGE_H

// End of File

