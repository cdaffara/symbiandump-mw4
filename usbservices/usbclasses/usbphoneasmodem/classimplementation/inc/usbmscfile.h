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
// Description: RUsbMscFile Client side header
//              Implements the Symbian OS USB mass storage server RUsbMscFile API 
// 

#ifndef USBMSCFILE_H
#define USBMSCFILE_H

#include <e32std.h>
#include "usbmscfileshared.h"

class RUsbMscFile : public RSessionBase
/**
 The RUsbMscFile class implements the Symbian OS USB mass storage RUsbMscFile API 
 
 @internalTechnology
 */
	{
public:
	/**
     Constructor
      
	 @internalTechnology
	 */
	inline RUsbMscFile();

	/**
     Extract the version of the server providing the RUsbMscFile API
     
     @return	Version of the server
     @internalTechnology
	 */
	inline TVersion Version() const;

	/**
	 * Setup a logical unit. Maps a file containing a file system image
	 * to a logical Mass Storage unit. The parameter aLun is reserved for 
	 * future expansion, currently only one unit with LUN = 0 is supported.
	 * 
	 * @param aFileName path to the file containing the file system image
	 * @param aProtocol type of SCSI protocol to use
	 * @param aLun logical unit number (LUN) to assign
     * @return KErrNone on success, otherwise system wide error code 
	 */
	inline TInt SetupLogicalUnit( const TDesC& aFileName, 
	                              const TInt aProtocol, 
	                              const TInt aLun = 0 );
	/**
     StartL the mass storage transport service
     
	 @param aMsConfig	mass storage configuration info
	 @internalTechnology
	 @return KErrNone on success, otherwise system wide error code 
	 */
	inline TInt Start( const TMassStorageConfig& aMsConfig );

	/**
     Stops mass storage transport service
     
	 @internalTechnology
	 @return KErrNone on success, otherwise system wide error code 
	 */
	inline TInt Stop();
	
	/**
     Shut down the Mass Storage server
     
	 @internalTechnology
	 @return KErrNone on success, otherwise system wide error code 
	 */
	inline TInt Shutdown();
	
	/**
	 Connects to mass storage file server
	 
	 @internalTechnology
	 @return KErrNone on success, otherwise system wide error code 
	 */
    inline TInt Connect();
    
	};

#include "usbmscfile.inl"

#endif //USBMSCFILE_H
