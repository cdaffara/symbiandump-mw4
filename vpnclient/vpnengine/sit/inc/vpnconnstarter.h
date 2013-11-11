/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN connection starter
*
*/

#ifndef __VPN_CONN_STARTER_H__
#define __VPN_CONN_STARTER_H__

#include "sit.h"
#include "taskhandler.h"
#include "vpnapi.h"
#include "kmdapi.h"
#include "ipsecpolapi.h"
#include "eventlogger.h"
#include "spdb.h" // CSecPolBundleList
#include "eventdefssit.h"

class CIkeData;

/**
 * VPN connection starter task handler
 *
 *  @lib (internal) eventmedsit.lib 
 *  @since S60 3.0
 */
NONSHARABLE_CLASS(CVpnConnStarter) : public CTaskHandler
    {
public:
    static CVpnConnStarter* NewL(MTaskHandlerManager* aManager,
                                 const TTaskArrivedEventData& aTaskInfo);
    ~CVpnConnStarter();

private: // From CTaskHandler
    void StartTaskHandling();
    void ChangeStateL();
    void CancelOngoingOperation();
    void ReportResult(TInt aStatus);

private: // From CActive
    TInt RunError(TInt aError);
 
private:
    CVpnConnStarter(MTaskHandlerManager* aManager,
                    const TTaskArrivedEventData& aTaskInfo);
    void ConstructL();

    void StateStartRealIap();
    void StateRealIapConnectedL();    
    void StateGetVpnPolicyL();
    void StateActivateKmdL();
    void StateAfterActivateKmdL();
    void StateLoadIpsecPolicy();
    void StateAfterLoadIpsecPolicyL();
    void StateActivateIpsecPolicy();
    void StateAfterActivateIpsecPolicyL();
    
    void RollbackProcess();
    void ResolveIpsecFQDNL(CSecPolBundleList* aSecPolBundleList);
    void ResolveIkeFQDNL(CIkeData* aIkeData);
    TInetAddr ResolveFQDNL(const TDesC& aAddress);
    
    /**
     * FormCustomTrafficSelectorsL
     * Store custom traffic selectors for IPsec
     * if there's something in IKE policy that requires
     * them. Nokia specific NAT-T is one feature that
     * needs custom traffic selectors.
     * @param aIkeData CIkeData pointer
     */
    void FormCustomTrafficSelectorsL(CIkeData *aIkeData);
    
    /**
     * InsertCustomTrafficSelectorsL
     * Insert custom traffic selectors to policy buffer
     * made by ipsecpolparser. Custom selector definitions
     * are inserted right after [POLICY]\n string.
     */
    void InsertCustomTrafficSelectorsL();
    
    void GetIkePolicyFromVpnPolicyL(const TDesC& aVpnPolicy);
    void GetIpsecPolicyFromVpnPolicyL(const TDesC& aVpnPolicy);
    void LoadIpsecPolicy();

    void IsDhcpEnabledL(TBool& aDhcpEnabled);
     
#if defined(__PFX_MIP4__)    
    TBool IsMip4FeaturePresent(void);
#endif   
    
private: // data
    TPckgBuf<TStartVpnConnEventSpec> iEventSpecDes;

    RVpnServ iVpnServ;
    RKMD iKmdServ;
    RIpsecPolicyServ iPolicyServ;

    HBufC8* iIkePolicyData;
    HBufC8* iIpsecPolicyData;
    
    // Custom IPsec traffic selectors
    HBufC8 *iCustomTs;
    
    TPolicyHandlePckg iIpsecPolicyHandleDes;

    TUint iProcessingFlags;    

    TVPNAddress iVpnAddressInfo;
    TVPNAddressPckg iVpnAddressInfoDes;

    TUint32 iVpnIapId;
    
    TVpnRealConnectionParams iIfInfo;
    TPckg<TVpnRealConnectionParams> iIfInfoDes;    

    TZoneInfoSetPckg iZoneInfoSetDes;
    
    DEFINE_EVENT_LOGGER
    };

#endif // __VPN_CONN_STARTER_H__
