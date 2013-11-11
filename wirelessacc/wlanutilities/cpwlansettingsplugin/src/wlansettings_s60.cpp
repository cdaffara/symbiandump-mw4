/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

// System includes

#include <internetconnectivitycrkeys.h>
#include <cmmanager.h>

// User includes

#include "wlansettings.h"
#include "wlansettings_s60_p.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansettings_s60Traces.h"
#endif

/*!
    \class CWlanSettingsPrivate
    \brief CWlanSettingsPrivate is the actual implementation for 
    reading/writing WLAN Settings from/to database.
*/

// External function prototypes

// Local constants
/**  Seconds per minute. */
static const TInt KWlanSettingsSecondsInMinute = 60;
/**  Default "Scan Networks" setting. */
static const TUint KWlanSettingsDefaultScanNetwork =
    WlanSettings::ScanNetworkAuto;
/**  Default "Power saving" setting. */
static const TBool KWlanSettingsDefaultPowerSaving = ETrue;

// ======== MEMBER FUNCTIONS ========

/*!
    Static NewL function for creating CWlanSettingsPrivate object.
    \return Pointer to CWlanSettingsPrivate object.
*/

CWlanSettingsPrivate* CWlanSettingsPrivate::NewL(WlanSettings *q_ptr)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_NEWL_ENTRY);
    
    CWlanSettingsPrivate* impl = new ( ELeave ) CWlanSettingsPrivate(q_ptr);
    CleanupStack::PushL( impl );
    impl->ConstructL();
    CleanupStack::Pop( impl ); 
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_NEWL_EXIT);
    return impl;
}

/*!
    Second phase Constructor.
*/

void CWlanSettingsPrivate::ConstructL()
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_CONSTRUCTL_ENTRY);
    
    mSession = CMDBSession::NewL( KCDLatestVersion );
    
#ifndef __WINS__
    mWlanMgmtClient = CWlanMgmtClient::NewL();
#endif
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_CONSTRUCTL_EXIT);
}

/*!
    Constructor.
*/

CWlanSettingsPrivate::CWlanSettingsPrivate(WlanSettings *q_ptr) :
    q_ptr(q_ptr),
    mPowerSaving(KWlanSettingsDefaultPowerSaving),
    mScanInterval(KWlanSettingsDefaultScanNetwork)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_CWLANSETINGPRIVATE_ENTRY);
    
    mCmSettings.iCellularDataUsageHome = ECmCellularDataUsageConfirm;
    mCmSettings.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;
    mCmSettings.iUsageOfWlan = ECmUsageOfWlanKnown;
    mJoinWlanType = EJoinWlanKnown;
    mSession = NULL;
    mWlanMgmtClient = NULL;
        
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_CWLANSETINGPRIVATE_EXIT);
}

/*!
    Destructor.
*/

CWlanSettingsPrivate::~CWlanSettingsPrivate()
{
    OstTraceFunctionEntry0(DUP1_CWLANSETINGPRIVATE_CWLANSETINGPRIVATE_ENTRY);
    
    delete mSession;
#ifndef __WINS__
    delete mWlanMgmtClient;
#endif
    
    OstTraceFunctionExit0(DUP1_CWLANSETINGPRIVATE_CWLANSETINGPRIVATE_EXIT);
}

/*!
    Function to load the WLAN Settings from CommsDB.
*/

