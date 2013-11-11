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
* Description:   Alf system effects
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <bitdev.h>
#include <implementationproxy.h>
#include <akntranseffect.h>
#include <pslninternalcrkeys.h>
#include <s32mem.h> //RMemReadStream

#include "alftfxplugincommon.hrh"
#include "alfgfxeffects.h"
#include "alf/alfsrveffectenv.h"
#include "alfstreamerbridge.h"
#include "alfwindowmanager.h"
#include "alfserverdrawer.h"
#include "repositoryhandler.h"
#include "alftfxserverdef.h"
#include "alfpolicyhandler.h"
#include "alflogger.h"

#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiLayout.h"
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/HuiCommand.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiTransformation.h"

#include "wsserverdrawercontroller.h"

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h" 

// Constants
//const TInt KAlfDefaultFocusGainedEffectDuration = 500;
//const TInt KAlfDefaultFocusLostEffectDuration = 500;

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CAlfGfxEffects::CAlfGfxEffects
// C++ constuctor.
// -----------------------------------------------------------------------------
//
CAlfGfxEffects::CAlfGfxEffects( ) 
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::CAlfGfxEffects");
    }


// -----------------------------------------------------------------------------
// CAlfGfxEffects::EndExpired
// Timeout for grouped effects, that have not received EndGroup or have received 
// EndGroup with non-existing group id.
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::EndExpired( ) 
    {
	TInt count;
	while((count = iActiveSyncronizedGroups.Count() - 1 ) >= 0)
		{
		iEngine->EndSyncronizedGroup(iActiveSyncronizedGroups[count], ETrue /* force removal, running effect should be skipped */);
		iActiveSyncronizedGroups.Remove(count);
		}
    __ALFFXLOGSTRING("CAlfGfxEffects::EndExpired - release all syncronized groups");
    }

// -----------------------------------------------------------------------------
// CAlfGfxEffects::NewExtensionL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAlfWindowFxPlugin* CAlfGfxEffects::NewFxPluginL()
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::NewFxPluginL >>");
    CAlfGfxEffects* self = new(ELeave) CAlfGfxEffects();
//    do not call constructor here, base class constructor gets called later
    __ALFFXLOGSTRING("CAlfGfxEffects::NewFxPluginL <<");
    return ( self );
    }


// -----------------------------------------------------------------------------
// CAlfGfxEffects::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::ConstructL(const CAlfWindowManager& aMgr, TInt aAlfBufferFormat)
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::ConstructL >>");
    CAlfWindowFxPlugin::ConstructL(aMgr, aAlfBufferFormat);
    
    // create a policy handler for registering the effect files.
    iPolicyHandler = CPolicyHandler::NewL();
    
    // create controller and engine
    iEngine = CAlfServerDrawer::NewL( CAlfWindowFxPlugin::WindowManager(), *iPolicyHandler );
    iSysDisable = CRepositoryHandler::NewL(EPriorityHigh + KTfxServerRepositoryHandler, 
	                                     KCRUidThemes, 
	                                     KThemesTransitionEffects);
    
    iSyncronizedGroupDefitionEndChecker = CEndCheck::NewL(*this);
    
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Initiliaze global data in TLS and Open global module testing chunk and mutex
    User::LeaveIfError(Dll::SetTls(new(ELeave) CAlfModuleTestDataControl()));
    User::LeaveIfError(AMT_CONTROL()->OpenGlobalObjects());
#endif

    __ALFFXLOGSTRING("CAlfGfxEffects::ConstructL <<");
    }


