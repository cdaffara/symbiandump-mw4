/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  KMD event logger
*
*/


#include <e32base.h>
#include <vpnlogmessages.rsg>
#include "eventviewer.h"
#include "ikedebug.h"
#include "vpnclientuids.h"

// CLASS HEADER
#include "kmdeventlogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CKmdEventLogger* CKmdEventLogger::NewL( MIkeDebug& aDebug )
    {
    CKmdEventLogger* self = new ( ELeave ) CKmdEventLogger( aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CKmdEventLogger::~CKmdEventLogger()
    {
    iEventMediator.Close();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CKmdEventLogger::CKmdEventLogger( MIkeDebug& aDebug )
 : iDebug( aDebug )
    {
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CKmdEventLogger::ConstructL()
    {
    User::LeaveIfError( iEventMediator.Connect() );
    }

// ---------------------------------------------------------------------------
// From class MKmdEventLoggerIf
// Writes event to event log.
// ---------------------------------------------------------------------------
//
void CKmdEventLogger::LogEvent( TLogCategory aCategory,
                                TInt aMsgId,
                                TInt aStatus,
                                TUint32 aVpnIapId,
                                const TInetAddr* aGwIp,
                                const TInetAddr* aLocalAddr )
    {
    TUid uid = TUid::Uid( KUidKmdServer );    
    
    TBuf<40> ipAddr16;  
    TBuf8<40> ipAddr;
    TBuf8<40> localIpAddr;  
    TIapName vpnAccessPointName;
    TInt addrIndex = KErrNotFound;
    TInt iapIndex = KErrNotFound;
    TInt statusIndex = KErrNotFound;
    TInt realIpIndex = KErrNotFound;      
    TInt desCount = 0;
    TUint8* pointers[4];
    TInt lengths[4];
    
    switch ( aMsgId )
        {
        case R_VPN_MSG_VPN_GW_NO_RESP:
        case R_VPN_MSG_VPN_GW_AUTH_FAIL:
        case R_VPN_MSG_VPN_GW_AUTH_OK:
            addrIndex = 0;
            iapIndex  = 1;                  
            break;

        case R_VPN_MSG_VPN_GW_ERR_RESP_RECEIVED:
        case R_VPN_MSG_SENT_ERROR_RESPONSE:         
            addrIndex   = 0;
            iapIndex    = 1;
            statusIndex = 2;
            break;

        case R_VPN_MSG_DATA_DROPPED_DUE_POLICY:
            addrIndex   = 0;            
            statusIndex = 1;
            break;

        case R_VPN_MSG_ADDR_INFO_FOR_VPN_AP:
            iapIndex    = 0;            
            addrIndex   = 1;
            realIpIndex = 2;
            statusIndex = 3;
            break;
            
        case R_VPN_MSG_REAL_IAP_ACT_FAILED: 
            realIpIndex = 0;        
            iapIndex    = 1;
            statusIndex = 2;
            break;

        default:
            break;
        }   
    
    if ( addrIndex != KErrNotFound )
        {
        desCount ++;
        pointers[addrIndex] = (TUint8*)ipAddr.Ptr();        
        if ( aGwIp )
            {   
            aGwIp->Output(ipAddr16);
            ipAddr.Copy(ipAddr16);
            lengths[addrIndex] = ipAddr.Length();       
            }
        else
            {
            lengths[addrIndex] = 0;
            }
        }
    
    if ( statusIndex != KErrNotFound )
        {
        desCount ++;
        pointers[statusIndex] = (TUint8*)&aStatus;
        lengths[statusIndex]  = sizeof(aStatus);
        }
    
    if ( iapIndex != KErrNotFound )
        {
        //
        // VPN IAP id shall be converted to VPN access point name using
        // eventviewer API.
        //
        desCount ++;
        pointers[iapIndex] = (TUint8*)vpnAccessPointName.Ptr();     
        if ( aVpnIapId != 0 )
            {   
            EventViewer::GetIapName(aVpnIapId, vpnAccessPointName);
            lengths[iapIndex] = vpnAccessPointName.Length();
            }
        else
            {
            lengths[iapIndex] = 0;
            }
        }

    if ( realIpIndex != KErrNotFound )
        {
        desCount ++;
        pointers[realIpIndex] = (TUint8*)localIpAddr.Ptr();
        ipAddr16.SetLength(0);        
        if ( aLocalAddr != NULL )
            {
            aLocalAddr->Output(ipAddr16);
            }
        localIpAddr.Copy(ipAddr16);
        lengths[realIpIndex] = localIpAddr.Length();
        }
   
    TInt ret( KErrNone );
    
    switch ( desCount )
        {
        case 0:
            {
            ret = iEventMediator.ReportLogEvent( uid,
                                                 aCategory,
                                                 aMsgId,
                                                 0 );
            }
            break;             
        
        case 1:
            {
            TPtr8 parm1(pointers[0], lengths[0], lengths[0]);
            ret = iEventMediator.ReportLogEvent( uid,
                                                 aCategory,
                                                 aMsgId,
                                                 1,
                                                 &parm1 );
            }
            break;
            
        case 2:
            {
            TPtr8 parm1(pointers[0], lengths[0], lengths[0]);
            TPtr8 parm2(pointers[1], lengths[1], lengths[1]);
            ret = iEventMediator.ReportLogEvent( uid,
                                                 aCategory,
                                                 aMsgId,
                                                 2,
                                                 &parm1,
                                                 &parm2 );
            }
            break;

        case 3:
            {
            TPtr8 parm1(pointers[0], lengths[0], lengths[0]);
            TPtr8 parm2(pointers[1], lengths[1], lengths[1]);
            TPtr8 parm3(pointers[2], lengths[2], lengths[2]);        
            ret = iEventMediator.ReportLogEvent( uid,
                                                 aCategory,
                                                 aMsgId,
                                                 3,
                                                 &parm1,
                                                 &parm2,
                                                 &parm3 );
            }
            break;

        case 4:
            {
            TPtr8 parm1(pointers[0], lengths[0], lengths[0]);
            TPtr8 parm2(pointers[1], lengths[1], lengths[1]);
            TPtr8 parm3(pointers[2], lengths[2], lengths[2]);
            TPtr8 parm4(pointers[3], lengths[3], lengths[3]);                   
            ret = iEventMediator.ReportLogEvent( uid,
                                                 aCategory,
                                                 aMsgId,
                                                 4,
                                                 &parm1,
                                                 &parm2,
                                                 &parm3,
                                                 &parm4 );
            }
            break;

        default:
            ret = ret;
            break;
            
        }

    DEBUG_LOG3( _L("Logging event %x with %d parameters, report status = %d"),
            aMsgId, desCount, ret );
    
    }

// ---------------------------------------------------------------------------
// From class MKmdEventLoggerIf
// Writes event to event log.
// ---------------------------------------------------------------------------
//
void CKmdEventLogger::LogEvent( TKmdLogCategory aCategory,
                                TInt aMsgId,
                                TInt aStatus,
                                TUint32 aVpnIapId,
                                const TInetAddr* aGwIp,
                                const TInetAddr* aLocalAddr )
    {
    TLogCategory logCategory;
    switch ( aCategory )
        {
        case KLogInfo:
            logCategory = EInfo;
            break;
        case KLogWarning:
            logCategory = EWarning;
            break;
        case KLogError:
            logCategory = EError;
            break;
        default:
            logCategory = EDebug;
            break;
        }

    LogEvent( logCategory,
              aMsgId,
              aStatus,
              aVpnIapId,
              aGwIp,
              aLocalAddr );
    }

