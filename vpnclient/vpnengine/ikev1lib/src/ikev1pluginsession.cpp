/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKEv1 plugin session
*
*/


#include <random.h>
#include <vpnlogmessages.rsg>
#include "ikev1plugin.h"
#include "ikedebug.h"
#include "ikev1negotiation.h"
#include "ikev1isakmpstream.h"
#include "pfkeymsg.h"
#include "ikepolparser.h"
#include "kmdapi.h"
#include "ikev1crack.h"
#include "ikev1infonegotiation.h"
#include "ikev1SA.h"
#include "ikev1sender.h"
#include "ikesocketdefs.h"
#include "ikedatainterface.h"
#include "vpnaddrinfo.h"
#include "ipsecsaspiretriever.h"

// CLASS HEADER
#include "ikev1pluginsession.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev1PluginSession* CIkev1PluginSession::NewL( TUint32 aVpnIapId,
                                                TUint32 aVpnNetId,
                                                TUint32 aVpnInterfaceIndex,
                                                MIkeDataInterface& aDataInterface,
                                                CIkev1Plugin& aPlugin,
                                                CPFKeySocketIf& aPFKeySocketIf,
                                                MIkeDebug& aDebug )
    {
    CIkev1PluginSession* self = new ( ELeave ) CIkev1PluginSession( aVpnIapId,
                                                                    aVpnNetId,
                                                                    aVpnInterfaceIndex,
                                                                    aDataInterface,
                                                                    aPlugin,
                                                                    aPFKeySocketIf,
                                                                    aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkev1PluginSession::~CIkev1PluginSession()
    {
    // Cancel client's requests.
    DoCompleteNegotiateWithHost( KErrCancel );
    DoCompleteDeleteSession( KErrCancel );
    DoCompleteNotifyError( KErrCancel );
    DoCompleteInternalAddressChanged( KErrCancel );    
    
    if ( iDialogWaitQueue )
        {
        CIkev1Dialog::PurgeDialogQueue( iDialogWaitQueue );
        }   

    CIkev1Negotiation* negotiation;    
    while ( iFirstNegotiation )
        {
        negotiation = iFirstNegotiation;
        delete negotiation; // destructor removes object from queue, too
        }
    
    DoEmptySendQueue();
    iSendQueue.Close();
    
    while ( iIkev1SAs.Count() )
        {
        CIkev1SA* ikev1SA = iIkev1SAs[0];
        iIkev1SAs.Remove(0);
        delete ikev1SA;
        }
    iIkev1SAs.Close();
    
    delete iReceiver;
    delete iSender;
    delete iIkeData;
    delete iInternalAddress;
    
    iPlugin.IkePluginSessionDeleted( this );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev1PluginSession::CIkev1PluginSession( TUint32 aVpnIapId,
                                          TUint32 aVpnNetId,
                                          TUint32 aVpnInterfaceIndex,
                                          MIkeDataInterface& aDataInterface,
                                          CIkev1Plugin& aPlugin,
                                          CPFKeySocketIf& aPFKeySocketIf,
                                          MIkeDebug& aDebug )
: iVpnIapId( aVpnIapId ),
  iVpnNetId( aVpnNetId ),
  iVpnInterfaceIndex( aVpnInterfaceIndex ),
  iDataInterface( aDataInterface ),
  iPlugin( aPlugin ),
  iPFKeySocketIf( aPFKeySocketIf ),
  iDebug( aDebug )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::ConstructL()
    {
    TPtr8 ptr( (TUint8*)&iSAIdSeed, sizeof(iSAIdSeed) );
    ptr.SetLength( sizeof(iSAIdSeed) );
    TRandom::RandomL( ptr );  
    iSAIdSeed &= 0x7fffffff;  // Reset the most significant bit    

    iReceiver = CIkev1Receiver::NewL( iDataInterface,
                                      *this );   
    iSender = CIkev1Sender::NewL( iDataInterface,
                                  *this,
                                  iDebug );    
    
    DEBUG_LOG1( _L("CIkev1PluginSession::ConstructL, SAId seed: %d"),
            iSAIdSeed );
    }    

// ---------------------------------------------------------------------------
// Handles IKE SA deletion request.
// ---------------------------------------------------------------------------
//
TBool CIkev1PluginSession::DeleteIkeSA( TIkev1SAData* aIkev1SaData,
                                        TBool aSilentClose )
    {
    //
    // An IKE SA delete request received
    // Check first does there exists an ongoing negotiation on this IKE
    // SA deleted and delete this block. 
    // Allocate a new negotiation with TIkev1SAData and initiate IKE SA
    // deletion request
    //
    DEBUG_LOG1( _L("Deleting IKEv1 SA SAID =  %d"),
            aIkev1SaData->iSAId );
    
    CIkev1Negotiation* negotiation = FindNegotiation( aIkev1SaData->iSAId );
    while ( negotiation )
        {
        delete negotiation; // destructor removes object from queue, too
        negotiation = FindNegotiation( aIkev1SaData->iSAId );         
        }

    TBool started( EFalse );

    if ( !aSilentClose )
        {
        DeleteIpsecSAs( aIkev1SaData->iSAId );
        
        TRAPD( err, 
                {
                // Trap the SendDeleteL -- it can fail, but the failure won't be fatal 
                // (delete payload just won't be sent)
                negotiation = CIkev1Negotiation::NewL( this,
                                                       iPFKeySocketIf,
                                                       iDebug,
                                                       aIkev1SaData,
                                                       RESPONDER ); // Nevermind INITIATOR or RESPONDER
                negotiation->SendDeleteL( PROTO_ISAKMP );
                } );
                
        delete negotiation;
        negotiation = NULL;
        
        if ( err == KErrNone ) 
            {
            // DELETE payload sent succesfully.
            DEBUG_LOG( _L("CIkev1PluginSession::DeleteIkeSAL() IKEv1 delete send OK") );            
            started = ETrue;
            }
        else 
            {
            DEBUG_LOG1( _L("CIkev1PluginSession::DeleteIkeSAL() IKEv1 delete send failed, err=%d"), err );
            }
        }
#ifdef _DEBUG
    else
        {
        DEBUG_LOG( _L("Forced close, no delete payload(s) sent"));
        }
#endif    
    ExpireIkev1SA( aIkev1SaData->iSAId );  // Set expired to delete IKE SA
    
    if ( FindIkev1SA() == NULL &&
         FirstNegotiation() == NULL )
        {
        // Set error status, when expired IKE SA was the only IKE SA and there
        // is no ongoing negotiation.        
        iErrorStatus = KKmdIkeNegotFailed;
        }

    return started;
    }

// ---------------------------------------------------------------------------
// Handles IKE SA rekeying request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::RekeyIkeSAL( TIkev1SAData* aIkev1SaData,
                                       CSARekeyInfo* aSaRekeyInfo )
    {
    CIkev1Negotiation* negotiation = CIkev1Negotiation::NewL( this,
                                                              iPFKeySocketIf,
                                                              iDebug,
                                                              aIkev1SaData->iRemoteAddr,
                                                              EFalse );
    CleanupStack::PushL( negotiation );
    negotiation->SetRekeyInfo( aSaRekeyInfo );                
    negotiation->InitNegotiationL();
    if ( negotiation->Finished() )
        {
        CleanupStack::PopAndDestroy( negotiation );
        }
    else 
        {
        CleanupStack::Pop( negotiation );  
        }
    }

// ---------------------------------------------------------------------------
// Handles IKE SA keepalive request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::KeepAliveIkeSAL( TIkev1SAData* aIkev1SaData )
    {
    CIkev1Negotiation* negotiation = CIkev1Negotiation::NewL( this,
                                                              iPFKeySocketIf,
                                                              iDebug,
                                                              aIkev1SaData,
                                                              RESPONDER );
    CleanupStack::PushL( negotiation );
    negotiation->SendKeepAliveMsgL( aIkev1SaData );
    if ( negotiation->Finished() )
        {
        CleanupStack::PopAndDestroy( negotiation );
        }
    else
        {
        CleanupStack::Pop( negotiation );  
        }
    }

// ---------------------------------------------------------------------------
// Creates IKE SA.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::CreateIkev1SAL( TIkev1SAData& aIkev1SaData,
                                          CSARekeyInfo* aSaRekey )
    {
    CIkev1SA* ikev1SA = CIkev1SA::NewL( *this,
                                        aIkev1SaData,
                                        aSaRekey,
                                        iDebug );
    
    if ( !aIkev1SaData.iInitiator )
        {
        // Move SPI list from previous IKE SA to new IKE SA
        for ( TInt i=0;i<iIkev1SAs.Count();i++ )
            {
            CIkev1SA* previousSA = iIkev1SAs[i];
            if ( previousSA->iSPIList != NULL )
                {
                DEBUG_LOG(_L("Move SPI list to new IKE SA"));
                delete ikev1SA->iSPIList;
                ikev1SA->iSPIList = previousSA->iSPIList;
                previousSA->iSPIList = new (ELeave) CIpsecSPIList(1);  // Dummy;
                break;
                }
            }        
        }        
    
    // Cancel IKE SA rekeying from other IKE SAs.
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* cancelSA = iIkev1SAs[i];
        cancelSA->CancelRekey();
        }
    
    CleanupStack::PushL( ikev1SA );
    iIkev1SAs.AppendL( ikev1SA );
    CleanupStack::Pop( ikev1SA );    
    }

