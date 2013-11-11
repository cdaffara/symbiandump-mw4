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
// Description: Class declaration for CMscFileController.
// 
// 

#ifndef MSCFILECONTROLLER_H
#define MSCFILECONTROLLER_H

#include <e32base.h>
#include "usbmscfileshared.h"
#include "filesystemimage.h"
#include "mscfileserver.h"
#include "protocol.h"

/**
Mass Storage Controller class.
Encapsulates the drive manager, transport and protocol for USB Mass Storage.
Its main purpose is to instantiate and initialize these objects.
*/
class CMscFileController : public CBase
	{
public:
    static CMscFileController* NewL();
	~CMscFileController();
	
private:
    CMscFileController();
	void ConstructL();
	
public:
    void SetupLogicalUnitL( const TDesC& aFileName, 
                           const TInt aProtocol, 
                           const TInt aLun );
	TInt Start( TMassStorageConfig& aConfig );
	TInt Stop();
	void Reset();
	CFileSystemImage* FsImage( TInt aLun );
private:
	CFileSystemImage* iFsImage;
	MTransportBase* iTransport;
	MProtocolBase* iProtocol;
	TMassStorageConfig iConfig;
	TInt iMaxDrives;
	};

#endif //MSCFILECONTROLLER_H
