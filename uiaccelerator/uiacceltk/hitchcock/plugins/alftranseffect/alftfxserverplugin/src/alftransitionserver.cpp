/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This is the implementation of the API used by theme server
*                to control transition appearance.
*
*/



#include <s32mem.h>
#include <TfxSrvEcomIds.h>
#include <f32file.h>

#include <alf/AlfTransEffectPlugin.h>
#include <mtransitionservercontroller.h>
#include "alftfxserverdef.h"
#include "alftransitionserver.h"
#include "alflogger.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfTransitionServerClient::CAlfTransitionServerClient():
iPluginImplementation( TUid::Null() )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::CAlfTransitionServerClient - starting");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfTransitionServerClient::~CAlfTransitionServerClient()
	{
	// We don't unload plugin here, we let CGfxTransAdapterTfx keep it.
	// Maybe CGfxTransAdapterTfx should not unload it either?
	// We assume that when two clients tell RAlfTfxClient to load the plugin,
	// we always get the same instance of the plugin as it is loaded in the
	// server side and runs in the server thread.
	__ALFFXLOGSTRING("CAlfTransitionServerClient::~CAlfTransitionServerClient >>");
	iTfxServer.Close();
	iConnected = EFalse;
	iHasPlugin = EFalse;
	delete iTransferBuffer;
	delete iReturnBuffer;
	iFs.Close();
	__ALFFXLOGSTRING("CAlfTransitionServerClient::~CAlfTransitionServerClient <<");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::Connect()
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::Connect >>");
	TInt err = KErrNone;
	TInt fsErr = KErrNone;
	if ( !iTransferBuffer )
	    {
        iTransferBuffer = HBufC8::New( 1024 );
	    }
	if ( !iReturnBuffer )
	    {
	    // This is actually not used for anything
        iReturnBuffer = HBufC8::New( 10 );
	    }
	    
	if ( !iTransferBuffer || !iReturnBuffer )
	    {
	    __ALFFXLOGSTRING("CAlfTransitionServerClient::Connect - return KErrNoMemory");
	    return KErrNoMemory;
	    }

	if ( iPluginImplementation == TUid::Null() )
	    {
        RImplInfoPtrArray pluginArray;
        TRAP( err, REComSession::ListImplementationsL( KAlfGfxPluginInterfaceUId, pluginArray ) );
    	if ( pluginArray.Count() > 0 )
    	    {
    	    iPluginImplementation = pluginArray[0]->ImplementationUid();
    	    }
	    }
	
	if( !iConnected )
	    {
        err = iTfxServer.Open();	
        if ( err == KErrNone )
            {
            iConnected = ETrue;
            }
	    }
	    
	if ( iConnected && !iHasPlugin )
	    {
        if( iPluginImplementation != TUid::Null() )
            {
            err = iTfxServer.LoadTfxPlugin( iPluginImplementation );
            // 
            if ( err == KErrNone || err == KErrAlreadyExists )
                {
                iHasPlugin = ETrue;
                err = KErrNone;
                // RFs is needed only to allow replacing drive given in manifest file
                fsErr = iFs.Connect();
                }
            __ALFFXLOGSTRING("CAlfTransitionServerClient Has Plugin!");
            }
	    }
	// We lie about the result.
	// At the time being starting alf is taking way too long.
	// We must retry later. 
	// We only return error if all is well except we cannot connect to the file server
   
	__ALFFXLOGSTRING1("CAlfTransitionServerClient::Connect - return %d ", fsErr);
	return fsErr;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfTransitionServerClient::Disconnect() 
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::Disconnect >>");
	iTfxServer.Close();
	iConnected = EFalse;
	iHasPlugin = EFalse;
	__ALFFXLOGSTRING("CAlfTransitionServerClient::Disconnect <<");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::Shutdown(TThreadId& /*aThreadId*/)
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::Shutdown >>");
    // we don't have a separate server to shut down as our server side
    // is actually alf. only the plugin could be destroyed.
    Disconnect();
    __ALFFXLOGSTRING("CAlfTransitionServerClient::Shutdown <<");
	return KErrNone;
    }

