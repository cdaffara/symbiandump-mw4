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
* Description:   VPN Connection Agent declaration.
*
*/

/**  
 * @file vpnconnagt.h
 *
 * VPN Connection Agent declaration.
 *
 */
#if !defined __VPN_CONN_AGT_H__
#define __VPN_CONN_AGT_H__

// INCLUDES
#include <comms-infras/cagentbase.h>

#include "eventmediatorapi.h"
#include "vpnconnagtprog.h"
#include "vpnconnagtdefs.h"
#include "eventdefssit.h"
#include "log_r6.h"

// CONSTANTS
const TInt KMajorVerNumber=1;
const TInt KMinorVerNumber=0;
const TInt KBuildVerNumber=0;

_LIT(KVPNConnAgtName,"vpnconnagt");


// P&S key observer for communication between VPN client and SIP Profile server.
class CVpnSipObserver;


/**
 * A Factory for creating a VPNConnAgent.
 */
class CVPNConnAgtFactory : public CNifAgentFactory
    {
    /**
    @internalComponent
    */
    protected:  // Methods
        /**
         * "Installs" this factory to the system. From this Agent point of view
         * it doesn't do anything...
         */
        void InstallL();

        /**
         * Creates a new VPN Agent instance.
         * @param aName Not used.
         * @return Returns the pointer to the agent created if succeed. Otherwise it
         * leaves.
         */
        CNifAgentBase *NewAgentL(const TDesC& aName);

        /**
         * Returns the information defined in TNifAgentInfo.
         * @param aInfo The reference to the TNifAgentInfo object to be used to pass
         * the needed information.
         * @param aIndex Not used.
         */
        TInt Info(TNifAgentInfo& aInfo, TInt aIndex) const;
    };

/**
 * Class declaration for VPN connection agent. Each agent reference
 * has it's own instance of agent and nif. Nif sends it's name via
 * agent reference to agent. This triggers agent to initialize the VPN connection
 * through the VPN Manager Server. After VPN tunnel has been created
 * agent sends the IP address and the DNS addresses to the nif which
 * in turn informs the Nifman and IP stack that the connection has
 * been created.
 */
