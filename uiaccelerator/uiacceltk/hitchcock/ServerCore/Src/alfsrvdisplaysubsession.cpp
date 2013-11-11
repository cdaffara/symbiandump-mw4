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
* Description:   Display sub session
*
*/



#include "alfsrvdisplaysubsession.h"
#include "alf/alfconstants.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfappsrvsession.h"
#include "alfsrvscreenbuffermanager.h"
#include "alfsrvtranseffect.h"
#include <alf/alfdisplaybackgrounditem.h>
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiDisplayCoeControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/huidisplaybackgrounditem.h>
#include <uiacceltk/HuiEvent.h>
#include <mtransitionserver.h>

// ETrue if client defined display quality is supported, EFalse if not.
// As there is other content than this ALF application content in the scene, 
// it's better ignore client quality hints.
// Note: If this is supported and client gains focus, client quality setting is
//       taken into use and applied to the whole scene. However, when client loses
//       focus, quality setting is not restored.
const TBool KAlfSrvClientQualitySupport = EFalse;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvDisplaySubSession::CAlfSrvDisplaySubSession( 
    CAlfAppSrvSessionBase& aSession,
    const TRect& aRect,
    TInt aWindowGroupId,
    TInt aDisplayType,
    const TUid& aBufferUid) 
    : CAlfSrvSubSessionBase( aSession ),
      iDisplayType(aDisplayType), 
      iScreenBufferUid(aBufferUid),
      iRect(aRect), 
      iDisplayClearBackground(CHuiDisplay::EClearWithSkinBackground),
      iDisplayRenderingQuality(EHuiQualityAccurate),
      iDisplayUseDepthTest(EFalse),
      iDisplayUsage(0)
      
    {
    if (!IsTvOut())
        {
        // Note: Might not work if there are multiple displays: the client drawing area is set
        // according to the last display. Also the new area is left there even if  ConstructL() 
        // fails(!).
        static_cast<CAlfAppSrvSession&>(Session()).SetClientDrawingArea( aRect );
        aSession.SetClientWindowGroupId( aWindowGroupId );           
        Session().AlfAppUi()->SetAlfAppWindowGroup(aWindowGroupId);
        }    
    }


