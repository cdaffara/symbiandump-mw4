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

void CUPnPDevice::AppendToServiceInfoTableL(const CUPnPServiceInfo* aValue)
	{
	iServiceInfo.AppendL(aValue);
	}

TInt CUPnPDevice::CountOfServiceInfoTable () const
	{
	return iServiceInfo.Count();
	}

CUPnPServiceInfo* CUPnPDevice::AtServiceInfoTable ( TInt aIndex) const
	{
	return iServiceInfo[aIndex];
	}

void CUPnPDevice::DeleteServiceInfoAtIndexL(const TInt aIndex)
	{
	if( aIndex>=0 && aIndex<iServiceInfo.Count() )
		{
		// first delete the object and then remove it from the array
		delete iServiceInfo[aIndex];
		iServiceInfo.Remove(aIndex);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}
void CUPnPDevice::AppendToEmbeddedDeviceInfoTableL(const CUPnPDevice* aValue)
	{
	iEmbeddedDeviceInfo.AppendL(aValue);
	}

TInt CUPnPDevice::CountOfEmbeddedDeviceInfoTable () const
	{
	return iEmbeddedDeviceInfo.Count();
	}

CUPnPDevice* CUPnPDevice::AtEmbeddedDeviceInfoTable(const TInt aIndex) const
	{
	return iEmbeddedDeviceInfo[aIndex];
	}
void CUPnPDevice::DeleteEmbeddedDeviceInfoAtIndexL(const TInt aIndex)
	{
	if( aIndex>=0 && aIndex<iEmbeddedDeviceInfo.Count() )
		{
		// first delete the object and then remove it from the array
		delete iEmbeddedDeviceInfo[aIndex];
		iEmbeddedDeviceInfo.Remove(aIndex);
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	}
void CUPnPDevice::AppendToIconInfoTableL(const CUPnPIconInfo* aValue)
	{
	TRAPD(err, iIconInfo.Append(aValue));
	TInt xx=0;
	}

TInt CUPnPDevice::CountOfIconInfoTable() const
	{
	return iIconInfo.Count();
	}

CUPnPIconInfo* CUPnPDevice::AtIconInfoTable( TInt aIndex) const
	{
	return iIconInfo[aIndex];
	}
void CUPnPDevice::SetDlna( TBool aValue)
	{
	iHasDlnaTag = aValue;
	}

TBool CUPnPDevice::Dlna() const
	{
	return iHasDlnaTag;
	}

//-------------------------------------------------------------------


void CUPnPDeviceDescription::SetDeviceObjectL( CUPnPDevice *aValue)
	{
	iDeviceObj = aValue;
	}

CUPnPDevice* CUPnPDeviceDescription::DeviceObject() const
	{
	return iDeviceObj;
	}


void CUPnPDeviceDescription::SetMinorNumberL(const TInt aValue)
	{
	iVersion.iMinor = aValue;
	}

const TInt CUPnPDeviceDescription::MinorNumber () const
	{
	return iVersion.iMinor;
	}

void CUPnPDeviceDescription::SetMajorNumberL(const TInt aValue)
	{
	iVersion.iMajor = aValue;
	}

const TInt CUPnPDeviceDescription::MajorNumber () const
	{
	return iVersion.iMajor;
	}


