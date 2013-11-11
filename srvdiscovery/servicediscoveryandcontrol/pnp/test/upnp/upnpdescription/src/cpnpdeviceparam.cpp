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

#include <e32cons.h>

#include "cpnpdeviceparam.h"
#include <upnpdescriptionschema.h>
#include "inetprottextutils.h"


EXPORT_C CUPnPIconInfo* CUPnPIconInfo::NewL()
    {
    CUPnPIconInfo* self = new (ELeave) CUPnPIconInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CUPnPIconInfo::CUPnPIconInfo()
    {

    }

void CUPnPIconInfo::ConstructL()
    {
    CUPnPProperty::ConstructL();
    }

EXPORT_C CUPnPIconInfo::~CUPnPIconInfo()
    {
    }

// Validation for icon
TBool CUPnPIconInfo::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) 
	{
	TBool result=EFalse;

	if(	(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EWidth, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EMimeType, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EHeight, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDepth, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EUrl, aStringTable)).Length()!=0) )
		{
		result = ETrue;
		}
		
	return result;
	}

//-------------------------------------------------------------------------

EXPORT_C CUPnPServiceInfo* CUPnPServiceInfo::NewL()
	{
	CUPnPServiceInfo* self = new (ELeave) CUPnPServiceInfo();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPServiceInfo::CUPnPServiceInfo()
	{
	
	}

void CUPnPServiceInfo::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}


EXPORT_C CUPnPServiceInfo::~CUPnPServiceInfo()
	{
	}

// Validation for service

TBool CUPnPServiceInfo::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) 
	{
	TBool result=EFalse;
	if(	(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EServiceType, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EServiceId, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EScpdUrl, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EContorlUrl, aStringTable)).Length()!=0) )
		{
		result = ETrue;
		}
		
	return result;

	}

// ------------------------------------------------------------------
EXPORT_C CUPnPDevice* CUPnPDevice::NewL()
	{
	CUPnPDevice* self = new (ELeave) CUPnPDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPDevice::CUPnPDevice()
	{
	}

void CUPnPDevice::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}

EXPORT_C CUPnPDevice::~CUPnPDevice()
	{
	iEmbeddedDeviceInfo.ResetAndDestroy();
	iIconInfo.ResetAndDestroy();
	iServiceInfo.ResetAndDestroy();
	}

// Validation for CUPnPDevice
TBool CUPnPDevice::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) 
	{
	TBool result=ETrue;
	TInt i;

	if(	(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EDeviceType, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EFriendlyName, aStringTable)).Length()!=0) &&
		(Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EUdn, aStringTable)).Length()!=0)  )
		{
		result = ETrue;
		}
	else
		result = EFalse;
	
	// Validate manufacturer attribute
	if (result & (Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EManufacturer, aStringTable)).Length()!=0) )
		result = ETrue;
	else
		result = EFalse;
	
	// Validate the model attribute
	if (result & (Property(aStringPool.String(UPNPDESCRIPTIONXMLTAGS::EModelName, aStringTable)).Length()!=0) )
		result = ETrue;
	else
		result = EFalse;
	
	if(result)
		{
		for(i=0;i<CountOfEmbeddedDeviceInfoTable();i++)
			{
			result = iEmbeddedDeviceInfo[i]->Validate(aStringPool,aStringTable);
			if(EFalse==result)
				break;
			}
		}

	if(result)
		{
		for(i=0;i<CountOfServiceInfoTable();i++)
			{
			result = iServiceInfo[i]->Validate(aStringPool,aStringTable);
			if(EFalse==result)
				break;
			}
		
		}
	if(result)
		{
		for(i=0;i<CountOfIconInfoTable();i++)
			{
			result = iIconInfo[i]->Validate(aStringPool,aStringTable);
			if(EFalse==result)
				break;
			}
		}
	return result;
	}

//-------------------------------------------------------------------
EXPORT_C CUPnPDeviceDescription* CUPnPDeviceDescription::NewL()
	{
	CUPnPDeviceDescription* self = new (ELeave) CUPnPDeviceDescription();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CUPnPDeviceDescription::CUPnPDeviceDescription()
	{
	}

void CUPnPDeviceDescription::ConstructL()
	{
	CUPnPProperty::ConstructL();
	}

EXPORT_C CUPnPDeviceDescription::~CUPnPDeviceDescription()
	{
	if(iDeviceObj!=NULL)
		delete iDeviceObj;
	}

// Validation for device description. It delegates the validation to the root device
TBool CUPnPDeviceDescription::Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const
	{
	
	if(iDeviceObj!=NULL)
		return iDeviceObj->Validate(aStringPool,aStringTable);
	else
		return EFalse;	
	
	}