// ---------------------------------------------------------------------------
// 2nd phasse constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvDisplaySubSession::ConstructL()
    {
    CAlfSrvSubSessionBase::ConstructL();
    
    // With one display, use the existing one or create one if there aren't any.
    const TRect& rect = CHuiStatic::ScreenDevice()->SizeInPixels();
    
    // when running oldstyle, we may have rect different from whole screen
    if (Session().AlfAppUi()->Container())
        {
        const TRect& rect = Session().AlfAppUi()->Container()->Rect();
        }
    
    // Shared normal screen 0
    if ((iDisplayType == CHuiDisplay::EDisplayLcd0) || 
       ((iDisplayType == CHuiDisplay::EDisplayNormal) && (iScreenBufferUid == KHuiUidBackBufferScreen0)))
        {            
        iDisplay = 
            Session().SharedHuiEnv()->DisplayCount() ? 
                &Session().SharedHuiEnv()->PrimaryDisplay() : 
                &Session().SharedHuiEnv()->NewDisplayL( rect, Session().AlfAppUi()->Container(), 0,
                	NULL,  CHuiDisplay::EDisplayNormal, iScreenBufferUid );
                
        Session().AlfAppUi()->AppendDisplayOnSharedWindowL(*iDisplay);
        
        #ifdef TFXSERVER_API_V2
		// Register to Tfx Server
        MTransitionServer2* tfxServer = Session().AlfServer()->TfxServer();
        if (tfxServer)
        	{
        	// Note: if this returns KErrNotFound, it is also send to Tfx server
        	TInt wgId = Session().ClientWindowGroup();
       		// Just ignore returned error code. Can this cause trouble? 
       		tfxServer->RegisterApp(MTransitionServer2::ETypeAlf, wgId, KNullUid, KNullUid);
        	}
   		#endif
        }
        
    // Off screen buffer    
    else if (iDisplayType == CHuiDisplay::EDisplayOffScreenBuffer)
        {            
        iDisplay = &Session().SharedHuiEnv()->NewDisplayL( rect, Session().AlfAppUi()->Container(), 0, NULL, iDisplayType, iScreenBufferUid );

        TRect clipped = rect;
        if (clipped.Intersects(iRect))
            {
            clipped.Intersection(iRect);        
            }

        iDisplay->SetVisibleArea(clipped);        
        }
        
    // Shared TV-out display    
    else if (IsTvOut())
        {
        if (Session().SharedHuiEnv()->DisplayCount() == 0)
            {
            // Primary lcd must exist before tv out is created            
            User::Leave(KErrNotFound);                    
            }                
        else
            {                
            iDisplay = &Session().SharedHuiEnv()->NewDisplayL( iRect,
                 Session().AlfAppUi()->Container(),
                 0, 
                 &Session().SharedHuiEnv()->PrimaryDisplay(), 
                 CHuiDisplay::EDisplayNormal, 
                 KHuiUidBackBufferTvOutNormal
                 );                  
            }
        }  
     else
     	{
     	User::Leave(KErrNotFound); 
     	} 
     
    // If we have screen buffer observers for the new display, add Alf server as an observer 	
    TAlfScreenBufferEvent event = {iScreenBufferUid, MHuiScreenBufferObserver::ECreated, TRect(), TRect()};
    if (Session().AlfServer()->ScreenBufferManager().TriggerScreenBufferEvent(event))
    	{
    	iDisplay->AddScreenBufferObserverL(&Session().AlfServer()->ScreenBufferManager());
    	}
	
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfSrvDisplaySubSession* CAlfSrvDisplaySubSession::NewL( 
    CAlfAppSrvSessionBase& aSession,
    const TRect& aRect,
    TInt aWindowGroupId,
    TInt aDisplayType,
    const TUid& aBufferUid )
    {
    CAlfSrvDisplaySubSession* self = 
        CAlfSrvDisplaySubSession::NewLC( aSession, aRect, aWindowGroupId, aDisplayType, aBufferUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfSrvDisplaySubSession* CAlfSrvDisplaySubSession::NewLC( 
    CAlfAppSrvSessionBase& aSession,
    const TRect& aRect,
    TInt aWindowGroupId,
    TInt aDisplayType,
    const TUid& aBufferUid )
    {
    CAlfSrvDisplaySubSession* self = 
        new( ELeave ) CAlfSrvDisplaySubSession( aSession, aRect, aWindowGroupId, aDisplayType, aBufferUid);
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvDisplaySubSession::~CAlfSrvDisplaySubSession()
    {
    if (iDisplay && (IsTvOut() || (iDisplayType == CHuiDisplay::EDisplayOffScreenBuffer)))
        {
        if (IsTvOut())
        	{
   	        Session().AlfAppUi()->RemoveTvDisplayOnSharedWindow(*iDisplay);
        	}
        delete iDisplay;    
        }
    iDisplay = NULL;
    iBackgroundItems.Close();    
    
    // If we are deleting shared screen subsession, deregister to Tfx Server
    if ((iDisplayType == CHuiDisplay::EDisplayLcd0) || 
    	((iDisplayType == CHuiDisplay::EDisplayNormal) && (iScreenBufferUid == KHuiUidBackBufferScreen0)))
    	{  
        #ifdef TFXSERVER_API_V2
    	MTransitionServer2* tfxServer = Session().AlfServer()->TfxServer();
    	if (tfxServer)
    		{
        	// Note: if this returns KErrNotFound, it is also send to Tfx server
        	TInt wgId = Session().ClientWindowGroup();
			tfxServer->UnregisterApp(MTransitionServer2::ETypeAlf, wgId);
        	}
    	#endif
        }
        
    // if this is a last displaysubsession remove "alfnative client wg" tag from bridge
     RPointerArray<CAlfSrvSubSessionBase> displays;
     CleanupClosePushL( displays );
     Session().GetSubsessionsByTypeL( displays, EHuiObjectTypeDisplay );

     if( displays.Count() == 0 )
         {
         Session().AlfAppUi()->RemoveAlfAppWindowGroup(Session().ClientWindowGroup());
         }
     CleanupStack::PopAndDestroy( &displays ); // displays.Close()
    }

// ---------------------------------------------------------------------------
// Gets display
// ---------------------------------------------------------------------------
//
CHuiDisplay& CAlfSrvDisplaySubSession::Display()
    {
    return *iDisplay;
    }
 
// ---------------------------------------------------------------------------
// Sets background clear mode.
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetClearBackgroundL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    // Parse parameters

    // 1: clear background (in)
    const TInt clearBackground = aMessage.Int1();
            
    if ( session.IsFocused() )
        {
        // Clear the background items, those are not used in this case
        iBackgroundItems.Reset();

        // Set background
        TRAPD(err, Display().SetClearBackgroundL( CHuiDisplay::TClearMode( clearBackground )) );
        // Don't let client see timeout errors from our internal services implementation
        if( err != KErrNone && err != KErrTimedOut ) 
            {
            User::Leave(err);
            }
        }
    
    // Complete
    aMessage.Complete( KErrNone );
    
    // Store value for this client
    iDisplayClearBackground = clearBackground;
    }

// ---------------------------------------------------------------------------
// Sets visible area.
// ---------------------------------------------------------------------------
//
void CAlfSrvDisplaySubSession::SetVisibleAreaL( const RMessage2& aMessage, TBool aForce )
    {
    // Parse parameters

    // 1: area rect (in)
    TRect rect;
    TPckg<TRect> rectPckg(rect); 
    aMessage.Read(1,rectPckg);
	        
		CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());	        
	           
    // Set clipping rect for display visible area to e.g. avoid unnecesssary 
    // drawing under Avkon status/control pane areas. TV out does not show
    // those so no clipping in that case.
    if (UseVisualAreaClipping())
       {
       iRect = rect;
		
			if ( session.IsFocused() )
        		{
    	      TRect clipped = Session().AlfAppUi()->Container()->Rect();
#ifndef SYMBIAN_BUILD_GCE
		       	if (clipped.Intersects(rect))
		           {
		           clipped.Intersection(rect);        
		           }
#endif           
		       	iDisplay->SetVisibleAreaClippingRect(clipped);        
		       	iDisplay->SetDirty();
	        	}
	       }           
	           
    // Set visible area        
			if ( session.IsFocused() || aForce )
        		{
    		    static_cast<CAlfAppSrvSession&>(Session()).SetClientDrawingArea( rect );
        		}
    // Complete
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// Returns the visible area.
// ---------------------------------------------------------------------------
//
void CAlfSrvDisplaySubSession::VisibleAreaL( const RMessage2& aMessage )
    {
    // Parse parameters

    // 2: area rect (out)
    TRect rect;
    TPckg<TRect> rectPckg(rect); 
    aMessage.Read(2,rectPckg);
            
    // Get visible area
    rect = static_cast<CAlfAppSrvSession&>(Session()).ClientDrawingArea();

    // Write parameters
    aMessage.Write(2,rectPckg);
    
    // Complete
    aMessage.Complete( KErrNone );
    }


// ---------------------------------------------------------------------------
// Sets display dirty
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetDirtyL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    if ( session.IsFocused() )
        {
        Display().SetDirty();    
        }
    
    // Complete
    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------------------------
// Sets display rendering quality
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetQualityL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 1: quality (in)
    THuiQuality value = EHuiQualityAccurate;
    TPckg<THuiQuality> valuePckg(value); 
    aMessage.Read(1,valuePckg);

    if ( KAlfSrvClientQualitySupport && session.IsFocused() )
        {
        // Set quality
        Display().SetQuality(value);    
        }
    
    // Complete
    aMessage.Complete( KErrNone );    

    // Store value for this client
    iDisplayRenderingQuality = value;
    }

// ---------------------------------------------------------------------------
// Gets display rendering quality
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::QualityL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 2: quality (out)
    THuiQuality value = EHuiQualityAccurate;
    TPckg<THuiQuality> valuePckg(value); 

    if ( KAlfSrvClientQualitySupport && session.IsFocused() )
        {
        // Get quality
        value = Display().Quality();    
        }
    else
        {
        value = THuiQuality(iDisplayRenderingQuality);
        }
    
    // Write parameters
    aMessage.Write(2,valuePckg);
    
    // Complete
    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------------------------
// Sets display depth test usage
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetUseDepthL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 1: bool (in)
    TBool value = EFalse;
    TPckg<TBool> valuePckg(value); 
    aMessage.Read(1,valuePckg);

    // Update value if the client is on focus
    if ( session.IsFocused() )
        {
        // Set use depth
        Display().SetUseDepth(value);    
        }
    
    // Complete
    aMessage.Complete( KErrNone );    

    // Store value for this client
    iDisplayUseDepthTest = value;
    }

// ---------------------------------------------------------------------------
// Sets display usage
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetUsageL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 1: TUint (in)
    TUint value = 0;
    TPckg<TUint> valuePckg(value); 
    aMessage.Read(1,valuePckg);

    // Update value if the client is on focus
    if ( session.IsFocused() )
        {
        // Set use depth
        Display().SetUsageL(value);    
        }
    
    // Complete
    aMessage.Complete( KErrNone );    

    // Store value for this client
    iDisplayUsage = value;
    }

