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
* Description:  VPN PFKEY extension data utility
*
*/


#include <in_sock.h>
#include <networking/pfkeyv2.h>
#include <networking/pfkeyext.h>

#include "ikemsgheader.h"
#include "pfkeyextdatautil.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// This static method builds a generic PFKEY API extension data for IPSEC.
// This data contains all parameters needed by the IPSEC to do UDP
// encapsulation/decpsulation for ESP packet. Generic extension data format is
// LID format begining with four bytes extension header. Extension header
// consists two bytes extension length and two bytes  extension ID. LID format
// consists from one byte length, one byte ID and parameter data. 
// Buffer format: HL,HID,LID,LID,...LID
// Generic extension data buffer handling macros are defined in pfkeyext.h
// (common macros with IPSEC)
// ---------------------------------------------------------------------------
//
EXPORT_C void PFKeyExtDataUtil::BuildUdpEncExtensionData( TDes8& aExtData,
                                                 TUint32 aNAT_D_Flags,
                                                 TBool aNATDetected,
                                                 TBool aNokiaNATProbeUsed,
                                                 TUint16 aUdpEncapsPort,
                                                 TUint16 aKeepAliveTimeout,
                                                 const TInetAddr& aDestinAddr,
                                                 const TInetAddr& aOriginalAddr )
    {
    if ( aNAT_D_Flags )
        {
        aUdpEncapsPort = FLOATED_IKE_PORT; // for IETF specified ESP UDP encapsulation
        if ( ( aNAT_D_Flags & LOCAL_END_NAT ) == 0 )
            {
            aKeepAliveTimeout = 0; // Local end is not behind NAT, no keepalive needed
            }
        }
    else
        {
        //
        // Nokia specific NAT traversal info (=ESP UDP tunneling)  
        // If aNATDetected is true connection is over NAT:ted
        // network (=local end behind NAT). UDP encapsulation shall
        // then be done using configured port iEspUdpPort. If that
        // value is undefined default port 9872 shall be used then.
        // If aNATDetected is false and aNokiaNATProbeUsed is true
        // the NAT probe procedure has confirmed that there is no
        // NAT device between. ESP UDP encapsulation port is zeroed
        // then to avoid unnecessary ESP UDP encapsulation.
        // If aNokiaNATProbeUsed is false ESP UDP encapsulation is done
        // without probing, if any aUdpEncapsPort is defined 
        //
        if ( !aNATDetected && aNokiaNATProbeUsed )
            {
            aUdpEncapsPort = 0;
            }
        }   

    if ( aUdpEncapsPort == 0 )
        {
        aExtData.SetLength(0); // No extension data needed
        return;
        }

    TPfkeyGenExtension NatExtension( aExtData, ESP_UDP_ENCAPSULATION_EXT );
    
    NatExtension.StoreParameter( UDP_ENCAPSULATION_PORT,
                                 2,
                                 (TUint8*)&aUdpEncapsPort );
    
    if ( aKeepAliveTimeout )
        {
        NatExtension.StoreParameter( NAT_KEEPALIVE_TIMEOUT,
                                     2,
                                     (TUint8*)&aKeepAliveTimeout );
        }
    
    if ( aNAT_D_Flags & REMOTE_END_NAT )
        {
        NatExtension.StoreParameter( DESTINATION_ADDRESS,
                                     sizeof(TInetAddr),
                                     (TUint8*)&aDestinAddr );
        }
    
    if ( aOriginalAddr.Family() != KAFUnspec )
        {
        NatExtension.StoreParameter( PEER_ORIGINAL_ADDRESS,
                                     sizeof(TInetAddr),
                                     (TUint8*)&aOriginalAddr );
        }       
    }   
