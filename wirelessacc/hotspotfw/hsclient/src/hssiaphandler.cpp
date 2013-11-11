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



// INCLUDE FILES
#include <cdbpreftable.h>
#include "hssiaphandler.h"
#include "hotspotclientserver.h"
#include "am_debug.h"
#include <es_enum.h>

#include <cmconnectionmethod.h>
#include <cmpluginwlandef.h>
#include <cmconnectionmethoddef.h>
#include <cmmanager.h>
#include <cmmanagerdef.h>
#include <cmdestination.h>

// CONSTANTS
const TInt  KRetryCount   = 20;
const TInt  KRetryTimeout = 100000;

using namespace CMManager;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHotSpotPluginSession
// -----------------------------------------------------------------------------
//
CHssIapHandler::CHssIapHandler( )
    {
    iWLANRecord = NULL;
    DEBUG("CHssIapHandler::CHssIapHandler");
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CHssIapHandler::ConstructL()
    {
    DEBUG("CHssIapHandler::ConstructL()");
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CHssIapHandler* CHssIapHandler::NewL()
    {
    CHssIapHandler* self = new( ELeave ) CHssIapHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// ~CHssIapHandler
// -----------------------------------------------------------------------------
//
CHssIapHandler::~CHssIapHandler()
    {
    DEBUG("CHssIapHandler::~CHssIapHandler()");
    
    if ( iWLANRecord != NULL )
        {
        delete iWLANRecord;
        }
    iWLANRecord = NULL;
    }

// -----------------------------------------------------------------------------
// ChangeSettingsL
// -----------------------------------------------------------------------------
//
TInt CHssIapHandler::ChangeSettingsL( const TUint aIapID, 
                                      const THssIapSettings& aSettings )
    {
    // Wlan table name -field not modified, because it is used to store UIDs
    // of clients. 
    DEBUG("CHssIapHandler::ChangeSettingsL");
    TInt ret( KErrNone );
    
    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );
    
    // Read WLAN table service id
    TUint32 serviceId(0);
    RCmConnectionMethod plugin = cmManager.ConnectionMethodL( aIapID );
    CleanupClosePushL( plugin );
    serviceId = plugin.GetIntAttributeL( EWlanServiceId );
    DEBUG1("CHssIapHandler::ChangeSettingsL WLAN serviceId: %d", serviceId);
    
    if  ( aSettings.Name.Length() > 0 )
        {
        plugin.SetStringAttributeL( CMManager::ECmName, aSettings.Name );
        }
    
    if  ( aSettings.iSSID.Length() > 0 )
        {
        plugin.SetStringAttributeL( CMManager::EWlanSSID, aSettings.iSSID );
        }
    
    plugin.SetIntAttributeL( CMManager::EWlanConnectionMode, 
                              aSettings.iConnectionMode );
    plugin.SetIntAttributeL( CMManager::EWlanChannelID, 0 );
    plugin.SetIntAttributeL( CMManager::EWlanSecurityMode, 
                              aSettings.iSecurityMode );
    
    plugin.UpdateL();
    CleanupStack::PopAndDestroy( &plugin ); 
    CleanupStack::PopAndDestroy( &cmManager ); 

    // CommsDat section starts
    CMDBSession* dbSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( dbSession );
    iWLANRecord = static_cast<CMDBGenericRecord*>
        ( CCDRecordBase::RecordFactoryL( 0 ) );
            
    iWLANRecord->InitializeL( KGenericTable(),NULL );
    iWLANRecord->SetRecordId( serviceId );
    iWLANRecord->LoadL( *dbSession );
    
    if( aSettings.iSecurityMode == EHssWep )
        {
        DEBUG("CHssIapHandler::ChangeSettingsL -> SecurityMode == EHssWep");
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanAuthMode)) = aSettings.iAuthenticationMode;
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanWepIndex )) = aSettings.iWepIndex;
                
        ret = WriteWepKeysL( aSettings );
            
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanFormatKey1 )) = aSettings.iWepKeyFormat[0];
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanFormatKey2 )) = aSettings.iWepKeyFormat[1];
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanFormatKey3 )) = aSettings.iWepKeyFormat[2];
        *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL( KCDTIdWlanFormatKey4 )) = aSettings.iWepKeyFormat[3];
        }
    else if( aSettings.iSecurityMode > EHssWep )
        {
        DEBUG("CHssIapHandler::ChangeSettingsL -> SecurityMode > EHssWep");
        if ( aSettings.iEnableWpaPsk ) // Use PreSharedKey
            {
            TBuf8<KHssWlanWpaPskLength> buf;
        
            TInt len = aSettings.iWPAPreSharedKey.Length();
        
            for( TInt i=0; i<len; i++)
                {
                buf.Append( aSettings.iWPAPreSharedKey[i]);
                }
        
            CMDBField<TDesC8>* binField = (CMDBField<TDesC8>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaPreSharedKey);
            binField->SetMaxLengthL( len);
            binField->SetL( buf);
       
            *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanWpaKeyLength)) = aSettings.iWPAKeyLength;
            *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEnableWpaPsk)) = aSettings.iEnableWpaPsk;
            }
        else // Use EAP. EAP settings can be set using EAP API.
            {
            DEBUG("CHssIapHandler::ChangeSettingsL -> SecurityMode > EHssWep -> EAP");
            *((CMDBField<TUint32>*)iWLANRecord->GetFieldByIdL(KCDTIdWlanEnableWpaPsk)) = aSettings.iEnableWpaPsk;
            }
        }
    
    // Update access point, be prepared that Commsdat might be locked
    TInt errCode( KErrLocked );
    TInt retryCount( 0 );
    
    while( errCode == KErrLocked && retryCount < KRetryCount )
        {
        TRAP( errCode, iWLANRecord->ModifyL( *dbSession ) );

        if ( errCode == KErrLocked )
            {	
            User::After( KRetryTimeout );
            }	
        retryCount++;	        
        }
    
    if ( errCode )
        {
        // override previous ret value only when error happened        	    	
        ret = errCode;
        }

    CleanupStack::PopAndDestroy( dbSession );
    DEBUG("CHssIapHandler::ChangeSettingsL DONE");
	return ret;
    }
 
