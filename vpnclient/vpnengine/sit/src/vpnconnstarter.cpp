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

#include <e32base.h>
#include <commsdat.h>
#include <cmmanagerext.h>

#if defined(__PFX_MIP4__)
#include <FeatMgr.h>
#endif

#include "vpnconnstarter.h"
#include "ikepolparser.h"
#include "ipsecpolparser.h"
#include "log.h"

// Task handling states
enum TConnStarterState
    {
    KStateStartRealIap,
    KStateRealIapConnected,
    KStateGetVpnPolicy,
    KStateActivateKmd,
    KStateAfterActivateKmd,
    KStateLoadIpsecPolicy,
    KStateAfterLoadIpsecPolicy,
    KStateActivateIpsecPolicy,
    KStateAfterActivateIpsecPolicy
    };


CVpnConnStarter* CVpnConnStarter::NewL(MTaskHandlerManager* aManager,
                                       const TTaskArrivedEventData& aTaskInfo)
    {
    LOG(Log::Printf(_L("CVpnConnStarter::NewL - begin\n")));
    CVpnConnStarter* self = new (ELeave) CVpnConnStarter(aManager, aTaskInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG(Log::Printf(_L("CVpnConnStarter::NewL - end\n")));
    return self;
    }

CVpnConnStarter::CVpnConnStarter(MTaskHandlerManager* aManager,
                                 const TTaskArrivedEventData& aTaskInfo)
    : CTaskHandler(aManager, aTaskInfo, EStartVpnConnCancelEvent, &iEventSpecDes),
      iVpnAddressInfoDes(iVpnAddressInfo),
      iIfInfoDes(iIfInfo)
    {
    iIfInfo.iRealIapId = 0;
    iIfInfo.iRealNetId = 0;
    }

void CVpnConnStarter::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iEventMediator.Connect());
    User::LeaveIfError(iVpnServ.Connect());
    User::LeaveIfError(iKmdServ.Connect());
    User::LeaveIfError(iPolicyServ.Connect());
#if defined(__PFX_MIP4__)
    FeatureManager::InitializeLibL();
#endif
    }
    
CVpnConnStarter::~CVpnConnStarter()
    {
    LOG(Log::Printf(_L("CVpnConnStarter::~CVpnConnStarter\n")));
    Cancel();
    iEventMediator.Close();
    iVpnServ.Close();
    iKmdServ.Close();
    iPolicyServ.Close();
    
    delete iIkePolicyData;
    delete iIpsecPolicyData;
    delete iCustomTs;
    
    RELEASE_EVENT_LOGGER;
#if defined(__PFX_MIP4__)    
    FeatureManager::UnInitializeLib();  
#endif    
    }

void CVpnConnStarter::StartTaskHandling()
    {
    // Read input parameters. 
    // Changing the content of iEventSpecDes is not allowed
    iVpnIapId = iEventSpecDes().iIfInfo.iVpnIapId;
    GotoState(KStateStartRealIap);
    }

void CVpnConnStarter::ChangeStateL()
    {
    switch (NextState())
        {
        case KStateStartRealIap:
            StateStartRealIap();
            break;

        case KStateRealIapConnected:
            StateRealIapConnectedL();
            break;            

        case KStateGetVpnPolicy:
            StateGetVpnPolicyL();
            break;

        case KStateLoadIpsecPolicy:
            StateLoadIpsecPolicy();
            break;

        case KStateAfterLoadIpsecPolicy:
            StateAfterLoadIpsecPolicyL();
            break;

        case KStateActivateIpsecPolicy:
            StateActivateIpsecPolicy();
            break;

        case KStateAfterActivateIpsecPolicy:
            StateAfterActivateIpsecPolicyL();
            break;

        case KStateActivateKmd:
            StateActivateKmdL();
            break;

        case KStateAfterActivateKmd:
            StateAfterActivateKmdL();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }    
    }

