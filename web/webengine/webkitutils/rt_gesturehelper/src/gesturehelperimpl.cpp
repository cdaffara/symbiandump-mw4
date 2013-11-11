/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Gesture helper implementation
*
*/


#include "gesturehelperimpl.h"

#include <e32base.h>
#include <w32std.h>

#include "gesture.h"
#include "gesturedefs.h"
#include "utils.h"
#include "gestureeventfilter.h"
#include "gesturehelpereventsender.h"
#include "flogger.h"

using namespace RT_GestureHelper;

namespace RT_GestureHelper
{

/// type of function in gesture helper to be called by the timer
/// when timer triggers
typedef void (CGestureHelperImpl::*CallbackFunctionL)();

NONSHARABLE_CLASS( CCallbackTimer ) : public CTimer
    {
public:
    /** Two-phase constructor */
    static CCallbackTimer* NewL( CGestureHelperImpl& aHelper, 
            CallbackFunctionL aCallbackFunctionL, TInt aDelay, TBool aIsEnabled )
        {
        CCallbackTimer* self = new ( ELeave ) CCallbackTimer( aHelper, 
            aCallbackFunctionL, aDelay, aIsEnabled );
        CleanupStack::PushL( self );
        self->ConstructL(); // construct base class
        CActiveScheduler::Add( self );
        CleanupStack::Pop( self );
        return self;
        }
        
    /** Destructor */
    ~CCallbackTimer()
        {
        Cancel();
        }
        
    /** Set whether sending holding events is currently enabled */
    void SetEnabled( TBool aEnabled )
        {
        iIsEnabled = aEnabled;
        // cancel in case hold timer is already running
        Cancel();
        }
        
    /** @return whether sending holding events is currently enabled */
    TBool IsEnabled() const
        {
        return iIsEnabled;
        }
        
    /** Start the timer. Calls CGestureHelperImpl::StartHoldingL upon completion */
    void Start()
        {
        // if sending hold events is disabled, do not ever start the hold timer, and 
        // hence hold events will never be triggered
        if ( iIsEnabled ) 
            {
            Cancel();
            After( iDelay );
            }
        }    
    void SetDelay(TInt aDelay) { iDelay = aDelay; }
    TInt GetDelay() { return iDelay; }
    
private:    
    /** Constructor */
    CCallbackTimer( CGestureHelperImpl& aHelper,
        CallbackFunctionL aCallbackFunctionL, TInt aDelay, TBool aIsEnabled )
            : CTimer( EPriorityUserInput - 1 ), // give higher priority to new pointer events with - 1
                iHelper( aHelper ), iCallbackFunctionL( aCallbackFunctionL ), 
                    iDelay( aDelay ), iIsEnabled( aIsEnabled ) 
        {
        }
        
    void RunL() // From CActive
        {
        (iHelper.*iCallbackFunctionL)();
        }

private:
    /// helper object that will be called back when timer is triggered
    CGestureHelperImpl& iHelper;
    /// Function in the iHelper object call 
    CallbackFunctionL iCallbackFunctionL;
    /// How long a time to wait befor calling back after Start()
    TInt iDelay;
    /// whether sending holding events is currently enabled
    TBool iIsEnabled;
    };

} // namespace GestureHelper

/** 
 * @return position from event. Use this instead of using aEvent direction to
 *         avoid accidentally using TPointerEvent::iPosition
 */
