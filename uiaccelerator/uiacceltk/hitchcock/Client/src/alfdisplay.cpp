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
* Description:   Display
*
*/



#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uikon/eikdefmacros.h>
#endif
#include <aknappui.h>
#include <coecntrl.h>
#include <coecobs.h>
#include <gdi.h>
#include <AknUtils.h>
#include <babitflags.h>

#include "alf/alfdisplay.h"
#include "alfdisplaysubsession.h"
#include "alf/alfroster.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfevent.h"
#include "alf/alfinputcapabilitiessupplier.h"
#include "alflogger.h"
#include "alf/alfevent.h"
#include <alf/alfscreenbuffer.h>
#include <alf/alfsymbiansbdrawer.h>

#include "uiacceltk/HuiUtil.h"

// Flags
enum TAlfDisplayFlags
    {
    EAlfDisplayActive =                     0x01,
    EAlfDisplayUpdateRenderState =          0x02,
    EAlfDisplayOwnRoster =                  0x04
    };
 
// Default flags.   
const TUint KAlfDisplayDefaultFlags = 0x00;

enum
    {
    EAlfDisplayRectCustom,
    EAlfDisplayRectMainPane,
    EAlfDisplayRectWholeSceen        
    };


// Private data.
struct CAlfDisplay::TPrivateData
    {
    TPrivateData():iEnv(0),iNativeControl(0),iRoster(0){}
    RAlfDisplaySubSession iDisplaySubSession; // Owned
    TUint iFlags;                           // Owned.
    CAlfEnv* iEnv;                          // Not owned.
    CAlfDisplayCoeControl* iNativeControl;  // Owned.
    CAlfRoster* iRoster;                    // Owned/not owned - see EAlfDisplayOwnRoster
    TInt iDisplayRectType; // To allow automatic screensize change in layout switch
    };

// ======== LOCAL FUNCTIONS ========

NONSHARABLE_CLASS( CAlfDisplayCoeControl ): 
    public CCoeControl
    {
public: // Enumerations
    enum 
        {
        EManipulatingInputFocusIndex,
        EIsActiveIndex
        };

public: 

    CAlfDisplayCoeControl(){}
    CAlfDisplayCoeControl( CAlfDisplay& aDisplay ) : iDisplay(&aDisplay){}
    void SetFocusedEditor( MAlfInputCapabilitiesSupplier* aFocusedEditor );
    
    /**
    * This returns whether the CCoeControl is focused in general. However, during an operation where 
    * focus is temporarily withdrawn from the CCoeControl, this state will continue to return ETrue.
    * This method cannot be called IsFocused since that would access the instantaneous 
    * CONE focus, which is not what is required here.
    */
    TBool IsActive() const;
    
    /**
    * Sets the active state according to the current focus state from CONE
    */
    void SetActiveState();
    
    virtual void InitL();
    
    virtual TBool DrawsIntoLocalWindow() const;
    
protected:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleResourceChange(TInt aType);
    /** 
    * From CCoeControl
    */
protected:
    virtual TCoeInputCapabilities InputCapabilities() const;
    virtual void FocusChanged(TDrawNow aDrawNow);

protected:
    CAlfDisplay* iDisplay;
    MAlfInputCapabilitiesSupplier* iFocusedEditor;
    TBitFlags iFlags; // Cleared at construction
    };

void CAlfDisplayCoeControl::InitL()
    {
    // Window is needed for HandleResourceChange to get events
    CreateWindowL();    
    Window().EnableOSB(EFalse);
    }


TKeyResponse CAlfDisplayCoeControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    TAlfEvent event(*iDisplay, aKeyEvent, aType);
    if(iDisplay->Roster().HandleEventL(event))
        {
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }


void CAlfDisplayCoeControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

void CAlfDisplayCoeControl::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange( aType );
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        iDisplay->Env().ReportWsEventAsActionCommand(KAlfActionIdDeviceLayoutChanged);            
        }
    }

TCoeInputCapabilities CAlfDisplayCoeControl::InputCapabilities() const
    {
    if ( iFocusedEditor )
        {
        return iFocusedEditor->InputCapabilities();
        }
    else
        {
        TCoeInputCapabilities nullCapabilities(TCoeInputCapabilities::ENavigation);
        return nullCapabilities;
        }
    }