// ---------------------------------------------------------------------------
// Sets visual content for the window
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetClientWindowForDrawingL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 1: TUint (in)
    TInt3 data(0,0,0);
    TPckg<TInt3> dataPckg(data); 
    aMessage.Read(1,dataPckg);    
    
    // Update value even if the client is not in focus
    
    CHuiVisual* visual = NULL;
    TInt visualHandle = data.iInt3;
    if (visualHandle)
    	{
    	visual = (CHuiVisual*) Session().GetInterfaceL(EHuiObjectTypeVisual, visualHandle);
    	}
		Session().AlfAppUi()->SetClientWindowForDrawingL(data.iInt1, data.iInt2, visual);
    
    // Complete
    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------------------------
// Sets display bqckground items
// ---------------------------------------------------------------------------
//    
void CAlfSrvDisplaySubSession::SetBackgroundItemsL( const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters
    HBufC8* buffer = HBufC8::NewLC(aMessage.GetDesLength(1));
    TPtr8 ptr = buffer->Des();
    aMessage.Read(1, ptr);

    RArray<TAlfDisplayBackgroundItem> array;
    CleanupClosePushL( array );
    InternalizeL(*buffer, array);

    // Store value for this client
    iBackgroundItems.Reset();
    for(TInt i = 0; i < array.Count(); ++i)
        {
        TAlfDisplayBackgroundItem alfItem = array[i];
        THuiDisplayBackgroundItem* huiItem = (THuiDisplayBackgroundItem*)&alfItem;
        iBackgroundItems.Append(*huiItem);
        }

    CleanupStack::PopAndDestroy( &array );

    // Reset full background to none
    iDisplayClearBackground = CHuiDisplay::EClearNone;

    // Update value if the client is on focus
    if ( session.IsFocused() )
        {        
        // Set use background items
        Display().SetBackgroundItemsL(iBackgroundItems);    
        }
    
    CleanupStack::PopAndDestroy( buffer );
    
    // Complete
    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------------------------
// Sets non fading
// ---------------------------------------------------------------------------
//
void CAlfSrvDisplaySubSession::SuppressAutomaticFadingL( 
        const RMessage2& aMessage )
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());
    
    // Parse parameters

    // 1: TBool (in)
    TBool value = EFalse;
    TPckg<TBool> valuePckg(value); 
    aMessage.Read(1,valuePckg);

    // Store value for this client
    iAutomaticFadingSuppressed = value;

    // Update value if the client is on focus and it's shown in a window.
    if ( session.IsFocused() )
        {
        UpdateAutomaticFading();
        }

    // Complete
    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------------------------
