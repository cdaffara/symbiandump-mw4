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


#ifndef __RPNPPSERVICEPUBLISHER_H__
#define __RPNPPSERVICEPUBLISHER_H__
#include <cpnpservicepublisherbase.h>

/**
A generic class which provides the functionality, to be implemented by a service point to service the remote clients.
@publishedPartner
@prototype
*/
class RPnPServicePublisher
{
	public:
	/**
	Default constructor
	*/
	IMPORT_C RPnPServicePublisher ( );

	/**
	Resolves the access point.
	@param aTierID implementation UID. Identifies technology for which service discovery is requested.
	@return KErrNone if successful. Returns system wide error codes on failure
	*/
	IMPORT_C TInt Open (TUint aTierId );

	/**
	Publishes service notifications to remote clients. The technology specific service information
	is packed in aServiceInfo.
	Note that a service point will only publish a single device or service.Consequently only one
	parameter set must be sent in the bundle.Also once published,the service point must be closed
	before another publish is done.
	
	The uris for device or service publish must conform to the schema specified in UPnP Device Architecture 1.0

	When the responses are received, MPnPObserver::OnPnPEventL is called for each response and calling clients
	must provide the implementation to handle the responses.

	In case of corrupted uris or value fields or low memory conditions, MPnPObserver::OnPnPError is called
	synchronously which must be handled by the client

	In UPnP implementation the publish notifications are sent to SSDP multicast address on UDP channel.
	@param Parameter bundle containing service information.
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 2 Invalid observer object supplied. It may be null or corrupted
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
	*/
	IMPORT_C void Publish ( const RPnPParameterBundle& aServiceInfo );

	/**
	Sends service notification to remote clients who've subscribed to the service/device

	No responses are received in MPnPObserver::OnPnPEventL and MPnPObserver::OnPnPError is also not called
	@param Parameter bundle contains the technology specific information.
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
	*/
	IMPORT_C void SendNotify ( const  RPnPParameterBundle & aServiceInfo );

	/**
	 Starts the Connection and attaches the sub-connection to it.This also
	 creates the socket and enables it to send and receive messages.
	 @return KErrNone if successful. Returns system wide error codes on failure
	 */
	IMPORT_C TInt InitiateControl ( RControlChannel& aControlChannel );

	/**
	Close technology specific session.
	In UPnP it sends SSDP::ByeBye notifications to SSDP multicast adress.
	*/
	IMPORT_C void Close ( );

	private:
		 CPnPServicePublisherBase* iUPnPServicePublisherImpl;
		 
};

#endif //__RPNPPSERVICEPUBLISHER_H__
