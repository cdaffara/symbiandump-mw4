/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include "alfdebugserver.h"
#include "alfdebugextensionconstants.h"
#include "alfdebuguid.h"
#include "alfdebug.h" // for TAlfDebugServerMeasurements

#include <uiacceltk/huienv.h>
#include <uiacceltk/huicommand.h>
#include <uiacceltk/huistatic.h>
#include <uiacceltk/huicontrol.h>
#include <uiacceltk/huicontrolgroup.h>
#include <uiacceltk/huitextvisual.h>
#include <uiacceltk/huieventhandler.h>
#include <uiacceltk/huievent.h>
#include <uiacceltk/huidisplay.h>
#include <uiacceltk/huiroster.h>
#include <uiacceltk/huigridlayout.h>

#include <implementationproxy.h>


_LIT8( KFrameRateTag, "FPS" );
_LIT8( KHeapTag, "heap" );
_LIT8( KContainerTag, "container" );
_LIT8( KIdleCheckerTag, "idle" );


enum TCustomEventCommands 
    {
    EUpdateFrameRate,
    EUpdateServerHeap
    };

// this is a helper class to know when the refreshing goes to idle
// assumptions: CHuiVisual::Changed() is called for every visual in the 
// shown control group. If something is drawn in the loop, the 
// CHuiVisual::ClearChanged() is called. If nothing is drawn, it is not called
class CIdleCheckVisual : public CHuiVisual
    {
public:
    static CIdleCheckVisual* AddNewL(
        CHuiControl& aOwnerControl,
        CHuiLayout* aParentLayout = 0);    
        
protected:
    CIdleCheckVisual(MHuiVisualOwner& aOwner);
    ~CIdleCheckVisual();
        
public: // from base classes
    void ClearChanged();
    TBool Changed() const;
    
// for async one shot
    mutable CAsyncCallBack* iAsyncCallBack;
    static TInt CallBackFunction(TAny* aAny);

    enum TState
        {
        EDisabled,
        EIdleNeedsReseting,
        EDrawning,
        EUpdatingText,
        EGoingToIdle1,
        EGoingToIdle2,
        EGoingToIdle3
        };
        
    mutable TState iState;
        
    };
    
    
// monitor control for showing the text visuals and receiving the events.
class CMonitorControl : public CHuiControl
    {
public:
    CMonitorControl(CHuiEnv& aEnv, CAlfDebugServer& aServerSession );
    void ConstructL();
    TBool OfferEventL(const THuiEvent& aEvent);
    
    void UpdateFrameRateTextL( TReal32 aFrameRate );
public:
    TInt iInitialVisualCount;
private:
    CAlfDebugServer& iServerSession;
    };    
    
    
    
    
    
    
CIdleCheckVisual* CIdleCheckVisual::AddNewL(
        CHuiControl& aOwnerControl,
        CHuiLayout* aParentLayout )
    {
    CIdleCheckVisual* visual = new (ELeave) CIdleCheckVisual(aOwnerControl);
    CleanupStack::PushL( visual );
    visual->ConstructL();
    aOwnerControl.AppendL(visual, aParentLayout);
    CleanupStack::Pop(visual);
    return visual;
    }
    
CIdleCheckVisual::CIdleCheckVisual(MHuiVisualOwner& aOwner) 
 : CHuiVisual( aOwner ), iState( EIdleNeedsReseting )
    {
    }
    
CIdleCheckVisual::~CIdleCheckVisual()
    {
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        }
    delete iAsyncCallBack;
    iAsyncCallBack = NULL;
    }
    
TBool CIdleCheckVisual::Changed() const
    {
    if ( iState == EIdleNeedsReseting )
        {
        CHuiStatic::FrameRate(); // just reset the framerate counter.
        iState = EDrawning;
        }
    
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        }
    else 
        {
        TCallBack callback( CallBackFunction, const_cast<CIdleCheckVisual*>(this) );
        iAsyncCallBack = new CAsyncCallBack( callback, CActive::EPriorityHigh );
        }  
        
    if ( iState != EDisabled )
        {
        iAsyncCallBack->CallBack();
        }
        
    return CHuiVisual::Changed();
    }
    
void CIdleCheckVisual::ClearChanged()
    {
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        }
    CHuiVisual::ClearChanged();
    }
    
