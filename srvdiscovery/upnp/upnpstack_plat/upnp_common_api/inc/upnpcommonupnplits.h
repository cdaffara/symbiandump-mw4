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
* Description:  Declares string definitions
*
*/


#ifndef C_UPNPCOMMONUPNPLITS_H
#define C_UPNPCOMMONUPNPLITS_H

#include <e32base.h> 

//CONSTANTS

const TInt KLongInfoSize = 1000;
const TInt KMaxInfoSize = 20;

// LITERALS

_LIT8( Keq,                 "=\"" );
_LIT8( Kfalse,              "false" );
_LIT8( Ktrue,               "true" );

_LIT8( KClosingBracket,     ")" );
_LIT8( KClosingBracketWithQuotation, "\")" );
_LIT8( KColon,              ":");
_LIT8( KComma,              "," );
_LIT8( KEmptyString,        "" );
_LIT8( KKorp,               "<" ); 
_LIT8( KKorpSlash,          "</" ); 
_LIT8( KObjectID,           "ObjectID" );
_LIT8( KOne,                "1" );
_LIT8( KOneNumber,          "%i" );
_LIT8( KOneNumber8,         "%i" );
_LIT8( KPlainLineFeed,      "\r\n" );
_LIT8( KProk,               ">" );
_LIT8( KSeparator,          "://" );    // from upnphttpsession.h
_LIT8( KSpace,              " " );
_LIT8( KSpace8,             " " );
_LIT8( KQuotation,          "\"" ); 
_LIT8( KQuotationWithSpace, "\" " );
_LIT8( KZero,               "0" );

// from upnphttpsession.h
_LIT16( KBackSlash,         "\\" );
_LIT16( KSpace16,           " " );
_LIT16( KDoubleBackSlash,   "\\" );     // from upnphttpsessionfileserving.h

// from messahandlerengine.h/(cpp)
_LIT( KMessagehandlerPanic, "UPnP messagehandler Panic" );
_LIT8( KEmptyMessage, " " );
_LIT8( KMulticastPacket, " " );

// from upnpdispatcherengine.h/(cpp)
//_LIT8( KZero, "0" ); // redefinition
_LIT16( KLanBearer, "LANBearer" );

// from upnpdispatchersession.h/(cpp)
#if defined(__HN_31__) || defined(__HN_32__)
_LIT( KMessageHandlerFilename, "hnMessageHandler" );
#else
_LIT( KMessageHandlerFilename, "MessageHandler" );
#endif




//----------------------------------------------------------------------------------
// Literals for logs & others & panics
//----------------------------------------------------------------------------------
// from upnpcm.h/(cpp)
_LIT8( KErrorHere, "Here is an ERROR!" );

// from upnphttpsessionfileserving.cpp
_LIT8( Servedbytes,"Sent %d/%d" );
_LIT8( NBytesServed,"Served %d bytes" );

// from upnptcpsession.cpp
_LIT8( KRunL,"RunL(), error %d" );

// from upnpdispatcherengine.h/(cpp)
_LIT8( KSendFail, "DISPATCHERENGINE *** Sending of http message fails!" );

// from service.pp
_LIT8(KPanic, "SERVICE *** Leave during service description parsing: ");


//----------------------------------------------------------------------------------
// *.CPP tiedostoista tulleet uudet literaalit
//----------------------------------------------------------------------------------
// from upnphttpserver.h

_LIT8( KIndexHtml,      "index.html" );
_LIT16( KIndexHtml16,   "index.html" );
_LIT8( KIndexXml,       "index.xml" );
_LIT16( KIndexXml16,    "index.xml" );


_LIT8( KArgTypeProtocolInfo,    "A_ARG_TYPE_ProtocolInfo" );
_LIT8( KAvtransId,              "A_ARG_TYPE_AVTransportID" );
_LIT8( KConMan,                 "A_ARG_TYPE_ConnectionManager" );
_LIT8( KConId,                  "A_ARG_TYPE_ConnectionID" );
_LIT8( KConStat,                "A_ARG_TYPE_ConnectionStatus" );
_LIT8( KDirection,                    "A_ARG_TYPE_Direction" );
_LIT8( KProtInf,                "A_ARG_TYPE_ProtocolInfo" );
_LIT8( KRcsId,                  "A_ARG_TYPE_RcsID" );

