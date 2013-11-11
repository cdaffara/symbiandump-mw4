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
* Description:        feed data class
*
*/







#include <e32std.h>
#include <badesca.h>

#include <SenDomFragmentBase.h>
#include <xml/dom/xmlengnodelist.h>

#include "senatomfeed.h"
#include "senatomentry.h"

#include "sendebug.h"
#include "senlogger.h"

EXPORT_C CSenAtomFeed* CSenAtomFeed::NewL( TInt64 aId )
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomFeed::NewL( TInt64 aId )");    
    CSenAtomFeed* self = NewLC( aId );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CSenAtomFeed* CSenAtomFeed::NewLC( TInt64 aId )
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomFeed::NewLC( TInt64 aId )");     
    CSenAtomFeed* self = new (ELeave) CSenAtomFeed;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    if ( aId != 0 )
        {
        self->iId = aId;
        }
    return self;
    }

EXPORT_C CSenAtomFeed::CSenAtomFeed(): CSenAtomMessage()
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomFeed()");     
    }

EXPORT_C void CSenAtomFeed::BaseConstructL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomFeed::BaseConstructL()");      
    CSenFragmentBase::BaseConstructL(_L8("http://www.w3.org/2005/Atom"),_L8("feed"));
    }

EXPORT_C CSenAtomFeed::~CSenAtomFeed()
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomFeed::~CSenAtomFeed()");
    iEntries.ResetAndDestroy();
    iEntries.Close();
    }



EXPORT_C TInt CSenAtomFeed::EntryCount() //const
    {
    /*/testing
            TXmlEngElement feed = AsElementL();
            RXmlEngNodeList<TXmlEngElement> list;
            const TPtrC8 tNsUri = NsUri();
            CleanupClosePushL(list);
            feed.GetElementsByTagNameL(list, KSenAtomEntryName, tNsUri);
            TInt a = list.Count();
            CleanupStack::PopAndDestroy(&list);
            TInt b = iEntries.Count();
    //testing*/
    return iEntries.Count();
    }

EXPORT_C TInt CSenAtomFeed::AddEntryL(CSenAtomEntry& aEntry)
    {
     TXmlEngElement feed = AsElementL();
     RSenDocument doc = AsDocumentL();
     feed.AppendChildL(aEntry.AsElementL());
     CSenAtomEntry* ptrEnt = &aEntry;
     TInt err = iEntries.Append(ptrEnt);
     return err;
    }

EXPORT_C CSenAtomEntry* CSenAtomFeed::Entry( TInt aIndex ) const
    {
    if ( aIndex >= 0 && aIndex < iEntries.Count() )
        return iEntries[aIndex];
    else
        return 0;
    }

EXPORT_C void CSenAtomFeed::RemoveEntryL(const TDesC8& aEntryId )
    {

    //from table
    for (TInt aIndex = 0; aIndex < iEntries.Count(); aIndex++ )
        {
       // const TDesC8& tmp = iEntries[aIndex]->IdTag();//delete this
        if(aEntryId.Compare(iEntries[aIndex]->IdTag())==0)
            {
            delete iEntries[aIndex];
            iEntries.Remove( aIndex );
            }
        }
    
    //from element
    TXmlEngElement feed = AsElementL();
    TXmlEngElement entry;
    TXmlEngElement id;
    RXmlEngNodeList<TXmlEngElement> list;
    RXmlEngNodeList<TXmlEngElement> listIn;
    const TPtrC8 tNsUri = NsUri();
    CleanupClosePushL(list);
    CleanupClosePushL(listIn);    
    feed.GetElementsByTagNameL(list, KSenAtomEntryName, tNsUri);
    while(list.HasNext()){
        entry = list.Next();
        entry.GetElementsByTagNameL(listIn, KSenAtomIdName, KSenAtomSpace);
        while(listIn.HasNext()){
            id = listIn.Next();
            TPtrC8 idBuf = id.Text();
            if(aEntryId.Compare(idBuf)==0){
                entry.Remove();
                break;
            }
        }
    }
    CleanupStack::PopAndDestroy(&listIn);
    CleanupStack::PopAndDestroy(&list);
    }


EXPORT_C void CSenAtomFeed::RemoveEntryL( CSenAtomEntry& aEntryToRemove )
    {
    const TDesC8& iEntryId = aEntryToRemove.IdTag();
    RemoveEntryL(iEntryId);
    }


EXPORT_C const TDesC8& CSenAtomFeed::SubtitleL(){

        TXmlEngElement feed = AsElementL();
        RXmlEngNodeList<TXmlEngElement> list;
        
        CleanupClosePushL(list);

        feed.GetElementsByTagNameL(list, KSenAtomSubtitleName, KSenAtomSpace);
        if(list.Count()>0) {
            TXmlEngElement subtitle = list.Next();
            TPtrC8 icPtr = subtitle.Text();
            CleanupStack::PopAndDestroy(&list);
            HBufC8* out = icPtr.Alloc();
            return *out;
        }else{
            CleanupStack::PopAndDestroy(&list);
            return KNullDesC8();
            };
}

EXPORT_C void CSenAtomFeed::SetSubtitleL( const TDesC8& aSubtitle){

        TXmlEngElement feed = AsElementL();
        TXmlEngElement logo = feed.AddNewElementL(KSenAtomSubtitleName);
        if(aSubtitle!=KNullDesC8){
         logo.SetTextNoEncL(aSubtitle);
        }
}

EXPORT_C void CSenAtomFeed::SetGeneratorL( const TDesC8& aUri, const TDesC8& aVersion, const TDesC8& aText){

        TXmlEngElement entry = AsElementL();
        TXmlEngElement generator = entry.AddNewElementL(KSenAtomGeneratorName);
        if(aUri!=KNullDesC8)
            generator.AddNewAttributeSameNsL(KSenAtomURI,aUri);
        if(aVersion!=KNullDesC8)
            generator.AddNewAttributeSameNsL(KSenAtomVersion,aUri);
        generator.SetTextNoEncL(aText);
}

