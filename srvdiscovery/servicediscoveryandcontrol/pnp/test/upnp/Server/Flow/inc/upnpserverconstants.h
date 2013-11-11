// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __UPNPSERVERCONSTANTS_H_
#define __UPNPSERVERCONSTANTS_H_

#include <in_sock.h>

_LIT8(KXmlMimeType, "text/xml");
_LIT8(KBody,"Body");
_LIT8 ( KSoapFieldSeparator, "\n");
_LIT8 ( KNotFound, "Not Found" );
_LIT8 ( KUPnPCodecName, "HTTP/UPnP" );

// These should be moved to the string table
_LIT8 ( KSsdpAlive, "ssdp:alive" );
_LIT8 ( KSsdpBye, "ssdp:byebye" );
_LIT8 ( KSsdpAll, "ssdp:all" );
_LIT8 ( KSsdpDiscover, "\"ssdp:discover\"");
_LIT8 ( KSearchUri, "*" );
_LIT8 ( KSeperator, "::" );
_LIT8 ( KMultiCastAddr, "239.255.255.250:1900" );
_LIT8 ( KRootDevice, "upnp:rootdevice" );
_LIT8 ( KColon, ":" );
_LIT8 ( KUnderScore, "_" );
_LIT8 ( KUrnService, "urn:upnp-org:serviceId:" );
_LIT8 ( KUrnSchema, "urn:schemas-upnp-org" );
_LIT8 ( KHttpScheme, "http://" );

_LIT(KUuidString,   "uuid:");

const TUint KIPv6HostOpenBrace = '[';
const TUint KIPv6HostCloseBrace = ']';

const TUint KIdentifiersLen = sizeof ( TInt );

// Option names. Used for Get & Set options from the socket.
/*
Set the absolute URI
*/
const TUint KCHAbsoluteUri = 1;		// Note ! dont use 1 it is KSOLSocket used by TCP

/*
The maximum receive length that is expected. When it is called with GetOpt
it can return KErrUnknown otherwise a valid length
*/
const TUint KCHMaxLength = 2;

/*
The message is last or not
*/
const TUint KCHLastMessage = 3;

/*
Option level --- Always 0.
*/
const TUint KCHOptionLevel = 0; // Always 0.


const TUint KFirstRegistration = 1;
const TInt KSubscriptionCleanupFrequency = 1000000;

/* Udp Upnp Multicast Port*/
const TUint	KUpnpMCPort = 1900;
/* Udp Upnp Port Number; [REF] DLNA Requirement-7.2.3.4 */
const TUint	KUpnpUdpPort = 1025;
const TInt KInvalidSeqStatusCode = 610;
const TInt KRequestTimeoutStatusCode = 408;
const TInt KRootDeviceRepublishFrequency = 605436607; // rounded-conversion of 600seconds to clockticks

const TUint32 KCacheControl = 1800;
const TUint32 KSsdpMulticastAddr = INET_ADDR ( 239, 255, 255, 250 );
const TInt KPreConditionFailedStatusCode = 412;
const TInt KOkStatusCode = 200;


const TInt KMajorVersion = 1;
const TInt KMinorVersion = 1;

#endif /*UPNPSERVERCONSTANTS_H_*/