// From CAlfSrvSubSessionBase
// Process the message.
// ---------------------------------------------------------------------------
// 
void CAlfSrvDisplaySubSession::DoProcessMessageL(const RMessage2& aMessage)
    {
    switch( aMessage.Int0() )
        {
        case EAlfDisplaySetClearBackground:
            {
            SetClearBackgroundL( aMessage );
            break;
            }
        case EAlfDisplaySetVisibleArea:
            {
            SetVisibleAreaL( aMessage );
            break;
            }
        case EAlfDisplayForceSetVisibleArea:
            {
            SetVisibleAreaL( aMessage, ETrue );
            break;
            }
        case EAlfDisplayVisibleArea:
            {
            VisibleAreaL( aMessage );
            break;
            }
        case EAlfDisplaySubSessionClose: // obsolete
            {
            Session().CloseSubSession( aMessage );
            break;
            }
        case EAlfDisplaySetDirty:
            {
            SetDirtyL( aMessage );
            break;
            }
        case EAlfDisplaySetQuality:
            {
            SetQualityL( aMessage );            
            break;
            }
        case EAlfDisplayQuality:
            {            
            QualityL( aMessage );
            break;
            }
        case EAlfDisplaySetUseDepth:
            {
            SetUseDepthL( aMessage );            
            break;
            }
        case EAlfDisplaySetUsage:
            {
            SetUsageL( aMessage );            
            break;
            }
        case EAlfDisplaySetBackgroundItems:
            {
            SetBackgroundItemsL( aMessage );            
            break;
            }
        case EAlfDisplayHandlePointerEvent:
            {
            HandlePointerEventL( aMessage );            
            break;
            }

        case EAlfDisplaySuppressAutomaticFading:
            {
            SuppressAutomaticFadingL( aMessage );
            break;
            }

        case EAlfDisplaySetClientWindowForDrawing:
            {
            SetClientWindowForDrawingL( aMessage );
            break;
            }

        default:
            CAlfSrvSubSessionBase::DoProcessMessageL( aMessage );
            break;
        }
    }
    
 
