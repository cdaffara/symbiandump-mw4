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
// Description: Adheres to the UsbMan USB Class Controller API and talks to mass storage file server
// 
// 

#include <barsc.h> 
#include <barsread.h>
#include <usb_std.h>
#include <cusbclasscontrollerplugin.h>
#include <centralrepository.h>
#include <usbpammscfile.rsg>
#include <data_caging_path_literals.hrh> 
#include "usbmscfileclasscontroller.h"
#include "usbmscfile.h"
#include "usbmscfileclasscontroller_crkeys.h"
#include "debug.h"
 
// Panic category 
#ifdef _DEBUG
_LIT( KMsCcPanicCategory, "UsbMscFileCc" );
#endif

_LIT( KUsbMsResource, "\\private\\101fe1db\\usbpammscfile.rsc" );

/**
 Panic codes for the USB MSC File Class Controller.
 */
enum TMscFileCcPanic
	{
	//Class called while in an illegal state
	EBadApiCall = 0,
    EUnusedFunction = 1,
	};


static TInt CreateServerProcess()
    {
    TRACE_FUNC

    RProcess server;
    TInt result = server.Create( KMscFileServerFileName, KNullDesC );
    if ( result != KErrNone )
        {
        return  result;
        }

    TRequestStatus stat;

    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 ); // abort startup
        }    
    else
        {
        server.Resume(); // logon OK - start the server
        server.SetPriority( EPriorityForeground );
        }
    User::WaitForRequest( stat ); // wait for start or death
  
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    result = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();    
    
    server.Close();

    return result;
    }

/**
 Start MSC File Server
 */
static TInt StartServer()
    {
    TInt result;

    TFindServer findMscFileServer( KMscFileServerName );
    TFullName name;

    result = findMscFileServer.Next( name );
    if ( result != KErrNone )
        {       
        result = CreateServerProcess();     
        }

    TRACE_INFO(( _L( "Server process created: %d" ), result ))
    return result;
    }

/**
 Constructs a CUsbMscFileClassController object
 
 @param	aOwner	USB Device that owns and manages the class
 @return	A new CUsbMscFileClassController object
 */