// -----------------------------------------------------------------------------
// CAlfGfxEffects::~CAlfGfxEffects
// C++ destructor.
// -----------------------------------------------------------------------------
//
CAlfGfxEffects::~CAlfGfxEffects()
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::~CAlfGfxEffects >>");
    delete iPolicyHandler;
    delete iSysDisable;
    delete iEngine;
    __ALFFXLOGSTRING("CAlfGfxEffects::~CAlfGfxEffects <<");
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::HandleMessageL( const TDesC8& aInBuf, TPtr8& aOutBuf )
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::HandleMessageL >>");
    // synchronous messages are completed by server 
    
    RDesReadStream inStream;
    inStream.Open( aInBuf );
    
    TInt err = KErrNone;
    TInt op = inStream.ReadInt32L();
    TUint action = 0;
    TUid uid1( KNullUid );
    TSecureId sid1( KNullUid );
    TInt wgid1= 0;
    TUid uid2( KNullUid );
    TSecureId sid2( KNullUid );
    TInt wgid2= 0;
    HBufC* resourceDir = NULL;
    HBufC* fileName = NULL;
    HBufC* backgroundFileName = NULL;
    TInt wantedTime = 0;
    TInt minTime = 0;
    
    TThreadId clientThreadId( 0 );
    // These are needed to read the 64 bit thread id from the stream
   	TUint64 id = 0;
    TUint8* ptr = reinterpret_cast<TUint8*>( &id );
    
    CAlfTransitionServerClient::TListBoxType listBoxType = CAlfTransitionServerClient::EListTypeAny;

    TInt value = 0;
	TInt error = iSysDisable->GetValue(value);
    RMemWriteStream outStream; // for return values
    TBool fillOutStream = ETrue; // most functions put the error into outbuffer

    // The engine reservers the windows if needed, it first runs all the checks
    // to see if the effects are allowed (effects are not done for some combinations)
    
    __ALFFXLOGSTRING4("CAlfGfxEffects::HandleMessageL - Operation: %d, Action: %d, Cenrep error: %d, Effect value: %d", op, action, error, value & AknTransEffect::EFullScreenTransitionsOff);
    __ALFFXLOGSTRING2("CAlfGfxEffects::HandleMessageL - Dir: %S, File: %S", &resourceDir, &fileName );
    switch ( op )
        {
        // full screen transitions
        case MAlfGfxEffectPlugin::EBeginFullscreen:
    	    if((error != KErrNone) || (value & AknTransEffect::EFullScreenTransitionsOff)) 
	    	    {
		        // fullscreen effects are turned off
		        // Should we return the error in aOutBuf?
		        // We should not get end full screen if the effect has not been even started
		        // (could return KErrAbort in aOutBuf)
		        err = KErrAbort;
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
		        action = inStream.ReadUint32L();
                TRect effectRect( inStream.ReadInt32L(),
                    inStream.ReadInt32L(), inStream.ReadInt32L(), inStream.ReadInt32L() );
                TInt type = inStream.ReadInt32L();
                uid1 = TUid::Uid( inStream.ReadInt32L() );
                uid2 = TUid::Uid( inStream.ReadInt32L() );
                TInt data = inStream.ReadInt32L();
                
                if ( type == AknTransEffect::EParameterType )
                    {
                    sid1 = TSecureId( inStream.ReadInt32L() ); // secureid is the only thing interesting to us
                    }
                TTime time;
                time.UniversalTime();
                AMT_ADD_TIME(sid1.iId, time.Int64(), EFalse);
#endif
		        break;
		        }
		    else
		        {
	            action = inStream.ReadUint32L();
#ifdef _ALF_FXLOGGING    
    PrintRequestInfo( op, action);
#endif    
	            TRect effectRect( inStream.ReadInt32L(),
                    inStream.ReadInt32L(), inStream.ReadInt32L(), inStream.ReadInt32L() );
                TInt type = inStream.ReadInt32L();
                // Depending on type, the rest of the parameters have different meaning
                uid1 = TUid::Uid( inStream.ReadInt32L() );
                uid2 = TUid::Uid( inStream.ReadInt32L() );
                TInt data = inStream.ReadInt32L();
                
                if ( type == AknTransEffect::EParameterType )
                    {
                    sid1 = TSecureId( inStream.ReadInt32L() );
                    wgid1 = inStream.ReadInt32L();
                    sid2 = TSecureId( inStream.ReadInt32L() );
                    wgid2 = inStream.ReadInt32L();
                    }
                
                iEngine->BeginFullscreen( action, effectRect, type,
                    uid1, uid2, data, sid1, wgid1, sid2, wgid2 );
		        }
            break;
        case MAlfGfxEffectPlugin::EEndFullscreen:
#ifdef _ALF_FXLOGGING    
    PrintRequestInfo( op, action);
#endif    
            iEngine->EndFullscreen(EFalse); // not timeout, but official endfullscreen
            break;
        case MAlfGfxEffectPlugin::EAbortFullscreen:
#ifdef _ALF_FXLOGGING
    PrintRequestInfo( op, action);
#endif    

            iEngine->AbortFullscreen();
            break;
        // component transitions
        case MAlfGfxEffectPlugin::EBeginComponentTransition: 
            if((error != KErrNone) || (value & AknTransEffect::EComponentTransitionsOff))
                {
                err = KErrAbort;
                break;
                }
            else
                {
                // We may not need all of this, but it is here now just to make sure
                // we have enough information.
                {
                action = inStream.ReadUint32L();
#ifdef _ALF_FXLOGGING    
    PrintRequestInfo( op, action);
#endif    
                
                uid1 = TUid::Uid( inStream.ReadInt32L() );
                // hack to keep disapear viewswitch transitions popping on top of everything
				// TODO: Remove when HS complies
                if ((uid1.iUid == 0x102750F1 || uid1.iUid == 0x102750F2 ) && action == 4)
                    {
                    action = 5;
                    }
                
                TInt windowGroup = inStream.ReadInt32L();
                TInt windowHandle = inStream.ReadInt32L();
                TBool nLayered = inStream.ReadInt32L();
                uid2 = TUid::Uid( inStream.ReadInt32L() ); // layer type uid
                TInt flags = inStream.ReadInt32L(); // hints
                iEngine->BeginControlTransition( action, uid1, windowGroup, windowHandle );
                }
            break;
            }
        // These are the effect registrations    
        case MAlfGfxEffectPlugin::ETfxServerOpAddFullscreenKML:
        case MAlfGfxEffectPlugin::ETfxServerOpAddFullscreenKMLEx:
            {
            action = inStream.ReadUint32L();
#ifdef _ALF_FXLOGGING    
    PrintRequestInfo( op, action);
#endif    

            uid1 = TUid::Uid( inStream.ReadInt32L() );
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
            resourceDir = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( resourceDir );
            fileName = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( fileName );
            // If the operation is ETfxServerOpAddFullscreenKMLEx, there are some more parameters
            TInt cachePriority = -1;
            wantedTime = -1;
            minTime = -1;
            if ( op == ETfxServerOpAddFullscreenKMLEx )
                {
                cachePriority = inStream.ReadInt32L();
                wantedTime = inStream.ReadInt32L();
                minTime = inStream.ReadInt32L();
                }
            err = VerifyResourceLocation( resourceDir->Des() );
            /*
            // Needed to be able to see the filename in carbide
            TPtrC filePtr = fileName->Des();
            RDebug::Print(_L("register file %S"), &filePtr );
            */
            if ( err == KErrNone )
                {
                // register the effect definition file
                if ( uid1.iUid != KAlfAppServerInterfaceUid3 )
                    {
                    TRAP( err, iPolicyHandler->RegisterFullscreenKmlL(
                        action, uid1, clientThreadId, *resourceDir, *fileName,
                        cachePriority, wantedTime, minTime) );
	                iEngine->RegisterAlfFullScreenEffect( action, resourceDir->Des(), fileName->Des() );
                    }
                else
	                {  
	                // effects for KAlfAppServerInterfaceUid3 go directly to Alf Bridge
	                iEngine->RegisterAlfFullScreenEffect( action, resourceDir->Des(), fileName->Des() );
	                }           
                }
                
            CleanupStack::PopAndDestroy( fileName );
            CleanupStack::PopAndDestroy( resourceDir );
            break;
            }
        case MAlfGfxEffectPlugin::ETfxServerOpRemoveFullscreenKML:
            action = inStream.ReadUint32L();
            uid1 = TUid::Uid( inStream.ReadInt32L() );
            // We don't keep the registration status the files that go directly to alf 
            if ( uid1.iUid != KAlfAppServerInterfaceUid3 )
                {
                err = iPolicyHandler->UnregisterFullscreenKml( action, uid1 );
                }
            else
                {               
                // effects for KAlfAppServerInterfaceUid3 go directly to Alf Bridge
                }        
            iEngine->UnregisterAlfFullScreenEffect( action );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpListBoxRegisterKML:
            uid1 = TUid::Uid( inStream.ReadInt32L() );
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
   	        listBoxType = (CAlfTransitionServerClient::TListBoxType) inStream.ReadInt32L();
            resourceDir = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( resourceDir );
            backgroundFileName = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( backgroundFileName );
            fileName = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( fileName );
            err = VerifyResourceLocation( resourceDir->Des() );
            if ( err == KErrNone )
                {
                iPolicyHandler->RegisterListBoxKmlL( uid1, clientThreadId, listBoxType,
                    *resourceDir, *backgroundFileName, *fileName );
                }
            CleanupStack::PopAndDestroy( fileName );
            CleanupStack::PopAndDestroy( backgroundFileName );
            CleanupStack::PopAndDestroy( resourceDir );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpListBoxUnregisterKML:
            uid1 = TUid::Uid( inStream.ReadInt32L() );
   	        listBoxType = (CAlfTransitionServerClient::TListBoxType) inStream.ReadInt32L();
   	        err = iPolicyHandler->UnregisterListBoxKml( uid1, listBoxType );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpListBoxSetFrameTime:
            iListBoxFrameTime = inStream.ReadInt32L();
            // this must be sent forward somewhere (list controller?)
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpListBoxSetMinFrameTime:
            iListBoxMinFrameTime = inStream.ReadInt32L();
            // this must be sent forward somewhere (list controller?)
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpControlSetFrameTime:
            iControlFrameTime = inStream.ReadInt32L();
            // this must be sent forward somewhere (list controller?)
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpControlSetMinFrameTime:
            iControlMinFrameTime = inStream.ReadInt32L();
            // this must be sent forward somewhere (list controller?)
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpBlockFullscreenUid:
            {
            uid1 = TUid::Uid( inStream.ReadInt32L() );
            TBool blockFrom = inStream.ReadInt32L();
            TBool blockTo = inStream.ReadInt32L();
            err = iPolicyHandler->BlockFullScreenUid( uid1, blockFrom, blockTo );
            break;
            }
        case MAlfGfxEffectPlugin::ETfxServerOpControlPolicyUpdate:
            {
            uid1 = TUid::Uid( inStream.ReadInt32L() );
            TUint actionId = inStream.ReadUint32L();
            HBufC* actionString = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( actionString );
            iPolicyHandler->RegisterControlAction( uid1, actionId, *actionString );
            CleanupStack::PopAndDestroy( actionString );
            break;
            }
        case MAlfGfxEffectPlugin::ETfxServerOpControlPolicyRemove:
            {
            uid1 = TUid::Uid( inStream.ReadInt32L() );
            TUint actionId = inStream.ReadUint32L();
            iPolicyHandler->UnregisterControlAction( uid1, actionId );
            break;
            }
        case MAlfGfxEffectPlugin::ETfxServerOpRegisterControlKml:
        case MAlfGfxEffectPlugin::ETfxServerOpRegisterControlKmlEx:
            uid1 = TUid::Uid( inStream.ReadInt32L() );
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
            resourceDir = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( resourceDir );
            fileName = HBufC::NewL( inStream, 256 );
            CleanupStack::PushL( fileName );
            // If the operation is ETfxServerOpRegisterControlKmlEx, there are some more parameters
            wantedTime = -1;
            minTime = -1;
            if ( op == ETfxServerOpRegisterControlKmlEx )
                {
                wantedTime = inStream.ReadInt32L();
                minTime = inStream.ReadInt32L();
                }
            err = VerifyResourceLocation( resourceDir->Des() );
            if ( err == KErrNone )
                {
                // register the effect definition file
	        	TRAP( err, iPolicyHandler->RegisterControlKmlL(
	        	    uid1, clientThreadId, *resourceDir, *fileName,
	        	    wantedTime, minTime) );
		iEngine->RegisterAlfFullScreenEffect( action, resourceDir->Des(), fileName->Des() );
                }
            CleanupStack::PopAndDestroy( fileName );
            CleanupStack::PopAndDestroy( resourceDir );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpRemoveAllKml:
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
            if ( uid1.iUid != KAlfAppServerInterfaceUid3 )
                {
                err = iPolicyHandler->UnregisterAllKml( clientThreadId );
                }
            else
                {               
                // effects for KAlfAppServerInterfaceUid3 go directly to Alf Bridge
                }   
            iEngine->UnregisterAllAlfFullScreenEffect( );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpRemoveControlKml:
            uid1 = TUid::Uid( inStream.ReadInt32L() );
            err = iPolicyHandler->RemoveControlKml( uid1 );
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpSetWantedTime:
            iTransitionTime = inStream.ReadInt32L();
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpSetMinTime:
            iTransitionMinTime = inStream.ReadInt32L();
            break;
        case MAlfGfxEffectPlugin::ETfxServerVerifyConnection:
            // If we get this far, it should prove that we have a good connection
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpFreeRam:
            // here we should return something
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpMemoryGood:
            // here we should return something
            break;
        case MAlfGfxEffectPlugin::ETfxServerOpControlPolicyGet:
            // get the policy structures from the policy handler
            // client id (we don't get the whole message because this is asynchronous
            
            // We fill the outbuffer with policies, do not add an error afterwards
            {
                
            fillOutStream = EFalse;
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
            // number of policy structures the return buffer can hold
            TInt count = inStream.ReadInt32L();
            iPolicyHandler->GetPolicyL( clientThreadId, count, aOutBuf );
            break;
            }
        case MAlfGfxEffectPlugin::ETfxServerOpControlCancelPolicyUpdate:
            // cancel any pending policy requests
	        inStream.ReadL( ptr, 8 );
	        clientThreadId = id;
            iPolicyHandler->RemoveClient( clientThreadId );
            break;
            
        case MAlfGfxEffectPlugin::ETfxServerOpBeginGroup:
        	{
        	TInt groupId= inStream.ReadUint32L();
        	iEngine->BeginSyncronizedGroup(groupId);
        	iActiveSyncronizedGroups.Append(groupId);
        	iSyncronizedGroupDefitionEndChecker->Start(4);
        	break;
        	}
        
        case MAlfGfxEffectPlugin::ETfxServerOpEndGroup:
			{
			// if all groups have been released, then we can kill the timer.
			if (iActiveSyncronizedGroups.Count() - 1 == 0)
				{
				iSyncronizedGroupDefitionEndChecker->Cancel();
				}
        	TInt groupId= inStream.ReadUint32L();
        	TInt index = iActiveSyncronizedGroups.Find(groupId);
        	if (index!=KErrNotFound)
        		{
				iEngine->EndSyncronizedGroup(groupId);
				iActiveSyncronizedGroups.Remove(index);
        		}
        	if (iActiveSyncronizedGroups.Count())
        		{
				// if there is still groups out there not closed, then this will create timeout for them
				iSyncronizedGroupDefitionEndChecker->Start(4);
        		}
			break;
			}
		
        default:
            break;
        }
        
    inStream.Release();
    inStream.Close();    
        
    if ( aOutBuf.MaxSize() >= sizeof(TInt32) && fillOutStream )
        {
        // The write will not leave as we have checked the length of the buffer beforeahdn
        unsigned char buffer[sizeof(TInt32)];
        outStream.Open( buffer, sizeof(TInt32) );
        outStream.WriteInt32L( err );
        outStream.CommitL();
        outStream.Release();
        outStream.Close();
        aOutBuf.Copy( &buffer[0], sizeof(TInt32) );
        }
    __ALFFXLOGSTRING("CAlfGfxEffects::HandleMessageL <<");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::HandleMessageL( const TDesC8& aInBuf, const RMessage2& aClientMessage )
    {
    // make sure that there are no pending asynch calls left here
    __ALFFXLOGSTRING1("CAlfGfxEffects::HandleMessageL - : %S", &aInBuf );
    RDesReadStream inStream;
    inStream.Open( aInBuf );
    
    TInt op = inStream.ReadInt32L();
    inStream.Release();
    inStream.Close();    
    switch ( op )
        {
        case MAlfGfxEffectPlugin::ETfxServerOpControlPolicyRequest:
           	// This message is completed when a new policy exists.
           	// All clients get the same policies
       	  	User::LeaveIfError( iPolicyHandler->RequestPolicy( aClientMessage ) );
            break;
        default:
            break;
        }
    __ALFFXLOGSTRING("CAlfGfxEffects::HandleMessageL <<");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::CancelMessage(TInt /*aMessageId*/)
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::CancelMessage - not implemented");
    // default implementation does not have pending messages
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::PrepareFrameL(TUint /*aEstimatedFrameInterval*/)
    {
    // __ALFFXLOGSTRING("CAlfGfxEffects::PrepareFrameL - not implemented");
    // not implemented
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAlfGfxEffects::WindowUpdated(TInt /*aWindowId*/, TInt /*aEventType*/)
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::WindowUpdated - not implemented");
    // not implemented
    }

// -----------------------------------------------------------------------------
// CAlfGfxEffects::GetSupportedEffects
// -----------------------------------------------------------------------------
//
TInt CAlfGfxEffects::GetSupportedEffects( RArray<TInt>& /*aSupportedEffectIds*/ )
	{ 
	/*
	// Add effect ids which are supported by this plugin
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextFocusGained ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextFocusLost ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextEmbeddedStart ); 
	aSupportedEffectIds.Append( MAlfTransEffectPlugin::EContextEmbeddedExit ); 
	*/
	__ALFFXLOGSTRING("CAlfGfxEffects::GetSupportedEffects");
	return KErrNone; 
	}
	

// -----------------------------------------------------------------------------
// CAlfGfxEffects::StartPhase
// -----------------------------------------------------------------------------
//
TInt CAlfGfxEffects::StartPhase( TInt /*aPhaseId*/, TInt /*aEffectId*/, 
    CAlfSrvEffectEnv& /*aEffectEnv*/, const TDesC8* /*aParams*/ ) 
	{ 
	/*
	TRAP_IGNORE( DoStartPhaseL( aPhaseId, aEffectId, aEffectEnv, aParams ));
	*/
	__ALFFXLOGSTRING("CAlfGfxEffects::StartPhase");
	return KErrNone;
	}
	
	
// -----------------------------------------------------------------------------
// CAlfGfxEffects::DoStartPhaseL
// -----------------------------------------------------------------------------
//
TInt CAlfGfxEffects::DoStartPhaseL( TInt /*aPhaseId*/, TInt /*aEffectId*/, 
    CAlfSrvEffectEnv& /*aEffectEnv*/, const TDesC8* /*aParams*/ ) 
	{ 
	__ALFFXLOGSTRING("CAlfGfxEffects::DoStartPhaseL >>");
/*	
	// Start effect here!
    switch (aEffectId)
        {
        case MAlfTransEffectPlugin::EContextFocusGained:
            {
            ShowVisibleGroupsAndMakeNonTransparentL( aEffectEnv );                           				
            break;
            }
            
        case MAlfTransEffectPlugin::EContextFocusLost:
            {
            HideGroupsAndMakeTransparentL( aEffectEnv );          
            break;
            }
            
        case MAlfTransEffectPlugin::EContextEmbeddedStart:
            {
            // do a normal show
            ShowVisibleGroupsAndMakeNonTransparentL( aEffectEnv );
            
            // in addition, do a expand effect for the visible groups
            TInt controlGroupCount = aEffectEnv.VisibleControlGroupCountL();
            for ( TInt g = 0 ; g < controlGroupCount ; g++ )
                {
                CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
		        if (group)
                    {
                    // Perhaps we should show controlgroup in other displays too...?
                    CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
                    if (defaultDisplay)
                        {
                        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );          
                       
                        // Cancel the previous 'disable transformation' commands.
                        aEffectEnv.SharedEnv()->CancelCommands( hostContainer, EHuiOpDisableTransformation );
                       
                        hostContainer->EnableTransformationL(); 
                        CHuiTransformation& xf = hostContainer->Transformation(); 
                        
                        THuiTimedValue scale(0);
                        scale.Set(1, KAlfDefaultFocusGainedEffectDuration);

                        // Clear previously created transformation steps.
                        xf.LoadIdentity();

                        xf.Scale(scale,scale);
                        
                        // Disable transformations after effect
                        THuiVisualCommand disableTransformationCommand( *hostContainer, EHuiOpDisableTransformation );
                        aEffectEnv.SharedEnv()->Send( disableTransformationCommand, KAlfDefaultFocusGainedEffectDuration );
                        }
                    }
                }            
            break;
            }
        case MAlfTransEffectPlugin::EContextEmbeddedExit:
            {
            // do normal hide
            HideGroupsAndMakeTransparentL( aEffectEnv );
            
            // in addition, do a shrink effect for the visible groups
            TInt controlGroupCount = aEffectEnv.VisibleControlGroupCountL();
            for ( TInt g = 0 ; g < controlGroupCount ; g++ )
                {
                CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
		        if (group)
                    {
                    // Perhaps we should show controlgroup in other displays too...?
                    CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
                    if (defaultDisplay)
                        {
                        // Do a little pop out effect
                        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
                               
                        // Cancel the previous 'disable transformation' commands.
                        aEffectEnv.SharedEnv()->CancelCommands( hostContainer, EHuiOpDisableTransformation );                

                        hostContainer->EnableTransformationL(); 
                        CHuiTransformation& xf = hostContainer->Transformation(); 
                        
                        THuiTimedValue scale(1);
                        scale.Set(0, KAlfDefaultFocusLostEffectDuration);

                        // Clear previously created transformation steps.
                        xf.LoadIdentity();

                        xf.Scale(scale,scale);
                        
                        // Disable transformations after effect
                        THuiVisualCommand disableTransformationCommand( *hostContainer, EHuiOpDisableTransformation );
                        aEffectEnv.SharedEnv()->Send( disableTransformationCommand, KAlfDefaultFocusLostEffectDuration );
                        }
                    }
                }            
            break;
            }
       default:
            {
            
            break;
            }        
        }
*/	
	__ALFFXLOGSTRING("CAlfGfxEffects::DoStartPhaseL <<");
	return KErrNone; 
	};
    
void CAlfGfxEffects::ShowVisibleGroupsAndMakeNonTransparentL(CAlfSrvEffectEnv& /*aEffectEnv*/)
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::ShowVisibleGroupsAndMakeNonTransparentL >>");
    // Set opacity of every control group to 1, even if it is not shown because
    // application may want to show it later and it cannot control the
    // opacity.
/*    
    TInt controlGroupCount = aEffectEnv.ControlGroupCountL();
    for ( TInt g = 0 ; g < controlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.ControlGroupL(g);
        if (group)
            {
            // Perhaps we should show controlgroup in other displays too...?
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Set the root visual opacity to 1 which affects all the visuals in the group
                CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );                                                
                hostContainer->iOpacity.Set(1.f, KAlfDefaultFocusGainedEffectDuration);                                                
                
                // Disable also previous transformations.
                hostContainer->EnableTransformationL(EFalse); 
                }
            }
        }   
        
    // Check how many control groups are shown (set visible by the application)
    
    // todo: the visible control group should be shown in the same order as they were
    // set into the roster originally.
    TInt visibleControlGroupCount = aEffectEnv.VisibleControlGroupCountL();
    for ( TInt g = 0 ; g < visibleControlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.VisibleControlGroupL(g);
        if (group)
            {
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Cancel the previous hide commands.
                aEffectEnv.SharedEnv()->CancelCommands( group, EHuiOpHide );
                
                // Show control group ! 
                defaultDisplay->Roster().ShowL(*group);                    
                }
            }
        }  
*/        
   __ALFFXLOGSTRING("CAlfGfxEffects::ShowVisibleGroupsAndMakeNonTransparentL <<");
    }
    