// -----------------------------------------------------------------------------
// WriteWepKeysL
// Writes WEP keys from aSettings to current WLANServiceTable record
// The record which the data is written to must be fetched from CommsDat
// before calling WriteWepKeysL()
// -----------------------------------------------------------------------------
//
TInt CHssIapHandler::WriteWepKeysL( const THssIapSettings& aSettings )
    {
    DEBUG("CHssIapHandler::WriteWepKeysL");
    // 1
    CMDBField<TDesC8>* wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey1));
    if ( aSettings.iWepKey1.KeyLength <= KHssMaxWEPKeyLength )
        {
        PackWepKeyCommsDatL( aSettings.iWepKey1, aSettings.iWepKeyFormat[0], wepKey );
        }
    else 
        {
        return KErrTooBig;
        }
    
    // 2
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey2));
    if ( aSettings.iWepKey2.KeyLength <= KHssMaxWEPKeyLength )
        {
        PackWepKeyCommsDatL( aSettings.iWepKey2, aSettings.iWepKeyFormat[1], wepKey );   
        }
    else 
        {
        return KErrTooBig;
        }
    
    // 3
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey3));
    if ( aSettings.iWepKey3.KeyLength <= KHssMaxWEPKeyLength )
        {
        PackWepKeyCommsDatL( aSettings.iWepKey3, aSettings.iWepKeyFormat[2], wepKey );    
        }
    else 
        {
        return KErrTooBig;
        }
    
    // 4
    wepKey = static_cast<CMDBField<TDesC8>*>(iWLANRecord->GetFieldByIdL(KCDTIdWlanWepKey4));
    if ( aSettings.iWepKey4.KeyLength <= KHssMaxWEPKeyLength )
        {
         PackWepKeyCommsDatL( aSettings.iWepKey4,aSettings.iWepKeyFormat[3],  wepKey );   
        }
    else 
        {
        return KErrTooBig;
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// PackWepKeyCommsDatL
// Copies wepkey string from wepkey structs TUint8 buffer to a WLANServiceTable
// record's WEP key field descriptor
// -----------------------------------------------------------------------------
//
void CHssIapHandler::PackWepKeyCommsDatL( const SHssWep& aInputKey,
                                          const EHssWEPKeyFormat&  aFormat,  
                                                  CMDBField<TDesC8>* aPackedKey )
    {
    DEBUG("CHssIapHandler:::PackWepKeyL");
    if( aInputKey.KeyLength > KHssMaxWEPKeyLength)
        {
        User::Leave( KErrArgument);
        }
    
    TBuf8<KHssMaxWEPKeyLength> wepBuf;
    
    for (TUint i = 0 ; i<aInputKey.KeyLength; i++)
        {
        wepBuf.Append(aInputKey.KeyMaterial[i]);
        }
        
    // Ascii to hexa convertion, if not already hexa
    if( EAscii == aFormat )
        {
        HBufC8* buf8Conv = HBufC8::NewLC( wepBuf.Length()*2 );
        ConvertAsciiToHex( wepBuf, buf8Conv );
        wepBuf.Copy( buf8Conv->Des() );
        CleanupStack::PopAndDestroy( buf8Conv );
        }
    
    aPackedKey->SetL( wepBuf);
    }
    
// -----------------------------------------------------------------------------
// ConvertAsciiToHex
// -----------------------------------------------------------------------------
//
void CHssIapHandler::ConvertAsciiToHex( const TDes8& aSource, 
                                                HBufC8*& aDest )
    {
    DEBUG("CHssIapHandler::ConvertAsciiToHex");
    _LIT( hex, "0123456789ABCDEF" );
    TInt size = aSource.Size();
    TPtr8 ptr = aDest->Des();
    for ( TInt ii = 0; ii < size; ii++ )
        {
        TText8 ch = aSource[ii];
        ptr.Append( hex()[(ch/16)&0x0f] );
        ptr.Append( hex()[ch&0x0f] );
        }
    }

// -----------------------------------------------------------------------------
// GetNetworkIdL
// -----------------------------------------------------------------------------
//
void CHssIapHandler::GetNetworkIdL( const TUint aIapId, TUint32& aNetId )
    {
    DEBUG( "CHssIapHandler::GetNetworkIdL()" );
    
    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    RCmConnectionMethod plugin = cmManager.ConnectionMethodL( aIapId );
    CleanupClosePushL( plugin );
    
    aNetId = plugin.GetIntAttributeL( ECmNetworkId );
    
    CleanupStack::PopAndDestroy( &plugin ); // Close() called on "plugin"
    CleanupStack::PopAndDestroy( &cmManager ); // Close() called on "cmManager"

    DEBUG1( "CHssIapHandler::GetNetworkIdL netId: %d", aNetId );
    }

// -----------------------------------------------------------------------------
// GetClientIapsL
// -----------------------------------------------------------------------------
//
void CHssIapHandler::GetClientIapsL( const TUid aUId, RArray<TUint>& aIapIdArray )
    {
    DEBUG("CHssIapSettingsHandler::GetClientsIapsL");
    TBuf<32> buffer;                    // Temporary buffer for found UID from destination.
    TUint32 iapId = 0;                  // IAP Identifiier.
    TBuf<KIapNameLength> uidClient;     // UID of the client.
    
    uidClient.Copy( aUId.Name() );
    ModifyClientUid( uidClient );

    RArray<TUint32> destArray = RArray<TUint32>( 10 );  // KCmArrayGranularity instead of 10
    CleanupClosePushL( destArray );

    RCmManager cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );    

    cmManager.AllDestinationsL( destArray );

    for (TInt i = 0; i < destArray.Count(); i++)
        {
        RCmDestination dest = cmManager.DestinationL( destArray[i] );
        CleanupClosePushL( dest );

        for (TInt j = 0; j < dest.ConnectionMethodCount(); j++)
            {
            TInt bearerType = dest.ConnectionMethodL(j).
                GetIntAttributeL( CMManager::ECmBearerType );

            if (bearerType == KUidWlanBearerType)
                {
                HBufC* uid = dest.ConnectionMethodL(j).
                        GetStringAttributeL( EWlanServiceExtensionTableName );
                CleanupStack::PushL( uid );

                // Copy found uid to temporary buffer
                buffer.Copy( *uid );    

                // If uids match, store corresponding IapId to aIapIdArray.    
                if (buffer.Compare( uidClient ) == 0)
                    {
                    iapId = dest.ConnectionMethodL(j).GetIntAttributeL( ECmIapId );
                    aIapIdArray.AppendL( iapId );
                    DEBUG("CHssIapSettingsHandler::GetClientsIapsL: UIDs matched");
                    DEBUG1("CHssIapSettingsHandler::Found IapId: %d", iapId);
                    }
                else
                    {
                    DEBUG("CHssIapSettingsHandler::GetClientsIapsL: NO match");
                    }
                CleanupStack::PopAndDestroy( uid );
                }
            }
        CleanupStack::PopAndDestroy( &dest );
        }

    CleanupStack::PopAndDestroy( &cmManager );
    CleanupStack::PopAndDestroy( &destArray );
    }

// -----------------------------------------------------------------------------
// ModifyClientUid
// -----------------------------------------------------------------------------
//
void CHssIapHandler::ModifyClientUid( TDes& aUid )
    {
    DEBUG("CHssIapHandler::ModifyClientUid");
    TInt indx = aUid.Find( KMark1 );
    if ( KErrNotFound != indx )
        {
        aUid.Delete( indx, 1 );
        indx = aUid.Find( KMark2 );
        if ( KErrNotFound != indx )
            {
            aUid.Delete( indx, 1 );
            }
        }
    }

// End of File