void CWlanSettingsPrivate::LoadDBSettingsL()
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_LOADDBSETTINGSL_ENTRY);
    
    TBool ownTransaction( EFalse );
    if ( !mSession->IsInTransaction() ) {
        ownTransaction = ETrue;
        mSession->OpenTransactionL();
    }
    TMDBElementId tableId = 0;
    
    tableId = CCDWlanDeviceSettingsRecord::TableIdL( *mSession );
        
    CCDWlanDeviceSettingsRecord* record = new( ELeave )
            CCDWlanDeviceSettingsRecord( tableId );         
    CleanupStack::PushL( record );
    
    record->iWlanDeviceSettingsType = KWlanUserSettings; 
    
    if ( record->FindL( *mSession ) ) {
        record->LoadL( *mSession );
    }

    // Read scan interval
    OstTrace1(
        TRACE_NORMAL,
        CWLANSETTINGSPRIVATE_LOADDBSETTINGSL_SCANINTERVAL,
        "CWlanSettingsPrivate::LoadDBSettingsL ScanInterval;iBgScanInterval=%u",
        record->iBgScanInterval );
    OstTrace1(
        TRACE_NORMAL,
        CWLANSETTINGSPRIVATE_LOADDBSETTINGSL_SAVEDSCANINTERVAL,
        "CWlanSettingsPrivate::LoadDBSettingsL SavedScanInterval;iSavedBgScanInterval=%u",
        record->iSavedBgScanInterval );
    mScanInterval = record->iSavedBgScanInterval;
    //Safe check for scan interval, control should not come here.
    if (mScanInterval > (WlanSettings::ScanNetworkMax
            * KWlanSettingsSecondsInMinute)) {
        mScanInterval = WlanSettings::ScanNetworkAuto;
    }
    
    // Read power saving mode
    mPowerSaving = record->iWlanPowerMode;
    OstTraceExt1(
        TRACE_NORMAL,
        CWLANSETTINGSPRIVATE_LOADDBSETTINGSL_POWERSAVE,
        "CWlanSettingsPrivate::LoadDBSettingsL PowerSave;mPowerSaving=%hhu",
        mPowerSaving );
    
    CleanupStack::PopAndDestroy( record ); 
    
    if ( ownTransaction ) {
        mSession->CommitTransactionL();
          // Rollback operation.
    }
    
    //To Load CM Settings.
    LoadJoinWlanSettingL();
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_LOADDBSETTINGSL_EXIT);
}

/*!
    Function to load the Join WLAN networks Setting from CMManager.
*/

void CWlanSettingsPrivate::LoadJoinWlanSettingL()
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_LOADJOINWLANSETTINGL_ENTRY);
    
    RCmManager CmManager;
    CmManager.CreateTablesAndOpenL();
    CleanupClosePushL(CmManager); 
    CmManager.ReadGenConnSettingsL( mCmSettings );
    CleanupStack::PopAndDestroy( 1 );     //CmManager
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_LOADJOINWLANSETTINGL_EXIT);
}

/*!
    Function to write the Join WLAN networks Setting using CMManager.
*/

void CWlanSettingsPrivate::SaveJoinWlanSettingL(TInt mode)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_SAVEJOINWLANSETTINGL_ENTRY);
    
    switch ( mode ) {        
        case EJoinWlanKnown:
            mCmSettings.iUsageOfWlan = ECmUsageOfWlanKnown;
            break;
            
        case EJoinWlanManual:
            mCmSettings.iUsageOfWlan = ECmUsageOfWlanManual;
            break;
                        
        default:
            break;
    }
    
    RCmManager CmManager;
    CmManager.CreateTablesAndOpenL();
    CleanupClosePushL(CmManager); 
    CmManager.WriteGenConnSettingsL( mCmSettings );
    CleanupStack::PopAndDestroy( 1 );
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_SAVEJOINWLANSETTINGL_EXIT);
}

/*!
    Function to get the Join WLAN network setting.
    \return 0 for Known, 1 for Manual.
*/

TInt CWlanSettingsPrivate::JoinWlanMode()
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_JOINWLANMODE_ENTRY);
    
    TInt mode = 0;
    
    switch ( mCmSettings.iUsageOfWlan ) {
        case ECmUsageOfWlanKnown: 
            mode = EJoinWlanKnown;
            break;
            
        case ECmUsageOfWlanManual:
            mode = EJoinWlanManual;
            break;
                        
        default:
            break;
    }
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_JOINWLANMODE_EXIT);
    return mode;
}

/*!
    Function to write the WLAN Settings to CommsDb.
*/

