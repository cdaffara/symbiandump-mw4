/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "senwsdescription.h"
#include "sentransportcontext.h"
#include "msencoreservicemanager.h"
#include "senservicesession.h"
#include "sencontextitem.h"

EXPORT_C CSenTransportContext::CSenTransportContext()
: CSenContext(SenContext::EBoth)
    {
    }

EXPORT_C CSenTransportContext::~CSenTransportContext()
    {
    }


EXPORT_C CSenTransportContext* CSenTransportContext::NewL(MSenCoreServiceManager& aCore,
                                                                 CSenWSDescription& aSessionInitializer)
    {
    CSenTransportContext* pNew = CSenTransportContext::NewLC(aCore, aSessionInitializer);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenTransportContext* CSenTransportContext::NewLC(MSenCoreServiceManager& aCore,
                                                           CSenWSDescription& aSessionInitializer)
    {
//    CSenXmlReader* reader = aCore.XMLReader();
//    if(!reader) User::Leave(KErrArgument); 
    CSenTransportContext* pNew = new (ELeave) CSenTransportContext();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aCore, aSessionInitializer, NULL);
    return pNew;
    }

EXPORT_C CSenTransportContext* CSenTransportContext::NewL(MSenCoreServiceManager& aCore,
                                                                 CSenWSDescription& aSessionInitializer,
                                                                 CSenServiceSession& aInvoker)
    {
    CSenTransportContext* pNew = CSenTransportContext::NewLC(aCore, aSessionInitializer, aInvoker);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenTransportContext* CSenTransportContext::NewLC(MSenCoreServiceManager& aCore,
                                                           CSenWSDescription& aSessionInitializer,
                                                           CSenServiceSession& aInvoker)
    {
    CSenTransportContext* pNew = new (ELeave) CSenTransportContext();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aCore, aSessionInitializer, &aInvoker);
    return pNew;
    }

EXPORT_C void CSenTransportContext::BaseConstructL(MSenCoreServiceManager& aCore,
                                                   CSenWSDescription& aSessionInitializer,
                                                   CSenServiceSession* apInvoker)
    {
    CSenContextItem* pCore = CSenContextItem::NewLC(aCore);
    User::LeaveIfError( iItems.Append( &KSenCtxKeyCoreServiceManager(), pCore ) );
    CleanupStack::Pop(pCore);

    CSenContextItem* pInit = CSenContextItem::NewLC(aSessionInitializer);
    User::LeaveIfError( iItems.Append( &KSenCtxKeySessionInitializer(), pInit ) );
    CleanupStack::Pop(pInit);

    if(apInvoker)
        {
        CSenContextItem* pSess = CSenContextItem::NewLC(*apInvoker);
        User::LeaveIfError( iItems.Append(&KSenCtxKeyServiceSession(), pSess ) );
        CleanupStack::Pop(pSess);
        }
    }

// From MSenTransportContext:
//TInt CSenTransportContext::AddCore(MSenCoreServiceManager& aValue)
//    {
//    }

MSenCoreServiceManager& CSenTransportContext::GetCoreL()
    {
    TInt index = iItems.Find( KSenCtxKeyCoreServiceManager );
    User::LeaveIfError( index );

    const CSenContextItem* pItem = iItems.ValueAt(index);
    if ( pItem == NULL ) 
        {
        User::Leave( KErrNotFound );
        }

    return pItem->ToSenCoreL();



    /*
    TInt leaveCode(KErrNone);
    MSenCoreServiceManager* pCore = NULL;
    TRAP( leaveCode, pCore = pItem->ToSenCoreL(); )
    if( leaveCode == KErrNone ) 
        {
        return pCore;
        }
    */
    }

// TInt CSenTransportContext::AddSession(CSenServiceSession* aValue) { };
// TInt CSenTransportContext::UpdateSession(CSenServiceSession* aValue) { };