TInt CIdleCheckVisual::CallBackFunction(TAny* aAny)
    {
    // if this function is called, we assume that the drawing has gone to idle
    CIdleCheckVisual* self = static_cast<CIdleCheckVisual*>( aAny );
    
    switch (self->iState)
        {
        case EIdleNeedsReseting:
            break;
        case EDrawning:
            // real drawing loop ended. update text which will cause some more drawing.
            TRAP_IGNORE(static_cast<CMonitorControl*>( &self->Owner() )->UpdateFrameRateTextL( CHuiStatic::FrameRate() ));
            self->iState = EUpdatingText;
            break;
        case EUpdatingText:
            // text update drawing ongoing
            self->iState = EGoingToIdle1;
            break;
        case EGoingToIdle1:
            // text update drawing ongoing
            self->iState = EGoingToIdle2;
            break;
        case EGoingToIdle2:
            // text update drawing ongoing
            self->iState = EGoingToIdle3;
            break;
        case EGoingToIdle3:
            // text update drawing ongoing
            self->iState = EIdleNeedsReseting;
            break;
        default:
            break;
        }
    
    return KErrNone;
    }


    
CMonitorControl::CMonitorControl(CHuiEnv& aEnv, CAlfDebugServer& aServerSession) : 
    CHuiControl(aEnv), 
    iServerSession( aServerSession )
    {
    }

void CMonitorControl::ConstructL()
    {
    CHuiControl::ConstructL();
    
    // create idle checker
    CIdleCheckVisual* idleChecker = CIdleCheckVisual::AddNewL( *this );
    idleChecker->SetTagL(KIdleCheckerTag);
    idleChecker->iState = CIdleCheckVisual::EDisabled; // activate when requested.
    
    // create container for the texts
    CHuiGridLayout* gridContainer = CHuiGridLayout::AddNewL( *this, 1 , 5 );
    gridContainer->SetTagL(KContainerTag);
    
    THuiBoxMetric padding( 
        THuiXYMetric( // top left
            THuiMetric( 0.5f , EHuiUnitMySize), // left
            THuiMetric( 0.2f , EHuiUnitMySize)  // top
            ),
        THuiXYMetric( // bottom right
            THuiMetric( 0.f , EHuiUnitMySize), // right
            THuiMetric( 0.1f , EHuiUnitMySize)// bottom
            ) );
            
    gridContainer->SetPadding( padding );
    
    
    // update this at the end
    iInitialVisualCount = VisualCount();
    }
    
    
    
TBool CMonitorControl::OfferEventL(const THuiEvent& aEvent)
    {
    TBool handled = EFalse;
    if (aEvent.IsCustomEvent() && aEvent.iParam == EUpdateFrameRate )
        {
        UpdateFrameRateTextL( CHuiStatic::FrameRate() );
        
        THuiCustomEventCommand command( EUpdateFrameRate, static_cast<MHuiEventHandler*>(this) );
        Env().Send( command, iServerSession.iFpsUpdatePeriod );        
        
        handled = ETrue;
        }
    else if ( aEvent.IsCustomEvent() && aEvent.iParam == EUpdateServerHeap )
        {
        CHuiTextVisual* textVisual = static_cast<CHuiTextVisual*>(FindTag( KHeapTag ) );
        
        User::Heap().Compress();
        
        TInt totalSize = 0;
        TInt usedSize = User::AllocSize( totalSize );
        TBuf<32> buffer;
        buffer.AppendNum( usedSize );
        buffer.Append( _L(" cells\n") );
        buffer.AppendNum( totalSize );
        buffer.Append( _L("B") );

        textVisual->SetTextL( buffer );
        
        THuiCustomEventCommand command( EUpdateServerHeap, static_cast<MHuiEventHandler*>(this) );
        Env().Send( command, iServerSession.iServerHeapUpdatePeriod );
        
        // make sure we are on top
        if ( Env().DisplayCount() )
            {
            CHuiDisplay& display = Env().PrimaryDisplay();
            display.Roster().ShowL( *ControlGroup() );
            }
            
        handled = ETrue;
        }
        
    return handled;
    }
    
