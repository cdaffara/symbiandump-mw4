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
* Description:   Server session
*
*/


#include <flogger.h>
#include "alfappsrvsession.h"
#include "alf/alfappserver.h"
#include "alf/alfappui.h"
#include "alf/alfconstants.h"
#include "alf/alfclientwindow.h"
#include "alfsrvdisplaysubsession.h"
#include "alfsrvcontrolgroupsubsession.h"
#include "alfsrvsettingshandler.h"
#include "alfsrvtranseffect.h"
#include "alf/alfsrveffectenv.h"
#include "alf/alfserverutils.h"
#include "alflogger.h"
#include "alfclientserver.h"
#include "alfsrvtexturemanager.h"
#include "alfsrvscreenbuffermanager.h"
#include "alfstreamerconsts.h"
#include "alfdecoderserverclient.h"



#include <coemain.h>

#include <alf/alfroster.h>
#include <alf/alftexture.h>

#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiDisplayCoeControl.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiTextVisual.h>
#include <uiacceltk/HuiTransformation.h>
#include <uiacceltk/HuiBorderBrush.h>
#include <uiacceltk/HuiImageVisual.h>
#include <uiacceltk/HuiGridLayout.h>
#include <uiacceltk/HuiTextureProcessor.h>
#include <uiacceltk/HuiSegmentedTexture.h>
#include <uiacceltk/huitextstylemanager.h>
#include <uiacceltk/huitextstyle.h>
#include <uiacceltk/HuiFont.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiUtil.h>

_LIT( KAlfFPSLogDir, "alf"); // remember to create the c:\logs\alf in order to enable logging
_LIT( KAlfFPSLogFile, "alffpslog.txt");
    

const TReal32 KAlfUseDefaultFrameRate = 0.f;

TBool RealCompare( 
    TReal32 aCompare1, 
    TReal32 aCompare2, 
    TReal32 aEpsilon = 0.001f  )
    {
    if ( Abs(aCompare1 - aCompare2) < aEpsilon )
        {
        return ETrue;
        }
    return EFalse;
    }

/**
 * Size of command batch buffer allocated from stack.
 * (This is used if all else fails)
 */
const TInt KAlfStackBatchBufferSize = 128;

// bitmap provider

NONSHARABLE_CLASS(CSharedBitmapProvider) : public CBase, public MHuiBitmapProvider
	{
public:
	CSharedBitmapProvider();
	CSharedBitmapProvider(TInt aBitmapHandle, TInt aMaskHandle);
	~CSharedBitmapProvider();
	// from MHuiBitmapProvider	
	void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);				
private:
	TInt iBitmapHandle;
	TInt iMaskHandle;
	};

CSharedBitmapProvider::CSharedBitmapProvider()
	{
		
	}

CSharedBitmapProvider::CSharedBitmapProvider(TInt aBitmapHandle, TInt aMaskHandle)
	{
	iBitmapHandle = aBitmapHandle;
	iMaskHandle = aMaskHandle;	
	}

CSharedBitmapProvider::~CSharedBitmapProvider()
	{
	}

void CSharedBitmapProvider::ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask)
	{
	CFbsBitmap* bitmap(0);
	CFbsBitmap* mask(0);
	
	if (iBitmapHandle) // mandatory
	    {
	    bitmap = new (ELeave) CFbsBitmap;
	    CleanupStack::PushL(bitmap);
	    bitmap->Reset();
	    User::LeaveIfError(bitmap->Duplicate(iBitmapHandle));
	    }
	else
	    {
	    User::Leave(KErrArgument);
	    }
	       
	if ( iMaskHandle ) // optional
	    {
     	mask = new (ELeave) CFbsBitmap;
	    CleanupStack::PushL(mask);
	    mask->Reset();
	    User::LeaveIfError(mask->Duplicate(iMaskHandle));
	    CleanupStack::Pop(); // mask
	    }
	
    CleanupStack::Pop(); // bitmap
    aBitmapMask = mask; 	
    aBitmap = bitmap; 	
    }



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfAppSrvSession::CAlfAppSrvSession(const CAlfAppServer* aServer)
    : CAlfAppSrvSessionBase(aServer),
      iExecutionFlags( 
        EAlfExecuteUsingDynamicBuffer + EAlfExecuteUsingCommonBuffer )
    {
    }

CAlfAppSrvSession* CAlfAppSrvSession::NewL(const CAlfAppServer* aServer)
    {
    CAlfAppSrvSession* me = new (ELeave) CAlfAppSrvSession(aServer);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    return me;
    }