void CVpnConnStarter::CancelOngoingOperation()
    {
    LOG_EVENT(R_VPN_MSG_VPN_IAP_ACT_CANCEL, NULL, 0, 0);
    
    switch (CurrState())
        {
        case KStateStartRealIap:
            DEB(LOG(Log::Printf(_L("Canceling start real connection\n") ));)             
            iKmdServ.CancelStartRealConnection();            
            break;
        case KStateRealIapConnected:
        case KStateGetVpnPolicy:
        case KStateAfterActivateKmd:
        case KStateAfterLoadIpsecPolicy:
        case KStateAfterActivateIpsecPolicy:
            // There's no ongoing external request to cancel
            break;
            
        case KStateActivateKmd:
            DEB(LOG(Log::Printf(_L("Canceling KMD activate\n") ));)             
            iKmdServ.CancelActivate();
            break;

        case KStateLoadIpsecPolicy:
            DEB(LOG(Log::Printf(_L("Canceling Ipsec policy load\n") ));)
            iPolicyServ.CancelLoad();
            break;

        case KStateActivateIpsecPolicy:
            DEB(LOG(Log::Printf(_L("Canceling Ipsec policy activate\n") ));)
            iPolicyServ.CancelActivate();
            break;

        default:
            User::Panic(KSitName, EPanicInvalidTaskHandlerState);
        }

    RollbackProcess();
    }
    
void CVpnConnStarter::StateStartRealIap()
    {       
    SetCurrState(KStateStartRealIap);
    
    DEB(LOG(Log::Printf(_L("StateStartRealIap, VPN IAP Id:%d\n"), 
        iVpnIapId));)                           
    iKmdServ.StartRealConnection(iVpnIapId, iIfInfoDes, iStatus);
    SetNextState(KStateRealIapConnected);
    SetActive();
    }

void CVpnConnStarter::StateRealIapConnectedL()
    {
    SetCurrState(KStateRealIapConnected);
    DEB(LOG(Log::Printf(_L("StateRealIapConnected:%d\n"), iStatus.Int()));)
    User::LeaveIfError( iStatus.Int() );
    INIT_EVENT_LOGGER(iEventMediator, 
    iVpnIapId, iIfInfoDes().iRealIapId);
    LOG_EVENT(R_VPN_MSG_VPN_IAP_ACT_START, this, 0, 0);    
    GotoState(KStateGetVpnPolicy);    
    }
      
void CVpnConnStarter::StateGetVpnPolicyL()
    {
    SetCurrState(KStateGetVpnPolicy);
    
    HBufC8* vpnPolicyData = NULL;
    DEB(LOG(Log::Printf(_L("Starting to get VPN policy: %S\n"), 
        &(iEventSpecDes().iPolicyId) ));)   
    // Get VPN policy data
    User::LeaveIfError(
        iVpnServ.GetPolicyData(iEventSpecDes().iPolicyId, vpnPolicyData));
    CleanupStack::PushL(vpnPolicyData);

    // Convert to 16 bit
    HBufC16* vpnPolicyData16 = CSit::To16BitL(*vpnPolicyData);
    CleanupStack::PushL(vpnPolicyData16);

    // Buffer containing Nokia NAT-T specific traffic selectors
    // if specified in IKE policy
    delete iCustomTs;
    iCustomTs = NULL;
    iCustomTs = HBufC8::NewL(256);
    
    // Extract IKE and IPSec policies from the VPN policy
    // Also stores custom Traffic Selectors if needed
    GetIkePolicyFromVpnPolicyL(*vpnPolicyData16);
    DEB(LOG(Log::Printf(_L("IKE policy parsing succeeded\n")));)
            
    GetIpsecPolicyFromVpnPolicyL(*vpnPolicyData16);
    DEB(LOG(Log::Printf(_L("Ipsec policy parsing succeeded\n")));)
    
    // Insert stored traffic selectors to IPsec policy
    InsertCustomTrafficSelectorsL();
            
    // Custom traffic selectors are no longer needed since
    // they were given to IPsec in above
    delete iCustomTs;
    iCustomTs = NULL;
    
    CleanupStack::PopAndDestroy(2); // vpnPolicyData16, vpnPolicyData
    
    GotoState(KStateLoadIpsecPolicy);   
    }
            
