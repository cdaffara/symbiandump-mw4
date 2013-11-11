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
#include "senmessagecontext.h" 
#include "sencontextitem.h"
#include "SenElement.h"

#include "sencontextitem.h"
#include "SenXmlReader.h"
#include "msenremoteserviceconsumer.h"

#include "MSenMessage.h"
#include "senmessagebase.h"
#include "SenSoapEnvelope2.h"
#include "senatomentry.h"
#include "SenSoapMessage2.h"

#include "MSenProperties.h"

#include "SenXmlReader.h"

#include "senlayeredtransportproperties.h"
#include <SenTransportProperties.h>


EXPORT_C CSenMessageContext* CSenMessageContext::NewL(SenContext::TContextDirection aDirection, 
                                                      MSenProperties* apOwnedTp)
    {
    CSenMessageContext* pNew = CSenMessageContext::NewLC(aDirection, apOwnedTp);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenMessageContext* CSenMessageContext::NewLC(SenContext::TContextDirection aDirection,
                                                       MSenProperties* apOwnedTp)
    {
    CSenMessageContext* pNew = new (ELeave) CSenMessageContext(aDirection, apOwnedTp);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenMessageContext::CSenMessageContext(SenContext::TContextDirection aDirection,
                                                MSenProperties* apOwnedTp)
:  CSenContext( aDirection),
   ipOwnedTp ( apOwnedTp )
    {
    }

EXPORT_C CSenMessageContext::~CSenMessageContext()
    {
    if( ipOwnedTp )
        {
        if( ipOwnedTp->IsSafeToCast(MSenProperties::ESenXmlProperties) )
            {
            CSenXmlProperties* p = (CSenXmlProperties*)ipOwnedTp;
            delete p;
            }
        else if( ipOwnedTp->IsSafeToCast(MSenProperties::ESenLayeredXmlProperties) )
            {
            CSenLayeredXmlProperties* lp = (CSenLayeredXmlProperties*)ipOwnedTp;
            delete lp;
            }
            ipOwnedTp = NULL;
        }
    }
       
EXPORT_C void CSenMessageContext::BaseConstructL()
    {
    CSenContext::BaseConstructL();
    }

// From MSenMessageContext:
SenContext::TContextType CSenMessageContext::Type() const
    {
    return SenContext::EMessageContext;
    }
       
SenContext::TContextDirection CSenMessageContext::Direction() const
    {
    return CSenContext::Direction();
    }
       
TInt CSenMessageContext::Add(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenMessageContext::Update(const TDesC8& aKey, const TDesC8& aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TDesC8* CSenMessageContext::GetDesC8L(const TDesC8& aKey)
    {
    return CSenContext::GetDesC8L( aKey );
    }
       
TInt CSenMessageContext::Add(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenMessageContext::Update(const TDesC8& aKey, TInt aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const TInt* CSenMessageContext::GetIntL(const TDesC8& aKey)
    {
    return CSenContext::GetIntL( aKey );
    }
       
TInt CSenMessageContext::Add(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenMessageContext::Update(const TDesC8& aKey, CSenElement* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
const CSenElement* CSenMessageContext::GetSenElementL(const TDesC8& aKey)
    {
    return CSenContext::GetSenElementL( aKey );
    }
       
TInt CSenMessageContext::Add(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Add( aKey, aValue );
    }
       
TInt CSenMessageContext::Update(const TDesC8& aKey, TAny* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
       
TAny* CSenMessageContext::GetAnyL(const TDesC8& aKey)		//codescannerwarnings
    {
    return CSenContext::GetAnyL( aKey );		//codescannerwarnings
    }
       
TInt CSenMessageContext::Remove(const TDesC8& aKey) 
    {
    return CSenContext::Remove(aKey);
    }

TInt CSenMessageContext::Count() const
    {
    return CSenContext::Count();
    }

TPtrC8 CSenMessageContext::KeyAtL(TInt aIndex)
    {
    return CSenContext::KeyAtL(aIndex);
    }

void CSenMessageContext::Reset()
    {
    CSenContext::Reset();
    }
    
CSenSoapMessage* CSenMessageContext::GetCurrentSoapMessage() // DEPRECATED: 
    {
    CSenSoapMessage* pSoapMessage = NULL;
    
    TInt index = iItems.Find( KSenCtxKeyCurrentSoapMessage );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        TRAPD( err, pSoapMessage = &(pItem->ToSenSoapMessageL()); )
        if( err )
            {
            pSoapMessage = NULL;   
            }
        }
    return pSoapMessage;
    }
    
TPtrC8 CSenMessageContext::GetMessageL()
    {
    TPtrC8 retVal( KNullDesC8 );        
    TInt index = iItems.Find( KSenCtxKeyCurrentMessageAsDesc );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        TInt err( KErrNone );
        //TRAP( err, retVal.Set ( *(pItem->ToDesC8L()) );
        TRAP
            ( 
            err, 
            const TDesC8* pDesc = pItem->ToDesC8L(); 
            if( pDesc )
                {
                retVal.Set( *pDesc );
                }
            )
        err = 0; // ignored
        }
    return retVal;
    }
    
TInt CSenMessageContext::Add(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Add( aKey, aValue );    
    }
    
TInt CSenMessageContext::Update(const TDesC8& aKey, CSenSoapEnvelope* aValue)
    {
    return CSenContext::Update( aKey, aValue );
    }
    
const CSenSoapEnvelope* CSenMessageContext::GetSenSoapEnvelopeL(const TDesC8& aKey)
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
    
MSenMessage* CSenMessageContext::MessageL()		//codescannerwarnigs
    {
    TInt index = iItems.Find( KSenCtxKeyMessage );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((MSenMessage*)&(pItem->ToSenMessageL()));
        }
    return NULL;
    }
    
TInt CSenMessageContext::SetMessage(MSenMessage* apMessage, TBool aOwned)
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
    
    
CSenChunk* CSenMessageContext::ChunkL()	//codescannerwarnings
    {
    TInt index = iItems.Find( KSenCtxKeyChunk );
    if ( index != KErrNotFound )
        {
        const CSenContextItem* pItem = iItems.ValueAt(index);
        return ((CSenChunk*)&(pItem->ToSenChunkL()));
        }
    return NULL;
    }
    
TInt CSenMessageContext::SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned)
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
    

TInt CSenMessageContext::SetProperties(MSenProperties* apProperties, TBool aOwned)
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


MSenProperties* CSenMessageContext::Properties()
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
TInt CSenMessageContext::Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
    {
    if( iItems.Count() > 0 && iItems.Find(aKey) != KErrNotFound )
        {
        return KErrAlreadyExists;
        }

    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = MessageToItem( apMessage, aOwned, leaveCode );
    if( leaveCode != KErrNone ) 
        {
         delete pItem;	
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
TInt CSenMessageContext::Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned)
    {
    TInt leaveCode(KErrNone);
    CSenContextItem* pItem = MessageToItem( apMessage, aOwned, leaveCode );
    if( leaveCode != KErrNone ) 
        {
        	delete pItem;
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
MSenMessage* CSenMessageContext::GetMessageL(const TDesC8& aKey)	//codescannerwarnings
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
CSenContextItem* CSenMessageContext::MessageToItem(MSenMessage* apMessage, TBool aOwned, TInt& aError)
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
        else if ( apMessage->IsSafeToCast( MSenMessage::EAtomMessage ) )
            {
            CSenAtomEntry* msgAtomEntry = (CSenAtomEntry*) apMessage;
            TRAP( aError,  pItem = CSenContextItem::NewL( msgAtomEntry ); )
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
    
TInt CSenMessageContext::SetDirection( SenContext::TContextDirection aDirection )
    {
    CSenContext::SetDirection( aDirection );
    return KErrNone;
    }

const CSenXmlReader* CSenMessageContext::GetParser()
    {
    return CSenContext::GetParser();
    }
       
// End of file

