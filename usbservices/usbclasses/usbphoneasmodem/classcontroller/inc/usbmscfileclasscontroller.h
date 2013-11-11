// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Adheres to the UsbMan USB Class API and talks to mass storage file system
// 
// 

#ifndef USBMSCLASSCONTROLLER_H
#define USBMSCLASSCONTROLLER_H

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include "usbmscfile.h"

class MUsbClassControllerNotify;

const TInt KMsStartupPriority = 3;

	
/**
 The CUsbMscFileClassController class
 Implements the USB Class Controller API 
 */
NONSHARABLE_CLASS(CUsbMscFileClassController) : public CUsbClassControllerPlugIn
	{

public: 
	static CUsbMscFileClassController* NewL(MUsbClassControllerNotify& aOwner);
	~CUsbMscFileClassController();

private: 
	// Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private: 
	// Functions derived from CUsbClassControllerBase
	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

private:
	CUsbMscFileClassController(MUsbClassControllerNotify& aOwner);
	void ConstructL();

private:

    void ReadMassStorageConfigL();
    void ConfigItem(const TPtrC& source, TDes& target, TInt maxLength);
    void SetupUnitsL();

private:
    RUsbMscFile iMscFile;
	TMassStorageConfig iMsConfig;
	};

#endif //USBMSCLASSCONTROLLER_H
