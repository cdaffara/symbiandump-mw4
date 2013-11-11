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


#ifndef WLANSETTINGS_S60_H_
#define WLANSETTINGS_S60_H_

// System includes

#include <e32base.h>
#include <wlancontainer.h>
#include <centralrepository.h>
#include <wlanmgmtclient.h>
#include <cmgenconnsettings.h>

// User includes

// Forward declarations

class WlanSettings;

// External data types

// Constants

// Class declaration

class CWlanSettingsPrivate : public CBase
{
public:
    
    enum JoinWlanType {
        EJoinWlanKnown = 0,
        EJoinWlanManual        
    };
    
    enum WlanOption {
        EWlanPowerSaving = 0,
        EWlanScanInterval        
    };
    
    static CWlanSettingsPrivate* NewL(WlanSettings *q_ptr);
    
    ~CWlanSettingsPrivate();
    
    void LoadDBSettingsL();
    
    void SaveDBSettingsL(TInt option);
    
    TUint ScanInterval();
    
    TBool PowerSaving();
    
    TInt JoinWlanMode();
    
    void SaveJoinWlanSettingL(TInt mode);
    
    void SetScanInterval(TUint interval);
    
    void SetPowerSaving(TBool powerSavingOption);
    
private:
    
    CWlanSettingsPrivate(WlanSettings *q_ptr);
    
    void ConstructL();
    
    void LoadJoinWlanSettingL();
    
private:    //data
    
    //! Pointer to public implementation.
    WlanSettings *q_ptr;

    CommsDat::CMDBSession *mSession;

    //Wlan management
    CWlanMgmtClient *mWlanMgmtClient;

    // Power saving.
    TBool mPowerSaving;

    // Scan networks interval in seconds. 0 for no scan, 0xFFFFFFFF for automatic. 
    TUint mScanInterval;
    
    JoinWlanType mJoinWlanType;
    
    //Struct to hold current settings
    TCmGenConnSettings mCmSettings;
};

#endif /* WLANSETTINGS_S60_H_ */