//CSenServiceSession& CSenTransportContext::GetSessionL()
const CSenServiceSession* CSenTransportContext::GetSession()
    {
    CSenServiceSession* pSession = NULL;
    TInt index = iItems.Find( KSenCtxKeyServiceSession );
    if(index != KErrNotFound) 
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        if ( pItem ) 
            {
            TInt leaveCode(KErrNone);
            TRAP( leaveCode, pSession = &(pItem->ToSenServiceSessionL()); )
            if ( leaveCode != KErrNone )
                {
                pSession = NULL;
                }
            }
        }
    return pSession;
    }

//TInt CSenTransportContext::AddInitializer(CSenWSDescription* aValue) { };

//  TInt CSenTransportContext::UpdateInitializer(CSenWSDescription* aValue);

//const CSenWSDescription* CSenTransportContext::GetInitializer()
CSenWSDescription& CSenTransportContext::GetInitializerL()
     {
    TInt index = iItems.Find( KSenCtxKeySessionInitializer );
    User::LeaveIfError( index );

    const CSenContextItem* pItem = iItems.ValueAt(index);
    if ( pItem == NULL ) 
        {
        User::Leave( KErrNotFound );
        }

    return pItem->ToSenServiceDescriptionL();
    }

// TInt CSenTransportContext::AddServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue);
// TInt CSenTransportContext::UpdateServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue);
// const CSenWSDescription* CSenTransportContext::GetServiceDescriptionL(const TDesC8& aKey);
// TInt CSenTransportContext::Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue); // protected context
// TInt CSenTransportContext::Update(const TDesC8& aKey, MSenRemoteServiceSession& aValue);
// const MSenRemoteServiceSession* CSenTransportContext::GetSenRemoteServiceSessionL(const TDesC8& aKey);


 // From MSenContext:
SenContext::TContextType CSenTransportContext::Type() const
    {
    return SenContext::ETransportContext;
    }
       
SenContext::TContextDirection CSenTransportContext::Direction() const
    {
    return CSenContext::Direction();
    }
       
TInt CSenTransportContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Add(aKey, aValue);
    }
       
TInt CSenTransportContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Update(aKey, aValue);
    }
       
const TDesC8* CSenTransportContext::GetDesC8L(const TDesC8& aKey)
    {
    return CSenContext::GetDesC8L(aKey);
    }
       
TInt CSenTransportContext::Add(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Add(aKey, aValue);
    }
       
TInt CSenTransportContext::Update(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Update(aKey, aValue);
    }
       
const TInt* CSenTransportContext::GetIntL(const TDesC8& aKey)
    {
    return CSenContext::GetIntL(aKey);
    }
       
TInt CSenTransportContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Add(aKey, aValue);
    }
       
TInt CSenTransportContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Update(aKey, aValue);
    }
       
const CSenElement* CSenTransportContext::GetSenElementL(const TDesC8& aKey)
    {
    return CSenContext::GetSenElementL(aKey);
    }
       
TInt CSenTransportContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Add(aKey, aValue);
    }
       
TInt CSenTransportContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Update(aKey, aValue);
    }
       
TAny* CSenTransportContext::GetAnyL(const TDesC8& aKey)	//codescannerwarnings
    {
    return CSenContext::GetAnyL(aKey);	//codescannerwarnings
    }

TInt CSenTransportContext::Remove(const TDesC8& aKey) 
    {
    return CSenContext::Remove(aKey);
    }

TInt CSenTransportContext::Count() const
    {
    return CSenContext::Count();
    }

TPtrC8 CSenTransportContext::KeyAtL(TInt aIndex)
    {
    return CSenContext::KeyAtL(aIndex);
    }

void CSenTransportContext::Reset()
    {
    CSenContext::Reset();
    }


TInt CSenTransportContext::SetDirection( SenContext::TContextDirection aDirection )
    {
    return CSenContext::SetDirection( aDirection );
    }
    
const CSenXmlReader* CSenTransportContext::GetParser()
    {
    CSenXmlReader* reader = NULL;
    MSenCoreServiceManager* core = NULL;
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, core = &GetCoreL(); )
    if( leaveCode == KErrNone && core)
        {
        TRAP( leaveCode, reader = core->XMLReader(); )
        if( leaveCode == KErrNone && reader )
            {
            return reader;
            }
        
        }
    return NULL;
    }



// End of File
