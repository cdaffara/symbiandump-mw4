/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <e32std.h>
#include <badesca.h>

#include <SenDomFragmentBase.h>

#include "senatommessage.h"

#include "sendebug.h"
#include "senlogger.h"
#include <xml/dom/xmlengnodelist.h>

CSenAtomMessage::CSenAtomMessage() 
//:iTitle(NULL), iId(NULL), iUpdated(NULL), iPublished(NULL)
// Automatically all members will be zeroed in  CBase derived classes, so initialization list is required only when to initializ other than zero
// This fixes the RVCT Compiler warning #1299-D
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomMessage::CSenAtomMessage()");     
    }

EXPORT_C CSenAtomMessage::~CSenAtomMessage()
    {
    if(iTitle)
        {
        delete iTitle;
        }
    if(iIdTag)
        {
        delete iIdTag;
        }
    if(iUpdated)
        {
        delete iUpdated;
        }
    if(iPublished)
        {
        delete iPublished;
        }
    
    }

EXPORT_C TInt64 CSenAtomMessage::Id() const
    {
    return iId;
    }

EXPORT_C void CSenAtomMessage::OnStartElementL(const RTagInfo& aElement,
                                                 const RAttributeArray& /* aAttributes*/,
                                                 TInt /*aErrorCode*/)
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomMessage::OnStartElementL");
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

    }

EXPORT_C void CSenAtomMessage::OnEndElementL(
                                            const RTagInfo& aElement,
                                            TInt /*aErrorCode*/)
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomMessage::OnEndElementL");
    
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();

    }


EXPORT_C CSenFragmentBase* CSenAtomMessage::CreateBodyFragmentL(const TDesC8& aNsUri,
                                                                  const TDesC8& aLocalName,
                                                                  const TDesC8& aPrefix,
                                                                  const RAttributeArray& aAttributes,
                                                                  TXmlEngElement& aParent,
                                                                  RSenDocument& aOwnerDocument)
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomMessage::CreateBodyFragmentL");
    return CSenDomFragmentBase::NewL(aNsUri, aLocalName, aPrefix, aAttributes, aParent, aOwnerDocument);    
    }
    
EXPORT_C TBool CSenAtomMessage::IsSafeToCast(MSenMessage::TClass aType)
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomMessage::IsSafeToCast(aType)");
    if ( aType == MSenMessage::EAtomMessage )
        {
        return ETrue;
        }

    return EFalse;
    }
EXPORT_C MSenMessage::TClass CSenAtomMessage::Type()
    {
    return MSenMessage::EAtomMessage;
    }   
    
EXPORT_C MSenMessage::TDirection CSenAtomMessage::Direction()
    {
    return MSenMessage::EOutbound;
    }
EXPORT_C TInt CSenAtomMessage::SetContext(MSenMessageContext* /*apNotOwnedContext*/)
    {
    return KErrNotSupported;
    }
EXPORT_C MSenMessageContext* CSenAtomMessage::Context()
    {
    return NULL;
    }    
EXPORT_C TInt CSenAtomMessage::SetProperties(MSenProperties* /*apOwnedProperties*/)
    {
    return KErrNotSupported;
    }
EXPORT_C MSenProperties* CSenAtomMessage::Properties()
    {
    return NULL;
    }
EXPORT_C TInt CSenAtomMessage::TxnId()
    {
    return KErrNotSupported;
    }

EXPORT_C MSenMessage* CSenAtomMessage::CloneL()
    {
    return NULL;
    }


EXPORT_C const TDesC8& CSenAtomMessage::Title() const
    {
    if ( iTitle )
    return *iTitle;
    else
    return KNullDesC8();
    }

EXPORT_C void CSenAtomMessage::SetTitleL( const TDesC8& aTitle )
    {
    if ( iTitle )
        {
        delete iTitle;
        iTitle = 0;
        }
    if ( aTitle.Length()> 0 )
        {

        iTitle = aTitle.AllocL();
        TXmlEngElement message = AsElementL();
        TXmlEngElement title;
        title = message.AddNewElementL(KSenAtomTitleName);
        title.SetTextNoEncL(aTitle);
        }
    }


