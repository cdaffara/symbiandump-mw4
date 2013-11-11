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
#include <xml/dom/xmlengbinarycontainer.h>

#include "SenWsSecurityHeader2.h"
#include "sendebug.h"

namespace
    {
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

EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewL(
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = CSenWsSecurityHeader2::NewLC(aDocument,
                                                               aElement);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewLC(
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = new (ELeave) CSenWsSecurityHeader2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aDocument, aElement);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewL(
                                                    const TDesC8& aData,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = CSenWsSecurityHeader2::NewLC(aData,
                                                               aDocument,
                                                               aElement);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewLC(
                                                    const TDesC8& aData,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = new (ELeave) CSenWsSecurityHeader2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aData, aDocument, aElement);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewL(
                                                    const TDesC8& aData,
                                                    const TDesC8& aSecurityNs,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = CSenWsSecurityHeader2::NewLC(aData,
                                                               aSecurityNs,
                                                               aDocument,
                                                               aElement);
    CleanupStack::Pop(pNew);
    return pNew;
    }
    
EXPORT_C CSenWsSecurityHeader2* CSenWsSecurityHeader2::NewLC(
                                                    const TDesC8& aData,
                                                    const TDesC8& aSecurityNs,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    CSenWsSecurityHeader2* pNew = new (ELeave) CSenWsSecurityHeader2;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aData, aSecurityNs, aDocument, aElement);
    return pNew;
    }

EXPORT_C CSenWsSecurityHeader2::CSenWsSecurityHeader2()
    {
    }

EXPORT_C void CSenWsSecurityHeader2::BaseConstructL(RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    RAttributeArray attrArray;
    CSenFragmentBase::BaseConstructL(XmlNs(), KSecurityName, XmlNsPrefix(),
                                     attrArray, aElement, aDocument);
    }

EXPORT_C void CSenWsSecurityHeader2::BaseConstructL(const TDesC8& aData,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    BaseConstructL(aDocument, aElement);
    if ( aData.Length() )
        {
        TXmlEngElement element = AsElementL();
        TPtrC8 content = element.Text();
        if ( content.Length() > 0 )
            {
            HBufC8* pContent = HBufC8::NewLC( content.Length() + aData.Length() );
            TXmlEngElement element = AsElementL();
            TPtr8 ptrContent = pContent->Des();
            ptrContent.Append(content);
            ptrContent.Append(aData);
            element.SetTextNoEncL(*pContent);
            CleanupStack::PopAndDestroy(pContent);    
            }
        else
            {
            element.SetTextNoEncL(aData);
            }        
        }
    }
    
EXPORT_C void CSenWsSecurityHeader2::BaseConstructL(const TDesC8& aData,
                                                    const TDesC8& aSecurityNs,
                                                    RSenDocument aDocument,
                                                    TXmlEngElement aElement)
    {
    RAttributeArray attrArray;
    CSenFragmentBase::BaseConstructL(aSecurityNs, KSecurityName, XmlNsPrefix(),
                                     attrArray, aElement, aDocument);
    if ( aData.Length() )
        {
        TXmlEngElement element = AsElementL();
        TPtrC8 content = element.Text();
        if ( content.Length() > 0 )
            {
            HBufC8* pContent = HBufC8::NewLC( content.Length() + aData.Length() );
            TXmlEngElement element = AsElementL();
            TPtr8 ptrContent = pContent->Des();
            ptrContent.Append(content);
            ptrContent.Append(aData);
            element.SetTextNoEncL(*pContent);
            CleanupStack::PopAndDestroy(pContent);    
            }
        else
            {
            element.SetTextNoEncL(aData);
            }        
        }
    }

EXPORT_C CSenWsSecurityHeader2::~CSenWsSecurityHeader2()
    {
    }

EXPORT_C HBufC8* CSenWsSecurityHeader2::UsernameTokenL(CSenIdentityProvider &aIdentityProvider)
    {
    TPtrC8 username = aIdentityProvider.AuthzID();
    HBufC8* pToken = NULL;
    User::LeaveIfError(UsernameTokenL(username, pToken));
    return pToken;
    }


EXPORT_C TPtrC8 CSenWsSecurityHeader2::XmlNs()
    {
    if ( iElement.NotNull() )
        {
        if ( iElement.NamespaceUri() != KNullDesC8 )
            {
            return iElement.NamespaceUri();
            }
        }

    return KSecurityXmlNs();
    }

EXPORT_C TPtrC8 CSenWsSecurityHeader2::XmlNsPrefix()
    {
    if ( iElement.NotNull() )
        {
        if ( iElement.Prefix() != KNullDesC8 )
            {
            return iElement.Prefix();
            }
        }
        
    return KSecurityXmlNsPrefix();
    }

EXPORT_C TInt CSenWsSecurityHeader2::UsernameTokenL(const TDesC8& aUsername, HBufC8*& aToken)
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


EXPORT_C HBufC8* CSenWsSecurityHeader2::UsernameTokenL(CSenIdentityProvider &aIdentityProvider,
                                                      CSenWsSecurityHeader2::TPasswordType aType)
    {
    TPtrC8 username = aIdentityProvider.AuthzID();
    TPtrC8 password = aIdentityProvider.Password();
    HBufC8* pToken = NULL;
    User::LeaveIfError(UsernameTokenL(username, password, aType, pToken));
    return pToken;
    }

EXPORT_C TInt CSenWsSecurityHeader2::UsernameTokenL(const TDesC8& aUsername,
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
    ptr.Format(KUsernameTokenStartTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameFmt, &nsPrefix, &aUsername, &nsPrefix);
    ptr.AppendFormat(KPasswordStartTagDefaultFmt,
                     &nsPrefix, &aPassword); // no Type -attribute used
    ptr.AppendFormat(KPasswordEndTagFmt, &nsPrefix);
    ptr.AppendFormat(KUsernameTokenEndTag, &nsPrefix);
    CleanupStack::Pop();    // aToken
    return KErrNone;
    }

EXPORT_C TInt CSenWsSecurityHeader2::UsernameTokenL(const TDesC8& aUsername,
                                    const TDesC8& aPassword,
                                    CSenWsSecurityHeader2::TPasswordType aType,
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

EXPORT_C TInt CSenWsSecurityHeader2::TimestampL(const TDesC8& aCreated, const TDesC8& aExpires, HBufC8*& aToken)
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

EXPORT_C TInt CSenWsSecurityHeader2::TimestampL(const TDesC8& aCreated, HBufC8*& aToken)
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

//EXPORT_C TInt CSenWsSecurityHeader2::SecurityTokenReferenceL(const TDesC8& aURI, HBufC8*& aToken)
//    {
//    //to do
//    }


// End of File
