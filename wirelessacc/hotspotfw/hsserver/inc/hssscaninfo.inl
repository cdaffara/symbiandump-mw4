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
* Description:   Implementation of ScanInfo inline methods.
*
*/



#include "hssscanoffsets.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 HssScanInfo::SignalNoiseRatio() const
{
    return *( current_m + RX_SNR_OFFSET );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 HssScanInfo::RXLevel() const
{
    return *( current_m + RX_LEVEL_OFFSET );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void HssScanInfo::BSSID( TUint8 bssid[BSSID_LENGTH] ) const
{
    TUint8* dst = bssid; 
    const TUint8* src = current_m + BSSID_OFFSET;
    const TUint8* end = src + BSSID_LENGTH;

    for ( ; src < end; ++dst, ++src )
    {
        *dst = *src;
    }
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16 HssScanInfo::BeaconInterval() const
{
    return *( reinterpret_cast<const TUint16*>( current_m + BEACON_INTERVAL_OFFSET ) );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16 HssScanInfo::Capability() const
{
    return *( reinterpret_cast<const TUint16*>( current_m + CAPABILITY_OFFSET ) );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool HssScanInfo::Privacy() const
    {
    if ( Capability() & SCAN_CAPABILITY_BIT_MASK_PRIVACY )
        {
        return 1;
        }

    return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void HssScanInfo::Timestamp( TUint8 timestamp[TIMESTAMP_LENGTH] ) const
    {
    TUint8* dst = timestamp; 
    const TUint8* src = current_m + TIMESTAMP_OFFSET;
    const TUint8* end = src + TIMESTAMP_LENGTH;

    for ( ; src < end; ++dst, ++src )
        {
        *dst = *src;
        }
    }
