/**
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
* Description: CUpnpTmServerImpl class declaration
*
*/

#ifndef __UPNPTMSERVERIMPL_H__
#define __UPNPTMSERVERIMPL_H__

// System Includes
#include <upnpsettings.h>
#include <upnphttpserversession.h>
#include <utf.h>

// User Includes
#include "upnptmserver.h"
#include "upnptminfoelement.h"
#include "upnptmserverdevice.h"
#include "upnpremotableappstore.h"
#include "upnptmfilteredapplist.h"
#include "upnptmiconmapping.h"
#include "upnpsvgimageconverter.h"

//Literals
_LIT8(KStartApp,                 "&lt;app&gt;");
_LIT8(KEndApp,                   "&lt;/app&gt;");
_LIT8(KStartIconList,            "&lt;iconList&gt;");
_LIT8(KEndIconList,              "&lt;/iconList&gt;");
_LIT8(KStartNameDesc,            "&lt;description&gt;");
_LIT8(KEndNameDesc,              "&lt;/description&gt;");
_LIT8(KWildCard,                 "*");
_LIT8(KAtToken,                  "@");

//Constants
const TUint KActionBufferLength  = 1000;

/**
 * Implementation class ( body ) for CUpnpTmServer class
 *
 */
class CUpnpTmServerImpl : public CBase 
    {
public:
    static CUpnpTmServerImpl* NewL( CUpnpTmServerDeviceInfo& aDeviceInfo, 
                                    MUpnpTmServerObserver& aTmServerObserver );
    
    virtual ~CUpnpTmServerImpl();   

    void RegisterAppL( CUpnpRemotableApp* aRemotableApp );
    void RegisterAppsL(const RPointerArray<CUpnpRemotableApp>& aRemotableAppList);
    TInt UnRegisterApp( TUint aAppId );
    TInt UnRegisterApps( const RArray<TUint>& aAppIdArray );
                                                                  
    void SetXmlSignatureL( const TDesC8& aSignature );  
    const TDesC8& XmlSignature()const;
    // Start and Stop methods
    void StartDeviceL();
    void StopDeviceL();
    CUpnpRemotableApp& GetRemotableApp( TUint aAppId, TInt& aErr );
    void GenerateUrlComponents( CUpnpHttpServerSession& aHttpServerSession );
    const TDesC8& ConvertIntToDescriptor( TUint aIntVal );
    
public:
    //Action handling methods
    const TDesC8& GetApplicationListL( const TDesC8& aAppListFilter,
                                   TUint aProfileId, TTerminalModeErrorCode& aErr );
    TTerminalModeErrorCode LaunchApp( TUint aAppId, TUint aProfileId, RBuf8& aUrl ); 
    TTerminalModeErrorCode TerminateApp( TUint aAppId, TUint aProfileId );
    void GetAppStatusL( const TDesC8& aAppIdBuffer,RBuf8& aStatusType,
                                              TTerminalModeErrorCode& aErr );
    TTerminalModeErrorCode GetMaxNumProfiles( TUint& aNumMaxProfiles );
    TTerminalModeErrorCode SetClientProfile( TUint aProfileId, const TDesC8& aInputClientProfile,
                                                                         RBuf8& aResultProfile );
    TTerminalModeErrorCode GetClientProfile( TUint aProfileId, RBuf8& aClientProfile );
    
    //Event Notification methods
    void UpdateAppStatusL( const RArray<TUint>& aUpdatedAppIdList );
    void UpdateAppListL( const RArray<TUint>& aUpdatedAppIdList );
    void UpdateUnusedProfileIdsL( const RArray<TUint>& aUnusedProfileIdList );
    
    // Application icon fetching method
    void GetIconInfoL( const TDesC8& aUrl, CUpnpHttpServerTransaction*& aResultTrans );
    
private:
    CUpnpTmServerImpl( MUpnpTmServerObserver& aTmServerObserver );
    void ConstructL(  CUpnpTmServerDeviceInfo&  aDeviceInfo );
    void RestoreIap();
    
    void AppendDataL( const TDesC8& aData );
    TTerminalModeErrorCode AuthorizeApp( TUint aAppId );
    void ConstructActionResponseBufferL( TInt aAppIndex ); 
    void ConstructLeadingSoapBufferL( TInt aAppIndex );
    void SetRemotableAppIconInfoL(  const CUpnpTerminalModeIcon& aTmIcon );
    void SetAllowedProfileIdListL( TInt aAppIndex );
    void SetTerminalModeInfoElementL( const CUpnpTmInfoElement& aTmInfoElement );
    void SetResourceStatusL( TInt aAppIndex );
    void CreateGetAppStatusResponseL( TUint aAppId, TTerminalModeErrorCode& aErr ); 
    void CreateEventMessageL( const RArray<TUint>& aIdList );
    friend class CUpnpTmFilteredAppList;
 
private:
    CUpnpTmServerDevice*                  iTmServerDevice;
    MUpnpTmServerObserver&                iTmServerObserver;
    RBuf8                                 iResponseBuf;
    TBuf8<UpnpString::KShortStringLength> iAddrBuf;
    TBuf8<UpnpString::KShortStringLength> iPortBuf;   
    RBuf8                                 iXmSignature;
    CUpnpRemotableAppStore*               iAppStore;
    RFs                                   iIconFileSession;
    CUpnpTmFilteredAppList*               iFilteredAppList;
    TBuf8<UpnpString::KShortStringLength> iBufVal;
    CUpnpSettings*                        iIapSetting;
    TInt                                  iIapId;   // Original IAP ID used by DLNA  
    TUint                                 iProfileId;
    // Mapping between application icon filepath and its URL
    CUpnpTmIconMapping*                   iIconMapping;
    };

#endif  // __UPNPTMSERVERIMPL_H__