void CVpnConnStarter::StateActivateKmdL()
    {
    SetCurrState(KStateActivateKmd);

    DEB(LOG(Log::Printf(_L("Starting KMD activate\n")));)                           
    iKmdServ.Activate(iVpnIapId,
                      iEventSpecDes().iIfInfo.iVPNIfName,
                      *iIkePolicyData,
                      iVpnAddressInfoDes,
                      iStatus);
    
    SetNextState(KStateAfterActivateKmd);
    SetActive();
    }

void CVpnConnStarter::StateAfterActivateKmdL()
    {
    SetCurrState(KStateAfterActivateKmd);

    DEB(LOG(Log::Printf(
        _L("KMD activate completed, status = %d, VPN IAP Id = %d\n"), 
        iStatus.Int(), iVpnIapId));)
    // Leave if KMD activation failed
    User::LeaveIfError(iStatus.Int());

    LOG_EVENT(R_VPN_MSG_VPN_IAP_ACTIVATED, 
        &(iVpnAddressInfo.iVPNIfAddr), iStatus.Int(), 0);
    
    TaskComplete(KErrNone);
    }
    
void CVpnConnStarter::StateLoadIpsecPolicy()
    {
    SetCurrState(KStateLoadIpsecPolicy);

    DEB(LOG(Log::Printf(_L("Starting to load Ipsec policy\n")));)       
    LoadIpsecPolicy();
    
    SetNextState(KStateAfterLoadIpsecPolicy);
    SetActive();
    }

void CVpnConnStarter::StateAfterLoadIpsecPolicyL()
    {
    SetCurrState(KStateAfterLoadIpsecPolicy);
    DEB(LOG(Log::Printf(
        _L("Ipsec policy load completed, status =  %d\n"), iStatus.Int()));)
    // Log some debug data if policy loading failed
    if (iStatus.Int() == ESelectorConflict)
        {
        DEB(TPolicyNameInfo conflictInfo;)
        DEB(TInt err = iPolicyServ.GetDebugInfo(conflictInfo, KConflictingPolicyInfo));

        DEB(if (!err))
            DEB(LOG(Log::Printf(_L("Additional debug info: %S\n"), &conflictInfo));)
        }

    if (iStatus.Int() == EParsingError)
        {
        DEB(typedef TBuf<200> TParsingErrorInfo;)
        DEB(TParsingErrorInfo errorInfo;)
        DEB(TInt err = iPolicyServ.GetDebugInfo(errorInfo, KParsingErrorInfo));

        DEB(if (!err))
            DEB(LOG(Log::Printf(_L("Additional debug info: %S\n"), &errorInfo));)
        }

    // Leave if policy loading failed
    User::LeaveIfError(iStatus.Int());
    
    GotoState(KStateActivateIpsecPolicy);
    }
    
void CVpnConnStarter::StateActivateIpsecPolicy()
    {
    SetCurrState(KStateActivateIpsecPolicy);

    DEB(LOG(Log::Printf(_L("Activating Ipsec policy, handle = %d\n"), iIpsecPolicyHandleDes()));)           
    iPolicyServ.ActivatePolicy(iIpsecPolicyHandleDes(), iStatus);

    SetNextState(KStateAfterActivateIpsecPolicy);
    SetActive();
    }

void CVpnConnStarter::StateAfterActivateIpsecPolicyL()
    {
    DEB(LOG(Log::Printf(_L("Ipsec policy activated, status = %d\n"), iStatus.Int()));)

    User::LeaveIfError(iStatus.Int());

    GotoState(KStateActivateKmd);
    }

