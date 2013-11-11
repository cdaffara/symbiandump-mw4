/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class to dig information of scan results.
*
*/




#include "hssscaninfo.h"
#include "hssscaninfoie.h"
#include <e32std.h>

// Default values for information element IDs.
const TUint8   WPA_IE_ID              = 221;
const TUint32 SCAN_WPA_OUI_LENGTH    = 4;
const TUint8   SCAN_WPA_OUI[]         = { 0x00, 0x50, 0xF2, 0x01 };

const TUint8   SCAN_OUI_TYPE_OFFSET = 3;
const TUint8   SCAN_OUI_SUBTYPE_OFFSET = 4;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanInfo::HssScanInfo( const HssScanList& scan_list )
    : HssScanListIterator( scan_list ), 
      ie_iter_m( current_m + BODY_OFFSET )
{
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssSecurityMode HssScanInfo::SecurityMode()
    {
    HssScanInfoIe ie;
    return ie.SecurityMode( *this );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::InformationElement( TUint8 ie_id, 
                                                     TUint8& ie_length, 
                                                     const TUint8** ie_data )
{
    // IE is not found when the whole scan info element is gone through.
    //
    // The Element format is:
    // +----+----+--...--+
    // | a  | b  | c     |
    // +----+----+--...--+
    // where
    // a) Element id (1 byte)
    // b) Length (1 byte)
    // c) Information (length bytes)
    //

    TUint8 ie, len;
    const TUint8* data;

    if ( FirstIE( ie, len, &data ) != HssScanError_Ok )
        {
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
        }

    HssScanError ret( HssScanError_Ok );

    while ( ie != ie_id && ret == HssScanError_Ok )
        {
        ret = NextIE( ie, len, &data );
        }

    ie_length = len;
    *ie_data = data;
    return ret;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::WpaIE( TUint8& ie_length, 
                                        const TUint8** ie_data )
{
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI | Ver | GKCS | PKCSC | PKSCL | AKMSC | AKMSL | RSNCap |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID = 221 (1 octet)
    // Len    = Length (1 octet)
    // OUI    = 00:50:F2:01 (4 octets)
    // Ver    = Version (2 octets)
    // GKCS   = Group Key Cipher Suite (4 octets)
    // PKCSC  = Pairwise Key Cipher Suite Count (2 octets)
    // PKCSL  = Pairwise Key Cipher Suite List (4*m octets)
    // AKMSC  = Authenticated Key Management Suite Count (2 octets)
    // AKMSL  = Authenticated Key Management Suite List (4*n octets)
    // RSNCap = RSN Capabilities

    TUint8 ie( WPA_IE_ID ), len;
    const TUint8* data;

    // Find out the first element
    if ( InformationElement( WPA_IE_ID, len, &data ) != HssScanError_Ok )
    { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    // The ID is correct but check also the UID.
    HssScanError ret( HssScanError_Ok );

    while ( ie == WPA_IE_ID && 
            ret == HssScanError_Ok &&
            ( len < SCAN_WPA_OUI_LENGTH ||
              !( data[0] == SCAN_WPA_OUI[0] && data[1] == SCAN_WPA_OUI[1] &&
                 data[2] == SCAN_WPA_OUI[2] && data[3] == SCAN_WPA_OUI[3] ) ) )
    {
        ret = NextIE( ie, len, &data );
    }

    // Check is the element was corrupted
    if ( ie != WPA_IE_ID )
    {
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    ie_length = len;
    *ie_data = data;
    return ret;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::InformationElement( TUint8 ie_id,
                                            const HssIeOui& ie_oui,
                                            TUint8 ie_oui_type,
                                            TUint8& ie_length, 
                                            const TUint8** ie_data )
{
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI |     |      |       |       |       |       |        |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID (1 octet)
    // Len    = Length (1 octet)
    // OUI    = OUI (3 octets)
    // OUItyp = OUI Type (1 octet)

    TUint8 ie( ie_id ); 
    TUint8 len( 0 );
    const TUint8* data;

    // Find the first element
    if ( InformationElement( ie_id, len, &data ) 
         != HssScanError_Ok )
    { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    // The ID is correct but also the OUI and OUI Type need to match
 	HssScanError ret( HssScanError_Ok );

    while ( ret == HssScanError_Ok && 
            ( ie != ie_id || 
              !( data[0] == ie_oui[0] && 
                 data[1] == ie_oui[1] &&
                 data[2] == ie_oui[2] 
               ) ||
              *( data + SCAN_OUI_TYPE_OFFSET ) != ie_oui_type
            ) 
          )
    {
        ret = NextIE( ie, len, &data );
    }

    // Check if the element is corrupted
    if ( ie != ie_id )
    {
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    ie_length = len;
    *ie_data = data;
    return ret;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::InformationElement( TUint8 ie_id,
                                            const HssIeOui& ie_oui,
                                            TUint8 ie_oui_type,
                                            TUint8 ie_oui_subtype,
                                            TUint8& ie_length, 
                                            const TUint8** ie_data )
{
    // Element format is:
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // | ID | Len | OUI |     |      |       |       |       |       |        |
    // +----+-----+-----+-----+------+-------+--...--+-------+--...--+--------+
    // where
    // ID     = Element ID (1 octet)
    // Len    = Length (1 octet)
    // OUI    = OUI (3 octets)
    // OUItyp = OUI Type (1 octet)

    TUint8 ie( ie_id ); 
    TUint8 len( 0 );
    const TUint8* data;

    // Find the first element
    if ( InformationElement( ie_id, len, &data ) 
         != HssScanError_Ok )
    { // Okay, it didn't exist.
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    // The ID is correct but also the OUI and OUI Type need to match
    HssScanError ret( HssScanError_Ok );

    while ( ret == HssScanError_Ok && 
            ( ie != ie_id || 
              !( data[0] == ie_oui[0] && 
                 data[1] == ie_oui[1] &&
                 data[2] == ie_oui[2] 
               ) ||
              *( data + SCAN_OUI_TYPE_OFFSET ) != ie_oui_type ||
              *( data + SCAN_OUI_SUBTYPE_OFFSET ) != ie_oui_subtype              
            ) 
          )
    {
        ret = NextIE( ie, len, &data );
    }

    // Check if the element is corrupted
    if ( ie != ie_id )
    {
        ie_length = 0;
        *ie_data = NULL;
        return HssScanError_IeNotFound;
    }

    ie_length = len;
    *ie_data = data;
    return ret;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::FirstIE( 
    TUint8& ie, 
    TUint8& length, 
    const TUint8** data)
{
    ie_iter_m = current_m + BODY_OFFSET;
    return CurrentIE( ie, length, data );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::NextIE(
    TUint8& ie, 
    TUint8& length, 
    const TUint8** data)
{
    ie_iter_m += *( ie_iter_m+1 ) + 2;
    return CurrentIE( ie, length, data );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HssScanError HssScanInfo::CurrentIE(
    TUint8& ie, 
    TUint8& length, 
    const TUint8** data) const
{
    if ( ie_iter_m >= current_m + Size() )
    {
        ie = 0;
        length = 0;
        *data = NULL;
        return HssScanError_IeNotFound;
    }
    ie = *ie_iter_m;
    length = *( ie_iter_m+1 );
    *data = ie_iter_m+2;
    return HssScanError_Ok;
}
