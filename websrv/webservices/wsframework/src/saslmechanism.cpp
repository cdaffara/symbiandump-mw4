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
* Description:        
*
*/








// INCLUDE FILES
#include "saslmechanism.h" 

#include <SenIdentityProvider.h>

EXPORT_C CSaslMechanism* CSaslMechanism::NewL()
    {
    CSaslMechanism* pNew = NewLC();
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSaslMechanism* CSaslMechanism::NewLC()
    {
    CSaslMechanism* pNew = new (ELeave) CSaslMechanism();
    CleanupStack::PushL(pNew);
    return pNew;
    }

EXPORT_C CSaslMechanism::CSaslMechanism()
:   ipAccount(NULL),
    ipIdentityManager(NULL)
    {
    }

EXPORT_C CSaslMechanism::~CSaslMechanism()
    {
    iTransforms.ResetAndDestroy(); // were owned
    }

EXPORT_C void CSaslMechanism::ClearPasswordL()
    {
    //subclasses should implement this.
    }

EXPORT_C HBufC8* CSaslMechanism::Password8L()
    {
	HBufC8* pPassWd = NULL;
	TSenAuthentication output;
	TPckgBuf<TSenAuthentication> outputPair(output);
	if(ipAccount)
		{
	    TInt error = ipIdentityManager->AuthenticationForL(*ipAccount, outputPair);
	    if (error == KErrNone)
		    {
		    pPassWd = outputPair().iPassword.AllocL();
		    }
		}
    return pPassWd;
    }

EXPORT_C void CSaslMechanism::InitFrom(CSaslMechanism aSaslMechanism)
    {
    ipIdentityManager = aSaslMechanism.ipIdentityManager;
    ipAccount = aSaslMechanism.ipAccount;
    iTransforms = aSaslMechanism.iTransforms;
    }

EXPORT_C HBufC8* CSaslMechanism::TransformL(TPtrC8 aPassphrase) const
    {
    HBufC8* pPassword = aPassphrase.AllocL();
    HBufC8* pTransformed = NULL;
    for(TInt i=0; i<iTransforms.Count(); i++)
        {
        if(pPassword)
            {
            CleanupStack::PushL(pPassword);
            pTransformed = iTransforms[i]->TransformL(*pPassword);
            CleanupStack::PopAndDestroy(); // pPassword
            pPassword = pTransformed;
            pTransformed = NULL;
            }
        }
    return pPassword;
    }

EXPORT_C TInt CSaslMechanism::MaxAttempts()
    {
    return 0; // 0 means that the system default can be applied
    }

EXPORT_C void CSaslMechanism::SetIdentity(
                                      CSenIdentityProvider* aIdentityProvider,
                                      MSenIdentityManager* aIdentityManager)
    {
    ipIdentityManager = aIdentityManager;
    ipAccount = aIdentityProvider;
    }

EXPORT_C TBool CSaslMechanism::IsPasswordFromUser()
    {
    return EFalse;
    }

EXPORT_C CSenIdentityProvider& CSaslMechanism::Account()
    {
    return *ipAccount;
    }

EXPORT_C void CSaslMechanism::SetTransforms(
                        RPointerArray<CSenPasswordTransform> aList)
    {
	//deallocate the original objects and the array itself which were owned
	if(iTransforms.Count())
		{
		iTransforms.ResetAndDestroy();
		}
    iTransforms = aList;
    }

// End of File