_LIT8( KConnectionIDs,          "ConnectionIDs" );
_LIT8( KCurConId,               "CurrentConnectionIDs" );
_LIT8( KGetProtInfo,            "GetProtocolInfo" );
_LIT8( KGetCurConIds,           "GetCurrentConnectionIDs" );
_LIT8( KGetCurConInfo,          "GetCurrentConnectionInfo" );
_LIT8( KSink,                   "Sink" );
_LIT8( KSinkInf,                "SinkProtocolInfo" );
_LIT8( KSource,                 "Source" );
_LIT8( KSourceInf,              "SourceProtocolInfo" );
_LIT8( KSupportedProtocols,     "http-get:*:*:*" );
_LIT8( KNull,                   "NULL" );
_LIT8( KOutput,                 "output" );
_LIT8( KOk,                     "OK" );
_LIT8( KTypeRcsID,              "RcsID" );
_LIT8( KTypeAVTransportID,      "AVTransportID" );
_LIT8( KTypeProtocolInfo,       "ProtocolInfo" );
_LIT8( KTypePeerConnectionManager, "PeerConnectionManager" );
_LIT8( KTypePeerConnectionID,   "PeerConnectionID" );
_LIT8( KTypeDirection,          "Direction" );
_LIT8( KTypeStatus,             "Status" );
_LIT8( KArgument,               "ConnectionID" );
_LIT8( KPrepareForConnection,   "PrepareForConnection" );
_LIT8( KConnectionComplete,     "ConnectionComplete" );

// from upnpdevice.h
_LIT8( KControlUrl,     "controlURL" );
_LIT8( KDevice,         "device" );
_LIT8( KDeviceType,     "deviceType" );
_LIT8( KDeviceList,		"deviceList" );
_LIT8( KDotXml,         ".xml" );
_LIT8( KFriendlyName,   "friendlyName" );
_LIT8( KHttp,           "http://" );
_LIT8( KIcon,           "icon" );
_LIT8( KIconList,       "iconList");
_LIT8( KLine,           "-" );
_LIT8( KMask8,          "%08x" );
_LIT8( KManufacturer,   "manufacturer" );
_LIT8( KManufacturerUrl, "manufacturerURL" );
_LIT8( KModelDescription, "modelDescription" );
_LIT8( KModelName,      "modelName" );
_LIT8( KModelNumber,    "modelNumber" );
_LIT8( KModelUrl,       "modelURL" );
_LIT8( KSepar,          ":" );
_LIT8( KSerialNumber,   "serialNumber" );
_LIT8( KService,        "service" );
_LIT8( KServiceList,	"serviceList" );
_LIT8( KServiceId,      "serviceId" );
_LIT8( KServiceType,    "serviceType" );
_LIT8( KServiceUrn,     "service:" );
_LIT8( KServiceDescriptionUrl, "SCPDURL" );
_LIT8( KSubscriptionUrl, "eventSubURL" );
_LIT8( KDeviceSearch,   ":device:" );
_LIT8( KR,              "\r" );
_LIT8( KSlash8,         "/" );
_LIT8( KUdn,            "UDN" );
_LIT8( KUuid,           "uuid:" );
_LIT8( KPresentationUrl, "presentationURL" );
_LIT8( KUrlBase, "URLBase" );

// from upnpgenamessage.h
_LIT8( KNoHeader,       "NoHeader" );
_LIT8( KDefaultHostPort, "80" );
_LIT8( KSidPrefix,      "uuid:" );
_LIT8( KTimeoutPrefix,  "Second-" );


// from upnpcontrolpoint.h
_LIT8( KPath,       "path" ); 
_LIT8( KGet,        "GET" );
_LIT8( KSeconds,    "Second-" );
_LIT8( KMimePng,    "image/png" );
_LIT8( KMimeJpeg,   "image/jpeg" );
_LIT8( KUpnpPath,   "C:\\System\\Data\\UPnP\\" );
_LIT8( KLessThan8,   "<"  ); // (8&16 bit)
_LIT8( KGreaterThan8, ">" ); // (8&16 bit)

_LIT( KMediaDriveLetter, "c:");
	
#endif	// C_UPNPCOMMONUPNPLITS_H

// End Of File