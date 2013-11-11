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
* Description:    Header declaration
*
*/











#ifndef WSSTAR_MESSAGE_HANDLERS_CONS_H
#define WSSTAR_MESSAGE_HANDLERS_CONS_H

namespace WSStarAddressingConfig
    {
    _LIT8(KHandlerAddressingVersion,      "1.0");
    _LIT8(KHandlerAddressingXMLNS,        "urn:com.nokia.ws.wsstar.handler.addressing:1.0");
    _LIT8(KHandlerAddressingCue,          "com.nokia.ws.wsstar.handler.addressing");
    _LIT8(KHandlerAddressingLocalName,      "Handler");
    }

namespace WSStarAddressing
    {
    _LIT8(KAddressingXmlns200403,    "http://schemas.xmlsoap.org/ws/2004/03/addressing");
    _LIT8(KAddressingXmlns200408,    "http://schemas.xmlsoap.org/ws/2004/08/addressing");
    _LIT8(KAddressingXmlns200412,    "http://www.w3.org/2004/12/addressing");
    _LIT8(KAddressingXmlns200502,    "http://www.w3.org/2005/02/addressing");
    _LIT8(KAddressingXmlns200503,    "http://www.w3.org/2005/03/addressing");
    _LIT8(KAddressingXmlns200508,    "http://www.w3.org/2005/08/addressing");

    _LIT8(KAddressingFaultActionXmlns,    "http://schemas.xmlsoap.org/ws/2004/08/addressing/fault");
    _LIT8(KAddressingPrefix,"wsa");


    _LIT8(KReplyToAnonymous200403, "http://schemas.xmlsoap.org/ws/2004/03/addressing/role/anonymous");
    _LIT8(KReplyToAnonymous200408, "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");
    _LIT8(KReplyToAnonymous200412, "http://www.w3.org/2004/12/addressing/role/anonymous");
    _LIT8(KReplyToAnonymous200502, "http://www.w3.org/2005/02/addressing/role/anonymous");
    _LIT8(KReplyToAnonymous200503, "http://www.w3.org/2005/03/addressing/role/anonymous");
    _LIT8(KReplyToAnonymous200508, "http://www.w3.org/2005/08/addressing/role/anonymous");
    

    
    _LIT8(KActionName,"Action");
    _LIT8(KActionQName,"wsa:Action");
    _LIT8(KMessageIDName,"MessageID");
    _LIT8(KMessageIDQName,"wsa:MessageID");
    _LIT8(KToName,"To");
    _LIT8(KToQName,"wsa:To");
    _LIT8(KReplyToName,"ReplyTo");
    _LIT8(KReplyToQName,"wsa:ReplyTo");
    _LIT8(KFromName,"From");
    _LIT8(KFromQName,"wsa:From");
    _LIT8(KRelatesToName,"RelatesTo");
    _LIT8(KRelatesToQName,"wsa:RelatesTo");
    _LIT8(KAddressName,"Address");
    _LIT8(KAddressQName,"wsa:Address");
    _LIT8(KFaultToName, "FaultTo");
    _LIT8(KFaultToQName, "wsa:FaultTo");
    _LIT8(KEndpointReferenceName, "EndpointReference");
    _LIT8(KEndpointReferenceQName, "wsa:EndpointReference");
    _LIT8(KReferencePropertiesName, "ReferenceProperties");
    _LIT8(KReferencePropertiesQName, "wsa:ReferenceProperties");

    _LIT8(KRetryAfterName, "RetryAfter");
    _LIT8(KRetryAfterQName, "wsa:RetryAfter");
    }
