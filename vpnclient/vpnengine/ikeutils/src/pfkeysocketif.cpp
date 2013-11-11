/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of VPN PFKEY socket interface
*
*/


#include <es_sock.h>
#include <in_sock.h>
#include <eikenv.h>
#include <pfkey_send.h>
#include <random.h>
#include "pfkeysocketif.h"
#include "pfkeymsg.h"
#include "ipsecsadata.h"
#include "ipsecsalifetime.h"
#include "ikedebug.h"

const TInt KDefaultPID( 0x2001E609 ); // UID3 of ikeutils.dll

// ======== MEMBER FUNCTIONS ========

EXPORT_C CPFKeySocketIf* CPFKeySocketIf::NewL( MPFKeyMessageListener* aListener,
                                               MIkeDebug& aDebug )
    {
    CPFKeySocketIf* reader = new ( ELeave ) CPFKeySocketIf( aListener, aDebug );
    CleanupStack::PushL( reader );
    reader->ConstructL();
    CleanupStack::Pop( reader );        
    return reader;
    }

//
// CPFKeySocketIf::~CPFKeySocketIf
//
CPFKeySocketIf::~CPFKeySocketIf()
    {
    Cancel();
    iPendingSpiRequests.Close();
    iSadb.Close();
    iSocketServer.Close();
    }

//
// CPFKeySocketIf::CPFKeySocketIf
//
CPFKeySocketIf::CPFKeySocketIf( MPFKeyMessageListener* aListener,
                                MIkeDebug& aDebug )
: CActive( EPriorityNormal ),
  iListener( aListener ),
  iSeq( 0 ),
  iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }
    
//
// CPFKeySocketIf::ConstructL()
// Open and activate the socket input
//
void CPFKeySocketIf::ConstructL()
    {   
    TPtr8 ptr( (TUint8*)&iSpiBase, sizeof( iSpiBase ) );
    ptr.SetLength( sizeof( iSpiBase ) );
    TRandom::RandomL( ptr );
    iSpiBase &= 0x7fffffff;
    
    User::LeaveIfError( iSocketServer.Connect() ); 
    User::LeaveIfError( iSadb.Open( iSocketServer ) );

    //
    // Register for ACQUIRE messages
    //
    TPfkeySendMsg reg( SADB_REGISTER, SADB_SATYPE_ESP, ++iSeq, KDefaultPID );
    TRequestStatus status;
	iSadb.FinalizeAndSend( reg, status );
    User::WaitForRequest( status );
    DEBUG_LOG1( _L("Register for ESP, status=%d"), iStatus.Int() );
    
    iMsg.Reset();
    iSadb.ReadRequest( iMsg, iStatus );
    SetActive();
    }
    
    
void CPFKeySocketIf::GetSpi( const TUint8 aType,
                             const TUint32 aSeq,
                             const TInetAddr& aSrc,
                             const TInetAddr& aDst,
                             TUint32& aSpi,
                             TRequestStatus& aClientStatus)
    {
    /*Params:
            aType:SADB_SATYPE_AH,SADB_SATYPE_ESP from prop_II
            aSeq: Seq number for the message
            aSrc,aDst: Src & dst addresses
    */
    TRequestStatus status;
    TUint32 start = NewSpi();

	TPfkeySendMsg msg( SADB_GETSPI, aType, aSeq, (TUint32)&aClientStatus );
    msg.Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), aSrc );
    msg.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aDst );
    msg.Add( Int2Type<SADB_EXT_SPIRANGE>(), start );
    
    aClientStatus = KRequestPending;
    TPendingSpiRequest pendingSpiRequest(aSpi, aClientStatus);
    TInt err = iPendingSpiRequests.Append(pendingSpiRequest);
    
    if (err == KErrNone)
        {
        iSadb.FinalizeAndSend( msg, status );
        User::WaitForRequest( status );
        }
    else
        {
        TRequestStatus* status = &aClientStatus;
        User::RequestComplete(status, err);
        }
    }


void CPFKeySocketIf::CancelGetSpi(TRequestStatus& aClientStatus)
    {
    for (TInt i = 0; i < iPendingSpiRequests.Count(); ++i)
        {
        TPendingSpiRequest& pendingSpiRequest = iPendingSpiRequests[i];
        if (&pendingSpiRequest.iClientStatus == &aClientStatus)
            {
            pendingSpiRequest.iSpi = 0;
            TRequestStatus* status = &pendingSpiRequest.iClientStatus;
            iPendingSpiRequests.Remove(i);
            User::RequestComplete(status, KErrCancel);
            break;
            }
        }
    }