// ---------------------------------------------------------------------------
// Checks if the system is connected.
// ---------------------------------------------------------------------------
//
TBool CAlfTransitionServerClient::IsConnected()
	{
	__ALFFXLOGSTRING1("CAlfTransitionServerClient::IsConnected - return %d", iHasPlugin);
	// We are not properly connected unless we also have managed to load the plugin
	return iHasPlugin;	
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::RegisterFullscreenKml(	TUint aActionID, const TUid& aUid,
														const TDesC& aResourceDir,
														const TDesC& aFilename )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterFullscreenKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
   
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	RThread me;
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpAddFullscreenKML;
    err =  FindEffectFile( aResourceDir, aFilename ); 
	TRAP( err, 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteUint32L( aActionID );
		inBuf.WriteInt32L( aUid.iUid );
		TUint64 id = me.Id();
		TUint8* ptr = reinterpret_cast<TUint8*>( &id );
		inBuf.WriteL( ptr, 8 );
//		inBuf << aResourceDir;
        inBuf << iParse.DriveAndPath();
		inBuf << aFilename;
        inBuf.CommitL();
		});
		
	inBuf.Release();
	inBuf.Close();	
	
	if ( err == KErrNone )
	    {
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
	    }

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterFullscreenKml - return %d <<", err);
    return err;
	}
	 
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::UnregisterFullscreenKml(TUint aActionID, const TUid& aUid)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::UnregisterFullscreenKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpRemoveFullscreenKML;
	// These writes cannot leave
	TRAP_IGNORE(
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteUint32L( aActionID );
		inBuf.WriteInt32L( aUid.iUid );
        inBuf.CommitL();		
		});
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::UnregisterFullscreenKml - return %d ", err);
    return err;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::RegisterListBoxKmlL(const TUid& aUid,
                        const TListBoxType aListBoxType, const TDesC& aResourceDir,
                        const TDesC& aBackgroundFilename, const TDesC& aListItemFilename)
    {
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterListBoxKmlL >>");
    TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	
	RThread me;
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpListBoxRegisterKML;
    err = FindEffectFile( aResourceDir, aBackgroundFilename );
	TRAP( err, 
	    {
        inBuf.WriteInt32L( op );
    	inBuf.WriteInt32L( aUid.iUid );
    	TUint64 id = me.Id();
    	TUint8* ptr = reinterpret_cast<TUint8*>( &id );
    	inBuf.WriteL( ptr, 8 );
        inBuf.WriteInt32L( ( TInt32 )aListBoxType );
        inBuf << iParse.DriveAndPath();
//    	inBuf << aResourceDir;
    	inBuf << aBackgroundFilename;
    	inBuf << aListItemFilename;
        inBuf.CommitL();
	    });
    
	inBuf.Release();
	inBuf.Close();	
	
	if ( err == KErrNone )
	    {
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
	    }

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterListBoxKmlL return: %d ", err);
    return err;
    }

