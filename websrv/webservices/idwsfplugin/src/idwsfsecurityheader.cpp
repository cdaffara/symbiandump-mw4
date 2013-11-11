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
#include <SenIdentityProvider.h>

#include "idwsfsecurityheader.h"

namespace
    {
    _LIT8(KNameTokenFormatString8,
        "<%S:UsernameToken><%S:Username>%S</%S:Username></%S:UsernameToken>");
    // NEW 1.0:
    //_LIT8(KSecurityXmlns8,
    //  "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wsswssecurity-utility-1.0.xsd");
    //_LIT8(KSecurityXmlnsPrefix8, "wsu");

    // DRAFT:
    //_LIT8(KSecurityXmlns8,
    //  "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wsswssecurity-secext-1.0.xsd");
    //_LIT8(KSecurityXmlnsPrefix8, "wsse");

    // AOL demo:
    //_LIT8(KSecurityXmlns8, "http://schemas.xmlsoap.org/ws/2003/06/secext");
    _LIT8(KSecurityXmlnsPrefix8, "wsse");

    }


CIdWsfSecurityHeader* CIdWsfSecurityHeader::NewL()
    {
    CIdWsfSecurityHeader* pNew = new (ELeave) CIdWsfSecurityHeader;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfSecurityHeader* CIdWsfSecurityHeader::NewL(const TDesC8& aData)
    {
    CIdWsfSecurityHeader* pNew = new (ELeave) CIdWsfSecurityHeader;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aData);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfSecurityHeader::CIdWsfSecurityHeader()
    {
    }

CIdWsfSecurityHeader::~CIdWsfSecurityHeader()
    {
    }

HBufC8* CIdWsfSecurityHeader::UsernameToken8L(
                                CSenIdentityProvider &aIdentityProvider)
    {
    TPtrC8 nsPrefix = KSecurityDraftXmlnsPrefix();
    TPtrC8 username = aIdentityProvider.AuthzID();
    HBufC8* pToken = HBufC8::NewLC(50+KNameTokenFormatString8().Length()+
        username.Length() + nsPrefix.Length()*4);	//CodeScannerWarnings
    TPtr8 ptr = pToken->Des();
    ptr.Format(KNameTokenFormatString8, &nsPrefix, &nsPrefix, &username,
        &nsPrefix, &nsPrefix);
    CleanupStack::Pop(pToken);    // pToken
    return pToken;
    }

TPtrC8 CIdWsfSecurityHeader::XmlNs()
    {
    return KSecurityDraftXmlns();
    }

TPtrC8 CIdWsfSecurityHeader::XmlNsPrefix()
    {
    return KSecurityDraftXmlnsPrefix();
    }


TInt CIdWsfSecurityHeader::UsernameTokenL(const TDesC8& aUsername,
                                                   HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityXmlnsPrefix8();
    aToken = HBufC8::NewLC(KNameTokenFormatString8().Length()
        + aUsername.Length() + nsPrefix.Length()*4);
    TPtr8 ptr = aToken->Des();
    ptr.Format(KNameTokenFormatString8, &nsPrefix, &nsPrefix, &aUsername,
        &nsPrefix, &nsPrefix);
    CleanupStack::Pop(aToken);    // aToken CodeScannerWarnings
    return KErrNone;
    }
