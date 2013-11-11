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
#include "e32cmn.h"

#include "senservercontext.h" 
#include "sencoreservicemanager.h" // Core\inc
#include "senapplicationcontext.h"
#include <SenServiceConnection.h> // KErrSenInternal

EXPORT_C CSenServerContext* CSenServerContext::NewL(MSenCoreServiceManager& aCore)
    {
    CSenServerContext* pNew = CSenServerContext::NewLC(aCore);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenServerContext* CSenServerContext::NewLC(MSenCoreServiceManager& aCore)
    {
    CSenServerContext* pNew = new (ELeave) CSenServerContext(aCore);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenServerContext::CSenServerContext(MSenCoreServiceManager& aCore)
: iApplications(ETrue, ETrue),
  iCore(aCore)
    {
    }

EXPORT_C CSenServerContext::~CSenServerContext()
    {
    iApplications.Reset(); // de-alloc secure-id's and application contexts
    }
       
EXPORT_C void CSenServerContext::BaseConstructL()
    {
//    CSenContext::BaseConstructL();
    }
    
MSenCoreServiceManager& CSenServerContext::Core()
    {
    return iCore;
    }

CSenApplicationContext& CSenServerContext::OpenApplicationContextL(TSecureId aSecureId)
    {
    CSenApplicationContext* pCtx = NULL;
    // Check if application context already exists, held by this server context
    TInt index = iApplications.Find( aSecureId );
    if ( index == KErrNotFound )
        {
        // New application context needs to be created
        pCtx = CSenApplicationContext::NewLC(aSecureId, *this );
        TUint32* pSecureId = new (ELeave) TUint32(aSecureId);
        TInt err = iApplications.Append( pSecureId, pCtx );
        if( err )
            {
            // Append failed: cleanup and leave
            delete pSecureId; // de-alloc this orphan
            CleanupStack::PopAndDestroy( pCtx );
            User::Leave( err ); 
            }
        else
            {
            // Remove the new context from cleanup stack, 
            // it is now owned by this server context
            CleanupStack::Pop( pCtx );
            }
        }
    else
        {
        // Return the existing application context
        pCtx = (CSenApplicationContext*)iApplications.ValueAt( index );
        if( !pCtx )
            {
            // Should NOT occur: map failed => leave with internal error code
            User::Leave( KErrSenInternal ); 
            }
        } 
    return *pCtx;
    }

TInt CSenServerContext::CloseApplicationContext(const CSenApplicationContext& aCtx)
    {
    TInt retVal(KErrNotFound);
    if ( aCtx.iClients.Count() == 0 )
        {
        // Note: RemoveByKey returns KErrNotFound or index of removed item(!)
        retVal = iApplications.RemoveByKey( aCtx.SecureId() );
        if( retVal != KErrNotFound )
            {
            retVal = KErrNone;
            }
        }
    else
        {
        retVal = KErrInUse;
        }
    return retVal;
    }

// From MSenServerContext:
/*
SenContext::TContextType CSenServerContext::Type() const
    {
    return SenContext::EMessageContext;
    }
       
SenContext::TContextDirection CSenServerContext::Direction() const
    {
    return CSenContext::Direction();
    }
       
TInt CSenServerContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenServerContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TDesC8* CSenServerContext::GetDesC8L(const TDesC8& aKey)
    {
    return CSenContext::GetDesC8L( aKey );
    }
       
TInt CSenServerContext::Add(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenServerContext::Update(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TInt* CSenServerContext::GetIntL(const TDesC8& aKey)
    {
    return CSenContext::GetIntL( aKey );
    }
       
TInt CSenServerContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenServerContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const CSenElement* CSenServerContext::GetSenElementL(const TDesC8& aKey)
    {
    return CSenContext::GetSenElementL( aKey );
    }
       
TInt CSenServerContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenServerContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
TAny* CSenServerContext::GetAny(const TDesC8& aKey)
    {
    return CSenContext::GetAny( aKey );
    }
       
TInt CSenServerContext::Remove(const TDesC8& aKey) 
    {
    return CSenContext::Remove(aKey);
    }

TInt CSenServerContext::Count() const
    {
    return CSenContext::Count();
    }

TPtrC8 CSenServerContext::KeyAtL(TInt aIndex)
    {
    return CSenContext::KeyAtL(aIndex);
    }

void CSenServerContext::Reset()
    {
    CSenContext::Reset();
    }
    
CSenSoapMessage* CSenServerContext::GetCurrentSoapMessage() // DEPRECATED: 
    {
    CSenSoapMessage* pSoapMessage = NULL;
    
    TInt index = iItems.Find( KSenCtxKeyCurrentSoapMessage );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        pSoapMessage = &(pItem->ToSenSoapMessageL());
        }
    return pSoapMessage;
    }
    
TPtrC8 CSenServerContext::GetMessage()
    {
    TInt index = iItems.Find( KSenCtxKeyCurrentMessageAsDesc );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        const TDesC8* desc = pItem->ToDesC8L();
        return *desc;
        }
    return KNullDesC8();
    }
    
TInt CSenServerContext::Add(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Add( aKey, aValue );    
    }
    
TInt CSenServerContext::Update(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
    
const CSenSoapEnvelope* CSenServerContext::GetSenSoapEnvelopeL(const TDesC8& aKey)
    {
    TInt index = iItems.Find( aKey );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        const CSenSoapEnvelope* envelope = ((CSenSoapEnvelope*)&(pItem->ToSenSoapMessageL()));
        return envelope;
        }
    return NULL;
    }
    
MSenMessage* CSenServerContext::Message()
    {
    TInt index = iItems.Find( KSenCtxKeyMessage );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((MSenMessage*)&(pItem->ToSenMessageL()));
        }
    return NULL;
    }
    
TInt CSenServerContext::SetMessage(MSenMessage* apMessage, TBool aOwned)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = MessageToItem( apMessage, aOwned, leaveCode );

    if( leaveCode != KErrNone ) 
        {
        delete pItem; // should ALWAYS be NULL(!)
        return leaveCode;
        }
        
    TInt retCode(iItems.UpdateValue(&KSenCtxKeyMessage(), pItem));
    if( retCode != KErrNone && aOwned )
        {
        delete pItem; // delete the orphan item
        } 
    return retCode;
    }
    
    
CSenChunk* CSenServerContext::Chunk()
    {
    TInt index = iItems.Find( KSenCtxKeyChunk );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((CSenChunk*)&(pItem->ToSenChunkL()));
        }
    return NULL;
    }
    
TInt CSenServerContext::SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned)
    {
    TInt leaveCode(KErrNone);
    
    CSenContextItem* pItem = NULL;
    if( aOwned )
        {
        TRAP( leaveCode, pItem = CSenContextItem::NewL(apMsgAsChunk); )
        }
    else // chunk is not to be owned
        {
        TRAP( leaveCode, pItem = CSenContextItem::NewL(*apMsgAsChunk); )
        }

    if( leaveCode != KErrNone ) 
        {
        delete pItem; // should ALWAYS be NULL(!)
        return leaveCode;
        }
        
    TInt retCode(iItems.UpdateValue(&KSenCtxKeyChunk(), pItem));
    if( retCode != KErrNone && aOwned )
        {
        delete pItem; // delete the orphan item
        } 
    return retCode;
    }
    

TInt CSenServerContext::SetProperties(MSenProperties* apProperties, TBool aOwned)
    {
    if( !apProperties )
        {
        return KErrArgument;
        }
        
    TInt retCode(KErrNone);
    if ( aOwned )
        {
        delete ipOwnedTp;
        ipOwnedTp = NULL;
        ipOwnedTp = apProperties;
        }
    else
        {
        
        CSenContextItem* pItem = NULL;
        TInt leaveCode(KErrNone);
//        if( aOwned )
//            {
//            TRAP( leaveCode, pItem = CSenContextItem::NewL(apProperties); )
//            }
//        else // chunk is not to be owned
//            {
            TRAP( leaveCode, pItem = CSenContextItem::NewL(*apProperties); )
//            }

        if( leaveCode != KErrNone ) 
            {
            delete pItem; // should ALWAYS be NULL(!)
            return leaveCode;
            }
            
        retCode = iItems.UpdateValue(&KSenCtxKeyChunk(), pItem);
        if( retCode != KErrNone && aOwned )
            {
            delete pItem; // delete the orphan item
            } 
        }
    return retCode;
    }


MSenProperties* CSenServerContext::Properties()
    {
    if( ipOwnedTp )
        {
        return ipOwnedTp;
        }
    else
        {
        TInt index = iItems.Find( KSenCtxKeyMessageProperties );
        if ( index != KErrNotFound )
            {
            const CSenContextItem* pItem = iItems.ValueAt(index);
            if( pItem->Type() == MSenContextItem::EMSenProperties )
                {
                MSenProperties* pProperties = (MSenProperties*)pItem;
                return pProperties;
                }
            }
        return NULL;
        }
    }

// Method for adding an "intermediate" message behind certain key; used normally to conduct final "service message"
TInt CSenServerContext::Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = MessageToItem( apMessage, aOwned, leaveCode );
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.Append(&aKey, pItem));
    if( retCode != KErrNone )
        {
        delete pItem;
        }
    return retCode;
    }
    
// Method for updating some "intermediate" message behind certain key; used normally to conduct final "service message"    
TInt CSenServerContext::Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = MessageToItem( apMessage, aOwned, leaveCode );
    if( leaveCode != KErrNone ) 
        {
        return leaveCode;
        }

    TInt retCode(iItems.UpdateValue(&aKey, pItem));
    if( retCode != KErrNone && aOwned )
        {
        delete pItem;
        }
    return retCode;    
    }
    
// Method for getting "intermediate" message behind certain key; used normally to conduct final "service message"    
MSenMessage* CSenServerContext::GetMessage(const TDesC8& aKey)
    {
    TInt index = iItems.Find(aKey);
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        if(pItem)
            {
            return &pItem->ToSenMessageL();
            }
        }
    return NULL;    
    }
 
// private helper method(s):
CSenContextItem* CSenServerContext::MessageToItem(MSenMessage* apMessage, TBool aOwned, TInt& aError)
    {
    aError = KErrNone;
    CSenContextItem* pItem = NULL;
    if( aOwned )
        {
        if( apMessage->IsSafeToCast( MSenMessage::EMessageBase ) )
            {
            CSenMessageBase* msgBase = (CSenMessageBase*) apMessage;
            TRAP( aError, pItem = CSenContextItem::NewL( msgBase ); )
            }
        else if ( apMessage->IsSafeToCast( MSenMessage::ESoapEnvelope2 ) )
            {
            CSenSoapEnvelope2* msgSoap2 = (CSenSoapEnvelope2*) apMessage;
            TRAP( aError,  pItem = CSenContextItem::NewL( msgSoap2 ); )
            }
        else 
            {
            aError = KErrNotSupported;
            return NULL;
            }
        }
    else // provided message is *not to be owned*
        {
        TRAP( aError, pItem = CSenContextItem::NewL(*apMessage); )
        }
    return pItem;        
    } 
    
TInt CSenServerContext::SetDirection( SenContext::TContextDirection aDirection )
    {
    CSenContext::SetDirection( aDirection );
    return KErrNone;
    }

const CSenXmlReader* CSenServerContext::GetParser()
    {
    return CSenContext::GetParser();
    }
*/       
// End of file