// ---------------------------------------------------------------------------
// Updates IKE SA.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::UpdateIkev1SAL( TUint32 aSaId,
                                          TBool aExpired,
                                          TIkev1SAData* aIkev1SaData )
    {
    if ( !aExpired )
        {
        CIkev1SA* Ikev1SA = FindIkev1SAWithId( aSaId );
        if ( Ikev1SA )
            {
            Ikev1SA->UpdateSAL( aExpired, aIkev1SaData );
            }
        }
    else
        {
        ExpireIkev1SA( aSaId );
        }
    }

// ---------------------------------------------------------------------------
// Expires IKE SA.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::ExpireIkev1SA( TUint32 aSaId )
    {
    CIkev1SA* ikev1SA = FindIkev1SAWithId( aSaId );
    if ( ikev1SA )
        {
        ikev1SA->ExpireSA();
        }
    }

// ---------------------------------------------------------------------------
// Removes IKE SA.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::RemoveIkeSA( CIkev1SA* aIkev1Sa,
                                       TInt aStatus )
    {        
    TInt dpdRetryCount( 0 );
    
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if ( aIkev1Sa == sa )
            {
            // Remove IKE SA from array.
            dpdRetryCount = sa->iHdr.iDPDRetry;
            iIkev1SAs.Remove(i);
            delete sa;
            break;
            }   
        }
    
    if ( (iErrorStatus == KErrNone) &&
         (dpdRetryCount > KMaxDpdRetryCount) &&
         (FindIkev1SA() == NULL) )
        {
        // If DPD retry count was reached for only IKE SA, set error status.
        iErrorStatus = KKmdIkeNoResponseErr;        
        }
        
    // If session deletion has been requested, complete session deletion
    // request.        
    DoCompleteDeleteSession( aStatus );
        
    // If fatal error has occured, complete error notification.    
    if ( iErrorStatus != KErrNone )
        {
        DoHandleError( iErrorStatus );
        }
    }

// IKE SA find methods.

CIkev1SA* CIkev1PluginSession::FindIkev1SA()
    {
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if ( (!sa->IsExpired()) )
            {
            return sa;
            }   
        }
    return NULL;
    }

CIkev1SA* CIkev1PluginSession::FindIkev1SA( const TCookie& aCookie_I,
                                            const TCookie& aCookie_R )
    {
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if ( (sa->iHdr.iCookie_I ==  aCookie_I) &&
             (sa->iHdr.iCookie_R ==  aCookie_R) &&
             (!sa->IsExpired()) )
            {
            return sa;
            }   
        }
    return NULL;
    }

CIkev1SA* CIkev1PluginSession::FindIkev1SAWithId( TUint32 aSaId )
    {
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if ( (sa->iHdr.iSAId == aSaId) &&
             (!sa->IsExpired()) )
            {
            return sa;
            }   
        }
    return NULL;
    }

CIkev1SA* CIkev1PluginSession::FindIkev1SA( const TInetAddr& aAddr )
    {
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if (  sa->iHdr.iRemoteAddr.Match(aAddr) &&
              (!sa->IsExpired()) ) 
            {
            return sa;
            }   
        }
    return NULL;
    }

CIkev1SA* CIkev1PluginSession::FindIkev1SA( const TInetAddr& aAddr,
                                            TUint32 aInboundSpi )
    {
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if (  sa->iHdr.iRemoteAddr.Match(aAddr) &&
              (!sa->IsExpired()) ) 
            {
            if ( sa->FindIpsecSPI(aInboundSpi, ETrue) )
                {
                return sa;
                }
            }   
        }
    return NULL;
    }

TIkev1SAData* CIkev1PluginSession::FindIkev1SAData()
    {
    TIkev1SAData* saData = NULL;        
    CIkev1SA* ikev1SA = FindIkev1SA();
    if ( ikev1SA )
        {
        saData = (TIkev1SAData*)&ikev1SA->iHdr;
        }
    return saData;
    }

TIkev1SAData* CIkev1PluginSession::FindIkev1SAData( const TCookie& aCookie_I,
                                                    const TCookie& aCookie_R )
    {
    TIkev1SAData* saData = NULL;        
    CIkev1SA* ikev1SA = FindIkev1SA( aCookie_I, aCookie_R );
    if ( ikev1SA )
        {
        saData = (TIkev1SAData*)&ikev1SA->iHdr;
        }
    return saData;
    }

TIkev1SAData* CIkev1PluginSession::FindIkev1SAData( TUint32 aSaId )
    {
    TIkev1SAData* saData = NULL;        
    CIkev1SA* ikev1SA = FindIkev1SAWithId( aSaId );
    if ( ikev1SA )
        {
        saData = (TIkev1SAData*)&ikev1SA->iHdr;
        }
    return saData;
    }