inline TPoint Position( const TPointerEvent& aEvent )
    {
    // use parent position, since the capturer is using full screen area,
    // and because the (Alfred) drag events are not local to visual even when
    // coming from the client
    
    return aEvent.iPosition;
    }

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl* CGestureHelperImpl::NewL( MGestureObserver& aObserver )
    {
    CGestureHelperImpl* self = new ( ELeave ) CGestureHelperImpl( aObserver );
    CleanupStack::PushL( self );
    self->iEventSender = CGestureEventSender::NewL( aObserver );
    self->iDoubleTapTimer = CCallbackTimer::NewL( *self, &CGestureHelperImpl::EmitFirstTapEvent, 
            KMaxTapDuration, EFalse ); // double tap is disabled by default
    self->iHoldingTimer = CCallbackTimer::NewL( *self, &CGestureHelperImpl::StartHoldingL, 
        KHoldDuration, EFalse ); // holding is enabled by default
    
    self->iLongTouchTimer = CCallbackTimer::NewL( *self, &CGestureHelperImpl::HandleLongTouch, 
            KLongTapDuration, ETrue ); // holding is enabled by default
    
    self->iGesture = new ( ELeave ) CGesture();
    self->iUnusedGesture = new ( ELeave ) CGesture();
    TInt tapLimit = Mm2Pixels(KFingerSize_mm) / 2;
    self->iEventFilter = new (ELeave) CGestureEventFilter(tapLimit);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl::CGestureHelperImpl( MGestureObserver& aObserver )
        : iObserver( aObserver )
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl::~CGestureHelperImpl()
    {
    delete iDoubleTapTimer;
    delete iHoldingTimer;
    delete iGesture;
    delete iPreviousTapGesture;
    delete iUnusedGesture;
    delete iLongTouchTimer;
    delete iEventFilter;
    delete iEventSender;
    }
    

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::SetHoldingEnabled( TBool aEnabled )
    {
    iHoldingTimer->SetEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::IsHoldingEnabled() const
    {
    return iHoldingTimer->IsEnabled();
    }

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::SetDoubleTapEnabled( TBool aEnabled )
    {
    iDoubleTapTimer->SetEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::IsDoubleTapEnabled() const
    {
    return iDoubleTapTimer->IsEnabled();
    }
    


// ----------------------------------------------------------------------------
// Reset state
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::Reset()
    {
    iHoldingTimer->Cancel();
    iLongTouchTimer->Cancel();
    iGesture->Reset();
    }

/** 
 * Helper function that calls Reset on the pointer to CGestureHelperImpl
 */
static void ResetHelper( TAny* aHelper )
    {
    static_cast< CGestureHelperImpl* >( aHelper )->Reset();
    }

// ----------------------------------------------------------------------------
// Handle a pointer event
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::HandlePointerEventL( const TPointerEvent& aEvent )
    {     
    TInt filterReason;
    SetLastEventTime();
    if (!iEventFilter->FilterDrag(aEvent, iLastEventTime, filterReason))
        {
        return noneAlf_HandlePointerEventL( aEvent );
        }
    else
        {
        /*
        TBuf<10> num;
        num.Num( filterReason );
        TBuf<128> str;
        str.AppendFormat(_L("Filter reason: %d"), filterReason);
        RFileLogger::Write( _L("gh"), _L("gh.txt"), EFileLoggingModeAppend, str);
        */
        return EFalse;
        }
    }


TBool CGestureHelperImpl::noneAlf_HandlePointerEventL( const TPointerEvent& aEvent)
    {
    switch ( aEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            HandleTouchDownL(aEvent);
            break;
            }
        case TPointerEvent::EDrag:
            {
            HandleMoveL(aEvent);
            break;
            }
        case TPointerEvent::EButton1Up:
            {
            if (KErrNone == AddPoint( aEvent ))
                {
                HandleTouchUp(aEvent);
                }
            else
                {
                EmitCancelEvent();
                }
            Reset();
            break;
            }
        default:
            break;
        }
    return ETrue;
    }

TBool CGestureHelperImpl::IsMovementGesture(TGestureCode aCode)
    {
    return (aCode == EGestureDrag || aCode == EGestureFlick || aCode == EGestureSwipeUp ||
            aCode == EGestureSwipeDown || aCode == EGestureSwipeRight || aCode == EGestureSwipeLeft);
    }

void CGestureHelperImpl::HandleLongTouch()
    {
    iDoubleTapTimer->Cancel();
    iGesture->SetLongTap(ETrue);
    iGesture->SetComplete();
    TPoint startPos = iGesture->StartPos();
    EmitEvent(*iGesture);
    iGesture->Reset();
    iGesture->AddPoint( startPos, GetLastEventTime() );
    }

void CGestureHelperImpl::HandleTouchDownL(const TPointerEvent& aEvent)
    {
    TGestureCode prevCode = iGesture->PreviousGestureCode();
    if (prevCode == EGestureStart) return;
    if (prevCode == EGestureDrag) 
        {
        iGesture->Reset();
        }
    AddPointL( aEvent );
    
    if (!iLongTouchTimer->IsActive())
        {
    iLongTouchTimer->Start();
        }
    if (!iDoubleTapTimer->IsActive())
        {
            EmitEvent( *iGesture );
        }
    }

void CGestureHelperImpl::HandleMoveL(const TPointerEvent& aEvent)
    {
    if (iGesture->IsLatestPoint( Position(aEvent))) return; // I'm not sure we need this
    //Cancel double tap time - it's neither tap nor double tap 
    iDoubleTapTimer->Cancel();
    iLongTouchTimer->Cancel();
    
    TBool isFirstPoint = IsIdle();
    
    AddPointL( aEvent );
    
    if (iPreviousTapGesture)
        {
        RecycleGesture(iPreviousTapGesture);
        }
    
    if (!isFirstPoint)
        {
        EmitEvent( *iGesture );
        }
    }

void CGestureHelperImpl::HandleTouchUp(const TPointerEvent& /*aEvent*/)
    {
    TGestureCode prevCode = iGesture->PreviousGestureCode();
    iLongTouchTimer->Cancel();
    iDoubleTapTimer->Cancel();
    TInt64 fromLastTouchUp = iLastEventTime.MicroSecondsFrom(iLastTouchUpTime).Int64();
    TInt64 fromLastDoubleTap = iLastEventTime.MicroSecondsFrom(iLastDoubleTapTime).Int64();
    /*
    TBuf<1024> str;
    str.AppendFormat(_L("fromLastTouchUp: %d, "), fromLastTouchUp);
    str.AppendFormat(_L("fromLastDoubleTap: %d, "), fromLastTouchUp);
    str.AppendFormat(_L("iPreviousTapGesture: %d, "), iPreviousTapGesture);
    RFileLogger::Write( _L("gh"), _L("gh.txt"), EFileLoggingModeAppend, str);
    */
    if ( prevCode == EGestureLongTap )
        {
        EmitReleasedEvent();
        }
    else if (IsMovementGesture(prevCode) || 
             !iDoubleTapTimer->IsEnabled() /* || !iGesture->IsTap()*/ ) 
        {
        iGesture->SetComplete();
        EmitEvent(*iGesture);
        }
    
    else 
        {
        if ( iPreviousTapGesture && 
         (fromLastTouchUp > KDoubleTapMinActivationInterval) &&       
         (fromLastTouchUp < KDoubleTapMaxActivationInterval) &&
         (fromLastDoubleTap > KDoubleTapIdleInterval))
            {
            // it's a double tap
            iLastTouchUpTime = iLastEventTime;
            iLastDoubleTapTime = iLastEventTime;
            EmitDoubleTapEvent();
            }
        else
            {
            // it's a first tap
            iLastTouchUpTime = iLastEventTime;
            if (iPreviousTapGesture)
                {
                   RecycleGesture(iPreviousTapGesture);
                }
                        
            iPreviousTapGesture = iGesture;
            iGesture = NewGesture();
            iDoubleTapTimer->Start(); 
            }
        }
    }



void CGestureHelperImpl::EmitDoubleTapEvent()
    {
    iPreviousTapGesture->SetDoubleTap();
    EmitFirstTapEvent();
    }


void CGestureHelperImpl::EmitReleasedEvent()
    {
    iGesture->SetComplete();
    iGesture->SetReleased();
    EmitEvent(*iGesture);
    }


// ----------------------------------------------------------------------------
// Is the helper idle?
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TBool CGestureHelperImpl::IsIdle() const
    {
    return iGesture->IsEmpty();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline void CGestureHelperImpl::AddPointL( const TPointerEvent& aEvent )
    {
    User::LeaveIfError( AddPoint( aEvent ) );
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TInt CGestureHelperImpl::AddPoint( const TPointerEvent& aEvent )
    {
    TPoint pos = Position ( aEvent );
    return iGesture->AddPoint( pos, GetLastEventTime() );
    }

// ----------------------------------------------------------------------------
// StartHoldingTimer
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::StartHoldingTimer( const TPointerEvent& aNewEvent )
    {
    if ( !( iGesture->IsHolding() ||
            iGesture->IsNearHoldingPoint( Position( aNewEvent ) ) ) )
        {
        // restart hold timer, since pointer has moved
        iHoldingTimer->Start();
        // Remember the point in which holding was started
        iGesture->SetHoldingPoint();
        }
    }

/** 
 * Helper function that calls ContinueHolding on the pointer to TGesture
 */
static void ContinueHolding( TAny* aGesture )
    {
    static_cast< CGesture* >( aGesture )->ContinueHolding();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::StartHoldingL()
    {
    // hold & tap event is specifically filtered out. Use case: in list fast 
    // scrolling activation (e.g. enhanced coverflow), tap & hold should not
    // start fast scroll. In addition, after long tap on start position,
    // drag and drag & hold swiping should emit normal swipe and swipe&hold
    // events. Therefore, tap & hold is not supported.
    __ASSERT_DEBUG( !iGesture->IsTap() && !iPreviousTapGesture, Panic( EGesturePanicIllegalLogic ) );
    
    // holding has just started, and gesture code should be provided to client.
    // set gesture state so that it produces a gesture code (other than drag)
    iGesture->StartHolding();
    
    // create an item in the cleanup stack that will set the gesture state
    // to holding-was-started-earlier state. NotifyL may leave, but the
    // holding-was-started-earlier state must still be successfully set,
    // otherwise, the holding gesture code will be sent twice
    CleanupStack::PushL( TCleanupItem( &ContinueHolding, iGesture ) );
    
    EmitEvent( *iGesture );
    
    // set holding state to "post holding"
    CleanupStack::PopAndDestroy( iGesture );
    }

// ----------------------------------------------------------------------------
// RecyclePreviousTapGesture
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::RecyclePreviousTapGesture( TAny* aSelf )
    {
    CGestureHelperImpl& self = *reinterpret_cast<CGestureHelperImpl*>( aSelf );
    self.RecycleGesture( self.iPreviousTapGesture );
    }

// ----------------------------------------------------------------------------
// Emit the remainder of the previous tap event (tap + released)
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitFirstTapEvent()
    {
    // when this function is called, a tap has turned out to _not_ be a double tap
    __ASSERT_DEBUG( IsDoubleTapEnabled(), Panic( EGesturePanicIllegalLogic ) );
    __ASSERT_DEBUG( iPreviousTapGesture, Panic( EGesturePanicIllegalLogic ) );
    
    iDoubleTapTimer->Cancel();
    CompleteAndEmit( *iPreviousTapGesture );
    RecycleGesture(iPreviousTapGesture);
     
    }

// ----------------------------------------------------------------------------
// EmitStartEventL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitStartEventL( const CGesture& aGesture )    
    {
    CGesture* startGesture = aGesture.AsStartEventLC();
    EmitEvent( *startGesture );
    CleanupStack::PopAndDestroy( startGesture );    
    }
    
// ----------------------------------------------------------------------------
// EmitCompletionEventsL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::CompleteAndEmit( CGesture& aGesture )
    {
    aGesture.SetComplete();
    // send gesture code if holding has not been started. If holding has 
    // been started, client has already received a "hold swipe left" e.g. event, in which
    // case don't another "swipe left" event
    if ( !aGesture.IsHolding() )
        {
        // if client leaves, the state is automatically reset.
        // In this case the client will not get the released event
        EmitEvent( aGesture ); 
        }
    
    // send an event that stylus was lifted
    aGesture.SetReleased();
    EmitEvent( aGesture ); 
    }
    
// ----------------------------------------------------------------------------
// EmitCancelEventL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitCancelEvent()
    {
    iDoubleTapTimer->Cancel();

    
    CGesture& gestureToCancel = iPreviousTapGesture ? *iPreviousTapGesture : *iGesture;
    gestureToCancel.SetCancelled();
    EmitEvent( gestureToCancel );
    RecycleGesture(iPreviousTapGesture);
    
    }

// ----------------------------------------------------------------------------
// Notify observer
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitEvent( const CGesture& aGesture )
    {
    // deallocation of the event is happening in CGestureEventSender::RunL() 
    TGestureEvent event;
    event.SetCode(const_cast<CGesture&>(aGesture).Code(EAxisBoth));
    event.SetCurrentPos(aGesture.CurrentPos());
    event.SetDistance(aGesture.Distance());
    event.SetStartPos(aGesture.StartPos());
    event.SetIsHolding(aGesture.IsHolding());
    event.SetSpeed(aGesture.Speed());
    iEventSender->AddEvent(event);
    }

// ----------------------------------------------------------------------------
// Return a fresh gesture from the gesture pool (pool of one gesture)
// ----------------------------------------------------------------------------
//
CGesture* CGestureHelperImpl::NewGesture()
    {
    __ASSERT_DEBUG( iUnusedGesture, Panic( EGesturePanicIllegalLogic ) ); // pool should no be empty
    
    iUnusedGesture->Reset();
    CGesture* freshGesture = iUnusedGesture;
    iUnusedGesture = NULL;
    return freshGesture;
    }

// ----------------------------------------------------------------------------
// Return a fresh gesture from the gesture pool (pool of one gesture)
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::RecycleGesture( CGesture*& aGesturePointer )
    {
    // only one object fits into the pool, and that should currently be enough
    // one pointer must be null, one non-null
    __ASSERT_DEBUG( !iUnusedGesture != !aGesturePointer, Panic( EGesturePanicIllegalLogic ) );
    if ( aGesturePointer )
        {
        iUnusedGesture = aGesturePointer;
        aGesturePointer = NULL;
        }
    }