void CVpnConnStarter::ReportResult(TInt aStatus)
    {
    DEB(LOG(Log::Printf(_L("CVpnConnStarter::ReportResult\n")));)
    // Report the startup result with associated
    // information to the Event Mediator
    TStartVpnConnEventData eventData;

    eventData.iTaskStatus = aStatus;

    if (aStatus == KErrNone)
        {
        eventData.iIkePolicyHandle.iHandle = iVpnIapId;
        eventData.iIpsecPolicyHandle = iIpsecPolicyHandleDes();
        eventData.iVpnAddressInfo = iVpnAddressInfo; 
        eventData.iIapId = iIfInfo.iRealIapId;
        eventData.iNetId = iIfInfo.iRealNetId;
        DEB(LOG(Log::Printf(_L("Iap:%d Net:%d\n"), 
            eventData.iIapId, eventData.iNetId));) 
        }
    
    TPckg<TStartVpnConnEventData> eventDataDes(eventData);

    ReportEvent(EStartVpnConnEvent, iEventSpecDes, eventDataDes);

    LOG_EVENT(R_VPN_MSG_VPN_IAP_ACT_END, 
        &(iEventSpecDes().iPolicyId), aStatus, 0);
    }

TInt CVpnConnStarter::RunError(TInt aError)
    {
    LOG_EVENT(R_VPN_MSG_VPN_IAP_ACT_FAILED, NULL, aError, 0);
            
    RollbackProcess();
    TaskComplete(aError);
    return KErrNone;
    }

void CVpnConnStarter::RollbackProcess()
    {
    // If the VPN connection activation process
    // fails at some point, we may need to reverse
    // some of the actions that we've done:
    // - If the KMD has been activated (the IKE policy has been loaded),
    //   deactivate KMD (unload IKE policy)
    // - If the IPSec policy has been loaded and activated,
    //   unload the policy
    // No actions are needed regarding the real IAP connection
    // as it will be closed by the system in the lack of users.

    DEB(LOG(Log::Printf(_L("Deactivating IKE policy due VPN activation error, VPN IAP Id =  %d\n"),iVpnIapId));)
    iKmdServ.StopVpnConnection(iVpnIapId, TKmdStopConnection::ENormal);

    if (iIpsecPolicyHandleDes().iHandle != 0)
        {
        DEB(LOG(Log::Printf(_L("Unloading Ipsec policy due VPN activation error, VPN IAP Id =  %d\n"),iVpnIapId));)            
        TRequestStatus status;
        iPolicyServ.UnloadPolicy(iIpsecPolicyHandleDes(), status);
        User::WaitForRequest(status);
        }
    }

void CVpnConnStarter::ResolveIpsecFQDNL(CSecPolBundleList* aSecPolBundleList)
    {
    if (aSecPolBundleList)
        {
        for (TInt i(0); i<aSecPolBundleList->Count(); ++i)
            {
            CSecpolBundleItem* item = aSecPolBundleList->At(i);
            HBufC* dnsname = item->iTunnelEpFQDN;
            
            TInetAddr result = ResolveFQDNL(*dnsname);
            // 39 for ipv6
            TBuf<39> ipAddress;
            result.Output( ipAddress );       
            item->iTunnel.Input(ipAddress);
            DEB(LOG(Log::Printf(_L("Resolved:%S\n"), &ipAddress));)                         
            }
        }
    }

void CVpnConnStarter::ResolveIkeFQDNL(CIkeData* aIkeData)
    {
    ASSERT(aIkeData);
    if (aIkeData->iFQDNAddr)
        {
        TInetAddr result = ResolveFQDNL(aIkeData->iFQDNAddr->GetData());
        // 39 for ipv6
        TBuf<39> ipAddress;
        result.Output( ipAddress );       
        aIkeData->iAddr.Input(ipAddress);
        DEB(LOG(Log::Printf(_L("Resolved:%S\n"), &ipAddress));) 
        }
    }

