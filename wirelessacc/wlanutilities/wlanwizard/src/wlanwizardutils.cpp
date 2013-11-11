/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: 
 *   WLAN Wizard Utilities
 *
 */

// System includes
#include <QString>
#include <QDebug>
#include <QVariant>

// User includes
#include "wlanwizardutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanwizardutilsTraces.h"
#endif

/*!
    \class WlanWizardUtils wlanwizardutils.cpp
    \brief Utilities for WLAN key validations.
    
*/
/*!
    \enum WlanWizardUtils::KeyStatus
    This enum defines the validation results.

    \var WlanWizardUtils::KeyStatusOk
    Key is valid.
    
    \var WlanWizardUtils::KeyStatusIllegalCharacters
    Key contains illegal characters.
    
    \var WlanWizardUtils::KeyStatusWpaTooShort
    WPA key is too short. Minimum allowed length is 8. See 
    WlanWizardUtils::validateWpaKey().
    
    \var WlanWizardUtils::KeyStatusWpaTooLong
    WPA key is too long. Minimum allowed length is 64 for hex key and 63 for 
    ascii key. See WlanWizardUtils::validateWpaKey().
    
    \var WlanWizardUtils::KeyStatusWepInvalidLength
    WEP key length is not valid. Valid keys are 
    - hex: 10 or 26 characters
    - ascii: 5 or 13 characters. See WlanWizardUtils::validateWpaKey().
    
    \var WlanWizardUtils::SsidMinLength
    Minumum acceptable length for SSID.
    
    \var WlanWizardUtils::SsidMaxLength
    Maximum acceptable length for SSID.
    
    \var WlanWizardUtils::WpaMinLength
    Minumum acceptable length for WPA key.
    
    \var WlanWizardUtils::WpaMaxLength
    Maximum acceptable length for WPA key.
    
    \var WlanWizardUtils::WepHex64BitMaxLength
    Length of WEP key: format hex 64-bits.
    
    \var WlanWizardUtils::WepHex128BitMaxLength
    Length of WEP key: format hex 128-bits.
    
    \var WlanWizardUtils::WepAscii64BitMaxLength
    Length of WEP key: format ascii 64-bits.
    
    \var WlanWizardUtils::WepAscii128BitMaxLength
    Length of WEP key: format ascii 128-bits.
    
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*! 
   Process WPA key validation. A passphrase can contain from 8 to 63 ASCII
   characters where each character MUST have a decimal encoding in the range of
   32 to 126, inclusive.
 
   A preshared key is stored as 64 character hex string.
   
   @param [in] key PSK to be validated
   
   @return Following values are possible
   - KeyStatusOk
   - KeyStatusIllegalCharacters
   - KeyStatusWpaTooShort
   - KeyStatusWpaTooLong
 */
WlanWizardUtils::KeyStatus WlanWizardUtils::validateWpaKey(const QString &key)
{
    OstTraceFunctionEntry0( WLANWIZARDUTILS_VALIDATEWPAKEY_ENTRY );
#ifdef OST_TRACE_COMPILER_IN_USE
    TPtrC tmp(key.utf16(),key.length() );
    OstTraceExt1( TRACE_NORMAL, WLANWIZARDUTILS_VALIDATEWPAKEY, 
        "WlanWizardUtils::validateWpaKey;key=%S", tmp );
#endif
    
    int length = key.length();
    KeyStatus ret = KeyStatusOk;

    if (length < WpaMinLength) {
        ret = KeyStatusWpaTooShort;
    } else if (length > WpaMaxLength) {
        ret = KeyStatusWpaTooLong;
    } else if (length == WpaMaxLength) {
        ret = isHex(key);
    } else {
        ret = isAscii(key);
    }

    OstTrace1( 
        TRACE_NORMAL, WLANWIZARDUTILS_VALIDATEWPAKEY_RETURN, 
        "WlanWizardUtils::validateWpaKey - Return;ret=%{KeyStatus}", ret );
    
    OstTraceFunctionExit0( WLANWIZARDUTILS_VALIDATEWPAKEY_EXIT );
    return ret;
}

/*!
   Process WEP key validation. Following keys are allowed:
   
   HEX: 
   - 64 bit: allowed key length = 10
   - 128 bit: allowed key length = 26
   
   ASCII:
   - 64 bit: allowed key length = 5   
   - 128 bit: allowed key length = 13
   
   @param [in] key WEP Key to be validated
   
   @return Following values are possible
   - KeyStatusOk
   - KeyStatusIllegalCharacters
   - KeyStatusWepInvalidLength
 */