TIkev1SAData* CIkev1PluginSession::FindIkev1SAData( const TInetAddr& aAddr,
                                                    TUint32 aInboundSpi )
    {
    TIkev1SAData* saData = NULL;        
    CIkev1SA* ikev1SA = FindIkev1SA( aAddr, aInboundSpi );
    if ( ikev1SA )
        {
        saData = (TIkev1SAData*)&ikev1SA->iHdr;
        }
    return saData;
    }    

TIkev1SAData* CIkev1PluginSession::FindIkev1SADataWithAddr( const TInetAddr& aAddr )
    {
    TIkev1SAData* saData = NULL;        
    CIkev1SA* ikev1SA = FindIkev1SA( aAddr );
    if ( ikev1SA )
        {
        saData = (TIkev1SAData*)&ikev1SA->iHdr;
        }
    return saData;
    }    

// ---------------------------------------------------------------------------
// Handles IPsec SA deletion request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DeleteIpsecSAL( TIkev1SAData* aIkev1SaData,
                                          TIpsecSPI* aIpsecSpi )
    {
    //
    // Send a delete payload for specified IPSec SA 
    //
    CIkev1Negotiation* negotiation = CIkev1Negotiation::NewL( this,
                                                              iPFKeySocketIf,
                                                              iDebug,                                                              
                                                              aIkev1SaData,
                                                              RESPONDER );
    CleanupStack::PushL( negotiation );
    negotiation->SendDeleteL( aIpsecSpi->iProtocol,
                              aIpsecSpi->iSPI );
    CleanupStack::PopAndDestroy( negotiation );
    }

// ---------------------------------------------------------------------------
// Deletes IPsec SAs.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DeleteIpsecSAs( TUint32 aSaId )
    {
    CIkev1SA* ikev1SA = FindIkev1SAWithId( aSaId );
    if ( ikev1SA )
        {
        ikev1SA->DeleteIpsecSAs(); 
        }   
    }

// ---------------------------------------------------------------------------
// Deletes IPsec SPI.
// ---------------------------------------------------------------------------
//
TBool CIkev1PluginSession::DeleteIpsecSpi( TUint32 aSaId,
                                           TUint32 aSpi,
                                           TBool aInbound )
    {
    TBool status = EFalse;
    CIkev1SA* ikev1SA = FindIkev1SAWithId( aSaId );
    if ( ikev1SA )
        {
        status = ikev1SA->DeleteIpsecSPI( aSpi, aInbound );
        }
    return status;
    }

// ---------------------------------------------------------------------------
// Adds IPsec SPI to IKE SA.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::AddIpsecSPIToSAL( TUint32 aSaId,
                                            TIpsecSPI& aIpsecSpi )
    {
    CIkev1SA* ikev1SA = FindIkev1SAWithId( aSaId );
    if ( ikev1SA )
        {
        ikev1SA->AddIpsecSPIL( aIpsecSpi );
        }
    }

// ---------------------------------------------------------------------------
// Returns dialog anchor.
// ---------------------------------------------------------------------------
//
CIkev1Dialog** CIkev1PluginSession::DialogAnchor()
    {
    return &iDialogWaitQueue;
    }

// ---------------------------------------------------------------------------
// Returns debug trace interface.
// ---------------------------------------------------------------------------
//
MIkeDebug& CIkev1PluginSession::Debug()
    {
    return iDebug;
    }

// ---------------------------------------------------------------------------
// Gets SA id.
// ---------------------------------------------------------------------------
//
TUint32 CIkev1PluginSession::GetSAId()
    {
    iSAIdSeed++;
    return iSAIdSeed;
    }

// ---------------------------------------------------------------------------
// Deletes ISAKMP SAs.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DeleteISAKMPSAsL( TDeleteISAKMP* aDeletePayload,
                                            const CIkev1Negotiation& aInfoNegotiation )
    {
    TCookie cookie_I, cookie_R;

    // It should always be only one.
    for ( TInt i=0; i < aDeletePayload->NumSPI(); i++ )
        {
        if ( aDeletePayload->SPISize() < 2 * ISAKMP_COOKIE_SIZE ) //The ISAKMPSA SPI is the union of both cookies
            {
            DEBUG_LOG( _L("Bad SPI Size for a ISAKMP SA. (SA Not deleted)") );
            return;
            }
        cookie_I.Copy( aDeletePayload->SPI(i), ISAKMP_COOKIE_SIZE );
        cookie_R.Copy( aDeletePayload->SPI(i) + ISAKMP_COOKIE_SIZE,
                       ISAKMP_COOKIE_SIZE);

        CIkev1Negotiation* neg = iFirstNegotiation;
        while ( neg )
            {
            CIkev1Negotiation* current = neg;
            neg = neg->iNext;

            // Delete any active negotiations with the same cookies.
            // Currently used negotiation is not deleted.
            if ( ( current != &aInfoNegotiation ) &&
                 ( current->iCookie_I == cookie_I ) &&
                 ( current->iCookie_R == cookie_R ) )
                {
                DEBUG_LOG( _L("Active negotiation deleted.") );
                delete current;
                current = NULL;
                }
            }
        
        // Expire IKE SA.
        TIkev1SAData* sa = FindIkev1SAData( cookie_I, cookie_R );
        if ( sa )
            {
            UpdateIkev1SAL( sa->iSAId, ETrue );
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets local IP address.
// ---------------------------------------------------------------------------
//
TInt CIkev1PluginSession::GetLocalAddress( TInetAddr& aAddr )
    {
    TInt err( KErrNone );
    if ( iLocalAddr.IsUnspecified() )
        {
        err = iDataInterface.GetLocalAddress( iLocalAddr );
        }
    
    aAddr = iLocalAddr;
    return err;
    }

// ---------------------------------------------------------------------------
// Sends IKE message.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::SendIkeMsgL( const TDesC8& aIkeMsg,
                                       TInetAddr& aDestAddr,
                                       TBool aUseNatPort )
    {
    // Construct buffer for storing IKE message data.
    TInt localPort = ( aUseNatPort ?
                       IkeSocket::KIkePort4500 :
                       IkeSocket::KIkePort500 );    
    TInt length = aIkeMsg.Length();    
    if ( localPort == IkeSocket::KIkePort4500 )
        {
        // Reserve space for <non-ESP marker>.
        length += NON_ESP_MARKER_SIZE;
        }    
    HBufC8* ikeMsg = HBufC8::NewL( length );    
    TPtr8 ptr = ikeMsg->Des();    
    if ( localPort == IkeSocket::KIkePort4500 )
        {    
        // Append <non-ESP marker> to the beginning of IKE message.
        TUint32 nonEspMarker = NON_ESP_MARKER;        
        TUint8* nonEspPtr = (TUint8*)&nonEspMarker;
        ptr.Append( nonEspPtr, NON_ESP_MARKER_SIZE );
        } 
    // Append IKE message data to descriptor.
    ptr.Append( aIkeMsg );    
    
    DoSendUdpDataL( ikeMsg, // Ownership transferred.
                    aDestAddr,
                    localPort,
                    0 );
    }

// ---------------------------------------------------------------------------
// Sends NAT keep-alive packet.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::SendNatKeepAliveL( TInetAddr& aDestAddr,
                                             const TDesC8& aData,
                                             TUint8 aDscp )
    {
    DEBUG_LOG(_L("CIkev1PluginSession::SendNatKeepAliveL"));
    HBufC8* udpData = HBufC8::NewL( aData.Length() );
    *udpData = aData;
    TInt localPort = aDestAddr.Port();
    DoSendUdpDataL( udpData, // Ownership transferred.
                    aDestAddr,
                    localPort,
                    aDscp );
    }

// ---------------------------------------------------------------------------
// Sends Nokia NAT keep-alive packet.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::SendNokiaNatKeepAliveL( TInetAddr& aDestAddr,
                                                  const TDesC8& aData,
                                                  TUint8 aDscp )    
    {
    DEBUG_LOG(_L("CIkev1PluginSession::SendNokiaNatKeepAliveL"));
    SendNatKeepAliveL( aDestAddr,
                       aData,
                       aDscp );
    }

