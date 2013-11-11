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








#ifndef M_SEN_CREDENTIAL_MANAGER_H
#define M_SEN_CREDENTIAL_MANAGER_H

// INCLUDES
#include <e32base.h>

//#include "MSenDAO.h"

// FORWARD DECLARATIONS
class CSenWSDescription;
class CSenIdentityProvider;
class CSenInternalCredential;
class RSenCredentialPtr;
class CSenXMLDAO;

typedef RPointerArray<CSenInternalCredential> RSenCredentialArray;
typedef RArray<RSenCredentialPtr> RSenCredentialPtrArray;

class MSenCredentialManager
    {
    
    friend class CSenXMLDAO; // only component that saves Credential DB outside CRED-MGR
    
    public:
		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
								  RSenCredentialArray& aCredentials) = 0;

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
								  const CSenIdentityProvider& aIdP,
								  RSenCredentialArray& aCredentials) = 0;

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
								  RSenCredentialPtrArray& aCredentials) = 0;

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
								  const CSenIdentityProvider& aIdP,
								  RSenCredentialPtrArray& aCredentials) = 0;

		virtual RSenCredentialPtr AddCredentialL(CSenInternalCredential* apCredential,
		                                          TInt& aErrorTo) = 0;

		virtual RSenCredentialPtr AddCredentialL(CSenIdentityProvider* apIdP,
									              CSenInternalCredential* apCredential,
									              TInt& aErrorTo) = 0;

		virtual RSenCredentialPtr AddCredentialL(const TDesC8& aCredential,
		                                          TInt& aErrorTo) = 0;

		virtual RSenCredentialPtr AddCredentialL(CSenIdentityProvider* apIdP,
                                                 const TDesC8& aCredential,
                                                 TInt& aErrorTo) = 0;

		virtual TInt RemoveCredentialsL(const CSenWSDescription& aPattern) = 0;

		virtual TInt RemoveCredentialsL(const CSenWSDescription& aPattern,
										const CSenIdentityProvider& aIdP) = 0;

        virtual TInt RemoveCredentialsL(const TDesC8& aProviderId) = 0;
										
        virtual TInt RemoveCredentialL(TInt aInternalCredentialId) = 0;	//codescannerwarnings
        
        virtual RSenCredentialPtr CredentialL(TInt aInternalCredentialId,
                                              TInt& aErrorTo) = 0;

        virtual RSenCredentialPtr UpdateCredentialL(TInt aInternalCredentialId,
                                                    CSenInternalCredential* apCredential,
                                                    TInt& aErrorTo) = 0;
        
        virtual RSenCredentialPtr UpdateCredentialL(TInt aInternalCredentialId,
                                                    const TDesC8& aCredential,
                                                    TInt& aErrorTo) = 0;
                                                    
    private:
        virtual TInt SaveCredentialDB() = 0;
    };

#endif // M_SEN_CREDENTIAL_MANAGER_H

// End of File
