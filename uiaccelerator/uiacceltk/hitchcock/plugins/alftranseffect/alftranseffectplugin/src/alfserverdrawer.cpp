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
* Description:   window server plugin, used by transition server to control
*                framebuffers.
*
*/



#include <bitstd.h>
#include <s32mem.h> //RMemReadStream
#include <e32cmn.h>
#include <apgwgnam.h>
#include <bautils.h> 

#include <akntranseffect.h> 
#include <akntransitionutils.h>

#include "alfwindowmanager.h"
#include "alfserverdrawer.h"
#include "wsserverdrawercontroller.h"
#include "alfgfxeffects.h"
#include "alfwindow.h"
#include "alfstreamerbridge.h"
#include "alfpolicyhandler.h"
#include <alflogger.h>
#include <uiacceltk/HuiUtil.h>

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h" 

// This sets the maximum time for an effect. Even if the fxml file has specified a longer duration, 
// this will stop the effect.
const TInt KAlfShortEffectTimeout     = 4000000;

// Note, The timeouts are DIFFERENT for EMULATOR AND ARMV5
#ifdef __WINS__
const TInt KAlfAppStartEffectTimeout  = 800000;
const TInt KAlfLongEffectTimeout      = 10000000;
#else
const TInt KAlfAppStartEffectTimeout  = 300000;
const TInt KAlfLongEffectTimeout      = 5000000;
#endif
const TInt KAlfActiveControlFxGranularity = 4;
const TInt KAlfLongApplicationStartContext = 20;

//const TInt KAlfLongEffectTimeout  = 500000;
// Timer to send finish full screen effect
// ---------------------------------------------------------
// CAlfFinishTimer
// ---------------------------------------------------------
//
NONSHARABLE_CLASS( CAlfFinishTimer ):public CTimer
    {
    public:  // Constructors and destructor
        static CAlfFinishTimer* NewL();
        virtual ~CAlfFinishTimer();

    public: // New functions
            
        void Start( TTimeIntervalMicroSeconds32 aPeriod, TCallBack aTimeoutCallback );
        
    protected:  // Functions from base classes
        
        void DoCancel();

    public:
        
        TBool iTimeoutTriggered;
        TBool iIsStartEffect;
        
    private:

        CAlfFinishTimer();
        void ConstructL();
        void RunL();
      
    private:    // Data
                
    TCallBack iCallback;
 
    };


// ---------------------------------------------------------
// CAlfFinishTimer
// ---------------------------------------------------------
//
CAlfFinishTimer::CAlfFinishTimer()
    :CTimer ( EPriorityStandard )
    {   
    }

void CAlfFinishTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

