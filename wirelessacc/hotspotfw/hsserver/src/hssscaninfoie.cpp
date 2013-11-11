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
* Description:   Implementation of the ScanInfoIe class.
*
*/



#include "hssscaninfoie.h"

// Defines the id byte of the RSN Information Element.
const TUint8 SCANINFOIE_RSNIE_ID = 48;

// Defines the static offsets for different fields in RSN IE.
const TUint32 SCANINFOIE_RSNIE_GROUP_SUITE_OFFSET = 2;
const TUint32 SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET = 6;
const TUint32 SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET = 8;

// Defines the OUIs used in RSN IEs.
const TUint32 SCANINFOIE_OUI_LENGTH = 4;
const TUint8 SCANINFOIE_RSNIE_OUI_CCMP[] = { 0x00, 0x0F, 0xAC, 0x04 };
const TUint8 SCANINFOIE_RSNIE_OUI_EAP[] = { 0x00, 0x0F, 0xAC, 0x01 };
const TUint8 SCANINFOIE_RSNIE_OUI_PSK[] = { 0x00, 0x0F, 0xAC, 0x02 };

// Defines the static offsets for different fields in RSN IE.
const TUint32 SCANINFOIE_WPAIE_PAIRWISE_SUITE_COUNT_OFFSET = 10;
const TUint32 SCANINFOIE_WPAIE_PAIRWISE_SUITE_OFFSET = 12;

// Defines the OUIs used in WPA IEs.
const TUint8 SCANINFOIE_WPAIE_OUI_EAP[] = { 0x00, 0x50, 0xF2, 0x01 };
const TUint8 SCANINFOIE_WPAIE_OUI_PSK[] = { 0x00, 0x50, 0xF2, 0x02 };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ScanInfoIe::ScanInfoIe
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
HssScanInfoIe::HssScanInfoIe()
    {
    }
    
// Destructor
HssScanInfoIe::~HssScanInfoIe()
    {    
    }

// -----------------------------------------------------------------------------
// ScanInfoIe::SecurityMode
// -----------------------------------------------------------------------------
//
HssSecurityMode HssScanInfoIe::SecurityMode(
    HssScanInfo& info )
    {
    TUint8 wpaie_length( 0 );
    const TUint8* wpaie_data = NULL;
    TUint8 rsnie_length( 0 );
    const TUint8* rsnie_data = NULL;
    
    info.InformationElement( SCANINFOIE_RSNIE_ID, rsnie_length, &rsnie_data );
    info.WpaIE( wpaie_length, &wpaie_data );

    if ( !info.Privacy() )
        {
        if ( !wpaie_length && !rsnie_length )
            {
            return HssSecurityModeOpen;
            }
        return HssSecurityMode802_1x;
        }
        
    if ( !wpaie_length && !rsnie_length )
        {
        return HssSecurityModeWep;
        }

    if ( rsnie_length )
        {        
        if ( IsKeyManagement(
            HssScanInfoIeTypeRsn,
            HssScanInfoIeKeyManagementPsk,
            rsnie_length,
            rsnie_data ) )
            {
            if ( IsWpa2Ciphers(
                HssScanInfoIeTypeRsn,
                rsnie_length,
                rsnie_data ) )
                {
                return HssSecurityModeWpa2Psk;
                }
            
            return HssSecurityModeWpaPsk;            
            }
        else if ( IsKeyManagement(
            HssScanInfoIeTypeRsn,
            HssScanInfoIeKeyManagementEap,
            rsnie_length,
            rsnie_data ) )
            {
            if ( IsWpa2Ciphers(
                HssScanInfoIeTypeRsn,
                rsnie_length,
                rsnie_data ) )
                {                
                return HssSecurityModeWpa2Eap;
                }
            
            return HssSecurityModeWpaEap;            
            }                        
        }
        
    if ( wpaie_length )
        {
        if ( IsKeyManagement(
            HssScanInfoIeTypeWpa,
            HssScanInfoIeKeyManagementPsk,
            wpaie_length,
            wpaie_data ) )
            {
            return HssSecurityModeWpaPsk;            
            }
        else if ( IsKeyManagement(
            HssScanInfoIeTypeWpa,
            HssScanInfoIeKeyManagementEap,
            wpaie_length,
            wpaie_data ) )
            {
            return HssSecurityModeWpaEap;            
            }        
        }
    
    return HssSecurityMode802_1x;
    }
 