// ---------------------------------------------------------------------------
// Sets the pointer to the object supplying input capabilities.
// This method needs to ensure that the FEP registers the change in focus of the AlfControl 
// - some sort of editor in this case. 
// ---------------------------------------------------------------------------
//
void CAlfDisplayCoeControl::SetFocusedEditor( MAlfInputCapabilitiesSupplier* aFocusedEditor )
    {
    // A flag is set to register the fact that we are currently manipulating focus of the CCoeControl, albeit temporarily.
    // The flag is used by call-backs to suppress Alf reacting too deeply to the focus changes.
    iFlags.Set( EManipulatingInputFocusIndex );
    if ( !aFocusedEditor )
        {
        // The focus state of this CCoeControl must be preserved through this "kicking" of the FEP
        // Use a local variable to highlight that the "IsActive" flag may change temporarily during this sequence
        // (depending on implementation)
        TBool focusStateToKeep = iFlags.IsSet( EIsActiveIndex );
        if ( focusStateToKeep ) // No need to even remove focus if off already
            {
            SetFocus(EFalse, ENoDrawNow); // Forces focus off this 
            // Force removal of focus to be synchronous
            ControlEnv()->SyncNotifyFocusObserversOfChangeInFocus();
            }
        iFocusedEditor = aFocusedEditor; // Removed only after FEP has taken focus off

        if ( !IsFocused() && focusStateToKeep ) // Ensure focus gets put back on
            {
            SetFocus(ETrue, ENoDrawNow); // Forces focus on again but now there is no AlfControl
            }
        }
    else
        { 
        iFocusedEditor = aFocusedEditor; // Put in place before focus is forced on
        SetFocus(ETrue, ENoDrawNow); // Forces focus onto this control
        }
    iFlags.Clear( EManipulatingInputFocusIndex );
    }
    
// ---------------------------------------------------------------------------
// This is a call-back from CONE when the focus is changed on a this CCoeControl.
// The roster must be kept "in step" with the focus change, and is called strictly 
// only when the focus state changes. 
// However Alf should not track focus changes when SetFocusedEditor is being processed
// This interlock is effected by the ManipulatingInputFocus flag
// ---------------------------------------------------------------------------
void CAlfDisplayCoeControl::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iFlags.IsClear( EManipulatingInputFocusIndex ) )
        {
        TBool coneFocus = IsFocused();
        if ( !COMPARE_BOOLS(iFlags.IsSet( EIsActiveIndex ), coneFocus ) )
            {
            iFlags.Assign( EIsActiveIndex, coneFocus );
            iDisplay->Roster().DisplayFocusChanged( *iDisplay, coneFocus );
            }
        }
    CCoeControl::FocusChanged( aDrawNow );
    }


// ---------------------------------------------------------------------------
// Implemenation of CCoeControl virtual.
// Used here to set up the focus state at the construction point of the control.
// ---------------------------------------------------------------------------
void CAlfDisplayCoeControl::SetActiveState()
    {
    iFlags.Assign( EIsActiveIndex, IsFocused() );
    iDisplay->Roster().DisplayFocusChanged( *iDisplay, iFlags.IsSet( EIsActiveIndex ) );
    }

TBool CAlfDisplayCoeControl::IsActive() const
    {
    return iFlags.IsSet( EIsActiveIndex );
    }

TBool CAlfDisplayCoeControl::DrawsIntoLocalWindow() const
    {
    return EFalse;    
    }

//
//
//
//	
NONSHARABLE_CLASS( CAlfDisplayOffScreenBufferCoeControl ): 
    public CAlfDisplayCoeControl, public MAlfScreenBufferObserver
    {
public:    
    CAlfDisplayOffScreenBufferCoeControl( CAlfDisplay& aDisplay );
    
    ~CAlfDisplayOffScreenBufferCoeControl();
    
    void InitL(TUid aBufferUid);

    void Draw( const TRect& aRect ) const;

    void SizeChanged();

    TBool DrawsIntoLocalWindow() const;

protected:
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private: // From MAlfScreenBufferObserver
	
	TBool BufferComplete(TUid aId, TRect& aDisplayRect, TRect& aDirtyRect);
	void HandleScreenBufferEvent(TUid aId, TInt aEvent);

private:
    CAlfScreenBuffer* iAlfScreenBuffer;  
	TRect iDisplayRect;		          
    CAlfSymbianBufferDrawer* iBufferDrawer;
    TUid iBufferUid;   
    };

CAlfDisplayOffScreenBufferCoeControl::CAlfDisplayOffScreenBufferCoeControl(CAlfDisplay& aDisplay)
    {
    iDisplay = &aDisplay;
    }

CAlfDisplayOffScreenBufferCoeControl::~CAlfDisplayOffScreenBufferCoeControl()
    {
    if(iBufferDrawer)
    	iBufferDrawer->ReleaseDrawer();
    delete iAlfScreenBuffer;        
    }
    