TInetAddr CVpnConnStarter::ResolveFQDNL(const TDesC& aAddress)
    {
    DEB(LOG(Log::Printf(_L("ResolveFQDNL:%S\n"), &aAddress));)     
    TRequestStatus status;
    TNameEntry result;
    iKmdServ.ResolveAddress(iVpnIapId, aAddress, result, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    TInetAddr address(result().iAddr);
    return address;    
    }

void CVpnConnStarter::FormCustomTrafficSelectorsL(CIkeData *aIkeData)
	{
	// Add Nokia NAT-T specific traffic selectors to IPsec
    // custom traffic selector buffer
    if (aIkeData->iUseNatProbing)
    	{
    	// Default port for Nokia NAT-T
    	TInt port(KNokiaNattDefaultPort);
    	if (aIkeData->iEspUdpPort)
    		port = aIkeData->iEspUdpPort;
    		
    	TBuf8<39> addr, mask;
    	TBuf<39> buffer;
    	aIkeData->iAddr.Output(buffer);
    	addr.Copy(buffer);
    	aIkeData->iMask.Output(buffer);
    	mask.Copy(buffer);
    	
    	HBufC8 *tsBuffer = HBufC8::NewLC(128);
    	tsBuffer->Des().Format(_L8("remote %S %S local_port %d = { }\n"),
    			&addr, &mask, port);
    	DEB(LOG(Log::Printf(_L8("Custom TS:%S\n"), tsBuffer));) 
    	// Make sure that TS fits in the ts buffer
    	if (iCustomTs->Des().MaxLength() < iCustomTs->Length() + tsBuffer->Length())
    		{
    		iCustomTs = iCustomTs->ReAllocL(iCustomTs->Length() + tsBuffer->Length());
    		}
    	iCustomTs->Des().Append(*tsBuffer);
    	CleanupStack::PopAndDestroy(tsBuffer);
    	}
	}

void CVpnConnStarter::InsertCustomTrafficSelectorsL()
	{
	_LIT8(KSearchTerm, "[POLICY]\n");
	TInt pos = iIpsecPolicyData->Find(KSearchTerm);
	if (pos != KErrNotFound)
		{
		HBufC8 *newIpsecPolicyData = HBufC8::NewL(
				iIpsecPolicyData->Length() + iCustomTs->Length());
		pos += KSearchTerm().Length();
		
		// Add data to "[POLICY]\n"
		newIpsecPolicyData->Des().Append(iIpsecPolicyData->Des().Left(pos));
		
		// Add custom traffic selectors
		newIpsecPolicyData->Des().Append(*iCustomTs);
		
		// Add rest of the original data
		newIpsecPolicyData->Des().Append(iIpsecPolicyData->Des().Right(
				iIpsecPolicyData->Length() - pos));
		
		delete iIpsecPolicyData;
		iIpsecPolicyData = newIpsecPolicyData;
		
		DEB(LOG(Log::Write(*iIpsecPolicyData)));
		}
	}

void CVpnConnStarter::GetIkePolicyFromVpnPolicyL(const TDesC& aVpnPolicy)
    {
    CIkeDataArray* ikeList = CIkeDataArray::NewL(1);
    CleanupStack::PushL(ikeList);
    
    TIkeParser ikeParser(aVpnPolicy);
    ikeParser.ParseIKESectionsL(ikeList);

    iIkePolicyData = HBufC8::NewL(KIkePolicyBufferSizeIncrement);
    const TInt maxRetrans(8);
    const TInt maxTraceFileSize(16);
    User::LeaveIfError(ikeParser.GeneralInfoWrite(ikeList->iMaxLifetimeSec, 
                                                  ikeList->iMaxLifetimeKB,
                                                  maxRetrans,
                                                  maxTraceFileSize, 
                                                  iIkePolicyData));

    for (TInt i(0); i < ikeList->Count(); i++)
        {
        CIkeData* ikeData = ikeList->At(i);
        ResolveIkeFQDNL(ikeData);
        FormCustomTrafficSelectorsL(ikeData);
        User::LeaveIfError(ikeParser.MainWrite(ikeData, iIkePolicyData));
        }

    CleanupStack::PopAndDestroy(ikeList);
    }

void CVpnConnStarter::GetIpsecPolicyFromVpnPolicyL(const TDesC& aVpnPolicy)
    {
    CIpSecurityPiece* pieceData  = new (ELeave) CIpSecurityPiece;
    CleanupStack::PushL(pieceData);
    pieceData->ConstructL();

    TIpSecParser parser(aVpnPolicy);

    TInt ret = parser.ParseAndIgnoreIKEL(pieceData);
    
    if (ret != KErrNone)
        {
        HBufC* errorInfo = HBufC16::NewL(200);
        errorInfo->Des().Copy(pieceData->iErrorInfo);
        LOG(TPtr ptr = errorInfo->Des(); Log::Printf(_L("Parsing error info: %S\n"), &ptr));
        delete errorInfo;
        CleanupStack::PopAndDestroy(); // pieceData
        User::Leave(ret);
        }

    ResolveIpsecFQDNL(pieceData->FQDNAddressListL());
    
    iIpsecPolicyData = HBufC8::NewL(KPolicyBufferSizeIncrement);
    ret = parser.Write(pieceData, iIpsecPolicyData);
    
    if (ret != KErrNone)
       {
        LOG(Log::Printf(_L("Ipsec policy write error: %d\n"), ret));        
        CleanupStack::PopAndDestroy(); // pieceData
        User::Leave(ret);
       }

    CleanupStack::PopAndDestroy(); // pieceData
    }

void CVpnConnStarter::LoadIpsecPolicy()
    {
    // Specify network IDs for tunnel end-point and selector definitions
    iZoneInfoSetDes().iSelectorZone.iScope = KScopeNetwork;
    iZoneInfoSetDes().iSelectorZone.iId = iEventSpecDes().iIfInfo.iVpnNetId;
    iZoneInfoSetDes().iEndPointZone.iScope = KScopeNetwork;
    iZoneInfoSetDes().iEndPointZone.iId = iIfInfo.iRealNetId;
    
    LOG(Log::Printf(_L("CVpnConnStarter::LoadIpsecPolicy - VPN NET ID=%d, Real NET ID=%d\n"),
            iEventSpecDes().iIfInfo.iVpnNetId, iIfInfo.iRealNetId ));

#if defined(__PFX_MIP4__)

    if (IsMip4FeaturePresent())
        {
        LOG(Log::Printf(_L("CVpnConnStarter::LoadIpsecPolicy - MIPv4 present, adding MIPv4 bypass selectors\n")));
        iProcessingFlags = KAddIkeBypassSelectors | KAddMip4BypassSelectors;
        }
    else
        {
        LOG(Log::Printf(_L("CVpnConnStarter::LoadIpsecPolicy - MIPv4 NOT present, NOT adding MIPv4 bypass selectors\n")));
        iProcessingFlags = KAddIkeBypassSelectors;
        }
    
#else
    
    LOG(Log::Printf(_L("CVpnConnStarter::LoadIpsecPolicy - MIPv4 NOT present, NOT adding MIPv4 bypass selectors\n")));
    iProcessingFlags = KAddIkeBypassSelectors;
    
#endif // __PFX_MIP4__

    // Check from commdb wether "real" interface uses DHCP
    // and if it does, KAddDhcpBypassSelectors is added to
    // the processing flags 
    
    TBool dhcpEnabled = EFalse;
    TRAPD(err, IsDhcpEnabledL(dhcpEnabled));
    
    if (!err && dhcpEnabled)
        {
        iProcessingFlags |= KAddDhcpBypassSelectors;
        }
    
    iPolicyServ.LoadPolicy(*iIpsecPolicyData, iIpsecPolicyHandleDes,
                           iStatus, iZoneInfoSetDes, iProcessingFlags);
    }

void CVpnConnStarter::IsDhcpEnabledL(TBool& aDhcpEnabled)
    {
    using namespace CMManager;
    
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL(cmManagerExt);
    
    RCmConnectionMethodExt connectionMethod = cmManagerExt.ConnectionMethodL( iIfInfo.iRealIapId );
    CleanupClosePushL(connectionMethod);
    
    aDhcpEnabled = connectionMethod.GetBoolAttributeL(ECmIPAddFromServer);
	if (!aDhcpEnabled)
	    {
        aDhcpEnabled = connectionMethod.GetBoolAttributeL(ECmIPDNSAddrFromServer);
		}

    CleanupStack::PopAndDestroy(2);
    }

#if defined(__PFX_MIP4__)
    
TBool CVpnConnStarter::IsMip4FeaturePresent(void)
    {
    return FeatureManager::FeatureSupported(KFeatureIdMIPv4);
    }

#endif // __PFX_MIP4__