CAlfFinishTimer* CAlfFinishTimer::NewL()
    {
    CAlfFinishTimer* self = new ( ELeave ) CAlfFinishTimer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfFinishTimer::~CAlfFinishTimer()
    {
    Cancel();        
    }

void CAlfFinishTimer::Start( TTimeIntervalMicroSeconds32 aPeriod, TCallBack aTimeoutCallback )
    {
    iCallback = aTimeoutCallback; 
    iTimeoutTriggered = EFalse;
    After( aPeriod );
    }

void CAlfFinishTimer::RunL()
    {
    //
    // timer completes and control is returned to caller
    //
    if ( iStatus.Int() != KErrCancel )
        {
        iTimeoutTriggered = ETrue;
        iCallback.CallBack();
        }
    }

void CAlfFinishTimer::DoCancel()
    {
    CTimer::DoCancel();
    }


NONSHARABLE_CLASS( CPSObserver ): public CActive
    {
    public:
        static CPSObserver* NewL( const TUid& aUid, TInt aKey );
        void StartObserving( TCallBack aCallback, TInt aHandle );
        ~CPSObserver();
    private:
        CPSObserver();
        void ConstructL( const TUid& aUid, TInt aKey );
        void DoCancel();
        void RunL();
    private:
        RProperty iProperty;
        TInt iKey;
        TCallBack iCallback;
        TInt iHandle;
    };
    
CPSObserver::CPSObserver():
    CActive( CActive::EPriorityHigh )
    {
    }
    
CPSObserver::~CPSObserver()
    {
    Cancel();
    iProperty.Close();
    }
    
CPSObserver* CPSObserver::NewL( const TUid& aUid, TInt aKey )
    {
    CPSObserver* self = new ( ELeave ) CPSObserver();
    CleanupStack::PushL( self );
    self->ConstructL( aUid, aKey );
    CleanupStack::Pop( self );
    return self;
    }

void CPSObserver::ConstructL( const TUid& aUid, TInt aKey ) 
    {
    iKey = aKey;
    User::LeaveIfError( iProperty.Attach( aUid, aKey ) );
    CActiveScheduler::Add( this );
    }

void CPSObserver::StartObserving( TCallBack aCallback, TInt aHandle )
    {
    Cancel();
    iHandle = aHandle;
    iCallback = aCallback;
    iProperty.Subscribe( iStatus );
    SetActive();
    }
    
void CPSObserver::RunL()
    {
    // prepare for stray signals
    iProperty.Subscribe( iStatus );
    SetActive();
    TInt value = -1;
    TInt err = iProperty.Get( value );
    if ( iStatus.Int() != KErrCancel && value == iHandle )
        {
        iCallback.CallBack();
        // Cancel wait for property change until a new effect is started
        Cancel();            
        }
    // If the handle does not match, the ending effect was some earlier one
    // We keep waiting for signal about ending of the correct effect.    
    }
    
void CPSObserver::DoCancel()
    {
    iProperty.Cancel();
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfServerDrawer* CAlfServerDrawer::NewL( CAlfWindowManager* aWindowManager,
    CPolicyHandler& aPolicyHandler )
    {
    CAlfServerDrawer* self = new(ELeave) CAlfServerDrawer( aWindowManager, aPolicyHandler );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfServerDrawer::ConstructL()
    {
    iControlController = CWsServerDrawerController::NewL(this);
    iFullScreenController = CWsServerDrawerController::NewL(this);

    iFullScreenTimeout = CAlfFinishTimer::NewL();
    iControlTimeout = CAlfFinishTimer::NewL();
    iFinishFullScreen = CAlfFinishTimer::NewL();
    
	// note, that the PS key is actually never updated and these do not do anything
    iControlTransitionEndObserver = CPSObserver::NewL( KPSAlfDomain, KAlfTransitionStatus );
    iFullScreenTransitionEndObserver = CPSObserver::NewL( KPSAlfDomain, KAlfTransitionStatus );
    iProperty.Attach( KPSAlfDomain, KAlfTransitionStatus );
    iProperty.Set( KPSAlfDomain, KAlfTransitionStatus, 0 );
    iFs.Connect();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CAlfServerDrawer::CAlfServerDrawer( CAlfWindowManager* aWindowManager,
    CPolicyHandler& aPolicyHandler  ):
    iWindowManager( aWindowManager ),
    iPolicyHandler( aPolicyHandler ),
    iActiveControlFx( KAlfActiveControlFxGranularity )
    {
    iScrModeChangedState = EScreenModeChangedIdle;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
CAlfServerDrawer::~CAlfServerDrawer()
    {
    delete iControlTransitionEndObserver;
    delete iFullScreenTransitionEndObserver;
    iProperty.Close();
    iFs.Close();
    delete iControlController;
    delete iFullScreenController;
    delete iFinishFullScreen;
    delete iFullScreenTimeout;
    iActiveControlFx.Close();
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    delete AMT_CONTROL();
    Dll::FreeTls();
#endif
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfServerDrawer::BeginControlTransition( TUint aAction, TUid aControlUid,
    TInt aWindowGroup, TInt aWindowHandle)
    {
  	iControlRect = TRect(); // clear the rectangle

    if ( iInScreenModeChange )
        {
        return KErrAbort;
        }
    else
        {
        iControlAction = aAction;
        iControlUid = aControlUid;
        iControlWindowGroup = aWindowGroup;
        iControlHandle = aWindowHandle;
        return iControlController->BeginControlTransition();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CAlfServerDrawer::EndControlTransition(TInt aHandle)
    {
    iControlController->EndControlTransition(aHandle);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
void CAlfServerDrawer::PrepareFullscreen()
    {
    __ALFFXLOGSTRING(" -> PrepareFullscreen");
    __ALFFXLOGSTRING(" -> PrepareFullscreen end");
    }

void CAlfServerDrawer::UnregisterAllAlfFullScreenEffect()
    {
    TAlfBridgerData bridgerData;  
    bridgerData.Set( EAlfUnregisterAllFx, 0, 0, NULL );              
    iWindowManager->PostIt(bridgerData);
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void CAlfServerDrawer::UnregisterAlfFullScreenEffect( TInt aAction )
    {
    TAlfBridgerData bridgerData;  
    bridgerData.Set( EAlfUnregisterEffectFx, aAction, 0, NULL );              
    iWindowManager->PostIt(bridgerData);
    }

// ---------------------------------------------------------------------------
// BeginSyncronizedGroup
// ---------------------------------------------------------------------------
//  
void CAlfServerDrawer::BeginSyncronizedGroup(TInt aGroupId)
    {
    TAlfBridgerData bridgerData;  
    bridgerData.Set( EAlfEffectFxBeginSyncronizedGroup, aGroupId, 0, NULL );              
    iWindowManager->PostIt(bridgerData);
    }

// ---------------------------------------------------------------------------
// EndSyncronizedGroup
// ---------------------------------------------------------------------------
//  
void CAlfServerDrawer::EndSyncronizedGroup(TInt aGroupId, TBool aForced)
    {
    TAlfBridgerData bridgerData;  
    bridgerData.Set( EAlfEffectFxEndSyncronizedGroup, aGroupId, aForced, NULL );              
    iWindowManager->PostIt(bridgerData);
    }
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void CAlfServerDrawer::RegisterAlfFullScreenEffect( TInt aAction, const TDesC& aResourceDir,
        const TDesC& aFileName)
    {
    TAlfBridgerData bridgerData;              
               
    TInt bufferSize = sizeof(TPtrC) + ( aResourceDir.Length()*2) + sizeof(TPtrC) + aFileName.Length() * 2;
    TInt index = 0;

    void* bridgeBuffer = NULL;
    // This function cannot actually leave
    TRAP_IGNORE( bridgeBuffer = (void*)iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ) ); 
    if ( bridgeBuffer )
       {
       RMemWriteStream stream( bridgeBuffer, bufferSize );
       // The writes should not leave if we have calculated our buffer length correctly.
       TRAP_IGNORE(
           {
           stream << aResourceDir;
           stream << aFileName;                 
           stream.CommitL();    
           });      
       bridgerData.Set( EAlfRegisterEffectFx, aAction, bufferSize, (TAny*) index );              
       iWindowManager->PostIt(bridgerData);
       stream.Close();
       }
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CAlfServerDrawer::BeginFullscreen(	TInt aAction, const TRect& aEffectRect,
                                        TInt aType, const TUid aUid1, const TUid aUid2, TInt aData,
                                        TSecureId aSid1, TInt aWg1, TSecureId aSid2, TInt aWg2 )
    {
    __ALFFXLOGSTRING2("CAlfServerDrawer::BeginFullscreen - Action: %d, Type: %d", aAction, aType);
    __ALFFXLOGSTRING4("CAlfServerDrawer::BeginFullscreen - ToAppUid/SecureId: 0x%x/0x%x, ToAppUid/SecureId: 0x%x/0x%x", aUid1, aSid1.iId, aUid2, aSid2.iId);
    if ( iFullScreenController->AppInfoCache().Action(aUid1) == aAction && 
         iFullScreenController->AppInfoCache().Action(aUid1) !=  AknTransEffect::EApplicationExit )
        {
        __ALFFXLOGSTRING2("CAlfServerDrawer::BeginFullscreen - skipping action: %d, because previous action is stronger: %d", aAction, iAction);
        // the action must be skipped, because stronger action has been request already.
        return;
        }
    
    TInt action = iFullScreenController->AppInfoCache().SetAction(aUid1, aAction);
    
    // Skip all embedded and background application effects, for now.
    // This addition is to prevent messaging app popping up in startup.
    // AVKON uses wrong parameter type and so NoEffects is not applied.
    if ( action == AknTransEffect::EEmbeddedApplicationExit ||
         action == AknTransEffect::EEmbeddedApplicationStart || 
         (action == AknTransEffect::EAppStartupBackground && aType != AknTransEffect::EParameterAvkonInternal) )
        {
        __ALFFXLOGSTRING2(" -> CAlfServerDrawer::BeginFullscreen - Embedded/background (%d) application, skip effect for AppUid: 0x%x", aAction, aUid1);
        return;
        }
  
    if (action == AknTransEffect::EAppStartupBackground && aType == AknTransEffect::EParameterAvkonInternal)
        {
    	__ALFFXLOGSTRING2(" -> CAlfServerDrawer::BeginFullscreen - Embedded/background (%d) application, skip effect for AppUid: 0x%x, but update the avkon info.", aAction, aUid1);
        }
    
    if ( aAction != AknTransEffect::ENone && aAction != AknTransEffect::EAppStartupBackground )
        {
        // no action or background activity won't stop current effect
        // Matrix Menu sends start rect and avkon sends start for the same application!
        // We assume that we want to keep the rect from Matrix Menu if the app id matches 
//        if ( !iDoNotClearEffectRect )
        if ( iToUid == aUid1 &&
            ( iAction == AknTransEffect::EApplicationStartRect || iAction == AknTransEffect::EApplicationStartSwitchRect ) &&
            ( aAction == AknTransEffect::EApplicationStart || aAction == AknTransEffect::EApplicationStartSwitch ) ) 
            {
            // if we have first got a start rect and the basic start for the same application,
            // we save the rectangle information and keep asking an effect with a rectangle
            aAction = iAction; 
            }
        else
            {
            // This is a new event, we change the rectangle
            iEffectRect = aEffectRect;
            }
        }
        
   	iFlags = AknTransEffect::TParameter::EFlagNone;
	iToUid = KNullUid;
	iFromUid = KNullUid;
	iFocusWg = KErrNotFound;
	
	iType = aType;
	
	if ( aType == AknTransEffect::EParameterType )
		{
        __ALFFXLOGSTRING("CAlfServerDrawer::BeginFullscreen - AknTransEffect::EParameterType");    
        iToUid = aUid1;
        iFromUid = aUid2;
        iFlags = aData;  
        iFromSid = aSid2;
        iToSid = aSid1;
        iFromWg = aWg2;
        iToWg = aWg1;
		}
	if ( aType == AknTransEffect::EParameterAvkonInternal )
	    {
        // here comes in the avkon info, which will tell us the window group id required for the effect EndFullScreen event
        __ALFFXLOGSTRING("CAlfServerDrawer::BeginFullscreen - AknTransEffect::EParameterAvkonInternal");    
        iToUid = aUid1;
        iParentUid = aUid2;
        iFocusWg = aData;	    
        // We save the action in case we haven't got an EndFullScreen call yet
        iOldAction = iAction;
	    }
	    
    iAction = aAction;
    // TODO: TEMPORARY HACK
    // REMOVE AFTER APPSHELL HAS BEEN FIXED
    // commented out to test if the system works now
    /*
    if ( ( iAction == AknTransEffect::EApplicationStartRect || iAction == AknTransEffect::EApplicationStartSwitchRect ) &&
        iToUid.iUid == 0x101F4CD2 )
        {
        iDoNotClearEffectRect = ETrue;
        }
    */   
    // As appshell uses strange values for folder open and close, we change them now.
    /*
    if ( iAction == 1001 && iToUid.iUid == KAknApplicationShellViewId )
       {
       iAction = AknTransEffect::EApplicationStart;
       }
    if ( iAction == 1002 && iToUid.iUid == KAknApplicationShellViewId  )
       {
       // This might not be the right one, but we try if this is OK
       iAction = AknTransEffect::EApplicationExit;
       }
    */       
	    
    __ALFFXLOGSTRING2("CAlfServerDrawer::BeginFullscreen - From UID: 0x%X, To UID: 0x%X, Application UID: 0x%X", iFromUid.iUid, iToUid.iUid );    
    __ALFFXLOGSTRING3("CAlfServerDrawer::BeginFullscreen - Parent UID: 0x%X, Focus window group UID: 0x%X, Flags: 0x%X", iParentUid.iUid, iFocusWg, iFlags);    
    if( aAction == AknTransEffect::ELayoutSwitch )
        {
        iScrModeChangedState = EScreenModeBeginHasBeenCalled;
        }
    else 
        {
        iScrModeChangedState = EScreenModeChangedIdle;
        }

    iFullScreenController->BeginFullscreen( aType, aUid1, aUid2, aData );
    __ALFFXLOGSTRING("CAlfServerDrawer::BeginFullscreen end");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfServerDrawer::EndFullscreen(TBool aTimeout)
    {
    __ALFFXLOGSTRING("CAlfServerDrawer::EndFullscreen >>");
    CancelEndChecker();
    iFullScreenController->EndFullscreen(aTimeout);
    iFullScreenController->AppInfoCache().ClearActions();
    __ALFFXLOGSTRING("CAlfServerDrawer::EndFullscreen <<");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CAlfServerDrawer::CancelFullscreen()
    {
    __ALFFXLOGSTRING(" -> CAlfServerDrawer::CancelFullscreen");
    __ALFFXLOGSTRING("CAlfServerDrawer::CancelFullscreen end");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
void CAlfServerDrawer::AbortFullscreen()
    {
    __ALFFXLOGSTRING(" -> CAlfServerDrawer::AbortFullscreen");
    iFullScreenController->AbortTransition(EAbortFullscreen);
    iFullScreenController->AppInfoCache().ClearActions();
    __ALFFXLOGSTRING("CAlfServerDrawer::AbortFullscreen end");
    }

TBool CAlfServerDrawer::EffectFinishedFullScreen() const
    {
    return iFullScreenFinished;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CAlfServerDrawer::SendBeginFullscreen()
    {
    TRAP_IGNORE(DoSendBeginFullscreenL());
    }
    
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void CAlfServerDrawer::DoSendBeginFullscreenL()
    {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    TTime time;
    time.UniversalTime();
    AMT_ADD_TIME(iToSid.iId, time.Int64(), ETrue);
#endif    
    __ALFFXLOGSTRING(" <- CAlfServerDrawer::SendBeginFullscreen");
    __ALFFXLOGSTRING(" <- Original window server ids");
    __ALFFXLOGSTRING2("From UID: 0x%X, To UID: 0x%X", iFromUid.iUid, iToUid.iUid);    
    __ALFFXLOGSTRING2("Parent UID: 0x%X, Focus window group UID: 0x%X", iParentUid.iUid, iFocusWg);    
    TInt err = KErrNone;
    
    // This should be only the first half of the effect, the second part should start
    // when we get EndFullScreen
    // Originally this was where the screen capture was made and the drawing redirected.
    // Here we can start doing some effect for the application that is currently on the
    // screen. The new application effect should start at "EndFullScreen"
    // That normally means that the application has drawn something to its window.
        
    TBool blocked = iFullScreenController->IsBlocked( iFromUid, iToUid );
    if (blocked)
        {
        return;
        }

    TBool isExit = ( iAction == AknTransEffect::EApplicationExit );
    
    // We should probably get out here if the uids are on the blocked list.
    
    // It appears that the exiting application is the second id
    // The same application appears as second id both when it starts and exits!

    // Get the wanted full screen effect from the policy handler
    
    TInt cachePriority = 0;
   	TInt wantedTime = 0;
	TInt minTime = 0;
	TPtrC resourceDir;
	TPtrC fileName;

    err = iPolicyHandler.GetFullscreenKml( iAction, iToUid, iFromUid, resourceDir, fileName,
        cachePriority, wantedTime, minTime );
    
    
    if ( err != KErrNone || fileName.Length() == 0 || !FileExistsL( resourceDir, fileName ) )
        {
        // Did not find an effect file - nothing to show
        __ALFFXLOGSTRING2("CAlfServerDrawer::DoSendBeginFullscreenL -  Could not get full screen kml, error %d %S", err, &fileName)
        iFromWg = KErrNotFound;
        iToWg = KErrNotFound;
        iFromScreen = KErrNotFound;
        iToScreen = KErrNotFound;
    	return;
        }
        
    // TODO: This context switch is done because matrix menu uses opposite context numbers
    // from the screen saver, and we want to handle folder open the same way as application
    // open and folder close the same way as application close.
    // We may need to change this after we have tested the system and seen how it works
    // (if at all)
    // We may need to assign some unique numbers for the folder open and close to make the system work.
    
    
    // TODO: We do the switch with screen saver hoping it is the only one using reverse logic 
    
    if ( iAction == 1001 && iToUid.iUid == KScreensaverUid )
        {
        iAction = AknTransEffect::EApplicationExit;
        }
    
    if ( iAction == 1002 && iToUid.iUid == KScreensaverUid )
        {
        iAction = AknTransEffect::EApplicationStart;
        }
    
/*    
    if ( iAction == 1001 && iToUid.iUid == KAknApplicationShellViewId )
       {
       iAction = AknTransEffect::EApplicationStart;
//       iAction = 1002;
       }
   if ( iAction == 1002 && iToUid.iUid == KAknApplicationShellViewId  )
       {
//       iAction = AknTransEffect::EApplicationExit;
       }
*/       
       
    
    // Send the data to CAlfAppUI via bridge
    
    TAlfBridgerData bridgerData;
    
    // We must make a stream of the data as the length may vary.
    // At the moment we only send some numbers, but we should probably include
    // also the effect file name or something else to identify the effect 
    // that is going to be used
    
    TInt bufferSize = 13 * sizeof(TInt); //operation, TYPE, 2 wg ids, 2 app uids, 2 secure ids, flags and rect
    bufferSize += sizeof(TPtrC);
    bufferSize += resourceDir.Length() * 2;
    bufferSize += sizeof(TPtrC);
    bufferSize += fileName.Length() * 2;
    
    // I don't think we need this
    TInt index = 0;

    iCurrentFullScreenHandle = iToSid.iId;

    iFullScreenTransitionEndObserver->Cancel();
    iFinishFullScreen->Cancel();
    iFullScreenTimeout->Cancel();

    // The first stage should actually just show some wait dialog - it is waiting for the second
    // stage of the effect to start.
    void* bridgeBuffer = (void*) iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ); 
    if ( bridgeBuffer )
        {
        RMemWriteStream stream( bridgeBuffer, bufferSize );
        // The writes should not leave if we have calculated our buffer length correctly.
        stream.WriteInt32L( MAlfGfxEffectPlugin::EBeginFullscreen );
        stream.WriteInt32L(  iToSid.iId );
        stream.WriteInt32L( iType );
        stream.WriteInt32L( 0 ); // timeout? 0 always in BeginFullScreen
        stream.WriteInt32L( 0 );
      	if ( iType == AknTransEffect::EParameterType && isExit )
      	    {
            stream.WriteInt32L( iToWg );
            stream.WriteInt32L( 0 );

            // AppUid
            stream.WriteInt32L( iToUid.iUid );
            stream.WriteInt32L( iFromUid.iUid );
            
            // SecureId
            stream.WriteInt32L( iToSid.iId );
            stream.WriteInt32L( iFromSid.iId );
                        
       	    }
       	else if ( iType == AknTransEffect::EParameterType )
       	    {
            stream.WriteInt32L( iToWg );
            stream.WriteInt32L( iFromWg );

            // AppUid
            stream.WriteInt32L( iToUid.iUid );
            stream.WriteInt32L( iFromUid.iUid );
            
            // SecureId
            stream.WriteInt32L( iToSid.iId );
            stream.WriteInt32L( iFromSid.iId );
                        
            }
        else
            {
            stream.WriteInt32L( KErrNotFound);  // dummy BEGIN
            stream.WriteInt32L( KErrNotFound);  
            stream.WriteInt32L( KErrNotFound);  
            stream.WriteInt32L( KErrNotFound); 
            stream.WriteInt32L( KErrNotFound);  
            stream.WriteInt32L( KErrNotFound);  // dummy END
            }
        stream.WriteInt32L( iFlags );
        stream.WriteInt32L( iEffectRect.iTl.iX );
        stream.WriteInt32L( iEffectRect.iTl.iY );
        stream.WriteInt32L( iEffectRect.iBr.iX );
        stream.WriteInt32L( iEffectRect.iBr.iY );
        
        stream << resourceDir;
        stream << fileName;    
        stream.CommitL();
        bridgerData.Set( EAlfEffectFx, iAction, bufferSize, (TAny*) index );
        __ALFFXLOGSTRING(" <- CAlfServerDrawer::sending bridgedata");
        iFullScreenEndSent = EFalse;
        iFullScreenFxSent = ETrue;
        
        iWindowManager->PostIt( bridgerData );
        
        stream.Close();
        iFullScreenTransitionEndObserver->StartObserving( TCallBack( FullScreenTransitionFinished, this ), iCurrentFullScreenHandle );
        }
    
    // iFlags and iAction will contain some extra information that can be used to control the effect:
    
    // All of these actions may not produce effects, they are here for now for debugging.
    TBool isAppStartEffect(EFalse);
    switch ( iAction )
        {
        case AknTransEffect::EApplicationActivate:
            __ALFFXLOGSTRING("EApplicationActivate - 1");
            break;
        case AknTransEffect::EApplicationStart:
            isAppStartEffect = ETrue;
            __ALFFXLOGSTRING("EApplicationStart - 3");
            break;
        case AknTransEffect::EEmbeddedApplicationStart:
            __ALFFXLOGSTRING("EEmbeddedApplicationStart - 4");
            break;
        case AknTransEffect::EApplicationExit:
            __ALFFXLOGSTRING("EApplicationExit - 5");
            break;
        case AknTransEffect::EApplicationStartRect:
            isAppStartEffect = ETrue;
            __ALFFXLOGSTRING("EApplicationStartRect - 6");
            break;
        case AknTransEffect::EApplicationStartSwitch:
            isAppStartEffect = ETrue;
            __ALFFXLOGSTRING("EApplicationStartSwitch - 7");
            break;
        case AknTransEffect::EApplicationStartSwitchRect:
            isAppStartEffect = ETrue;
            __ALFFXLOGSTRING("EApplicationStartSwitchRect - 8");
            break;
        case AknTransEffect::EEmbeddedApplicationExit:
            __ALFFXLOGSTRING("EEmbeddedApplicationExit - 10");
            break;
        case AknTransEffect::EAppStartComplete:
            __ALFFXLOGSTRING("EAppStartComplete - 11");
            break;
        case AknTransEffect::ELayoutSwitch:
            __ALFFXLOGSTRING("ELayoutSwitch - 12");
            break;
        case AknTransEffect::ELayoutSwitchExit:
            __ALFFXLOGSTRING("ELayoutSwitchExit - 13");
            break;
        case AknTransEffect::ELayoutSwitchStart:
            __ALFFXLOGSTRING("ELayoutSwitchStart - 14");
            break;
        case AknTransEffect::EAppSpesificEvent:
            __ALFFXLOGSTRING("EAppSpesificEvent - 1000");
            break;
        case AknTransEffect::EAppStartupBackground:
            __ALFFXLOGSTRING("EAppStartupBackground - 15");
            break;
        default:
            __ALFFXLOGSTRING1("%d", iAction);
            break;
        }

    // This is a precaution for cases where we never get end full screen
    // the delay is too long for normal cases, but because of the slowness of the emulater
    // the delay must be long enough so that it does not interfere with actual
    // endfullscreen signals.
    
    // It appears that folder open and close never send endfullscreen, so for them the effect
    // must be started as soon as we get beginfullscreen
    
    // isAppStartEffect = EFalse; // disable different timeout effect for appstart effects
	
    if (isAppStartEffect)
        {
		// The shorter timeout can be disabled here!
        iFullScreenTimeout->Start( KAlfAppStartEffectTimeout, TCallBack( FullScreenTimeout, this ) );
        iFullScreenTimeout->iIsStartEffect = ETrue;
        }
    else
        {
        iFullScreenTimeout->Start( KAlfLongEffectTimeout, TCallBack( FullScreenTimeout, this ) );
        iFullScreenTimeout->iIsStartEffect = EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CAlfServerDrawer::SendEndFullscreen(TBool aTimeout)
    {
    __ALFFXLOGSTRING3("CAlfServerDrawer::SendEndFullscreen - Timeout: %d, iFullScreenEndSent: %d, IsStartEffect: %d", aTimeout, iFullScreenEndSent, iFullScreenTimeout->iIsStartEffect);
    
    if ( iFullScreenEndSent )
        {
        __ALFFXLOGSTRING("CAlfServerDrawer::SendEndFullscreen END no need");
        return KErrNone;
        }
        
    __ALFFXLOGSTRING("CAlfServerDrawer::SendEndFullscreen");
    iFullScreenTransitionEndObserver->Cancel();
    iFullScreenTimeout->Cancel();
    iFinishFullScreen->Cancel();
    TBool timeout = iFullScreenTimeout->iTimeoutTriggered;
    TBool triggerStartEffectTimeout= iFullScreenTimeout->iTimeoutTriggered && iFullScreenTimeout->iIsStartEffect;
    iFullScreenEndSent = !triggerStartEffectTimeout;
    iFullScreenTimeout->iTimeoutTriggered = EFalse;
    iFullScreenTimeout->iIsStartEffect = EFalse;
    
     // This is where the second part of the effect should start
    // The new window should already have something on the screen.
	// It might not be the case, if aTimeout is ETrue 
    
    // Send the data to CAlfAppUI via bridge
    
    TAlfBridgerData bridgerData;
    
    // Get the wanted full screen effect from the policy handler
    
	TPtrC resourceDir;
	TPtrC fileName;
	TInt cachePriority = 0;
    TInt wantedTime = 0;
    TInt minTime = 0;

	if (triggerStartEffectTimeout)
	    {
        // if the following fails, then the effect from beginfullscreen will be used.
        TInt err = iPolicyHandler.GetFullscreenKml( KAlfLongApplicationStartContext, iToUid, iFromUid, resourceDir, fileName,
        cachePriority, wantedTime, minTime );
		if (err != KErrNone)
			{
			triggerStartEffectTimeout = EFalse;
			}
	    }
	
    // We must make a stream of the data as the length may vary.
    // At the moment we only send some numbers, but we should probably include
    // also the effect file name or something else to identify the effect 
    // that is going to be used
    
    TInt bufferSize = 13 * sizeof(TInt); // operation, type, 2 wg ids, 2 app uids, flags and rect, timeoutTriggered
    bufferSize += sizeof(TPtrC);
    bufferSize += resourceDir.Length() * 2;
    bufferSize += sizeof(TPtrC);
    bufferSize += fileName.Length() * 2;
    
    TInt index = 0;

    // Send End fullscreen indication. Note that most of the data is invalid
    // and those should not be used in alfbridge. Data cannot be guaranteed
    // to be valid as we may receive multiple BeginFullscreen before EndFullscreen.
    
    void* bridgeBuffer = NULL;
    // This function cannot actually leave
    TRAP_IGNORE( bridgeBuffer = (void*) iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ) ); 
    if ( bridgeBuffer )
        {
        RMemWriteStream stream( bridgeBuffer, bufferSize );
        // The writes should not leave if we have calculated our buffer length correctly.
        TRAP_IGNORE(
            {
            stream.WriteInt32L( MAlfGfxEffectPlugin::EEndFullscreen );
            stream.WriteInt32L( iCurrentFullScreenHandle );
            stream.WriteInt32L( iType );
            stream.WriteInt32L( triggerStartEffectTimeout );
            stream.WriteInt32L( timeout );
            stream.WriteInt32L( iToWg );
            stream.WriteInt32L( iFromWg );

            // AppUid for the future. alfbridge can figure out the windog groups based on that
            stream.WriteInt32L( iToUid.iUid );
            stream.WriteInt32L( iFromUid.iUid );
            
            stream.WriteInt32L( iToSid.iId );
            stream.WriteInt32L( iFromSid.iId );

            stream.WriteInt32L( iFlags ); 
            stream.WriteInt32L( iEffectRect.iTl.iX );
            stream.WriteInt32L( iEffectRect.iTl.iY );
            stream.WriteInt32L( iEffectRect.iBr.iX );
            stream.WriteInt32L( iEffectRect.iBr.iY );
            
            stream << resourceDir;
            stream << fileName;    
            stream.CommitL();    
            });    
                
        bridgerData.Set( EAlfEffectFx, iAction, bufferSize, (TAny*) index );
        __ALFFXLOGSTRING1("CAlfServerDrawer::SendEndFullscreen - sending bridgedata, Current handle %d", iCurrentFullScreenHandle);
        iWindowManager->PostIt( bridgerData );
        stream.Close();
        iFullScreenTransitionEndObserver->StartObserving( TCallBack( FullScreenTransitionFinished, this ), iCurrentFullScreenHandle  );
        }

    iFullScreenFinished = EFalse;
    iFullScreenTimeout->iIsStartEffect = EFalse;
 	
    if (triggerStartEffectTimeout)
        {
        //return KErrNone;  // even long app start effects need some hardcore timeout
        }
    if ( iAction == AknTransEffect::EApplicationExit /*||
        iAction == 1001*/  ) 
        {
		// no more events expected
        iFinishFullScreen->Start( KAlfShortEffectTimeout, TCallBack( FinishFullScreenTimeout, this ) );
        }
    else
        {
        iFinishFullScreen->Start( KAlfLongEffectTimeout, TCallBack( FinishFullScreenTimeout, this ) );
        }
    return triggerStartEffectTimeout;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	
TInt CAlfServerDrawer::SendAbortFullscreen()
    {
    __ALFFXLOGSTRING(" <- CAlfServerDrawer::SendAbortFullscreen");
    // The windows may close any time now because we are aborting
   
    if (!iFullScreenFxSent)
        {
        return KErrNone;
        }

    iFullScreenTransitionEndObserver->Cancel();
    iFinishFullScreen->Cancel();
    iFullScreenTimeout->Cancel();
    iFullScreenFxSent = EFalse;
    iFullScreenEndSent = ETrue;
    
    // Send the data to CAlfAppUI via bridge
    TAlfBridgerData bridgerData;
    
    // We must make a stream of the data as the length may vary.
    // This function only sends numbers, others send filenames, too.
    
    TInt bufferSize = 11 * sizeof(TInt32); // type, 2 wg ids, 2 app uids, flags and rect
    
    TInt index = 0;
    void* bridgeBuffer = NULL;
    // This function cannot actually leave
    TRAP_IGNORE( bridgeBuffer = (void*) iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ) ); 
    if ( bridgeBuffer )
        {
        RMemWriteStream stream( bridgeBuffer, bufferSize );
        // The writes should not leave if we have calculated our buffer length correctly.
        TRAP_IGNORE(
            {
            stream.WriteInt32L( MAlfGfxEffectPlugin::EAbortFullscreen );
            stream.WriteInt32L( iCurrentFullScreenHandle );
            stream.WriteInt32L( iType );
          	//if ( iType == AknTransEffect::EParameterType )
          	    {
                stream.WriteInt32L( iToWg );
                stream.WriteInt32L( iFromWg );
                stream.WriteInt32L( iToUid.iUid );
                stream.WriteInt32L( iFromUid.iUid );
                                    
                stream.WriteInt32L( iToScreen );
                stream.WriteInt32L( iFromScreen );
                }
            /*else
                {
                stream.WriteInt32L( iFocusWg );
                stream.WriteInt32L( iFocusWg );
                }*/
            stream.WriteInt32L( iFlags ); 
            stream.CommitL();    
            }
            );
        bridgerData.Set( EAlfStopEffectFx, iAction, bufferSize, (TAny*) index );
        __ALFFXLOGSTRING(" <- CAlfServerDrawer::sending bridgedata");
        iWindowManager->PostIt( bridgerData );
        stream.Close();
        }
        
    iFullScreenFinished = ETrue;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TInt CAlfServerDrawer::SendBeginControlTransition()
    {
    __ALFFXLOGSTRING(" <- CAlfServerDrawer::SendBeginControlTransition");
    TInt cachePriority = 0;
   	TInt wantedTime = 0;
	TInt minTime = 0;
	TPtrC resourceDir;
	TPtrC fileName;
	TPtrC actionString;
	
	TInt err = KErrNone;
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
	TTime time;
    time.UniversalTime();
    AMT_ADD_TIME(iControlHandle, time.Int64(), ETrue);
#endif    
	err = iPolicyHandler.GetControlKml( iControlUid, iControlAction, resourceDir,
	    fileName, actionString, cachePriority, wantedTime, minTime );
	    
	if ( err != KErrNone )
	    {
        return err;
	    }
	
	TBool fileExists = EFalse;
	TRAP_IGNORE(fileExists = FileExistsL( resourceDir, fileName ));
	
	if ( fileName.Length() == 0 || !fileExists)
	    {
        __ALFFXLOGSTRING2("CAlfServerDrawer::SendBeginControlTransition -  Could not get control effect kml, file exists: %d %S. ABORTING!", fileExists, &fileName)
	    return KErrNotFound;
	    }
	    
	// We must append the action string to the filename to be able to use different actions
	// for control entry and exit.
	
	TParse parse;
	parse.Set( fileName, NULL, NULL );
	
	TFileName actionFile;
	if ( actionString.Length() > 0 )
	    {
    	actionFile.Copy( parse.Name() );
    	actionFile.Append( _L("_") );
    	actionFile.Append( actionString );
    	actionFile.Append( parse.Ext() );
    	}
    else
        {
    	actionFile.Copy( fileName );
        }
        
    // Send the data to CAlfAppUI via bridge
    TAlfBridgerData bridgerData;
    
    TInt bufferSize = 9 * sizeof(TInt); // operation, action
    bufferSize += sizeof(TPtrC);
    bufferSize += resourceDir.Length() * 2;
    bufferSize += sizeof(TPtrC);
    bufferSize += fileName.Length() * 2;

    TInt index = 0;

    IncreaseControlHandle();
    iControlTransitionEndObserver->Cancel();
    iControlTimeout->Cancel();

    // control transitions are not supposed to have the phasing of full screen transitions,
    // we should be ready to go immediately.
    
    // However, if we are doing control exit, we should reserve the window in order
    // to keep it on screen while the effect lasts.
 
    void* bridgeBuffer = NULL;
    // This function cannot actually leave
    TRAP_IGNORE( bridgeBuffer = (void*) iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ) ); 
    if ( bridgeBuffer )
        {
        RMemWriteStream stream( bridgeBuffer, bufferSize );
        // The writes should not leave if we have calculated our buffer length correctly.
        TRAP_IGNORE(
            {
            stream.WriteInt32L( MAlfGfxEffectPlugin::EBeginComponentTransition );
            stream.WriteInt32L( iControlHandle ); //iCurrentControlHandle );
            stream.WriteInt32L( iControlHandle ); // window handle
            stream.WriteInt32L( iControlWindowGroup ); // window group id
            stream.WriteInt32L( 0 ); // "screen number"; not used; save place for future
            // should send starting and ending points and possible other position/rectange
            // data, too.
            // Also, should send the action string if the effect parser can handle it
            stream << resourceDir;
            stream << actionFile;    
            stream.CommitL();
            });
        bridgerData.Set( EAlfControlEffectFx, iControlAction, bufferSize, (TAny*) index );
        __ALFFXLOGSTRING1("CAlfServerDrawer::SendBeginControlTransition - sending bridgedata, Current handle: %d", iControlHandle)
        iWindowManager->PostIt( bridgerData );
        stream.Close();
        iControlTransitionEndObserver->StartObserving( TCallBack( ControlTransitionFinished, this ), iControlHandle );
        
        if ( iActiveControlFx.Find( iControlHandle ) == KErrNotFound )
            {
            iActiveControlFx.Append( iControlHandle );
            }
        }
	
    iFullScreenFinished = EFalse;
    
    iControlTimeout->Start( KAlfLongEffectTimeout, TCallBack( ControlTimeout, this ) );
	
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TInt CAlfServerDrawer::SendFinishControlTransition()
    {
    // We should now delete the effects from any controls that remain active.
    __ALFFXLOGSTRING("CAlfServerDrawer::SendFinishControlTransition >>");
    iControlTransitionEndObserver->Cancel();
    
    while ( iActiveControlFx.Count() )
        {
        TInt handle = iActiveControlFx[ iActiveControlFx.Count() - 1 ];
        iActiveControlFx.Remove( iActiveControlFx.Count() - 1 );
        DoSendFinishControlTransition( handle );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CAlfServerDrawer::DoSendFinishControlTransition(TInt aHandle)
    {
    __ALFFXLOGSTRING1("CAlfServerDrawer::DoSendFinishControlTransition %d >>", aHandle)
    	
    // Send the data to CAlfAppUI via bridge
    TAlfBridgerData bridgerData;
    
    // We must make a stream of the data as the length may vary.
    // This function only sends numbers, others send filenames, too.
    
    TInt bufferSize = 9 * sizeof(TInt); // type, 2 wg ids, flags and rect
    TInt index = 0;

    void* bridgeBuffer = NULL;
    // This function cannot actually leave
    TRAP_IGNORE( bridgeBuffer = (void*) iWindowManager->Bridge()->AppendEffectsDataL( bufferSize, index ) ); 
    if ( bridgeBuffer )
        {
        RMemWriteStream stream( bridgeBuffer, bufferSize );
        // The writes should not leave if we have calculated our buffer length correctly.
        TRAP_IGNORE(
            {
            stream.WriteInt32L( MAlfGfxEffectPlugin::EAbortComponentTransition );
            stream.WriteInt32L( aHandle );
            stream.WriteInt32L( 0 );
            stream.WriteInt32L( 0 );
            stream.WriteInt32L( 0 );
            stream.WriteInt32L( 0 ); 
            stream.CommitL();    
            });
			// TODO, check, if iCurrentHandle is approriate
        bridgerData.Set( EAlfStopControlEffectFx, aHandle, bufferSize, (TAny*) index );
        __ALFFXLOGSTRING1("CAlfServerDrawer::sending bridgedata, Stop control handle: %d", aHandle);
        iWindowManager->PostIt( bridgerData );
        stream.Close();
        }
    __ALFFXLOGSTRING("CAlfServerDrawer::DoSendFinishControlTransition <<")
    return KErrNone;
    }
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TInt CAlfServerDrawer::SendAbortControlTransition()
    {
    __ALFFXLOGSTRING(" <- CAlfServerDrawer::SendAbortControlTransition");
    iControlTransitionEndObserver->Cancel();
    // We should now delete the effects from any controls that remain active.
    return SendFinishControlTransition();
    }

// ---------------------------------------------------------------------------
// CAlfServerDrawer::FromUid
// ---------------------------------------------------------------------------
//		
TUid& CAlfServerDrawer::FromUid()
	{
	return iFromUid;
	}

// ---------------------------------------------------------------------------
// CAlfServerDrawer::FromSid
// ---------------------------------------------------------------------------
//      
TSecureId& CAlfServerDrawer::FromSid()
    {
    return iFromSid;
    }
    
// ---------------------------------------------------------------------------
// CAlfServerDrawer::FromWg
// ---------------------------------------------------------------------------
//      
TInt CAlfServerDrawer::FromWg()
    {
    return iFromWg;
    }


// ---------------------------------------------------------------------------
// CAlfServerDrawer::ToUid
// ---------------------------------------------------------------------------
//   	
TUid& CAlfServerDrawer::ToUid()
	{
	return iToUid;
	}

// ---------------------------------------------------------------------------
// CAlfServerDrawer::ToSid
// ---------------------------------------------------------------------------
// 
TSecureId& CAlfServerDrawer::ToSid()
    {
    return iToSid;
    }

// ---------------------------------------------------------------------------
// CAlfServerDrawer::ToWg
// ---------------------------------------------------------------------------
// 
TInt CAlfServerDrawer::ToWg()
    {
    return iToWg;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TUint& CAlfServerDrawer::Action()
	{
	return iAction;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TInt& CAlfServerDrawer::Flags()
	{
	return iFlags;	
	}

// ---------------------------------------------------------------------------
// Guarantee, that control and fullscreen handles are unique. 
// ---------------------------------------------------------------------------
//		
void CAlfServerDrawer::IncreaseControlHandle()
	{
    // handles must be unique, but increasing control handle should not change fullscreen handle
    TInt highestHandle = iCurrentControlHandle > iCurrentFullScreenHandle ? iCurrentControlHandle : iCurrentFullScreenHandle;
    highestHandle++;
    iCurrentControlHandle = highestHandle; 
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
TInt CAlfServerDrawer::CurrentControlHandle()
	{
	return iCurrentControlHandle;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
TInt CAlfServerDrawer::CurrentFullScreenHandle()
    {
    return iCurrentFullScreenHandle;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
void CAlfServerDrawer::StartEndChecker()
	{
    iFullScreenController->StartEndChecker();
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//		
void CAlfServerDrawer::CancelEndChecker()
	{
    iFullScreenController->CancelEndChecker();
	}

TInt CAlfServerDrawer::FullScreenTimeout( TAny* aServerDrawer )
    {
    static_cast<CAlfServerDrawer*>(aServerDrawer)->HandleFullScreenTimeout();
    return KErrNone;
    }

void CAlfServerDrawer::HandleFullScreenTimeout()
    {
    if ( iFullScreenTimeout->iStatus.Int() != KErrCancel )
        {
        iFullScreenTimeout->Cancel();
        // If we don't get an EndFullScreen in a timely manner, 
        // we generate an EndFullScreen signal by ourselves
        EndFullscreen(ETrue);
        }
    }

TInt CAlfServerDrawer::FinishFullScreenTimeout( TAny* aServerDrawer )
    {
    static_cast<CAlfServerDrawer*>(aServerDrawer)->HandleFinishFullScreenTimeout();
    return KErrNone;
    }

void CAlfServerDrawer::HandleFinishFullScreenTimeout()
    {
    iFullScreenTransitionEndObserver->Cancel();
    iFullScreenTimeout->Cancel();
    // if we haven't got a pubsub trigger,
    // we must finish the transition by calling AlfBridge.
    // If we gor the pubsub trigger, AlfBridge has done its part,
    // and all we have to do is release the reserved windows.
    iFullScreenController->FullscreenFinished( iCurrentFullScreenHandle );
    }

TInt CAlfServerDrawer::ControlTimeout( TAny* aServerDrawer )
    {
    static_cast<CAlfServerDrawer*>(aServerDrawer)->HandleControlTimeout();
    return KErrNone;
    }

void CAlfServerDrawer::HandleControlTimeout()
    {
    iControlTransitionEndObserver->Cancel();
    iFullScreenFinished = EFalse;
    iControlTimeout->Cancel();
    iControlController->EndControlTransition( iCurrentControlHandle );
    }

TInt CAlfServerDrawer::FullScreenTransitionFinished( TAny* aServerDrawer )
    {
    static_cast<CAlfServerDrawer*>(aServerDrawer)->HandleFullScreenTransitionFinished();
    return KErrNone;
    }

TInt CAlfServerDrawer::ControlTransitionFinished( TAny* aServerDrawer )
    {
    static_cast<CAlfServerDrawer*>(aServerDrawer)->HandleControlTransitionFinished();
    return KErrNone;
    }


void CAlfServerDrawer::HandleFullScreenTransitionFinished()
    {
    iFullScreenFinished = ETrue;
    iFinishFullScreen->Cancel();
    iFullScreenTimeout->Cancel();
    iFullScreenController->FullscreenFinished( iCurrentFullScreenHandle );
    } 

void CAlfServerDrawer::HandleControlTransitionFinished() 
    {
    iControlTimeout->Cancel();
    iControlController->EndControlTransition( iCurrentControlHandle );
    }


// ---------------------------------------------------------------------------
TBool CAlfServerDrawer::FileExistsL( TPtrC& aResourceDir, TPtrC& aFileName )
	{
	HBufC* fullName = HBufC::NewL( aResourceDir.Length() + aFileName.Length() );
	CleanupStack::PushL( fullName );
	fullName->Des().Copy( aResourceDir );
	fullName->Des().Append( aFileName );
	TBool found = EFalse;
	if (iFs.Handle() && BaflUtils::FileExists( iFs, *fullName ))
		{
        __ALFFXLOGSTRING1("CAlfServerDrawer::FileExists : %S", fullName);
        found = ETrue;
		}
	else
	    {
	
#ifdef __WINS__
       // Check for valid effect names during loading. DO NOT ENABLE IN BUILDS
       // __ASSERT_DEBUG(0,USER_INVARIANT());
#endif
	    }
	CleanupStack::PopAndDestroy(fullName);
	return found;
	}