EXPORT_C TXmlEngElement CSenAtomFeed::GetGeneratorL(){

TXmlEngElement feed = AsElementL();
RXmlEngNodeList<TXmlEngElement> list;

CleanupClosePushL(list);

feed.GetElementsByTagNameL(list, KSenAtomGeneratorName, KSenAtomSpace);
if(list.Count()>0) {
    TXmlEngElement generator = list.Next();
    return generator;
}else{
    CleanupStack::PopAndDestroy(&list);
    return TXmlEngElement(0);
    }
}

/*EXPORT_C const TDesC8& CSenAtomFeed::GeneratorL(){

        TXmlEngElement feed = AsElementL();
        RXmlEngNodeList<TXmlEngElement> list;
        
        CleanupClosePushL(list);

        feed.GetElementsByTagNameL(list, KSenAtomGeneratorName, KSenAtomSpace);
        if(list.Count()>0) {
            TXmlEngElement logo = list.Next();
            TPtrC8 icPtr = logo.AttributeValueL(KSenAtomURI, KSenAtomSpace);
            CleanupStack::PopAndDestroy(&list);
            HBufC8* out = icPtr.Alloc();
            return *out;
        }else{
            CleanupStack::PopAndDestroy(&list);
            return KNullDesC8();
            };
}
*/    
EXPORT_C const TDesC8& CSenAtomFeed::LogoL(){

        TXmlEngElement feed = AsElementL();
        RXmlEngNodeList<TXmlEngElement> list;
        
        CleanupClosePushL(list);

        feed.GetElementsByTagNameL(list, KSenAtomLogoName, KSenAtomSpace);
        if(list.Count()>0) {
            TXmlEngElement logo = list.Next();
            TPtrC8 icPtr = logo.AttributeValueL(KSenAtomURI, KSenAtomSpace);
            CleanupStack::PopAndDestroy(&list);
            HBufC8* out = icPtr.Alloc();
            return *out;
        }else{
            CleanupStack::PopAndDestroy(&list);
            return KNullDesC8();
            }
}

EXPORT_C void CSenAtomFeed::SetLogoL( const TDesC8& aUri){

        TXmlEngElement entry = AsElementL();
        TXmlEngElement logo = entry.AddNewElementL(KSenAtomLogoName);
        if(aUri!=KNullDesC8)
            logo.AddNewAttributeSameNsL(KSenAtomURI,aUri);

}

EXPORT_C void CSenAtomFeed::SetIconL( const TDesC8& aUri){

        TXmlEngElement feed = AsElementL();
        TXmlEngElement icon = feed.AddNewElementL(KSenAtomIconName);
        if(aUri!=KNullDesC8)
            icon.AddNewAttributeSameNsL(KSenAtomURI,aUri);

}

EXPORT_C const TDesC8& CSenAtomFeed::IconL(){

        TXmlEngElement feed = AsElementL();
        RXmlEngNodeList<TXmlEngElement> list;
        
        CleanupClosePushL(list);

        feed.GetElementsByTagNameL(list, KSenAtomIconName, KSenAtomSpace);
        if(list.Count()>0) {
            TXmlEngElement icon = list.Next();
            TPtrC8 icPtr = icon.AttributeValueL(KSenAtomURI, KSenAtomSpace);
            CleanupStack::PopAndDestroy(&list);
            HBufC8* out = icPtr.Alloc();
            return *out;
        }else{
            CleanupStack::PopAndDestroy(&list);
            return KNullDesC8();
            };
}

        //*********************************************** CALLBACKS ***************/////


EXPORT_C void CSenAtomFeed::OnStartElementL(const RTagInfo& aElement,
        const RAttributeArray& aAttributes,    TInt /*aErrorCode*/)
    {
    
    
    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();
    
    if(saxLocalName==KSenAtomFeedName){
        iState= KSenStateIgnore;
    } else{
        iState= KSenStateSave;
    }
    if(saxLocalName==KSenAtomEntryName){
        ParseEntryL(aElement, aAttributes);
        CSenFragmentBase::OnStartElementL(aElement,aAttributes,TInt(KErrNone));
    }else
    {
        CSenFragmentBase::OnStartElementL(aElement,aAttributes,TInt(KErrNone));
    }
    }

EXPORT_C void CSenAtomFeed::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
    {
    CSenFragmentBase::OnContentL(aBytes, TInt(KErrNone));
    }

EXPORT_C void CSenAtomFeed::OnEndElementL(const RTagInfo& aElement,TInt /*aErrorCode*/)
    {

    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();
    
    const TPtrC8 tLocalName = LocalName();
    const TPtrC8 tNsUri = NsUri();
    const TPtrC8 tPrefix = NsPrefix();
    //const TPtrC8 tmp = AsXmlL()->Des();
    
    if(saxLocalName==KSenAtomFeedName){
        iState= KSenStateIgnore;
    } else{
        iState= KSenStateSave;
    }
    if(saxLocalName==KSenAtomEntryName)
        {
        RSenDocument& feed = AsDocumentL();
        iEntries.Append(iEntry);
        iEntry->AsElementL().CopyToL(feed);
        delete iEntry;
        iEntry = NULL;
        CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
        }
    else
    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));

    }

EXPORT_C void CSenAtomFeed::ParseEntryL(const RTagInfo& aElement, const RAttributeArray& /*aAttributes*/)
    {
    
    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();
    
    iEntry = CSenAtomEntry::NewL();
    OnDelegateParsingL(*iEntry);
    }
