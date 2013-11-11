/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN Connection Agent
*
*/

#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <cmpluginvpndef.h>
#include <commdbconnpref.h>
#include <in_iface.h>
#include <tunnelnifvar.h>
#include <d32dbmsconstants.h>
#include <featmgr.h>

#include "vpnconnagt.h"
#include "vpnsipobserver.h"


/***************CVPNConnAgtFactory********************/

extern "C" EXPORT_C CNifAgentFactory* NewAgentFactoryL()
    {   
    return new(ELeave) CVPNConnAgtFactory;
    }

void CVPNConnAgtFactory::InstallL() 
    {
    }
 
CNifAgentBase* CVPNConnAgtFactory::NewAgentL(
    const TDesC& /*aName*/)
    {
    return CVPNConnAgt::NewL();
    }

TInt CVPNConnAgtFactory::Info(
    TNifAgentInfo&  aInfo, 
    TInt            /*aIndex*/) const
    {
    aInfo.iName = KVPNConnAgtName;
    aInfo.iVersion = TVersion(KMajorVerNumber,KMinorVerNumber,KBuildVerNumber);
    return KErrNone;
    }

/***************CVPNConnAgt********************/

// ---------------------------------------------------------------------------
// CVPNConnAgt 
// ---------------------------------------------------------------------------
//
CVPNConnAgt::CVPNConnAgt() :
    iServiceStartedCallback(CActive::EPriorityStandard),
    iConnectCompleteCallback(CActive::EPriorityStandard), 
    iDisconnectCallback(CActive::EPriorityStandard)
    {
    TCallBack serviceStartedCallback(ServiceStartedCb, this);
    iServiceStartedCallback.Set(serviceStartedCallback);
    
    TCallBack connectionCompleteCallback(ConnectCompleteCb, this);
    iConnectCompleteCallback.Set(connectionCompleteCallback);

    TCallBack disconnectionCompleteCallback(DisconnectCompleteCb, this);
    iDisconnectCallback.Set(disconnectionCompleteCallback);

    iEventActivatedClose = EFalse;
    iDisconnectType = (TDeactivateType)NORMAL_DISCONNECT_MODE; // means the normal way to shutdown
    
    iFeatureManagerInitialized = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor 
// ---------------------------------------------------------------------------
//
CVPNConnAgt::~CVPNConnAgt()
    {
    LOG(TName name;
    name.AppendFormat(_L("[0x%08x]"), this);
    Log::Printf(_L("%s Destructing VPN Connection Agent\n"),name.PtrZ()));
    
    if ( iFeatureManagerInitialized )
        {
        // We can safely call UnInitializeLib as we have really intialized it.
        FeatureManager::UnInitializeLib();  // Decreases ref.count
        }

    iEventMediator.Close();
    
    iServiceStartedCallback.Cancel();
    iConnectCompleteCallback.Cancel();
    iDisconnectCallback.Cancel();
    
    delete iSipObserver;
    iSipObserver = NULL;
    
    LOG_("CVPNConnAgt::~CVPNConnAgt end");
    }

// ---------------------------------------------------------------------------
// NewL() 
// ---------------------------------------------------------------------------
//
CVPNConnAgt* CVPNConnAgt::NewL()
    {
    CVPNConnAgt* self = new (ELeave) CVPNConnAgt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ConstructL() 
// ---------------------------------------------------------------------------
//
inline void CVPNConnAgt::ConstructL()
    {
    
    LOG_1("[0x%08x] Constructing VPN Connection Agent\n", this);
    // construct the database and dialog processor
    CAgentBase::ConstructL();
    iConnected = EFalse;
    iDisconnecting = EFalse;

    // Initialize Feature Manager.
    FeatureManager::InitializeLibL();  // Successfull call increases reference count
    iFeatureManagerInitialized = ETrue;

    // Create CVpnSipObserver for communicating with SIP profile server via 
    // P&S keys for SIP de/re-registration. this pointer is passed to have call back.
    if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn ) )
        {
        LOG_("CVPNConnAgt::ConstructL FfImsDeregistrationInVpn");
        iSipObserver = CVpnSipObserver::NewL( *this );
        }

    User::LeaveIfError(iEventMediator.Connect());

    LOG_("CVPNConnAgt::ConstructL end");
    }

