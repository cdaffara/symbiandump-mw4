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
* Description:   Implementation of iapsettings
*
*/


#ifndef HSSIAPSETTINGSHANDLER_H
#define HSSIAPSETTINGSHANDLER_H

// INCLUDES
#include <e32base.h>
#include "hssiapsettings.h"
#include <EapSettings.h>
#include <commsdattypesv1_1.h>
#include <commdb.h>
#include <wdbifwlansettings.h>
#include <cmpluginwlandef.h>
#include <wlanmgmtclient.h>

// CONSTANTS

/**
* Max attempts to reserve a trasnaction for secure key saving to IAP
*/
const TUint KHssMaxTransAttempts = 10;

/**
* Max wait time between secure key saving attempts
*/
const TUint KHssRetryAfter = 100000;

_LIT( KGenericTable, "WLANServiceTable");
_LIT8(KEapPlus,"+0");
_LIT8(KDot,", ");

_LIT(KMark1, "[" );
_LIT(KMark2, "]" );

_LIT8(KEapNone,"0");
_LIT8(KEapGtc,"6");
_LIT8(KEapTls,"13");
_LIT8(KEapLeap,"17");
_LIT8(KEapSim,"18");
_LIT8(KEapTtls,"21");
_LIT8(KEapAka,"23");
_LIT8(KEapPeap,"25");
_LIT8(KEapMschapv2,"26");
_LIT8(KEapSecurid,"32");
_LIT8(KEapPlainMschapv2,"99");

const TUint8 KEapNoneId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const TUint8 KEapTlsId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d};
const TUint8 KEapGtcId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
const TUint8 KEapLeapId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
const TUint8 KEapSimId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12};
const TUint8 KEapTtlsId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15};
const TUint8 KEapAkaId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17};
const TUint8 KEapPeapId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19};
const TUint8 KEapMschapv2Id[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};
const TUint8 KEapSecuridId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
const TUint8 KEapPlainMschapv2Id[] = {0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x63};

const TInt KEapLength = 2;
const TUint KEapListMaxLength = 1024;
const TUint KEapExpandedIdLength = 8;

// WLAN Service Table
const TMDBElementId KCDTIdWlanServiceId        = 0x00030000;
const TMDBElementId KCDTIdWlanConnMode         = 0x00040000;
   
const TMDBElementId KCDTIdWlanSSID             = 0x00050000;
const TMDBElementId KCDTIdWlanWepKey1          = 0x00060000; //EDesC8
const TMDBElementId KCDTIdWlanWepKey2          = 0x00070000; //EDesC8
const TMDBElementId KCDTIdWlanWepKey3          = 0x00080000; //EDesC8
const TMDBElementId KCDTIdWlanWepKey4          = 0x00090000; //EDesC8
const TMDBElementId KCDTIdWlanWepIndex         = 0x000A0000; //EUint32
const TMDBElementId KCDTIdWlanSecMode          = 0x000B0000; //EUint32
const TMDBElementId KCDTIdWlanAuthMode         = 0x000C0000; //EUint32
const TMDBElementId KCDTIdWlanEnableWpaPsk     = 0x000D0000; //EUint32
const TMDBElementId KCDTIdWlanWpaPreSharedKey  = 0x000E0000; //EDesC8

const TMDBElementId KCDTIdWlanWpaKeyLength     = 0x000F0000; //EUint32
const TMDBElementId KCDTIdWlanEaps             = 0x00100000; //ELongText
const TMDBElementId KCDTIdWlanScanSSID         = 0x00110000;
const TMDBElementId KCDTIdWlanChannelID        = 0x00120000;
const TMDBElementId KCDTIdWlanUsedSSID         = 0x00130000;
const TMDBElementId KCDTIdWlanFormatKey1       = 0x00140000;
const TMDBElementId KCDTIdWlanFormatKey2       = 0x00150000;
const TMDBElementId KCDTIdWlanFormatKey3       = 0x00160000;
const TMDBElementId KCDTIdWlanFormatKey4       = 0x00170000;
const TMDBElementId KCDTIdWlanEnabledEaps      = 0x00190000;
const TMDBElementId KCDTIdWlanDisabledEaps     = 0x001A0000;

// CLASS DECLARATION
/**
* @brief IAP creator and settings handler 
*/
class CHssIapSettingsHandler : public CBase
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        static CHssIapSettingsHandler* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CHssIapSettingsHandler();
    
   	    /**
        * Create new IAP for client application use
        * @param aIapName, Name of IAP
        * @param aUid, UID of the client
        * @return Iap ID, Otherwise aIapId will be null
        */
        void CreateClientIapL( const TDesC& aIapName, 
                               TUint32& aIapId, 
                               const TDesC& aUid );
        
        /**
        * Deletes the IAP
        * @param aIapID, IAP id
        */
        void DeleteIapL( const TUint aIapId );
                  
    protected:
    
        /**
        * C++ default constructor.
        */
        CHssIapSettingsHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();	
        
    private: 
        
        /**
        * Saves WEP key to newly created IAP
        * @param aFrom, From which IAP
        * @param aTo, To what IAP
        */
        void SaveWEPKeyL( const TUint32 aFrom, const TUint32 aTo );
        
        /**
        * Saves WPA key to newly created IAP
        * @param aFrom, From which IAP
        * @param aTo, To what IAP
        */    
        void SaveWPAKeyL( const TUint32 aFrom, const TUint32 aTo );
        
        /**
        * Rolls back CMDBSession
        * @param aDbSession, Session to CommsDat
        */  
        static void RollbackCommsDat( TAny* aDbSession );

        /**
        * Convert security mode types
        * @param aSecurityMode, WLAN connection security mode
        */
        CMManager::TWlanSecMode ConvertConnectionSecurityModeToSecModeL(
                TWlanConnectionSecurityMode aSecurityMode );

        /**
        * Convert connection mode types
        * @param aConnMode, WLAN connection mode
        */
        CMManager::TWlanNetMode ConvertConnectionModeToNetModeL(
                TWlanConnectionMode aConnMode );
        
    };


#endif