// ---------------------------------------------------------------------------
// Handles completion of IKE SA establishment.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::IkeSaCompleted( TInt aStatus,
                                          CInternalAddress* aInternalAddress )
    {
    delete iInternalAddress;
    iInternalAddress = aInternalAddress;
    
    DoCompleteNegotiateWithHost( aStatus );
    }

// ---------------------------------------------------------------------------
// Deletes negotiation object.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::DeleteNegotiation( CIkev1Negotiation* aNegotiation )
    {
    TInt err = ErrorStatus();
    delete aNegotiation;        
    
    if ( err == KErrNone &&
         (iClientStatusNegotiate != NULL ||
          iClientStatusDelete != NULL) &&
         (FindIkev1SA() == NULL) )
        {
        // If negotiate or delete session request has been issued, and
        // there are no IKE SAs, client is completed with error.
        err = KKmdIkeNegotFailed;
        }

    DEBUG_LOG1(_L("IKEv1 negotiation deleted, status=%d"),
            err );
    
    if ( err != KErrNone )
        {
        //
        // IKE negotiation failed.
        //
        DoHandleError( err );
        }        
    }

// Negotiation linking and find methods.

void CIkev1PluginSession::LinkNegotiation( CIkev1Negotiation* aNegotiation )
    {  
    aNegotiation->iNext = iFirstNegotiation;  
    iFirstNegotiation = aNegotiation; 
    }    
    
CIkev1Negotiation* CIkev1PluginSession::FirstNegotiation()
    {
    return iFirstNegotiation;
    }

CIkev1Negotiation* CIkev1PluginSession::FindNegotiation( TCookie aInit,
                                                         TCookie aResp,
                                                         TUint8 aExchange,
                                                         TUint32 aMsgId )
    {
    CIkev1Negotiation* negotiation;
    TCookie NULL_COOKIE;
    NULL_COOKIE.FillZ(ISAKMP_COOKIE_SIZE);

    if ( aExchange == ISAKMP_EXCHANGE_INFO )
        {
        for ( negotiation = iFirstNegotiation;
              negotiation;
              negotiation = negotiation->iNext )
            {
            if ( (negotiation->iCookie_I.Compare(aInit) == 0 ) &&
                 ((negotiation->iCookie_R.Compare(aResp) == 0 ) ||
                   (negotiation->iCookie_R.Compare(NULL_COOKIE) == 0)) )
                {   
                return negotiation;
                }   
            }
        
        }   
    else
        {
        for ( negotiation = iFirstNegotiation;
              negotiation;
              negotiation = negotiation->iNext )
            {            
            if ( negotiation->iCookie_I.Compare(aInit) == 0 )
                {
                if ( (negotiation->iMessageId == aMsgId) ||
                     (negotiation->iMessageId == 0) )
                    {
                    if ( (negotiation->iCookie_R.Compare(aResp) == 0 ) ||
                         (negotiation->iCookie_R.Compare(NULL_COOKIE) == 0) ||
                         (aResp.Compare(NULL_COOKIE) == 0) )
                        {
                        return negotiation;
                        }
                    }
                }
            }           
        }   

    return NULL; // Not found
    }

CIkev1Negotiation* CIkev1PluginSession::FindNegotiation( TUint32 aSaId )
    {
    //
    // Find IKEv1 negotiation object using SA id as search argument 
    //
    CIkev1Negotiation* negotiation = iFirstNegotiation;
    while ( negotiation )
        {
        if ( negotiation->SAId() == aSaId )
            {   
            break;
            }   

        negotiation = negotiation->iNext;
        }   
    return negotiation;     
    }

void CIkev1PluginSession::RemoveNegotiation( CIkev1Negotiation* aNegotiation )
    {
    CIkev1Negotiation* prev = NULL;
    CIkev1Negotiation* negotiation  = iFirstNegotiation;
    
    while ( negotiation )
        {
        if ( negotiation == aNegotiation )
            {
            if ( prev )
                {
                prev->iNext = negotiation->iNext;
               }
           else 
               {
               iFirstNegotiation = negotiation->iNext;
               }
            break;  
            }
        prev = negotiation;
        negotiation = negotiation->iNext;
        }   
    }