CHuiDisplay* CAlfSrvDisplaySubSession::AsHuiDisplay()
    {
    return iDisplay;
    }
    
void CAlfSrvDisplaySubSession::SetSessionFocused(TBool aFocused)
    {
    if ( iDisplay )
        {
        if (aFocused)
            {
            // Set clipping rect for display visible area to e.g. avoid unnecesssary 
            // drawing under Avkon status/control pane areas. TV out does not show
            // those so no clipping in that case.
            if (UseVisualAreaClipping())
                {
                TRect clipped = Session().AlfAppUi()->Container()->Rect();
                if (clipped.Intersects(iRect))
                    {
                    clipped.Intersection(iRect);        
                    }
                iDisplay->SetVisibleAreaClippingRect(clipped);        
                iDisplay->SetDirty();
                }

            // If background items are used
            if (iBackgroundItems.Count() != 0)
                {
                TRAP_IGNORE(iDisplay->SetBackgroundItemsL(iBackgroundItems))                        
                }
            else
                {
                TRAP_IGNORE(iDisplay->SetClearBackgroundL(
                        CHuiDisplay::TClearMode(iDisplayClearBackground)))                                        
                }    

            if ( KAlfSrvClientQualitySupport )
                {
                iDisplay->SetQuality(THuiQuality(iDisplayRenderingQuality));
                }
            iDisplay->SetUseDepth(iDisplayUseDepthTest);
            
            UpdateAutomaticFading(); // update non-fading to app ui container
            }
        else
            {
            // Disable clipping rect 
            if (UseVisualAreaClipping())
                {
                iDisplay->SetVisibleAreaClippingRect(TRect(0,0,0,0));        
                iDisplay->SetDirty();
                }                
            }    
        
        if (IsTvOut())
            {
            if (aFocused)
                {
                TRAP_IGNORE(
                    {
                    iDisplay->RestoreL();
                    Session().AlfAppUi()->AppendTvDisplayOnSharedWindowL(*iDisplay);
                    })
                }
            else
                {
                Session().AlfAppUi()->RemoveTvDisplayOnSharedWindow(*iDisplay);
                iDisplay->Release();    
                }                
            }
        }
    }

