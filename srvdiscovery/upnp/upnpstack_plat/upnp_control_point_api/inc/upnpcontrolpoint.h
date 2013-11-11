/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  implementation of Control Point
*
*/


#ifndef C_CUPNPCONTROLPOINT_H
#define C_CUPNPCONTROLPOINT_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <upnpnetworkeventobserver.h>
#include "upnpdispatcher.h"
#include "upnpdispatchercustomer.h"
#include "upnpdiscoveryobserver.h"

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpService;
class CUpnpAction;
class CUpnpHttpMessage;
class CUpnpGenaMessage;
class CUpnpServiceInfo;
class CUpnpContentHandlersController;
class CUpnpCpbDescriptionAgent;
class CUpnpCpbCurrentHttpClient;
class CUpnpCpbDiscoveryAgent;
class CUpnpCpbDeviceRepository;
class MUpnpCpStackRequestor;
class MUpnpCpHttpRequestor;
class CUpnpConnectionManagerNetworkEventProvider;
class CUpnpCpbInitialEventRetryHandler;

/**
 * @brief Base class for control points.
 *
 * An abstract control point class. This class has an interfaces for following
 * areas:
 *
 * Device discovery
 * Resolving properties of devices and services
 * Action requests to the services
 * Eventing subscriptions
 *
 *  @since Series60 2.6
 */
