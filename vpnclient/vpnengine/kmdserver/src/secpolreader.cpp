/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Security policy module
*
*/


#include <es_sock.h>
#include <in_sock.h>
#include <networking/ipsecerr.h>
#include <vpnlogmessages.rsg>
#include "ikedebug.h"
#include "kmdeventloggerif.h"
#include "secpolpayload.h"

// CLASS HEADER
#include "secpolreader.h"

// ======== LOCAL FUNCTIONS ========

#ifdef _DEBUG

// ---------------------------------------------------------------------------
// Returns error description.
// ---------------------------------------------------------------------------
//
static const TPtrC IpsecError( TInt aReason )
    {
    switch ( aReason )
        {
        case EIpsec_RMBUF:              return _L("RMBUF operation failed unexcpectedly");
        //
        // AH and ESP
        //
        case EIpsec_CorruptPacketIn:    return _L("Truncated or corrupt packet or header");
        case EIpsec_CorruptPacketOut:   return _L("Corrupt packet after IPSEC operations");
        case EIpsec_EspInboundSA:       return _L("The inbound SA for ESP does not exist");
        case EIpsec_EspAuthentication:  return _L("Authentication check failed in ESP");
        case EIpsec_EspAuthAlg:         return _L("Required auth algorithm for ESP not available/installed");
        case EIpsec_EspEncrAlg:         return _L("Required encrypt algorithm for ESP not available/installed");
        case EIpsec_AhAuthAlg:          return _L("Required auth algorithm for AH not available/installed");    
        case EIpsec_AhInboundSA:        return _L("The inbound SA for AH does not exist");
        case EIpsec_AhIcvLength:        return _L("ICV length in packet does not match algorithm");
        case EIpsec_AhAuthentication:   return _L("Authentication check failed in AH");
        case EIpsec_PacketLength:       return _L("Invalid/corrupt length of the packet");
        case EIpsec_DataAlignment:      return _L("Data not aligned by block size (ESP)");
        case EIpsec_EspPadByte:         return _L("The ESP pad byte content is invalid");
        case EIpsec_EspPadLength:       return _L("The ESP pad length is corrupt (probably wrong key)");
        case EIpsec_ReplayDuplicate:    return _L("Duplicate packet (replay window test)");
        //
        // SECPOL
        //
        case EIpsec_OutboundNotFound:   return _L("Outbound SA does not exist, ACQUIRE started");
        case EIpsec_OutboundPending:    return _L("Outbooud SA does not exits, ACQUIRE pending");
        case EIpsec_NoSelectorMatch:    return _L("None of the policy selectors matched");
        case EIpsec_MaxTransforms:      return _L("Incoming packet exceed configured max limit of transforms");
        case EIpsec_TooFewTransforms:   return _L("Policy requires IPSEC, none or too little was present");
        case EIpsec_TunnelMismatch:     return _L("Tunnelmode does not match the policy");
        case EIpsec_MismatchedSA:       return _L("Applied SA does not match the policy");
        case EIpsec_UnrequiredSA:       return _L("Applied SA where policy has none");
        case EIpsec_TooManyTransforms:  return _L("Incoming packet had more transforms than policy requires");
        case EIpsec_NoBundle:           return _L("Incoming packet had transforms, but policy doesn't require any");
        //
        // IPv6 additions
        //
        case EIpsec_AhRMBufSplit:       return _L("Inbound AH processing failed (Memory?)");
        case EIpsec_AhPacketTooLong:    return _L("Outbound packet would exeed 2**16-1 with AH");
        case EIpsec_AhSequenceWrap:     return _L("Outbound sequence # wrapped around for this SA");
        case EIpsec_EspSequenceWrap:    return _L("Outbound sequence # wrapped around for this SA");
        case EIpsec_EspBadCipherBlockSize:  return _L("Configuration error, cipher block size must be < 256");
        case EIpsec_AcquireFailed:      return _L("Acquiring SA failed (no SA available or negotiated)");
        //
        // Detail reasons for SA not matching the SA spec in the policy
        // (replace one EIpsec_MismatchedSA with multiple detail errors)
        //
        case EIpsec_MismatchedDestination:  return _L("SA destination does not match (internal error?)");
        case EIpsec_MismatchedType:     return _L("SA Type (AH/ESP) does not match");
        case EIpsec_MismatchedPFS:      return _L("PFS bit is not same");
        case EIpsec_MismatchedAuthAlg:  return _L("Auth algorithm doesn't match");
        case EIpsec_MismatchedEncryptAlg:   return _L("Encrypt algorithm doesn't match");
        case EIpsec_MismatchReplayWindow:   return _L("ReplayWindow length is shorter than required");
        case EIpsec_MismatchSource:     return _L("source address does not match");
        case EIpsec_MismatchProxy:      return _L("proxy address does not match");
        case EIpsec_MismatchSourcePort: return _L("source port does not match");  
        case EIpsec_MismatchDestinationPort:return _L("destination port does not match");
        case EIpsec_MismatchProtocol:   return _L("protocol does not match");
        case EIpsec_MismatchSourceIdentity: return _L("source identity does not match");
        case EIpsec_MismatchDestinationIdentity: return _L("destination identity does not match");
    
        case EIpsec_BadCipherKey:       return _L("Key in SA is too short (for the algorithm) or is weak");
        case EIpsec_UnknownCipherNumber: return _L("Attempting to use algorithm number that is not known");
        case EIpsec_UnknownDigestNumber: return _L("Attempting to use algorithm number that is not known");
        case EIpsec_UnavailableCipher: return _L("No installed library implements the cipher");
        case EIpsec_UnavailableDigest: return _L("No installed library implements the digest");
        //
        // Temporary place for new errors
        //
        case EIpsec_IcmpError:  return _L("An ICMP error report containing AH or ESP (for INET6)");
        case EIpsec_LostSA:     return _L("An SA has been lost between Apply and Verify, expired? (for SECPOL)");
        case EIpsec_NoInnerSource: return _L("Cannot find inner-src for outbound packet when tunneling (for SECPOL)");
        //
        // Special code for NAT Traversal  
        //
        case EIpsec_NotANATTPacket: return _L("UDP packet is NOT a NAT Taversal packet");
        case EIpsec_FragmentMismatch: return _L("IPSEC on fragment is not same as before, packet dropped");
    
        default:
            return _L("Unknown reason");
        }
    }