// Sends Acquire with errno informing about key management failure.
EXPORT_C void CPFKeySocketIf::AcquireSAError( const TIpsecSAData& aSAData,
                                              const TInt aError )
 	 {
     TRequestStatus status;
     TInt err = -aError;
     TPfkeySendMsg msg( SADB_ACQUIRE,
                        aSAData.iSAType,
                        aSAData.iSeq,
                        aSAData.iPid );

     struct sadb_msg& msgHdr = msg.MsgHdr();
     msgHdr.sadb_msg_errno = (TUint8) err;
     msgHdr.sadb_msg_reserved = (TUint16) ( err>>8 );
     
     msg.Add( Int2Type<SADB_EXT_SA>(), aSAData.iSPI );
     msg.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aSAData.iDst );     
     iSadb.FinalizeAndSend( msg, status );
     User::WaitForRequest( status );
 	 }

EXPORT_C void CPFKeySocketIf::UpdateSAL( const TIpsecSAData& aSAData )
    {
    AddUpdateSAL( SADB_UPDATE, aSAData );
    }

EXPORT_C void CPFKeySocketIf::AddSAL( const TIpsecSAData& aSAData )
    {
    AddUpdateSAL( SADB_ADD, aSAData );
    }

EXPORT_C void CPFKeySocketIf::DeleteSA( const TUint32 aSPI,
                                        const TInetAddr& aSrc,
                                        const TInetAddr& aDst,
                                        const TUint8 aProtocol )
    {
    TRequestStatus status;
	TPfkeySendMsg msg( SADB_DELETE,
	                   aProtocol );
	msg.Add( Int2Type<SADB_EXT_SA>(), aSPI );
	msg.Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), aSrc );
	msg.Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aDst );
	iSadb.FinalizeAndSend( msg, status );
	User::WaitForRequest( status );
    }

EXPORT_C void CPFKeySocketIf::FlushSAs()
    {
    TRequestStatus status;
	TPfkeySendMsg msg( SADB_FLUSH,
	                   SADB_SATYPE_UNSPEC,
	                   ++iSeq,
	                   KDefaultPID );
	iSadb.FinalizeAndSend( msg, status );
    User::WaitForRequest( status );
	DEBUG_LOG1( _L("Request FLUSH, iStatus=%d"), iStatus.Int() );	    
    }

//Updates an SA from the SA database.
//SPI in Net order.
void CPFKeySocketIf::AddUpdateSAL( const TUint8 aType,
                                   const TIpsecSAData &aSAData )
    {
    TRequestStatus status;
    TPfkeySendMsg* msg = new( ELeave ) TPfkeySendMsg( aType,
                                                      aSAData.iSAType,
                                                      aSAData.iSeq,
                                                      aSAData.iPid );
    msg->Add( Int2Type<SADB_EXT_SA>(),
              aSAData.iSPI,
              aSAData.iAuthAlg,
              aSAData.iEncrAlg,
              SADB_SASTATE_MATURE,
              aSAData.iReplayWindowLength,
              aSAData.iFlags );
    
    if( aSAData.iHard )
        {
        msg->Add( Int2Type<SADB_EXT_LIFETIME_HARD>(), 
                  aSAData.iHard->iAllocations, 
                  aSAData.iHard->iBytes, 
                  aSAData.iHard->iAddtime, 
                  aSAData.iHard->iUsetime);
        }
    if( aSAData.iSoft )
        {
        msg->Add( Int2Type<SADB_EXT_LIFETIME_SOFT>(),
                  aSAData.iSoft->iAllocations, 
                  aSAData.iSoft->iBytes, 
                  aSAData.iSoft->iAddtime,
                  aSAData.iSoft->iUsetime);
        }
    msg->Add( Int2Type<SADB_EXT_ADDRESS_SRC>(),
              aSAData.iSrc,
              aSAData.iProtocol );
    msg->Add( Int2Type<SADB_EXT_ADDRESS_DST>(),
              aSAData.iDst,
              aSAData.iProtocol );
    
    // Deliver internal address for IPSEC4 
    if ( aSAData.iFlags & SADB_SAFLAGS_INT_ADDR )
       msg->Add( Int2Type<SADB_EXT_ADDRESS_PROXY>(),
                 aSAData.iInternalAddress );
    
    if ( aSAData.iAuthKey.Length() > 0 )
        {
        msg->Add( Int2Type<SADB_EXT_KEY_AUTH>(),
                  aSAData.iAuthKey );
        }
    if ( aSAData.iEncrKey.Length() > 0 )
        {
        msg->Add( Int2Type<SADB_EXT_KEY_ENCRYPT>(),
                  aSAData.iEncrKey );
        }
    if ( aSAData.iSrcIdent.Length() > 0 )
        {
        msg->Add( Int2Type<SADB_EXT_IDENTITY_SRC>(),
                  aSAData.iSrcIdent,
                  aSAData.iSrcIdType );
        }
    if ( aSAData.iDstIdent.Length() > 0 )
        {
        msg->Add( Int2Type<SADB_EXT_IDENTITY_DST>(),
                  aSAData.iDstIdent,
                  aSAData.iDstIdType );
        }

    // Deliver generic private PFKEY API extension, if exist.
    // In this phase extension can consists NAT traversal information for ESP UDP encapsulation (done by IPSEC)
    if ( aSAData.iGenericExtension.Length() )
        {
        msg->Add( Int2Type<SADB_PRIV_GENERIC_EXT>(),
                  aSAData.iGenericExtension );
        }
    
    iSadb.FinalizeAndSend( *msg, status );
    User::WaitForRequest( status );
    delete msg;
    }

