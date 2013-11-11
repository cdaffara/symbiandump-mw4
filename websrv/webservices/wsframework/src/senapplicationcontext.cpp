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
#include "senapplicationcontext.h" 
#include "senservercontext.h"
#include "senclientcontext.h"
#include "SenServiceConnection.h" // KErrSenInternal

EXPORT_C CSenApplicationContext* CSenApplicationContext::NewL(TSecureId aSecureId,
                                                              CSenServerContext& aServerContext)
    {
    CSenApplicationContext* pNew = CSenApplicationContext::NewLC(aSecureId, aServerContext);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenApplicationContext* CSenApplicationContext::NewLC(TSecureId aSecureId,
                                                               CSenServerContext& aServerContext)
    {
    CSenApplicationContext* pNew = new (ELeave) CSenApplicationContext(aSecureId, aServerContext);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenApplicationContext::CSenApplicationContext(TSecureId aSecureId,
                                                        CSenServerContext& aServerContext)
:   iSecureId(aSecureId),
    iServerContext(aServerContext),
    iClients(ETrue, ETrue)
    {
    }

EXPORT_C CSenApplicationContext::~CSenApplicationContext()
    {
    }
       
EXPORT_C void CSenApplicationContext::BaseConstructL()
    {
//    CSenContext::BaseConstructL();
    }
    
CSenServerContext& CSenApplicationContext::ServerContext() const
    {
    return iServerContext;
    }
    
TSecureId CSenApplicationContext::SecureId() const
    {
    return iSecureId;
    }

CSenClientContext& CSenApplicationContext::OpenClientContextL(TInt aClientId)
    {
    CSenClientContext* pCtx = NULL;
    // Check if requested client context already exists, held by this server context
    TInt index = iClients.Find( aClientId );
    if ( index == KErrNotFound )
        {
        // New client context needs to be created
        pCtx = CSenClientContext::NewLC(aClientId, *this );
        TInt* pClientId = new (ELeave) TInt(aClientId);
        TInt err = iClients.Append( pClientId, pCtx );
        if( err )
            {
            // Append failed: cleanup and leave
            delete pClientId; // de-alloc this orphan
            CleanupStack::PopAndDestroy( pCtx );
            User::Leave( err ); 
            }
        else
            {
            // Remove the new context from cleanup stack, 
            // it is now owned by this application context
            CleanupStack::Pop( pCtx );
            }
        }
    else
        {
        // Return the existing client context
        pCtx = (CSenClientContext*)iClients.ValueAt( index );
        if( !pCtx )
            {
            // Should NOT occur: map failed => leave with internal error code
            User::Leave( KErrSenInternal ); 
            }
        } 
    return *pCtx;
    }

TInt CSenApplicationContext::CloseClientContext(const CSenClientContext& aCtx)
    {
    TInt retVal(KErrNotFound);
    if ( aCtx.iSessions.Count() == 0 )
        {
        // Note: RemoveByKey returns KErrNotFound or index of removed item(!)
        retVal = iClients.RemoveByKey( aCtx.ClientId() );
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

// From MSenApplicationContext:
/*
SenContext::TContextType CSenApplicationContext::Type() const
    {
    return SenContext::EMessageContext;
    }
       
SenContext::TContextDirection CSenApplicationContext::Direction() const
    {
    return CSenContext::Direction();
    }
       
TInt CSenApplicationContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenApplicationContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TDesC8* CSenApplicationContext::GetDesC8L(const TDesC8& aKey)
    {
    return CSenContext::GetDesC8L( aKey );
    }
       
TInt CSenApplicationContext::Add(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenApplicationContext::Update(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TInt* CSenApplicationContext::GetIntL(const TDesC8& aKey)
    {
    return CSenContext::GetIntL( aKey );
    }
       
TInt CSenApplicationContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenApplicationContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const CSenElement* CSenApplicationContext::GetSenElementL(const TDesC8& aKey)
    {
    return CSenContext::GetSenElementL( aKey );
    }
       
TInt CSenApplicationContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenApplicationContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
TAny* CSenApplicationContext::GetAny(const TDesC8& aKey)
    {
    return CSenContext::GetAny( aKey );
    }
       
TInt CSenApplicationContext::Remove(const TDesC8& aKey) 
    {
    return CSenContext::Remove(aKey);
    }

TInt CSenApplicationContext::Count() const
    {
    return CSenContext::Count();
    }

TPtrC8 CSenApplicationContext::KeyAtL(TInt aIndex)
    {
    return CSenContext::KeyAtL(aIndex);
    }

void CSenApplicationContext::Reset()
    {
    CSenContext::Reset();
    }
    
CSenSoapMessage* CSenApplicationContext::GetCurrentSoapMessage() // DEPRECATED: 
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
    
TPtrC8 CSenApplicationContext::GetMessage()
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
    
TInt CSenApplicationContext::Add(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Add( aKey, aValue );    
    }
    
TInt CSenApplicationContext::Update(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
    
const CSenSoapEnvelope* CSenApplicationContext::GetSenSoapEnvelopeL(const TDesC8& aKey)
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
    
MSenMessage* CSenApplicationContext::Message()
    {
    TInt index = iItems.Find( KSenCtxKeyMessage );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((MSenMessage*)&(pItem->ToSenMessageL()));
        }
    return NULL;
    }
    
TInt CSenApplicationContext::SetMessage(MSenMessage* apMessage, TBool aOwned)
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
    
    
CSenChunk* CSenApplicationContext::Chunk()
    {
    TInt index = iItems.Find( KSenCtxKeyChunk );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((CSenChunk*)&(pItem->ToSenChunkL()));
        }
    return NULL;
    }
    
TInt CSenApplicationContext::SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned)
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
    

TInt CSenApplicationContext::SetProperties(MSenProperties* apProperties, TBool aOwned)
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


MSenProperties* CSenApplicationContext::Properties()
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
TInt CSenApplicationContext::Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
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
TInt CSenApplicationContext::Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
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
MSenMessage* CSenApplicationContext::GetMessage(const TDesC8& aKey)
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
CSenContextItem* CSenApplicationContext::MessageToItem(MSenMessage* apMessage, TBool aOwned, TInt& aError)
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
    
TInt CSenApplicationContext::SetDirection( SenContext::TContextDirection aDirection )
    {
    CSenContext::SetDirection( aDirection );
    return KErrNone;
    }

const CSenXmlReader* CSenApplicationContext::GetParser()
    {
    return CSenContext::GetParser();
    }
*/       
// End of file