// ---------------------------------------------------------------------------
// Info() 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::Info(
    TNifAgentInfo& aInfo) const
    {
    aInfo.iName = KVPNConnAgtName;
    aInfo.iName.AppendFormat(_L("-AgentFactory[0x%08x]"), this);
    aInfo.iVersion = TVersion(
        TCommDbConnPref::KMajorVersionNumber,
        TCommDbConnPref::KMinorVersionNumber,
        TCommDbConnPref::KBuildVersionNumber
    );
    }

// ---------------------------------------------------------------------------
// Connect() 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::Connect(
    TAgentConnectType /*aType*/)
    {
    iServiceStartedCallback.CallBack();
    }

// ---------------------------------------------------------------------------
// Connect() 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::Connect(
    TAgentConnectType   aType, 
    CStoreableOverrideSettings* /*aOverrideSettings*/)
    {
    Connect(aType);
    }

// ---------------------------------------------------------------------------
// CancelConnect() 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::CancelConnect()
    {
    LOG_("CVPNConnAgt::CancelConnect");
    iServiceStartedCallback.Cancel();
    iConnectCompleteCallback.Cancel();
    }

// ---------------------------------------------------------------------------
// Disconnect() 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::Disconnect(
    TInt aReason)
    {
    LOG_1("Disconnect called from Nifman with reason:%d\n", aReason);
    if ( iDisconnecting )
        {
        LOG_("Already disconnecting, return\n");
        return;
        }
    
    if ( aReason == KErrConnectionTerminated )
        iDisconnectType = (TDeactivateType)SILENT_DISCONNECT_MODE;

    iLastErrorCode = aReason;
    iDisconnectCallback.CallBack();
    }

    
/****************************************************************************/
/* GetExcessData()                                                          */
/****************************************************************************/
TInt CVPNConnAgt::GetExcessData(
    TDes8& /*aBuffer*/)
    {
    return KErrNotSupported;
    }

/****************************************************************************/
/* Notification()                                                           */
/* Nif calls this via agentref to pass the name of the nif.                 */
/****************************************************************************/
TInt CVPNConnAgt::Notification(
    TNifToAgentEventType aEvent,
    TAny* aInfo)
    {
    if ( (TTunnelNifToAgentEventType)aEvent == ENifToAgentEventTypeSetIfName )
        {
        iVPNParameters.SetVPNNifName(*(TName*)aInfo);
        return KErrNone;
        }

    if ( aEvent == ENifToAgentEventTypeLinkLayerDown )
        {// Tunnelnif down
        return KErrNone;
        }

    return KErrNotSupported;
    }

/****************************************************************************/
/* GetLastError()                                                           */
/****************************************************************************/
void CVPNConnAgt::GetLastError(TInt& aError)
    {
    aError = iLastErrorCode;
    }

TInt CVPNConnAgt::IncomingConnectionReceived()
    {
    return KErrNotSupported;
    }

/****************************************************************************/
/* ServiceStartedCb()                                                       */
/****************************************************************************/
TInt CVPNConnAgt::ServiceStartedCb(TAny* aThisPtr)
    {
    CVPNConnAgt* self = (CVPNConnAgt*)aThisPtr;

    TInt err = KErrNone;
    self->ServiceStarted(err);
    return err;
    }

/****************************************************************************/
/* ConnectCompleteCb()                                                      */
/****************************************************************************/
TInt CVPNConnAgt::ConnectCompleteCb(
    TAny* aThisPtr)
    {
    CVPNConnAgt* self = (CVPNConnAgt*) aThisPtr;
    self->ConnectionComplete(KErrNone);
    return KErrNone;
    }