// -----------------------------------------------------------------------------
// ScanInfoIe::Compare
// -----------------------------------------------------------------------------
//   
TInt HssScanInfoIe::Compare(
    const unsigned char* pl,
    int ll, 
    const unsigned char* pr, 
    int rl )
    {
    if ( ll != rl )
        return ll - rl;

    if ( pl == pr )
        return 0;

    for ( int i( 0 ); i < ll; ++i )
        if ( *(pl+i) != *(pr+i) )
            return *(pl+i) - *(pr+i);

    return 0;    
    }

// -----------------------------------------------------------------------------
// ScanInfoIe::IsKeyManagement
// -----------------------------------------------------------------------------
//

TBool HssScanInfoIe::IsKeyManagement(
    HssScanInfoIeType ie_type,
    HssScanInfoIeKeyManagement key_type,
    TUint8 /* ie_length */,
    const TUint8* ie_data )
    {
    const TUint8* key_data = NULL;
    const TUint8* key_comp = NULL;
   
    if ( ie_type == HssScanInfoIeTypeRsn )
        {
        if ( key_type == HssScanInfoIeKeyManagementEap )
            {
            key_comp = &SCANINFOIE_RSNIE_OUI_EAP[0];
            }
        else
            {
            key_comp = &SCANINFOIE_RSNIE_OUI_PSK[0];
            }
        TUint32 suites( *( ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
        key_data = ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET +
            ( suites * SCANINFOIE_OUI_LENGTH );
        }
    else
        {
        if ( key_type == HssScanInfoIeKeyManagementEap )
            {
            key_comp = &SCANINFOIE_WPAIE_OUI_EAP[0];
            }
        else
            {
            key_comp = &SCANINFOIE_WPAIE_OUI_PSK[0];
            }
        TUint32 suites( *( ie_data + SCANINFOIE_WPAIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
        key_data = ie_data + SCANINFOIE_WPAIE_PAIRWISE_SUITE_OFFSET +
            ( suites * SCANINFOIE_OUI_LENGTH );        
        }
    TUint16 key_suites( *key_data );
    key_data += sizeof( key_suites );

    while ( key_suites-- )
        {
        if ( !Compare(
            key_data + ( key_suites * SCANINFOIE_OUI_LENGTH ), SCANINFOIE_OUI_LENGTH,
            key_comp, SCANINFOIE_OUI_LENGTH ) )
            {
            return true;
            }
        }

    return false;
    };
   
// -----------------------------------------------------------------------------
// ScanInfoIe::IsWpa2Ciphers
// -----------------------------------------------------------------------------
//
TBool HssScanInfoIe::IsWpa2Ciphers(
    HssScanInfoIeType ie_type,
    TUint8 /* ie_length */,
    const TUint8* ie_data )
    {
    /**
    * WPA IE is not used in WPA2
    */    
    if ( ie_type == HssScanInfoIeTypeWpa )
        {
        return false;
        }

    const TUint8* cipher =
        ie_data + SCANINFOIE_RSNIE_GROUP_SUITE_OFFSET;

    if ( Compare(
        cipher, SCANINFOIE_OUI_LENGTH,
        &SCANINFOIE_RSNIE_OUI_CCMP[0], SCANINFOIE_OUI_LENGTH ) )
        {
        return false;
        }

    TUint32 suites( *( ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_COUNT_OFFSET ) );
    cipher = ie_data + SCANINFOIE_RSNIE_PAIRWISE_SUITE_OFFSET;

    while ( suites-- )
        {
        if ( !Compare(
            cipher + ( suites * SCANINFOIE_OUI_LENGTH ), SCANINFOIE_OUI_LENGTH,
            &SCANINFOIE_RSNIE_OUI_CCMP[0], SCANINFOIE_OUI_LENGTH ) )
            {
            return true;
            }
        }

    return false;
    }