EXPORT_C TInt CSenAtomMessage::LinkCountL()
    {
    TXmlEngElement message = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    //const TPtrC8 tNsUri = NsUri();
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomLinkName, KSenAtomSpace);
    TInt a = list.Count();
    CleanupStack::PopAndDestroy(&list);
    return a;
    }

EXPORT_C void CSenAtomMessage::AddLinkL( const TDesC8& aHref, const TDesC8& aRel, const TDesC8& aType, const TDesC8& aHrefLang, const TDesC8& aTitle, const TDesC8& aLength)
    {

    TXmlEngElement message = AsElementL();
    TXmlEngElement link;
    link = message.AddNewElementL(KSenAtomLinkName);

    if(aHref!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomHref,aHref);
        }
    if(aRel!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomRel,aRel);
        }
    if(aType!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomType,aType);
        }
    if(aHrefLang!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomHrefLang,aHrefLang);
        }
    if(aLength!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomLength,aLength);
        }
    if(aTitle!=KNullDesC8)
        {
        link.AddNewAttributeSameNsL(KSenAtomTitle,aTitle);
        }
    }

EXPORT_C void CSenAtomMessage::RemoveLinkL(TInt aIndex )
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement link;
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomLinkName, KSenAtomSpace);
    TInt count =0;
    while(list.HasNext()){
        link = list.Next();
        if(count == aIndex){
            link.Remove();
            break;
        }
        count++;
    }
    CleanupStack::PopAndDestroy(&list);
    }

EXPORT_C void CSenAtomMessage::RemoveLinkL(const TDesC8& aHref )
    {

    TXmlEngElement message = AsElementL();
    TXmlEngElement link;
    RXmlEngNodeList<TXmlEngElement> list;
    
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomLinkName, KSenAtomSpace);
    while(list.HasNext()){
        link = list.Next();
        TPtrC8 icPtr = link.AttributeValueL(KSenAtomHref, KSenAtomSpace);
        if(aHref.Compare(icPtr)==0){
            link.Remove();
            break;
        }
    }
    CleanupStack::PopAndDestroy(&list);
    }



EXPORT_C void CSenAtomMessage:: AddCategoryL( const TDesC8& aTerm, const TDesC8& aScheme, const TDesC8& aLabel)
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement category;
    category = message.AddNewElementL(KSenAtomCategoryName);

    if(aTerm!=KNullDesC8)
        {
        category.AddNewAttributeSameNsL(KSenAtomTerm,aTerm);
        }
    if(aScheme!=KNullDesC8)
        {
        category.AddNewAttributeSameNsL(KSenAtomScheme,aScheme);
        }
    if(aLabel!=KNullDesC8)
        {
        category.AddNewAttributeSameNsL(KSenAtomLabel,aLabel);
        }
    }

EXPORT_C TInt CSenAtomMessage::CategoryCountL()
    {
    TXmlEngElement message = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomCategoryName, KSenAtomSpace);
    TInt a = list.Count();
    CleanupStack::PopAndDestroy(&list);
    return a;
    }

EXPORT_C void CSenAtomMessage::RemoveCategoryL(const TDesC8& aTerm )
    {

    TXmlEngElement message = AsElementL();
    TXmlEngElement category;
    RXmlEngNodeList<TXmlEngElement> list;
    
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomCategoryName, KSenAtomSpace);
    while(list.HasNext()){
        category = list.Next();
        TPtrC8 icPtr = category.AttributeValueL(KSenAtomTerm, KSenAtomSpace);
        if(aTerm.Compare(icPtr)==0){
            category.Remove();
            break;
        }
    }
    CleanupStack::PopAndDestroy(&list);
    }



EXPORT_C const TDesC8& CSenAtomMessage::Updated() const
    {
    if ( iUpdated )
    return *iUpdated;
    else
    return KNullDesC8;
    }

EXPORT_C void CSenAtomMessage::SetUpdatedL( const TDesC8& aUpdated )
    {
    if ( iUpdated )
        {
        delete iUpdated;
        iUpdated = 0;
        }
    if ( aUpdated.Length()> 0 )
        {
        iUpdated = aUpdated.AllocL();
        TXmlEngElement message = AsElementL();
        TXmlEngElement updated;
        updated = message.AddNewElementL(KSenAtomUpdatedName);
        updated.SetTextNoEncL(aUpdated);
        }
    }