TInt CAlfTransitionServerClient::RegisterListBoxKml(const TUid& aUid,
                                 const TListBoxType aListBoxType, const TDesC& aResourceDir,
                                 const TDesC& aBackgroundFilename, const TDesC& aListItemFilename)
    {
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterListBoxKml >>");
    TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
    TRAP( err, RegisterListBoxKmlL( aUid, aListBoxType, aResourceDir,
                                     aBackgroundFilename, aListItemFilename ) );

    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterListBoxKml - return %d ", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::UnregisterListBoxKml(const TUid& aUid, const TListBoxType aListBoxType)
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::UnregisterListBoxKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpListBoxUnregisterKML;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aUid.iUid );
        inBuf.WriteInt32L( ( TInt32 )aListBoxType );
        inBuf.CommitL();		
		} );
		
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::UnregisterListBoxKml - return %d ", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::SetListBoxFrameTime( const TInt aWantedTime )
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::SetListBoxFrameTime >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpListBoxSetFrameTime;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aWantedTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetListBoxFrameTime - return: %d <<", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::SetListBoxMinFrameTime( const TInt aMinTime )
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::SetListBoxMinFrameTime >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpListBoxSetMinFrameTime;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aMinTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetListBoxMinFrameTime - return: %d <<", err );
    return err;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::SetControlFrameTime( const TInt aWantedTime )
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::SetControlFrameTime >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlSetFrameTime;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aWantedTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetControlFrameTime - return: %d <<", err);
    return err;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::SetControlMinFrameTime( const TInt aMinTime )
    {
    __ALFFXLOGSTRING("CAlfTransitionServerClient::SetControlMinFrameTime >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlSetMinFrameTime;
	// These writes cannot leave
	TRAP( err, 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aMinTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetControlMinFrameTime - return: %d <<", err );
    return err;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::BlockFullScreenUid(const TUid& aUid, TBool aBlockFrom, TBool aBlockTo )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::BlockFullScreenUid >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpBlockFullscreenUid;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aUid.iUid );
		inBuf.WriteInt32L( aBlockFrom );
		inBuf.WriteInt32L( aBlockTo );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::BlockFullScreenUid - return: %d <<", err);
    return err;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::RegisterControlAction(	const TUid& aUid, TUint aActionID, 
														const TDesC& aActionString )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterControlAction >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlPolicyUpdate;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aUid.iUid );
		inBuf.WriteUint32L( aActionID );
		inBuf << aActionString;
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterControlAction - return: %d <<", err);
    return err;
	}
	
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::UnregisterControlAction(const TUid& aUid, TUint aActionID )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::UnregisterControlAction >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpControlPolicyRemove;
	// These writes cannot leave
	TRAP_IGNORE(  
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aUid.iUid );
		inBuf.WriteUint32L( aActionID );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::UnregisterControlAction - return: %d <<", err);
    return err;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::RegisterControlKml(const TUid& aUid, const TDesC& aResourceDir, 
													const TDesC& aFilename)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterControlKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
    RDesWriteStream inBuf( inPtr );
    err = FindEffectFile( aResourceDir, aFilename ); 

	TRAP( err, 
	    {
    	RThread me;
    	TInt op = MAlfGfxEffectPlugin::ETfxServerOpRegisterControlKml;
        inBuf.WriteInt32L( op );
    	inBuf.WriteInt32L( aUid.iUid );
    	TUint64 id = me.Id();
    	TUint8* ptr = reinterpret_cast<TUint8*>( &id );
    	inBuf.WriteL( ptr, 8 );
        inBuf << iParse.DriveAndPath();
//    	inBuf << aResourceDir;
    	inBuf << aFilename;
        inBuf.CommitL();		
	    });
	    
	inBuf.Release();
	inBuf.Close();	
	
	if ( err == KErrNone )
	    {
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
	    }

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterControlKml - return: %d <<", err );
    return err;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::RegisterControlKml(const TUid& aUid, const TDesC& aResourceDir, 
													const TDesC& aFilename, TInt aWantedTime,
													TInt aMinTime)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterControlKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
    RDesWriteStream inBuf( inPtr );

	RThread me;
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpRegisterControlKmlEx;
    err = FindEffectFile( aResourceDir, aFilename ); 
	TRAP( err, 
	    {
        inBuf.WriteInt32L( op );
    	inBuf.WriteInt32L( aUid.iUid );
    	TUint64 id = me.Id();
    	TUint8* ptr = reinterpret_cast<TUint8*>( &id );
    	inBuf.WriteL( ptr, 8 );
        inBuf << iParse.DriveAndPath();
//    	inBuf << aResourceDir;
    	inBuf << aFilename;
        inBuf.WriteInt32L( aWantedTime );
    	inBuf.WriteInt32L( aMinTime );
        inBuf.CommitL();		
	    });
	
	inBuf.Release();
	inBuf.Close();	
	
	if ( err == KErrNone )
	    {
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
	    }

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::RegisterControlKml - return: %d <<", err);
    return err;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::UnregisterAllKml()
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::UnregisterAllKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );

	// Out only parameter will be our thread id 
	RThread me;
   	TInt op = MAlfGfxEffectPlugin::ETfxServerOpRemoveAllKml;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
    	TUint64 id = me.Id();
	    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
	    inBuf.WriteL( ptr, 8 );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();
   
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
	 
    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::UnregisterAllKml - return: %d <<", err);
    return KErrNone;
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::UnregisterControlKml(const TUid& aUid)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::UnregisterControlKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpRemoveControlKml;
	// These writes cannot leave
	TRAP( err, 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aUid.iUid );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::UnregisterControlKml - return: %d <<", err);
    return err;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::SetWantedTime( TInt aTime )
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::SetWantedTime >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpSetWantedTime;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetWantedTime - return: %d <<", err);
    return err;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CAlfTransitionServerClient::SetMinTime(TInt aTime)
	{
	__ALFFXLOGSTRING1("CAlfTransitionServerClient::SetMinTime - aTime: %d >>", aTime );
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetMinTime - return: %d <<", err);
	    return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpSetMinTime;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteInt32L( aTime );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::SetMinTime - return: %d <<", err);
    return err;
	}	

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CAlfTransitionServerClient::RegisterFullscreenKml(	TUint aActionID, const TUid& aUid, 
														const TDesC& aResourceDir, const TDesC& aFilename,
														TInt aCachePriority, TInt aWantedTime,
														TInt aMinTime)
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterFullscreenKml >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	// Streaming test    
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	RThread me;
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpAddFullscreenKMLEx;
    err = FindEffectFile( aResourceDir, aFilename ); 
	TRAP( err, 
	    {
	    inBuf.WriteInt32L( op );
		inBuf.WriteUint32L( aActionID );
		inBuf.WriteInt32L( aUid.iUid );
    	TUint64 id = me.Id();
	    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
	    inBuf.WriteL( ptr, 8 );
        inBuf << iParse.DriveAndPath();
//		inBuf << aResourceDir;
		inBuf << aFilename;
		inBuf.WriteInt32L( aCachePriority );
		inBuf.WriteInt32L( aWantedTime );
		inBuf.WriteInt32L( aMinTime );
        inBuf.CommitL();		
		} );
		
	inBuf.Release();
	inBuf.Close();	
   
    if ( err == KErrNone )
        {
        err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
        }

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING("CAlfTransitionServerClient::RegisterFullscreenKml - return KErrNone <<");
    return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CAlfTransitionServerClient::VerifyConnection()
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::VerifyConnection >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerVerifyConnection;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::VerifyConnection - return: %d <<", err);
    return err;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::FreeRam()
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::FreeRam >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpFreeRam;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    
    // Here the server should actually return something

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::FreeRam - return: %d <<", err);
    return err;
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfTransitionServerClient::MemoryGood()
	{
	__ALFFXLOGSTRING("CAlfTransitionServerClient::MemoryGood >>");
	TInt err = KErrNone;
	
	if ( !iHasPlugin )
	    {
	    err = Connect();
	    }
	if ( !iHasPlugin || err != KErrNone )
	    {
	    // if we are started too early we just bail out without doing anything
        return err;
	    }
	    
	TPtr8 inPtr = iTransferBuffer->Des();
	inPtr.Zero();
	TPtr8 outPtr = iReturnBuffer->Des();
	outPtr.Zero();
	RDesWriteStream inBuf( inPtr );
	    
	TInt op = MAlfGfxEffectPlugin::ETfxServerOpMemoryGood;
	// These writes cannot leave
	TRAP_IGNORE( 
	    {
	    inBuf.WriteInt32L( op );
        inBuf.CommitL();		
		} );
	inBuf.Release();
	inBuf.Close();	
	
    err = iTfxServer.SendSynchronousData( iPluginImplementation, inPtr, outPtr );
    
    // Here the server should actually return something

    inPtr.Zero();
    outPtr.Zero();
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::MemoryGood - return: %d <<", err);
    return err;
	}