void CAlfAppSrvSession::ConstructL()
    {
    BaseConstructL();
    
    AlfServer()->SetWindowChangeObserverL(this, TInt(this)); 
    
    iRefreshMode = EHuiRefreshModeAutomatic;
    iMaxFrameRate = KAlfUseDefaultFrameRate;
    iIdleThreshold = 10 * 1000; // 10 from HuiEnv as seconds
    iEffectEnv = CAlfSrvEffectEnv::NewL(*this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfAppSrvSession::~CAlfAppSrvSession()
    {
    iOwnershipItems.ResetAndDestroy();    
    
    if ( iTextureOwnedIdSet )
        {
        CAlfSrvTextureManager& manager = AlfTextureManager();
        TProcessId ownerId = TextureOwnerId();
        manager.DestroyDeletedTextureIds( ownerId );
        manager.RemoveClient( *this );
        }
    iTextures.ResetAndDestroy();        
    iAnimatedTextures.ResetAndDestroy();
    
    delete iEffectEnv;
    iEffectEnv = NULL;
    
    iTextureInfo.Close();
    
    iControlGroupOrder.Close();
    }


TRect CAlfAppSrvSession::ClientDrawingArea() const
    {
    return iClientDisplayRect;
    }
    
void CAlfAppSrvSession::SetClientDrawingArea( const TRect& aRect )
    {
    TRAP_IGNORE(SetClientDrawingAreaL(aRect))
    }

void CAlfAppSrvSession::SetClientDrawingAreaL( const TRect& aRect )
    {
    iClientDisplayRect = aRect;
    
    // Update all the control groups
    RPointerArray<CAlfSrvSubSessionBase> groups;
    CleanupClosePushL( groups );
    GetSubsessionsByTypeL( groups, EHuiObjectTypeControlGroup );
    for ( TInt g = 0 ; g < groups.Count() ; g++ )
        {
        CHuiControlGroup* group = groups[g]->AsHuiControlCroup();
        ASSERT( group ); 
        CHuiLayout* hostContainer = group->Control(0).ContainerLayout( NULL );
        
        hostContainer->SetSize( iClientDisplayRect.Size() );
        hostContainer->SetPos( iClientDisplayRect.iTl );
        }
    CleanupStack::PopAndDestroy( &groups ); // groups.Close()
    }
    
// ---------------------------------------------------------------------------
// From class CAlfAppSrvSessionBase.
// Called when client receives focus.
// ---------------------------------------------------------------------------
//   
void CAlfAppSrvSession::FocusGainedL( TBool aDoTransitionEffect )
    {
    iFocused = ETrue;
    DeliverTextureInfo();
    
    // update frame rate
    if ( RealCompare( iMaxFrameRate, KAlfUseDefaultFrameRate ) )
        {
        SharedHuiEnv()->SetMaxFrameRate( 
            TReal32(AlfAppUi()->SettingsHandler().DefaultFramerate()) );
        }
    else
        {
        SharedHuiEnv()->SetMaxFrameRate( iMaxFrameRate );
        }
    
    // update refresh mode
    if ( SharedHuiEnv()->RefreshMode() != iRefreshMode )
        {
        SharedHuiEnv()->SetRefreshMode( iRefreshMode );
        }

    // update idle threshold
    SharedHuiEnv()->SetIdleThreshold( iIdleThreshold );
    
    // update display attributes for each display
    CHuiDisplay* display = NULL;
    RPointerArray<CAlfSrvSubSessionBase> displays;
    CleanupClosePushL( displays );
    GetSubsessionsByTypeL( displays, EHuiObjectTypeDisplay );
    for ( TInt d = 0 ; d < displays.Count() ; d++ )
        {
        CAlfSrvDisplaySubSession* displaySubSession = 
                static_cast<CAlfSrvDisplaySubSession*>( displays[d] );     
        displaySubSession->SetSessionFocused();
        if ( !display && !displaySubSession->IsTvOut())
            {
            display = &displaySubSession->Display();
            }
        }     
    CleanupStack::PopAndDestroy( &displays ); // displays.Close()
    
    if ( !display )
        {
        return;
        }
    
    // Enable animated textures for the session
    for (TInt index = 0; index < iAnimatedTextures.Count(); index++)
        {
       	iAnimatedTextures.operator[](index)->EnableAnimation(ETrue);     
        }

	// This is needed for Huitk BitBlit() to succeed 
    
#ifdef SYMBIAN_BUILD_GCE
    SharedHuiEnv()->ContinueRefresh();
#else
    AlfAppUi()->Container()->DrawNow();
#endif
    }
    
// ---------------------------------------------------------------------------
// From class CAlfAppSrvSessionBase.
// Called when client loses focus.
// ---------------------------------------------------------------------------
//
TBool CAlfAppSrvSession::FocusLostL( TBool aDoTransitionEffect )
    {
    iFocused = EFalse;

    TBool didTransition = EFalse;
    
    if ( !SharedHuiEnv()->DisplayCount() )
        {
        return EFalse;
        }

    // Notify displays about losing focus so that tv out can be released
    CHuiDisplay* display = NULL;
    RPointerArray<CAlfSrvSubSessionBase> displays;
    CleanupClosePushL( displays );
    GetSubsessionsByTypeL( displays, EHuiObjectTypeDisplay );
    for ( TInt d = 0 ; d < displays.Count() ; d++ )
        {
        CAlfSrvDisplaySubSession* displaySubSession = 
                static_cast<CAlfSrvDisplaySubSession*>( displays[d] );     
        displaySubSession->SetSessionFocused(EFalse);
        if ( !display && !displaySubSession->IsTvOut())
            {
            display = &displaySubSession->Display();
            }
            
        }     
    CleanupStack::PopAndDestroy( &displays ); // displays.Close()

    if ( !display )
        {
        return didTransition;
        }

    if (display->DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer) 
    	{
    	// Disable all animated textures for the session
    	for (TInt index = 0; index < iAnimatedTextures.Count(); index++)
        	{
      		iAnimatedTextures.operator[](index)->EnableAnimation(EFalse); 
        	}
#ifndef SYMBIAN_BUILD_GCE       	
      	StoreControlGroupOrderL(*display, EFalse ); // do not hide  
#endif
      	// background must be drawn as long as the effect is displayed 
       	// as the background will be visible if the effect does not cover full screen
       	// or is partially transparent.
    	}
    
    return didTransition;
    }

void CAlfAppSrvSession::StoreControlGroupOrderL(CHuiDisplay& aDisplay, TBool aAlsoHide )
    {
    for ( TInt g = 0 ; g < aDisplay.Roster().Count() ; g++ )
        {
        // first append the bottom one
        CHuiControlGroup& group = aDisplay.Roster().ControlGroup( g );
        
        // make sure we only store control groups for this session
        if ( GetHandleFromInterface(EHuiObjectTypeControlGroup, &group) != KErrNotFound )
            {
            iControlGroupOrder.AppendL( &group );
            }
        }
    
    // at the end the lowest index the bottom most and the biggest index the top most.
    
    // hide at the end so it does not affect the order of the group in the roster
    if ( aAlsoHide )
        {
        for ( TInt i = 0 ; i < iControlGroupOrder.Count() ; i++ )
            {
            CHuiLayout* hostContainer = iControlGroupOrder[i]->Control(0).ContainerLayout( NULL );

            aDisplay.Roster().Hide( *iControlGroupOrder[i] );
            }
        }
    }
    
void CAlfAppSrvSession::ShowControlGroupsInOrderL(CHuiDisplay& /*aDisplay*/)
    {
/*#ifdef SYMBIAN_BUILD_GCE
    iControlGroupOrder.Reset();
    // gather all the control groups that belong to this session
    for ( TInt g = 0 ; g < aDisplay.Roster().Count() ; g++ )
        {
        // first append the bottom one
        CHuiControlGroup& group = aDisplay.Roster().ControlGroup( g );
        
        // make sure we only store control groups for this session
        if ( GetHandleFromInterface(EHuiObjectTypeControlGroup, &group) != KErrNotFound )
            {
            iControlGroupOrder.AppendL( &group );
            }
        }
#endif
    // put this session's controlgroups on top
    while ( iControlGroupOrder.Count() )
        {
        CHuiControlGroup& group = *iControlGroupOrder[iControlGroupOrder.Count()-1];
        
        // make sure that the group still exists.
        if ( GetHandleFromInterface(EHuiObjectTypeControlGroup, &group) != KErrNotFound )
            {
            CHuiLayout* hostContainer = group.Control(0).ContainerLayout( NULL );

#ifdef SYMBIAN_BUILD_GCE
            
            AlfAppUi()->ShowControlGroupL(aDisplay.Roster(), group, KHuiRosterShowAtTop, 0); 
            
#else    
            aDisplay.Roster().ShowL( group, KAlfRosterShowAtBottom );
#endif
            iControlGroupOrder.Remove(iControlGroupOrder.Count()-1);
            }
        }
    iControlGroupOrder.Reset();*/
    }
    
void CAlfAppSrvSession::ReOrderControlGroupSessionsL( RPointerArray<CAlfSrvSubSessionBase>& aGroupSessions )
    {
    if ( iControlGroupOrder.Count() && aGroupSessions.Count() > 1 )
        {
        // go through the items from bottom to top
        for ( TInt g = iControlGroupOrder.Count() -1 ; g >= 0 ; g-- ) 
            {
            CHuiControlGroup* nextGroupFromBottom = iControlGroupOrder[g];
            
            // Find the session and move it into first position
            for ( TInt s = 0 ; s < aGroupSessions.Count() ; s++ )
                {
                CAlfSrvControlGroupSubSession* controlGroupSubSession = static_cast<CAlfSrvControlGroupSubSession*>( aGroupSessions[s] );   
                if ( &controlGroupSubSession->ControlGroup() == nextGroupFromBottom )
                    {
                    // move into g:th position
                    aGroupSessions.Remove( s );
                    User::LeaveIfError( aGroupSessions.Insert( controlGroupSubSession, 0 ) );
                    }
                }
            }
        }
    }

void CAlfAppSrvSession::SetBackgroundMaxFps( TBool aBackground )
    {
    
    TReal32 newMaxFrameRate = 0;
    
    // update frame rate
    if ( RealCompare( iMaxFrameRate, KAlfUseDefaultFrameRate ) )
        {
        newMaxFrameRate = TReal32( AlfAppUi()->SettingsHandler().DefaultFramerate() );
        SharedHuiEnv()->SetMaxFrameRate( 
            TReal32(AlfAppUi()->SettingsHandler().DefaultFramerate()) );
        }
    else
        {
        newMaxFrameRate = iMaxFrameRate;
        }    
    
    if( aBackground )
        {  
        newMaxFrameRate /= 2;
        }
    SharedHuiEnv()->SetMaxFrameRate( newMaxFrameRate );
    }


// ---------------------------------------------------------------------------
// Is client application focused?
// ---------------------------------------------------------------------------
//    
TBool CAlfAppSrvSession::IsFocused() const
    {
    return iFocused;
    }
    
    
// ---------------------------------------------------------------------------
// From class MWindowVisibilityObserver.
// Called when window becomes fully visible.
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::WindowFullyVisible()
    {
    iPartiallyVisible = ETrue;
    DeliverTextureInfo();
    }
   
// ---------------------------------------------------------------------------
// From class MWindowVisibilityObserver.
// Called when window becomes partially visible.
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSession::WindowPartiallyVisible()
    {
    iPartiallyVisible = ETrue;
    DeliverTextureInfo();
    }
    
// ---------------------------------------------------------------------------
// From class MWindowVisibilityObserver.
// Called when window becomes fully invisible.
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::WindowNotVisible()
    {
    // already not shown...
    if ( !iPartiallyVisible )
        {
        return;
        }
    iPartiallyVisible = EFalse;
    }

// ---------------------------------------------------------------------------
// From class MWindowVisibilityObserver.
// Returns window-owning control.
// ---------------------------------------------------------------------------
//
CCoeControl* CAlfAppSrvSession::CoeControl()
    {
    return NULL;//iCoeDisplay;
    }

CAlfAppServer::TAlfWGPostion CAlfAppSrvSession::PreferredWindowGroupPosition() const
    {
    if ( iUsesFullScreen )
        {
        return CAlfAppServer::EOnTopOfParent;
        }
    else
        {
        return CAlfAppServer::EBehindOfParent;
        }
    }
    
THuiRefreshMode CAlfAppSrvSession::PreferredRefreshMode() const
    {
    return iRefreshMode;
    }


// ---------------------------------------------------------------------------
// From class CAlfAppSrvSessionBase.
// Called when a message is received from the client.
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::DoHandleCommandL(const RMessage2& aMessage)
    {
    switch( aMessage.Function() )
        {
        case EAlfQtCommandBuffer:
            {
            PostQtCommandBufferL( aMessage );
            break;
            }
        
		case EAlfEnvSetRefreshMode:
            {
            EnvSetRefreshModeL( aMessage );
            break;    
            }
            
        case EAlfEnvSetMaxFrameRate:
            {
            EnvSetMaxFrameRateL( aMessage );
            break;
            }
            
        case EAlfEnvContinueRefresh:
            {
            EnvContinueRefreshL( aMessage );
            break;
            }
            
        case EAlfEnvPauseRefresh:
            {
            EnvPauseRefreshL( aMessage );
            break;
            }
            
        case EAlfEnvRefreshCallBack:
            {
            EnvRefreshCallBackL( aMessage );
            break;
            }
            
        case EAlfEnvRenderer:
            {
            EnvRendererL( aMessage );
            break;
            }
        case EAlfEnvSetIdleThreshold:
            {
            EnvSetIdleThresholdL( aMessage );
            break;
            }
            
        case EAlfRosterShow:
            {
            
            // Parse parameters
            // 0: control group handle (in)
            const TInt cntrlGroupHandle = aMessage.Int0();
            CAlfSrvSubSessionBase& subSession1 = SubSession( cntrlGroupHandle );
            CAlfSrvControlGroupSubSession& controlGroupSubSession = 
                static_cast<CAlfSrvControlGroupSubSession&>(subSession1);
            CHuiControlGroup& controlGroup = controlGroupSubSession.ControlGroup();
                
            // 1: where (in)
            const TInt where = aMessage.Int1();
            // 2: display handle (in)
            const TInt displayHandle = aMessage.Int2();
            CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
            CAlfSrvDisplaySubSession& displaySubSession = 
                static_cast<CAlfSrvDisplaySubSession&>(subSession2);
            CHuiDisplay& display = displaySubSession.Display();

            controlGroupSubSession.SetIsShown( ETrue );
            controlGroupSubSession.PreferredPos() = where;
            
            // Show only if session is focused, otherwise delay until focus is gained
            if (iFocused)
                {
#ifdef SYMBIAN_BUILD_GCE
                CHuiLayout* hostContainer = controlGroup.Control(0).ContainerLayout( NULL );                
                if(hostContainer)
                    hostContainer->ClearFlags( EHuiVisualFlagUnderOpaqueHint);
                AlfAppUi()->ShowControlGroupL(display.Roster(), controlGroup, where, 0);
#else    
                display.Roster().ShowL(controlGroup, where);                                    
#endif
                AlfServer()->AdjustWindowGroupPositionL( 
                    *CHuiStatic::RootWin(), 
                    ClientWindowGroup(), 
                    PreferredWindowGroupPosition()  );
                }
            else
                {
#ifdef SYMBIAN_BUILD_GCE
            CHuiLayout* hostContainer = controlGroup.Control(0).ContainerLayout( NULL );                
                if(hostContainer)
                    hostContainer->ClearFlags(EHuiVisualFlagUnderOpaqueHint);
                AlfAppUi()->ShowControlGroupL(display.Roster(), controlGroup, where, 0);
#else                    

                if (where == KHuiRosterShowAtTop)
                    {
                    if (iControlGroupOrder.Count())
                        {
                        iControlGroupOrder.InsertL(&controlGroup, 0);            
                        }
                    else
                        {
                        iControlGroupOrder.AppendL(&controlGroup);                
                        }    
                    }
                else if (where == KHuiRosterShowAtBottom)
                    {
                
                    iControlGroupOrder.AppendL(&controlGroup);                
                    }
                else
                    {
                    if (iControlGroupOrder.Count() < where)
                        {
                        iControlGroupOrder.AppendL(&controlGroup);                        
                        }
                    else
                        {
                        iControlGroupOrder.InsertL(&controlGroup, where);                                        
                        }                                                
                    }
#endif                    
                }    
                        
            break;
            }
            
        case EAlfRosterHide:
            {
            RosterHideL(aMessage);
            break;
            }
            
        case EAlfRosterShowVisual:
            {
            RosterShowVisualL(aMessage);
            break;
            }
            
        case EAlfRosterHideVisual:
            {
            RosterHideVisualL(aMessage);
            break;
            }
        case EAlfRosterMoveVisualToFront:
            {
            RosterMoveVisualToFrontL(aMessage);
            break;
            }
        case EAlfRosterSetPointerEventFlags:
            {
            RosterSetPointerEventFlagsL(aMessage);
            break;
            }
        case EAlfRosterAddPointerEventObserver:
            {
            RosterAddPointerEventObserverL(aMessage);
            break;
            }
        case EAlfRosterRemovePointerEventObserver:
            {
            RosterRemovePointerEventObserverL(aMessage);
            break;
            }
        case EAlfRosterSetPointerDragTreshold:
            {
            RosterSetPointerDragTresholdL(aMessage);
            break;
            }
        case EAlfRosterDisableLongTapEventsWhenDragging:
            {
            RosterDisableLongTapEventWhenDraggingL(aMessage);
            break;
            }
        case EAlfTextureStopAnimation:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureStopAnimation(aMessage);
                }
            break;
            }
        case EAlfTextureStartAnimation:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureStartAnimation(aMessage);
                }
            break;
            }
        case EAlfTextureCreateAnimated:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureCreateAnimatedL(aMessage);
                }
            break;
            }
        case EAlfTextureCreate:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureCreateL(aMessage);
                }
            break;
            }
             
        case EAlfTextureUnload:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureUnload(aMessage);
                }
            break;
            }
        case EAlfTextureDelete:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureDelete(aMessage);
                }
            break;
            }
        case EAlfTextureRelease:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureRelease(aMessage);
                }
            break;
            }
        case EAlfTextureRestore:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureRestore(aMessage);
                }
            break;
            }
        case EAlfTextureNotifySkinChanged:
            {
        	if ( RequireTextureOwnerId( aMessage ) )
        	    {
        	    TextureNotifySkinChanged(aMessage);
        	    }
            break;
            }             
        case EAlfTextureLoad:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureLoadL(aMessage);
                }
            break;
            } 
        case EAlfTextureBlur:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureBlurL(aMessage);
                }
            break;
            } 
        case EAlfTextureHasContent:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureHasContentL(aMessage);
                }
            break;
            } 
        case EAlfTextureUpdateOwnerId:
            {
            UpdateTextureOwnerIdL( aMessage );    
            break;
            }
        case EAlfTextureSetAutoSizeParams:
            {
            if ( RequireTextureOwnerId( aMessage ) )
                {
                TextureSetAutoSizeParamsL( aMessage );    
                }
            break;
            }
            
            
        case EAlfNotifyAppVisibility:
            {
            TBool foreground = aMessage.Int0();
          	if (ClientWindowGroup() != KErrNotFound) // if Alf client have not set wg, treat it like non-Alf app 
          		{	
            	if ( foreground )
                	{         	
#ifdef SYMBIAN_BUILD_GCE
                	ActivateContainerLayoutL(ETrue);         	
#endif
                	AlfServer()->FocusedWindowGroupChangedL( this );
                	AlfServer()->AppUi()->UpdateActiveSession(this);
                
                	// Make sure that Env will refresh the screen 
                	// when we gain focus
                	AlfServer()->AppUi()->HuiEnv().ContinueRefresh();          			
                	}
            	else
                	{
                	// check if there is some other alf application on top of this one and set
                	// that application's session as a activesession
                	CAlfAppSrvSessionBase* newFocusSession = AlfServer()->UpMostClientAboveWg( ClientWindowGroup() );
 
            	    if( newFocusSession )
            	        {
                        AlfServer()->FocusedWindowGroupChangedL( newFocusSession );
            	        }
            	    else
            	        {
            	        // do the "event window" repositioning immediately
            	        AlfServer()->FocusedWindowGroupChangedL( NULL );
            	        }
            	    
            	    AlfServer()->AppUi()->UpdateActiveSession( newFocusSession );

                    // The following line removes the flickering (=drawing and empty frame)
                    // when non-alfred application is exited to appshell.
#ifndef SYMBIAN_BUILD_GCE
                	AlfServer()->AppUi()->Container()->DrawDeferred();
#endif
                	}    
          		}
            break;
            }
            
        case EAlfGetPointerEvent:
            {
            TriggerPointerEvent(&aMessage);
            return; // don't complete message here
            }
    
        case EAlfCancelPtrEvents:
            {
            CancelPointerEvents();
            break;
            }

        case EAlfGetSystemEvent:
            {
            GetSystemEvents(&aMessage);
            return; // don't complete message here
            }
    
        case EAlfCancelSystemEvents:
            {
            CancelSystemEvents();
            break;
            }
            
        case ESetFullScreenDrawing:
            {
            iUsesFullScreen = aMessage.Int0();
            break;
            }
        case EAlfLayoutMetricsTextStyleData:
            {
        	LayoutMetricsTextStyleDataL(aMessage);
            break;
            } 

        case EAlfSetWgParent:
            {
            SetParentWindowGroupId(aMessage.Int0());
            break;
            }
        case EAlfSBufAddObserver:
            {
            AlfServer()->ScreenBufferManager().AddScreenBufferObserver(this, aMessage);
            return; // message is completed in the method above
            } 
        case EAlfSBufRemoveObserver:
            {
            AlfServer()->ScreenBufferManager().RemoveScreenBufferObserver(this, aMessage);
            break;
            } 
        case EAlfSBufRequestNextBuffer:
            {
            AlfServer()->ScreenBufferManager().RequestNextBuffer(this, aMessage);
            return; // message is completed in the method above
            } 
        case EAlfSBufRequestBufferDraw:
            {
            AlfServer()->ScreenBufferManager().RequestBufferDraw(this, aMessage);
            return; // message is completed in the method above
            } 
        case EAlfSBufRequestEvent:
            {
            AlfServer()->ScreenBufferManager().RequestScreenBufferEvent(this, aMessage);
            return; // don't complete message here
            }          
            
        case EAlfDoSubSessionBatchCmd:
            {
            ExecuteBatchCommandsL(aMessage);
            break;
            }   
      
        case EAlfConfigureBatchCmd:
            {
            ConfigureBatchCommandExecutionL( aMessage );
            break;
            }

        case EAlfNotifyTextureInfo:
            {           
            NotifyTextureInfo( aMessage );            
            }
            return;
            
        case EAlfCancelNotifyTextureInfo:
            {
            if ( !iTextureInfoEvent.IsNull() )
                {
                iTextureInfoEvent.Complete( KErrCancel );
                }
            }
            break;
        case EAlfDirectClientFPSCounterOn:
            {
            ReportFrameRateBegin( aMessage );
            break;
            }
        case EAlfDirectClientFPSCounterOff:
            {
            ReportFrameRateEnd( aMessage );
            break;
            }
        case EAlfEnableLowMemoryState:
            {
            EnvEnableLowMemoryState( aMessage );
            break;
            }    
            
        case EAlfForceSwRendering:
            {
            EnvForceSwRendering( aMessage );
            break;    
            }
        
        case EAlfGetSizeAndRotation:
            {
            EnvGetSizeAndRotation( aMessage );
            break;
            }
            
        case EAlfReadPixels:
            {
            EnvReadPixels( aMessage );
            break;
            }
                        
        case EAlfBlankScreen:
            {
            AlfAppUi()->DoBlankScreen(aMessage);
            break;    
            }
            
            
        default:
            User::Leave( KErrNotSupported );
        }

    if ( !aMessage.IsNull() )
        {
        aMessage.Complete( KErrNone );
        }
    }
  
// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   SetRefreshMode
// param 0  :   IN refresh mode in THuiRefreshMode
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvSetRefreshModeL(const RMessage2& aMessage)
    {
    // 0: refresh mode (in)
    const THuiRefreshMode newMode = static_cast<THuiRefreshMode>(aMessage.Int0());
    
    if ( iRefreshMode != newMode )
        {
        iRefreshMode = newMode;
        
        if ( iFocused )
            {
            SharedHuiEnv()->SetRefreshMode( iRefreshMode );
            }
        }
    }
  
// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   SetMaxFrameRate
// param 0  :   IN framerate in Int
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvSetMaxFrameRateL(const RMessage2& aMessage)
    {
    // 0: max framerate in TPckgC (in)
    TReal32 newRate = KAlfUseDefaultFrameRate;
    TPckg<TReal32> frameRatePckg(newRate);
    aMessage.Read(0,frameRatePckg);
    
    if ( !RealCompare( iMaxFrameRate, newRate ) )
        {
        iMaxFrameRate = newRate;
        
        if ( iFocused )
            {
            if ( RealCompare( iMaxFrameRate, KAlfUseDefaultFrameRate ) )
                {
                SharedHuiEnv()->SetMaxFrameRate( 
                    TReal32(AlfAppUi()->SettingsHandler().DefaultFramerate() ) );
                }
            else
                {
                SharedHuiEnv()->SetMaxFrameRate( iMaxFrameRate );
                }
            }
        }
    }
  
// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   ContinueRefresh
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvContinueRefreshL(const RMessage2& /*aMessage*/)
    {
    SharedHuiEnv()->ContinueRefresh();
    }

// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   PauseRefresh
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvPauseRefreshL(const RMessage2& /*aMessage*/)
    {
    SharedHuiEnv()->PauseRefresh();
    }
   
// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   RefreshCallBack
// ---------------------------------------------------------------------------
//   
void CAlfAppSrvSession::EnvRefreshCallBackL(const RMessage2& /*aMessage*/)
    {
    CHuiEnv::RefreshCallBack( SharedHuiEnv() );
    }

