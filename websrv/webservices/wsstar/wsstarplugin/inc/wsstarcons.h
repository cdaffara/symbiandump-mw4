/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           Keep all constanece within WSTar Plugin
*
*/










#ifndef CONSPLUGIN_H
#define CONSPLUGIN_H

#include "senpointermap.h"
#include "SenXmlElement.h"

typedef RSenPointerMap<TDesC8, TInt> RMapInt;
typedef RSenPointerMap<TDesC8, TDesC8> RMapDescriptors;


//global
const TInt KSizeOfMD5        = 16;
const TInt KBase64SizeOfMD5 = 24;//16 * 1,5



namespace WSStarConfig
    {
    _LIT8(KFrameworkVersion,      "1.0");
    _LIT8(KFrameworkXMLNS,        "urn:com.nokia.ws.wsstar.config.1.0");
    _LIT8(KFrameworkCue,          "com.nokia.ws.wsstar.WSStarFramework");
    _LIT8(KFrameworkLocalName,    "Framework");
    _LIT8(KHandlerLocalName,      "Handler");
    _LIT8(KNameAttr,              "name");
    _LIT8(KDeviceIdLocalName,     "DeviceId");
    
    _LIT8(KClassAttr,              "class");
    _LIT8(KVersionAttr,              "version");    
    _LIT8(KEnveloperValue,              "urn:com.nokia.ws.wsstar.handler.enveloper:1.0");
    _LIT8(KRegisterValue,              "urn:com.nokia.ws.wsstar.handler.register:1.0");
    _LIT8(KServiceUpdateValue,              "urn:com.nokia.ws.wsstar.handler.serviceUpdate:1.0");
    _LIT8(KCredentialCollectorValue,              "urn:com.nokia.ws.wsstar.handler.credentialCollector:1.0");    
    _LIT8(KAddressingValue,              "urn:com.nokia.ws.wsstar.handler.addressing:1.0");    
    _LIT8(KValidateValue,              "urn:com.nokia.ws.wsstar.handler.validate:1.0");    
    _LIT8(KSecurityValue,              "urn:com.nokia.ws.wsstar.handler.security:1.0");    
    _LIT8(KPassportValue,              "urn:com.nokia.ws.wsstar.handler.passport:1.0"); 
    _LIT8(KPolicyValue,                "urn:com.nokia.ws.wsstar.handler.policy:1.0");        
    _LIT8(KCoBrandingValue,            "urn:com.nokia.ws.wsstar.handler.cobranding:1.0");//co-branding
    }
    
namespace WSStarSession
    {
    
    //FAULTS
    _LIT8( KErrorLayer, "KErrorLayer");
     enum TFaultLayer
        {
        EAddressing,
        ETrust
        };
        
        
        
    const TInt KSubmitStateOK                         = 1;
    const TInt KSubmitStateInvalidCredentials        = 2;
    const TInt KSubmitStateRevalidationAttempted    = 3;
    
    //in seconds
    const TInt KMaxTimeForRetry    = 5;
    const TInt KCounterMax    = 3;
    const TInt KMaxHttpTimeOut   = 60;
    
    }
namespace WSStarSessionConsumer
    {
    _LIT8(KFaultInvalidRelatesToTag, "Fault.WS-Addressing : InvalidMessageInformationHeader in wsa:RelatesTo Detail: Invalid value");
    _LIT8(KFaultInvalidToTag, "Fault.WS-Addressing : InvalidMessageInformationHeader in wsa:To Detail: Invalid value");
    }

