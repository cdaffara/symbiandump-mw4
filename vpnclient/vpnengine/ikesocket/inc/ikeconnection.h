/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE connection
*
*/


#ifndef C_IKECONNECTION_H
#define C_IKECONNECTION_H

#include <e32base.h>
#include <in_sock.h>
#include <extendedconnpref.h>
#include "connobserver.h"
#include "datatransfer.h"
#include "ikeconnectioninterface.h"

// FORWARD DECLARATIONS
class CConnObserver;
class CDataTransfer;
class CLocalAddressResolver;
class MIkeDebug;
class RSocketServ;

/**
 *  IKE socket connection class.
 *
 *  This class provides functionality for managing VPN AP's real network
 *  connection. This class implements IKE socket connection interface
 *  (CIkeConnectionInterface).
 * 
 *  This class owns data transfer object, which implements data interface
 *  (MIkeDataInterface). Possible fatal data transfer errors are handled
 *  in this class.
 * 
 *  @lib ikesocket.lib
 */
NONSHARABLE_CLASS( CIkeConnection ) : public CIkeConnectionInterface,
                                      public MDataTransferCallback,
                                      public MConnObserverCallback
    {
public:    
    /**
     * Two-phased constructor.
     * @param aDebug Debug trace interface
     */
    static CIkeConnection* NewL( MIkeDebug& aDebug );
    
    /**
     * Destructor.
     */
    ~CIkeConnection();
        
    /**
     * Opens data interface.
     *
     * @param aIkeMajorVersion IKE major version
     * @param aIpVersion IP protocol version
     * @return Data interface
     */
    MIkeDataInterface& OpenDataInterfaceL( const TIkeMajorVersion aIkeMajorVersion,
                                           const TIpVersion aIpVersion );
    
// from base class MIkeConnectionInterface
    
    void StartConnection( const TUint32 aIapId,
                          const TUint32 aSnapId,
                          TRequestStatus& aStatus,
                          const TBool aForcedRoaming = EFalse );
    void CancelStartConnection();
    void StopConnection();
    void ResolveFQDNAddress( const TDesC& aFQDN,
                             TNameEntry& aNameEntry,
                             TRequestStatus& aStatus );
    void CancelResolveFQDNAddress();
    void NotifyDisconnect( TRequestStatus& aStatus );
    void CancelNotifyDisconnect();
    TUint32 IapId() const;
    TUint32 NetId() const;
    TUint32 SnapId() const;
    TInt GetLocalAddress( const TIpVersion aIpVersion,
                          TInetAddr& aLocalIp );
    
// from base class MDataTransferCallback
    
    /**
     * Notification about fatal data transfer error.
     * 
     * @param aError Error value
     * @param aErrorType Error type
     */        
    void DataTransferError( const TInt aError,
                            const TErrorType aErrorType );
    
// from base class MConnObserverCallback
    
    /**
     * Notification about link disconnection.
     * 
     * @param aStatus Link disconnection status
     */
    void LinkDisconnected( const TInt aStatus );
    
private:
    
    enum TConnectionState
        {
        EIdle,
        EConnecting,
        EResolvingFQDN,
        EConnected        
        };
    
    CIkeConnection( MIkeDebug& aDebug );
    
    void ConstructL();
        
    /**
     * Creates connection preferences.
     * @param aIapId IAP id
     * @param aSnapId SNAP id
     * @param aForcedRoaming Indicates whether forced roaming is enabled or
     *                       disabled for SNAP 
     */
    void CreatePreferencesL( const TUint32 aIapId,
                             const TUint32 aSnapId,
                             const TBool aForcedRoaming );
    
    /**
     * Cleans connection preferences.
     */
    void CleanPreferences();
    
    /**
     * Updates IAP id and NET id.
     */
    void UpdateRealIapData();
    
    /**
     * Handles completion of asynchronous request in EConnecting state. 
     */
    void DoStateAfterConnecting();
    
    /**
     * Handles completion of asynchronous request in EResolvingFQDN state. 
     */
    void DoStateAfterResolvingFQDN();
    
    /**
     * Implements cancellation of connection starting. 
     */
    void DoCancelStartConnection();
    
    /**
     * Implements cancellation of FQDN address resolving. 
     */
    void DoCancelResolveFQDNAddress();        
    
// from base class CActive
        
    /**
     * From CActive.
     * Handles an active object's request completion event.
     */
    void RunL();
    
    /**
     * From CActive.
     * Implements cancellation of an active request.
     */ 
    void DoCancel();    
    
private: // data
    
    /**
     * Connection state.
     * Own.
     */
    TConnectionState        iState;
    
    /**
     * Socket server.
     * Own.
     */
    RSocketServ             iSocketServer;
    
    /**
     * Network connection.
     * Own.
     */
    RConnection             iConnection;
    
    /**
     * Host resolver.
     * Own.
     */
    RHostResolver           iResolver;    
    
    /**
     * IP version.
     * Own.
     */
    TIpVersion              iIpVersion;
    
    /**
     * IAP id.
     * Own.
     */
    TUint32                 iIapId;

    /**
     * NET id.
     * Own.
     */
    TUint32                 iNetId;
    
    /**
     * SNAP id.
     * Own.
     */
    TUint32                 iSnapId;

    /**
     * Extended connection preferences.
     * Own.
     */
    TExtendedConnPref       iExtendedPrefs;
    
    /**
     * Connection preferences list.
     * Own.
     */
    TConnPrefList           iConnPrefList;
    
    /**
     * Pointer to client's request status. Used for starting connection and
     * resolving FQDN address.
     * Not own.
     */
    TRequestStatus*         iClientStatus;
    
    /**
     * Pointer to client's request status. Used for requesting disconnect
     * notification.
     * Not own.
     */
    TRequestStatus*         iClientStatusNotifyDisconnect;
    
    /**
     * Data transfer object.
     * Own.
     */
    CDataTransfer*          iDataTransfer;
    
    /**
     * Link status observer.
     * Own.
     */
    CConnObserver*          iLinkObserver;
    
    /**
     * Local address resolver.
     * Own.
     */
    CLocalAddressResolver*  iLocalAddressResolver;    
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&              iDebug;
    
    };

#endif // C_IKECONNECTION_H
