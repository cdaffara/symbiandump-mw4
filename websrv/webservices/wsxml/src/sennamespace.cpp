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
#include "SenNameSpace.h"
#include "SenXmlConstants.h"

EXPORT_C CSenNamespace* CSenNamespace::NewL(const TDesC8& aUri)
    {
    // Note: intentionally calling the other version of NewLC to leave out
    // one call in call stack
    CSenNamespace* pNew = CSenNamespace::NewLC(KNullDesC8, aUri);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenNamespace* CSenNamespace::NewLC(const TDesC8& aUri)
    {
    CSenNamespace* pNew = CSenNamespace::NewLC(KNullDesC8, aUri);
    return pNew;
    }

EXPORT_C CSenNamespace* CSenNamespace::NewL(const TDesC8& aPrefix, const TDesC8& aUri)
    {
    CSenNamespace* pNew = CSenNamespace::NewLC(aPrefix, aUri);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenNamespace* CSenNamespace::NewLC(const TDesC8& aPrefix, const TDesC8& aUri)
    {
    CSenNamespace* pNew = new (ELeave) CSenNamespace();
    CleanupStack::PushL(pNew);

    pNew->ConstructL(aPrefix, aUri);
    return pNew;
    }


CSenNamespace::CSenNamespace()
:   ipPrefix(NULL),
    ipUri(NULL)
    {

    }

void CSenNamespace::ConstructL(const TDesC8& aPrefix, const TDesC8& aUri)
    {
    if (aPrefix.Length() > -1)
        {
        ipPrefix = aPrefix.AllocL();
        }

    if (aUri.Length() > 0)
        {
        ipUri = aUri.AllocL();
        }
    }

EXPORT_C CSenNamespace::~CSenNamespace()
    {
    delete ipPrefix;
    delete ipUri;
    }

EXPORT_C TBool CSenNamespace::Compare(const TDesC8& aOtherPrefix,
                                       const TDesC8& aOtherUri) const
    {
    TBool retVal(EFalse);
    if (aOtherUri == URI())
        {
        if (aOtherPrefix == KNullDesC8 || aOtherPrefix == Prefix())
            {
            retVal = ETrue;
            }
        }
    return retVal;
    }

EXPORT_C TBool CSenNamespace::Compare(
                                const CSenNamespace& aOtherNamespace) const
    {
    return this->Compare(aOtherNamespace.Prefix(), aOtherNamespace.URI());
    }

EXPORT_C TBool CSenNamespace::MatchesL(
                                const CSenNamespace& aOtherNamespace) const
    {
    return (*ipUri == aOtherNamespace.URI());
    }

EXPORT_C const TDesC8& CSenNamespace::URI() const
    {
    if(ipUri)
        {
        return *ipUri;
        }
    else
        {
        return KNullDesC8;
        }
    }

EXPORT_C void CSenNamespace::SetUriL(const TDesC8& aUri)
    {
    HBufC8* pUri = NULL;
    if(aUri!=KNullDesC8)
        {
        pUri = aUri.AllocL();
        }
    delete ipUri;
    ipUri = pUri;
    }

EXPORT_C const TDesC8& CSenNamespace::Prefix() const
    {
    if(ipPrefix)
        {
        return *ipPrefix;
        }
    else
        {
        return KNullDesC8;
        }
    }

EXPORT_C void CSenNamespace::SetPrefixL(const TDesC8& aPrefix)
    {
    HBufC8* pPrefix = NULL;
    if(aPrefix!=KNullDesC8)
        {
        pPrefix = aPrefix.AllocL();
        }
    delete ipPrefix;
    ipPrefix = pPrefix;
    }



EXPORT_C void CSenNamespace::InsertL(RBufWriteStream& aWs)
    {
    if(!ipUri)
        {
        //SENDEBUG((_L("CSenNamespace::InsertL illegal ns, it has no URI!")));
        return;
        }

    aWs.WriteL(KSenSpaceXmlns);
    if(ipPrefix && ipPrefix->Length()>0)
        {
        aWs.WriteL(KSenColon);
        aWs.WriteL(*ipPrefix);
        }
    aWs.WriteL(KSenEquals);
    aWs.WriteL(*ipUri);
    }

//  End of File
