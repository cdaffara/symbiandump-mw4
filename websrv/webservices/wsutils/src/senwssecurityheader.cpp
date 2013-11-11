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

#include "SenWsSecurityHeader.h"
#include "sendebug.h"

namespace
    {
    _LIT8(KColon, ":");

    _LIT8(KUsernameTokenStartTagFmt,    "<%S:UsernameToken>");

    _LIT8(KUsernameFmt,                 "<%S:Username>%S</%S:Username>");

    _LIT8(KPasswordStartTagDefaultFmt,  "<%S:Password>%S");
    _LIT8(KPasswordStartTagTypeFmt,     "<%S:Password Type=\"%S:%S\">%S");
    _LIT8(KPasswordEndTagFmt,           "</%S:Password>");

    _LIT8(KUsernameTokenEndTag,         "</%S:UsernameToken>");

    _LIT8(KTimestampExpiresFormatString8,
        "<%S:Timestamp xmlns:%S=\"%S\"><%S:Created>%S</%S:Created><%S:Expires>%S</%S:Expires></%S:Timestamp>");

    _LIT8(KTimestampFormatString8,
        "<%S:Timestamp xmlns:%S=\"%S\"><%S:Created>%S</%S:Created></%S:Timestamp>");

    _LIT8(KBinarySecurityTokenValueTypeFmt,             "<%S:BinarySecurityToken wsu:Id=\"token\" ValueType=\"%S\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">%S</%S:BinarySecurityToken>");
    _LIT8(KBinarySecurityTokenFmt,             "<%S:BinarySecurityToken wsu:Id=\"token\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">%S</%S:BinarySecurityToken>");
 
    /*
    TInt TokenL(const TDesC8& aSecurityNsPrefix, const TDesC8& aUsername, HBufC8*& aToken)
        {
        aToken = HBufC8::NewLC( KUsernameTokenStartTagFmt().Length() +
                                KUsernameFmt().Length() +
                                KUsernameTokenEndTag().Length() +
                                aUsername.Length() + 
                                aSecurityNsPrefix.Length() * 4 );

        TPtr8 ptr = aToken->Des();
        ptr.Format(KUsernameTokenStartTagFmt, &aSecurityNsPrefix);
        ptr.AppendFormat(KUsernameFmt, &aSecurityNsPrefix, &aUsername, &aSecurityNsPrefix);
        ptr.AppendFormat(KUsernameTokenEndTag, &aSecurityNsPrefix);
        CleanupStack::Pop();    // aToken
        return KErrNone;
        }
    */

    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewL()
    {
    CSenWsSecurityHeader* pNew = CSenWsSecurityHeader::NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewLC()
    {
    CSenWsSecurityHeader* pNew = new (ELeave) CSenWsSecurityHeader;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewL(const TDesC8& aData)
    {
    CSenWsSecurityHeader* pNew = CSenWsSecurityHeader::NewLC(aData);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewLC(const TDesC8& aData)
    {
    CSenWsSecurityHeader* pNew = new (ELeave) CSenWsSecurityHeader;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aData);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewL(const TDesC8& aData, const TDesC8& aSecurityNs)
    {
    CSenWsSecurityHeader* pNew = CSenWsSecurityHeader::NewLC(aData, aSecurityNs);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader* CSenWsSecurityHeader::NewLC(const TDesC8& aData, const TDesC8& aSecurityNs)
    {
    CSenWsSecurityHeader* pNew = new (ELeave) CSenWsSecurityHeader;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aData, aSecurityNs);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader::CSenWsSecurityHeader()
    {
    }

EXPORT_C void CSenWsSecurityHeader::BaseConstructL()
    {
    // create and push pQualifiedName
    HBufC8 *pQualifiedName = HBufC8::NewLC(XmlNsPrefix().Length()+
                                        KColon().Length() +
                                        KSecurityName().Length());

    TPtr8 qualified = pQualifiedName->Des();
    qualified.Append(XmlNsPrefix());
    qualified.Append(KColon);
    qualified.Append(KSecurityName);
    CSenBaseFragment::BaseConstructL(XmlNs(), KSecurityName, qualified);

    CleanupStack::PopAndDestroy(); // pQualifiedName
    }

EXPORT_C void CSenWsSecurityHeader::BaseConstructL(const TDesC8& aData, const TDesC8& aSecurityNs)
    {
    // create and push pQualifiedName
    HBufC8 *pQualifiedName = HBufC8::NewLC(XmlNsPrefix().Length()+
                                        KColon().Length() +
                                        KSecurityName().Length());

    TPtr8 qualified = pQualifiedName->Des();
    qualified.Append(XmlNsPrefix());
    qualified.Append(KColon);
    qualified.Append(KSecurityName);
    CSenBaseFragment::BaseConstructL(aSecurityNs, KSecurityName, qualified);
    CleanupStack::PopAndDestroy(); // pQualifiedName
    if (aData.Length())
        {
        AsElement().SetContentL(aData);    
        }
    }

EXPORT_C void CSenWsSecurityHeader::BaseConstructL(const TDesC8& aData)
    {
    BaseConstructL();
    AsElement().SetContentL(aData);
    }

EXPORT_C CSenWsSecurityHeader::~CSenWsSecurityHeader()
    {
    }

EXPORT_C HBufC8* CSenWsSecurityHeader::UsernameTokenL(CSenIdentityProvider &aIdentityProvider)
    {
    TPtrC8 username = aIdentityProvider.AuthzID();
    HBufC8* pToken = NULL;
    User::LeaveIfError(UsernameTokenL(username, pToken));
    return pToken;

    /*
    TPtrC8 nsPrefix = XmlNsPrefix();
    HBufC8* aToken = HBufC8::NewLC( KUsernameTokenStartTagFmt().Length() +
                                    KUsernameFmt().Length() +
                                    KUsernameTokenEndTag().Length() +
                                    username.Length() + 
                                    nsPrefix.Length() * 4 );

    _LIT8(KUsernameTokenStartTagFmt, "<%S:UsernameToken>");
    _LIT8(KUsernameFmt,              "<%S:UsernameToken><%S:Username>%S</%S:Username>");
    _LIT8(KUsernameTokenEndTag,      "</%S:UsernameToken>");

    TPtr8 ptr = aToken->Des();
    ptr.Format(KUsernameTokenStartTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameFmt, &nsPrefix, &username, &nsPrefix);
    ptr.AppendFormat(KUsernameTokenEndTag, &nsPrefix);

    CleanupStack::Pop();    // aToken
    return aToken;
    */
    }


EXPORT_C TPtrC8 CSenWsSecurityHeader::XmlNs()
    {
    // Note that AsElement() will panic, if CSenBaseFragment::ipElement == NULL
    if(ipElement)
        {
        return AsElement().NamespaceURI();
        }
    else
        {
        return KSecurityXmlNs();  
        }
    }

EXPORT_C TPtrC8 CSenWsSecurityHeader::XmlNsPrefix()
    {
    // Note that AsElement() will panic, if CSenBaseFragment::ipElement == NULL
    if(ipElement)
        {
        return AsElement().NsPrefix();
        }
    else
        {
        return KSecurityXmlNsPrefix();
        }
    }

EXPORT_C TInt CSenWsSecurityHeader::UsernameTokenL(const TDesC8& aUsername, HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityXmlNsPrefix();
    aToken = HBufC8::NewLC( KUsernameTokenStartTagFmt().Length() +
                            KUsernameFmt().Length() +
                            KUsernameTokenEndTag().Length() +
                            aUsername.Length() + 
                            nsPrefix.Length() * 4 );

    TPtr8 ptr = aToken->Des();
    ptr.Format(KUsernameTokenStartTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameFmt, &nsPrefix, &aUsername, &nsPrefix);
    ptr.AppendFormat(KUsernameTokenEndTag, &nsPrefix);
    CleanupStack::Pop();    // aToken
    return KErrNone;
    }


EXPORT_C HBufC8* CSenWsSecurityHeader::UsernameTokenL(CSenIdentityProvider &aIdentityProvider,
                                                      CSenWsSecurityHeader::TPasswordType aType)
    {
    TPtrC8 username = aIdentityProvider.AuthzID();
    TPtrC8 password = aIdentityProvider.Password();
    HBufC8* pToken = NULL;
    User::LeaveIfError(UsernameTokenL(username, password, aType, pToken));
    return pToken;
    }

EXPORT_C TInt CSenWsSecurityHeader::UsernameTokenL(const TDesC8& aUsername,
                                    const TDesC8& aPassword,
                                    HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityXmlNsPrefix();
    aToken = HBufC8::NewLC( KUsernameTokenStartTagFmt().Length() +
                            KUsernameFmt().Length() +
                            KUsernameTokenEndTag().Length() +
                            KPasswordStartTagDefaultFmt().Length() +
                            KPasswordEndTagFmt().Length() +
                            aPassword.Length() +
                            aUsername.Length() + 
                            nsPrefix.Length() * 6 );

    TPtr8 ptr = aToken->Des();
//    ptr.Format(KNameTokenFormat, &nsPrefix, &nsPrefix, &aUsername, &nsPrefix, &nsPrefix);
    ptr.Format(KUsernameTokenStartTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameFmt, &nsPrefix, &aUsername, &nsPrefix);
    ptr.AppendFormat(KPasswordStartTagDefaultFmt, 
                     &nsPrefix, &aPassword); // no Type -attribute used
    ptr.AppendFormat(KPasswordEndTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameTokenEndTag, &nsPrefix);
    CleanupStack::Pop();    // aToken
    return KErrNone;
    }

EXPORT_C TInt CSenWsSecurityHeader::UsernameTokenL(const TDesC8& aUsername,
                                    const TDesC8& aPassword,
                                    CSenWsSecurityHeader::TPasswordType aType,
                                    HBufC8*& aToken)
    {
    switch(aType)
        {
        case EText:
            {
            return UsernameTokenL(aUsername, aPassword, aToken);
            }
        case EDigest:
            {
            TPtrC8 nsPrefix = KSecurityXmlNsPrefix();
            TPtrC8 typeAttr(KSecurityAttrTypeDigest);

            aToken = HBufC8::NewLC( KUsernameTokenStartTagFmt().Length() +
                                    KUsernameFmt().Length() +
                                    KUsernameTokenEndTag().Length() +
                                    KPasswordStartTagTypeFmt().Length() +
                                    KPasswordEndTagFmt().Length() +
                                    aUsername.Length() + 
                                    aPassword.Length() +
                                    typeAttr.Length() +
                                    nsPrefix.Length() * 7 );
            TPtr8 ptr = aToken->Des();
            ptr.Format(KUsernameTokenStartTagFmt, &nsPrefix);
            ptr.AppendFormat(KUsernameFmt, &nsPrefix, &aUsername, &nsPrefix);
            ptr.AppendFormat(KPasswordStartTagTypeFmt, &nsPrefix, &nsPrefix, 
                             &typeAttr, &aPassword); // Type = wsse:PasswordDigest
            ptr.AppendFormat(KPasswordEndTagFmt, &nsPrefix);
            ptr.AppendFormat(KUsernameTokenEndTag, &nsPrefix);
            CleanupStack::Pop();    // aToken
            return KErrNone;
            }
        default:
            {
            return KErrNotSupported;
            }
        }
    }

EXPORT_C TInt CSenWsSecurityHeader::BinarySecurityTokenL(const TDesC8& aContent, const TDesC8& aValueType, HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityXmlNsPrefix();
    aToken = HBufC8::NewLC( KBinarySecurityTokenValueTypeFmt().Length() +
                            aContent.Length() + 
                            aValueType.Length() + 
                            nsPrefix.Length() * 2 );

    TPtr8 ptr = aToken->Des();
    ptr.Format(KBinarySecurityTokenValueTypeFmt, &nsPrefix, &aValueType, &aContent, &nsPrefix);
    CleanupStack::Pop();    // aToken
    return KErrNone;
    }

EXPORT_C TInt CSenWsSecurityHeader::BinarySecurityTokenL(const TDesC8& aContent, HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityXmlNsPrefix();
    aToken = HBufC8::NewLC( KBinarySecurityTokenFmt().Length() +
                            aContent.Length() +
                            nsPrefix.Length() * 2 );

    TPtr8 ptr = aToken->Des();
    ptr.Format(KBinarySecurityTokenFmt, &nsPrefix, &aContent, &nsPrefix);
    CleanupStack::Pop();    // aToken
    return KErrNone;
    }

EXPORT_C TInt CSenWsSecurityHeader::TimestampL(const TDesC8& aCreated, const TDesC8& aExpires, HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityUtilityXmlNsPrefix();
    aToken = HBufC8::NewLC(KTimestampExpiresFormatString8().Length()
                            + aCreated.Length() 
                            + aExpires.Length() 
                            + nsPrefix.Length()*7
                            + KSecurityUtilityXmlNs().Length());

    TPtr8 ptr = aToken->Des();
    ptr.Format(KTimestampExpiresFormatString8, 
               &nsPrefix, 
               &nsPrefix, 
               &KSecurityUtilityXmlNs(),
               &nsPrefix, 
               &aCreated,
               &nsPrefix, 
               &nsPrefix, 
               &aExpires, 
               &nsPrefix, 
               &nsPrefix);
    CleanupStack::Pop(aToken);
    return KErrNone;
    }
    
EXPORT_C TInt CSenWsSecurityHeader::TimestampL(const TDesC8& aCreated, HBufC8*& aToken)
    {
    TPtrC8 nsPrefix = KSecurityUtilityXmlNsPrefix();
    aToken = HBufC8::NewLC(KTimestampFormatString8().Length()
                            + aCreated.Length() 
                            + nsPrefix.Length()*5
                            + KSecurityUtilityXmlNs().Length());

    TPtr8 ptr = aToken->Des();
    ptr.Format(KTimestampFormatString8, 
               &nsPrefix, 
               &nsPrefix, 
               &KSecurityUtilityXmlNs(),
               &nsPrefix, 
               &aCreated,
               &nsPrefix, 
               &nsPrefix);
    CleanupStack::Pop(aToken);
    return KErrNone;
    }
    
//EXPORT_C TInt CSenWsSecurityHeader::SecurityTokenReferenceL(const TDesC8& aURI, HBufC8*& aToken)
//    {
//    //to do 
//    }


// End of File