/****************************************************************************/
/* DisconnectCompleteCb()                                                   */
/****************************************************************************/
TInt CVPNConnAgt::DisconnectCompleteCb(
    TAny* aThisPtr)
    {
    CVPNConnAgt* self = (CVPNConnAgt*) aThisPtr;
    self->DisconnectionComplete();
    return KErrNone;
    }

/****************************************************************************/
/* ServiceStarted()                                                         */
/****************************************************************************/
void CVPNConnAgt::ServiceStarted(TInt& aError)
    {
    iLastErrorCode = KErrNone;

    iNotify->AgentProgress(EVPNConnAgtConnecting, iLastErrorCode);

    // read IAP configuration
    TRAP(iLastErrorCode, ReadConfigurationL());
    if ( iLastErrorCode )
        {
        LOG(TName name;
        name.AppendFormat(_L("[0x%08x]"), this);
        Log::Printf(_L("%s Error %d in reading configuration\n"),name.PtrZ(), iLastErrorCode));

        iNotify->AgentProgress(EVPNConnAgtDisconnected, iLastErrorCode);
        iNotify->ConnectComplete(iLastErrorCode);
        
        aError = iLastErrorCode;
        return;
        }
 
    // SIP is deregistered before starting a VPN session. First tell SIP profile server to 
    // perform deregistration.  And when it's completed, continue VPN session start.
    // Note: A wait note is not used here for holding VPN session starting process because 
    // CInternetConnectionManager is showing one.
    // Tell SIP Profile Server to perform deregistration before proceeding VPN start.
    // Multiple VPN connection use cases are handled in SIP side.  VPN just notifies SIP
    // a VPN session is about to start.
    if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn ) )
        {
        // For some reason, if the request fails, just proceeds.
        TInt err = iSipObserver->RequestDeregister();
        if ( err != KErrNone )
            {
            LOG_1( "CVPNConnAgt::ServiceStarted:  iSipObserver->RequestDeregister failed with %d", err );
            ProceedServiceStart();
            }
        // Return for now. ProceedServiceStart() will be called later by iSipObserver when
        // deregistration process has been completed. Then VPN start process will be actually
        // initiated.
        return;
        }
    // If KFeatureIdFfImsDeregistrationInVpn is disabled, no SIP deregisration is performed.
    else
        {
        ProceedServiceStart();
        }
    }
 
/****************************************************************************/
/* ProceedServiceStart()                                                    */
/* Proceed VPN session start process                                        */
/****************************************************************************/
void CVPNConnAgt::ProceedServiceStart()
    {
    LOG_1("[0x%08x] Get protocol version\n", this);
    iProtocolVersionDes().iId = iEventMediator.NewEventSpecId();
    iProtocolVersionDes().iPolicyId = *(iVPNParameters.GetVPNPolicy());
    iEventMediator.ListenToEvent(
        EGetProtocolVersionEvent, iProtocolVersionDes, *this);
    iState = EGettingProtocolVersion;
    }