class CVPNConnAgt : public CAgentBase, public MEventObserver
    {
    /**
    @internalComponent
    */
    friend class CAsyncDisconnecter;
    public: // Methods
        /**
         * Static constructor.
         * @return The return value is the pointer to the CVPNConnAgt
         * if it succeed. Otherwise the fuction leaves.
         */
        static CVPNConnAgt* NewL();

        /**
         * Default C++ destructor.
         * Cancels all the Callbacks.
         */
        virtual ~CVPNConnAgt();
       
        // from CNifAgentBase
        /**
         * Returns the information defined in TNifAgentInfo.
         * @param aInfo A reference to TNifAgentInfo object which is
         * used to pass the information to caller of this function.
         */
        void Info(TNifAgentInfo& aInfo) const;

        /**
         * Starts the connection creation process.
         * @param aType Not used.
         */
        void Connect(TAgentConnectType aType);

        /**
         * Starts the connection by calling Connect(TAgentConnectType aType).
         * @param aType Type of the connection e.g. EAgentReconnect.
         * @param aOverrideSettings Not used.
         */
        void Connect(TAgentConnectType aType, CStoreableOverrideSettings* aOverrideSettings);

        /**
         * Starts the cancellation of the connect.
         */
        void CancelConnect();

        /**
         * Disconnects an ongoing connection.
         * @param aReason Not used.
         */
        void Disconnect(TInt aReason);

        /**
         * Called after Connect() in connection creation phase. Connection configuration
         * parameters are read and request for the VPNManServer to open the connection
         * is done. During the execution of this function VPN Nif is created.
         * @param aError Not used.
         */
        void ServiceStarted(TInt& aError);

        /**
         * The addresses got from the VPN Manager Server are send to VPN Nif.
         * Nifman is informed with the progress call (with parameter EVPNConnAgtConnected).
         * MNifAgentNotify interface's ConnectComplete() is called (this generates the Start()
         * call to VPN Nif.
         * @param aError Not used.
         */
        void ConnectionComplete(TInt aError);

        /**
         * Causes the deactivationing/unloading of the VPN Policies.
         */
        void DisconnectionComplete();

        /**
         * Not Supported.
         * @param aBuffer Not used.
         * @return Always returns KErrNotSupported.
         */
        TInt GetExcessData(TDes8& aBuffer);

        /**
         * Used to deliver the name of the VPN Nif to the VPN Agent.
         * @param aEvent If the value of this is ENifToVPNAgentEventTypeIfName the name of the
         * VPN Nif is copied from the aInfo parameter.
         * @param aInfo TName type pointer containing the name of the VPN NIf (if aEvent value is
         * ENifToVPNAgentEventTypeIfName).
         * @return Returns KErrNone if the fuction is used to deliver the name of the VPN Nif,
         * otherwise it returns KErrNotSupported.
         */
        TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);

        /**
         * Passes the last error code to the caller.
         * @param aError Reference to the TInt type error code.
         */
        void GetLastError(TInt& aError);

        /**
         * 
         */
        TBool IsReconnect() const {return (ETrue);};

        /**
         * Not supported.
         */
        TInt IncomingConnectionReceived();

        /**
         * Is called when an event comes from real interface via event mediator.
         * @param aStatus Error code if the call fails. KErrNone if everything succees.
         * @param aType Type pf the event e.g. ESingleInterfaceEvent.
         * @param aData Pointer to the TInterfaceNotificationBuf type packaged buffer. Contains
         * the status of the real interface.
         */
        void EventOccured(TInt aStatus, TEventType aType, TDesC8* aData);
        
        /**
         * Is called when SIP deregistration is completed. In here a VPN session
         * is actually started.
         */
        void ProceedServiceStart();

    protected:  // Methods

        /**
         * Overwrites the default implementation to catch the ip version reading from the
         * database. This value is retrieved from the VPN Manager Server.
         * @param aField Colum name in the CommDb. Ip version colum name is SERVICE_IF_NETWORKS.
         * @param aValue Value to be returned. In SERVICE_IF_NETWORKS case it's ip or ip6.
		 * @param aMessage Message to validate capabilities
         * @return Returns KErrNone if the function succeed. Otherwise it returns system wide
         * error code.
         */
        TInt DoReadDes(const TDesC& aField, TDes16& aValue, const RMessagePtr2* aMessage );

        /**
         * Returns the pointer to the parameter object defined in vpnconnagtdefs.h.
         * @return The pointer to the connection parameter object.
         */
        TVPNParameters* VPNParameters() { return &iVPNParameters; };

        /**
         * Returns the pointer to the name of the VPN Nif sent by the nif itself.
         * @return The pointer to the TName type object containing the name of the
         * VPN Nif.
         */
        TName* VPNNifName(void){ return iVPNParameters.GetVPNNifName();};

        /**
         * Returns the pointer to the interface from the Agent to Nifman. This interface
         * is used to e.g. send progress notifications from Agent to Nifman.
         * @return The pointer to the interface from the Agent to Nifman.
         */
        MNifAgentNotify* Notify() { return iNotify; };
        
        /**
         * After deactivationing/unloading of the VPN Policies Nifman is informed with the
         * progress call (with EVPNConnAgtDisconnected).
         * MNifAgentNotify interface's DisconnectComplete() is called (this generates the deletion
         * of the VPN Nif and at the end deletion of the agent.
         * 
         */
        void VPNDisconnCompleted(TInt aError);

    private:    // Methods
        /**
         * Second phase constructor.
         */
        void ConstructL();

        /**
         * Default constructor.
         */
        CVPNConnAgt();

        /**
         * CallBack to start the service (means the connection creation).
         * @param aThisPtr Pointer to the CVPNConnAgt type object.
         * @return Always returns KErrNone. 
         */
        static TInt ServiceStartedCb(TAny* aThisPtr);

        /**
         * CallBack to complete the connection.
         * @param aThisPtr Pointer to the CVPNConnAgt type object.
         * @return Always returns KErrNone. 
         */
        static TInt ConnectCompleteCb(TAny* aThisPtr);

        /**
         * CallBack to disconnect.
         * @param aThisPtr Pointer to the CVPNConnAgt type object.
         * @return Always returns KErrNone. 
         */
        static TInt DisconnectCompleteCb(TAny* aThisPtr);

        /**
         * Reads needed configuration settings from the CommDb database.
         */
        void ReadConfigurationL();

        /**
         * Starts the listening of real interface events.
         */
        void ListenRealIAP();

        /**
         * Starts the listening of virtual nif address change event.
         */
        void ListenAddressChange();

        /**
         * Cancels the listening of the real interface events and the
         * listening of virtual nif address change event.
         */
        void CancelListeners();

    private:
        /**
         * Defines the internal states.
         */
        enum TRequestState
            {
            EGettingProtocolVersion,
            EConnecting,
            EConnected,
            EClosingConnection,
            EConnectionClosed,
            EStartingRealIf,
            EListeningEvents,
            EIdle
            };

        /** CallBack object to implement ServiceStarted asynchronously. */
        CAsyncCallBack      iServiceStartedCallback;

        /** CallBack object to implement ConnectCompleted asynchronously. */
        CAsyncCallBack      iConnectCompleteCallback;

        /** CallBack object to implement Disconnect asynchronously. */
        CAsyncCallBack      iDisconnectCallback;

        /** Defines wether Agent is connected (Connection completed successfully). */
        TBool               iConnected;

        /** Connection configuration parameter member object. See vpnconnagtdefs.h. */
        TVPNParameters      iVPNParameters;

        /** An integer to keep last error code in memory. */
        TInt                iLastErrorCode;

        /** An API to EventMediator */
        REventMediator      iEventMediator;

        /** Tells if vpnconnagt must close tunnelnif */
        TBool               iEventActivatedClose;

        /** This defines the shutdown type to the kmd. Value 0 is normal and 1 is to 
            force it to shutdown without trying to send any packets anymore */
        TDeactivateType		iDisconnectType;

        /** State of the agent object */
        TRequestState       iState;

        /** Boolean to define wether disconnecting operation is ongoing */
        TBool               iDisconnecting;

        /** PckgBufs used when creating connection through eventmediator api */
        TPckgBuf<TGetProtocolVersionEventSpec>  iProtocolVersionDes;
        TPckgBuf<TStartVpnConnEventSpec>        iStartVpnConnDes;
        TPckgBuf<TCloseVpnConnEventSpec>        iCloseVpnConnDes;
        TPckgBuf<TObserveRealIapConnEventSpec>  iObserveRealIapConnDes;
        
        /** Own: P&S key observer */
        CVpnSipObserver* iSipObserver;
        
        /** Feature Manager initialzation flag */
        TBool iFeatureManagerInitialized;
    };


#endif // VPN_CONN_AGT_H