class CUpnpControlPoint : public CBase, public MUpnpDiscoveryObserver,
         public MUpnpNetworkEventObserver
    {

public:
    static const TInt KDefaultSubscriptionTimeout = 300; // 5 minutes

    enum TFilteringMode
        {
        EBoth,
        ERada,
        EControlPoint,
        };

public:
    // Constructors and destructors

    /**
     * Virtual destructor function.
     */
    IMPORT_C virtual ~CUpnpControlPoint();

protected:
    // Constructors


    /**
     * Basic constructor function. Should be called in the constructor of
     * the deriving class.
     * @param aFriendlyName The friendly name of the control point. Friendly
     * name of a control point cannot be seen outside the emulator.
     */
    IMPORT_C CUpnpControlPoint();

    /**
     * For internal, automated test cases use only.
     */
    IMPORT_C CUpnpControlPoint(
                MUpnpCpStackRequestor& aStackRequestor,
                MUpnpCpHttpRequestor& aHttpRequestor );

    /**
     * One of the two ConstructL functions of CUpnpControlPoint has to be called
     * in the ConstructL (or corresponding) function of the deriving class.
     * \n Sets essential variables and the target device types.
     * Examples of target device types:
     * <ul><li>AV control point: MediaServer:1, MediaRenderer:1
     * <li>Lighting control point: SaunaLight:1, CandleLight:1 (an imaginary example)
     * </ul>
     * @since Series60 2.6
     * @param aTargetDeviceTypes A descriptor array containing the target device
     * types in separate descriptors.
     */
    IMPORT_C void ConstructL( const CDesC8Array& aTargetDeviceTypes );

    /**
     * One of the two ConstructL functions of CUpnpControlPoint has to be called
     * in the ConstructL (or corresponding) function of the deriving class.
     * \n Sets essential variables and the target device types.
     * Examples of target device types:
     * <ul><li>Printer control point: Printer:1
     * <li>Cooking control point: RobotCook:1 (an imaginary example)
     * </ul>
     * @since Series60 2.6
     * @param aTargetDeviceType A descriptor containing the target device type
     */
    IMPORT_C void ConstructL( const TDesC8& aTargetDeviceType );

public:
    // Functions from devired class

    /**
     * Make a SSDP search. Search string should be some of these strings (defined in UPnP Device Architecture):\n\n
     * <ul>
     * <li><b>urn:schemas-upnp-org:device:<i>deviceType:v</i></b>,\r where <i>deviceType:v</i> should be replaced by target device type and version. Search for devices of specific type.\n
     * <li><b>urn:schemas-upnp-org:service:<i>serviceType:v</i></b>,\r where <i>serviceType:v</i> should be replaced by target service type and version. Search for services of specific type.\n
     * <li><b>upnp:rootdevice</b>\r Search for root devices.\n
     * <li><b>ssdp:all</b>\r Search for any devices or services\n
     * </ul>
     * @since Series60 2.6
     * @param aSearchString The search target.
     */
    IMPORT_C void SearchL( const TDesC8& aSearchString );

    /**
     * Sends an action. This action can be instantiated using
     * CUpnpService::CreateActionLC() function. Target addresses and some other
     * necessary information are added in the action during construction.
     * After instantiation of the action, set the arguments using e.g.
     * CUpnpAction::SetArgument() function. After this, you are ready to call
     * this function.
     * After the target service has reacted to the action,
     * it will send a response. This response is forwarded to
     * ActionResponseReceived function.
     *
     * The user of this function has the responsibility to delete the CUpnpAction instance.
     * @since Series60 2.6
     * @param aAction The action to be sent.
     */
    IMPORT_C void SendL( CUpnpAction* aAction );

    /**
     * Sends a HTTP message. This HTTP message can be instantiated in many
     * ways; it's recommended to use RUpnpHttpMessageFactory class. When the
     * response to this request has been receiveid, the HttpResponseReceived()
     * virtual function is called.
     *
     * Notice that the user of this function has the responsibility to delete
     * the CUpnpHttpMessage instance.
     * @since Series60 2.6
     * @param aMessage The message to be sent.
     */
    IMPORT_C void SendL( CUpnpHttpMessage* aMessage );

    /**
     * Returns the device list maintained by CUpnpControlPoint. Only target type
     * devices are added to this list. (Recall: target types are defined in
     * ConstructL(...) function of CUpnpControlPoint.
     *
     * While discovering devices, the DeviceDiscovered(...) function is called
     * once for every device after the device has been added to this list.
     */
    IMPORT_C const RPointerArray<CUpnpDevice>& DeviceList() const;

    /**
     * Subscribes to a service. This starts the eventing to this control point
     * done by the target service. Practically, the StateUpdated virtual
     * function is called when an event is received.
     *
     * It's necessary to subscribe to a service when the service is used by
     * this control point.
     *
     * See service definitions to see the list of the evented state variables;
     * not every state variable is evented.
     *
     * It's not needed to maintain a separate list of subscribed services in
     * the deriving class. Control point manages a such list. If the target
     * service is already evented, this function does nothing.
     * @since Series60 2.6
     * @param aService The target service.
     */
    IMPORT_C void SubscribeL( CUpnpService* aService );

    /**
     * Removes the subscription of the target services. Practically, eventing
     * from target service is stopped
     * @since Series60 2.6
     * @param aService The target service.
     */
    IMPORT_C void UnsubscribeL( const CUpnpService* aService );

    /**
     * Removes the subscription of the target services. Practically, eventing
     * from target service is stopped
     * @since Series60 2.6
     * @param aService The target service.
     * @param aSendUnsubscribeMessage ETrue if unsubscription message is sent to
     * remote host. Otherwise EFalse ( only remove service ).
     */
    void UnsubscribeL( CUpnpService* aService, TBool aSendUnsubscribeMessage );

    /**
     * Resubscribes to a service.
     * @param aServiceInfo The target, already subscribed service info
     */
    void ResubscribeL( CUpnpServiceInfo* aServiceInfo );

public: // From MUpnpDiscoveryObserver

    // interface for CUpnpDispatcher
    /**
     * For internal use; called by CUpnpDispatcher.
     * @since Series60 2.6
     * @param aDevice The target service.
     */
    IMPORT_C void DeviceFoundL( CUpnpDevice& aDevice );

    /**
     * For internal use; called by CUpnpDispatcher.
     * @since Series60 2.6
     * @param aDevice The target service.
     */
    IMPORT_C void DeviceLostL( CUpnpDevice& aDevice );

    /**
     * For internal use; called by CUpnpDispatcher.
     * @since Series60 2.6
     * @return the application path
     */
    IMPORT_C const TDesC8& Path();

public:
    /**
     * This function will be invoke if IP Address of UPnP changes
     */
    void AddressChangedL();

    /**
     * For internal use.
     * @since Series60 2.6
     * @param aDevice The target service.
     */
    void MessageReceived( CUpnpHttpMessage* aHttpMessage );

    /**
     * ProcessDeviceMessageL
     * @since Series60 5.0
     * @param aHttpMessage
     */
    void ProcessDeviceMessageL( CUpnpHttpMessage* aHttpMessage );

    /**
     * ProcessServiceMessageL
     * @since Series60 5.0
     * @param aHttpMessage
     */
    void ProcessServiceMessageL( CUpnpHttpMessage* aHttpMessage );

    /**
     * SubscriptionResponseReceivedL
     * @since Series60 2.6
     * @param aServiceInfo
     */
    void SubscriptionResponseReceivedL( CUpnpHttpMessage& aHttpMessage );

    /**
     * UnSubscriptionResponseReceived
     * @since Series60 3.2
     * @param aHttpMessage
     */
    void UnSubscriptionResponseReceived();

    /**
     * ProcessActionMessageL
     * @since Series60 3.2
     * @param aHttpMessage
     */
    void ProcessActionMessageL( CUpnpHttpMessage* aHttpMessage );

   /**
    * NotifyReceivedL
    * @since Series60 2.6
    * @param aHttpMessage
    */
   void NotifyReceivedL( CUpnpHttpMessage& aHttpMessage );
    
    /**
     * Provides handling for postponed initial event that comes before subscription 
     * transaction has been completed
     * @param aGenaMsg 
     */
   void HandlePostponedInitialEventL( CUpnpGenaMessage& aGenaMsg );
   
private: // New functions

    /**
     * ActionMessage
     * @since Series60 3.1
     * @param aHttpMessage
     */
    void ActionMessage(CUpnpHttpMessage* aHttpMessage);

    /**
     * Removes root device from device repository, and deletes passed object
     * @since Series60 3.2
     * @param aDevice
     */
    void RemoveRootDeviceLD( CUpnpDevice* aDevice );

    /**
     * RemoveActionsL
     * @since Series60 3.2
     * @param aDevice
     */
    void RemoveActionsL(CUpnpDevice* aDevice );

    /**
     * RemoveServicesL
     * @since Series60 3.2
     * @param aDevice
     */
    void RemoveServicesL( CUpnpDevice* aDevice );

    /**
     * FindServiceInfoByService
     * @param aService
     */
    CUpnpServiceInfo* FindServiceInfoByService( CUpnpService* aService );

    /**
     * FindServiceInfoByGenaMessage
     * @param aGenaMsg
     */
    CUpnpServiceInfo* FindServiceInfoByGenaMessage(
            CUpnpGenaMessage* aGenaMsg );

    /**
     * RemoveFromSubscribedListD
     * @param aServiceInfoToRemove service info that will be removed from
     *     subscribed list
     */
    void RemoveFromSubscribedList( CUpnpServiceInfo* aServiceInfoToRemove );

    /**
     * MapHttpError
     * @param http error
     * @since Series60 3.2
     * @return generic symbian error
     */
    TInt MapHttpError( TInt aError );

    /**
     * Sends a HTTP message that is response and destroy it. Leave safe.
     * @param aMessage response to be sent.
     */
    void SendResponseMessageLD( CUpnpHttpMessage* aResponse );

    /**
     * CheckEventKey
     * @param aIncomingSeq incoming event key in notify message
     * @param aExistingSeq existing event key
     * @since Series60 3.2
     * @return generic symbian error
     */
    TInt CheckEventKey( const TDesC8& aIncomingSeq, TUint32 aExistingSeq );

    /**
     * Send a proper error basing on httpmsg
     * @since Series60 2.6
     * @param aHttpMessage http message
     * @param aCode error code
     */
    void SendErrorResponseL( CUpnpHttpMessage* aHttpMessage, THttpStatusCode aCode );
    
    // Gena events handling methods
    CUpnpServiceInfo* FindServiceInfoBySid( RPointerArray<CUpnpServiceInfo>& aInfos,
            const TDesC8& aSid );
    
    void ResubscribeL( CUpnpServiceInfo& aInfo );
    
    void ParseEventUpdateStateVariablesL(
            CUpnpGenaMessage& aGenaMsg, CUpnpServiceInfo& aInfo );
    
    THttpStatusCode ValidateEventResolveHttpResponseCode(
            CUpnpGenaMessage& aGenaMsg );

public:

    /**
     * Device getter.
     * @since Series 60 2.6
     * @param aUuid A Universal Unique Identifier of device.
     * @return Founded device.
     */
    IMPORT_C const CUpnpDevice* Device(const TDesC8& aUuid);

    /**
     * Callback function for eventing. This function is called when
     * a) before this, we have made a subscription to the service
     * b) at least one of the evented state variables of the service
     * has changed
     *
     * Note that you cannot see which of the evented state variables has
     * changed. Fortunately, there are not many evented state variables.
     * @since Series60 2.6
     * @param aService At least one of the evented state variables of this
     * service has changed. CUpnpControlPoint class owns the CUpnpService instance;
     * do not delete it after inspection!
     */
    virtual void StateUpdatedL( CUpnpService* aService ) = 0;

    /**
     * Callback function for device discovery. This function is called once
     * for every device. The device is already then in the device list
     * accessible through DeviceList() function.
     * @since Series60 2.6
     * @param aDevice The newly discovered device.
     */
    virtual void DeviceDiscoveredL( CUpnpDevice* aDevice ) = 0;

    /**
     * Callback function for device disappearance. This function is called
     * once for every device that has disappeared.
     *
     * Notice that before this, the device is removed from the generic
     * control point
     * @since Series60 2.6
     * @param aDevice The disappeared device.
     */
    virtual void DeviceDisappearedL( CUpnpDevice* aDevice ) = 0;

    /**
     * Callback function for action responses. This function is called
     * @since Series60 2.6
     * @param aAction
     */
    virtual void ActionResponseReceivedL( CUpnpAction* aAction ) = 0;

    /**
     * Callback function for HTTP responses
     * @since Series60 2.6
     * @param aMessage
     */
    virtual void HttpResponseReceivedL( CUpnpHttpMessage* aMessage ) = 0;

    /**
     * This function creates Gena subscription message, which will be used for data retrieval.
     */
    CUpnpGenaMessage* CreateSubscriptionMessageL(CUpnpService& aService,
            const TDesC8& aPath, TInt aTimeout, THTTPMsgType aType);

    /**
     * Gets address from Url
     * @since Series60 3.2
     * @param aUrl
     * @return TInetAddr
     */
    TInetAddr GiveAddressL(const TDesC8& aUrl);

    /**
     * Non-leaving caller of callback function for eventing.
     * @since Series60 3.2
     * @see CUpnpControlPoint::StateUpdatedL
     */
    void StateUpdated( CUpnpService* aService );

    /**
     * Non-leaving caller of callback function for device discovery.
     * @since Series60 3.2
     * @see CUpnpControlPoint::DeviceDiscoveredL
     */
    void DeviceDiscovered( CUpnpDevice* aDevice );

    /**
     * Non-leaving caller of callback function for device disappearance.
     * @since Series60 3.2
     * @see CUpnpControlPoint::DeviceDisappearedL
     */
    void DeviceDisappeared( CUpnpDevice* aDevice );

    /**
     * Non-leaving caller of callback function for action responses.
     * @since Series60 3.2
     * @see CUpnpControlPoint::ActionResponseReceivedL
     */
    void ActionResponseReceived( CUpnpAction* aAction );

    /**
     * Non-leaving caller of callback function for HTTP responses.
     * @since Series60 3.2
     * @see CUpnpControlPoint::HttpResponseReceivedL
     */
    void HttpResponseReceived( CUpnpHttpMessage* aMessage );

    /**
     * Checks if all device in tree expired
     * @since Series60 3.2
     * @param aRoot Device
     * @return ETrue when all devices expired, otherwise EFalse
     */
    TBool AllDevicesExpired( CUpnpDevice* aDevice );

public:

    /**
     * Starts rada device asynchronusly
     * sience Series60 5.0
     * @param aStatus asychronus object to completes the request
     */
    IMPORT_C void EnableRadaDeviceL( TRequestStatus& aStatus );

    /**
     * Starts rada device synchronusly
     * sience Series60 5.0
     * @return rada device start status
     */
    IMPORT_C TInt EnableRadaDeviceL();

    /**
     * Stops rada device synchronusly
     * sience Series60 5.0
     */
    IMPORT_C void DisableRadaDeviceL();

    /*****IPFiltering****/
    IMPORT_C void StartIPFilteringL( TFilteringMode aMode = EBoth );
    IMPORT_C void StopIPFilteringL( TFilteringMode aMode = EBoth );
    /********************/

public:    // from MUpnpNetworkEventObserver

    /**
     * It is called from bottom layer when network event occurs [callback method]
     * sience Series60 5.0
     * @result None
     */
    IMPORT_C void NetworkEvent( CUpnpNetworkEventBase* aEvent );

private: // Data

    // HTTP messages sender/response, owned
    CUpnpCpbCurrentHttpClient* iCpbHttpClient;

    // It's necessary to store this pointer, because in
    // AddressChanged method iCpbHttpClient is recreated.
    // not owned
    // NULL if default stackRequestor should be constructed
    MUpnpCpStackRequestor* const iPredefinedStackRequestor;

    // not owned
    // NULL if default httpRequestor should be constructed
    MUpnpCpHttpRequestor* const iPredefinedHttpRequestor;

    // Discovery processing class
    CUpnpCpbDescriptionAgent* iDescriptionAgent;

    // Target device types, owned
    CDesC8ArrayFlat* iTargetDeviceTypes;

    // CP owns array objects
    // Subscribed services list, owned
    RPointerArray<CUpnpServiceInfo> iSubscribedServicesInfos;

    // Action list, owned
    RPointerArray<CUpnpAction> iActionList;

    // Ropository with devices
    CUpnpCpbDeviceRepository* iDeviceRepository;

    // Discovery processing class
    CUpnpCpbDiscoveryAgent* iDiscoveryAgent;

    //sax parsers
    CUpnpContentHandlersController* iSaxController;

    // Network events provider used for WLAN lost and address change
    CUpnpConnectionManagerNetworkEventProvider *iNetworkEventProvider;
    
    CUpnpCpbInitialEventRetryHandler* iInitialEventRetry;
    };

#endif // C_CUPNPCONTROLPOINT_H

//  End of File