/****************************************************************************/
/* ConnectionComplete()                                                     */
/****************************************************************************/
void CVPNConnAgt::ConnectionComplete(
    TInt aError)
    {
    if ( aError )
        {
        switch ( iState )
            {
            case EGettingProtocolVersion:
                iEventMediator.CancelListening(EGetProtocolVersionEvent, iProtocolVersionDes);
                iNotify->AgentProgress(EVPNConnAgtDisconnected, aError);
                iNotify->ConnectComplete(aError);
                iState = EConnectionClosed;
                return;
            case EConnecting:
                iEventMediator.CancelListening(EStartVpnConnEvent, iStartVpnConnDes);
                iNotify->AgentProgress(EVPNConnAgtDisconnected, aError);
                iNotify->ConnectComplete(aError);
                iState = EConnectionClosed;
                return;
            default:
                break;
            }
        return;
        }

    TInetIfConfig addr;
    addr.iAddress = iVPNParameters.GetVPNNifAddress();
    addr.iNameSer1 = iVPNParameters.GetVPNNifDNS1();
    addr.iNameSer2 = iVPNParameters.GetVPNNifDNS2();

    LOG(TName name;
    name.AppendFormat(_L("[0x%08x]"), this);
    TBuf<39> addrBuf;
    iVPNParameters.GetVPNNifAddress().OutputWithScope(addrBuf);
    Log::Printf(_L("%s Vpn interface address = %s\n"),name.PtrZ(), addrBuf.PtrZ()));

    LOG(iVPNParameters.GetVPNNifDNS1().OutputWithScope(addrBuf);
    Log::Printf(_L("%s Vpn DNS1 = %s\n"),name.PtrZ(), addrBuf.PtrZ()));

    LOG(iVPNParameters.GetVPNNifDNS2().OutputWithScope(addrBuf);
    Log::Printf(_L("%s Vpn DNS2 = %s\n"),name.PtrZ(), addrBuf.PtrZ()));

    // Send VPN interface address to the tunnelnif
    iNotify->Notification((TAgentToNifEventType)EAgentToNifEventTypeSetAddress, (void*)(&addr));

    // Inform the Nifman
    iNotify->AgentProgress(EVPNConnAgtConnected, KErrNone);
    iNotify->ConnectComplete(aError); 

    iConnected = ETrue;

    // initialize real if observer
    iObserveRealIapConnDes().iRealIapConnInfo.iIapId = iVPNParameters.GetRealIapId();
    iObserveRealIapConnDes().iRealIapConnInfo.iNetId = iVPNParameters.GetRealNetworkId();
    iObserveRealIapConnDes().iVpnIapId = iVPNParameters.GetVPNIapId();

    ListenRealIAP();
    ListenAddressChange();
    
    iState = EListeningEvents;
    LOG_1("[0x%08x] VPN CONNECTION READY!\n", this);
    }

/****************************************************************************/
/* DisconnectionComplete()                                                  */
/****************************************************************************/
void CVPNConnAgt::DisconnectionComplete()
    {
    if ( iDisconnecting )
        {
        LOG(TName name;
        name.AppendFormat(_L("[0x%08x]"), this);
        Log::Printf(_L("%s Already disconnecting, return\n"),name.PtrZ()));
        return;
        }
    else
        iDisconnecting = ETrue;
    
    iNotify->AgentProgress(EVPNConnAgtDisconnecting, KErrNone);
        
    if ( iConnected && (iState == EListeningEvents || iState == EStartingRealIf) )
        {
        // Start the deactivate/unload of the VPN policies.
        CancelListeners();

        if ( iState == EStartingRealIf )
            iDisconnectType = (TDeactivateType)SILENT_DISCONNECT_MODE;

        iCloseVpnConnDes().iId = iEventMediator.NewEventSpecId();
        iCloseVpnConnDes().iIkePolicyHandle = iVPNParameters.GetIkePolicyHandle();
        iCloseVpnConnDes().iIpsecPolicyHandle = iVPNParameters.GetIpsecPolicyHandle();
        iCloseVpnConnDes().iDeactivateType = iDisconnectType;
        iCloseVpnConnDes().iVpnIapId = iVPNParameters.GetVPNIapId();
        iCloseVpnConnDes().iRealIapId = iVPNParameters.GetRealIapId();
        iEventMediator.ListenToEvent(ECloseVpnConnEvent, iCloseVpnConnDes, *this);
        iState = EClosingConnection;

        LOG(TName name;
        name.AppendFormat(_L("[0x%08x]"), this);
        Log::Printf(_L("%s Disconnecting type is %d\n"),name.PtrZ(), (TInt)iDisconnectType));

        return;
        }

    if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn ) )
        {
        // SIP is re-registered when a VPN session ends.
        // Note: return value ignored. Nothing to do here for error cases.
        TInt err = iSipObserver->RequestRegister();
        if (err != KErrNone)
            {
            LOG(Log::Printf(_L("SipObserver->RequestRegister failed with %d\n"), err));
            }
        }

    iNotify->AgentProgress(EVPNConnAgtDisconnected, KErrNone);
    iNotify->DisconnectComplete();
    iDisconnecting = EFalse;
    }


