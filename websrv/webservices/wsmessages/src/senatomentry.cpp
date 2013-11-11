/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <xml/dom/xmlengnodelist.h>

#include "senatomentry.h"

#include "sendebug.h"
#include "senlogger.h"

const TInt KStateParsingMediaGroup       = 19;  // save state     (odd number)

EXPORT_C CSenAtomEntry* CSenAtomEntry::NewL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomEntry::NewL()");
    CSenAtomEntry* pNew = CSenAtomEntry::NewLC();
    CleanupStack::Pop( pNew ); // pNew
    return pNew;
    }

EXPORT_C CSenAtomEntry* CSenAtomEntry::NewLC()
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomEntry::NewLC()");
    CSenAtomEntry* pNew = new (ELeave) CSenAtomEntry();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenAtomEntry::CSenAtomEntry () :
    CSenAtomMessage(),localState(KSenStateSave)
    {
    }

EXPORT_C CSenAtomEntry::~CSenAtomEntry()
    {
    iBuffer.Close();
    }

EXPORT_C void CSenAtomEntry::BaseConstructL()
    {
    CSenFragmentBase::BaseConstructL(KSenAtomEntryXmlns, KSenAtomEntryName);

    }

//                     CALLBACKS

EXPORT_C void CSenAtomEntry::OnStartElementL(const RTagInfo& aElement,
        const RAttributeArray& aAttributes,
        TInt /*aErrorCode*/)
    {
    
    
    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();

    if(saxLocalName==KSenAtomGroupName){
        localState=KStateParsingMediaGroup;
    }
    if(saxLocalName==KSenAtomEntryName){
            iState=KSenStateIgnore;
        } 
    CSenFragmentBase::OnStartElementL(aElement,aAttributes,TInt(KErrNone));

    }

EXPORT_C void CSenAtomEntry::OnContentL(const TDesC8& aBytes, TInt /*aErrorCode*/)
    {
    iBuffer.Close();
    iBuffer.Create(aBytes);
    CSenFragmentBase::OnContentL(aBytes, TInt(KErrNone));
    }

EXPORT_C void CSenAtomEntry::OnEndElementL(
        const RTagInfo& aElement,
        TInt /*aErrorCode*/)
    {
    TLSLOG_L(KSenMessagesLogChannel, KSenMessagesLogLevel, "CSenAtomEntry::OnEndElementL");

    const TPtrC8 saxLocalName = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri = aElement.Uri().DesC();
    const TPtrC8 saxPrefix = aElement.Prefix().DesC();
    
    const TPtrC8 tLocalName = LocalName();
    const TPtrC8 tNsUri = NsUri();
    const TPtrC8 tPrefix = NsPrefix();

    switch (localState)
        {
        case KStateParsingMediaGroup:
            {
                if(saxLocalName == KSenAtomGroupName)
                    localState = KSenStateSave;
                CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
            }
            break;

        case KSenStateSave:
            {
            if(saxLocalName == KSenAtomTitleName  && localState != KStateParsingMediaGroup)
                {

                if ( iTitle )
                    {
                    delete iTitle;
                    iTitle = 0;
                    }
                if ( iBuffer.Length()> 0 )
                    {
                    iTitle = iBuffer.AllocL();

                    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
                    }
                }else
            if(saxLocalName == KSenAtomIdName  && localState != KStateParsingMediaGroup)
                {

                if ( iIdTag )
                    {
                    delete iIdTag;
                    iIdTag = 0;
                    }
                if ( iBuffer.Length()> 0 )
                    {
                    iIdTag = iBuffer.AllocL();

                    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
                    }
                }else
            if(saxLocalName == KSenAtomPublishedName  && localState != KStateParsingMediaGroup)
                {

                if ( iPublished )
                    {
                    delete iPublished;
                    iPublished = 0;
                    }
                if ( iBuffer.Length()> 0 )
                    {
                    iPublished = iBuffer.AllocL();

                    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
                    }
                }else
            if(saxLocalName == KSenAtomUpdatedName  && localState != KStateParsingMediaGroup)
                {

                if ( iUpdated )
                    {
                    delete iUpdated;
                    iUpdated = 0;
                    }
                if ( iBuffer.Length()> 0 )
                    {
                    iUpdated = iBuffer.AllocL();

                    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
                    }
                }
                else {
                    CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
                }
            }
        break;
    
        default:
        CSenFragmentBase::OnEndElementL(aElement, TInt(KErrNone));
        break;
        }

    iBuffer.Close();
    
    }

// SET         GET       COUNT          REMOVE