void CMonitorControl::UpdateFrameRateTextL( TReal32 aFrameRate )
    {
    CHuiTextVisual* textVisual = static_cast<CHuiTextVisual*>(FindTag( KFrameRateTag ) );
        
    TBuf<32> buffer;
    buffer.AppendNum(aFrameRate,  TRealFormat( 4, 1 ) );
    buffer.Append( _L(" fps") );
    textVisual->SetTextL( buffer );
    
    // make sure we are on top
    if ( Env().DisplayCount() )
        {
        CHuiDisplay& display = Env().PrimaryDisplay();
        display.Roster().ShowL( *ControlGroup() );
        }
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
MAlfExtension* TAlfDebugHandler::CreateExtensionL(
        const TInt aObjectId, 
        const TDesC8& /*aInitialParams*/, 
        MAlfInterfaceProvider& aResolver )
    {
    MAlfExtension* result = NULL;
    switch( aObjectId )
        {
        case EAlfDebugExtensionCreateDebug:
            {
            result = new (ELeave) CAlfDebugServer( *aResolver.SharedHuiEnv() );
            break;
            }  

        default:
            User::Leave( KErrNotSupported );
        }
    return result;
    }
    
void TAlfDebugHandler::Release()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
// 
CAlfDebugServer::CAlfDebugServer( CHuiEnv& aEnv ) : iEnv( aEnv )
    {
    iFpsUpdatePeriod = 500;
    iServerHeapUpdatePeriod = 400;
    }
   
CAlfDebugServer::~CAlfDebugServer()
    {
    if ( iGroup )
        {
        iEnv.DeleteControlGroup( TInt(this) );
        }
    iGroup = NULL;
    }
    
void CAlfDebugServer::Release()
    {
    delete this;
    }
    
TAny* CAlfDebugServer::GetInterface( const THuiInterfaceSupport& /*aInterface*/ )
    {
    return NULL;
    }
    
void CAlfDebugServer::HandleCmdL( TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse )
    {
    switch (aCommandId)
        {
        case EAlfDebugCmdSetTimeFactor:
            {
            const TReal32* inParam = (TReal32*) aInputBuffer.Ptr();
            CHuiStatic::SetTimeFactor(*inParam);
            break;
            }
        case EAlfDebugCmdGetTimeFactor:
            {
            const TReal32 result = CHuiStatic::TimeFactor();
            TPckg<TReal32> resultPckg(result);
            aResponse = resultPckg;
            break;
            }
        case EAlfDebugCmdGetFrameCount:
            {    
            const TUint result = CHuiStatic::FrameCount();
            TPckg<TUint> resultPckg(result);
            aResponse = resultPckg;
            break;
            }
        case EAlfDebugCmdGetFrameRate:
            {    
            const TReal32 result = CHuiStatic::FrameRate();
            TPckg<TReal32> resultPckg(result);
            aResponse = resultPckg;
            
            // show on screen if needed
            if ( iControl && iFpsUpdatePeriod == -2 )
                {
                iControl->UpdateFrameRateTextL( result );
                }
            break;
            }
        case EAlfDebugCmdShowFrameRate:
            {
            const TInt* inParam = (TBool*) aInputBuffer.Ptr();
            ShowFrameRateL(*inParam);
            break;
            }
            
        case EAlfDebugCmdShowServerHeap:
            {
            const TBool* inParam = (TBool*) aInputBuffer.Ptr();
            ShowServerHeapL(*inParam);
            break;
            }
        
        case EAlfDebugCmdMeasure:
            {
            TAlfDebugServerMeasurements measurements;
            TPckg<TAlfDebugServerMeasurements> result(measurements);
            
            // Timestamp
            measurements.iTimeStamp = User::NTickCount();
            
            // Memory measures
            measurements.iServerCells = 
                User::AllocSize( measurements.iServerMemory );
            TInt dummy;
            measurements.iServerFree = User::Available( dummy );
            
            // Rendering measures
            measurements.iFrameCount = CHuiStatic::FrameCount();

            aResponse = result;
            }
            break;
            
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }
    
void CAlfDebugServer::CreateControlIfNeededL()
    {
    if ( !iGroup )
        { 
        iGroup = &iEnv.NewControlGroupL( TInt(this) );
        }
        
    if ( !iControl )
        {
        CMonitorControl* control = new (ELeave) CMonitorControl(iEnv, *this);
        CleanupStack::PushL( control );
        control->ConstructL();
        iGroup->AppendL( control );
        CleanupStack::Pop( control );
        iControl = control;
        }
        
    // make sure we are on top
    if ( iEnv.DisplayCount() )
        {
        CHuiDisplay& display = iEnv.PrimaryDisplay();
        display.Roster().ShowL( *iGroup );
        }
    }
    
    
void CAlfDebugServer::DeteteControlIfNeeded()
    {
    if ( iControl && iControl->VisualCount() == iControl->iInitialVisualCount )
        {
        iGroup->Remove( iControl );
        delete iControl;
        iControl = NULL;
        }
    
    if ( iGroup && iGroup->Count() == 0 )
        {
        iEnv.DeleteControlGroup( TInt(this) );
        iGroup = NULL;
        }
    }

void CAlfDebugServer::ShowFrameRateL( TInt aInterval )
    {
    CHuiTextVisual* textVisual = iControl ? static_cast<CHuiTextVisual*>(iControl->FindTag( KFrameRateTag ) ) : 0;
    if ( aInterval != KAlfDebugHideFraweRate )
        {
        CreateControlIfNeededL();
        
        if ( !textVisual )
            {
            textVisual = CHuiTextVisual::AddNewL(*iControl, static_cast<CHuiLayout*>(iControl->FindTag( KContainerTag )) );
            textVisual->SetTagL( KFrameRateTag );
            textVisual->SetColor( KRgbRed );
            }
        
        
        CIdleCheckVisual* idleChecker = static_cast<CIdleCheckVisual*>(iControl->FindTag( KIdleCheckerTag ));
        if ( aInterval == KAlfDebugShowFraweRateAfterDrawLoop ) // idle-idle
            {
            // use idle-idle
            idleChecker->iState = CIdleCheckVisual::EIdleNeedsReseting;
            }
        else if ( aInterval == KAlfDebugShowFraweRateWhenQueried ) // manual
            {
            iFpsUpdatePeriod = aInterval;
            idleChecker->iState = CIdleCheckVisual::EDisabled;
            }
        else
            {
            iFpsUpdatePeriod = aInterval;
            idleChecker->iState = CIdleCheckVisual::EDisabled;
            
            THuiCustomEventCommand command( EUpdateFrameRate, static_cast<MHuiEventHandler*>(iControl) );
            iEnv.Send( command, iFpsUpdatePeriod );
            }
        }
    else
        {
        if ( iControl )
            {
            if ( textVisual )
                {
                textVisual->RemoveAndDestroyAllD();
                }
            iEnv.CancelCommands( static_cast<MHuiEventHandler*>(iControl) , EHuiCommandTypeCustomEvent, EUpdateFrameRate );
            }
            
        DeteteControlIfNeeded();
        }
    }
    
void CAlfDebugServer::ShowServerHeapL( TBool aShow )
    {
    CHuiTextVisual* textVisual = iControl ? static_cast<CHuiTextVisual*>(iControl->FindTag( KHeapTag ) ) : 0;
    if ( aShow )
        {
        CreateControlIfNeededL();
        
        if ( !textVisual )
            {
            textVisual = CHuiTextVisual::AddNewL(*iControl, static_cast<CHuiLayout*>(iControl->FindTag( KContainerTag )));
            textVisual->SetTagL( KHeapTag );    
            textVisual->SetColor( KRgbRed );
            }
        
        THuiCustomEventCommand command( EUpdateServerHeap, static_cast<MHuiEventHandler*>(iControl) );
        iEnv.Send( command, iServerHeapUpdatePeriod );
        }
    else
        {
        if ( iControl )
            {
            if ( textVisual )
                {
                textVisual->RemoveAndDestroyAllD();
                }
            iEnv.CancelCommands( static_cast<MHuiEventHandler*>(iControl) , EHuiCommandTypeCustomEvent, EUpdateServerHeap );
            }
        DeteteControlIfNeeded();
        }
    }


// Global functions:

MAlfExtensionFactory* Instance()
    {
    TAlfDebugHandler* me = NULL;
    me = new TAlfDebugHandler;
    return me;
    }

const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    {{KAlfDebugExtensionImplementationId}, (TFuncPtr)Instance}
#else
    {{KAlfDebugExtensionImplementationId}, Instance}
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
    return ImplementationTable;
    }

