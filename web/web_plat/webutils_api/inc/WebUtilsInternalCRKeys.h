/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  List of CR keys.
*
*/

#ifndef WEBUTILSINTERNALCRKEYS_H
#define WEBUTILSINTERNALCRKEYS_H

#warning The Web Utils API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

#include <webutilssdkcrkeys.h>

// AppName fragment of useragent string
const TUint32 KWebUtilsUsrAg1 =	0x00000000;

// Compatability fragment of useragent string
const TUint32 KWebUtilsUsrAg2 =	0x00000001;

// PlatformVersion fragment of useragent string
const TUint32 KWebUtilsUsrAg3 =	0x00000002;

// MidPVersion fragment of useragent string
const TUint32 KWebUtilsUsrAg4 =	0x00000003;

// CLDConfiguration fragment of useragent string
const TUint32 KWebUtilsUsrAg5 =	0x00000004;

// Security fragment of usergeent string
const TUint32 KWebUtilsUsrAg6 =	0x00000005;

// Localization fragment of useragent string
const TUint32 KWebUtilsUsrAg7 =	0x00000006;

// Comment fragment of useragent string
const TUint32 KWebUtilsUsrAg8 =	0x00000007;

// Url to useragent profile
const TUint32 KWebUtilsUaProf =	0x00000008;

// Url to 3G useragent profile
const TUint32 KWebUtilsUaProf3G = 0x00000009;

// Name fragment of useragent string for MMS
const TUint32 KWebUtilsMMSUsrAg1 =	0x0000000A;

// PlatformVersion fragment of useragent string for MMS
const TUint32 KWebUtilsMMSUsrAg2 =	0x0000000B;

// Compatability fragment of useragent string for MMS
const TUint32 KWebUtilsMMSUsrAg3 =	0x0000000C;

// MidPVersion fragment of useragent string for MMS
const TUint32 KWebUtilsMMSUsrAg4 =	0x0000000D;

// CLDConfiguration fragment of useragent string for MMS 
const TUint32 KWebUtilsMMSUsrAg5 =	0x0000000E;

// Browser Version Major 
const TUint32 KWebUtilsBrowserVersionMajor =	0x0000000F;

// Browser Version Minor
const TUint32 KWebUtilsBrowserVersionMinor =	0x00000010;

// Browser Version SVN Rev 
const TUint32 KWebUtilsBrowserVersionSVNRev =	0x00000011;

// Browser Name
const TUint32 KWebUtilsBrowserName =	0x00000012;

// Ignore product program config when TRUE 
const TUint32 KWebUtilsIgnoreConfig =	0x00000013;

		
#endif // WEBUTILSINTERNALCRKEYS_H
