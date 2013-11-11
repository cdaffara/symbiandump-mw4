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

#ifndef __CPNPSERVICEPUBLISHERBASE_H__
#define __CPNPSERVICEPUBLISHERBASE_H__

#include <pnp/pnp.h>

class RPnPParameterBundle;
class CControlChannelBase;

/** This class acts as an abstract base class which technology specific information need to implement
    @publishedPartner
    @prototype
 */
class CPnPServicePublisherBase : public CBase
	{
public:
	/** Static factory method */
	static CPnPServicePublisherBase* NewL( TUint aTierId );
    /**
	Default Destructor
	*/
	IMPORT_C virtual ~CPnPServicePublisherBase();	
	/** Second phase constructor */
	virtual TInt Construct( TUint aTierId) = 0;
	/**
	Publishes service notifications. The technology specific service information
	is packed in aServiceInfo.
	@param Parameter bundle containing service information.		
	*/
	virtual void Publish ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	Sends service notification.
    @param Parameter bundle contains the technology specific information.
	*/
	virtual void SendNotify ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**	  
	 It enables to send and receive messages.
	 */
	virtual CControlChannelBase* InitiateControlL () = 0;

protected:
    /**
	Default constructor
	*/
	IMPORT_C CPnPServicePublisherBase();
		
private:
	TUid iEcomDtorID;		
	};
	
#endif // __CPNPSERVICEPUBLISHERBASE_H__ 