namespace WSStarAddressingFault
    {
    _LIT8(KInvalidHeader,    "InvalidMessageInformationHeader");
    _LIT8(KRequiredHeader,    "MessageInformationHeaderRequired");
    _LIT8(KDestinationUnreachable,    "DestinationUnreachable");
    _LIT8(KActionNotSupported,    "ActionNotSupported");
    _LIT8(KEndpointUnavailable,    "EndpointUnavailable");

    _LIT8(KInvalidHeaderFormatString8,
    "Fault.WS-Addressing : InvalidMessageInformationHeader in %S Detail: %S");
    _LIT8(KRequiredHeaderFormatString8,
    "Fault.WS-Addressing : MessageInformationHeaderRequired : %S ");
    _LIT8(KMissingTag, "Tag is missing");
    _LIT8(KInvalidUri, "EUriUtilsParserErrInvalidUri");
    }

namespace WSStarSecurityFault
    {
    _LIT8(KUnsupportedAlgorithm, "Fault.WS-Security : UnsupportedAlgorithm");
    }

namespace WSStarCredentialCollector
    {
    _LIT8(KSecurityUtilsPrefix,"wsu");
    _LIT8(KSecurityUtilsIdQName,"wsu:Id");
    _LIT8(KSecurityUtilsXmlns, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd");        
    _LIT8(KSecurityUtilsTimestampName, "Timestamp");
    _LIT8(KSecurityUtilsCreatedName, "Created");
    }

namespace WSPassport32
    {
    //tags
    _LIT8(KPassportXmlns,    "http://schemas.microsoft.com/Passport/SoapServices/PPCRL");
    _LIT8(KPassportPrefix,"ps");
    
    
    _LIT8(KAuthInfoTag,"AuthInfo");
    _LIT8(KAuthInfoQTag,"ps:AuthInfo");
    _LIT8(KHostingAppTag,"HostingApp");
    _LIT8(KHostingAppQTag,"ps:HostingApp");
    _LIT8(KBinaryVersionTag,"BinaryVersion");
    _LIT8(KBinaryVersionQTag,"ps:BinaryVersion");
    _LIT8(KUIVersionTag,"UIVersion");
    _LIT8(KUIVersionQTag,"ps:UIVersion");
    _LIT8(KCookiesTag,"Cookies");
    _LIT8(KCookiesQTag,"ps:Cookies");
    _LIT8(KRequestParamsTag,"RequestParams");
    _LIT8(KRequestParamsQTag,"ps:RequestParams");
    _LIT8(KRequestMultipleSecurityTokensTag , "RequestMultipleSecurityTokens");
    _LIT8(KRequestMultipleSecurityTokensQTag, "ps:RequestMultipleSecurityTokens");
    
    _LIT8(KPPAuthInfoAttrValue, "PPAuthInfo");
    
    //values
    _LIT8(KHostingAppTagValue,"{DF60E2DF-88AD-4526-AE21-83D130EF0F68}");
    _LIT8(KBinaryVersionTagValue,"3");
    _LIT8(KUIVersionTagValue,"1");
    _LIT8(KCookiesTagValue,"");
    _LIT8(KRequestParamsTagValue,"AQAAAAIAAABsYwQAAAAxMDMz");
    
    //passport faults
    _LIT8(KRedirectUrlTag,"redirectUrl");
    _LIT8(KPassportFaultNamespaceXmlns, "http://schemas.microsoft.com/Passport/SoapServices/SOAPFault");
    _LIT8(KCredProperty, "credProperty");
    _LIT8(KCredProperties, "credProperties");
    _LIT8(KName, "Name");
    _LIT8(KMainBrandID, "MainBrandID");
    _LIT8(KBrandIDList, "BrandIDList");
    _LIT8(KIsWinLiveUser, "IsWinLiveUser");
    _LIT8(KCID, "CID");
    }

namespace WSStarAddressingEPR
    {
    _LIT8(KMwsPrefix,   "mws");
    _LIT8(KDeviceAddressName,   "DeviceAddress");
    _LIT8(KDeviceAddressQName,  "mws:DeviceAddress");
    }
    
#endif // WSSTAR_MESSAGE_HANDLERS_CONS_H
