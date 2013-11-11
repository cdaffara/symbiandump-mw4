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

#ifndef __CPNPSERVICEPUBLISHERBASE_H_
#define __CPNPSERVICEPUBLISHERBASE_H_

#include <rcontrolchannel.h>
#include <pnpparameterbundle.h>

class CPnPServicePublisherBase : public CBase
	{
public:
	static CPnPServicePublisherBase* NewL( TUint aTierId );
	IMPORT_C virtual ~CPnPServicePublisherBase();
	
	virtual TInt Construct( TUint aTierId) = 0;
	virtual void Publish ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual void SendNotify ( const RPnPParameterBundle& aServiceInfo ) = 0;
	virtual TInt InitiateControl ( RControlChannel& aControlChannel ) = 0;


protected:
	IMPORT_C CPnPServicePublisherBase();
		
private:
	TUid iEcomDtorID;
		
	};
	
#endif