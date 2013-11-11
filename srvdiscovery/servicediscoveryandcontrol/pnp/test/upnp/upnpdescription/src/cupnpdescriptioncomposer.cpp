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

#include <cupnpdescriptioncomposer.h>
#include "cupnpservicecomposer.h"
#include "cupnpdevicecomposer.h"

EXPORT_C CUPnPDescriptionComposer* CUPnPDescriptionComposer::NewL(  const RStringPool& aStringPool )
	{
	return new ( ELeave ) CUPnPDescriptionComposer( aStringPool );
	}
EXPORT_C void CUPnPDescriptionComposer::ComposeDescriptionXmlL( TAny* aDescriptionObj , TUPnPDescriptionType aType , RBuf8 &aXmlData )
	{
	if( aType == CUPnPDescriptionComposer::EService )
		{
		
		iServiceComposer = CUPnPServiceXmlComposer::NewL( iStringPool );
		CUPnPServiceDescription* serviceDescObj = static_cast<CUPnPServiceDescription*> ( aDescriptionObj );
		iServiceComposer->ComposeServiceXmlL(serviceDescObj, aXmlData);
		}
	else 
		{
		iDeviceComposer = CUPnPDeviceXmlComposer::NewL(  iStringPool );
		CUPnPDeviceDescription* deviceDescObj = static_cast<CUPnPDeviceDescription*> ( aDescriptionObj );
		iDeviceComposer->ComposeDeviceXmlL(deviceDescObj, aXmlData);
		}
	}

EXPORT_C CUPnPDescriptionComposer::~CUPnPDescriptionComposer()
	{
	delete iServiceComposer;
	delete iDeviceComposer;
	}

CUPnPDescriptionComposer::CUPnPDescriptionComposer(	const RStringPool& aStringPool): iStringPool (aStringPool)
	{
	
	}
	