/****************************************************************************/
/* EventOccured()                                                           */
/****************************************************************************/
void CVPNConnAgt::EventOccured(TInt aStatus, TEventType aType, TDesC8* aData)
    {
    LOG(TName name;
    name.AppendFormat(_L("[0x%08x]"), this);
    Log::Printf(_L("%s Event occured with status %d\n"),name.PtrZ(), aStatus));

    if ( aStatus == KErrServerTerminated )
        {                
        iNotify->Notification(EAgentToNifEventTypeDisableConnection, NULL);
        iConnected = EFalse; // Disable listening of ECloseVpnConnEvent event 
        return;
        }
    
    if ( aStatus != KErrNone || !aData )
        {
        if ( !iConnected )
            ConnectionComplete(aStatus);
        else
            DisconnectionComplete();
        
        return;
        }

    TGetProtocolVersionEventData* protocolVersion=NULL;
    TStartVpnConnEventData* connection = NULL;
    TCloseVpnConnEventData* closeData = NULL;
    switch (aType)
        {
        case EGetProtocolVersionEvent:
            protocolVersion = (TGetProtocolVersionEventData*)(aData->Ptr());
            if ( protocolVersion->iTaskStatus )
                {
                TProtocolVersion pv(EVersionIp);
                iVPNParameters.SetProtocolVersion(pv);
                }
            else          
                iVPNParameters.SetProtocolVersion(protocolVersion->iProtocolVersion);
                                
                
            LOG(Log::Printf(_L("%s Protocol resolved = %d\n"),name.PtrZ(), protocolVersion->iProtocolVersion));
    
            // Inform Nifman to go forward
            iNotify->ServiceStarted();

            // Initialise the connection parameters
            iStartVpnConnDes().iId = iEventMediator.NewEventSpecId();
            iStartVpnConnDes().iIfInfo.iVpnIapId = iVPNParameters.GetVPNIapId();
            iStartVpnConnDes().iIfInfo.iVpnNetId = iVPNParameters.GetVPNNetworkId();
            iStartVpnConnDes().iIfInfo.iRealIapId = iVPNParameters.GetRealIapId();
            iStartVpnConnDes().iIfInfo.iRealNetId = iVPNParameters.GetRealNetworkId();
            iStartVpnConnDes().iIfInfo.iVPNIfName = *(VPNNifName());
            iStartVpnConnDes().iPolicyId = *(iVPNParameters.GetVPNPolicy());

            LOG(Log::Printf(_L("%s Start connection\n"),name.PtrZ()));

            // Start connection
            iEventMediator.ListenToEvent(EStartVpnConnEvent, iStartVpnConnDes, *this);
            iState = EConnecting;
            break;
        case EStartVpnConnEvent:
            // Read the connection data (addresses and policy
            // handles)
            connection = (TStartVpnConnEventData*)(aData->Ptr());
            ASSERT(connection);
            if ( connection->iTaskStatus )
                {
                ConnectionComplete(connection->iTaskStatus);
                break;
                }

            LOG(Log::Printf(_L("%s Connection started, Ikehandle: %d, Ipsechandle: %d\n"),
                name.PtrZ(), connection->iIkePolicyHandle, connection->iIpsecPolicyHandle));
            iVPNParameters.SetRealNetworkId(connection->iNetId); 
            iVPNParameters.SetRealIapId(connection->iIapId); 
            iVPNParameters.SetIkePolicyHandle(connection->iIkePolicyHandle);
            iVPNParameters.SetIpSecPolicyHandle(connection->iIpsecPolicyHandle);
            iVPNParameters.SetAddresses(connection->iVpnAddressInfo);
            iState = EConnected;
            ConnectionComplete(KErrNone);
            break;
        case ECloseVpnConnEvent:
            LOG(Log::Printf(_L("%s ECloseVpnConnEvent\n"),name.PtrZ()));
                
            if ( FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn ) )
                {
                // SIP is re-registered when a VPN session ends.
                // Note: return value ignored. Nothing to do here for error cases.
                TInt err = iSipObserver->RequestRegister();
                if (err != KErrNone)
                    {
                    LOG(Log::Printf(_L("SipObserver->RequestRegister failed with %d\n"), err));
                    }
                }    
            
            closeData = (TCloseVpnConnEventData*)(aData->Ptr());
            if ( closeData->iTaskStatus )
                {
                LOG(Log::Printf(_L("%s ECloseVpnConnEvent: TaskStatus %d\n"),name.PtrZ(), closeData->iTaskStatus));
                iLastErrorCode = closeData->iTaskStatus;
                }

            if ( iEventActivatedClose )
                {
                iEventActivatedClose = EFalse;
                iNotify->Notification(EAgentToNifEventTypeDisableConnection, NULL);
                }
            
            iNotify->AgentProgress(EVPNConnAgtDisconnected, KErrNone);
            iNotify->DisconnectComplete();
            
            iDisconnecting = EFalse;
            iConnected = EFalse;
            
            iState = EIdle;
            break;
        case EAllInterfaceEvents:
        case EObserveRealIapConnEvent:
            {
            if ( iDisconnecting ) // Shouldn't be possible
                break;
            
            LOG(Log::Printf(_L("%s Real interface down, shutdown silently\n"),name.PtrZ()));

            iEventActivatedClose = ETrue;
            iDisconnectType = (TDeactivateType)SILENT_DISCONNECT_MODE; // means that KMD doesn't send any packets to the gateway
            DisconnectionComplete();
            break;
            }
        case EKmdAddressChangeEvent:
            {
            LOG_1("[0x%08x] Address changed event\n",this);

            TVPNAddress* ptr = (TVPNAddress*)(aData->Ptr());

            if ( iVPNParameters.GetVPNNifAddress().CmpAddr(ptr->iVPNIfAddr)
                 && iVPNParameters.GetVPNNifDNS1().CmpAddr(ptr->iVPNIfDNS1)
                 && iVPNParameters.GetVPNNifDNS2().CmpAddr(ptr->iVPNIfDNS2) )
                {
                LOG_("Address change informed but no change found");

                TConnectionInfo vpnIfaceInfo;
                vpnIfaceInfo.iIapId = iVPNParameters.GetVPNIapId();
                TConnectionInfoBuf vpnIfaceInfoBuf(vpnIfaceInfo);
                iEventMediator.ListenToEvent(
                    EKmdAddressChangeEvent, vpnIfaceInfoBuf, *this);
                }
            else
                {
                iVPNParameters.SetAddresses(*ptr);
                TInetIfConfig addr;
                addr.iAddress = iVPNParameters.GetVPNNifAddress();
                addr.iNameSer1 = iVPNParameters.GetVPNNifDNS1();
                addr.iNameSer2 = iVPNParameters.GetVPNNifDNS2();

                iNotify->Notification(
                    (TAgentToNifEventType)EAgentToNifEventTypeUpdateAddress,
                    (void*)(&addr));

                LOG(Log::Printf(_L("%s Address changed, continue listening\n"),name.PtrZ()));
                
                TConnectionInfo vpnIfaceInfo;
                vpnIfaceInfo.iIapId = iVPNParameters.GetVPNIapId();
                TConnectionInfoBuf vpnIfaceInfoBuf(vpnIfaceInfo);
                iEventMediator.ListenToEvent(EKmdAddressChangeEvent, vpnIfaceInfoBuf, *this);

                LOG(TBuf<39> addrBuf;
                iVPNParameters.GetVPNNifAddress().OutputWithScope(addrBuf);
                Log::Printf(_L("%s New Vpn interface address = %s\n"),name.PtrZ(), addrBuf.PtrZ()));

                LOG(iVPNParameters.GetVPNNifDNS1().OutputWithScope(addrBuf);
                Log::Printf(_L("%s Vpn DNS1 = %s\n"),name.PtrZ(), addrBuf.PtrZ()));

                LOG(iVPNParameters.GetVPNNifDNS2().OutputWithScope(addrBuf);
                Log::Printf(_L("%s Vpn DNS2 = %s\n"),name.PtrZ(), addrBuf.PtrZ()));
                }
            break;
            }
        default:
            LOG(Log::Printf(_L("Unknown event --> Silently Ignore.\n")));
            break;
        }
    }