void CAlfDisplayOffScreenBufferCoeControl::InitL(TUid aBufferUid)
    {
    CAlfDisplayCoeControl::InitL();    

    // Set the windows size
    SetRect( iDisplay->VisibleArea() );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    
    iAlfScreenBuffer = CAlfScreenBuffer::NewL(iDisplay->Env());
   	iAlfScreenBuffer->AddObserverL(aBufferUid, this);   
    iBufferUid = aBufferUid;
    
    EnableDragEvents();
    }


void CAlfDisplayOffScreenBufferCoeControl::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    if (iBufferDrawer)
    	{
  		iBufferDrawer->DrawBuffer(gc, TPoint(), iDisplayRect);
    	}
        
    }

void CAlfDisplayOffScreenBufferCoeControl::SizeChanged()
    {
    DrawDeferred();  
    }

TBool CAlfDisplayOffScreenBufferCoeControl::BufferComplete(TUid aId, TRect& aDisplayRect, TRect& /*aDirtyRect*/)
	{
	if (aId == iBufferUid)
	    {	        
    	if (!iBufferDrawer)
    		{
        	iBufferDrawer = (CAlfSymbianBufferDrawer*)iAlfScreenBuffer->GetDrawingInterface(KAlfSymbianBufferDrawerUid, iBufferUid);		
    		}
    	iDisplayRect = aDisplayRect;
    	DrawNow();
	    }

	return ETrue;
	}
	
	
void CAlfDisplayOffScreenBufferCoeControl::HandleScreenBufferEvent(TUid aId, TInt aEvent)
	{
	if (aId == iBufferUid)
	    {	        
    	if (aEvent == MAlfScreenBufferObserver::ECreated)
    		{
    		}
    	else if (aEvent == MAlfScreenBufferObserver::EDeleted)
    		{
    		// delete bitmap drawer etc.
        	if(iBufferDrawer)
        		iBufferDrawer->ReleaseDrawer();
        	iBufferDrawer = NULL;
        	// Draw
    		DrawNow();
    		}
	    }
	}
	
void CAlfDisplayOffScreenBufferCoeControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    iDisplay->HandlePointerEventL( aPointerEvent );
    }
    
TBool CAlfDisplayOffScreenBufferCoeControl::DrawsIntoLocalWindow() const
    {
    return ETrue;    
    }
	
	
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfDisplay::CAlfDisplay()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfDisplay::ConstructL(
    CAlfEnv& aEnv, 
    TBool aAsCoeControl,
    const TRect& aRect, 
    CAlfRoster* aSharedRoster,
    TInt aDisplayType,
    TUid aBufferUid)
    {
    iData = new (ELeave) TPrivateData;
    
    User::LeaveIfError( iData->iDisplaySubSession.Open(aEnv.Client(), aRect, aDisplayType, aBufferUid) );
        
    
    iData->iFlags = KAlfDisplayDefaultFlags;

    iData->iEnv = &aEnv;
    iData->iNativeControl = NULL;
    iData->iFlags |= EAlfDisplayActive;
    iData->iFlags |= EAlfDisplayUpdateRenderState;
    
    
    // Create a control roster.
    if(aSharedRoster)
        {
        iData->iRoster = aSharedRoster;
        iData->iFlags &= ~EAlfDisplayOwnRoster;
        }
    else
        {
        // Construct a private roster.
        CAlfRoster* roster = new (ELeave) CAlfRoster;
        CleanupStack::PushL(roster);
        roster->ConstructL(this);
        CleanupStack::Pop(roster);
        iData->iRoster= roster;
        iData->iFlags |= EAlfDisplayOwnRoster;
        }
        
    if ( aAsCoeControl )
        {        
        if (aDisplayType == EDisplayOffScreenBuffer && aBufferUid.iUid != 0)
            {
            CAlfDisplayOffScreenBufferCoeControl* osbf = new (ELeave) CAlfDisplayOffScreenBufferCoeControl(*this);
            iData->iNativeControl = osbf;
            iData->iNativeControl->SetMopParent( iAvkonAppUi );
            CCoeEnv::Static()->AppUi()->AddToStackL(iData->iNativeControl);
            iData->iNativeControl->SetActiveState();
            osbf->InitL(aBufferUid);                       
            }
        else
            {                
            iData->iNativeControl = new (ELeave) CAlfDisplayCoeControl(*this);
            iData->iNativeControl->SetMopParent( iAvkonAppUi );
            CCoeEnv::Static()->AppUi()->AddToStackL(iData->iNativeControl);
            iData->iNativeControl->SetActiveState();
            iData->iNativeControl->InitL();       
            }    
        }

    iData->iDisplayRectType = EAlfDisplayRectCustom;   
    if ( CCoeEnv::Static() ) // check that we are running this from Symbian application
        {
        CheckScreenRect(aRect);

        // In case of display is created after application has already received foregound
        // we notify just to make sure we really have foreground
        aEnv.Client().ApplicationIsForeground(iAvkonAppUi->IsForeground() || 
            iAvkonAppUi->IsPartialForeground());    
        }
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfDisplay::~CAlfDisplay()
    {
    if ( iData )
        {
        if ( (iData->iFlags&EAlfDisplayOwnRoster) && iData->iRoster )
            {
            delete iData->iRoster;
            }
        iData->iFlags &= ~EAlfDisplayOwnRoster;
        iData->iRoster = NULL;
        
        if ( iData->iNativeControl )
            {
            CCoeEnv::Static()->AppUi()->RemoveFromStack(iData->iNativeControl);
            delete iData->iNativeControl;
            iData->iNativeControl = NULL;
            }
        
        if ( iData->iEnv )
            {
            iData->iEnv->RemoveDisplay(*this);
            }
            
        iData->iDisplaySubSession.Close();
        }
    
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Sets clear background method
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDisplay::SetClearBackgroundL(TInt aClearBackground)
    {
    TInt err =
        iData->iDisplaySubSession.SetClearBackground( 
            aClearBackground );
            
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDisplay::SetClearBackgroundL leave error %d", err )
        User::Leave( err );
        }
    }
 
// ---------------------------------------------------------------------------
// Returns roster
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfRoster& CAlfDisplay::Roster()
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    __ASSERT_ALWAYS( iData->iRoster, USER_INVARIANT() );
    return *iData->iRoster;
    }
    