TBool CAlfSrvDisplaySubSession::IsTvOut() const
    {
    return((iDisplayType == CHuiDisplay::EDisplayTvOut) || 
           (iDisplayType == CHuiDisplay::EDisplayTvOutWide) ||
           ((iDisplayType == CHuiDisplay::EDisplayNormal) && 
           ((iScreenBufferUid == KHuiUidBackBufferTvOutNormal) ||
           (iScreenBufferUid == KHuiUidBackBufferTvOutWide))));
    }

TInt CAlfSrvDisplaySubSession::DisplayType() const
    {
    return iDisplayType;    
    }

void CAlfSrvDisplaySubSession::HandlePointerEventL( const RMessage2& aMessage )
    {
    // This function handles pointer events that are (optionally) sent from client side

    // Parse parameters
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());

    // 1: TPointerEvent (in)
    TAdvancedPointerEvent value;
    TPckg<TAdvancedPointerEvent> valuePckg(value); 
    aMessage.Read(1,valuePckg);

    // Update value if the client is on focus
    if ( session.IsFocused() && AsHuiDisplay() )
        {
        // iPosition has no meaning as client does not know the location of visuals
        // Threfore only iParentposition is used, it is assumed to be relative to 
        // display coordinates.
        value.iPosition = value.iParentPosition; 
        session.StartPointerHandling();
        THuiEvent event(AsHuiDisplay(), value);
               
        AsHuiDisplay()->Roster().HandleEventL(event);
        session.FlushPointerHandling();
        }
    
    // Complete
    aMessage.Complete( KErrNone );    
    }

void CAlfSrvDisplaySubSession::UpdateAutomaticFading()
    {
    CAlfAppSrvSession& session = static_cast<CAlfAppSrvSession&>(Session());

    if ( ( iDisplayType == CHuiDisplay::EDisplayLcd0 ) || 
         ( ( iDisplayType == CHuiDisplay::EDisplayNormal ) && 
           ( iScreenBufferUid == KHuiUidBackBufferScreen0 ) ) )
        {
        Session().AlfAppUi()->SetContainerNonFading( iAutomaticFadingSuppressed );
        }
    }

TBool CAlfSrvDisplaySubSession::UseVisualAreaClipping()
    {
    // This method decides wheter we try to clip drawing to the visible area
    // and thus avoid unnesessary drawing under status/control
    // panes and thus gain some perfomance improvement.
    //
    // - For TV out visual area is never clipped as there is no Avkon
    // statuspane/control panes.
    //
    // - We dont use clipping if we use Hitchcock transitions between apps
    // because setting clipping in the middle of transition may look ugly
    // if clipped areas are different sizes.
    //
    // - Clipping is only done for BitGdi renderer.
    //
#ifdef SYMBIAN_BUILD_GCE
    return EFalse;
#else
    return (!IsTvOut() && 
            !Session().AlfServer()->TransitionEffects()->IsEnabled() && 
            Session().AlfAppUi()->HuiEnv().Renderer() == EHuiRendererBitgdi);
#endif
    }