EXPORT_C const TDesC8& CSenAtomMessage::Published() const
    {
    if ( iPublished )
    return *iPublished;
    else
    return KNullDesC8;
    }

EXPORT_C void CSenAtomMessage::SetPublishedL( const TDesC8& aPublished )
    {
    if ( iPublished )
        {
        delete iPublished;
        iPublished = 0;
        }
    if ( aPublished.Length()> 0 )
        {
        iPublished = aPublished.AllocL();
        TXmlEngElement message = AsElementL();
        TXmlEngElement published;
        published = message.AddNewElementL(KSenAtomPublishedName);
        published.SetTextNoEncL(aPublished);
        }
    }

EXPORT_C const TDesC8& CSenAtomMessage::IdTag() const
    {
    if ( iIdTag )
    return *iIdTag;
    else
    return KNullDesC8;
    }

EXPORT_C void CSenAtomMessage::SetIdTagL( const TDesC8& aId )
    {
    if ( iIdTag )
        {
        delete iIdTag;
        iIdTag = 0;
        }
    if ( aId.Length()> 0 )
        {
        iIdTag = aId.AllocL();
        TXmlEngElement message = AsElementL();
        TXmlEngElement id;
        id = message.AddNewElementL(KSenAtomIdName);
        id.SetTextNoEncL(aId);
        }
    }


EXPORT_C void CSenAtomMessage::SetAuthorL( const TDesC8& aName, const TDesC8& aUri, const TDesC8& aEmail )
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement auth;

    auth = message.AddNewElementL(KSenAtomAuthorName);

    if(aName!=KNullDesC8)
        {
        TXmlEngElement nameEl;
        nameEl = auth.AddNewElementL(KSenAtomName);
        nameEl.SetTextNoEncL(aName);
        }
    if(aUri!=KNullDesC8)
        {
        TXmlEngElement uriEl;
        uriEl = auth.AddNewElementL(KSenAtomURI);
        uriEl.SetTextNoEncL(aUri);
        }
    if(aEmail!=KNullDesC8)
        {
        TXmlEngElement emailEl;
        emailEl = auth.AddNewElementL(KSenAtomEmail);
        emailEl.SetTextNoEncL(aEmail);
        }
    }

EXPORT_C void CSenAtomMessage::RemoveAuthorL()
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement author;
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomAuthorName, KSenAtomSpace);
    while(list.HasNext()){
        author = list.Next();
        author.Remove();
    }
    CleanupStack::PopAndDestroy(&list);
    }

EXPORT_C TInt CSenAtomMessage::ContributorCountL()
    {
    TXmlEngElement message = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    //const TPtrC8 tNsUri = NsUri();
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, KSenAtomContributorName, KSenAtomSpace);
    TInt a = list.Count();
    CleanupStack::PopAndDestroy(&list);
    return a;
    }

EXPORT_C void CSenAtomMessage::AddContributorL( const TDesC8& aName, const TDesC8& aUri, const TDesC8& aEmail )
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement contributor;
    contributor = message.AddNewElementL(KSenAtomContributorName);

    if(aName!=KNullDesC8)
        {
        TXmlEngElement nameEl;
        nameEl = contributor.AddNewElementL(KSenAtomName);
        nameEl.SetTextNoEncL(aName);
        }
    if(aUri!=KNullDesC8)
        {
        TXmlEngElement uriEl;
        uriEl = contributor.AddNewElementL(KSenAtomURI);
        uriEl.SetTextNoEncL(aUri);
        }
    if(aEmail!=KNullDesC8)
        {
        TXmlEngElement emailEl;
        emailEl = contributor.AddNewElementL(KSenAtomEmail);
        emailEl.SetTextNoEncL(aEmail);
        }
    }

