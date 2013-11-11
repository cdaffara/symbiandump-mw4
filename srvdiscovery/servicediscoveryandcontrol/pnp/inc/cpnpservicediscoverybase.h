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

#ifndef __CPNPSERVICEDISCOVERYBASE_H__
#define __CPNPSERVICEDISCOVERYBASE_H__

#include <pnp/pnp.h>

class RPnPParameterBundle;
class CControlChannelBase;

/** This class acts as an abstract base class which technology specific information need to implement
    @publishedPartner
    @prototype
 */
class CPnPServiceDiscoveryBase : public CBase
	{
public:
	/** Static factory method */
	static CPnPServiceDiscoveryBase* NewL( TUint aTierId ); 	
    /**
	Default Destructor
	*/ 
	IMPORT_C virtual ~CPnPServiceDiscoveryBase();
	/** Second phase constructor */
	virtual TInt Construct( TUint aTierId) = 0;
	/**
	Issues a discovery request. The technology specific discovery information is packed
	in aServiceInfo parameter.
	@param aServiceInfo Parameter bundle contains the discovery information.
	*/	
	virtual void Discover ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	Retrieves the description of the service. The technology specific description information is packed
	in aServiceInfo parameter.
	@param Parameter bundle contains the information about the service.
	*/
	virtual void Describe ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	Subscribes for event notifications to the service .
	The technology specific description information is packed in aServiceInfo parameter.
	@param Parameter bundle contains the information about the service.	
	*/
	virtual void Subscribe ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	Registers for events of service. The technology specific registration information
	is packed in aServiceInfo.
	@param Parameter bundle contains the register information.
	*/
	virtual void RegisterNotify ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	Issues a cancel request.
	@param Parameter bundle contains the cancel request.
	*/
	virtual void Cancel ( const RPnPParameterBundle& aServiceInfo ) = 0;
	/**
	It enables to send and receive messages.
	*/
	virtual CControlChannelBase* InitiateControlL ( const TDesC8& aUri ) = 0;
	
protected:  
    /**
	Default constructor
	*/
	IMPORT_C CPnPServiceDiscoveryBase();

private:
	TUid iEcomDtorID;
	};

#endif //__CPNPSERVICEDISCOVERYBASE_H__