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
* Description:   IAP Settings for HotSpot FW 
*
*/




#ifndef HSSIAPSETTINGS_H
#define HSSIAPSETTINGS_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

//  CONSTANTS
const TInt KHssWlanWpaPskLength = 64;
const TInt KHssMaxMedTextLength	= 255;
const TUint8 KHssMaxSSIDLength = 32;
const TUint32 KHssMaxWEPKeyLength = 29;
const TInt KHssMaxNumberOfKeys = 4;

//  DATA TYPES

enum EHssWepAuthenticationMode
    {
    EHssOpen = 0,
    EHssShared = 1    
    };

enum EHssConnectionMode
    {
    EHssAdhoc = 0,
    EHssInfrastructure = 1
    };

enum EHssWlanSecurityMode
    {
    EHssAllowUnsecure = 1,
    EHssWep = 2,
    EHssWlan8021x = 4,
    EHssWpa = 8,
    EHssWpa2Only = 16
    };

enum EHssWEPKeyFormat
    {
    EAscii,                 // Ascii format
    EHexadecimal            // Hex format
    };

/**
* Data structure for storing a WEP key.
*/
struct SHssWep
    {
    /** Specifies 64/128/152/256-bit WEP. Range: 0-3 */
    TUint32 KeyIndex;
    /** The length of KeyMaterial in bytes. Range: 0-29 */
    TUint32 KeyLength;
    /** Array that stores the WEP key. */
    TUint8 KeyMaterial[KHssMaxWEPKeyLength];
    };
    
/**
* struct containing all wlan settings data
*/
class THssIapSettings 

    {
    public:
    
    TBuf<KHssMaxMedTextLength>      Name;
    
    EHssConnectionMode              iConnectionMode;        
    TBuf<KHssMaxSSIDLength>         iSSID;                

    EHssWlanSecurityMode            iSecurityMode;         
    
    SHssWep                         iWepKey1; 
    SHssWep                         iWepKey2;
    SHssWep                         iWepKey3;
    SHssWep                         iWepKey4;
    EHssWEPKeyFormat                iWepKeyFormat[KHssMaxNumberOfKeys];
    // Defines which WEP is in use. Range 0-3.
    TUint32                         iWepIndex;  

    EHssWepAuthenticationMode       iAuthenticationMode; 
    
    TBuf8<KHssWlanWpaPskLength>     iWPAPreSharedKey;
    TUint32                         iWPAKeyLength;
    TBool                           iEnableWpaPsk;
    };

#endif // HSSIAPSETTINGS_H

// end of file

