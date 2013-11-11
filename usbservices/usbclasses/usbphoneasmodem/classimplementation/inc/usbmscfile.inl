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
// Description: Implementation of client API of mass storage file server
// 
// 

#ifndef USBMSCFILE_INL
#define USBMSCFILE_INL


inline RUsbMscFile::RUsbMscFile()
	{
	// Intentionally left blank
	}

inline TVersion RUsbMscFile::Version() const
	{
	return ( TVersion( KUsbMsSrvMajorVersionNumber,
	                   KUsbMsSrvMinorVersionNumber,
	                   KUsbMsSrvBuildVersionNumber ) );
	}

inline TInt RUsbMscFile::Connect()
	{
    // 1: only a single session is required
#ifdef __T_MS_CLISVR__
   	static _LIT_SECURITY_POLICY_S0(KFileServerPolicy,0x101F7774);
#else
  	static _LIT_SECURITY_POLICY_S0(KFileServerPolicy,KFileServerUidValue);
  	static _LIT_SECURITY_POLICY_PASS(KPolicyPass);
#endif
  	return CreateSession( KMscFileServerName, 
  	                      Version(), 
  	                      1, 
  	                      EIpcSession_Unsharable,
  	                      &KPolicyPass,
  	                      0);
	}

inline TInt RUsbMscFile::SetupLogicalUnit( const TDesC& aFileName, 
                                        const TInt aProtocol, 
                                        const TInt aLun /* = 0 */ )

    {
    return SendReceive( EMscFileSetupLu, TIpcArgs( &aFileName, aProtocol, aLun ) );
    }
/**
 @capability NetworkControl 
 */
inline TInt RUsbMscFile::Start( const TMassStorageConfig& aMsConfig )
	{
	return SendReceive( EMscFileStart, TIpcArgs( &aMsConfig.iVendorId, 
	                                             &aMsConfig.iProductId, 
	                                             &aMsConfig.iProductRev ) );
	}

/**
 @capability NetworkControl 
 */	
inline TInt RUsbMscFile::Stop()
	{
	return SendReceive( EMscFileStop );
	}

/**
 @capability NetworkControl 
 */	
inline TInt RUsbMscFile::Shutdown()
	{
	return SendReceive(EMscFileShutdown);
	}

#endif  //USBMSCFILE_INL
