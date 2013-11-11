/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32strm.h>
#include "senmessagebase.h"
#include <MSenProperties.h>
#include <SenHttpTransportProperties.h>
#include "senvtcptransportproperties.h"
//#include "SenLayeredHttpTransportProperties.h"
//#include "SenLayeredVtcpTransportProperties.h"

#include "senchunk.h"

#include "sendebug.h"
#include "senlogger.h"
//#include <SenXmlReader.h>

#include "msenmessagecontext.h"
#include "senmessagecontext.h"
#include "senlayeredtransportproperties.h" // private

#include <SenServiceConnection.h>

EXPORT_C CSenMessageBase* CSenMessageBase::NewL(CSenChunk& aChunk)
    {
    CSenMessageBase* pNew = CSenMessageBase::NewLC(aChunk);
    CleanupStack::Pop(pNew);
    return pNew;
    }

EXPORT_C CSenMessageBase* CSenMessageBase::NewLC(CSenChunk& aChunk)
    {
    CSenMessageBase* pNew = new (ELeave) CSenMessageBase(aChunk);
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }


EXPORT_C void CSenMessageBase::BaseConstructL()
    {
    TLSLOG_L( KSenMessagesLogChannel, KMinLogLevel, "CSenMessageBase::BaseConstructL" );
    MSenProperties::TSenPropertiesClassType type = iChunk.ChunkHeader().PropertiesType();
    TLSLOG_FORMAT(( KSenMessagesLogChannel, KNormalLogLevel, 
                    _L("- Message type: (%d)"), type ));
    
    TInt error = iChunk.DescFromChunk( iSerializedMessage, 0 );
#ifdef _SENDEBUG
    if( error )
        {
        TLSLOG_FORMAT(( KSenMessagesLogChannel, KMinLogLevel, 
                        _L("- Fatal(!): cannot find message (descriptor) from chunk, error: (%d)"), error ));
        }
#endif // _SENDEBUG        
    User::LeaveIfError ( error );
    
    if( type != MSenProperties::ENotInUse )
        {
        //User::LeaveIfError( iChunk.DescsFromChunk(iSerializedMessage, iSerializedProperties) );
        error = iChunk.DescFromChunk( iSerializedProperties, 1 );
#ifdef _SENDEBUG        
        if ( error )
            {
            TLSLOG_FORMAT(( KSenMessagesLogChannel, KMinLogLevel, 
                _L("CSenMessageBase::BaseConstructL, MAJOR: properties type != MSenProperties::ENotInUse, but desc not in chunk, error: (%d)"), error ));
            }
#endif // _SENDEBUG
        }
    
    switch( type )
        {
        case MSenProperties::ENotInUse:
            {
            
            break;
            }
        case MSenProperties::ESenXmlProperties:
        case MSenProperties::ESenLayeredXmlProperties: // flatten the props
            {
            ipProperties = CSenXmlProperties::NewL(); // iSerializedMessage, *pReader);
            break;
            }
        case MSenProperties::ESenTransportProperties:
        case MSenProperties::ESenLayeredTransportProperties: // flatten the props
            {
            ipProperties = CSenTransportProperties::NewL();
            break;
            }
        case MSenProperties::ESenHttpTransportProperties:
        case MSenProperties::ESenLayeredHttpTransportProperties: // flatten the props
            {
            ipProperties = CSenHttpTransportProperties::NewL();
            break;
            }
        case MSenProperties::ESenVtcpTransportProperties:
        case MSenProperties::ESenLayeredVtcpTransportProperties: // flatten the props
            {
            ipProperties = CSenVtcpTransportProperties::NewL();
            break;
            }
            /*
        case MSenProperties::ESenLayeredXmlProperties:
            {
            ipProperties = CSenLayeredXmlProperties::NewL(iSerializedMessage, *pReader);
            break;
            }
        case MSenProperties::ESenLayeredTransportProperties:
            {
            ipProperties = CSenLayeredTransportProperties::NewL(iSerializedMessage, *pReader);
            break;
            }
        case MSenProperties::ESenLayeredHttpTransportProperties:
            {
            ipProperties = CSenLayeredHttpTransportProperties::NewL(iSerializedMessage, *pReader);
            break;
            }
        case MSenProperties::ESenLayeredVtcpTransportProperties:
            {
            ipProperties = CSenLayeredVtcpTransportProperties::NewL(iSerializedMessage, *pReader);
            break;
            }
            */
        default: 
            {
            break;
            }
        }
        
    if ( ipProperties && iSerializedProperties.Length() > 0 )
        {
        ipProperties->ReadFromL( iSerializedProperties );
        }
    }

EXPORT_C CSenMessageBase::CSenMessageBase(CSenChunk& aChunk)
:   iChunk( aChunk ),
    ipProperties( NULL ),
    iTxnId( KErrNotFound ),
    iSerializedMessage( KNullDesC8 ),
    iSerializedProperties( KNullDesC8 )
    {
    }

EXPORT_C CSenMessageBase::~CSenMessageBase()
    {
    if( ipProperties )
        {
        if( ipProperties->IsSafeToCast( MSenProperties::ESenXmlProperties) )
            {
            CSenXmlProperties* pFlatProps = (CSenXmlProperties*)ipProperties;
            delete pFlatProps;
            }
        else if( ipProperties->IsSafeToCast( MSenProperties::ESenLayeredXmlProperties) )
            {
            CSenLayeredXmlProperties* pLayeredProps = (CSenLayeredXmlProperties*)ipProperties;
            delete pLayeredProps;
            }
#ifdef _SENDEBUG            
        else 
            { 
            // TLS log as fatal(!) 
            TLSLOG_L( KSenMessagesLogChannel, KMinLogLevel, 
                "CSenMessageBase::~CSenMessageBase():" );            
            TLSLOG_L( KSenMessagesLogChannel, KMinLogLevel, 
                "- Fatal(!): ipProperties != NULL, but cannot be cast to XML properties(!)" );            
            }
#endif // _SENDEBUG            
        }
    }


MSenMessage::TClass CSenMessageBase::Type()
    {
    return iChunk.ChunkHeader().MessageType();
    }
    
MSenMessage::TDirection CSenMessageBase::Direction()
    {
    return iChunk.ChunkHeader().MessageDirection();
    }

MSenMessageContext* CSenMessageBase::Context() 
    {
    return ipContext; 
    }

MSenProperties* CSenMessageBase::Properties()
    {
    return ipProperties;    
    }
    
TInt CSenMessageBase::SetContext(MSenMessageContext* apNotOwnedContext)
    {
    if( !apNotOwnedContext )
        {
        return KErrArgument;
        }
    // ipContext is NOT owned(!)
    delete ipProperties;
    ipProperties = NULL;
    ipContext = apNotOwnedContext;
    return KErrNone;
    }
    
TInt CSenMessageBase::SetProperties(MSenProperties* apOwnedProperties)
    {
    if( !apOwnedProperties )
        {
        return KErrArgument;
        }
    delete ipProperties;
    ipProperties = apOwnedProperties;
    return KErrNone;
    }
    
    
TBool CSenMessageBase::IsSafeToCast(TClass aType)
    {
    if ( aType == MSenMessage::EMessageBase )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }        
    }
    
TInt CSenMessageBase::TxnId()
    {
    return iChunk.ChunkHeader().TransactionId();
    }

MSenMessage* CSenMessageBase::CloneL()
    {
    return NULL;
    }

// End of File