// ---------------------------------------------------------------------------
// class    :   CHuiEnv
// function :   SetIdleThreshold
// param 0  :   IN Idle threshold time in milliseconds
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvSetIdleThresholdL(const RMessage2& aMessage)
    {
    // 0: time in milliseconds (in)
    const TInt time = (TInt)aMessage.Int0();
    
    if ( iIdleThreshold != time )
        {
        iIdleThreshold = time;
        
        if ( iFocused )
            {
            SharedHuiEnv()->SetIdleThreshold( iIdleThreshold );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// class    : CHuiEnv  
// function : Renderer  
// param 0  : IN/OUT renderer 
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::EnvRendererL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: renderer (in/out)
    TInt renderer = 0;
    TPckg<TInt> rendererPckg(renderer);
    aMessage.Read(0,rendererPckg);
    renderer = SharedHuiEnv()->Renderer();    
    aMessage.Write(0,rendererPckg);    
    }  
    
// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   Hide
// param 0  :   IN Handle to CHuiControlGroup object
// param 1  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::RosterHideL(const RMessage2& aMessage)
    {
    // Parse parameters
    // 0: control group handle (in)
    const TInt cntrlGroupHandle = aMessage.Int0();
    CAlfSrvSubSessionBase& subSession1 = SubSession( cntrlGroupHandle );
    CAlfSrvControlGroupSubSession& controlGroupSubSession = 
        static_cast<CAlfSrvControlGroupSubSession&>(subSession1);
    CHuiControlGroup& controlGroup = controlGroupSubSession.ControlGroup();
                
    // 1: display handle (in)
    const TInt displayHandle = aMessage.Int1();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();

    // hide
    controlGroupSubSession.SetIsShown( EFalse );
  
    
#ifdef SYMBIAN_BUILD_GCE
    CHuiLayout* hostContainer = controlGroup.Control(0).ContainerLayout( NULL );      
    controlGroup.SetAcceptInput(EFalse);
    if(hostContainer)
        hostContainer->SetFlags(EHuiVisualFlagUnderOpaqueHint);
#else    
    // hide from the roster only if session is focused
    if ( iFocused )
        {
        display.Roster().Hide(controlGroup);
        }
    else // otherwise remove from the iControlGroupOrder array
        {
         TInt index = iControlGroupOrder.Find( &controlGroup );
         if(  index != KErrNotFound)
             {
             iControlGroupOrder.Remove( index );
             }
        }
#endif
    }
    
// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   ShowVisualL
// param 0  :   IN Handle to CHuiVisual object
// param 1  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//     
void CAlfAppSrvSession::RosterShowVisualL(const RMessage2& aMessage)
    {
    // Parse parameters
    // 0: visual handle (in)
    TAny* huiVisualAnyPtr = GetInterfaceL( EHuiObjectTypeVisual, aMessage.Int0() );
    CHuiVisual* huiVisual = static_cast<CHuiVisual*>(huiVisualAnyPtr);
    
    // 1: display handle (in)
    const TInt displayHandle = aMessage.Int1();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    // show visual
    display.Roster().ShowVisualL(huiVisual);
    }
    
// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   HideVisual
// param 0  :   IN Handle to CHuiVisual object
// param 1  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
// 
void CAlfAppSrvSession::RosterHideVisualL(const RMessage2& aMessage)
    {
    // Parse parameters
    // 0: visual handle (in)
    TAny* huiVisualAnyPtr = GetInterfaceL( EHuiObjectTypeVisual, aMessage.Int0() );
    CHuiVisual* huiVisual = static_cast<CHuiVisual*>(huiVisualAnyPtr);
    
    // 1: display handle (in)
    const TInt displayHandle = aMessage.Int1();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    // show visual
    display.Roster().HideVisual(huiVisual);
    }
  
// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   MoveVisualToFront
// param 0  :   IN Handle to CHuiVisual object
// param 1  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//   
void CAlfAppSrvSession::RosterMoveVisualToFrontL(const RMessage2& aMessage)
    {
    // Parse parameters
    // 0: visual handle (in)
    TAny* huiVisualAnyPtr = GetInterfaceL( EHuiObjectTypeVisual, aMessage.Int0() );
    CHuiVisual* huiVisual = static_cast<CHuiVisual*>(huiVisualAnyPtr);
    
    // 1: display handle (in)
    const TInt displayHandle = aMessage.Int1();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    // move visual to front
#ifdef SYMBIAN_BUILD_GCE
    TInt firstAlfPositionInRoster = AlfAppUi()->FirstAlfControlGroupIndex();
    display.Roster().Move(huiVisual, firstAlfPositionInRoster); 

#else    
    display.Roster().MoveVisualToFront(huiVisual);
#endif
    }

// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   Observers().AppendIfNotFoundL, Observers.RemoveIfFound
// param 0  :   IN flags
// param 1  :   IN Handle to CHuiControl object
// param 2  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//      
void CAlfAppSrvSession::RosterSetPointerEventFlagsL(const RMessage2& aMessage)
    {
    // 0: flags (in)
    const TInt flags = aMessage.Int0();
    
    // 1: control handle (in)
    TAny* huiControlAnyPtr = GetInterfaceL( EHuiObjectTypeControl, aMessage.Int1() );
    CHuiControl* huiControl = static_cast<CHuiControl*>(huiControlAnyPtr);
    
    // 2: display handle (in)
    const TInt displayHandle = aMessage.Int2();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    if ( flags & EAlfPointerEventReportDrag )
        {
        display.Roster().Observers(EHuiInputPointerDrag).AppendIfNotFoundL(*huiControl);
        }
    else
        {
        display.Roster().Observers(EHuiInputPointerDrag).RemoveIfFound(*huiControl);
        }
        
    if ( flags & EAlfPointerEventReportLongTap )
        {
        display.Roster().Observers(EHuiInputPointerLongTap).AppendIfNotFoundL(*huiControl);
        }
    else
        {
        display.Roster().Observers(EHuiInputPointerLongTap).RemoveIfFound(*huiControl);
        }
        
    if ( flags & EAlfPointerEventReportUnhandled  )
        {
        display.Roster().Observers(EHuiInputPointerUnhandled).AppendIfNotFoundL(*huiControl);
        }
    else
        {
        display.Roster().Observers(EHuiInputPointerUnhandled).RemoveIfFound(*huiControl);
        }
    }
    
// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   Observers().AppendIfNotFoundL
// param 0  :   IN TAlfPointerEventFlags
// param 1  :   IN Handle to CHuiControl object
// param 2  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//      
void CAlfAppSrvSession::RosterAddPointerEventObserverL(const RMessage2& aMessage)
    {
    // 0: observer (in)
    const TAlfPointerEventFlags observer = (TAlfPointerEventFlags)aMessage.Int0();
    
    // 1: control handle (in)
    TAny* huiControlAnyPtr = GetInterfaceL( EHuiObjectTypeControl, aMessage.Int1() );
    CHuiControl* huiControl = static_cast<CHuiControl*>(huiControlAnyPtr);
    
    // 2: display handle (in)
    const TInt displayHandle = aMessage.Int2();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    // add observer
    THuiInputType huiType = EHuiInputPointerDrag;
    switch ( observer )
        {
        case EAlfPointerEventReportDrag:
            huiType = EHuiInputPointerDrag;
            break;
        case EAlfPointerEventReportLongTap:
            huiType = EHuiInputPointerLongTap;
            break;
        case EAlfPointerEventReportUnhandled:
            huiType = EHuiInputPointerUnhandled;
            break;
        default:
            User::Leave( KErrNotSupported );
        }
        
    display.Roster().Observers(huiType).AppendIfNotFoundL(*huiControl);
    }
    
 // ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   Observers.RemoveIfFound
// param 0  :   IN TAlfPointerEventFlags
// param 1  :   IN Handle to CHuiControl object
// param 2  :   IN Handle to CHuiDisplay object
// ---------------------------------------------------------------------------
//      
void CAlfAppSrvSession::RosterRemovePointerEventObserverL(const RMessage2& aMessage)
    {
    // 0: observer (in)
    const TAlfPointerEventFlags observer = (TAlfPointerEventFlags)aMessage.Int0();
    
    // 1: control handle (in)
    TAny* huiControlAnyPtr = GetInterfaceL( EHuiObjectTypeControl, aMessage.Int1() );
    CHuiControl* huiControl = static_cast<CHuiControl*>(huiControlAnyPtr);
    
    // 2: display handle (in)
    const TInt displayHandle = aMessage.Int2();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
    // remove observer
    THuiInputType huiType = EHuiInputPointerDrag;
    switch ( observer )
        {
        case EAlfPointerEventReportDrag:
            huiType = EHuiInputPointerDrag;
            break;
        case EAlfPointerEventReportLongTap:
            huiType = EHuiInputPointerLongTap;
            break;
        case EAlfPointerEventReportUnhandled:
            huiType = EHuiInputPointerUnhandled;
            break;
        default:
            User::Leave( KErrNotSupported );
        }
        
    display.Roster().Observers(huiType).RemoveIfFound(*huiControl);
    }

// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   
// param 0  :   
// ---------------------------------------------------------------------------
//      
void CAlfAppSrvSession::RosterSetPointerDragTresholdL(const RMessage2& aMessage)
    {
	// Parse parameters
    TAlfPointerEventDragTreshold params;
    TPckg<TAlfPointerEventDragTreshold> paramsPckg(params);
    aMessage.Read(0,paramsPckg);
    
    // control handle 
    TAny* huiControlAnyPtr = GetInterfaceL( EHuiObjectTypeControl, params.iControlHandle );
    CHuiControl* huiControl = static_cast<CHuiControl*>(huiControlAnyPtr);
    
	// display handle
    CAlfSrvSubSessionBase& subSession2 = SubSession( params.iDisplayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
	THuiXYMetric huiXYMetric;
    AlfXYMetricUtility::CopyMetric(params.iXYMetric, huiXYMetric);    
    
    display.Roster().SetPointerDragThresholdL(*huiControl, huiXYMetric );
    
    }

// ---------------------------------------------------------------------------
// class    :   CHuiRoster
// function :   
// param 0  :   
// ---------------------------------------------------------------------------
//      
void CAlfAppSrvSession::RosterDisableLongTapEventWhenDraggingL(const RMessage2& aMessage)
    {
	// Parse parameters
    // control handle (in)
    TAny* huiControlAnyPtr = GetInterfaceL( EHuiObjectTypeControl, aMessage.Int0() );
    CHuiControl* huiControl = static_cast<CHuiControl*>(huiControlAnyPtr);

    // observer (in)
    const TBool disable = (TBool)aMessage.Int1();
    
    // display handle (in)
    const TInt displayHandle = aMessage.Int2();
    CAlfSrvSubSessionBase& subSession2 = SubSession( displayHandle );
    CAlfSrvDisplaySubSession& displaySubSession = 
        static_cast<CAlfSrvDisplaySubSession&>(subSession2);
    CHuiDisplay& display = displaySubSession.Display();
    
	display.Roster().DisableLongTapEventsWhenDraggingL(*huiControl, disable);
    }

// ---------------------------------------------------------------------------
// class    :   
// function :   
// param 0  :   
// param 1  :   
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureCreateL(const RMessage2& aMessage)
    {
    // Owner id is stored in UpdateTextureOwnerIdL via IPC command 
    // EAlfTextureUpdateOwnerId.
    
    // Parse parameters

    // 0: handle (in/out)
    TInt handle;
    TPckg<TInt> handlePckg(handle);
    aMessage.Read(0,handlePckg);

    // 1: TAlfTextureCreateParams (in)
    TAlfCreateTextureParams params;
    TPckg<TAlfCreateTextureParams> paramsPckg(params);
    aMessage.Read(1,paramsPckg);


    CHuiEnv* env = SharedHuiEnv();
    if (params.iBitmapHandle && params.iId != 0) 
        {            
        const TInt bitmapHandle = params.iBitmapHandle;
        const TInt maskHandle = params.iMaskBitmapHandle;
    	const TInt flags = params.iFlags;
        const TInt managerId = params.iManagerId;
        const TInt id = 
            AlfTextureManager().CreateTextureId(
                TextureOwnerId(),
                params.iId, 
                managerId, 
                ETrue );

    	CSharedBitmapProvider* provider = new (ELeave) CSharedBitmapProvider(bitmapHandle, maskHandle);
        CleanupStack::PushL(provider);
    	
        // Convert the TAlfTextureFlags to THuiTextureUploadFlags
        TInt inputFlags = flags;
        inputFlags &= ~EAlfTextureFlagSkinContent;
        inputFlags &= ~EAlfTextureFlagAutoSize;
        inputFlags &= ~EAlfTextureFlagLoadAnimAsImage;
        
        TBool textureAlreadyExists = (env->TextureManager().Texture(id) != &env->TextureManager().BlankTexture());
        
        CHuiTexture& texture = env->TextureManager().CreateTextureL(id,
                                     provider,
                                     (THuiTextureUploadFlags)inputFlags);

        // If we are reusing deleted texture, remove it from "deleted" array
        TInt index = iTextures.Find(&texture);
        if (index == KErrNotFound)
           {
           iTextures.Append(env->TextureManager().Texture(id));                
           }                    


        // Add texture to skin content
        texture.SetSkinContent((flags & EAlfTextureFlagSkinContent) != 0);

        // Add texture to automatic dynamic size calculations
        texture.EnableAutoSizeCalculation((flags & EAlfTextureFlagAutoSize) != 0);
        
        // Check if texture already existed, if yes then we need to update its content
        // because 
        if (textureAlreadyExists)
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            provider->ProvideBitmapL(0, bitmap, mask);
            CleanupStack::PushL(bitmap);
            CleanupStack::PushL(mask);
            texture.EnableShadow((flags & EHuiTextureUploadFlagGenerateShadow) != 0);            
            TSize bitmapSize = bitmap->SizeInPixels();
            if (bitmapSize.iWidth != 0 && bitmapSize.iHeight != 0)
                {
                // This uses direct upload if possible and specified in the upload flags
                TRAPD(err, env->TextureManager().UpdateTextureFromBitmapL(id, provider))        
                
                if (err != KErrNone)
                    {
                    // We do this because texture may be in undetermined state (?)
                    texture.Reset();    
                    User::Leave(err);
                    }
                }
            texture.SetSize(bitmap->SizeInPixels());
            CleanupStack::PopAndDestroy(2); // bitmap, mask
            }  
                                     
        // We delete provider here, because the release/restore
        // is supported in the client library. There is currently
        // no need to restore textures in the server.
        CleanupStack::PopAndDestroy(); // provider;

        // Write parameters
        handle = TInt((MHuiTexture*)(&texture));
        aMessage.Write(0,handlePckg);
        }
    else
        {
        CHuiTexture& texture = env->TextureManager().BlankTexture();    
        // Write parameters
        handle = TInt((MHuiTexture*)(&texture));
        aMessage.Write(0,handlePckg);
        }    
    }

void CAlfAppSrvSession::TextureCreateAnimatedL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: handle (in/out)
    TInt handle = 0;
    TPckg<TInt> handlePckg(handle);
    aMessage.Read(0,handlePckg);

    // 1: TAlfTextureCreateParams (in)
    TAlfCreateTextureAnimatedParams params;
    TPckg<TAlfCreateTextureAnimatedParams> paramsPckg(params);
    aMessage.Read(1,paramsPckg);
    CHuiEnv* env = SharedHuiEnv();
    
    CHuiGifAnimationTexture* animtext = CHuiGifAnimationTexture::NewL(params.iFilename,env->TextureManager(),params.iId, (THuiTextureUploadFlags)(params.iFlags));
    //MHuiTexture* foo = dynamic_cast<MHuiTexture*>(animtext);
    iAnimatedTextures.Append(animtext);
    handle = TInt((MHuiTexture*)(animtext));
    aMessage.Write(0,handlePckg);
    }




// ---------------------------------------------------------------------------
// class    :   
// function :   
// param 0  :   
// param 1  :   
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureLoadL(const RMessage2& aMessage)
    {
    // Owner id is stored in UpdateTextureOwnerIdL via IPC command 
    // EAlfTextureUpdateOwnerId.

    // 0: handle (in/out)
    TInt handle = 0;
    TPckg<TInt> handlePckg(handle);
    aMessage.Read(0,handlePckg);

    // 1: TAlfTextureLoadParams (in)
    TAlfLoadTextureParams params;
    TPckg<TAlfLoadTextureParams> paramsPckg(params);
    aMessage.Read(1,paramsPckg);

    const TInt bitmapHandle = params.iBitmapHandle;
    const TInt maskHandle = params.iMaskBitmapHandle;
	const TInt flags = params.iFlags;
    const TInt managerId = params.iManagerId;
    const TInt id = 
        AlfTextureManager().CreateTextureId(
            TextureOwnerId(),
            params.iId, 
            managerId, 
            ETrue );

    CHuiEnv* env = SharedHuiEnv();
    if (bitmapHandle && params.iId != 0)
        {            
    	CSharedBitmapProvider* provider = 
    	    new (ELeave) CSharedBitmapProvider(bitmapHandle, maskHandle);
        CleanupStack::PushL(provider);
        
        // Convert the TAlfTextureFlags to THuiTextureUploadFlags
        TInt inputFlags = (THuiTextureUploadFlags)flags;
        inputFlags &= ~EAlfTextureFlagSkinContent;
        inputFlags &= ~EAlfTextureFlagAutoSize;
        inputFlags &= ~EAlfTextureFlagLoadAnimAsImage;
        
        TBool textureAlreadyExists = (env->TextureManager().Texture(id) != &env->TextureManager().BlankTexture());

        // Make sure texture exists
        CHuiTexture& textureRef = env->TextureManager().CreateTextureL(id, provider, THuiTextureUploadFlags(inputFlags));

        // If we are reusing deleted texture, remove it from "deleted" array
        TInt index = iTextures.Find(&textureRef);
        if (index == KErrNotFound)
           {
           iTextures.Append(env->TextureManager().Texture(id));                
           }                    
        
        // Add texture to skin content
        textureRef.SetSkinContent((flags & EAlfTextureFlagSkinContent) != 0);

        // Add texture to automatic dynamic size calculations
        textureRef.EnableAutoSizeCalculation((flags & EAlfTextureFlagAutoSize) != 0);
    	    	
        // Upload texture, this is needed because CreateTexture does not necessarely do anything if texture
        // already exists.
        if (textureAlreadyExists)
            {                
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            provider->ProvideBitmapL(id, bitmap, mask);
            CleanupStack::PushL(bitmap);
            CleanupStack::PushL(mask);
            textureRef.EnableShadow((flags & EHuiTextureUploadFlagGenerateShadow) != 0);            
            TSize bitmapSize = bitmap->SizeInPixels();
            if (bitmapSize.iWidth != 0 && bitmapSize.iHeight != 0)
                {
                // This uses direct upload if possible and specified in the upload flags
                TRAPD(err, env->TextureManager().UpdateTextureFromBitmapL(id, provider))        

                if (err != KErrNone)
                    {
                    // We do this because texture may be in undetermined state (?)
                    textureRef.Reset();    
                    User::Leave(err);
                    }
                }
            textureRef.SetSize(bitmap->SizeInPixels());
            CleanupStack::PopAndDestroy(2); // bitmap, mask
            }
        // We delete provider here, because the release/restore
        // is supported in the client library. There is currently
        // no need to restore textures in the server.
        CleanupStack::PopAndDestroy( provider );


        // Write parameters
        handle = TInt((MHuiTexture*)(&textureRef));
        aMessage.Write(0,handlePckg);        
        }
    else
        {
        CHuiTexture& texture = env->TextureManager().BlankTexture();    
        // Write parameters
        handle = TInt((MHuiTexture*)(&texture));
        aMessage.Write(0,handlePckg);
        }    
        
    }

// ---------------------------------------------------------------------------
// class    : CHuiTextureManager  
// function : UnloadTexture  
// param 0  : IN Texture id  
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureUnload(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	const TInt managerId = aMessage.Int1();

	if (originalId != 0)
	    {
        const TInt id = 
            AlfTextureManager().ExistingTextureId(
                TextureOwnerId(), originalId, managerId);
        
    	if ( id != 0 && 
    	     AlfTextureManager().ReleaseTextureId( 
    	        TextureOwnerId(), originalId, managerId ) == 0)
    	    {
            if ( !managerId )
                {
                RemoveTextureInfo( originalId );
                }

            CHuiEnv* env = SharedHuiEnv();    	
           	env->TextureManager().UnloadTexture(id);                                         	            	        
    	    }
	    }
    }

// ---------------------------------------------------------------------------
// class    : 
// function : 
// param 0  : IN Texture id  
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureDelete(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	const TInt managerId = aMessage.Int1();

	if (originalId != 0)
	    {
        const TInt id = 
            AlfTextureManager().ExistingTextureId( 
                TextureOwnerId(), originalId, managerId );
        
    	if ( id != 0 && 
    	     AlfTextureManager().ReleaseTextureId( 
    	        TextureOwnerId(), originalId, managerId ) == 0)
    	    {
            CHuiEnv* env = SharedHuiEnv();
           	env->TextureManager().UnloadTexture(id);
           	
            if ( !managerId )
                {
                RemoveTextureInfo( originalId );
                }
           	
            if ( AlfTextureManager().DeleteTextureId(
                     TextureOwnerId(), originalId, managerId) == 0 )
                {
        	    // For now just unload textures, actual deletion happens 
        	    // when session is closed (for safety reasons).
                CHuiTexture* texture = env->TextureManager().Texture(id); 
                if (texture)
                    {
                    // We should set texture attributes to default values
                    // here if we recycle hui-textures during this session.
                    texture->SetAutoSizeParams(THuiTextureAutoSizeParams());
                    texture->EnableShadow(EFalse);
                    texture->SetSkinContent(EFalse);
                    
                    if (iTextures.Find(texture) == KErrNotFound)
                        {
                        iTextures.Append(texture);                
                        }                    
                    }                                
                }
    	    }
	    }
    }

// ---------------------------------------------------------------------------
// class    : CHuiTextureManager  
// function :  
// param 0  : IN Texture id  
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureRelease(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	const TInt managerId = aMessage.Int1();

	if (originalId != 0)
	    {
	    // Just release texture id
        AlfTextureManager().ReleaseTextureId(
            TextureOwnerId(), originalId, managerId );	    
	    }
    }

// ---------------------------------------------------------------------------
// class    : CHuiTextureManager  
// function :  
// param 0  : IN Texture id  
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureRestore(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	const TInt managerId = aMessage.Int1();

	if (originalId != 0)
	    {
	    // Just restore texture id
        AlfTextureManager().CreateTextureId(
            TextureOwnerId(), originalId, managerId, EFalse );
	    }
    }

// ---------------------------------------------------------------------------
// class    : CHuiTextureManager  
// function :  
// param 0  : IN Texture id  
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureNotifySkinChanged(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	const TInt managerId = aMessage.Int1();

	if (originalId != 0)
	    {
        const TInt id = 
            AlfTextureManager().ExistingTextureId( 
                TextureOwnerId(), originalId, managerId );
        
    	if ( id != 0 && 
    	     AlfTextureManager().ValidateSkinForTextureId( 
    	        TextureOwnerId(), originalId, managerId ) == 0)
    	    {
            CHuiEnv* env = SharedHuiEnv();    	
           	env->TextureManager().UnloadTexture(id);                                         	            	        
    	    }
	    }
    }

static CHuiTexture* CastTexture( MHuiTexture* aMHuiTexture )
    {
    CHuiTexture* resultCHuiTexture = NULL;
    if ( aMHuiTexture )
        {
        // only CHuiTexture implements the MHuiShadowedTexture interface
        MHuiShadowedTexture* shadowedTexture = aMHuiTexture->ShadowedTexture();
        if ( shadowedTexture )
            {
            resultCHuiTexture = static_cast<CHuiTexture*>( shadowedTexture );
            }
        }
            
    return resultCHuiTexture;
    }

// ---------------------------------------------------------------------------
// class    :   
// function :   
// param 0  :   
// param 1  :   
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureBlurL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 0: TAlfBlurTextureParams (in)
    TAlfBlurTextureParams params;
    TPckg<TAlfBlurTextureParams> paramsPckg(params);
    aMessage.Read(0,paramsPckg);

    CHuiEnv* env = SharedHuiEnv();
    if (params.iServerSideSrcHandle && params.iServerSideDstHandle)
        {
        CHuiTexture* srcTexture = CastTexture(reinterpret_cast<MHuiTexture*>(params.iServerSideSrcHandle));
        CHuiTexture* dstTexture = CastTexture(reinterpret_cast<MHuiTexture*>(params.iServerSideDstHandle));
        
        if (srcTexture && dstTexture)
            {
            
            const THuiTextureHandle srcHandle = srcTexture->Handle();
            THuiTextureHandle dstHandle = dstTexture->Handle();
            const TSize size = params.iPreferredSize;
                
            env->TextureManager().Processor().BlurL(srcHandle,
                dstHandle, 
                size,
                params.iFilterSize,
                params.iFlag);            
            }
        else
            {
            User::Leave(KErrArgument);
            }
        }
    }

