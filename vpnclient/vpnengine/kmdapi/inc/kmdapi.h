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
* Description:   KMD api
*
*/

#ifndef R_KMD_H
#define R_KMD_H

#include "vpnmandefs.h"

//
// KMD API Error codes
// NOTE! The error code values below MUST be kept in sync with
// the corresponding error code values defined together by
// vpnapi/data/vpnerr.rss and vpnapi/data/vpnerr.ra
//
const TInt KKmdTooWeakCryptoLib             = -5253;
const TInt KKmdNoAlgorithmsFile             = -5254;
const TInt KKmdIkePolicyFileErr             = -5255;
const TInt KKmdIkeNegotFailed               = -5256;
const TInt KKmdIkeNoResponseErr             = -5257;
const TInt KKmdIkeNoProposalErr             = -5258;
const TInt KKmdIkeAuthFailedErr             = -5259;
const TInt KKmdIkePeerAuthFailed            = -5260;
const TInt KKmdIkeNoCertFoundErr            = -5261;
const TInt KKmdIkeNoPolicyErr               = -5262;

/**
 *  VPN real connection parameters.
 *
 *  @lib kmdapi.lib
 */
class TVpnRealConnectionParams
    {
public:    
    /**
     * Real IAP Id.
     * Own.
     */
    TUint32 iRealIapId;
    
    /**
     * Real NET Id.
     * Own.
     */
    TUint32 iRealNetId;
    };

/**
 *  Enumeration of VPN connection stopping types.
 *
 *  @lib kmdapi.lib
 */
class TKmdStopConnection
    {	
public:	
    enum TType
        {           
        ENormal = 0,
        EForced 
        };
    };

/**
 *  KMD API.
 *  A handle to KMD server.
 *
 *  @lib kmdapi.lib
 */
class RKMD : public RSessionBase
    {
public:
        
	/**
     * Creates session to KMD server. If server is not running, it is started.
     */
    IMPORT_C TInt Connect();

	/**
	 * Start a real network connection.
	 * Outstanding request can be cancelled any time by calling
	 * CancelStartConnection(). 
	 * 
	 *  @param aVpnIapId Used VPN IAP Id
	 *  @param aRealConfig If request completes with KErrNone, contains IAP
	 *                     and NET Ids of used real network connecetion
	 *                     (returned).
	 *  @param aStatus Completes with KErrNone if real network connection 
	 *                 is successfully opened (returned).
	 * 
     */
    IMPORT_C void StartRealConnection( TUint32 aVpnIapId, 
                                       TPckg<TVpnRealConnectionParams>& aRealConfig,
                                       TRequestStatus& aStatus ) const;
        
	/**
     * Cancel starting of real network connection.
     */
    IMPORT_C void CancelStartRealConnection() const;        
    	

	/**
     * Asynchronous activation command to activate VPN connection (IKE policy).
     * If there is no virtual IP address protocols configured the activate
     * request completes immediatelly.
     * 
	 *  @param aVpnIapId Used VPN IAP Id
	 *  @param aVpnIfName VPN interface name 
     *  @param aIkeConf IKE policy data 
     *  @param aVPNAddress Contains the IP config of the VPN connection, when 
     *                     request completes (returned).
     *  @param aStatus Completion status, KErrNone if successfull (returned).
     */
    IMPORT_C void Activate( TUint32 aVpnIapId,
                            const TDesC& aVpnIfName,
                            const TDesC8& aIkeConf, 
                            TVPNAddressPckg& aVPNAddress,
                            TRequestStatus& aStatus ) const;

	/**
     * Cancels ongoing activate request.
     */
    IMPORT_C void CancelActivate() const;
	
	/**
     * Stops VPN connection.
     * @param aVpnIapId Used VPN IAP
   	 * @param aType: Specifies stopping type.
	 *               ENormal = All IKE and IPSEC SA:s related to VPN connection
	 *               shall be deleted (= ISAKMP delete payload is transmitted
	 *               to SA peer)
	 *               EForced = Delete payload is not transmitted to SA peer.
     */
    IMPORT_C TInt StopVpnConnection( TUint32 aVpnIapId,
                                     TKmdStopConnection::TType aType = TKmdStopConnection::ENormal );

	/**
     * Resolve an IP address from FQDN. 
     * There can be only one outstanding query per session.
     * @param aVpnIapId Used VPN IAP Id                       
	 * @param aFqdn: Domain Name to be resolved.
     * @param aResult: Result of name resolution (returned)
     * @param aStatus: Completion status (returned)
     */
    IMPORT_C void ResolveAddress( TUint32 aVpnIapId, 
                                  const TDesC& aFqdn,
                                  TNameEntry& aResult,
                                  TRequestStatus& aStatus ) const;
	/**
     * Cancel outstanding resolve address query.
     */        
    IMPORT_C void CancelResolve() const;	    
    };

#endif // R_KMD_H