/****************************************************************************/
/* ListenRealIAP()                                                          */
/****************************************************************************/
void CVPNConnAgt::ListenRealIAP()
    {
    // Listen the real interface if it shutsdown or...
    iObserveRealIapConnDes().iId = iEventMediator.NewEventSpecId();
    iEventMediator.ListenToEvent(
        EObserveRealIapConnEvent, iObserveRealIapConnDes, *this);
    }

/****************************************************************************/
/* ListenAddressChange()                                                    */
/****************************************************************************/
void CVPNConnAgt::ListenAddressChange()
    {
    // Listen to the VPN interface for address changes
    TConnectionInfo vpnIfaceInfo;
    vpnIfaceInfo.iIapId = iVPNParameters.GetVPNIapId();
    TConnectionInfoBuf vpnIfaceInfoBuf(vpnIfaceInfo);
    iEventMediator.ListenToEvent(
        EKmdAddressChangeEvent, vpnIfaceInfoBuf, *this);
    }

/****************************************************************************/
/* CancelListeners()                                                        */
/****************************************************************************/
void CVPNConnAgt::CancelListeners()
    {
    // Cancel real interface observer
    iEventMediator.CancelListening(
        EObserveRealIapConnEvent, iObserveRealIapConnDes);

   // Cancel address change event listening
    TConnectionInfo vpnIfaceInfo;
    vpnIfaceInfo.iIapId = iVPNParameters.GetVPNIapId();
    TConnectionInfoBuf vpnIfaceInfoBuf(vpnIfaceInfo);
    iEventMediator.CancelListening(EKmdAddressChangeEvent, vpnIfaceInfoBuf);
    }

