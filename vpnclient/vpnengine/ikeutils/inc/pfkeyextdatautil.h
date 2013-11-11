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


#ifndef PFKEYEXTDATAUTIL_H
#define PFKEYEXTDATAUTIL_H

#include <e32base.h>

/**
 *  PFKEY extension data utility class
 *
 *  PFKEY extension data utility class for building generic PFKEY API extension
 *  data.
 *
 *  @lib ikeutils.lib
 */
class PFKeyExtDataUtil
    {
public:    
    /**
     * This static method builds a generic PFKEY API extension data which is
     * needed for IPSEC do UDP encapsulation/decapsulation for ESP packet.
     * @param aExtData Generic extension data (returned)
     * @param aNAT_D_Flags NAT_D flags
     * @param aNATDetected Informs if NAT detected (local end behind NAT)
     * @param aNokiaNATProbeUsed Nokia NAT probe used
     * @param aUdpEncapsPort UDP encapsulation port
     * @param aKeepAliveTimeout Keep alive timeout
     * @param aDestinAddr Destination address
     * @param aOriginalAddr Peer original address
     */
    IMPORT_C static void BuildUdpEncExtensionData( TDes8& aExtData,
                                                   TUint32 aNAT_D_Flags,
                                                   TBool aNATDetected,
                                                   TBool aNokiaNATProbeUsed,
                                                   TUint16 aUdpEncapsPort,
                                                   TUint16 aKeepAliveTimeout,
                                                   const TInetAddr& aDestinAddr,
                                                   const TInetAddr& aOriginalAddr );    
    };

#endif // PFKEYEXTDATAUTIL_H
