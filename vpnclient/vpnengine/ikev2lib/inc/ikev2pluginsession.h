/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKEv2 plugin session, handles one IKEv2 vpn connection
*
*/

#ifndef C_IKEV2PLUGINSESSION_H
#define C_IKEV2PLUGINSESSION_H

#include <e32base.h>

#include "ikepluginsessionif.h"
#include "ikev2receiver.h"
#include "ikev2deactivationtimer.h"


class MIkeDataInterface;
class CIkev2PlugIn;
class MKmdEventLoggerIf;
class MIkeDebug;
class CIkev2Negotiation;
class CIkev2SA;
class TIkev2SAData;      
class TIkeV2IpsecSAData;
class CIkev2Receiver;
class TIpsecSaSpec;
class TIpsecSAData;
class TPfkeyMessage;
class CIpsecSaSpecList;
class CPFKeySocketIf;
class CIpsecPolicyUtil;
class CIkev2MessageSendQueue;


NONSHARABLE_CLASS(CIkev2PluginSession) : public CBase,
                                         public MIkePluginSessionIf,
                                         public MIkev2ReceiverCallback,
                                         public MIkev2DeactivationTimerCallback
    {
public:
    static CIkev2PluginSession* NewL( TUint32 aVpnIapId,
                                      TUint32 aVpnNetId,
                                      TUint32 aVpnInterfaceIndex,
                                      MIkeDataInterface& aDataInterface,
                                      CIkev2PlugIn& aPlugin,
                                      CPFKeySocketIf& aPfKeySocketIf,
                                      CIpsecPolicyUtil& aIpsecPolicyUtil,
                                      MKmdEventLoggerIf& aEventLogger,
                                      MIkeDebug& aDebug );
    ~CIkev2PluginSession();
    
    /**
     * Starts negotiation with a peer.
     *
     * @param aIkeData IKE policy data
     * @param aVpnInterfaceIndex VPN interface index
     * @param aInternalAddress Internal address (returned)
     * @param aStatus Completion status (returned) 
     */
    void NegotiateWithHost( const CIkeData& aIkeData,
                            TVPNAddress& aInternalAddress,
                            TRequestStatus& aStatus );
    
    /**
     * Cancels negotiate request. DeleteSession() method needs to be called
     * after this method to delete session.
     */
    void CancelNegotiateWithHost();
    
    /**
     * Deletes session. IKE/IPSec SA:s are deleted.
     *
     * @param aSilentClose Specified if a silent close in question (Delete
     * payloads not transmitted to peer)
     * @param aStatus Completion status (returned)
     */
    void DeleteSession( const TBool aSilentClose,
                        TRequestStatus& aStatus );
    
    /**
     * Cancels deletion requests. IKE/IPSec SA:s are deleted.
     */
    void CancelDeleteSession();
    
    void NotifyError( TRequestStatus& aStatus );
    
    /**
     * Cancels error notification request.
     */
    void CancelNotifyError();
    
    
    /**
     * Requests notification about change of internal address.
     *
     * @param aStatus Completion status (returned)
     */
    void NotifyInternalAddressChanged( TVPNAddress& aInternalAddress,
                                       TRequestStatus& aStatus ); 
    
    /**
     * Cancels internal address notification request.
     */
    void CancelNotifyInternalAddressChanged();
    
    void IkeMessageReceivedL( const ThdrISAKMP& aIkeMessage,
                              const TInetAddr &aRemote,
                              TUint16 aLocalPort );
        
    void LinkNegotiation( CIkev2Negotiation* aNegotiation );
    void RemoveNegotiation( CIkev2Negotiation* aNegotiation );

    CIkev2SA* FindIkev2SA( TUint32 aSAId,
                           TInt aRequiredState,
                           TInt aNewState );
    TBool UpdateIkev2SAL( TIkev2SAData* aIkev2SAData,
                          TIkeV2IpsecSAData* aIpsecSAData );
    TIkeV2IpsecSAData* FindIpsecSAData( TUint32 aSAId,
                                        const TDesC8& aSpi,
                                        TBool aInbound );
    void DeleteIkev2SA( TUint32 aSAId );
        
    TUint32 GetSAId();
    TBool CreateIkev2SAL( TIkev2SAData& aIkev2SAData );
    
    void IkeSaCompleted( TInt aStatus,
                         TVPNAddress& aInternalAddress );
    void VirtualIpChanged( TVPNAddress& VirtualIp );
    
    void StartResponding();
    void StopResponding();
    
    void DeleteIpsecSAData( TUint32 aSAId,
                            const TDesC8& aSpi,
                            TBool aInbound );
    
    void IkeSaDeleted( TInt aStatus );
    
    CIpsecSaSpecList* GetIPsecSaSpecListL( const TInetAddr& aLocalAddr,
                                           const TInetAddr& aLocalMask, 
                                           const TInetAddr& aRemoteAddr,
                                           const TInetAddr& aRemoteMask,
                                           TInt aProtocol );

    TBool InheritIpsecSas( TUint32 aDstSAId,
                           TUint32 aSrcSAId );
    
    TUint32 VpnInterfaceIndex() const; 
    TBool RemoteAddrChanged( TIkev2SAData* aIkev2SAData,
                             TInetAddr& aNewIp );
    
    void KeepAliveIkeSAL( TIkev2SAData* aIkev2SAdata );
    CIkev2Negotiation* FindNegotiation( TUint32 aSAId,
                                        TInt aRequiredState );
    
    TBool DeleteIkeSAL( TIkev2SAData* aIkev2SAdata,
                        TBool aNormal );
    void RekeyIkeSAL( TIkev2SAData* aIkev2SAdata );
    
    void DeleteIpsecSA( const TUint32 aSPI,
                        const TInetAddr& aSrc,
                        const TInetAddr& aDst,
                        const TUint8 aProtocol );
    
    void AddSAL( const TIpsecSAData& aSAData );
    void UpdateSAL( const TIpsecSAData& aSAData );
    
    void PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage );    
    
    TBool MatchDestinationAddress( const TInetAddr& aDestAddr ) const;
    