TInt CAlfTransitionServerClient::FindEffectFile( const TDesC& aResourceDir, const TDesC& aFilename )
    {
    __ALFFXLOGSTRING2("CAlfTransitionServerClient::FindEffectFile - Dir: %S File: %S>>", &aResourceDir, &aFilename);
    // TODO:
    // This function allows an alternative drive for finding effect files
    // By default the effect files are in ROM
    // To make testing easier, the it is allowed to have them on C-drive in the emulator
    // or memory card in the phone.
    // If the default memory card in the phone is not letter E, this needs to be adjusted.
    // When this testing functionality is not needed any more (should not be present in final release builds)
    // modify the function so that iParse always contains the original resource directory.
#ifdef __WINS__
    _LIT( KOptionalDrive, "C:" );
#else
    _LIT( KOptionalDrive, "F:" );
#endif
    
    TInt err = KErrNone;
    iParse.Set( KOptionalDrive, &aResourceDir, &aFilename );
    
    err = iFile.Open( iFs, iParse.FullName(), EFileRead );
    if ( err != KErrNone )
        {
        iParse.Set( aResourceDir, NULL, NULL );
        }
    else
        {
        // close the file if we managed to open it
        iFile.Close();
        iParse.Set( KOptionalDrive, &aResourceDir, NULL );  
        }
    
    // Now iParse contains the alternative path if the file is found in the alternative
    // location, else it contains the original path.
    __ALFFXLOGSTRING1("CAlfTransitionServerClient::FindEffectFile - return: %d <<", err);
    return err;
    }
	
// --- EOF ---