CUsbMscFileClassController* CUsbMscFileClassController::NewL(
	MUsbClassControllerNotify& aOwner)
	{
	TRACE_FUNC

	CUsbMscFileClassController* r = new (ELeave) CUsbMscFileClassController(aOwner);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

/**
 Destructor
 */
CUsbMscFileClassController::~CUsbMscFileClassController()
	{
	Cancel();
	}

/**
 Constructor.
 
 @param	aOwner	USB Device that owns and manages the class
 */
CUsbMscFileClassController::CUsbMscFileClassController(
	MUsbClassControllerNotify& aOwner )
	: CUsbClassControllerPlugIn( aOwner, KMsStartupPriority )	
	{
	// Intentionally left blank
	}

/**
 2nd Phase Construction.
 */
void CUsbMscFileClassController::ConstructL()
	{
	TRACE_FUNC

	ReadMassStorageConfigL();
	}

/**
 Called by UsbMan when it wants to start the mass storage class. 
 
 @param aStatus The caller's request status, filled in with an error code
 */
void CUsbMscFileClassController::Start( TRequestStatus& aStatus )
	{
	TRACE_FUNC_ENTRY
	
	// The service state should always be idle when this function is called 
	// (guaranteed by CUsbSession).
	__ASSERT_DEBUG( iState == EUsbServiceIdle, 
	        User::Panic( KMsCcPanicCategory, EBadApiCall ) );

	TRequestStatus* reportStatus = &aStatus;

	iState = EUsbServiceStarting;

    TInt ret = StartServer();
    if ( ret != KErrNone )
        {
		iState = EUsbServiceIdle;
		User::RequestComplete( reportStatus, ret );
        TRACE_ERROR(( _L( "Error %d Can't start server." ), ret ))
        return;
        }
    
    TRAP( ret, SetupUnitsL() );
    if (ret != KErrNone)
        {
		iState = EUsbServiceIdle;
		User::RequestComplete( reportStatus, ret );
        TRACE_ERROR(( _L( "Error %d in SetupUnitsL." ), ret ))
        return;
        }


	// Connect to USB Mass Storage server
	ret = iMscFile.Connect();

	if ( ret != KErrNone )
		{
		iState = EUsbServiceIdle;
		User::RequestComplete( reportStatus, ret );
		TRACE_ERROR(( _L( "Error %d connecting to msc file server" ), ret ))
		return;
		}

	// StartL mass storage device
	ret = iMscFile.Start( iMsConfig );

	if (ret != KErrNone)
		{
		iState = EUsbServiceIdle;
		User::RequestComplete( reportStatus, ret );
		TRACE_ERROR(( _L( "Error %d starting msc file server" ), ret ))
		return;
		}

	iState = EUsbServiceStarted;

	User::RequestComplete(reportStatus, KErrNone);
	TRACE_FUNC_EXIT
	}

/**
 Called by UsbMan when it wants to stop the USB ACM class.
 
 @param aStatus KErrNone on success or a system wide error code
 */
void CUsbMscFileClassController::Stop( TRequestStatus& aStatus )
	{
	TRACE_FUNC_ENTRY
	
	// The service state should always be started when this function is called
	// (guaranteed by CUsbSession)
	__ASSERT_DEBUG( iState == EUsbServiceStarted, 
	                User::Panic( KMsCcPanicCategory, EBadApiCall ) );

	TRequestStatus* reportStatus = &aStatus;
	
	TInt ret = iMscFile.Stop();
	
	if ( ret != KErrNone )
		{
		iState = EUsbServiceStarted;
		User::RequestComplete( reportStatus, ret );
		TRACE_ERROR(( _L( "Error %d stopping msc file server" ), ret ))
		return;
		}	

	ret = iMscFile.Shutdown();
	if ( ret != KErrNone )
		{
		User::RequestComplete( reportStatus, ret );
		TRACE_ERROR(( _L( "Error %d stopping msc file server" ), ret ))
		}	
  else
	  {
		User::RequestComplete( reportStatus, KErrNone );
	  }    
	iMscFile.Close();
	
	iState = EUsbServiceIdle;
	TRACE_FUNC_EXIT
	}

/**
 Gets information about the descriptor which this class provides. Never called
 by usbMan.
 
 @param aDescriptorInfo Descriptor info structure filled in by this function
 */
void CUsbMscFileClassController::GetDescriptorInfo(TUsbDescriptor& /*aDescriptorInfo*/) const
	{
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction));
	}

/**
 Standard active object RunL. Never called because this class has no
 asynchronous requests.
 */
void CUsbMscFileClassController::RunL()
	{
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	}

/**
 Standard active object cancellation function. Never called because this
 class has no asynchronous requests.
 */
void CUsbMscFileClassController::DoCancel()
	{
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	}

/**
 Standard active object error function. Never called because this class has
 no asynchronous requests, and hence its RunL is never called.
 
 @param aError The error code (unused)
 @return Always KErrNone to avoid an active scheduler panic
 */
TInt CUsbMscFileClassController::RunError(TInt /*aError*/)
	{
	__ASSERT_DEBUG( EFalse, User::Panic(KMsCcPanicCategory, EUnusedFunction) );
	return KErrNone;
	}

/**
 Read mass storage configuration info from the resource file
 */