TUint32 CPFKeySocketIf::NewSpi()
    {
    iSpiBase++;
    return iSpiBase;
    }

//
// SocketReader::ShowMessage
//  Output actual "payload" messages (e.g. PFKEY)
//
#ifdef _DEBUG       
void CPFKeySocketIf::ShowMessageL( TPfkeyRecvMsg &aMsg )
    {
    HBufC* buffer = HBufC::NewL( 1000 );
    TPtr str( buffer->Des() );
    TPfkeyMessage msg( aMsg );

    if ( msg.iError )
        {
        str.Format( _L("Received malformed PFKEY msg of %d bytes: %d\n"),
                aMsg.Length(), msg.iError );
        }   
    else
        {
        msg.iBase.String( str, _L(" ") );
        msg.iSa.String( str, _L(" ") );
        msg.iCurrent.String( str, _L(" C=") );
        msg.iHard.String( str, _L(" H=") );
        msg.iSoft.String( str, _L(" S=") );
        msg.iSrcAddr.String( str, _L(" SRC=") );
        msg.iDstAddr.String( str, _L(" DST=") );
        msg.iProxyAddr.String( str, _L(" PROXY=") );
        msg.iAuthKey.String( str, _L(" AUTHKEY=") );
        msg.iEncryptKey.String( str, _L(" ENCRYPTKEY=") );
        msg.iSrcIdent.String( str, _L(" SRCI=") );
        msg.iDstIdent.String( str, _L(" DSTI=") );
        msg.iSensitivity.String( str, _L(" SENS=") );
        msg.iProposal.String( str, _L(" PROP=") );
        msg.iAuthAlgs.String( str, _L(" AUTH=") );
        msg.iEncryptAlgs.String( str, _L(" ENCR=") );
        msg.iSpirange.String( str, _L(" SPIR=") );
        msg.iTs.String( str, _L(" TS=") );
        msg.iPrivateExtension.String( str, _L(" GEN_EXT=") );
        }
    DEBUG_LOG( str );
    
    delete buffer;
    buffer = NULL;
    }
#endif    

//
// CPFKeySocketIf::RunL
//  Called when request completed
//
void CPFKeySocketIf::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        DEBUG_LOG1( _L("Socket read, iStatus=%d"), iStatus.Int() );    
        }

#ifdef _DEBUG    
    TRAP_IGNORE( ShowMessageL( iMsg ) );
#endif    
            
    TPfkeyMessage msg(iMsg);
    if ( ( msg.iError == KErrNone ) &&
         ( msg.iBase.iMsg->sadb_msg_errno == KErrNone ) ) // No error
        {
        switch ( msg.iBase.iMsg->sadb_msg_type )
            {
            case SADB_GETSPI:
                for (TInt i = 0; i < iPendingSpiRequests.Count(); ++i)
                    {
                    TPendingSpiRequest& pendingSpiRequest = iPendingSpiRequests[i];
                    if ((TUint32)&pendingSpiRequest.iClientStatus == msg.iBase.iMsg->sadb_msg_pid)
                        {
                        pendingSpiRequest.iSpi = msg.iSa.iExt->sadb_sa_spi;
                        TRequestStatus* status = &pendingSpiRequest.iClientStatus;
                        iPendingSpiRequests.Remove(i);
                        User::RequestComplete(status, KErrNone);
                        break;
                        }
                    }
                break;
            case SADB_ADD:     // Fall through
            case SADB_UPDATE:  // Fall through
            case SADB_ACQUIRE: // Fall through
            case SADB_EXPIRE:  // Fall through               
                iListener->PfkeyMessageReceived( msg );
                break;

            default:
                break;
            }       
        }
    else
        {   
        DEBUG_LOG2( _L("Error in Pfkey message, iError=%d, sadb_msg_errno=%d"),
                iStatus.Int(), msg.iBase.iMsg->sadb_msg_errno );
        }
    iMsg.Reset();
    iSadb.ReadRequest( iMsg, iStatus ); // Start a new read
    SetActive();
    }

//
// CPFKeySocketIf::DoCancel
//  Called when a pending request should be cancelled
//
void CPFKeySocketIf::DoCancel()
    {
    iSadb.CancelRecv();
    }

//
// CPFKeySocketIf::RunError
// Called when RunL() leaves 
//
TInt CPFKeySocketIf::RunError( TInt aError )
    {
    DEBUG_LOG1( _L("CPFKeySocketIf::RunError() aError=%d, PFKEY message lost"),
            aError );
    aError = aError;

    iMsg.Reset();
    iSadb.ReadRequest( iMsg, iStatus ); // Start a new read. 
    SetActive();
    
    return KErrNone; // Active scheduler Error() method NOT called              
    }

