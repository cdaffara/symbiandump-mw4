/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKEv1 plugin session
*
*/


#ifndef C_IKEV1PLUGINSESSION_H
#define C_IKEV1PLUGINSESSION_H

#include <in_sock.h>

#include "vpnmandefs.h"
#include "ikemsgheader.h"
#include "ikepluginsessionif.h"
#include "ikev1receiver.h"
#include "ikev1sender.h"
#include "ikesendqueueitem.h"

// FORWARD DECLARATIONS
class CIkev1Plugin;
class CIkev1Dialog;
class CIkev1Negotiation;
class CIkev1PluginSession;
class CInternalAddress;
class TDeleteISAKMP;
class CAuthDialogInfo;
class CIkev1SA;
class TIkev1SAData;
class CSARekeyInfo;
class TIpsecSPI;
class MIkeDebug;
class ThdrISAKMP;
class TPfkeyMessage;
class TIpsecSAData;
class MKmdEventLoggerIf;
class CIpsecSaSpecList;
class CPFKeySocketIf;

NONSHARABLE_CLASS(CIkev1PluginSession) : public CBase,
                                         public MIkePluginSessionIf,
                                         public MIkev1ReceiverCallback,
                                         public MIkev1SenderCallback
                                         
    {
public:
    /**
     * Two-phased constructor.
     * @param aVpnIapId VPN IAP id
     * @param aVpnNetId VPN NET id
     * @param aVpnInterfaceIndex VPN interface index
     * @param aDataInterface Data interface
     * @param aPlugin IKE plugin
     * @param aPFKeySocketIf PF key socket interface
     * @param aDebug Debug trace interface
     */
    static CIkev1PluginSession* NewL( TUint32 aVpnIapId,
                                      TUint32 aVpnNetId,
                                      TUint32 aVpnInterfaceIndex,
                                      MIkeDataInterface& aDataInterface,
                                      CIkev1Plugin& aPlugin,
                                      CPFKeySocketIf& aPFKeySocketIf,
                                      MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CIkev1PluginSession();
            
    /**
     * Handles IKE SA deletion request.
     * @param aIkev1SaData IKEv1 SA data
     * @param aSilentClose Silent close
     */
    TBool DeleteIkeSA( TIkev1SAData* aIkev1SAdata,
                       TBool aSilentClose = EFalse );
        
    /**
     * Handles IKE SA rekeying request.
     * @param aIkev1SaData IKEv1 SA data
     * @param aSaRekeyInfo Rekey info
     */
    void RekeyIkeSAL( TIkev1SAData* aIkev1SaData,
                      CSARekeyInfo* aSaRekeyInfo );
        
    /**
     * Handles IKE SA keepalive request.
     * @param aIkev1SaData IKEv1 SA data
     */
    void KeepAliveIkeSAL( TIkev1SAData* aIkev1SaData );
    
    /**
     * Creates IKE SA.
     * @param aIkev1SaData IKEv1 SA data
     * @param aSaRekeyInfo Rekey info
     */
    void CreateIkev1SAL( TIkev1SAData& aIkev1SaData,
                         CSARekeyInfo* aSaRekeyInfo );                

    /**
     * Updates IKE SA.
     * @param aSaId SA id
     * @param aExpired Expired
     * @param aIkev1SaData IKEv1 SA data
     */
    void UpdateIkev1SAL( TUint32 aSaId,
                         TBool aExpired,
                         TIkev1SAData* aIkev1SaData = NULL );
    
    /**
     * Expires IKE SA.
     * @param aSaId SA id
     */
    void ExpireIkev1SA( TUint32 aSaId );         
    
    /**
     * Removes IKE SA.
     * @param aIkev1Sa IKE SA
     * @param aStatus Error status
     */
    void RemoveIkeSA( CIkev1SA* aIkev1Sa,
                      TInt aStatus );    
    
// IKE SA find methods
    
    CIkev1SA* FindIkev1SA();
    CIkev1SA* FindIkev1SA( const TCookie& aCookie_I,
                           const TCookie& aCookie_R );
    CIkev1SA* FindIkev1SAWithId( TUint32 aSaId );
    CIkev1SA* FindIkev1SA( const TInetAddr& aAddr );
    CIkev1SA* FindIkev1SA( const TInetAddr& aAddr,
                           TUint32 aInboundSpi );
    
    TIkev1SAData* FindIkev1SAData();
    TIkev1SAData* FindIkev1SAData( const TCookie& aCookie_I,
                                   const TCookie& aCookie_R );
    TIkev1SAData* FindIkev1SAData( TUint32 aSaId );
    TIkev1SAData* FindIkev1SAData( const TInetAddr& aAddr,
                                   TUint32 aInboundSpi );
    TIkev1SAData* FindIkev1SADataWithAddr( const TInetAddr& aAddr );

    /**
     * Handles IPsec SA deletion request.
     * @param aIkev1SaData IKEv1 SA data
     * @param aIpsecSpi IPsec SPI
     */
    void DeleteIpsecSAL( TIkev1SAData* aIkev1SaData,
                         TIpsecSPI* aIpsecSpi );
    
    /**
     * Deletes IPsec SAs.
     * @param aSaId SA id
     */
    void DeleteIpsecSAs( TUint32 aSAId );
    
    /**
     * Deletes IPsec SPI.
     * @param aSaId SA id
     * @param aSpi SPI
     * @param aInbound Inbound
     */
    TBool DeleteIpsecSpi( TUint32 aSaId,
                          TUint32 aSpi,
                          TBool aInbound = EFalse );
        
    /**
     * Adds IPsec SPI to IKE SA.
     * @param aSaId SA id
     * @param aIpsecSpi IPsec SPI
     */
    void AddIpsecSPIToSAL( TUint32 aSaId,
                           TIpsecSPI& aIpsecSpi );

    /**
     * Returns dialog anchor.
     */
    CIkev1Dialog** DialogAnchor();
    
    /**
     * Returns debug trace interface.
     */
    MIkeDebug& Debug();
    
    /**
     * Gets SA id.
     */
    TUint32 GetSAId();     
          
    /**
     * Deletes ISAKMP SAs.
     * @param aDeletePayload Delete payload which identifies ISAKMP/IKE SA.
     * @param aInfoNegotiation Negotiation object used by CIkev1InfoNegotiation
     *                         object.
     */
    void DeleteISAKMPSAsL( TDeleteISAKMP* aDeletePayload,
                           const CIkev1Negotiation& aInfoNegotiation );
    
    /**
     * Requests sending of IKE message.
     * @param aIkeMsg IKE message
     * @param aDestAddr Destination IP address/port
     * @param aUseNatPort NAT used or not
     */
    void SendIkeMsgL( const TDesC8& aIkeMsg,
                      TInetAddr& aDestAddr,
                      TBool aUseNatPort );

    /**
     * Gets local IP address.
     * @param aAddr Local IP address (returned)
     * @return Error value
     */
    TInt GetLocalAddress( TInetAddr& aAddr );
            
    /**
     * Sends NAT keep-alive packet.
     * @param aDestAddr Destination IP address/port
     * @param Keep-alive data
     * @param aDscp DSCP value
     */
    void SendNatKeepAliveL( TInetAddr& aDestAddr,
                            const TDesC8& aData,
                            TUint8 aDscp );
    
    /**
     * Sends Nokia NAT keep-alive packet.
     * @param aDestAddr Destination IP address/port
     * @param Keep-alive data
     * @param aDscp DSCP value
     */
    void SendNokiaNatKeepAliveL( TInetAddr& aDestAddr,
                                 const TDesC8& aData,
                                 TUint8 aDscp );        
            
    /**
     * Handles completion of IKE SA establishment.
     * @param aStatus Completion status
     * @param aInternalAddress Internal address. Ownership transferred.
     */
    void IkeSaCompleted( TInt aStatus, 
                         CInternalAddress* aInternalAddress = NULL );
    
    /**
     * Deletes negotiation object.
     * @param aNegotiation Negotiation
     */
    void DeleteNegotiation( CIkev1Negotiation* aNegotiation );
    
    // Negotiation linking and finding methods
    void LinkNegotiation( CIkev1Negotiation* aNegotiation );
    CIkev1Negotiation* FirstNegotiation();
    CIkev1Negotiation* FindNegotiation( TUint32 aSaId );
    CIkev1Negotiation* FindNegotiation( TCookie aInit,
                                        TCookie aResp,
                                        TUint8 aExchange,
                                        TUint32 aMsgId ); 
    void RemoveNegotiation( CIkev1Negotiation* aNegotiation );    

    /**
     * Handles completion of authentication dialog processing.
     * @param aUserInfo User info
     * @return Error value
     */
    TInt AuthDialogCompletedL( CAuthDialogInfo* aUserInfo );  
    
    /**
     * Handles change of internal address.
     * @param aInternalAddr Internal address
     * @return Informs if internal address differs from existing internal address
     */
    TBool InternalAddressChangedL( const CInternalAddress& aInternalAddr );
    
    /**
      * Gets acceptable IPsec policies for specified selectors.
      * 
      * @param aLocalAddr IP address, including possible port, of the local end selector
      * @param aLocalMask Local end selector mask
      * @param aRemoteAddr IP address, including possible port, of the remote end selector
      * @param aRemoteMask Remote end selector mask
      * @param aProtocol Protocol id
      * @param aVpnNetId VPN net id
      */
     CIpsecSaSpecList* GetIpseSaSpecListLC( const TInetAddr& aLocalAddr, const TInetAddr& aLocalMask, 
                                            const TInetAddr& aRemoteAddr, const TInetAddr& aRemoteMask,
                                            TInt aProtocol );     
                            
    /**
     * Handles fatal error.
     * @param aStatus Error status
     */
    void HandleError( TInt aStatus );
    
    /**
     * Returns error status.
     * @return Error status
     */
    TInt ErrorStatus();
    
    /**
     * Sets error status.
     * @param aStatus Error status
     */
    void SetErrorStatus( TInt aStatus );
    
    /**
     * Returns VPN IAP id.
     * @return VPN IAP id
     */
    TUint32 VpnIapId();    

    /**
     * Returns VPN interface index.
     */
    TUint32 VpnInterfaceIndex();
    
    /**
     * Returns IKE policy data.
     * @return IKE policy data
     */
    CIkeData& IkeData();

    /**
     * Returns UID.
     * @return UID
     */
    TUint32 Uid();

    /**
     * Returns event logger interface.
     * @return Eveng logger interface
     */
    MKmdEventLoggerIf& EventLogger();
    
    /**
     * Returns internal address (NULL if does not exist).
     * @return Internal address. Ownership transferred.
     */
    CInternalAddress* InternalAddressL();    
    

// PFKEY related methods
    
    /**
     * Matches destination address to remote address in IKE policy data.
     * @param aDestAddr Destination IP address
     * @return ETrue if matches. 
     */
    TBool MatchDestinationAddress( const TInetAddr& aDestAddr );
    
    /**
     * Handles received PFKEY message.
     * @param aPfkeyMessage PFKEY message
     */
    void PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage );

// Methods to build and send PFKEY API primitives to IPsec
    
    void GetIpsecSPI( TUint8 aType,
                      TUint32 aSeq,
                      TInetAddr& aSrc, 
                      TInetAddr& aDst );
    
    void AcquireSAError( TIpsecSAData& aSAData,
                         TInt aError );
    
    void UpdateSAL( TIpsecSAData& aSaData );
    
    void AddSAL( TIpsecSAData& aSaData );
    
    void DeleteIpsecSA( TIpsecSPI& aIpsecSpi );
    
    void DeleteIpsecSA( TUint32 aSPI,
                        TInetAddr& aSrc, 
                        TInetAddr& aDst,
                        TUint8 aProtocol );
        
// from base class MIkePluginSessionIf
    
    /**
     * From MIkePluginSessionIf.
     * Starts negotiation with a peer.
     * @param aIkeData IKE policy data
     * @param aInternalAddress Internal address (returned)
     * @param aStatus Completion status (returned) 
     */
    void NegotiateWithHost( const CIkeData& aIkeData,
                            TVPNAddress& aInternalAddress,
                            TRequestStatus& aStatus );
    
    /**
     * From MIkePluginSessionIf.
     * Cancels negotiate request. DeleteSession() method needs to be called
     * after this method to delete session.
     */
    void CancelNegotiateWithHost();
    
    /**
     * From MIkePluginSessionIf.
     * Deletes session. IKE/IPSec SA:s are deleted.
     * @param aSilentClose Specified if a silent close in question (Delete
     * payloads not transmitted to peer)
     * @param aStatus Completion status (returned)
     */
    void DeleteSession( const TBool aSilentClose,
                        TRequestStatus& aStatus );
    
    /**
     * From MIkePluginSessionIf.
     * Cancels deletion requests. IKE/IPSec SA:s are deleted.
     */
    void CancelDeleteSession();
            
    /**
     * From MIkePluginSessionIf.
     * Requests notification about error condition.
     * @param aStatus Completion status (returned)
     */
    void NotifyError( TRequestStatus& aStatus );
    
    /**
     * From MIkePluginSessionIf.
     * Cancels error notification request.
     */
    void CancelNotifyError();
    
    /**
     * From MIkePluginSessionIf.
     * Requests notification about change of internal address.
     * @param aInternalAddress Internal address (returned)
     * @param aStatus KErrNone. Error condition needs to be indicated via
     *                NotifyError() method. (returned)
     */
    virtual void NotifyInternalAddressChanged( TVPNAddress& aInternalAddress,
                                               TRequestStatus& aStatus );
    
    /**
     * From MIkePluginSessionIf.
     * Cancels internal address change notification request.
     */
    void CancelNotifyInternalAddressChanged();
        
// from base class MIkev1ReceiverCallback

    /**
     * From MIkev1ReceiverCallback.
     * Notification about received IKE message.
     * @param aIkeMsg IKE message
     * @param aSrcAddr Source IP address/port
     * @param aLocalPort Local port
     */
    void IkeMsgReceivedL( const ThdrISAKMP& aIkeMsg,
                          const TInetAddr& aSrcAddr,
                          TInt aLocalPort );
    
    /**
     * From MIkev1ReceiverCallback.
     * Notification about receive error.
     * @param aStatus Error value
     */
    void ReceiveError( TInt aError );
    
    
// from base class MIkev1SenderCallback   
    
    /**
     * From MIkev1SenderCallback.
     * Notification about completion sending.
     * @param aStatus Completion status
     */
    void SendUdpDataCompleted( TInt aStatus );
    
private:
    
    CIkev1PluginSession( TUint32 aVpnIapId,
                         TUint32 aVpnNetId,
                         TUint32 aVpnInterfaceIndex,
                         MIkeDataInterface& aDataInterface,
                         CIkev1Plugin& aPlugin,
                         CPFKeySocketIf& aPFKeySocketIf,
                         MIkeDebug& aDebug );
    void ConstructL();
    
    /**
     * Handles received PFKEY message.
     * @param aPfkeyMessage PFKEY message
     */
    void DoPfkeyMessageReceivedL( const TPfkeyMessage& aPfkeyMessage );
    
    /**
     * Deletes IKE SAs.
     * @param aSilentClose silent close
     * @return Deactivation started or not.
     */
    TBool DeleteSAsWithHost( TBool aSilentClose );
        
    /**
     * Requests sending of UDP data.
     * @param aUdpData UDP data. Ownership transferred.
     * @param aDestAddr Destination IP address/port
     * @param aLocalPort Local port
     * @param aDscp DSCP value
     */
    void DoSendUdpDataL( HBufC8* aUdpData,
                         const TInetAddr& aDestAddr,
                         TInt aLocalPort,
                         TUint8 aDscp );
    /**
     * Sends UDP data.
     * @param aUdpData UDP data. Ownership transferred.
     * @param aDestAddr Destination IP address/port
     * @param aLocalPort Local port
     * @param aDscp DSCP value
     */
    void DoSendUdpData( HBufC8* aUdpData,
                        const TInetAddr& aDestAddr,
                        TInt aLocalPort,
                        TUint8 aDscp );  
    
    /**
     * Handles starting of negotiation with a peer.
     * @param aIkeData IKE policy data
     */
    void DoNegotiateWithHostL( const CIkeData& aIkeData );
 
    /**
     * Handles fatal error.
     * @param aError Error status
     */
    void DoHandleError( TInt aError );
    
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
     * Handles completion of client's notify internal address change request.
     * @param aStatus Status
     */
    void DoCompleteInternalAddressChanged( TInt aStatus );
    
    /**
     * Cancels data transfer.
     */    
    void DoCancelDataTransfer();
    
    /**
     * Empties send queue.
     */
    void DoEmptySendQueue();
    
private: // data
    
    /**
     * VPN IAP id.
     * Own.
     */
    TUint32                     iVpnIapId;

    /**
     * VPN NET id.
     * Own.
     */
    TUint32                     iVpnNetId;

    /**
     * VPN interface index.
     * Own.
     */
    TInt                        iVpnInterfaceIndex;

    /**
     * IKE policy data.
     * Own.
     */
    CIkeData*                   iIkeData;

    /**
     * IKEv1 negotiations.
     * Own.
     */
    CIkev1Negotiation*          iFirstNegotiation;

    /**
     * Dialog wait queue. Used by CIkeDialog class.
     * Own.
     */
    CIkev1Dialog*               iDialogWaitQueue;  

    /**
     * SA id seed.
     * Own.
     */
    TUint32                     iSAIdSeed;            
    
    /**
     * IKE message send queue.
     * Own.
     */
    RArray<TIkeSendQueueItem>   iSendQueue;
    
    /**
     * IKEv1 SAs.
     * Own.
     */
    RPointerArray<CIkev1SA>     iIkev1SAs;
    
    /**
     * Local IP address.
     * Own.
     */
    TInetAddr                   iLocalAddr;
    
    /**
     * Receiver.
     * Own.
     */
    CIkev1Receiver*             iReceiver;
    
    /**
     * Sender.
     * Own.
     */
    CIkev1Sender*               iSender;
    
    /**
     * Internal address.
     * Own.
     */
    CInternalAddress*           iInternalAddress;
    
    /**
     * Error status.
     * Own.
     */
    TInt                        iErrorStatus;    
    
    /**
     * Client's negotiate requests status.
     * Not own.
     */
    TRequestStatus*             iClientStatusNegotiate;
    
    /**
     * Client's internal address variable for negotiate request.
     * Not own.
     */
    TVPNAddress*                iClientIaNegotiate;

    /**
     * Client's delete session requests status.
     * Not own.
     */
    TRequestStatus*             iClientStatusDelete;
    
    /**
     * Client's notify error requests status.
     * Not own.
     */
    TRequestStatus*             iClientStatusNotifyError;
    
    /**
     * Client's notify IA change requests status.
     * Not own.
     */
    TRequestStatus*             iClientStatusNotifyIaChange;
    
    /**
     * Client's internal address variable for notify request.
     * Not own.
     */
    TVPNAddress*                iClientIaNotify;
        
    /**
     * Data interface.
     * Not own.
     */
    MIkeDataInterface&          iDataInterface;
    
    /**
     * IKEv1 plugin.
     * Not own.
     */
    CIkev1Plugin&               iPlugin;
    
    /**
     * VPN PF key socket.
     * Not own.
     */
    CPFKeySocketIf&             iPFKeySocketIf;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;    
    };

#endif // C_IKEV1PLUGINSESSION_H