// ---------------------------------------------------------------------------
// class    :   
// function :   
// param 0  :   
// param 1  :   
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureHasContentL(const RMessage2& aMessage)
    {

    // 0: ret val (in/out)
    TBool retVal = EFalse;
    TPckg<TBool> retValPckg(retVal);
    aMessage.Read(0,retValPckg);

    // 1: id (in)
   	const TInt originalId = aMessage.Int1();
   	
    // 2: manager id (in)
   	const TInt managerId = aMessage.Int2();

	if (originalId != 0)
	    {
        const TInt id = 
            AlfTextureManager().ExistingTextureId( 
                TextureOwnerId(), originalId, managerId, ETrue);
        
        if (id != 0)
            {
            CHuiEnv* env = SharedHuiEnv();    	
            CHuiTexture* tex = env->TextureManager().Texture(id);                                         	            	        
            if (tex)
                {
                retVal = tex->HasContent();               
                }                
            }
	    }
    aMessage.Write(0,retValPckg);        
    }

void CAlfAppSrvSession::TextureStartAnimation(const RMessage2& aMessage)
    {
    TInt id = aMessage.Int0();
    CHuiGifAnimationTexture* tex = NULL;
    for (TInt index = 0; index < iAnimatedTextures.Count(); index++)
        {
        tex = iAnimatedTextures.operator[](index);
        if (tex->Id() == id)
            {
            tex->Start();
            break;
            }
        }
    }
    