void CWlanSettingsPrivate::SaveDBSettingsL(TInt option)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_SAVEDBSETTINGSL_ENTRY);
    
    TBool ownTransaction( EFalse );
    if ( !mSession->IsInTransaction() ) {
        ownTransaction = ETrue;
        mSession->OpenTransactionL();
    }
    TMDBElementId tableId = 0;
    tableId = CCDWlanDeviceSettingsRecord::TableIdL( *mSession );
        
    CCDWlanDeviceSettingsRecord* record = new( ELeave )
            CCDWlanDeviceSettingsRecord( tableId );         
    
    CleanupStack::PushL( record );
    
    record->iWlanDeviceSettingsType = KWlanUserSettings; 
    
    if ( record->FindL( *mSession ) ) {
        record->LoadL( *mSession );
    }
    
    switch (option) {
        case EWlanScanInterval:
            record->iBgScanInterval = mScanInterval;
            if (!q_ptr->isDevicePowerSavingEnabled()) {
                record->iSavedBgScanInterval = mScanInterval;
            }
            OstTrace1(
                TRACE_NORMAL,
                CWLANSETTINGSPRIVATE_SAVEDBSETTINGSL_SCANINTERVAL,
                "CWlanSettingsPrivate::SaveDBSettingsL ScanInterval;iBgScanInterval=%u",
                record->iBgScanInterval );
            OstTrace1(
                TRACE_NORMAL,
                CWLANSETTINGSPRIVATE_SAVEDBSETTINGSL_SAVEDSCANINTERVAL,
                "CWlanSettingsPrivate::SaveDBSettingsL SavedScanInterval;iSavedBgScanInterval=%u",
                record->iSavedBgScanInterval );
            break;

        case EWlanPowerSaving:
            OstTraceExt1(
                TRACE_NORMAL,
                CWLANSETTINGSPRIVATE_SAVEDBSETTINGSL_POWERSAVING,
                "CWlanSettingsPrivate::SaveDBSettingsL PowerSaving;mPowerSaving=%hhu",
                mPowerSaving );
            
            record->iWlanPowerMode = mPowerSaving;
            break;
        
        default:
            break;
    }
    
    // Whenever settings are modified, iUseDefaultSettings must be set to false.
    record->iUseDefaultSettings = EFalse;

    record->ModifyL( *mSession );
        
    CleanupStack::PopAndDestroy( record );        
    if ( ownTransaction ) {
        mSession->CommitTransactionL();
    }

#ifndef __WINS__
    // Notifying WLAN Engine about changes in settings
    mWlanMgmtClient->NotifyChangedSettings();
#endif
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_SAVEDBSETTINGSL_EXIT);
}

/*!
    Function to get the scan interval.
    \return Scan interval  in minutes.
*/

TUint CWlanSettingsPrivate::ScanInterval()
{
    TUint retVal;
    if ( mScanInterval == WlanSettings::ScanNetworkAuto ) {
        retVal = mScanInterval;   
    } else {
        // Return scan time in minutes
        retVal = mScanInterval / KWlanSettingsSecondsInMinute;
    }
    
    OstTrace1(
        TRACE_NORMAL,
        CWLANSETTINGSPRIVATE_SCANINTERVAL,
        "CWlanSettingsPrivate::ScanInterval;retVal=%u",
        retVal );
    
    return retVal;
}

/*!
    Function to get Power Saving Option.
    \return True if Power Saving option is enabled, otherwise False.
*/

TBool CWlanSettingsPrivate::PowerSaving()
{
    OstTraceExt1(
        TRACE_NORMAL,
        CWLANSETTINGSPRIVATE_POWERSAVING,
        "CWlanSettingsPrivate::PowerSaving;mPowerSaving=%hhx",
        mPowerSaving );
    
    return mPowerSaving;
}

/*!
    Function to set Scan interval.
    \param interval Scan interval in minutes. 
*/

void CWlanSettingsPrivate::SetScanInterval(TUint interval)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_SETSCANINTERVAL_ENTRY);
    
    if ( interval == WlanSettings::ScanNetworkAuto ) {
        mScanInterval = interval;
    }
    else {
        // Scan time stored in seconds
        mScanInterval = interval * KWlanSettingsSecondsInMinute;
    }
    
    OstTraceFunctionExit0(CWLANSETINGPRIVATE_SETSCANINTERVAL_EXIT);
}

/*!
    Function to set Power Saving Option.
    \param powerSavingOption True to enable or false to disable power saving option. 
*/

void CWlanSettingsPrivate::SetPowerSaving(TBool powerSavingOption)
{
    OstTraceFunctionEntry0(CWLANSETINGPRIVATE_SETPOWERSAVING_ENTRY);
    
    mPowerSaving = powerSavingOption;

    OstTraceFunctionExit0(CWLANSETINGPRIVATE_SETPOWERSAVING_EXIT);
}