void CAlfGfxEffects::HideGroupsAndMakeTransparentL(CAlfSrvEffectEnv& /*aEffectEnv*/)
    {
    __ALFFXLOGSTRING("CAlfGfxEffects::HideGroupsAndMakeTransparentL >>");
/*    
    TInt controlGroupCount = aEffectEnv.ControlGroupCountL();
    for ( TInt g = 0 ; g < controlGroupCount ; g++ )
        {
        CHuiControlGroup* group = aEffectEnv.ControlGroupL(g);
        if (group)
            {
            // Perhaps we should show controlgroup in other displays too...?
            CHuiDisplay* defaultDisplay = aEffectEnv.DisplayL(0);                
            if (defaultDisplay)
                {
                // Set the root visual opacity to 0 which affects all the visuals in the group
                CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
                hostContainer->iOpacity.Set(0.f, KAlfDefaultFocusGainedEffectDuration);   
                                                             
				if (defaultDisplay->DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
					{
					// Hide the group so that it does not use resources when we dont have focus
                    THuiGroupCommand command( *group, EHuiOpHide, defaultDisplay );
                    aEffectEnv.SharedEnv()->Send(command, KAlfDefaultFocusLostEffectDuration);
					}
                }
            }
        } 
*/        
    __ALFFXLOGSTRING("CAlfGfxEffects::HideGroupsAndMakeTransparentL <<");
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfGfxEffects::VerifyResourceLocation(const TDesC& aResourceDir)
	{
	__ALFFXLOGSTRING("CAlfGfxEffects::VerifyResourceLocation >>");
	// For starters anything goes (we want to be able to test effect files on C drive if needed)
	if (aResourceDir.Length() > 1)
	    {
	    __ALFFXLOGSTRING("CAlfGfxEffects::VerifyResourceLocation - return KErrNone");
	    return KErrNone;
	    }
/*
	if(aResourceDir.Length() > 1 &&
	   (aResourceDir[0] == 'Z' || aResourceDir[0] == 'z') )
		{
		return KErrNone;
		}
*/		
	__ALFFXLOGSTRING("CAlfGfxEffects::VerifyResourceLocation - return KErrNotSupported <<");
	return KErrNotSupported;
	}

#ifdef _ALF_FXLOGGING
void CAlfGfxEffects::PrintRequestInfo(TInt aOperation, TInt aAction )
    {
    HBufC16* buffer = HBufC16::NewL(256);
    TPtr debugText = buffer->Des();
    debugText.Format( _L("Effect request: Operation "));
    
    switch(aOperation)
        {
        case MAlfGfxEffectPlugin::EBeginFullscreen:
            {
            debugText.AppendFormat(_L("EBeginFullscreen"));
            break;
            }
        case MAlfGfxEffectPlugin::EEndFullscreen:
            {
            debugText.AppendFormat(_L("EEndFullscreen"));
            break;
            }
        case MAlfGfxEffectPlugin::EAbortFullscreen:
            {
            debugText.AppendFormat(_L("EAbortFullscreen"));
            break;
            }
        case MAlfGfxEffectPlugin::EBeginComponentTransition:
            {
            debugText.AppendFormat(_L("EBeginComponentTransition"));
            break;
            }
        case MAlfGfxEffectPlugin::EAbortComponentTransition:
            {
            debugText.AppendFormat(_L("EAbortComponentTransition"));
            break;
            }
        case MAlfGfxEffectPlugin::ERegisterFullScreenEffectFile:
            {
            debugText.AppendFormat(_L("ERegisterFullScreenEffectFile"));
            break;
            }
        case MAlfGfxEffectPlugin::EUnregisterFullScreenEffectFile:
            {
            debugText.AppendFormat(_L("EUnregisterFullScreenEffectFile"));
            break;
            }
        case MAlfGfxEffectPlugin::EBlockFullScreenUid:
            {
            debugText.AppendFormat(_L("EBlockFullScreenUid"));
            break;
            }
        case MAlfGfxEffectPlugin::EUnregisterAllEffects:
            {
            debugText.AppendFormat(_L("EUnregisterAllEffects"));
            break;
            }
        }

    debugText.Format( _L(", Action: "));
    
    switch(aAction)
        {
        case AknTransEffect::ENone:
            {
            debugText.AppendFormat(_L("ENone"));
            break;
            }
        case AknTransEffect::EApplicationActivate:
            {
            debugText.AppendFormat(_L("EApplicationActivate"));
            break;
            }
        case AknTransEffect::EApplicationStart:
            {
            debugText.AppendFormat(_L("EApplicationStart"));
            break;
            }
        case AknTransEffect::EEmbeddedApplicationStart:
            {
            debugText.AppendFormat(_L("EEmbeddedApplicationStart"));
            break;
            }
        case AknTransEffect::EApplicationExit:
            {
            debugText.AppendFormat(_L("EApplicationExit"));
            break;
            }
        case AknTransEffect::EApplicationStartRect:
            {
            debugText.AppendFormat(_L("EApplicationStartRect"));
            break;
            }
        case AknTransEffect::EApplicationStartSwitch:
            {
            debugText.AppendFormat(_L("EApplicationStartSwitch"));
            break;
            }
        case AknTransEffect::EApplicationStartSwitchRect:
            {
            debugText.AppendFormat(_L("EApplicationStartSwitch"));
            break;
            }
        case AknTransEffect::EEmbeddedApplicationExit:
            {
            debugText.AppendFormat(_L("EEmbeddedApplicationExit"));
            break;
            }
        case AknTransEffect::EAppStartComplete:
#ifdef RD_UI_TRANSITION_EFFECTS_LAYOUT_SWITCH
            {
            debugText.AppendFormat(_L("EAppStartComplete"));
            break;
            }
        case AknTransEffect::ELayoutSwitch: // default : swap to portrait.
            {
            debugText.AppendFormat(_L("ELayoutSwitch"));
            break;
            }
        case AknTransEffect::ELayoutSwitchExit:
            {
            debugText.AppendFormat(_L("ELayoutSwitchExit"));
            break;
            }
        case AknTransEffect::ELayoutSwitchStart:        
#endif //RD_UI_TRANSITION_EFFECTS_LAYOUT_SWITCH
        case AknTransEffect::EAppSpesificEvent:
            {
            debugText.AppendFormat(_L("ELayoutSwitchStart"));
            break;
            }
        case AknTransEffect::EAppStartupBackground:
            {
            debugText.AppendFormat(_L("EAppStartupBackground"));
            break;
            }
        default:
            {
            debugText.AppendFormat(_L("UNKNOWN"));
            break;
            }
        }
    
    __ALFFXLOGSTRING1("CAlfGfxEffects::PrintInfo %S", &debugText);
    delete buffer; 
    }
#endif

void CAlfGfxEffects::ClientAboutToExit(TThreadId aClientId)
    {
    if (iPolicyHandler)
        {
        iPolicyHandler->RemoveClient( aClientId );
        }   
    }



//  End of File  