// ---------------------------------------------------------------------------
// Overwrite version of ReadDes to catch the ip version asking
// ---------------------------------------------------------------------------
//
TInt CVPNConnAgt::DoReadDes(
    const TDesC& aField, TDes16& aValue, const RMessagePtr2* /*aMessage*/ )
    {
    TInt err(KErrNone);
    LOG_1("CVPNConnAgt::DoReadDes aField=%S", &aField);
    
    // If SERVICE_IF_NETWORKS is asked, get the ip version from the VPN
    // Manager Server. Otherwise use default ReadDes() function.
    //    TBuf<KCommsDbSvrMaxColumnNameLength> columnName=TPtrC(
    //          SERVICE_IF_NETWORKS);
    // temporary solution
    TBuf<KDbStoreMaxColumnLength> columnName=TPtrC(SERVICE_IF_NETWORKS);
    if ( aField.CompareF(columnName) == 0 )
        {
        TProtocolVersion protocol = iVPNParameters.GetProtocolVersion();
        if ( protocol == EVersionUnknown )
            return KErrNotFound;
        
        if ( protocol == EVersionIp )
            {
            _LIT(KIP4, "ip");
            aValue = KIP4;
            }
        else
            {
            _LIT(KIP6, "ip6");
            aValue = KIP6;            
            }
        LOG_1("CVPNConnAgt::DoReadDes aValue=%S", &aValue);
        return KErrNone;
        }
    else
        {
        err = iDatabase->ReadDes(aField, columnName);
        if ( err )
            return err;

        aValue.Copy(columnName);
        }
    
    LOG_1("CVPNConnAgt::DoReadDes aValue=%S", &aValue);
    return err;
    }