// from base class MIkev2ReceiverCallback
    
    /**
     * From MIkev2ReceiverCallback.
     * Notification about received IKE message.
     * @param aIkeMsg IKE message
     * @param aSrcAddr Source IP address/port
     * @param aLocalPort Local port
     */
    void IkeMsgReceived( const ThdrISAKMP& aIkeMsg,
                         const TInetAddr& aSrcAddr,
                         TInt aLocalPort);
    
    /**
     * From MIkev2ReceiverCallback.
     * Notification about receive error.
     * @param aStatus Error value
     */
    void ReceiveError( TInt aError );            
    
    
// from base class MIkev2DeactivationTimerCallback
    
    /**
     * From MIkev2DeactivationTimerCallback
     * Notification about deactivation timeout.
     */
    void DeactivationTimeout();
    
    
private:
    CIkev2PluginSession( TUint32 aVpnIapId,
                         TUint32 aVpnNetId,
                         TUint32 aVpnInterfaceIndex,
                         MIkeDataInterface& aDataInterface,
                         CIkev2PlugIn& aPlugin,  
                         CPFKeySocketIf& aPfKeySocketIf,
                         CIpsecPolicyUtil& aIpsecPolicyUtil,
                         MKmdEventLoggerIf& aEventLogger,
                         MIkeDebug& aDebug );
    void ConstructL();
    
    
    void DoNegotiateWithHostL( const CIkeData& aIkeData );
    void DoDeleteIkeSAExhangeL( TIkev2SAData& aIkev2SAdata );
    
    TIkev2SAData* FindIkev2SAData( TUint32 aSAId,
                                   TInt aRequiredState,
                                   TInt aNewState );
    TBool CheckIkeMessageHeader( const ThdrISAKMP& aIkeMessage,
                                 TUint32& NegotiationId );
    void PfkeyMessageReceivedL( const TPfkeyMessage& aPfkeyMessage );
    
    /**
     * Handles completion of client's negotiate request.
     * @param aStatus Status
     */
    void DoCompleteNegotiateWithHost( TInt aStatus );
    
    /**
     * Handles completion of client's delete session request.
     * @param aStatus Status
     */
    void DoCompleteDeleteSession( TInt aStatus );    
    
    /**
     * Handles completion of client's notify error request.
     * @param aStatus Status
     */
    void DoCompleteNotifyError( TInt aStatus );    
    
    /**
     * Cancels active operations.
     */    
    void DoCancelActiveOperations();
    
    /**
     * Cancels data transfer.
     */    
    void DoCancelDataTransfer();
    
private: // data	
	
    TUint32             iVpnIapId;
    TUint32             iVpnNetId;
    MIkeDataInterface&  iDataInterface;
    CIkev2PlugIn&       iPlugin;   
    CPFKeySocketIf&     iPfKeySocketIf;
    CIpsecPolicyUtil&   iIpsecPolicyUtil;
    MKmdEventLoggerIf&  iEventLogger;
    MIkeDebug&          iDebug;  
    
    TUint32              iSAIdSeed;        
    TUint32              iVpnInterfaceIndex;
    
    
    TRequestStatus*      iClientStatusNegotiate;
    TVPNAddress*         iInternalAddress; //Not owned by this class
    CIkeData*            iIkeData;
    
    CIkev2Negotiation*   iFirstNegotiation;
    CIkev2SA*            iFirstIkev2SA;
    
    TRequestStatus*      iClientStatusDelete;
    
    CIkev2Receiver*         iReceiver;   
    CIkev2MessageSendQueue* iMessageSendQue;    
    
    TRequestStatus*      iClientStatusInternalAddressChange;
    TVPNAddress*          iChangedInternalAddress;
    
    TUint32              iCurrIkeSaRespCount;
    
    TRequestStatus*      iClientStatusNotifyError;
        
    CIkev2DeactivationTimer* iDeactivationTimer;
    TBool                 iActivated;
    };                                         


#endif //C_IKEV2PLUGINSESSION_H
