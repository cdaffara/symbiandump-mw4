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
// Description: CMscFileController implementation.
// 
// 

#include "mscfilecontroller.h"
#include "scsiprot.h"
#include "bulkonlytransport.h"
#include "debug.h"

CMscFileController* CMscFileController::NewL()
    {
    TRACE_FUNC
    CMscFileController* self = new (ELeave) CMscFileController;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
/**
Destructor
*/
CMscFileController::~CMscFileController()
	{
	delete iProtocol;
	delete iTransport;
	delete iFsImage;
	}

CMscFileController::CMscFileController()
    {
    }
/**
Creates the drive manager, transport, protocol and server

@param aMaxDrives Maximum number of Mass Storage drives supported.
*/
void CMscFileController::ConstructL()
	{
	TRACE_FUNC_ENTRY
	iMaxDrives = 1; 
    TRACE_INFO(( _L( "Creating transport" ) ))
    iTransport = CBulkOnlyTransport::NewL(iMaxDrives, *this);
	TRACE_FUNC_EXIT
	}

void CMscFileController::SetupLogicalUnitL( const TDesC& aFileName, 
                                            const TInt /*aProtocol*/, 
                                            const TInt aLun )
    {
    TRACE_FUNC_ENTRY
    if ( aLun != 0 )
        {
        // More than one logical unit is not supported in this version
        LEAVE( KErrArgument );
        }
    if(iFsImage)
        {
        delete iFsImage;
        iFsImage = NULL;
        }
    iFsImage = CFileSystemImage::NewL( aFileName );

    TRACE_INFO(( _L( "Creating protocol" ) ))
    //create protocol according to aProtocol
    if(iProtocol)
        {
        delete iProtocol;
        iProtocol = NULL;
        }
    iProtocol = CScsiProtocol::NewL( *this );
    iTransport->RegisterProtocol( *iProtocol );
    iProtocol->RegisterTransport( iTransport );
    TRACE_FUNC_EXIT
    }

/**
Starts the transport and initializes the protocol.

@param aConfig Reference to Mass Storage configuration data
*/
TInt CMscFileController::Start( TMassStorageConfig& aConfig )
	{
	TRACE_FUNC_ENTRY
	//Save this value for use in the Reset method.
	iConfig = aConfig;
	TInt err = KErrNotReady;
	if ( iProtocol && iTransport )
		{
		TRACE_INFO(( _L( "Starting" ) ))
		((CScsiProtocol*)iProtocol)->SetScsiParameters(aConfig);
		err = iTransport->Start();
		}

	TRACE_FUNC_EXIT
	return err;
	}

/**
Stops the transport.
*/
TInt CMscFileController::Stop()
	{
	TRACE_FUNC_ENTRY
	TInt err = KErrNotReady;
	if ( iTransport )
		{
		TRACE_INFO(( _L( "Stopping" ) ))
		err = iTransport->Stop();
		}
	TRACE_FUNC_EXIT
	return err;
	}

/**
Delete the transport and protocol and start new ones.
*/
void CMscFileController::Reset()
	{
	TRACE_FUNC
	delete iProtocol;
	iProtocol = NULL;

	//Create transport and protocol and initialize them
	TRACE_INFO(_L("Creating  protocol"));

	TRAPD(err,iProtocol = CScsiProtocol::NewL(*this));
	err = err;
	__ASSERT_DEBUG(err==KErrNone, User::Invariant());
	iTransport->RegisterProtocol(*iProtocol);
	iProtocol->RegisterTransport(iTransport);
	}

CFileSystemImage* CMscFileController::FsImage( TInt aLun )
    {
    // Only 1 LUN supported in this version
    if ( aLun == 0 )
        {
        return iFsImage;
        }
    else
        {
        return NULL;
        }
    }


