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

#ifndef __CPNPDEVICEPARAM_H_
#define __CPNPDEVICEPARAM_H_

#include <e32cmn.h>
#include <e32base.h>
#include "cupnpproperty.h"

class CUPnPServiceInfo;
class CUPnPIconInfo;
class CUPnPDevice;

/** 
Container for device object. 

@publishedPartner
@prototype */

class CUPnPDeviceDescription: public CUPnPProperty
	{
public:

	IMPORT_C static CUPnPDeviceDescription* NewL();
	IMPORT_C ~CUPnPDeviceDescription();

    inline void SetDeviceObjectL( CUPnPDevice *aValue);
    inline  CUPnPDevice* DeviceObject() const;

    inline void SetMinorNumberL(const TInt aValue);
    inline const TInt MinorNumber () const;

    inline void SetMajorNumberL(const TInt aValue);
    inline const TInt MajorNumber () const;
	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) const;

private :

	CUPnPDeviceDescription();
	void ConstructL();

private :
	TVersion 		iVersion;
	CUPnPDevice	*iDeviceObj;

	};


/** 
Describes a device object.
Can contain embedded device, service and icon info

@publishedPartner
@prototype */

class CUPnPDevice: public CUPnPProperty
	{

public:
	IMPORT_C static CUPnPDevice* NewL();
	IMPORT_C ~CUPnPDevice();

    inline void AppendToEmbeddedDeviceInfoTableL(const CUPnPDevice* aValue);
    inline TInt CountOfEmbeddedDeviceInfoTable () const;
    inline CUPnPDevice* AtEmbeddedDeviceInfoTable(const TInt aIndex) const;
    inline void DeleteEmbeddedDeviceInfoAtIndexL(const TInt aIndex);

    inline void AppendToServiceInfoTableL(const CUPnPServiceInfo* aValue);
    inline TInt CountOfServiceInfoTable () const;
    inline CUPnPServiceInfo* AtServiceInfoTable(const TInt aIndex) const;
    inline void DeleteServiceInfoAtIndexL(const TInt aIndex);

    inline void AppendToIconInfoTableL(const CUPnPIconInfo* aValue);
    inline TInt CountOfIconInfoTable () const;
    inline CUPnPIconInfo* AtIconInfoTable(const TInt aIndex) const;
    inline void SetDlna( TBool aValue);
    inline TBool Dlna() const ;
	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable );
private :

	CUPnPDevice();
	void ConstructL();

private :
	TBool iHasDlnaTag;
	RPointerArray<CUPnPDevice>			iEmbeddedDeviceInfo;
	RPointerArray<CUPnPServiceInfo> 		iServiceInfo;
	RPointerArray<CUPnPIconInfo> 			iIconInfo;
	};


/** 
Following structure store service information.

@publishedPartner
@prototype */
class CUPnPServiceInfo: public CUPnPProperty
	{

public:
	IMPORT_C static CUPnPServiceInfo* NewL();
	IMPORT_C ~CUPnPServiceInfo();
	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) ;

private :

	CUPnPServiceInfo();
	void ConstructL();

private:
	};

/** 

CUPnPIconInfo stores icon information.

@publishedPartner
@prototype */	
	
class CUPnPIconInfo: public CUPnPProperty
	{

public:
	IMPORT_C static CUPnPIconInfo* NewL();
	IMPORT_C ~CUPnPIconInfo();

	TBool Validate(const RStringPool& aStringPool, const TStringTable& aStringTable ) ;
private :

	CUPnPIconInfo();
	void ConstructL();
	};

#include "cpnpdeviceparam.inl"
#endif /*CPNPDEVICEPARAM_H_*/