#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CSecpolReader* CSecpolReader::NewL( MKmdEventLoggerIf& aEventLogger,
                                    MIkeDebug& aDebug )
    {
    CSecpolReader* self = new (ELeave) CSecpolReader( aEventLogger,
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
CSecpolReader::~CSecpolReader()
    {
    DEBUG_LOG(_L("CSecpolReader::~CSecpolReader"));
    Cancel();
    iSocket.Close();
    iSocketServer.Close();        
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CSecpolReader::CSecpolReader( MKmdEventLoggerIf& aEventLogger,
                              MIkeDebug& aDebug )
: CActive( EPriorityStandard ),
  iEventLogger( aEventLogger ),
  iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSecpolReader::ConstructL()
    {
    TInetAddr addr;
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iSocket.Open( iSocketServer, _L("secpol6") ) );
    addr.SetAddress( KInetAddrNone );
    addr.SetPort( 0 );
    User::LeaveIfError( iSocket.Bind( addr ) );
    iSocket.RecvFrom( iMsg, iAddr, 0, iStatus );
    SetActive();
    DEBUG_LOG(_L("CSecpolReader::ConstructL - constructed"));
    }

// ---------------------------------------------------------------------------
// Returns event logger.
// ---------------------------------------------------------------------------
//
MKmdEventLoggerIf& CSecpolReader::EventLogger()
    {
    return iEventLogger;
    }


// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous reading. 
// ---------------------------------------------------------------------------
//
void CSecpolReader::RunL()
    {        
#ifdef _DEBUG
    TBuf<40> buf;
    TBuf<1000> str;
	TInt protocol = -1;
#endif // _DEBUG    
    TInt  len = 0;
	TBool processEvent;
    TSecpolPayload packet( iMsg.Ptr() );
    
    DEBUG_LOG1(_L("Secpol read, status=%d"), iStatus.Int());
    __ASSERT_DEBUG( iStatus.Int() == KErrNone,
                    User::Invariant() );
    if ( iStatus.Int() == KErrNone )
        {
        switch ( packet.iIP4->Version() )
            {
            case 4:
    			processEvent = ((THdrIP4 *)packet.iRaw)->IsUnicast();
    			if ( processEvent )
    			    {	
#ifdef _DEBUG			
    			    ((THdrIP4 *)packet.iRaw)->Dump( str, iMsg.Length() );
    			    protocol = packet.iIP4->Protocol();
#endif // _DEBUG			   
    			    len = packet.iIP4->HeaderLength();
    			    }    
                    break;
            case 6:
    			processEvent = ((THdrIP6 *)packet.iRaw)->IsUnicast();
    			if ( processEvent )
    			    {	
#ifdef _DEBUG			
    			    ((THdrIP6 *)packet.iRaw)->Dump( str, iMsg.Length() );
    			    protocol = packet.iIP6->NextHeader();
#endif // _DEBUG			   
    			    len = packet.iIP6->HeaderLength();
    			    }    
                    break;
            default:
    			    processEvent = ETrue;
#ifdef _DEBUG			
    			    str.Format( _L("Unknown IP protocol version %d"),
    			                (TInt)packet.iIP4->Version() );
#endif // _DEBUG            
    			    break;
            }
    	
    	if ( processEvent )
    	    {		 
    	    if ( len )
               {
               packet.iRaw += len;
#ifdef _DEBUG          
               packet.Dump( str, iMsg.Length()-len, protocol );
#endif // _DEBUG          
               }
#ifdef _DEBUG       
    	    str.Append( _L(" from ") );
    	    (TInetAddr::Cast(iAddr)).OutputWithScope( buf );
    	    str.Append( buf );
    	    str.Append( _L(" because ") );
    	    str.Append( IpsecError( iAddr.Port() ) );
    	    str.AppendFormat( _L(" (%d)"), iAddr.Port() );
    	    DEBUG_LOG(str);
#endif // _DEBUG       
    
    	    LOG_KMD_EVENT( MKmdEventLoggerIf::KLogWarning,
    	                   R_VPN_MSG_DATA_DROPPED_DUE_POLICY,
    	                   (TInt)iAddr.Port(),
    	                   0,
    	                   &iAddr );    	        	    
    	    }
        }
        
    iSocket.RecvFrom( iMsg, iAddr, 0, iStatus );  // start a new read
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous reading. 
// ---------------------------------------------------------------------------
//
void CSecpolReader::DoCancel()
    {
    iSocket.CancelRecv();
    }
