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
* Description:   Event definitions for SIT
*
*/



#ifndef EVENTDEFSSIT_H
#define EVENTDEFSSIT_H

/*
 * The following event types are "SIT events". When a client
 * makes a SIT event listening request, the Event Mediator
 * uses the SIT to fulfill the request. 
 *
 * When the SIT completes the task that is needed to fulfill
 * a certain SIT event listening request, the SIT reports an
 * event to the Event Mediator. The event report contains
 * event data that the Event Mediator passes further to the
 * listeners of the SIT event.
 *
 * EGetProtocolVersionEvent
 *     Event specification: VPN policy ID
 *     Event data:          Status of the task
 *                          Protocol type enumeration
 *
 * EStartVpnConnEvent
 *     Event specification: VPN IAP ID
 *                          VPN network ID
 *                          Real IAP ID
 *                          Real network ID
 *                          VPN policy ID
 *                          VPN NIF name
 *     Event data:          Status of the task
 *                          IKE policy handle
 *                          IPSec policy handle
 *                          VPN internal address information
 *
 * ECloseVpnConnEvent
 *     Event specification: IKE policy handle
 *                          IPSec policy handle
 *     Event data:          Status of the task
 *
 * EStartRealIapConnEvent
 *     Event specification: Real IAP ID
 *                          Real network ID
 *     Event data:          Status of the task
 *
 * EObserveRealIapConnEvent
 *     Event specification: Real IAP ID
 *                          Real network ID
 *     Event data:          Status of the task
 *                          Real IAP connection event
 *
 * The following event types are SIT related event types that are
 * used by the SIT for fetching task related information from the
 * Event Mediator. These event types are meant only for internal
 * use between the SIT and the Event Mediator.
 *
 * ETaskArrivedEvent
 *     Event specification: -
 *     Event data:          Event type of the task request
 *                          that requires fulfilling
 *                          Event specification ID of the task
 *                          request that requires fulfilling
 *
 * EFetchTaskInfoEvent
 *     Event specification: Event type of the task request
 *                          that requires fulfilling
 *                          Event specification ID of the task
 *                          request that requires fulfilling
 *     Event data:          One of the event specifications
 *                          listed below (depends on the event
 *                          type)
 *
 */

/**
 * Protocol version information used with EGetProtocolVersionEvent
 */
enum TProtocolVersion
    {
    EVersionUnknown = 0,
    EVersionIp,
    EVersionIp6
    };

/**
 *
 *  Enumeration of deactivation types.
 *
 */
enum TDeactivateType
    {
    EDeactivateNormal = 0,
    EDeactivateForced 
    };


/**
 * Event specification for EGetProtocolVersionEvent and
 * EGetProtocolVersionCancelEvent
 */
class TGetProtocolVersionEventSpec : public TEventSpec
    {
public:
    inline TGetProtocolVersionEventSpec() : TEventSpec()
        {
        iPolicyId.Zero();
        }
    TVpnPolicyId iPolicyId;
    };

/**
 * Event data for EGetProtocolVersionEvent
 */
class TGetProtocolVersionEventData : public TEventData
    {
public:
    inline TGetProtocolVersionEventData() : TEventData(), iProtocolVersion(EVersionUnknown)
        {}
    TProtocolVersion iProtocolVersion;
    };

/**
 * Event specification for EStartVpnConnEvent and EStartVpnConnCancelEvent
 */
class TStartVpnConnEventSpec : public TEventSpec
    {
public:
    inline TStartVpnConnEventSpec() : TEventSpec()
        {
        iPolicyId.Zero();
        }
    TRealConfig iIfInfo;
    TVpnPolicyId iPolicyId;
    };

/**
 * Event data for EStartVpnConnEvent
 */
class TStartVpnConnEventData : public TEventData
    {
public:
    inline TStartVpnConnEventData() : TEventData()
        {
        iIkePolicyHandle.iHandle = 0;
        iIpsecPolicyHandle.iHandle = 0;
        }
    TPolicyHandle iIkePolicyHandle;
    TPolicyHandle iIpsecPolicyHandle;
    TVPNAddress iVpnAddressInfo;
    TUint32 iIapId;
    TUint32 iNetId;    
    };

/**
 * Event specification for ECloseVpnConnEvent and ECloseVpnConnCancelEvent
 */
class TCloseVpnConnEventSpec : public TEventSpec
    {
public:
    inline TCloseVpnConnEventSpec() 
        :TEventSpec(), 
        iDeactivateType(EDeactivateNormal), 
        iVpnIapId(0), 
        iRealIapId(0)
        {
        iIkePolicyHandle.iHandle = 0;
        iIpsecPolicyHandle.iHandle = 0;
        }
    TPolicyHandle iIkePolicyHandle;
    TPolicyHandle iIpsecPolicyHandle;
    TDeactivateType iDeactivateType;
    TUint32 iVpnIapId;
    TUint32 iRealIapId;
    };

/**
 * Event data for ECloseVpnConnEvent
 */
typedef TEventData TCloseVpnConnEventData;

/**
 * Event specification for EStartRealIapConnEvent and
 * EStartRealIapConnCancelEvent
 */
class TStartRealIapConnEventSpec : public TEventSpec
    {
public:    
    inline TStartRealIapConnEventSpec() 
        :TEventSpec(), iRealIapConnInfo(), iVpnIapId(0), iRealIapId(0)
        {}
    TConnectionInfo iRealIapConnInfo;
    TUint32 iVpnIapId;
    TUint32 iRealIapId;
    };

/**
 * Event data for EStartRealIapConnEvent
 */
typedef TEventData TStartRealIapConnEventData;

/**
 * Event specification for EObserveRealIapConnEvent and
 * EObserveRealIapConnCancelEvent
 */
typedef TStartRealIapConnEventSpec TObserveRealIapConnEventSpec;

/**
 * Event data for EObserveRealIapConnEvent
 */
class TObserveRealIapConnEventData : public TEventData
    {
public:
    inline TObserveRealIapConnEventData()
        : TEventData(), iInterfaceState(ESingleInterfaceStateUnknown)
        {}
    TSingleConnInterfaceState iInterfaceState;
    };


#endif // EVENTDEFSSIT_H