// ---------------------------------------------------------------------------
// Returns roster
// ---------------------------------------------------------------------------
//
EXPORT_C const CAlfRoster& CAlfDisplay::Roster() const
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    __ASSERT_ALWAYS( iData->iRoster, USER_INVARIANT() );
    return *iData->iRoster;
    }
   
// ---------------------------------------------------------------------------
// Sets visible area.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetVisibleArea(const TRect& aArea)
    {
    TInt err = iData->iDisplaySubSession.SetVisibleArea( aArea );
    
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::SetVisibleArea ignore error %d", err )
        }
        
    if ( CCoeEnv::Static() ) // check that we are running this from Symbian application
        {
        CheckScreenRect(aArea);
        
        // If offscreenbuffer is drawn, then set its rect too.
        if (iData->iNativeControl && iData->iNativeControl->DrawsIntoLocalWindow())
            {
            iData->iNativeControl->SetRect(aArea);    
            }        
        }
    }

// ---------------------------------------------------------------------------
// Sets visible area.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfDisplay::ForceSetVisibleArea(const TRect& aArea)
    {
    // forces to set visible area
    TInt err = iData->iDisplaySubSession.SetVisibleArea( aArea, ETrue );
    
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::ForceSetVisibleArea ignore error %d", err )
        }
        
    if ( CCoeEnv::Static() ) // check that we are running this from Symbian application
        {
        CheckScreenRect(aArea);
        
        // If offscreenbuffer is drawn, then set its rect too.
        if (iData->iNativeControl && iData->iNativeControl->DrawsIntoLocalWindow())
            {
            iData->iNativeControl->SetRect(aArea);    
            }        
        }
    }

// ---------------------------------------------------------------------------
// Returns visible area
// ---------------------------------------------------------------------------
//   
EXPORT_C TRect CAlfDisplay::VisibleArea() const
    {
    TRect area(0,0,0,0);

    TInt err = iData->iDisplaySubSession.VisibleArea( area );
    
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::VisibleArea ignore error %d", err )
        }
        
    return area;
    }

// ---------------------------------------------------------------------------
// Returns env.
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfEnv& CAlfDisplay::Env()
    {
    return *iData->iEnv;
    }
    
    
// ---------------------------------------------------------------------------
// Returns server handle
// ---------------------------------------------------------------------------
// 
TInt CAlfDisplay::ServerHandle() const
    {
    return iData->iDisplaySubSession.SubSessionHandle();
    }
// ---------------------------------------------------------------------------
// Set the current focused editor. 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetFocusedEditor( MAlfInputCapabilitiesSupplier*  aCurrentEditor  )
    {
    __ASSERT_ALWAYS(iData->iNativeControl, USER_INVARIANT() );
    iData->iNativeControl->SetFocusedEditor(aCurrentEditor);
    }