// ---------------------------------------------------------------------------
// Read IAP configurations 
// ---------------------------------------------------------------------------
//
void CVPNConnAgt::ReadConfigurationL()
    {
    // vpn iap id    
    LOG_1("CVPNConnAgt::ReadConfigurationL iIAPId:%d", iSettings.iIAPId);    
    iVPNParameters.SetVPNIapId(iSettings.iIAPId);
    
    // read vpn connection method
    using namespace CMManager;
    RCmManager cmManager;    
    cmManager.OpenL();
    CleanupClosePushL( cmManager );      
    RCmConnectionMethod vpnConnection = 
        cmManager.ConnectionMethodL( iSettings.iIAPId );
    CleanupClosePushL(vpnConnection);

    if ( EFalse == vpnConnection.GetBoolAttributeL(ECmVirtual) )
        {
        User::Leave( KErrNotSupported );
        }

    // Read VPN Network Id
    const TUint32 vpnNetworkId(vpnConnection.GetIntAttributeL(ECmNetworkId));
    iVPNParameters.SetVPNNetworkId(vpnNetworkId);

    LOG_1("CVPNConnAgt::ReadConfigurationL VpnIapId:%d", 
        iVPNParameters.GetVPNIapId());
    LOG_1("CVPNConnAgt::ReadConfigurationL VpnNetworkId:%d", 
        iVPNParameters.GetVPNNetworkId());    
        
    // vpn policy id
    HBufC* policy = vpnConnection.GetStringAttributeL(EVpnServicePolicy);
    if (policy)
        {     
        TVpnPolicyId policyId;
        ASSERT( policy->Length() <= policyId.MaxLength() );

        policyId.Copy(*policy);
        delete policy;
        iVPNParameters.SetVPNPolicy(policyId);        
        }
    
    // real IAP or SNAP. ECmNextLayerIapId == EVpnIapId
    const TUint32 realIap(vpnConnection.GetIntAttributeL(EVpnIapId)); 
    const TUint32 snap( !realIap ? 
        vpnConnection.GetIntAttributeL(ECmNextLayerSNAPId):0);
       
    CleanupStack::PopAndDestroy(); // vpnConnection
    
    if ( realIap )
        {        
        // Now load real connection
        RCmConnectionMethod realConnection = 
            cmManager.ConnectionMethodL( realIap );
        CleanupClosePushL(realConnection);      

        if ( EFalse != realConnection.GetBoolAttributeL(ECmVirtual) )
            {
            User::Leave( KErrNotSupported );    
            }
        
        // real iap id
        const TUint32 realIapId( realConnection.GetIntAttributeL(ECmIapId) );
        iVPNParameters.SetRealIapId( realIapId );
        
        // real network id
        iVPNParameters.SetRealNetworkId(
            realConnection.GetIntAttributeL(ECmNetworkId) );       
        CleanupStack::PopAndDestroy(); // realConnection
        }
    else
        {
        LOG_1("CVPNConnAgt::ReadConfigurationL snap:%d", snap);
        ASSERT( snap ); // must be snap then
        iStartVpnConnDes().iIfInfo.iSnapId = snap;
        }
    CleanupStack::PopAndDestroy(); // cmManager
    
    LOG_1("CVPNConnAgt::ReadConfigurationL RealIap:%d", 
        realIap);    
    LOG_1("CVPNConnAgt::ReadConfigurationL RealIapId:%d", 
        iVPNParameters.GetRealIapId());
    LOG_1("CVPNConnAgt::ReadConfigurationL RealNetworkId:%d", 
        iVPNParameters.GetRealNetworkId());
    }