void CAlfAppSrvSession::TextureStopAnimation(const RMessage2& aMessage)
    {
    TInt id = aMessage.Int0();
    CHuiGifAnimationTexture* tex = NULL;
    for (TInt index = 0; index < iAnimatedTextures.Count(); index++)
        {
        tex = iAnimatedTextures.operator[](index);
        if (tex->Id() == id)
            {
            tex->Stop();
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// class    :   
// function :   
// param 0  :   
// param 1  :   
// ---------------------------------------------------------------------------
//  
void CAlfAppSrvSession::TextureSetAutoSizeParamsL(const RMessage2& aMessage)
    {

    // 0: id (in)
   	const TInt originalId = aMessage.Int0();
   	
    // 1: manager id (in)
   	const TInt managerId = aMessage.Int1();

    // 2: params (in)
    TAlfTextureAutoSizeParams params;
    TPckg<TAlfTextureAutoSizeParams> paramPckg(params);
    aMessage.Read(2,paramPckg);


	if (originalId != 0)
	    {
        const TInt id = 
            AlfTextureManager().ExistingTextureId( 
                TextureOwnerId(), originalId, managerId, ETrue);
        
        if (id != 0)
            {
            CHuiEnv* env = SharedHuiEnv();    	
            CHuiTexture* tex = env->TextureManager().Texture(id);                                         	            	        
            if (tex)
                {
                THuiTextureAutoSizeParams* huiparams = (THuiTextureAutoSizeParams*) &params;                
                tex->SetAutoSizeParams(*huiparams);               
                }                
            }
	    }
    }

void CAlfAppSrvSession::LayoutMetricsTextStyleDataL(const RMessage2& aMessage)
    {
    // 0: ret val (in/out)
    TInt retVal = 0;
    TPckg<TInt> retValPckg(retVal);
    aMessage.Read(0,retValPckg);

    // 1: textStyle (in)
   	const TInt textStyle = aMessage.Int1();
   	
    CHuiEnv* env = SharedHuiEnv();
    retVal = env->TextStyleManager().TextStyle(textStyle)->FontStyleId();    	   	
    
    aMessage.Write(0,retValPckg);                
    }
    
TBool CAlfAppSrvSession::RequireTextureOwnerId(const RMessage2& aMessage)
    {
    if ( !iTextureOwnedIdSet )
        {
        AlfPanicClient( aMessage, EAlfSrvTexturePanicTextureOwnerIdNotUpdated );        
        }
        
    return iTextureOwnedIdSet;
    }
    
void CAlfAppSrvSession::UpdateTextureOwnerIdL(const RMessage2& aMessage)
    {
    if ( !iTextureOwnedIdSet )
        {
        // Open process, get id and then close.
        RThread client;
        User::LeaveIfError( aMessage.Client(client) );
        CleanupClosePushL( client );
        RProcess process;
        User::LeaveIfError( client.Process( process ) );
        TProcessId id = process.Id();
        process.Close();
        CleanupStack::PopAndDestroy( &client );
        
        // Register & then store owner id.
        AlfTextureManager().AddClientL( id, *this );
        iTextureOwnedIdSet = ETrue;
        iTextureOwnerId = id;
        }        
    }
    
inline TProcessId CAlfAppSrvSession::TextureOwnerId() const
    {
    __ASSERT_ALWAYS( iTextureOwnedIdSet, USER_INVARIANT() );
    return iTextureOwnerId;
    }

void CAlfAppSrvSession::ConfigureBatchCommandExecutionL( 
        const RMessage2& aMessage )
    {
    // This should be called only by unit tests. Anyway,
    // this has impact on this session only, so it does
    // not cause any problems.
    __ALFLOGSTRING1( "ConfigureBatchCommandExecutionL: %x", aMessage.Int0() )
    iExecutionFlags = aMessage.Int0();
    }

void CAlfAppSrvSession::ExecuteBatchCommandsL(const RMessage2& aMessage)
    {
    // First, let's try to execute using dynamic buffer..
    if ( !AllowExecuteUsingDynamicBuffer() || 
         !ExecuteBatchCommandsUsingDynamicBufferL( aMessage ) )
        {
        // If that fails, then try with fixed buffer.
        ExecuteBatchCommandsUsingFixedBufferL( aMessage );
        }
    }

TBool CAlfAppSrvSession::ExecuteBatchCommandsUsingDynamicBufferL( 
        const RMessage2& aMessage )
    {       
    TBool result = EFalse;

    CAlfAppServer* server = AlfServer();

    const TInt commonBatchBufferLength = 
        server->CommonCommandBatchBufferMaxLength();

    const TInt requiredBufferLength = aMessage.GetDesLengthL(0);

    TBool commonBatchBufferAcquired = EFalse;

    // This will finally hold reference to buffer to be used for
    // reading message from client side.
    TPtr8 ptrBuffer( 0, 0 );

    // First, try to acquire common command batch buffer
    // (if it's not in use and it has sufficient space)
    if ( AllowExecuteUsingCommonBuffer() &&
         ( commonBatchBufferLength >= requiredBufferLength ) )
        {
        commonBatchBufferAcquired = 
            server->AcquireCommonCommandBatchBuffer( ptrBuffer );
        }

    // If common command batch buffer were not usable,
    // let's try to allocate bigger buffer.
    HBufC8* memoryBuffer = NULL;
    if ( commonBatchBufferAcquired )
        {
        result = ETrue;
        }
    else
        {
        memoryBuffer = HBufC8::New( requiredBufferLength );
        if ( memoryBuffer )
            {
            result = ETrue;
            ptrBuffer.Set( memoryBuffer->Des() );
            }
        }

    if ( result )
        {
        TInt err = aMessage.Read( 0, ptrBuffer );

        if ( err != KErrNone )
            {
            // Release and then leave.
            if ( memoryBuffer )
                {
                delete memoryBuffer;
                memoryBuffer = NULL;
                }
            if ( commonBatchBufferAcquired )
                {
                server->ReleaseCommonCommandBatchBuffer();
                }
            
            User::Leave( err );
            }

        TInt dummy;
        TInt length = 0;

        // Note - this method is implemented so that this works even in OOM, that's
        // why TRAP is used here intentionally.
        TRAP( err, length = ExecuteCommandsFromDescriptorL( ptrBuffer, dummy ) );
        const TInt ptrBufferLength = ptrBuffer.Length();

        // Release - note that buffer length must be fetched before deletion.
        if ( memoryBuffer )
            {
            delete memoryBuffer;
            memoryBuffer = NULL;
            }
        if ( commonBatchBufferAcquired )
            {
            server->ReleaseCommonCommandBatchBuffer();
            }

#ifdef _ALF_LOGGING
        if ( err != KErrNone )
            {
            __ALFLOGSTRING3( 
                "ExecuteBatchCommandsUsingDynamicBufferL execution failed, err=%d (%d,%d)", 
                err, length, ptrBufferLength )
            }
#endif // _ALF_LOGGING

        if ( !err && ( length != ptrBufferLength ) )
            {
            // All commands were not executed!
            __ALFLOGSTRING2( 
                "ExecuteBatchCommandsUsingDynamicBufferL corrupted buffer (%d,%d)", 
                length, ptrBufferLength )
            err = KErrCorrupt;
            }

        User::LeaveIfError( err );
        }

    return result;
    }

void CAlfAppSrvSession::ExecuteBatchCommandsUsingFixedBufferL( 
        const RMessage2& aMessage )
    {
    CAlfAppServer* server = AlfServer();
    const TInt messageLength = aMessage.GetDesLengthL(0);

    TBufC8< KAlfStackBatchBufferSize > stackBuffer;
    TPtr8 ptrBuffer( stackBuffer.Des() );

    TBool commonBatchBufferAcquired = EFalse;
    if ( AllowExecuteUsingCommonBuffer() )
        {
        commonBatchBufferAcquired = 
            server->AcquireCommonCommandBatchBuffer( ptrBuffer );
        }
    
    TInt length = 0;

    // Note - this method is implemented so that this works even in OOM, that's
    // why TRAP is used here intentionally.
    TRAPD( err, length = 
        ExecuteBatchCommandsUsingBufferL( ptrBuffer, messageLength, aMessage ) );

    if ( commonBatchBufferAcquired )
        {
        server->ReleaseCommonCommandBatchBuffer();
        }

#ifdef _ALF_LOGGING
    if ( err != KErrNone )
        {
        __ALFLOGSTRING3( 
            "ExecuteBatchCommandsUsingFixedBufferL execution failed, err=%d (%d,%d)", 
            err, length, messageLength )
        }
#endif // _ALF_LOGGING

    if ( !err && ( length != messageLength ) )
        {
        // All commands were not executed!
        __ALFLOGSTRING2( 
            "ExecuteBatchCommandsUsingFixedBufferL corrupted buffer (%d,%d)", 
            length, messageLength )

        err = KErrCorrupt;
        }

    User::LeaveIfError( err );
    }

TInt CAlfAppSrvSession::ExecuteBatchCommandsUsingBufferL( 
        TDes8& aBuffer, 
        TInt aMessageLength,
        const RMessage2& aMessage )
    {
    const TInt bufferMaxLength = aBuffer.MaxLength();
    
    // It is assumed that bufferMaxLength > command header length.
    // If you check ExecuteBatchCommandsUsingFixedBufferL, you can see
    // that this assumption holds.

    TInt offset = 0;
    TBool failed = EFalse;

    while ( !failed && ( offset < aMessageLength ) ) 
        {
        aMessage.ReadL( 0, aBuffer, offset );

        TInt spaceNeeded = 0;
        offset += ExecuteCommandsFromDescriptorL( aBuffer, spaceNeeded );

        if ( spaceNeeded > ( aMessageLength - offset ) )
            {
            // More space is needed than there is left in message.
            failed = ETrue;
            }
        else if ( spaceNeeded > bufferMaxLength )
            {
            // Too bad, message does not fit to our fixed size buffer.
            // Let's try to allocate buffer for this.

            HBufC8* buffer = HBufC8::NewLC( spaceNeeded );
            TPtr8 bufferPtr = buffer->Des();

            aMessage.ReadL( 0, bufferPtr, offset );

            // Maximum length of buffer may be larger than spaceNeeded,
            // thus bufferPtr may be larger as well. But to keep this
            // simple, let's constraint to one message.
            if ( bufferPtr.Length() > spaceNeeded )
                {
                bufferPtr.SetLength( spaceNeeded );
                }
                
            if ( bufferPtr.Length() == spaceNeeded )
                {
                TInt dummy;
                TInt offsetDelta = 
                    ExecuteCommandsFromDescriptorL( bufferPtr, dummy );

                if ( offsetDelta == spaceNeeded )
                    {
                    // Move offset forward.
                    offset += offsetDelta;    
                    }
                else
                    {
                    // There was supposed to be exactly one message
                    // in buffer, but now there is not.
                    failed = ETrue;
                    }
                }
            else
                {
                // spaceNeeded amount of bytes were required for 
                // next message, but somehow we did not get that many bytes.
                failed = ETrue;
                }

            CleanupStack::PopAndDestroy( buffer );
            }
        else
            {
            // Do nothing - we can continue reading next part of buffer.
            }
        }

    return offset;
    }

TInt CAlfAppSrvSession::ExecuteCommandsFromDescriptorL( 
        const TDesC8& aSource, 
        TInt& aSpaceNeeded )
    {
    TPtrC8 unreadArea( aSource );
    aSpaceNeeded = 0;

    while ( !aSpaceNeeded && ( unreadArea.Length() > 0 ) )
        {
        TInt3 params(0,0,0);
        TPckg<TInt3> paramsPckg( params );

        // Check if there is complete parameter structure in unreadArea.
        const TInt paddedParamsPckgMaxLength = 
            AlfPaddedLength( paramsPckg.MaxLength() );
        if ( paddedParamsPckgMaxLength > unreadArea.Length() )
            {
            // Bail out from while loop.
            aSpaceNeeded = paddedParamsPckgMaxLength;
            }
        else
            {
            // Copy to local parameters.
            paramsPckg.Copy( unreadArea.Left( paramsPckg.MaxLength() ) );

            // Read parameters
            const TInt operation = params.iInt1;
            const TInt subSessionHandle = params.iInt2;
            const TInt inputBufferLength = params.iInt3;
            
            if ( inputBufferLength < 0 )
                {
                // Input buffer length must not be negative.
                __ALFLOGSTRING1( 
                    "ExecuteCommandsFromDescriptorL corrupted: neg. length %d",
                    inputBufferLength )
                User::Leave( KErrCorrupt );
                }
                
            const TInt paddedInputBufferLength = 
                AlfPaddedLength( inputBufferLength );
            
            if ( paddedParamsPckgMaxLength + paddedInputBufferLength < 0 )
                {
                // There must NOT be overflow.
                __ALFLOGSTRING1( 
                    "ExecuteCommandsFromDescriptorL corrupted: length %d",
                    inputBufferLength )
                User::Leave( KErrCorrupt );
                }

            // Check if there is complete input buffer in unreadArea.
            if ( paddedParamsPckgMaxLength + paddedInputBufferLength > 
                 unreadArea.Length() )
                {
                // Bail out from while loop.
                aSpaceNeeded = paddedParamsPckgMaxLength + paddedInputBufferLength;
                }
            else
                {                    
                // Now we know that all data is available. 
                // It's time to execute command.

                // Skip over parameters (already read above)
                unreadArea.Set( unreadArea.Mid( paddedParamsPckgMaxLength ) );
               
                // Read input buffer
                TPtrC8 inputbuffer = unreadArea.Left( inputBufferLength );
                unreadArea.Set( unreadArea.Mid( paddedInputBufferLength ) );
        
                if ( !CAlfAppSrvSessionBase::HasSession( subSessionHandle ) )
                    {
                    // Subsession handle must exist.
                    __ALFLOGSTRING1( 
                        "ExecuteCommandsFromDescriptorL corrupted: handle %d",
                        subSessionHandle )
                    User::Leave( KErrCorrupt );
                    }
        
                // Execute command
                CAlfSrvSubSessionBase& subsession = 
                    CAlfAppSrvSessionBase::SubSession( subSessionHandle );
                MAlfExtension* subsessionEx = subsession.AsCommandHandler();

                if ( !subsessionEx )
                    {
                    // Instance implementing extension interface must exist.
                    __ALFLOGSTRING1( 
                        "ExecuteCommandsFromDescriptorL corrupted: no ext, handle %d",
                        subSessionHandle )
                    User::Leave( KErrCorrupt );
                    }

                TBuf8<1> dummy;
#ifdef _ALF_LOGGING
                TRAPD( err,
                    subsessionEx->HandleCmdL( operation, inputbuffer, dummy ) );
                if ( err != KErrNone )
                    {
                    __ALFLOGSTRING3( 
                        "ExecuteCommandsFromDescriptorL HandleCmdL fail %d (subsession %d, operation %d)", 
                        err, subSessionHandle, operation )
                    }
                User::LeaveIfError( err );
#else
                subsessionEx->HandleCmdL( operation, inputbuffer, dummy );
#endif // _ALF_LOGGING
                }
            }
        }
    
    return aSource.Length() - unreadArea.Length();
    }

inline TBool CAlfAppSrvSession::AllowExecuteUsingDynamicBuffer() const
    {
    return ( iExecutionFlags & EAlfExecuteUsingDynamicBuffer );
    }

inline TBool CAlfAppSrvSession::AllowExecuteUsingCommonBuffer() const
    {
    return ( iExecutionFlags & EAlfExecuteUsingCommonBuffer );
    }

inline CAlfSrvTextureManager& CAlfAppSrvSession::AlfTextureManager()
    {
    return AlfServer()->TextureManager();
    }

void CAlfAppSrvSession::NotifyTextureInfo( const RMessage2& aMessage )
    {
    if ( iTextureInfoEvent.IsNull() )
        {
        iTextureInfoEvent = aMessage;
                
        // If there has been texture info updates, 
        // complete immediately.
        DeliverTextureInfo();
        }
    else
        {
        aMessage.Complete( KErrInUse );
        }   
    }

inline TBool CAlfAppSrvSession::HasPendingTextureInfo() const
    {
    TBool found = EFalse;
    
    for ( TInt i = 0; i < iTextureInfo.Count(); i++ )
        {
        const TAlfTextureInfo& info = iTextureInfo[ i ];
        if ( info.iFlags & TAlfTextureInfo::EAlfFlagTextureSizeChanged )
            {
            found = ETrue;
            break;
            }
        }
        
    return found;
    }
      
void CAlfAppSrvSession::DeliverTextureInfo()
    {
    if ( !(iFocused || iPartiallyVisible) || 
         iTextureInfoEvent.IsNull() || !HasPendingTextureInfo() )
        {
        // Events are not delivered if
        // - application does not have focus not partially visible
        // - there is no pending notification
        // - there is no texture info events to be delivered.
        return;
        }
        
    const TInt maxLength = iTextureInfoEvent.GetDesMaxLength( 0 );
    
    TAlfTextureInfoEvent event;
    TPckgC<TAlfTextureInfoEvent> eventPckg( event );
    const TInt eventPckgLength = eventPckg.Length();
    
    if ( maxLength <= eventPckgLength )
        {
        // Max length is less than size of one event buffer.
        // So bail out.
        iTextureInfoEvent.Complete( KErrBadDescriptor );
        }
    else
        {
        HBufC8* buffer = HBufC8::New( maxLength );
        if ( buffer )
            {
            TPtr8 ptr = buffer->Des();

            TInt priority = 0;
            TInt foundTexturePos = 0;
            
            const TInt totalTextureInfo = iTextureInfo.Count();
            
            while ( ( foundTexturePos != KErrNotFound ) && 
                    ( maxLength - ptr.Length() >= eventPckgLength ) )
                {
                // As long as there is space and another texture, 
                // append to buffer.
                
                // Find using current priority.
                foundTexturePos = 
                    FindNextTextureInfoForDelivery( priority, foundTexturePos );
                if ( ( foundTexturePos == KErrNotFound ) && !priority )
                    {
                    // If not found and priority was high, try
                    // with lower priority.
                    priority++;
                    foundTexturePos = 0;
                    
                    foundTexturePos = FindNextTextureInfoForDelivery( 
                        priority, foundTexturePos );
                    }
                                
                if ( foundTexturePos != KErrNotFound )
                    {
                    TAlfTextureInfo& current = iTextureInfo[ foundTexturePos ];
                    current.iFlags |= 
                        TAlfTextureInfo::EAlfFlagTextureDelivered;
                    current.iFlags &= 
                        ~TAlfTextureInfo::EAlfFlagTextureSizeChanged;
                    
                    // Append event data
                    event.iTextureId = current.iTextureId;
                    event.iTextureSize = current.iTextureSize;
                    ptr.Append( eventPckg );
                    
                    foundTexturePos++; // this has been delivered, skip to next
                    }
                }
        
            iTextureInfoEvent.Write( 0, ptr );
            iTextureInfoEvent.Complete( KErrNone );
            
            delete buffer;
            buffer = NULL;
            }
        else
            {
            // It is expected that maxLength is relatively small value.
            // So it's better to try again a bit later.
            }
        }
    }

TInt CAlfAppSrvSession::FindNextTextureInfoForDelivery( 
        TBool aPriority, TInt aStartPos ) const
    {
    const TInt count = iTextureInfo.Count();
    TInt result = KErrNotFound;
    
    for ( TInt index = aStartPos; index < count; index++ )
        {
        const TAlfTextureInfo& current = iTextureInfo[ index ];

        if ( ( current.iFlags & TAlfTextureInfo::EAlfFlagTextureSizeChanged ) &&
             ( current.iDeliveryPriority == aPriority ) )
            {
            result = index;
            break;
            }
        }
    
    return result;    
    }

TInt CAlfAppSrvSession::FindTextureInfoById( TInt aTextureId ) const
    {
    const TInt count = iTextureInfo.Count();
    
    // Optimization to handle the case that this method is called
    // several times using the same texture id.
    if ( iPreviousTextureInfoIndex < count )
        {
        if ( iTextureInfo[ iPreviousTextureInfoIndex ].iTextureId == 
             aTextureId )
            {
            return iPreviousTextureInfoIndex;
            }
        }
         
    
    TInt pos = KErrNotFound;
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( aTextureId == iTextureInfo[ i ].iTextureId )
            {
            pos = i;
            iPreviousTextureInfoIndex = pos;
            break;
            }
        }
    
    return pos;
    }

void CAlfAppSrvSession::RemoveTextureInfo( TInt aTextureId )
    {
    const TInt pos = FindTextureInfoById( aTextureId );
    if ( pos != KErrNotFound )
        {
        iTextureInfo.Remove( pos );
        }

    //Photos is never killed if it is excluded here. hence commenting
	/*
	if (!iTextureInfo.Count())
        {
        if (AlfAppUi()->BridgerClient())
            {
            AlfAppUi()->BridgerClient()->SendBlind(EAlfExcludeFromGoomTargets, TIpcArgs(SecureId(),ClientWindowGroup()));
            }
        }
	*/
    }

TInt CAlfAppSrvSession::GetTextureSize( 
        TInt aTextureId, 
        TSize& aTextureSize,
        TBool& aHasBeenDelivered )
    {
    TInt pos = FindTextureInfoById( aTextureId );
    if ( pos != KErrNotFound )
        {
        // TAlfTextureInfo struct was found - pos >= 0
        aTextureSize = iTextureInfo[ pos ].iTextureSize;
        aHasBeenDelivered = iTextureInfo[ pos ].iFlags & 
            TAlfTextureInfo::EAlfFlagTextureDelivered;
        }
        
    return pos != KErrNotFound;
    }
    
void CAlfAppSrvSession::SetTextureSize(
        TInt aTextureId,
        const TSize& aTextureSize,
        TInt aPriority )
    {
    const TBool highPriority = ( aPriority < 0 );
    aPriority = Max( 0, aPriority );
    aPriority = Min( 1, aPriority );

    if (!iTextureInfo.Count())
        {
        if (AlfAppUi()->BridgerClient())
            {
            AlfAppUi()->BridgerClient()->SendBlind(EAlfVolunteerForGoomTarget, TIpcArgs(SecureId(), ClientWindowGroup()));
            }
        }


    TInt pos = FindTextureInfoById( aTextureId );
    if ( pos != KErrNotFound )
        {
        // TAlfTextureInfo struct was found - pos >= 0
        TAlfTextureInfo& info = iTextureInfo[ pos ];
        info.iTextureSize = aTextureSize;
        info.iFlags |= TAlfTextureInfo::EAlfFlagTextureSizeChanged;
        info.iFlags &= ~TAlfTextureInfo::EAlfFlagTextureDelivered;
        info.iDeliveryPriority = aPriority;
        
        if ( highPriority )
            {
            // Move to the beginning. After this, above info
            // struct is invalid.
            TAlfTextureInfo tmp = iTextureInfo[ pos ];
            iTextureInfo.Remove( pos );
            iTextureInfo.Insert( tmp, 0 );
            }
        }
    else
        {
        // TAlfTextureInfo struct was not found - append a new one.
        TAlfTextureInfo info;
        info.iFlags = TAlfTextureInfo::EAlfFlagTextureSizeChanged;
        info.iTextureId = aTextureId;
        info.iTextureSize = aTextureSize;
        info.iDeliveryPriority = aPriority;
        
        if ( highPriority )
            {
            iTextureInfo.Insert( info, 0 );
            }
        else
            {
            iTextureInfo.Append( info );
            }
        }
    }
    
void CAlfAppSrvSession::RemoveTextureSize( TInt aTextureId )
    {
    RemoveTextureInfo( aTextureId );
    }
    
void CAlfAppSrvSession::TextureSizeChangesCompleted()
    {
    DeliverTextureInfo();
    }

// ---------------------------------------------------------------------------
// PostQtCommandBufferL
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::PostQtCommandBufferL( const RMessage2& aMessage )
    {
  	TAlfQtCommandBufferParams params;
    TPckg<TAlfQtCommandBufferParams> paramsPckg(params);
    aMessage.Read(0,paramsPckg);    
	AlfAppUi()->PostQTCommandBufferL( params );  
	}

// ---------------------------------------------------------------------------
// ReportFrameRateBeginL
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::ReportFrameRateBegin( const RMessage2& aMessage )
    {
    TInt32 id = aMessage.Int0();
    TFrameStamp* stamp = iFPSMeasurementArray.Find( id );
    // remove existing id, if any
    if ( stamp )
        {
        iFPSMeasurementArray.Remove(id);
        }
    iFPSMeasurementArray.Insert( id, TFrameStamp( CHuiStatic::FrameCount() ));
    }
            
// ---------------------------------------------------------------------------
// ReportFrameRateEndL
// ---------------------------------------------------------------------------
//
void CAlfAppSrvSession::ReportFrameRateEnd( const RMessage2& aMessage )
    {
    TInt32 id = aMessage.Int0();
    TFrameStamp FPSMeasurementEnd= TFrameStamp( CHuiStatic::FrameCount() );
    TBuf<40> logText;
            
    TFrameStamp* begin = iFPSMeasurementArray.Find( id );
    if ( begin )
        {
        TReal32 fps = FPSMeasurementEnd.FrameRate( *begin );
        iFPSMeasurementArray.Remove(id);
        
        logText.Format( _L("%d\t%4.2f"), id, fps );    
        RFileLogger::Write( KLogsDir, KAlfFPSLogFile, EFileLoggingModeAppend, logText );
        }
    else
        {
        logText.Format( _L("Id %d not found"), id );    
        RFileLogger::Write( KAlfFPSLogDir, KAlfFPSLogFile, EFileLoggingModeAppend, logText );
        }
    RDebug::Print(_L("CAlfAppSrvSession::ReportFrameRateL - %S"), &logText );
    }


// ---------------------------------------------------------------------------
// EnvEnableLowMemoryState
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvEnableLowMemoryState(const RMessage2& aMessage)
    {
    TBool mode = aMessage.Int0();
    AlfAppUi()->NotifyLowMemory(mode);
    }

// ---------------------------------------------------------------------------
// EnvForceSwRendering
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvForceSwRendering(const RMessage2& aMessage)
    {
    TBool enabled = aMessage.Int0();
    TInt err = AlfAppUi()->ForceSwRendering( enabled );
    aMessage.Complete( err );
    }

// ---------------------------------------------------------------------------
// EnvGetSizeAndRotation
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvGetSizeAndRotation(const RMessage2& aMessage)
    {
    TPckgBuf<TSize> size;
    TPckgBuf<TInt> rotation;
    TInt err = AlfAppUi()->GetSizeAndRotation(size(), rotation());
    if ( err == KErrNone )
        {
        aMessage.Write(0, size);
        aMessage.Write(1, rotation);
        }
    aMessage.Complete( err );
    }

// ---------------------------------------------------------------------------
// EnvReadPixels
// ---------------------------------------------------------------------------
//    
void CAlfAppSrvSession::EnvReadPixels(const RMessage2& aMessage)
    {
    TInt bitmapHandle = aMessage.Int0();
    CFbsBitmap* bmp = new CFbsBitmap;
    TInt err = KErrNoMemory;

    if ( bmp )
        {
        err = bmp->Duplicate( bitmapHandle );
        if ( err == KErrNone )
            {
            err = AlfAppUi()->ReadPixels( bmp );
            }        
        delete bmp;
        }
    
    aMessage.Complete( err );
    }


// End of file    