// ---------------------------------------------------------------------------
// Set display dirty.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetDirty()
    {
    iData->iDisplaySubSession.SetDirty();
    }
    
// ---------------------------------------------------------------------------
// Set display rendering quality.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetQuality(TAlfQuality aRenderingQuality)
    {
    TInt err = iData->iDisplaySubSession.SetQuality(aRenderingQuality);
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::SetQuality ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// Get display rendering quality.
// ---------------------------------------------------------------------------
// 
EXPORT_C TAlfQuality CAlfDisplay::Quality() const
    {
    TAlfQuality renderingQuality;
    TInt err = iData->iDisplaySubSession.Quality(renderingQuality);
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::Quality ignore error %d", err )
        }
    return renderingQuality;
    }
    
// ---------------------------------------------------------------------------
// Set display depth test use
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetUseDepth(TBool aUseDepth)
    {
    TInt err = iData->iDisplaySubSession.SetUseDepth(aUseDepth);
    if ( err != KErrNone )
        {
        // panic?
        __ALFLOGSTRING1( "CAlfDisplay::SetUseDepth ignore error %d", err )
        }                
    }

// ---------------------------------------------------------------------------
// Handles layout switch
// ---------------------------------------------------------------------------
// 
void CAlfDisplay::NotifyLayoutChangedL()
    {
    // If screen size had been set to main pane or whole screen, 
    // set it automatically here to new value.
    // Otherwise let application handle it.
    if (iData->iDisplayRectType == EAlfDisplayRectMainPane)
        {
        TRect metricsRect = TRect(0,0,0,0);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, metricsRect);
        ForceSetVisibleArea(metricsRect);
        }
    else if (iData->iDisplayRectType == EAlfDisplayRectWholeSceen)
        {
        TRect metricsRect = TRect(0,0,0,0);
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, metricsRect);
        ForceSetVisibleArea(metricsRect);
        }
    else
        {
        // for PC lint
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CAlfDisplay::CheckScreenRect(TRect aRect)
    {
    TRect mainpaneRect = TRect(0,0,0,0);
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainpaneRect);

    TRect screenRect = TRect(0,0,0,0);
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
    
    if (aRect == mainpaneRect)
        {
        iData->iDisplayRectType = EAlfDisplayRectMainPane;    
        }
    else if (aRect == screenRect)
        {
        iData->iDisplayRectType = EAlfDisplayRectWholeSceen;    
        }
    else
        {
        iData->iDisplayRectType = EAlfDisplayRectCustom;           
        }            
    }
    
// ---------------------------------------------------------------------------
// Access the object provider for the display. 
// ---------------------------------------------------------------------------
// 
EXPORT_C MObjectProvider* CAlfDisplay::ObjectProvider() const
    {
    return iData->iNativeControl;
    }
    
TBool CAlfDisplay::IsFocused() const
    {
    return iData->iNativeControl ? iData->iNativeControl->IsActive() : ETrue;
    }


// ---------------------------------------------------------------------------
// Sets usage hint
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetUsageL(TUint aUsageHint)
    {
    TInt err = iData->iDisplaySubSession.SetUsage(aUsageHint);
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDisplay::SetUsageL leave error %d", err )
        User::Leave( err );
        }                        
    }

// ---------------------------------------------------------------------------
// Suppresses automatic fading
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CAlfDisplay::SuppressAutomaticFading( TBool aSuppress )
    {
    TInt err = iData->iDisplaySubSession.SuppressAutomaticFading( aSuppress );
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDisplay::SuppressAutomaticFading return error %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::SetBackgroundItemsL(const RArray<TAlfDisplayBackgroundItem>& aItems)
    {
    TInt err = iData->iDisplaySubSession.SetBackgroundItemsL(aItems);
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDisplay::SetBackgroundItemsL ignore error %d", err )
        }                        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfDisplay::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TInt err = iData->iDisplaySubSession.HandlePointerEventL(aPointerEvent);
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfDisplay::HandlePointerEventL ignore error %d", err )
        }        
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
// 

EXPORT_C void CAlfDisplay::SetClientWindowForDrawingL(RWindow* aWindow, CAlfVisual* aVisual)
	{
	TInt windowGroupId = 0; 
	TInt clientWindowHandle = 0;
	TInt visualHandle = NULL;
	windowGroupId = aWindow->WindowGroupId(); 
	clientWindowHandle = aWindow->ClientHandle();
	if (aVisual)
		{
		visualHandle = aVisual->Identifier();
		}
    iData->iDisplaySubSession.SetClientWindowForDrawingL(windowGroupId, clientWindowHandle, visualHandle);
	}
