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
* Description:  Security policy module payload structures
*
*/


#include <ext_hdr.h>
#include <tcp_hdr.h>
#include <udp_hdr.h>
#include <icmp6_hdr.h>

// CLASS HEADER
#include "secpolpayload.h"

// ======== LOCAL FUNCTIONS ========

#ifdef _DEBUG
static TInt IsShort( TDes &aStr, const TDesC &aName, TInt aMin, TInt aLength )
    {
    aStr.Append( aName );
    if ( aMin <= aLength )
        {
        return 0;
        }
    aStr.Append(_L(".. truncated"));
    return 1;
    }
#endif  

// ======== MEMBER FUNCTIONS ========

//
// IPv4 HDR Dump routine
//
void THdrIP4::Dump( TDes &aStr, TInt /*aLength*/ )
    {
#ifndef _DEBUG
    (void)aStr;
#endif

#ifdef _DEBUG
    const TInt KMaxBufLength( 40 );
    TBuf<KMaxBufLength> buf;
    TInetAddr addr;

    addr.SetAddress( SrcAddr() );
    addr.OutputWithScope( buf );
    aStr.Append( _L(" src=") );
    aStr.Append( buf );
    addr.SetAddress( DstAddr() );
    addr.OutputWithScope( buf );
    aStr.Append( _L(" dst=") );
    aStr.Append( buf );
#endif
    }

TBool THdrIP4::IsUnicast()
    {
    TInetAddr addr;
    TUint32 ip4Addr = DstAddr();    
    addr.SetAddress( ip4Addr );
    TBool status = addr.IsUnicast();
    if ( status != KErrNone )
        {
        //
        // If at least 7 leftmost bits in address are set to 1 this
        // address is interpreted to be IPv4 subnet broadcast
        //
        status = ( ( ip4Addr & 0x7f ) != 0x7f );
        }   
    return status;
    }   

//
// IPv6 HDR Dump routine
//
void THdrIP6::Dump( TDes &aStr, TInt /*aLength*/ )
    {
#ifndef _DEBUG
    (void)aStr;
#endif

#ifdef _DEBUG
    const TInt KMaxBufLength( 40 );
    TBuf<KMaxBufLength> buf;
    TInetAddr addr;

    addr.SetAddress( SrcAddr() );
    addr.OutputWithScope( buf );
    aStr.Append( _L(" src=") );
    aStr.Append( buf );
    addr.SetAddress( DstAddr() );
    addr.OutputWithScope( buf );
    aStr.Append( _L(" dst=") );
    aStr.Append( buf );
#endif
    }

TBool THdrIP6::IsUnicast()
    {
    TInetAddr addr;
    addr.SetAddress( DstAddr() );
    return addr.IsUnicast();
    }

TSecpolPayload::TSecpolPayload( const TUint8* aPtr )
 : iRaw( aPtr )
     {     
     }

void TSecpolPayload::Dump( TDes &aStr, TInt aLength, TInt aProtocol )
    {
#ifndef _DEBUG
    (void)aStr;
    (void)aLength;
    (void)aProtocol;
#endif

#ifdef _DEBUG
    for (;;)
        {
        TInt skip;
        switch ( aProtocol )
            {
            case 50:    // ESP
                if ( IsShort( aStr,
                              _L(" ESP"),
                              iESP->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aStr.AppendFormat( _L("(spi=%x,seq=%d)"),
                                    ByteOrder::Swap32( iESP->SPI() ),
                                    iESP->Sequence() );
                return; // Cannot go past ESP
            case 51:
                if ( IsShort( aStr,
                              _L(" AH"),
                              iAH->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aStr.AppendFormat(_L("(spi=%x,seq=%d,protcol=%d)"),
                        iAH->SPI(), iAH->Sequence(), iAH->NextHeader() );
                skip = iAH->HeaderLength();
                aProtocol = iAH->NextHeader();
                break;  // AH
            case 4:
                if ( IsShort( aStr,
                              _L(" IP4-in-IP"),
                              iIP4->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                ((THdrIP4 *)iIP4)->Dump(aStr, aLength);
                skip = iIP4->HeaderLength();
                aProtocol = iIP4->Protocol();
                break;
            case 1: // Fall through
            case 58:
                if ( IsShort( aStr,
                              _L(" ICMP"),
                              iICMP->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aStr.AppendFormat(_L("(type=%d,code=%d)"),
                        iICMP->Type(), iICMP->Code() );
                return;
            case 6:
                if ( IsShort( aStr,
                              _L(" TCP"),
                              iTCP->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aStr.AppendFormat(_L("(src=%d,dst=%d)"),
                    iTCP->SrcPort(), iTCP->DstPort());
                return;
            case 17:
                if ( IsShort( aStr,
                              _L(" UDP"),
                              iUDP->MinHeaderLength(),
                              aLength ) )
                    return;
                aStr.AppendFormat(_L("(src=%d,dst=%d)"),
                    iUDP->SrcPort(), iUDP->DstPort());
                return;
            case KProtocolInet6Ipip:
                if ( IsShort( aStr,
                              _L(" IP6-in-IP"),
                              iIP6->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                ((THdrIP6 *)iIP6)->Dump( aStr, aLength );
                skip = iIP6->HeaderLength();
                aProtocol = iIP6->NextHeader();
                break;
            case KProtocolInet6HopOptions:
                if ( IsShort( aStr,
                              _L(" HBH"),
                              iOPT->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                skip = iOPT->HeaderLength();
                aProtocol = iOPT->NextHeader();
                break;
            case KProtocolInet6RoutingHeader:
                if ( IsShort( aStr,
                              _L(" RTH"),
                              iRTH->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aProtocol = iRTH->NextHeader();
                skip = iRTH->HeaderLength();
                break;
            case KProtocolInet6Fragment:
                if ( IsShort( aStr,
                              _L(" FRAG"),
                              iFRH->MinHeaderLength(),
                              aLength ) )
                    {
                    return;
                    }
                aStr.AppendFormat(_L("(prot=%d,id=%d,offset=%d)"),
                    iFRH->NextHeader(), iFRH->Id(), iFRH->FragmentOffset());
                if ( iFRH->MFlag() )
                    {
                    aStr.Append(_L(" More"));
                    }
                return;
            case KProtocolInet6NoNextHeader:
                aStr.AppendFormat(_L(" NNH"));
                return;
            case KProtocolInet6DestinationOptions:
                if ( IsShort( aStr,
                              _L(" DOP"),
                              iOPT->MinHeaderLength(),
                              aLength ) )
                    return;
                skip = iOPT->HeaderLength();
                aProtocol = iOPT->NextHeader();
                break;
            default:
                aStr.AppendFormat(_L(" protocol=%d"), aProtocol);
                return;
            }
        aLength -= skip;
        iRaw += skip;
        }
#endif      
    }
