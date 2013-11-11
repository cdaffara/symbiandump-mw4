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








#ifndef IDWSF_SIU_HEADER_H
#define IDWSF_SIU_HEADER_H

// INCLUDES
#include "SenCredential.h"
#include "SenBaseFragment.h"

// CONSTANTS
_LIT8(KSiuXmlns, "urn:liberty:sb:2004-04");
_LIT8(KSiuName, "ServiceInstanceUpdate");
_LIT8(KCredentialElementLocalNameName, "Credential");

// CLASS DECLARATION
class CIdWsfSiuHeader :  public CSenBaseFragment
    {
    public: // Constructors and destructor
        
        static CIdWsfSiuHeader* NewL();
        virtual ~CIdWsfSiuHeader();
        
        // New functions
        
        TPtrC8 Endpoint();
        TPtrC8 SecurityMechId();
        const TTime& NotOnOrAfter();
        TBool IsNotOnOrAfterSet();

        /**
        * Retains ownership.
        */
        CSenCredential* Credential();

    private:
        
        /**
        * C++ default constructor.
        */
        CIdWsfSiuHeader();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // From CBaseFragment
        virtual void StartElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName,
                                   const RAttributeArray& aAttributes);
    
        virtual void EndElementL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName);

    private: // Data
        CSenCredential* ipCredential;   // Owned
        TTime iNotOnOrAfter;
    };

#endif // IDWSF_SIU_HEADER_H

// End of File