WlanWizardUtils::KeyStatus WlanWizardUtils::validateWepKey(const QString &key)
{
    OstTraceFunctionEntry0( WLANWIZARDUTILS_VALIDATEWEPKEY_ENTRY );
#ifdef OST_TRACE_COMPILER_IN_USE
    TPtrC tmp(key.utf16(),key.length() );
    OstTraceExt1( TRACE_NORMAL, WLANWIZARDUTILS_VALIDATEWEPKEY, 
        "WlanWizardUtils::validateWepKey;key=%S", tmp );
#endif
    
    int length = key.length();
    KeyStatus ret = KeyStatusOk;
    
    if (length == WepHex64BitMaxLength || length == WepHex128BitMaxLength) {
        ret = isHex(key);
    } else if (length == WepAscii64BitMaxLength || length == WepAscii128BitMaxLength) {
        ret = isAscii(key);
    } else {
        ret = KeyStatusWepInvalidLength;
    }

    OstTrace1( 
        TRACE_NORMAL, WLANWIZARDUTILS_VALIDATEWEPKEY_RETURN, 
        "WlanWizardUtils::validateWepKey - Return;ret=%{KeyStatus}", ret );
    
    OstTraceFunctionExit0( WLANWIZARDUTILS_VALIDATEWEPKEY_EXIT );
    return ret;
}

/*!
   Process Ascii validation. Allowed characters are from 32 to 126.
   
   @param [in] key to be validated.
   
   @return Following values are possible
   - KeyStatusOk
   - KeyStatusIllegalCharacters
 */
WlanWizardUtils::KeyStatus WlanWizardUtils::isAscii(const QString &key)
{
    OstTraceFunctionEntry0( WLANWIZARDUTILS_ISASCII_ENTRY );
    static const QChar ch32(32);   // First visible ascii character
    static const QChar ch126(126); // Last visible ascii character
  
    const QChar *data = key.data();
    while (!data->isNull()) {
        if ((*data) < ch32 || (*data) > ch126) {
            OstTraceFunctionExit0( WLANWIZARDUTILS_ISASCII_EXIT );
            return KeyStatusIllegalCharacters;
        }
        ++data;
    }
    OstTraceFunctionExit0( DUP1_WLANWIZARDUTILS_ISASCII_EXIT );
    return KeyStatusOk;
}

/*!
   Process Hex validation. Allowed characters are
   - from 0 to 9
   - from a to f
   - from A to F
   
   @param [in] key to be validated.
   
   @return Following values are possible
   - KeyStatusOk
   - KeyStatusIllegalCharacters
 */
WlanWizardUtils::KeyStatus WlanWizardUtils::isHex(const QString &key)
{
    OstTraceFunctionEntry0( WLANWIZARDUTILS_ISHEX_ENTRY );
    static const QChar ch_A(65); // Character: A
    static const QChar ch_F(70); // Character: F
    static const QChar ch_a(97); // Character: a
    static const QChar ch_f(102);// Character: f

    const QChar *data = key.data();
    while (!data->isNull()) {
        if (data->isDigit() ||
            (*data) >= ch_a && (*data) <= ch_f ||
            (*data) >= ch_A && (*data) <= ch_F) {
            ++data;
        } else {
            OstTraceFunctionExit0( WLANWIZARDUTILS_ISHEX_EXIT );
            return KeyStatusIllegalCharacters;
        }
    }
    OstTraceFunctionExit0( DUP1_WLANWIZARDUTILS_ISHEX_EXIT );
    return KeyStatusOk;
}

/*!
   Process SSID validation.
 
   @param [in] ssid to be validated.
   
   @return Following values are possible
   - SsidStatusOk
   - SsidStatusInvalidLength
 */
WlanWizardUtils::SsidStatus WlanWizardUtils::validateSsid(const QString &ssid)
{
    OstTraceFunctionEntry0( WLANWIZARDUTILS_VALIDATESSID_ENTRY );
    TPtrC tmp(ssid.utf16(), ssid.length() );
    OstTraceExt1( TRACE_NORMAL, WLANWIZARDUTILS_VALIDATESSID, 
        "WlanWizardUtils::validateSsid;ssid=%S", tmp );
        
    SsidStatus ret = SsidStatusOk;
    
    int len = ssid.length();
    if (len < SsidMinLength || len > SsidMaxLength) {
        ret = SsidStatusInvalidLength;
    }
    OstTrace1( 
        TRACE_NORMAL, WLANWIZARDUTILS_VALIDATESSID_RETURN, 
        "WlanWizardUtils::validateSsid - Return;ret=%{SsidStatus}", ret );
    OstTraceFunctionExit0( WLANWIZARDUTILS_VALIDATESSID_EXIT );
    return ret;
}
