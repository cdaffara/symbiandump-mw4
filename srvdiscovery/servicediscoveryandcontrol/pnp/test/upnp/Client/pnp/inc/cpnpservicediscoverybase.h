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

#ifndef __CPNPSERVICEDISCOVERYBASE_H_
#define __CPNPSERVICEDISCOVERYBASE_H_

#include <pnpparameterbundle.h>
#include <rcontrolchannel.h>

class CPnPServiceDiscoveryBase : public CBase
	{
public:
	static CPnPServiceDiscoveryBase* NewL( TUint aTierId );
	IMPORT_C virtual ~CPnPServiceDiscoveryBase();
	
	virtual TInt Construct( TUint aTierId) = 0;
	virtual void Discover ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual void Describe ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual void Subscribe ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual void RegisterNotify ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual void Cancel ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual TInt InitiateControl ( RControlChannel& aControlChannel, const TDesC8& aUri ) = 0;
	
protected:
	IMPORT_C CPnPServiceDiscoveryBase();

private:
	TUid iEcomDtorID;
	};

#endif