namespace WSStarContextKeys
    {
    //MESSAGE KEYS
     //--keys for enveloper handler
    _LIT8( KSoapVersion, "SOAPver");
    _LIT8( KBody, "Body");
    _LIT8( KBodyElement, "BodyElement");
    
    //--keys for enveloper handler
    //passort is enabled only berween C and STS
    _LIT8( KPassportEnabled, "PassportEnabled");
    
    //--keys for addressing handler
    _LIT8( KAddressingVersion, "AddressingVer");
    _LIT8( KTo, "To");
    _LIT8( KMessageID, "MessageID");
    _LIT8( KRelatesTo, "RelatesTo");
    _LIT8( KAction, "Action");
    _LIT8( KResponseAction, "ResponseAction");
    _LIT8( KReplyTo, "ReplyTo");
    _LIT8( KReplyToDeviceAddress, "ReplyToDeviceAddress");
    _LIT8( KFrom, "From");
    _LIT8( KFaultTo, "FaultTo");
    _LIT8( KMwsNamespace, "MwsNamespace");
      
    
    //--keys for credential handler
    _LIT8( KSecurityToken, "SecurityToken");
    _LIT8( KSecurityTokenBackup, "SecurityTokenBackup");
    _LIT8( KMTIsReplacedBySCT, "MTIsReplacedBySCT");
    _LIT8( KTimestampCreated, "Created");
    _LIT8( KTimestampExpires, "Expires");
    _LIT8( KPhoneTimeWhenMTResolved, "PhoneTimeWhenMTResolved");
    
    //--keys  for signing handler
    _LIT8( KPOPBase64, "POPBase64");
    _LIT8( KBinaryType, "BinaryType");
    _LIT8( KSTR, "STR");
    _LIT8( KSignedPartsArray, "SignedPartsArray");
    
   
    _LIT8( KRetryAfter, "RetryAfter");
    _LIT8( KTokenType, "TokenType");
    //--keys  for passport handler
    _LIT8( KRedirect, "Redirect");
    _LIT8( KMainBrandID, "MainBrandID");
    _LIT8( KBrandIDList, "BrandIDList");
    _LIT8( KBrandIDEnabled,"BrandIDEnabled");
    _LIT8( KAppInfo,"AppInfo" );
    _LIT8( KApplication,"Application" );
    _LIT8( KNsPApplication,"mws:Application" );
    _LIT8( KApplicationName,"ApplicationName" );
    _LIT8( KApplicationVendor,"ApplicationVendor" );
    _LIT8( KApplicationVersion,"ApplicationVersion" );
    _LIT8( KDeviceOptions,"DeviceOptions" );
    _LIT8( KNsPDeviceOptions,"mws:DeviceOptions" );
    _LIT8( KLocale,"Locale" );
    _LIT8( KNsPLocale,"mws:Locale" );
    _LIT8( KUserAgent,"UserAgent" );
    _LIT8( KNsPUserAgent,"mws:UserAgent" );
    _LIT8( KManufacturer,"Manufacturer" );
    _LIT8( KNsPManufacturer,"mws:Manufacturer" );
    _LIT8( KModel,"Model" );
    _LIT8( KNsPModel,"mws:Model" );
    _LIT8( KOS,"OS" );
    _LIT8( KNsPOS,"mws:OS" );
    _LIT8( KIMSI,"IMSI" );
    _LIT8( KNsPIMSI,"mws:IMSI" );
    _LIT8( KDeviceName,"DeviceName" );
    _LIT8( KDeviceVersion,"DeviceVersion" );
    _LIT8( KWAPGW,"WAPGW" );
    _LIT8( KName,"Name" );
    _LIT8( KNsPName,"mws:Name" );
    _LIT8( KVendor,"Vendor" );
    _LIT8( KNsPVendor,"mws:Vendor" );
    _LIT8( KVersion,"Version" );
    _LIT8( KNsPVersion,"mwsVersion" );
    _LIT8( KAppInfoPresent,"AppInfoPresent" );
    
    //SESSION KEYS
    //--keys for discovery phase
    //-----register handler
    _LIT8(KRegisterAction,       "action");
    _LIT8(KServiceDescription,   "serviceDescription");
    
    //--keys for validation phase (handler)
    _LIT8(KServiceSession,       "serviceSession");
    _LIT8(KRemoteConsumer,       "RemoteConsumer");
    _LIT8(KOnlySharing,       "OnlySharing");
    
    //--keys for faults
    _LIT8(KReAuthNeeded, "ReAuthNeeded");
	_LIT8(KReAuthNeededFromCredObserv, "ReAuthNeededFromCredObserv");
    _LIT8(KErrMessage, "ErrorMessage");
    
    
    //MARKERS
    _LIT8( KAddressAttrMark, "mark_address");
    _LIT8( KMultiReqTagMark, "mark_multiReq");
    
    //holder for shared inforamtiom.Keep handler version specific information
    _LIT8( KAddressingNsHolder, "addressingNsHolder");
    _LIT8( KAddressingEndpointTagHolder, "addressingEndpointHolder");
        //holder for Policy
    _LIT8( KWSStarPolicyHolder, "PolicyHolder");

    _LIT8( KWSStarTranspProp, "TransportPropertiesForResend");
    }

namespace WSStarContextValues
    {
    _LIT8( KPOPTypeSimmetric, "simmetric");
    _LIT8( KPOPTypeAsimmetric, "asimmetric");   
    _LIT8( KPOPTypeNonce, "nonce");   
    
    //--values for discovery phase
    //-----register handler
    _LIT8(KActionRegister,       "register");
    _LIT8(KActionUnregister,       "unregister");
    }

#endif // CONSPLUGIN_H
