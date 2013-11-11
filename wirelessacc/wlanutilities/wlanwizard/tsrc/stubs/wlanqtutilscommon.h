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
* Description:
*
*/

#ifndef STUB_WLANQTUTILSCOMMON_H
#define STUB_WLANQTUTILSCOMMON_H
/*
#ifdef BUILD_WLAN_QT_UTILS_STUBS
#define WLANQTUTILITIESDLL_EXPORT Q_DECL_EXPORT
#else
#define WLANQTUTILITIESDLL_EXPORT Q_DECL_IMPORT
#endif
*/
#ifdef BUILD_WLANQTUTILITIES_DLL
#define WLANQTUTILITIESDLL_EXPORT Q_DECL_EXPORT
#else
#define WLANQTUTILITIESDLL_EXPORT Q_DECL_IMPORT
#endif
// Maximum length of WLAN SSID (limit also for number of characters for user input)
static const int KMaxSsidLen = 32;
// Maximum length of WEP key
static const int KMaxWEPKeyLen = 64;
// Maximum length of WPA key
static const int KMaxWPAKeyLen = 256;

/**
 * Bearer type.
 */
enum WlanQtUtilsBearerType {
    WlanQtUtilsBearerTypeNone = 0,     ///< None of the bearer types.
    WlanQtUtilsBearerTypeCellular ,    ///< Cellular (2G, 3G, 3.5G) bearer type.
    WlanQtUtilsBearerTypeWlan          ///< WLAN bearer type.
};

/**
 * Connection status.
 */
enum WlanQtUtilsConnectionStatus {
    WlanQtUtilsConnectionStatusNone = 0,      ///< None of the connection statuses.
    WlanQtUtilsConnectionStatusConnecting,    ///< Connecting.
    WlanQtUtilsConnectionStatusConnected,     ///< Connected.
    WlanQtUtilsConnectionStatusDisconnected   ///< Disconnected.
};

/**
 * Signal strength category boundaries in dB (absolute value).
 * Smaller value means better signal.
 */
// TODO: WLAN signal level values valid are copied from 08wk41 Sniffer. OK?
// (WINS emulator gives just 0 which is equal to "Absolute max"...)
enum
{
    WlanQtUtilsWlanSignalStrengthAbsoluteMax = 0, ///< Absolute maximum signal strength.
    WlanQtUtilsWlanSignalStrengthMax = 60,        ///< Maximum signal strength.
    WlanQtUtilsWlanSignalStrengthGood = 74,       ///< Good signal strength.
    WlanQtUtilsWlanSignalStrengthLow = 87,        ///< signal strength.
    WlanQtUtilsWlanSignalStrengthMin = 100,       ///< Minimum signal strength.
    WlanQtUtilsWlanSignalUnavailable = 9999,      ///< Signal is unavailable.
};

/** WLAN signal is unknown. */
const int WlanQtUtilsWlanSignalUnknown = 1000000;

/** WLAN transmit power is unknown. */
const unsigned WlanQtUtilsWlanTransmitPowerUnknown = 0xFFFFFFFF;

#endif // WLANQTUTILSCOMMON_H