// ---------------------------------------------------------------------------
// Handles completion of authentication dialog processing.
// ---------------------------------------------------------------------------
//    
TInt CIkev1PluginSession::AuthDialogCompletedL( CAuthDialogInfo* aUserInfo )
    {
    CIkev1Negotiation* negotiation = FindNegotiation( aUserInfo->SAId() );
    if ( negotiation )
        {
        DEBUG_LOG1( _L("Dialog completed for SAID: %d"),
                aUserInfo->SAId() );
        
        negotiation->AuthDialogCompletedL(aUserInfo);
        if ( negotiation->Finished() )
            {
            DeleteNegotiation( negotiation );
            }   
        return KErrNone;
        }   
    DEBUG_LOG1( _L("Dialog completed, no negotiation found for SAID: %d"),
            aUserInfo->SAId() );
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Handles change of internal address.
// ---------------------------------------------------------------------------
//    
TBool CIkev1PluginSession::InternalAddressChangedL( const CInternalAddress& aInternalAddr )
    {
    TBool internalAddressChanged( ETrue );
    
    if ( iInternalAddress )
        {
        if ( iInternalAddress->iClientIntAddr.Match(aInternalAddr.iClientIntAddr) )                    
            {
            internalAddressChanged = EFalse;
            }
        }        
    
    delete iInternalAddress;
    iInternalAddress = NULL;
    iInternalAddress = CInternalAddress::NewL( aInternalAddr );
    
    if ( internalAddressChanged )
        {
        DoCompleteInternalAddressChanged( KErrNone );
        }
    
    return internalAddressChanged;
    }

// ---------------------------------------------------------------------------
// Gets acceptable IPsec policies for specified selectors.
// ---------------------------------------------------------------------------
//
CIpsecSaSpecList* CIkev1PluginSession::GetIpseSaSpecListLC( const TInetAddr& aLocalAddr, const TInetAddr& aLocalMask, 
                                                            const TInetAddr& aRemoteAddr, const TInetAddr& aRemoteMask,
                                                            TInt aProtocol )

    {
    return iPlugin.GetIpseSaSpecListLC( aLocalAddr, aLocalMask, 
                                        aRemoteAddr, aRemoteMask,
                                        aProtocol, iVpnNetId );
    }

// ---------------------------------------------------------------------------
// Matches destination address to remote address in IKE policy data.
// ---------------------------------------------------------------------------
//
TBool CIkev1PluginSession::MatchDestinationAddress( const TInetAddr& aDestAddr )
    {
    TBool match( EFalse );
    
    if ( iIkeData )
        {
        match = iIkeData->iAddr.Match( aDestAddr );
        }    
    return match;
    }

// ---------------------------------------------------------------------------
// Handles fatal error.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::HandleError( TInt aStatus )
    {
    DoHandleError( aStatus );
    }

// ---------------------------------------------------------------------------
// Returns error status.
// ---------------------------------------------------------------------------
//
TInt CIkev1PluginSession::ErrorStatus()
    {
    return iErrorStatus;
    }

// ---------------------------------------------------------------------------
// Sets error status.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::SetErrorStatus( TInt aStatus )
    {
    if ( iErrorStatus == KErrNone )
        {
        iErrorStatus = aStatus;
        }
    }

// ---------------------------------------------------------------------------
// Returns VPN IAP id.
// ---------------------------------------------------------------------------
//
TUint32 CIkev1PluginSession::VpnIapId()
    {
    return iVpnIapId;
    }

// ---------------------------------------------------------------------------
// Returns VPN interface index.
// ---------------------------------------------------------------------------
//
TUint32 CIkev1PluginSession::VpnInterfaceIndex()
    {
    return iVpnInterfaceIndex;
    }

// ---------------------------------------------------------------------------
// Returns IKE policy data.
// ---------------------------------------------------------------------------
//
CIkeData& CIkev1PluginSession::IkeData()
    {
    __ASSERT_DEBUG( iIkeData != NULL,
                    User::Invariant() );
    return *iIkeData;
    }

// ---------------------------------------------------------------------------
// Returns UID.
// ---------------------------------------------------------------------------
//
TUint32 CIkev1PluginSession::Uid()
    {
    return iPlugin.Uid();
    }

// ---------------------------------------------------------------------------
// Returns event logger interface.
// ---------------------------------------------------------------------------
//
MKmdEventLoggerIf& CIkev1PluginSession::EventLogger()
    {
    return iPlugin.EventLogger();
    }      

// ---------------------------------------------------------------------------
// Returns internal address (NULL if does not exist). 
// ---------------------------------------------------------------------------
//
CInternalAddress* CIkev1PluginSession::InternalAddressL()
    {
    DEBUG_LOG(_L("CIkev1PluginSession::InternalAddressL"));
    
    CInternalAddress* internalAddress = NULL;
    if ( iInternalAddress != NULL )
        {
        internalAddress = CInternalAddress::NewL( *iInternalAddress );    
        }
    return internalAddress;            
    }

// ---------------------------------------------------------------------------
// From class MIkev1SenderCallback
// Handles completion of sending.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::SendUdpDataCompleted( TInt aStatus )
    {    
    if ( iSendQueue.Count() != 0 )
        {
        // Send queue is not empty.
        // Send next item from queue.
        TIkeSendQueueItem item = iSendQueue[0];
        HBufC8* udpData = item.UdpData();
        TInetAddr destAddr = item.DestAddr();
        TInt localPort = item.LocalPort();
        TUint8 dscp = item.Dscp();
        iSendQueue.Remove(0);
        DoSendUdpData( udpData,
                       destAddr,
                       localPort,
                       dscp );
        
        }
    else
        {
        // IKE message send queue is empty.
        // If session deletion has been requested, complete request.
        DoCompleteDeleteSession( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// Handles PFKEY message.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage )
    {
    TRAPD( err, DoPfkeyMessageReceivedL( aPfkeyMessage) );
    err = err;
    DEBUG_LOG1(_L("CIkev1PluginSession::PfkeyMessageReceivedL, err=%d"), err);
    }

// Methods to build and send PFKEY API primitives to IPsec

void CIkev1PluginSession::AcquireSAError( TIpsecSAData& aSAData,
                                          TInt aError )
    {
    iPlugin.AcquireSAError( aSAData, aError );
    }

void CIkev1PluginSession::UpdateSAL( TIpsecSAData& aSaData )
    {
    iPlugin.UpdateSAL( aSaData );
    }

void CIkev1PluginSession::AddSAL( TIpsecSAData& aSaData )
    {
    iPlugin.AddSAL( aSaData );
    }

void CIkev1PluginSession::DeleteIpsecSA( TIpsecSPI& aIpsecSpi )
    {
    iPlugin.DeleteIpsecSA( aIpsecSpi );
    }

void CIkev1PluginSession::DeleteIpsecSA( TUint32 aSPI,
                                         TInetAddr& aSrc, 
                                         TInetAddr& aDst,
                                         TUint8 aProtocol )
    {
    iPlugin.DeleteIpsecSA( aSPI, aSrc, aDst, aProtocol );
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Starts negotiation with a peer. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::NegotiateWithHost( const CIkeData& aIkeData,
                                             TVPNAddress& aInternalAddress,
                                             TRequestStatus& aStatus )
    {
    DEBUG_LOG(_L("CIkev1PluginSession::NegotiateWithHost"));    
    __ASSERT_DEBUG( iClientStatusNegotiate == NULL,
                    User::Invariant() );
    
    // Store client's request status and internal address.
    iClientStatusNegotiate = &aStatus;
    *iClientStatusNegotiate = KRequestPending;
    iClientIaNegotiate = &aInternalAddress;
    
    TRAPD( err, DoNegotiateWithHostL( aIkeData ) );
    
    if ( err != KErrNone )
        {
        DoCompleteNegotiateWithHost( err );
        }
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Cancels negotiate request. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::CancelNegotiateWithHost()
    {    
    DEBUG_LOG(_L("CIkev1PluginSession::CancelNegotiateWithHost"));
    
    if ( iClientStatusNegotiate != NULL )
        {
        // Completion is enough as deletion of session is requested after
        // cancellation.
        DoCompleteNegotiateWithHost( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Deletes session. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DeleteSession( const TBool aSilentClose,
                                         TRequestStatus& aStatus )
    {
    DEBUG_LOG1(_L("CIkev1PluginSession::DeleteSession, silent=%d"),
            aSilentClose);
    
    iClientStatusDelete = &aStatus;
    *iClientStatusDelete = KRequestPending;
    
    TBool deactivatingStarted = DeleteSAsWithHost( aSilentClose );    
    if ( !deactivatingStarted )
        {
        DoCompleteDeleteSession( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Cancels deletion requests. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::CancelDeleteSession()
    {
    DEBUG_LOG(_L("CIkev1PluginSession::CancelDeleteSession"));

    if ( iClientStatusDelete != NULL )
        {
        // Delete SAs silently.
        DeleteSAsWithHost( ETrue );
        
        DoCancelDataTransfer();
        
        User::RequestComplete( iClientStatusDelete, KErrCancel );
        iClientStatusDelete = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Requests notification about error condition. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::NotifyError( TRequestStatus& aStatus )
    {
    DEBUG_LOG(_L("CIkev1PluginSession::NotifyError"));
    
    iClientStatusNotifyError = &aStatus;
    *iClientStatusNotifyError = KRequestPending;
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Cancels error notification request. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::CancelNotifyError()
    {
    DEBUG_LOG(_L("CIkev1PluginSession::CancelNotifyError"));
    
    if ( iClientStatusNotifyError != NULL )
        {
        User::RequestComplete( iClientStatusNotifyError, KErrCancel );
        iClientStatusNotifyError = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Requests notification about change of internal address. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::NotifyInternalAddressChanged( TVPNAddress& aInternalAddress,
                                                        TRequestStatus& aStatus )
    {
    DEBUG_LOG(_L("CIkev1PluginSession::NotifyInternalAddressChanged"));
    __ASSERT_DEBUG( iClientStatusNotifyIaChange == NULL,
                    User::Invariant() );
    
    iClientStatusNotifyIaChange = &aStatus;
    *iClientStatusNotifyIaChange = KRequestPending;
    iClientIaNotify = &aInternalAddress;    
    }

// ---------------------------------------------------------------------------
// From class MIkePluginSessionIf
// Cancels internal address change notification request. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::CancelNotifyInternalAddressChanged()
    {    
    DEBUG_LOG(_L("CIkev1PluginSession::CancelNotifyInternalAddressChanged"));
    
    if ( iClientStatusNotifyIaChange != NULL )
        {
        DoCompleteInternalAddressChanged( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// From class MIkev1ReceiverCallback
// Handles notification about received IKE message. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::IkeMsgReceivedL( const ThdrISAKMP& aIkeMsg,
                                           const TInetAddr& aSrcAddr,
                                           TInt aLocalPort )
    {
    CIkev1Negotiation* negotiation = NULL;
    TIkev1SAData* sa = NULL;
    
    TUint8 exchange = aIkeMsg.GetExchange();
    negotiation = FindNegotiation( aIkeMsg.GetCookieI(),
                                   aIkeMsg.GetCookieR(),
                                   exchange,
                                   aIkeMsg.GetMessageId() );
    
    if ( exchange == ISAKMP_EXCHANGE_INFO || exchange == ISAKMP_EXCHANGE_TRANSACT )
        {
#ifdef _DEBUG
        if ( exchange == ISAKMP_EXCHANGE_INFO )
            {
            DEBUG_LOG( _L("---ISAKMP_EXCHANGE_INFO message received---") );
            }
        else 
            {
            DEBUG_LOG( _L("---ISAKMP_EXCHANGE_TRANSACTION message received---") );
            }                
#endif
        TBool inactive = EFalse;                 
        if ( !negotiation ) 
            {
            if ( exchange == ISAKMP_EXCHANGE_INFO )
                {
                sa = FindIkev1SAData( aIkeMsg.GetCookieI(),
                                      aIkeMsg.GetCookieR() );
                if ( sa )
                    {
                    negotiation = CIkev1Negotiation::NewL( this,
                                                           iPFKeySocketIf,
                                                           iDebug,
                                                           sa,
                                                           RESPONDER );
                    }       
                }       
            if ( !negotiation )
                {
                DEBUG_LOG( _L("Cannot find a matching negotiation") );
#ifdef _DEBUG                    
                const TPtrC8 ikeMsgPtr( (TUint8 *)&aIkeMsg, (TUint16)aIkeMsg.GetLength() );
                TInetAddr dstAddr;
                GetLocalAddress( dstAddr );
                dstAddr.SetPort( aLocalPort );
                TRACE_MSG_IKEV1( ikeMsgPtr, aSrcAddr, dstAddr );
#endif // _DEBUG                    
                return; //Not found
                }
            CleanupStack::PushL( negotiation );
            inactive = ETrue;   //Not enqueued because not active. Only used to process the packet
            }

        if ( exchange == ISAKMP_EXCHANGE_INFO )
            {                   
            CIkev1InfoNegotiation* info_neg = new (ELeave) CIkev1InfoNegotiation( *this,
                                                                                  *negotiation,
                                                                                  iDebug );
            CleanupStack::PushL( info_neg );            
            // Update the negotiation state
            info_neg->ExecuteL( aIkeMsg,
                                aSrcAddr,
                                aLocalPort  );
            CleanupStack::PopAndDestroy( info_neg );
            
            if ( inactive )
                {   
                CleanupStack::PopAndDestroy( negotiation );
                }
            else
                {
                if ( negotiation->Finished() )
                    {
                    DeleteNegotiation( negotiation );
                    }
                }   
            }
        else
            { 
            //
            // An ISAKMP transaction exchange message received
            // The handling of this requires that there exists a
            // CTransNegotiation object pointer linked into current
            // CIkev1Negotiation object 
            //
            if ( negotiation )
                {
                if ( negotiation->ExecuteTransactionL( aIkeMsg,
                                                       aSrcAddr,
                                                       aLocalPort ) )
                    {
                    if ( negotiation->Finished() )
                        {
                        DeleteNegotiation( negotiation );
                        }
                    }
                else
                    {
                    DEBUG_LOG( _L("Unexpected Transaction excange message") );
#ifdef _DEBUG   
                    const TPtrC8 ikeMsgPtr( (TUint8 *)&aIkeMsg, (TUint16)aIkeMsg.GetLength() );
                    TInetAddr dstAddr;
                    GetLocalAddress( dstAddr );
                    dstAddr.SetPort( aLocalPort );
                    TRACE_MSG_IKEV1( ikeMsgPtr, aSrcAddr, dstAddr );                        
#endif // _DEBUG                        
                    }     
                }      
            }   
        return;
        }

    //
    // IKE Main, Aggressive and Quick mode exchanges
    //
    if ( negotiation )
        {
        negotiation->ExecuteL( aIkeMsg, aSrcAddr, aLocalPort );
        if ( negotiation->Finished() )
            {
            DeleteNegotiation( negotiation );
            }
        return;
        }   
    DEBUG_LOG( _L("No active negotiation found...Searching existing PHASE_II") );

    TBool status;
    sa = FindIkev1SAData( aIkeMsg.GetCookieI(),
                          aIkeMsg.GetCookieR() );
    if ( sa )
        {
        DEBUG_LOG( _L("Creating a NEW IKE Phase 2 Negotiation") );           
        
        TRAPD( err, negotiation = CIkev1Negotiation::NewL( this,
                                                           iPFKeySocketIf,
                                                           iDebug,
                                                           sa,
                                                           RESPONDER ) );
        if ( err == KErrNone )
            {
            CleanupStack::PushL( negotiation );
            status = negotiation->ExecutePhase2L( aIkeMsg, aSrcAddr, aLocalPort ); 
            if ( status && !negotiation->Finished() )
               {
               //Negotiation OK
               CleanupStack::Pop();    //negotiation safe
               return;
               }
            CleanupStack::Pop();
            DeleteNegotiation(negotiation);
            }            
        return;
        }
    else
        {
        TCookie NULL_COOKIE;
        NULL_COOKIE.FillZ(ISAKMP_COOKIE_SIZE);
        if ( aIkeMsg.GetCookieR() == NULL_COOKIE )
            {
            //
            // This is the initial opening message from a remote host
            // Start a new negotiation
            //
            DEBUG_LOG( _L("Creating a NEW IKE Phase 1 Negotiation") );
            TRAPD( err, negotiation = CIkev1Negotiation::NewL( this,
                                                               iPFKeySocketIf,
                                                               iDebug,
                                                               aSrcAddr,
                                                               aIkeMsg.GetCookieI(),
                                                               EFalse ) );
            
            if ( err == KErrNone )
                {
                CleanupStack::PushL( negotiation );
                status = negotiation->ExecuteL( aIkeMsg, aSrcAddr, aLocalPort );                   
                if ( status && !negotiation->Finished() )
                    {
                    //Negotiation OK
                    CleanupStack::Pop();    //negotiation safe
                    return;
                    }
                CleanupStack::PopAndDestroy( negotiation );
                }                
            return;
            }
        }

    if ( !negotiation )
        {
        DEBUG_LOG( _L("Cannot find a matching negotiation") );
#ifdef _DEBUG
        const TPtrC8 ikeMsgPtr((TUint8 *)&aIkeMsg, (TUint16)aIkeMsg.GetLength());
        TInetAddr dstAddr;
        GetLocalAddress( dstAddr );
        dstAddr.SetPort( aLocalPort );
        TRACE_MSG_IKEV1( ikeMsgPtr, aSrcAddr, dstAddr );
#endif // _DEBUG            
        return;
        }
    }

// ---------------------------------------------------------------------------
// From class MIkev1ReceiverCallback
// Handles notification about receive error. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::ReceiveError( TInt aError )
    {
    HandleError( aError );
    }

// ---------------------------------------------------------------------------
// Requests sending of UDP data.
// ---------------------------------------------------------------------------
//    
void CIkev1PluginSession::DoSendUdpDataL( HBufC8* aUdpData,
                                          const TInetAddr& aDestAddr,
                                          TInt aLocalPort,
                                          TUint8 aDscp )
    {
    if ( !iSender->IsActive() &&
         iSendQueue.Count() == 0 )
        {
        // Sending is not in progress and send queue is empty.
        // Start sending UDP data.
        DoSendUdpData( aUdpData,
                       aDestAddr,
                       aLocalPort,
                       aDscp );
        }
    else
        {
        // Store buffer into send queue for later sending.
        TIkeSendQueueItem item = TIkeSendQueueItem( aUdpData,
                                                    aDestAddr,
                                                    aLocalPort,
                                                    aDscp );
        iSendQueue.Append( item );                
        }        
    }

// ---------------------------------------------------------------------------
// Sends UDP data. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoSendUdpData( HBufC8* aUdpData,
                                         const TInetAddr& aDestAddr,
                                         TInt aLocalPort,
                                         TUint8 aDscp )
    {
    __ASSERT_DEBUG( aUdpData != NULL,
                    User::Invariant() );
    
    iSender->SendUdpData( aUdpData, aDestAddr, aLocalPort, aDscp );
    }

// ---------------------------------------------------------------------------
// Handles PFKEY message.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoPfkeyMessageReceivedL( const TPfkeyMessage& aPfkeyMessage )
    {
    //
    //  Process received PFKEY message according to message type
    //
#ifdef _DEBUG    
    TBuf<40> txt_addr;
#endif        
            
    TIkev1SAData* ikev1SAdata = NULL;
    CIkev1Negotiation* negotiation = NULL;   
    
    switch ( aPfkeyMessage.iBase.iMsg->sadb_msg_type )
        {
        case SADB_ACQUIRE:
             //
             // A request to negotiate an IPSEC SA received
             // Try to find an existing IKE SA with remote address 
             //
#ifdef _DEBUG
            aPfkeyMessage.iDstAddr.Address().OutputWithScope( txt_addr );
#endif                
            ikev1SAdata = FindIkev1SADataWithAddr( aPfkeyMessage.iDstAddr.Address() );
            if ( ikev1SAdata )
                {
                //
                // An IKE SA found for Acquire. Get a negotiation
                // object for IKE Quick mode SA exchange 
                //
                negotiation = CIkev1Negotiation::NewL( this,
                                                       iPFKeySocketIf,
                                                       iDebug,
                                                       ikev1SAdata,
                                                       INITIATOR,
                                                       &aPfkeyMessage );                     
                CleanupStack::PushL( negotiation );                   
                negotiation->InitPhase2L();    //Because is initiator
                DEBUG_LOG1( _L("IKEv1 SA found for Acquire IP: %S"), &txt_addr );
                CleanupStack::Pop();                                                
                }
            else
                {
                //
                // No IKE SA found for Acquire.
                //
                // If rekeying is in progress, IKE Quick mode SA exchange
                // is started after Phase I has completed.
                //
                CIkev1Negotiation* negotiation = iFirstNegotiation;
                while ( negotiation != NULL )
                    {
                    if ( negotiation->IsRekeyingIkeSa() )
                        {                        
                        break;
                        }
                    negotiation = negotiation->iNext;
                    }                
                if ( negotiation != NULL )
                    {
                    negotiation->PreparePhase2L( aPfkeyMessage );                    
                    DEBUG_LOG1( _L("Negotiation found for Acquire IP: %S"), &txt_addr );
                    break;
                    }                    
                
                //
                // Otherwise we shall start a new IKE SA negotiation to
                // defined destination address.
                //
                negotiation = CIkev1Negotiation::NewL( this,
                                                       iPFKeySocketIf,
                                                       iDebug,
                                                       (TInetAddr&)aPfkeyMessage.iDstAddr.Address(),
                                                       aPfkeyMessage );
                CleanupStack::PushL( negotiation );
                negotiation->InitNegotiationL();
                if ( negotiation->Finished() )
                    {
                    CleanupStack::PopAndDestroy();
                    }
                else 
                    {
                    CleanupStack::Pop();                                              
                    }
                DEBUG_LOG1( _L("Negotiate a new IKE SA for Acquire IP: %S"), &txt_addr );
                }
            break;

        case SADB_EXPIRE:
            //
            // An IPSEC SA has been expired.
            // Try to find an existing IKE SA with source address
            // (= inbound SA destination address)
            //
#ifdef _DEBUG                
            aPfkeyMessage.iDstAddr.Address().OutputWithScope( txt_addr );
#endif                  
            ikev1SAdata = FindIkev1SAData( aPfkeyMessage.iSrcAddr.Address(),
                                           aPfkeyMessage.iSa.iExt->sadb_sa_spi );
            if ( ikev1SAdata )
                {
                //
                // An IKE SA found for Expire. Get a negotiation
                // object for IKE Informational exchange 
                //
                if ( DeleteIpsecSpi(ikev1SAdata->iSAId,
                                    aPfkeyMessage.iSa.iExt->sadb_sa_spi,
                                    ETrue) )
                    {
                    negotiation = CIkev1Negotiation::NewL( this,
                                                           iPFKeySocketIf,
                                                           iDebug,
                                                           ikev1SAdata,
                                                           RESPONDER );
                    CleanupStack::PushL( negotiation );
                    negotiation->SendDeleteL( aPfkeyMessage.iBase.iMsg->sadb_msg_satype,
                                              aPfkeyMessage.iSa.iExt->sadb_sa_spi );
                    CleanupStack::PopAndDestroy();
                    DEBUG_LOG3(_L("Notifying SGW, IPsec SA Expiration (addr = %S, spi = %x , proto = %d)"), &txt_addr,
                            ByteOrder::Swap32(aPfkeyMessage.iSa.iExt->sadb_sa_spi), aPfkeyMessage.iDstAddr.iExt->sadb_address_proto);
                    }       
                }
            else
                {                       
                DEBUG_LOG1( _L("No IKE SA found Expire IP: %S"), &txt_addr );
                }                       
            break;
             
        default:
            break;
            
        }
    }

// ---------------------------------------------------------------------------
// Deletes IKE SAs. 
// ---------------------------------------------------------------------------
//
TBool CIkev1PluginSession::DeleteSAsWithHost( TBool aSilentClose )
    {        
    DEBUG_LOG( _L("Deactivating IKEv1 SA:s") );
    
    //
    // For sure check if there is any ongoing negotiations for this
    // and delete these negotiations immediatelly.
    //
    while ( iFirstNegotiation )
        {
        CIkev1Negotiation* negotiation = iFirstNegotiation;
        iFirstNegotiation = negotiation->iNext;             
        delete negotiation;
        }
    
    TInt deactivatingStarted( EFalse );
    for ( TInt i=0;i<iIkev1SAs.Count();i++ )
        {
        CIkev1SA* sa = iIkev1SAs[i]; 
        if ( !sa->IsExpired() )
            {
            TIkev1SAData& ikeSaData = sa->iHdr;
            deactivatingStarted = DeleteIkeSA( &ikeSaData, aSilentClose );
            }   
        }

    return deactivatingStarted;
    }    

// ---------------------------------------------------------------------------
// Handles starting of negotiation with a peer. 
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoNegotiateWithHostL( const CIkeData& aIkeData )
    {
    __ASSERT_DEBUG( iIkeData == NULL,
                    User::Invariant() );    
    
    iIkeData = CIkeData::NewL( &aIkeData ); 

    // Start ISAKMP Phase 1 negotiation to the specified gateway immediately
    // if the Internal VPN address feature is enabled in policy (= IA payload
    // or CONFIG-MODE). Otherwise postpone negotiation.    
    if ( !aIkeData.iUseInternalAddr &&
         !aIkeData.iUseCfgMode )
        {
        DEBUG_LOG(_L("Negotiation postponed."));
        User::RequestComplete( iClientStatusNegotiate, KErrNone );
        iClientStatusNegotiate = NULL;
        return;
        }   
    
    CIkev1Negotiation* negotiation = CIkev1Negotiation::NewL( this,
                                                              iPFKeySocketIf,
                                                              iDebug,
                                                              iIkeData->iAddr,
                                                              ETrue );
    CleanupStack::PushL( negotiation );
    negotiation->InitNegotiationL();        
    if ( negotiation->Finished() )
        {
        CleanupStack::PopAndDestroy( negotiation );
        User::Leave( KKmdIkeNegotFailed );            
        }
    else
        {           
        CleanupStack::Pop( negotiation );
        }   
    }  

// ---------------------------------------------------------------------------
// Handles fatal error.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoHandleError( TInt aError )
    {
    DEBUG_LOG1(_L("CIkev1PluginSession::DoHandleError, err=%d"), aError);
    
    while ( iFirstNegotiation )
        {
        CIkev1Negotiation* negotiation = iFirstNegotiation;
        iFirstNegotiation = negotiation->iNext;             
        delete negotiation;
        }
    
    while ( iIkev1SAs.Count() )
        {
        CIkev1SA* ikev1SA = iIkev1SAs[0];
        iIkev1SAs.Remove(0);
        delete ikev1SA;
        }
    
    // Complete client's requests.
    DoCompleteNegotiateWithHost( aError );
    DoCompleteDeleteSession( aError );
    DoCompleteNotifyError( aError );
    }

// ---------------------------------------------------------------------------
// Handles completion of client's negotiate request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoCompleteNegotiateWithHost( TInt aStatus )
    {         
    if ( iClientStatusNegotiate != NULL )
        {
        if ( aStatus == KErrNone )
            {
            if ( iInternalAddress != NULL )
                {
                __ASSERT_DEBUG( iIkeData != NULL,
                                User::Invariant() );
                // Build internal address for client.       
                VPNAddrInfo::BuildVPNAddrInfo( iInternalAddress,
                                               iIkeData->iDnsServer,
                                               *iClientIaNegotiate,
                                               iDebug );
                }
            }

        // Complete client's request.
        DEBUG_LOG1(_L("CIkev1PluginSession::DoCompleteNegotiateWithHost, aStatus=%d"),
                aStatus);        
        User::RequestComplete( iClientStatusNegotiate, aStatus );
        iClientStatusNegotiate = NULL;
        iClientIaNegotiate = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Handles completion of client's delete session request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoCompleteDeleteSession( TInt aStatus )
    {       
    if ( iClientStatusDelete != NULL )
        {
        DoCancelDataTransfer();
        
        DEBUG_LOG1(_L("CIkev1PluginSession::DoCompleteDeleteSession, aStatus=%d"),
                aStatus);
        User::RequestComplete( iClientStatusDelete, aStatus );
        iClientStatusDelete = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// Handles completion of client's notify error request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoCompleteNotifyError( TInt aStatus )
    {   
    if ( iClientStatusNotifyError != NULL )
        {
        DoCancelDataTransfer();

        DEBUG_LOG1(_L("CIkev1PluginSession::DoCompleteNotifyError, aStatus=%d"),
                aStatus);
        User::RequestComplete( iClientStatusNotifyError, aStatus );
        iClientStatusNotifyError = NULL;
        }        
    }

// ---------------------------------------------------------------------------
// Handles completion of client's notify internal address change request.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoCompleteInternalAddressChanged( TInt aStatus )
    {
    if ( iClientStatusNotifyIaChange != NULL )
        {       
        DEBUG_LOG1(_L("CIkev1PluginSession::DoCompleteInternalAddressChange, aStatus=%d"),
                aStatus);

        if ( aStatus == KErrNone )
            {
            if ( iInternalAddress != NULL )
                {
                // Build internal address for client.       
                VPNAddrInfo::BuildVPNAddrInfo( iInternalAddress,
                                               iIkeData->iDnsServer,
                                               *iClientIaNotify,
                                               iDebug );            
                }
            }
        
        if ( aStatus != KErrNone &&
             aStatus != KErrCancel )
            {
            HandleError( aStatus);
            return;
            }
        
        User::RequestComplete( iClientStatusNotifyIaChange, aStatus );
        iClientStatusNotifyIaChange = NULL;
        iClientIaNotify = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Cancels data transfer.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoCancelDataTransfer()
    {
    iReceiver->CancelReceive();
    iDataInterface.StopReceive();
    DoEmptySendQueue();
    iSender->Cancel();
    }

// ---------------------------------------------------------------------------
// Empties send queue.
// ---------------------------------------------------------------------------
//
void CIkev1PluginSession::DoEmptySendQueue()
    {
    while ( iSendQueue.Count() )
        {
        TIkeSendQueueItem item = iSendQueue[0];
        HBufC8* udpData = item.UdpData();
        iSendQueue.Remove(0);
        delete udpData;
        udpData = NULL;
        }    
    }