EXPORT_C void CSenAtomMessage::RemoveContributorL(const TDesC8& aName )
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement contributor;
    TXmlEngElement nameTag;
    RXmlEngNodeList<TXmlEngElement> list;
    RXmlEngNodeList<TXmlEngElement> listIn;
    
    CleanupClosePushL(list);
    CleanupClosePushL(listIn);
    
    message.GetElementsByTagNameL(list, KSenAtomContributorName, KSenAtomSpace);
    while(list.HasNext()){
        contributor = list.Next();
        contributor.GetElementsByTagNameL(listIn, KSenAtomName, KSenAtomSpace);
        while(listIn.HasNext()){
            nameTag = listIn.Next();
            TPtrC8 idBuf = nameTag.Text();
            if(aName.Compare(idBuf)==0){
                contributor.Remove();
                break;
            }
        }
    }
    CleanupStack::PopAndDestroy(&listIn);
    CleanupStack::PopAndDestroy(&list);
    }


EXPORT_C void CSenAtomMessage::SetRightsL( const TDesC8& aRights)
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement rightsEl;
    rightsEl = message.AddNewElementL(KSenAtomRightsName);
    rightsEl.SetTextNoEncL(aRights);
    }

EXPORT_C const TDesC8& CSenAtomMessage::RightsL()
    {
        TXmlEngElement message = AsElementL();
        RXmlEngNodeList<TXmlEngElement> list;
        CleanupClosePushL(list);

        message.GetElementsByTagNameL(list, KSenAtomRightsName, KSenAtomSpace);
        if(list.Count()>0) {
            TXmlEngElement rights = list.Next();
            TPtrC8 icPtr = rights.Text();
            CleanupStack::PopAndDestroy(&list);
            HBufC8* out = icPtr.Alloc();
            return *out;
        }else{
            CleanupStack::PopAndDestroy(&list);
            return KNullDesC8();
            }
    }

EXPORT_C TInt CSenAtomMessage::AttributeCountL(const TDesC8& aName, const TDesC8& aNameSpaceURI)
    {
    TXmlEngElement message = AsElementL();
    RXmlEngNodeList<TXmlEngElement> list;
    //const TPtrC8 tNsUri = NsUri();
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, aName, aNameSpaceURI);
    TInt a = list.Count();
    CleanupStack::PopAndDestroy(&list);
    return a;
    }

EXPORT_C void CSenAtomMessage::RemoveByAttributeL(const TDesC8& atomtag, const TDesC8& aNameSpaceUri, const TDesC8& attributeName, const TDesC8& aValue)
    {

    TXmlEngElement message = AsElementL();
    TXmlEngElement tagToRemove;
    RXmlEngNodeList<TXmlEngElement> list;
    
    CleanupClosePushL(list);
    message.GetElementsByTagNameL(list, atomtag, aNameSpaceUri);
    while(list.HasNext()){
        tagToRemove = list.Next();
        TPtrC8 icPtr = tagToRemove.AttributeValueL(attributeName, KSenAtomSpace);
        if(aValue.Compare(icPtr)==0){
            tagToRemove.Remove();
            break;
        }
    }
    CleanupStack::PopAndDestroy(&list);
    }

EXPORT_C void CSenAtomMessage::RemoveBySubtagL(const TDesC8& aAtomtag, const TDesC8& aNameSpaceUri, const TDesC8& aSubtagName, const TDesC8& aValue)
    {
    TXmlEngElement message = AsElementL();
    TXmlEngElement tagToRemove;
    TXmlEngElement name;
    RXmlEngNodeList<TXmlEngElement> list;
    RXmlEngNodeList<TXmlEngElement> listIn;
    CleanupClosePushL(list);
    CleanupClosePushL(listIn);
    message.GetElementsByTagNameL(list, aAtomtag, aNameSpaceUri);
    while(list.HasNext()){
        tagToRemove = list.Next();
        tagToRemove.GetElementsByTagNameL(listIn, aSubtagName, aNameSpaceUri);
        while(listIn.HasNext()){
                name = listIn.Next();
                TPtrC8 idBuf = name.Text();
                if(aValue.Compare(idBuf)==0){
                    tagToRemove.Remove();
                    break;
                }
        }
    }
    CleanupStack::PopAndDestroy(&listIn);
    CleanupStack::PopAndDestroy(&list);
    }