void CUsbMscFileClassController::ReadMassStorageConfigL()
	{
	TRACE_FUNC_ENTRY

	// Try to connect to file server
	RFs fs;
	LEAVE_IF_ERROR( fs.Connect() );
	CleanupClosePushL( fs );

    TFileName fileName;
    const TDriveNumber KResourceDrive = EDriveZ;
    TDriveUnit driveUnit( KResourceDrive );
    TDriveName drive = driveUnit.Name();
    fileName.Insert( 0, drive );

    fileName += KUsbMsResource;

	RResourceFile resource;
	TRAPD( err, resource.OpenL( fs, fileName ) );

	if ( err != KErrNone )
		{
		TRACE_ERROR(( _L( "Error %d opening resource file" ), err ))
		CleanupStack::PopAndDestroy( &fs );
		return;
		}

	CleanupClosePushL( resource );

	resource.ConfirmSignatureL( KUsbMsResourceVersion );

	HBufC8* msConfigBuf = 0;
	TRAPD( ret, msConfigBuf = resource.AllocReadL( USBMS_CONFIG ) );
	if ( ret != KErrNone )
		{
		TRACE_ERROR(( _L( "Error %d opening mass storage config" ), ret ))
		CleanupStack::PopAndDestroy(2, &fs); 
		return;
		}
	CleanupStack::PushL( msConfigBuf );
	
	// The format of the USB resource structure is:
	
	/* 	
	 * 	STRUCT USBMASSSTORAGE_CONFIG
	 *	{
	 *	LTEXT	vendorId;           // no more than 8 characters
	 *	LTEXT	productId;          // no more than 16 characters
	 *	LTEXT	productRev;        	// no more than 4 characters
	 *	};
	 */
	 
	// Note that the resource must be read in this order!
	
	TResourceReader reader;
	reader.SetBuffer( msConfigBuf );

	TPtrC	vendorId		= reader.ReadTPtrC();
	TPtrC	productId		= reader.ReadTPtrC();
	TPtrC	productRev		= reader.ReadTPtrC();
	
	// populate iMsConfig, truncate if exceeding limit
	ConfigItem( vendorId, iMsConfig.iVendorId, 8 );
	ConfigItem( productId, iMsConfig.iProductId, 16 );
	ConfigItem( productRev, iMsConfig.iProductRev, 4 );
	
	// Debugging
	TRACE_INFO(( _L( "vendorId = %s" ),   &vendorId ))
	TRACE_INFO(( _L( "productId = %s" ),  &productId ))
	TRACE_INFO(( _L( "productRev = %s" ), &productRev ))
		
	CleanupStack::PopAndDestroy( 3, &fs ); // msConfigBuf, resource, fs
	TRACE_FUNC_EXIT
	}
	
/**
 Utility. Copies the data from TPtr to TBuf and checks data length 
 to make sure the source does not exceed the capacity of the target
 */
 void CUsbMscFileClassController::ConfigItem( const TPtrC& source, 
                                              TDes& target, 
                                              TInt maxLength )
 	{
 	if ( source.Length() < maxLength )
 		{
 		maxLength = source.Length();
 		}
 		
 	target.Copy( source.Ptr(), maxLength );	 	
 	}

void CUsbMscFileClassController::SetupUnitsL()
    {
    RFs fs;
    RUsbMscFile mscFile;
    
    LEAVE_IF_ERROR( mscFile.Connect() );
    CleanupClosePushL( mscFile );
    CRepository* repository = CRepository::NewLC(KCRUidUsbPhoneAsModemMscFileClassController);
    
	RArray<TUint32> lunKeys;
	CleanupClosePushL(lunKeys);
	
	repository->FindL(KCRMscFileLogicalUnitNumberColumn, KCRMscFileColumnMask, lunKeys);

	
	TInt count = lunKeys.Count();
	for (TInt i=0; i<count; i++)
		{
		TUint32 lunKey = lunKeys[i];
		TUint32 fullNameKey = ((lunKey&KCRMscFileRowMask)|KCRMscFileImageFullNameColumn);
		TRACE_INFO( (_L( "lunKey=%d,fullNameKey=%d" ), lunKey, fullNameKey) )

		TInt lun = 0;
		LEAVE_IF_ERROR(repository->Get(lunKey, lun));
		
		TFileName fullName;
		LEAVE_IF_ERROR(repository->Get(fullNameKey, fullName));	
		
		TRACE_INFO( (_L("lun=%d, fullName=%S"), lun, &fullName) );
	
		TInt protocol = 0; // It is not used. Bulk Transport(0x50), SCSI transparent command Set(0x06), PDT CD/DVD device(0x05) is supported only.
		
		LEAVE_IF_ERROR(mscFile.SetupLogicalUnit( fullName, protocol, lun ));
		}
    CleanupStack::PopAndDestroy( 3 ); // mscFile, repository, lunKeys
    }
