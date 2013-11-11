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








#ifndef SASL_MECHANISM_H
#define SASL_MECHANISM_H

// INCLUDES
#include <e32base.h>

#include <SenIdentityProvider.h>

#include "senpasswordtransform.h"

#include "msenidentitymanager.h" // internal IdentityManager\inc

// CLASS DECLARATION

class CSaslMechanism : public CBase
    {
    public: // Constructors and destructor
        IMPORT_C static CSaslMechanism* NewL();
        IMPORT_C static CSaslMechanism* NewLC();
        IMPORT_C ~CSaslMechanism();

        // New functions
        IMPORT_C virtual void ClearPasswordL();
        IMPORT_C virtual HBufC8* Password8L();
        IMPORT_C virtual void InitFrom( CSaslMechanism aSaslMechanism );
        IMPORT_C virtual TInt MaxAttempts();
        IMPORT_C virtual void SetIdentity(
                              CSenIdentityProvider* aIdentityProvider,
                              MSenIdentityManager* aIdentityManager);

        IMPORT_C virtual TBool IsPasswordFromUser();
        IMPORT_C virtual CSenIdentityProvider& Account();
        IMPORT_C virtual void SetTransforms(
                                RPointerArray<CSenPasswordTransform> aList);

        IMPORT_C virtual HBufC8* TransformL(TPtrC8 aPassphrase) const;

    protected:
    
        /**
        * C++ default constructor.
        */
        IMPORT_C CSaslMechanism();

    protected: // Data
        CSenIdentityProvider*   ipAccount;          // NOT owned
        MSenIdentityManager*    ipIdentityManager;  // NOT owned
        RPointerArray<CSenPasswordTransform> iTransforms; // owned
    };

#endif //SASL_MECHANISM_H

// End of File
