/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implements CHuiVisual class. Visuals are graphical entities 
*                visible on a display.
*
*/



#include "uiacceltk/HuiVisual.h"  // Class definition
#include "uiacceltk/HuiLayout.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiDrawing.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/huidropshadow.h"
#include "HuiRosterImpl.h"
#include "HuiFxEffect.h"
#include "HuiFxEffectParser.h"
#include "alflogger.h"
#include "HuiCmdBufferBrush.h"
#include "huicanvasrenderbuffer.h"

#include "huiextension.h"

struct CHuiVisual::THuiVisualPrivateData
    {
public: // Functions
    // Constructor: initialize the data
    THuiVisualPrivateData() : iTacticons(), iDropShadow(NULL)
        {
        iEffect = NULL;
        iEffectParser = NULL;
        }
        
    // Destructor: destruct the data
    ~THuiVisualPrivateData()
        {
        iTacticons.Close();
        delete iDropShadow;
        delete iEffectParser;
        delete iEffect;
        }
    
    // structs
    struct TTacticon
        {
        TInt iFeedbackType;
        TInt iEventType;
        };
    
    // Data
    RArray<TTacticon> iTacticons;
    CHuiDropShadow* iDropShadow; 
    CHuiFxEffect* iEffect;
    CHuiFxEffectParser* iEffectParser;
    MHuiEffectable *iEffectable;
    CHuiCanvasRenderBuffer* iStoredRenderBuffer;
    TBool iStoredRenderBufferModificationsEnabled;
    TBool iFreezed;
    };
class CHuiEffectable : public CBase, public MHuiEffectable
    {
public: // from MHuiEffectable
    CHuiEffectable(CHuiVisual *aVisual) : iVisual(aVisual) { }
    void EffectSetEffect(CHuiFxEffect* aEffect);
    TReal32 EffectOpacityTarget() const;
    void EffectSetOpacity(TReal32 aOpacity);
    void EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const;
    THuiRealRect EffectDisplayRect() const __SOFTFP;
    void SetLoadingEffect(TBool aLoading);    
    void EffectSetSource( TBool aIsInput1 );
    TBool EffectGetSource() const;

private:
    CHuiVisual *iVisual;
    TBool iIsInput1;
    };
void CHuiEffectable::EffectSetEffect(CHuiFxEffect* aEffect)
    {
    iVisual->SetEffect(aEffect);
    }
void CHuiEffectable::EffectSetSource( TBool aIsInput1 )
    {
    iIsInput1 = aIsInput1;
    }
TBool CHuiEffectable::EffectGetSource() const
    {
    return iIsInput1;
    }

TReal32 CHuiEffectable::EffectOpacityTarget() const
    {
    return iVisual->iOpacity.Target();
    }

void CHuiEffectable::EffectSetOpacity(TReal32 aOpacity)
    {
    iVisual->iOpacity.Set( aOpacity );
    }

void CHuiEffectable::EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const
    {
    iVisual->DrawSelf(aGc, aDisplayRect);
    }

THuiRealRect CHuiEffectable::EffectDisplayRect() const __SOFTFP
    {
    return iVisual->DisplayRect();
    }

void CHuiEffectable::SetLoadingEffect(TBool aLoading)
    {
    iVisual->SetLoadingEffect(aLoading);
    }


/// Descriptor that is returned when the tag is queried and no tag has been
/// been defined for a visual.
_LIT8(KHuiVisualUndefinedTag, "");

EXPORT_C MHuiVisualOwner::~MHuiVisualOwner()
    {
    }

EXPORT_C CHuiCanvasRenderBuffer *CHuiVisual::StoredRenderBuffer() const
    {
    return iVisualData->iStoredRenderBuffer;
    }
EXPORT_C void CHuiVisual::SetStoredRenderBuffer(CHuiCanvasRenderBuffer *aRenderBuffer)
    {
    if (iVisualData->iStoredRenderBufferModificationsEnabled)
        {
        if (iVisualData->iStoredRenderBuffer)
            {
            iVisualData->iStoredRenderBuffer->UnInitialize();
            }
        delete iVisualData->iStoredRenderBuffer;
        iVisualData->iStoredRenderBuffer = aRenderBuffer;
        }
    else
        {
        if (aRenderBuffer != iVisualData->iStoredRenderBuffer)
            {
#ifdef _DEBUG
            RDebug::Printf("CHuiVisual::SetStoredRenderBuffer - Warning: Not enabled (%x / %x)", 
                aRenderBuffer, iVisualData->iStoredRenderBuffer);
#endif
            // Ownership was passed, but we do not need it.
            delete aRenderBuffer;
            aRenderBuffer = NULL;
            }
        }
    }
EXPORT_C void CHuiVisual::SetStoredRenderBufferModificationsEnabled(TBool aEnabled)
    {
    iVisualData->iStoredRenderBufferModificationsEnabled = aEnabled;
    }

EXPORT_C void CHuiVisual::SetFreezeState(TBool aEnabled)
    {
    iVisualData->iFreezed = aEnabled;
    }
TBool CHuiVisual::Freezed() const
{
    return iVisualData->iFreezed;
}

EXPORT_C CHuiVisual* CHuiVisual::AddNewL(CHuiControl& aOwnerControl,
                                         CHuiLayout* aParentLayout)
    {
    CHuiVisual* visual = STATIC_CAST(CHuiVisual*,
        aOwnerControl.AppendVisualL(EHuiVisualTypeVisual, aParentLayout));
    return visual;
    }


EXPORT_C CHuiVisual::CHuiVisual(MHuiVisualOwner& aOwner)
        : iOpacity(1.f),
          iOwner(&aOwner), 
          // Set the maximum size to unlimited.
          iMaxSize(TSize(KMaxTInt, KMaxTInt)),
          iPadding(0), 
          iDisplayRectFrameNumber(KMaxTUint),
          iDisplayRectTargetFrameNumber(KMaxTUint)
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    iOpacity.SetStyle(EHuiTimedValueStyleLinear);
    }


EXPORT_C void CHuiVisual::ConstructL()
    {
    iVisualData = new (ELeave) THuiVisualPrivateData;
    iVisualData->iEffectable = new CHuiEffectable(this);
    iVisualData->iStoredRenderBuffer = NULL;
    }


EXPORT_C CHuiVisual::~CHuiVisual()
    {
    // Cancel scheduled commands for this visual.
    Env().CancelCommands(this);

	// Update previous dirty rect to relevant displays so that the visual can be erased
    if(Display())
        {        
        CHuiRoster& roster = Display()->Roster();
		// Check if this roster is shared between several displays and add previous dirty to all 
		// of them
    	for(TInt i = 0; i < Env().Displays().Count(); ++i)
        	{ 
        	CHuiDisplay* display = &Env().Display(i);
        	if (&(display->Roster()) == &roster)
        		{
				const TRect& previousDirtyRect = iPreviousDrawn;
				TRect empty;
        		display->CombineAndAddDirtyRegion(previousDirtyRect, empty);
        		}
        	}	
        }

    // If this is a root-level visual, it must be hidden on the display.
    if(iDisplay)
        {
        iDisplay->Roster().HideVisual(this);
        }

    // If the visual is still attached to a layout, it is time to remove it.
    if(iLayout)
        {
        iLayout->Remove(this);
        }
        
    // If the visual is attached to some visual as external content visual,
    // remove the link from the parent visual
    if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
    	{
    	TRAP_IGNORE(CHuiStatic::Env().Display(0).RosterImpl().RemoveExternalContentVisualFromParentL(this));
    	}

    delete iBrushes;
    delete iTag;
    delete iTransform;
    delete iVisualData->iEffectable;
    if (iVisualData->iStoredRenderBuffer)
        {
        iVisualData->iStoredRenderBuffer->UnInitialize();
        }
    delete iVisualData->iStoredRenderBuffer;
    delete iVisualData;
    
    iOwner = NULL;
    iDisplay = NULL;
    iLayout = NULL;

    HUI_PROBE_REPORT_DESTRUCTED
    }


EXPORT_C void CHuiVisual::RemoveAndDestroyAllD()
    {
    ASSERT(iOwner != NULL);
    iOwner->VisualDestroyed(*this);
    delete this;
    }


EXPORT_C CHuiLayout* CHuiVisual::Layout() const
    {
    return iLayout;
    }


void CHuiVisual::SetLayout(CHuiLayout* aLayout)
    {
    iLayout = aLayout;
    }


EXPORT_C CHuiEnv& CHuiVisual::Env() const
    {
    return Owner().Env();
    }


EXPORT_C CHuiSkin& CHuiVisual::Skin() const
    {
    return Owner().Env().Skin();
    }


EXPORT_C CHuiDisplay* CHuiVisual::Display() const
    {
    const CHuiVisual* iter = this;

    while(iter)
        {
        if(iter->iDisplay)
            {
            return iter->iDisplay;
            }
        iter = iter->Layout();
        }

    // Not shown on any display.
    return NULL;
    }


void CHuiVisual::SetDisplay(CHuiDisplay* aDisplay)
    {
    iDisplay = aDisplay;
    }


EXPORT_C void CHuiVisual::SetFlag(THuiVisualFlags aFlag)
    {
    SetFlags(aFlag);
    }


EXPORT_C void CHuiVisual::SetFlags(TInt aAnyFlags)
    {
    const TInt oldFlags = iFlags;
    iFlags |= aAnyFlags;

    if(!CHuiStatic::Renderer().Allows(EHuiRenderPluginAllow3DProjection))
        {
        // The rendering plugin does not support 3D projection.
        ClearFlag(EHuiVisualFlag3DProjection);
        }
        
    HandleFlagsChanged( oldFlags );
    }


EXPORT_C void CHuiVisual::ClearFlag(THuiVisualFlags aFlag)
    {
    ClearFlags( aFlag );
    }


EXPORT_C void CHuiVisual::ClearFlags(TInt aAnyFlags)
    {
    const TInt oldFlags = iFlags;
    iFlags &= ~aAnyFlags;
    
    HandleFlagsChanged( oldFlags );
    }


EXPORT_C MHuiVisualOwner& CHuiVisual::Owner() const
    {
    ASSERT(iOwner != NULL);
    return *iOwner;
    }
    
    
EXPORT_C void CHuiVisual::SetOwner(MHuiVisualOwner& aOwner)
    {
    iOwner = &aOwner;
    }
    
    
EXPORT_C TInt CHuiVisual::Count() const
    {
    return 0;
    }


EXPORT_C CHuiVisual& CHuiVisual::Visual(TInt /*aIndex*/) const
    {
    USER_INVARIANT();
    return *(CHuiVisual*)this; // never reached
    }


EXPORT_C void CHuiVisual::SetOrigin(THuiVisualHOrigin aHoriz, THuiVisualVOrigin aVert)
    {
    ClearFlags(EHuiVisualFlagHOriginCenter | EHuiVisualFlagHOriginRight);
    switch(aHoriz)
        {
        case EHuiVisualHOriginCenter:
            SetFlag(EHuiVisualFlagHOriginCenter);
            break;

        case EHuiVisualHOriginRight:
            SetFlag(EHuiVisualFlagHOriginRight);
            break;

        default:
            break;
        }

    ClearFlags(EHuiVisualFlagVOriginCenter | EHuiVisualFlagVOriginBottom);
    switch(aVert)
        {
        case EHuiVisualVOriginCenter:
            SetFlag(EHuiVisualFlagVOriginCenter);
            break;

        case EHuiVisualVOriginBottom:
            SetFlag(EHuiVisualFlagVOriginBottom);
            break;

        default:
            break;
        }
    }


EXPORT_C THuiVisualHOrigin CHuiVisual::HorizontalOrigin() const
    {
    if(iFlags & EHuiVisualFlagHOriginCenter)
        {
        return EHuiVisualHOriginCenter;
        }
    if(iFlags & EHuiVisualFlagHOriginRight)
        {
        return EHuiVisualHOriginRight;
        }
    return EHuiVisualHOriginLeft;
    }


EXPORT_C THuiVisualVOrigin CHuiVisual::VerticalOrigin() const
    {
    if(iFlags & EHuiVisualFlagVOriginCenter)
        {
        return EHuiVisualVOriginCenter;
        }
    if(iFlags & EHuiVisualFlagVOriginBottom)
        {
        return EHuiVisualVOriginBottom;
        }
    return EHuiVisualVOriginTop;
    }


EXPORT_C void CHuiVisual::AlignByOrigin(THuiRealPoint& aPointInPixels, TBool aUndo) const
    {
    TInt factor = (aUndo? -1 : 1);
    THuiRealPoint itemSize(0.f, 0.f); 
    
    TInt horizOrig = HorizontalOrigin();
    TInt vertOrig = VerticalOrigin();

    // The position of the origin depends whether the coordinates are mirrored.    
    if(LocalBaseUnit().iX.iMagnitude < 0)
        {
        if(horizOrig == EHuiVisualHOriginLeft)
            {
            horizOrig = EHuiVisualHOriginRight;
            }
        else if(horizOrig == EHuiVisualHOriginRight)
            {
            horizOrig = EHuiVisualHOriginLeft;
            }
        else
            {
            // For PC-lint
            }
        }
    if(LocalBaseUnit().iY.iMagnitude < 0)
        {
        if(vertOrig == EHuiVisualVOriginTop)
            {
            vertOrig = EHuiVisualVOriginBottom;
            }
        else if(vertOrig == EHuiVisualVOriginBottom)
            {
            vertOrig = EHuiVisualVOriginTop;
            }
        else
            {
            // For PC-lint
            }
        }
    
    if(horizOrig != EHuiVisualHOriginLeft ||
        vertOrig != EHuiVisualVOriginTop)
        {
        // Need the size of the visual for aligning to non-top/left corner.
        itemSize = LocalPointInPixels(Size().RealNow());
        }
    
    switch(horizOrig)
        {
        case EHuiVisualHOriginCenter:
            aPointInPixels.iX -= factor * itemSize.iX / 2.f;
            break;

        case EHuiVisualHOriginRight:
            aPointInPixels.iX -= factor * itemSize.iX;
            break;

        default:
            break;
        }

    switch(vertOrig)
        {
        case EHuiVisualVOriginCenter:
            aPointInPixels.iY -= factor * itemSize.iY / 2.f;
            break;

        case EHuiVisualVOriginBottom:
            aPointInPixels.iY -= factor * itemSize.iY;
            break;

        default:
            break;
        }
    }


EXPORT_C const THuiTimedPoint& CHuiVisual::Pos() const
    {
    return iPos;
    }


EXPORT_C const THuiTimedPoint& CHuiVisual::Size() const
    {
    return iSize;
    }


EXPORT_C void CHuiVisual::SetPos(const THuiRealPoint& aPos, TInt aTransitionTime)
    {
    iPos.iX.Set(aPos.iX, aTransitionTime);
    iPos.iY.Set(aPos.iY, aTransitionTime);
    }


EXPORT_C void CHuiVisual::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    iSize.iX.Set(aSize.iWidth < 0 ? aSize.iWidth *-1 : aSize.iWidth, aTransitionTime);
    iSize.iY.Set(aSize.iHeight < 0 ? aSize.iHeight *-1 : aSize.iHeight, aTransitionTime);
    }


EXPORT_C void CHuiVisual::SetRect(const THuiRealRect& aRect, TInt aTransitionTime)
    {
    SetPos(aRect.iTl, aTransitionTime);
    SetSize(aRect.Size(), aTransitionTime);
    }


// deprecated
EXPORT_C void CHuiVisual::Move(const TPoint& aOffset, TInt aTransitionTime)
    {
	Move(PixelPointInLocal(THuiRealPoint(aOffset), EHuiReferenceStateNow), aTransitionTime);  
	}


EXPORT_C void CHuiVisual::Move(const THuiRealPoint& aOffset, TInt aTransitionTime)
    {
    iPos.iX.Set(iPos.iX.Target() + aOffset.iX, aTransitionTime);
    iPos.iY.Set(iPos.iY.Target() + aOffset.iY, aTransitionTime);
    }


EXPORT_C void CHuiVisual::Mimic(const CHuiVisual& aVisual, TInt aTransitionTime)
    {
    SetPos(aVisual.iPos.RealTarget(), aTransitionTime);
    SetSize(aVisual.iSize.RealTarget().AsSize(), aTransitionTime);
    }


// deprecated
EXPORT_C void CHuiVisual::SetCenteredPosAndSize(const TPoint& aCenterPoint,
                                                const TSize& aSize,
                                                TInt aTransitionTime)
    {
    THuiRealPoint visualSize = PixelPointInLocal(THuiRealPoint(aSize.iWidth, aSize.iHeight),
    	EHuiReferenceStateNow);
    SetCenteredPosAndSize(PixelPointInLocal(THuiRealPoint(aCenterPoint), EHuiReferenceStateNow), 
    	visualSize.AsSize(), aTransitionTime);
    }


EXPORT_C void CHuiVisual::SetCenteredPosAndSize(const THuiRealPoint& aCenterPoint,
                                                const THuiRealSize& aSize,
                                                TInt aTransitionTime)
    {
    SetPos(THuiRealPoint(aCenterPoint.iX - aSize.iWidth/2,
    	aCenterPoint.iY - aSize.iHeight/2), aTransitionTime);
    SetSize(aSize, aTransitionTime);
    }


EXPORT_C void CHuiVisual::UpdateChildrenLayout(TInt /*aTransitionTime*/)
    {
    }


void CHuiVisual::ReportLayoutUpdate()
    {
    // Notify owner, if requested.
    if(iFlags & EHuiVisualFlagLayoutUpdateNotification)
        {
        Owner().VisualLayoutUpdated(*this);
        }
    }

// deprecated
EXPORT_C void CHuiVisual::SetMinSize(const TSize& aMinSize)
    {
    THuiRealPoint minPoint = PixelPointInLocal(THuiRealPoint(aMinSize.iWidth, aMinSize.iHeight),
    	EHuiReferenceStateNow);
    SetMinSize(minPoint.AsSize());
    }

EXPORT_C void CHuiVisual::SetMinSize(const THuiRealSize& aMinSize)
    {
    iMinSize = aMinSize;
    }
    
// deprecated
EXPORT_C TSize CHuiVisual::MinSize() const
    {
    THuiRealSize minSize = MinSizeInBaseUnits();
    THuiRealPoint minPoint = LocalPointInPixels(THuiRealPoint(minSize.iWidth, minSize.iHeight),
    	EHuiReferenceStateNow);
    // @todo: Should we round this value?
    return (TSize(minPoint.AsSize()));
    }

EXPORT_C THuiRealSize CHuiVisual::MinSizeInBaseUnits() const __SOFTFP 
    {
    return iMinSize;
    }


// deprecated
EXPORT_C void CHuiVisual::SetMaxSize(const TSize& aMaxSize)
    {
    THuiRealPoint maxPoint = PixelPointInLocal(THuiRealPoint(aMaxSize.iWidth, aMaxSize.iHeight),
    	EHuiReferenceStateNow);
    SetMaxSize(maxPoint.AsSize());
    }

EXPORT_C void CHuiVisual::SetMaxSize(const THuiRealSize& aMaxSize)
    {
    iMaxSize = aMaxSize;
    }


// deprecated
EXPORT_C TSize CHuiVisual::MaxSize() const
    {
    THuiRealSize maxSize = MaxSizeInBaseUnits();
    THuiRealPoint maxPoint = LocalPointInPixels(THuiRealPoint(maxSize.iWidth, maxSize.iHeight),
    	EHuiReferenceStateNow);
    // @todo: Should we round this value?
    return (TSize(maxPoint.AsSize()));
    }

EXPORT_C THuiRealSize CHuiVisual::MaxSizeInBaseUnits() const __SOFTFP
    {
    return iMaxSize;
    }


EXPORT_C void CHuiVisual::SetClipping(TBool aClipping)
    {
    if(aClipping)
        {
        SetFlag(EHuiVisualFlagClipping);
        }
    else
        {
        ClearFlag(EHuiVisualFlagClipping);
        }
    }


EXPORT_C TBool CHuiVisual::Clipping() const
    {
    return (iFlags & EHuiVisualFlagClipping) != 0;
    }


EXPORT_C THuiRealRect CHuiVisual::DisplayRect() const __SOFTFP
    {
    // if this visual is wserv owned, we can take some shortcuts
    // when calculating the displayrect (ie. none of the "advanced"
    // functionality is used
    if (iFlags & EHuiVisualFlagWserv)
        {
        TReal x = iPos.iX.iInterpolationStartValue;
        TReal y = iPos.iY.iInterpolationStartValue;
        TReal width = iSize.iX.iInterpolationStartValue;
        TReal height = iSize.iY.iInterpolationStartValue;
        
        const CHuiVisual* iter = this;
        while(iter)
            {
            // Move up in the tree.
            iter = iter->iLayout;
            if(iter)
                {
                x+=iter->iPos.iX.iInterpolationStartValue;
                y+=iter->iPos.iY.iInterpolationStartValue;
                }
            }

        iDisplayRect.iTl.iX = x;
        iDisplayRect.iTl.iY = y;
        iDisplayRect.iBr.iX = x+width;
        iDisplayRect.iBr.iY = y+height;
        return iDisplayRect;
        }
    // Recalculate and cache as necessary. 
    TBool recalculationNeeded = ETrue;
    
    TUint frameno = 0; // CHuiStatic::FrameCount() is called before used
    const TBool calledDuringDraw = Env().CurrentDisplay() ? ETrue : EFalse;
    
    if ( calledDuringDraw ) // before the draw, the cache is not updated
        {
        frameno = CHuiStatic::FrameCount();
        if ( frameno == iDisplayRectFrameNumber )
            {
            recalculationNeeded = EFalse;
            }
        }
    
    if( recalculationNeeded )
	    {
	    THuiRealRect rect(LocalToDisplay(Pos().RealNow()),
	                      LocalPointInPixels(Size().RealNow()).AsSize());

        if (rect.iTl.iX > rect.iBr.iX)
            {
            TReal tempX = rect.iTl.iX;
            rect.iTl.iX = rect.iBr.iX;
            rect.iBr.iX = tempX;
            }
  
        if (rect.iTl.iY > rect.iBr.iY)
            {
            TReal tempY = rect.iTl.iY;
            rect.iTl.iY = rect.iBr.iY;
            rect.iBr.iY = tempY;
            }
	                      
	    iDisplayRect = rect;
	    
	    if ( calledDuringDraw ) // update frame number only during draw
            {
	        iDisplayRectFrameNumber = frameno;
            }
	    }
    return iDisplayRect;
    }

THuiRealRect CHuiVisual::CachedDisplayRect() const
    {
    return iDisplayRect;    
    }


EXPORT_C THuiRealRect CHuiVisual::DisplayRectTarget() const __SOFTFP
    {
    // Calculate effect display area if the visual has an attached effect
    // For OpenVG nothing is needed as the area is the final position,
    // not something that happens during transition.
    // Recalculate and cache as necessary. 
    TBool recalculationNeeded = ETrue;
    
    TUint frameno = 0; // CHuiStatic::FrameCount() is called before used
    const TBool calledDuringDraw = Env().CurrentDisplay() ? ETrue : EFalse;
    
    if ( calledDuringDraw ) // before the draw, the cache is not updated
        {
        frameno = CHuiStatic::FrameCount();
        if ( frameno == iDisplayRectTargetFrameNumber )
            {
            recalculationNeeded = EFalse;
            }
        }

    if( recalculationNeeded )
	    {
		THuiRealRect rect(LocalToDisplay(Pos().RealTarget(), EHuiReferenceStateTarget),
		                  LocalPointInPixels(Size().RealTarget(), EHuiReferenceStateTarget).AsSize());

        if (rect.iTl.iX > rect.iBr.iX)
            {
            TReal32 tempX = rect.iTl.iX;
            rect.iTl.iX = rect.iBr.iX;
            rect.iBr.iX = tempX;
            }
  
        if (rect.iTl.iY > rect.iBr.iY)
            {
            TReal32 tempY = rect.iTl.iY;
            rect.iTl.iY = rect.iBr.iY;
            rect.iBr.iY = tempY;
            }
        
        iDisplayRectTarget = rect;
        
        if ( calledDuringDraw ) // update frame number only during draw
            {
	        iDisplayRectTargetFrameNumber = frameno;
            }
	    }
	return iDisplayRectTarget;
    }


EXPORT_C TReal32 CHuiVisual::EffectiveOpacity() const __SOFTFP
    {
    TReal32 opacity = iOpacity.Now();

    // Controls in control groups (i.e., normally all controls)
    // get a common opacity from the group.
    if(Owner().ControlGroup())
        {
        opacity *= Owner().ControlGroup()->iOpacity.Now();
        }

    CHuiVisual* iter = Layout();

    // Factor in all the opacities of the parent visuals.
    while(iter)
        {
        opacity *= iter->iOpacity.Now();
        if(opacity <= 0)
            {
            // Fully transparent;
            return 0;
            }
        iter = iter->Layout();
        }

    return opacity;
    }


EXPORT_C TBool CHuiVisual::PrepareDrawL()
    {
    // The visual base class doesn't prepare anything.	
    return ETrue;
    }


EXPORT_C void CHuiVisual::Draw(CHuiGc& aGc) const
    {
    if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
    	{
   	    // This is used only as external content visual. Return now if we are not currently drawing
   	    // external content.
    	if (!Display() || !Display()->RosterImpl().IsDrawingExternalContent())
    		{
			return;
    		}
    	}

    TReal32 opacity = EffectiveOpacity();
    if(opacity <= 0)
        {
        // The visual is invisible because it's fully transparent.
        return;
        }

    // This is the currently effective area on the display used
    // by the visual.
    THuiRealRect displayRect = DisplayRect();

    EnterLocalProjection(aGc, ETrue, 0.f, &displayRect);

    // Apply local transformation.
    Transform(aGc, ETrue, &displayRect);

    // Draw background brushes (under the content).
    DrawBrushes(aGc, EHuiBrushLayerBackground);

    // Clip contents to visual rectangle.
    if(Clipping())
        {
        // Visual content clipping is affected by transformation.
        aGc.PushClip();
        aGc.Clip(displayRect);
        aGc.Enable(CHuiGc::EFeatureClipping);
        }
    if( iVisualData->iEffect )
        {
        // Can we draw effect
        TBool canUseEffectDrawing = Effect() && !LoadingEffect();
        
        // Flag to know what we did
        TBool didDrawEffect = EFalse;
        
        if (canUseEffectDrawing)
            {
            // Note that EHuiVisualFlagOpaqueHint improves performance a lot in cached effect drawing 
            TBool transparent = !(Flags() & EHuiVisualFlagOpaqueHint);
            TBool refreshCache = Changed();                
            didDrawEffect = iVisualData->iEffect->CachedDraw(aGc, displayRect, refreshCache, !transparent);
            }

        if (!didDrawEffect) 
            {
            // Draw the visual instead if the effect failed
            DrawSelf(aGc, displayRect);
            }
        else
            {
            // Effect was drawn, OpenGLES/OpenVG may be in different state than what Gc thinks so we restore it.
            aGc.RestoreState();
            }
        }
    else if ( !LoadingEffect() )
        {
        // Draw the visual itself.
        // But don't draw if we are just building the effect - 
        // We may get a view of the visual with its default values though it may
        // want to start from size 0 or whatever
        DrawSelf(aGc, displayRect);
        }
    else
        {
        // just to keep codescanners and other stupid tools happy
        }

    if(Clipping())
        {
        aGc.PopClip();
        }

    // Draw foreground brushes (over the content).
    DrawBrushes(aGc, EHuiBrushLayerForeground);

    // Undo local transformation.
    Transform(aGc, EFalse);
    EnterLocalProjection(aGc, EFalse);

    }


EXPORT_C void CHuiVisual::DrawBrushes(CHuiGc& aGc, THuiBrushLayer aLayer) const
    {
    if(iBrushes)
        {
        iBrushes->ActivateBrushGuide(this);
        iBrushes->Draw(aLayer, aGc, *this);
        iBrushes->ActivateBrushGuide(NULL);        
        }
    }


EXPORT_C void CHuiVisual::DrawSelf(CHuiGc& /*aGc*/,
                                   const TRect& /*aDisplayRect*/) const
    {
    // The visual base class doesn't draw a3nything.
    }
    
    
EXPORT_C void CHuiVisual::BoxMetricToPixelRect(const THuiBoxMetric& aBox, 
        THuiRealRect& aRectInPixels) const
    {
    aRectInPixels.iTl = MetricToPixels(THuiXYMetric(aBox.iLeft, aBox.iTop));
    aRectInPixels.iBr = MetricToPixels(THuiXYMetric(aBox.iRight, aBox.iBottom));
    }
    
    
/*EXPORT_C void CHuiVisual::BoxMetricToLocalRect(const THuiBoxMetric& aBox,
        THuiRealRect& aLocalRect) const
    {
    THuiRealRect pixelRect;
    BoxMetricToPixelRect(aBox, pixelRect);
    aLocalRect.iTl = PixelPointInLocal(pixelRect.iTl);
    aLocalRect.iBr = PixelPointInLocal(pixelRect.iBr);
    }*/


EXPORT_C THuiRealPoint CHuiVisual::LocalPointInPixels(const THuiRealPoint& aLocalPoint) const __SOFTFP
    {
    return LocalPointInPixels(aLocalPoint, EHuiReferenceStateNow);
    }
    
    
EXPORT_C THuiRealPoint CHuiVisual::PixelPointInLocal(const THuiRealPoint& aPixelPoint) const __SOFTFP
    {
    return PixelPointInLocal(aPixelPoint, EHuiReferenceStateNow);
    }
    
    
THuiRealPoint CHuiVisual::PixelPointInUnits(const THuiRealPoint& aPixelPoint, 
    const THuiXYMetric& aUnit, THuiReferenceState aReferenceState ) const __SOFTFP
    {
    THuiRealPoint result(0.f, 0.f);
    THuiXYMetric metric = aUnit;
    PixelsToMetric(aPixelPoint, metric, aReferenceState); // sets X and Y lengths

    if(aUnit.iX.iMagnitude != 0)
        {
        result.iX = metric.iX.iMagnitude / aUnit.iX.iMagnitude;
        }
    else
        {
        result.iX = 0;
        }

    if(aUnit.iY.iMagnitude != 0)
        {
        result.iY = metric.iY.iMagnitude / aUnit.iY.iMagnitude;
        }
    else
        {
        result.iY = 0;
        }

    return result;
    }

EXPORT_C THuiRealPoint CHuiVisual::MetricReferenceInPixels(const THuiXYMetric& aMetric) const __SOFTFP
    {
    return MetricReferenceInPixels(aMetric, EHuiReferenceStateNow);
    }


EXPORT_C THuiRealPoint CHuiVisual::MetricReferenceInPixels(const THuiXYMetric& aMetric, 
                                                           THuiReferenceState aReferenceState) const __SOFTFP
    {
    THuiRealPoint refSizePx(0.f, 0.f);
    
    // if weights are used in this context, treat them as normalized units
    TInt xUnit = (aMetric.iX.iUnit == EHuiUnitWeight) ? EHuiUnitNormalized : aMetric.iX.iUnit;
    TInt yUnit = (aMetric.iY.iUnit == EHuiUnitWeight) ? EHuiUnitNormalized : aMetric.iY.iUnit;
        
    // For normalized units, the reference is the parent size in pixels.
    if(xUnit == EHuiUnitNormalized || yUnit == EHuiUnitNormalized ||
       xUnit == EHuiUnitParentWidth || yUnit == EHuiUnitParentWidth ||
       xUnit == EHuiUnitParentHeight || yUnit == EHuiUnitParentHeight)        
       {
        THuiRealPoint sizePx(0.f, 0.f);
        
        // Determine parent size in pixels.
        if(iLayout)
            {
            // Recursive fun. We don't want to do this more than once.
            // (LocalPointInPixels => MetricsToPixels => MetricReferenceInPixels)
            sizePx = iLayout->LocalPointInPixels(
                            aReferenceState == EHuiReferenceStateTarget? iLayout->Size().RealTarget() :
                            iLayout->Size().RealNow(), aReferenceState);
  	        }
        else if(Display())
            {
            // @todo Jaakko please code review this change!
//            sizePx = Display()->Size().AsPoint();
            sizePx = Display()->VisibleArea().Size().AsPoint();
            }
        else
            {
            // For PC-lint
            }
            
        if(xUnit == EHuiUnitNormalized || xUnit == EHuiUnitParentWidth)
            {
            refSizePx.iX = sizePx.iX;
            }
        else if(xUnit == EHuiUnitParentHeight)
            {
            refSizePx.iX = sizePx.iY;
            }
        else
            {
            // For PC-lint
            }

        if(yUnit == EHuiUnitNormalized || yUnit == EHuiUnitParentHeight)
            {
            refSizePx.iY = sizePx.iY;
            }
        else if(yUnit == EHuiUnitParentWidth)
            {
            refSizePx.iY = sizePx.iX;
            }
        else
            {
            // For PC-lint
            }
        }

    // Display-relative units are always relative to the display where the visual is on.
    if(xUnit == EHuiUnitRelativeToDisplay && Display())
        {
        refSizePx.iX = Display()->VisibleArea().Width();
        }
    if(yUnit == EHuiUnitRelativeToDisplay && Display())
        {
        refSizePx.iY = Display()->VisibleArea().Height();
        }        

    // For relative to my size units, the reference is the size of this visual in pixels.
    if(xUnit == EHuiUnitRelativeToMySize || yUnit == EHuiUnitRelativeToMySize ||
        xUnit == EHuiUnitMyWidth || yUnit == EHuiUnitMyWidth ||
        xUnit == EHuiUnitMyHeight || yUnit == EHuiUnitMyHeight ||
        xUnit == EHuiUnitMyDimensionAverage || yUnit == EHuiUnitMyDimensionAverage)
        {
        THuiRealPoint sizePx(0.f, 0.f);
        if(iLayout)
            {
            // Recursive fun. We don't want to do this more than once.
            // (MetricsToPixels => MetricReferenceInPixels)
            sizePx = iLayout->MetricToPixels(LocalBaseUnit().Abs() * 
                ((aReferenceState == EHuiReferenceStateTarget) ? iSize.RealTarget() : iSize.RealNow()));
            }
        else if(Display())
            {
            sizePx = Display()->VisibleArea().Size().AsPoint();
            }
        else
            {
            // For PC-lint
            }
        
        if(xUnit == EHuiUnitRelativeToMySize || xUnit == EHuiUnitMyWidth)
            {
            refSizePx.iX = sizePx.iX;
            }
        else if(xUnit == EHuiUnitMyHeight)
            {
            refSizePx.iX = sizePx.iY;
            }
        else
            {
            // For PC-lint
            }
        
        if(yUnit == EHuiUnitRelativeToMySize || yUnit == EHuiUnitMyHeight)
            {
            refSizePx.iY = sizePx.iY;
            }
        else if(yUnit == EHuiUnitMyWidth)
            {
            refSizePx.iY = sizePx.iX;
            }
        else
            {
            // For PC-lint
            }

        if( xUnit == EHuiUnitMyDimensionAverage ||
      	    yUnit == EHuiUnitMyDimensionAverage )
            {
            TReal32 average = (sizePx.iX + sizePx.iY) / 2;
            if(xUnit == EHuiUnitMyDimensionAverage)
      	        {
                refSizePx.iX = average;
                }
            if(yUnit == EHuiUnitMyDimensionAverage)
                {           
                refSizePx.iY = average;
                }
            }
        }
                
    if(xUnit == EHuiUnitS60 && Display())
        {
        refSizePx.iX = Display()->UnitValue();
        }
    if(yUnit == EHuiUnitS60 && Display())
        {
        refSizePx.iY = Display()->UnitValue();
        }

    return refSizePx;        
    }
    
void CHuiVisual::ConvertMetricLengthToPixels(
    TReal32& aResult, 
    const THuiMetric& aMetric, 
    TReal32 aReference)
    {
    switch(aMetric.iUnit)
        {
        case EHuiUnitPixel:
            aResult = aMetric.iMagnitude;
            break;
        default:
            aResult = aMetric.iMagnitude * aReference;
            break;
        }
    }
    
    
void CHuiVisual::ConvertPixelsToMetricLength(
    THuiMetric& aResult, 
    TReal32 aPixels, 
    TReal32 aReference)
    {
    switch(aResult.iUnit)
        {
        case EHuiUnitPixel:
            aResult.iMagnitude = aPixels;
            break;
            
        default:
            if(aReference != 0)
                {
                aResult.iMagnitude = aPixels / aReference;
                }
            else
                {
                aResult.iMagnitude = 0;
                }
            break;
        }
    }

EXPORT_C THuiRealPoint CHuiVisual::MetricToPixels(const THuiXYMetric& aMetric) const __SOFTFP
    {
    return MetricToPixels(aMetric,EHuiReferenceStateNow);        
    }   
    
    
EXPORT_C THuiRealPoint CHuiVisual::MetricToPixels(const THuiXYMetric& aMetric,
                                                  THuiReferenceState aReferenceState) const __SOFTFP
    {
    THuiRealPoint refSizePx = MetricReferenceInPixels(aMetric, aReferenceState);
    THuiRealPoint resultPx(0.f, 0.f);

    ConvertMetricLengthToPixels(resultPx.iX, aMetric.iX, refSizePx.iX);
    ConvertMetricLengthToPixels(resultPx.iY, aMetric.iY, refSizePx.iY);
        
    return resultPx;        
    }   

EXPORT_C void CHuiVisual::PixelsToMetric(const THuiRealPoint& aPixels, THuiXYMetric& aMetric) const
    {
    PixelsToMetric(aPixels, aMetric, EHuiReferenceStateNow);
    }


EXPORT_C void CHuiVisual::PixelsToMetric(const THuiRealPoint& aPixels, THuiXYMetric& aMetric,
                                         THuiReferenceState aReferenceState) const
    {
    THuiRealPoint refSizePx = MetricReferenceInPixels(aMetric, aReferenceState);

    ConvertPixelsToMetricLength(aMetric.iX, aPixels.iX, refSizePx.iX);
    ConvertPixelsToMetricLength(aMetric.iY, aPixels.iY, refSizePx.iY);
    }
    
    
EXPORT_C THuiXYMetric CHuiVisual::LocalBaseUnit() const
    {
    if(iLayout)
        {
        return iLayout->BaseUnit();
        }
    else
        {
        return THuiXYMetric(THuiMetric(1.f), THuiMetric(1.f));
        }
    }
    
    
void CHuiVisual::ApplyMirroring(THuiRealPoint& aPixels, THuiReferenceState aReferenceState) const
    {
    THuiRealPoint refSizePx = MetricReferenceInPixels(
        THuiXYMetric( THuiMetric(1.f, EHuiUnitNormalized), THuiMetric(1.f, EHuiUnitNormalized)), aReferenceState );
    
    if(LocalBaseUnit().iX.iMagnitude < 0)
        {
        aPixels.iX = refSizePx.iX - aPixels.iX;
        }
    if(LocalBaseUnit().iY.iMagnitude < 0)
        {
        aPixels.iY = refSizePx.iY - aPixels.iY;
        }    
    }
    

EXPORT_C TPoint CHuiVisual::LocalToDisplay(const TPoint& aPoint) const
    {
    return LocalToDisplay(THuiRealPoint(aPoint));
    }


EXPORT_C THuiRealPoint CHuiVisual::LocalToDisplay(const THuiRealPoint& aPoint) const __SOFTFP
    {
    return LocalToDisplay(aPoint, EHuiReferenceStateNow);    
    }

THuiRealPoint CHuiVisual::LocalToDisplay(const THuiRealPoint& aPoint, 
    THuiReferenceState aReferenceState) const
    {
    THuiRealPoint pos = aPoint;
    THuiRealPoint resultPx(0.f, 0.f);
    
    const CHuiVisual* iter = this;
    while(iter)
        {
        // Add the scroll offset to the position in the local coordinate system.
        if(iter->iLayout && iter->iLayout->Scrolling())
            {
            pos -= (aReferenceState == EHuiReferenceStateNow ? iter->iLayout->ScrollOffset().RealNow() : iter->iLayout->ScrollOffset().RealTarget());
            }
        
        THuiRealPoint coordsPx = iter->LocalPointInPixels(pos);
        iter->ApplyMirroring(coordsPx, aReferenceState);
        iter->AlignByOrigin(coordsPx);
        resultPx += coordsPx;
        
        // Move up in the tree.
        iter = iter->iLayout;
        if(iter)
            {
            pos = (aReferenceState == EHuiReferenceStateNow ? iter->Pos().RealNow() : iter->Pos().RealTarget());
            }
        }
    
    return resultPx;
    }


EXPORT_C TPoint CHuiVisual::DisplayToLocal(const TPoint& aPoint) const
    {
    return DisplayToLocal(THuiRealPoint(aPoint));
    }


EXPORT_C THuiRealPoint CHuiVisual::DisplayToLocal(const THuiRealPoint& aPointInPixels) const __SOFTFP
    {
    return DisplayToLocal(aPointInPixels, EHuiReferenceStateNow);
    }

THuiRealPoint CHuiVisual::DisplayToLocal(const THuiRealPoint& aPointInPixels, 
    THuiReferenceState aReferenceState) const
    {
    THuiRealPoint posPx = aPointInPixels;

    // Undo the effect of the parents.
    CHuiLayout* ancestor = iLayout;
    while(ancestor)
        {
        THuiRealPoint ancestorPos = (aReferenceState == EHuiReferenceStateNow ? ancestor->Pos().RealNow() : ancestor->Pos().RealTarget());
        if(ancestor->Scrolling())
            {
            ancestorPos -= (aReferenceState == EHuiReferenceStateNow ? ancestor->ScrollOffset().RealNow() : ancestor->ScrollOffset().RealTarget());
            }
        THuiRealPoint coordsPx = ancestor->LocalPointInPixels(ancestorPos);
        ancestor->ApplyMirroring(coordsPx, aReferenceState);
        ancestor->AlignByOrigin(coordsPx);
        posPx -= coordsPx;        
        
        // Move up in the tree.
        ancestor = ancestor->iLayout;
        }
        
    // Now we have the point as pixels in the local coordinate system.
    AlignByOrigin(posPx, ETrue); // undo alignment
    ApplyMirroring(posPx, aReferenceState); // undo mirroring
    return PixelPointInLocal(posPx);
    }


EXPORT_C void CHuiVisual::SetChanged()
    {
    if ( Display() )
        {
        Owner().Env().ContinueRefresh();
        }
    iFlags |= EHuiVisualChanged;
    }


EXPORT_C TBool CHuiVisual::Changed() const
    {
    if(iFlags & EHuiVisualChanged)
        {
        return ETrue;
        }

    if(iBrushes && iBrushes->Changed())
        {
        return ETrue;
        }

    if(iTransform && iTransform->Changed())
        {
        return ETrue;
        }

    if (iVisualData->iEffect && iVisualData->iEffect->Changed())
        {
        return ETrue;
        }
    
    if(Owner().ControlGroup() && Owner().ControlGroup()->iOpacity.Changed())
        {
        return ETrue;
        }
        
    if ( iVisualData->iDropShadow && iVisualData->iDropShadow->Changed() )
        {
        return ETrue;
        }

    return iPos.Changed() || iSize.Changed() || iOpacity.Changed();
    }


EXPORT_C void CHuiVisual::ClearChanged()
    {
    if (iFlags & EHuiVisualChanged)
        {
        HUI_DEBUGF1(_L("CHuiVisual::ClearChanged() - Clearing change flags of visual %x."), this);
        }
    iFlags &= ~EHuiVisualChanged;

    if(iBrushes)
        {
        iBrushes->ClearChanged();
        }

    if(iTransform)
        {
        iTransform->ClearChanged();
        }
        
    if ( iVisualData->iDropShadow )
        {
        iVisualData->iDropShadow->ClearChanged();
        }

    iPos.ClearChanged();
    iSize.ClearChanged();
    iOpacity.ClearChanged();
    }


EXPORT_C void CHuiVisual::ReportChanged()
    {
    // Notify the display that the visual's area should be redrawn.
    CHuiDisplay* display = NULL;
        
    // Select active display, fallback to own display.
    // For scaled tv-out cloning the active display takes care of scaling direty areas too. 
    if (iOwner && iOwner->Env().CurrentDisplay())
        {
        display = iOwner->Env().CurrentDisplay();    
        }
    else
        {
        display = Display();    
        }

    if(display)
        {
        // If nothing has changed, then we don't need to notify anything.
        // The change flags will be cleared after the frame has been fully drawn.
        if(!Changed() && !ParentChanged())
            {
            return;
            }

        TRect dirty;
        TBool transformedReport = EFalse;
        
        if(iTransform && /*!(Flags() & EHuiVisualFlagClipping) &&*/
           iTransform->NonIdentityCount() > 0)
            {
            __ASSERT_DEBUG( Display() != NULL,
                            THuiPanic::Panic(THuiPanic::EVisualHasNoDisplayDuringRefresh) );

            if (display->UseTransformedDirtyRegions())
                {
                // There is a transformation and display would like to handle transformed dirty 
                // rects...in practice this means that we are using bitgdi and in that case
                // the transformations do not happen in 3D, so calculating transformed dirty
                // rects might be possible. 
                // -> so we enable transfromation during dirty area reporting. 
                THuiRealRect displayRect = DisplayRect();
                EnterLocalProjection(*display->Gc(), ETrue, 0.f, &displayRect);
                Transform(*display->Gc(), ETrue, &displayRect);
                dirty = TRect(HUI_ROUND_FLOAT_TO_INT(displayRect.iTl.iX), HUI_ROUND_FLOAT_TO_INT(displayRect.iTl.iY),
                              HUI_ROUND_FLOAT_TO_INT(displayRect.iBr.iX), HUI_ROUND_FLOAT_TO_INT(displayRect.iBr.iY));
                ExpandRectWithContent(dirty);
                transformedReport = ETrue;                    
                }
            else
                {
                // Display says it does not want to handle transformed dirty rects.                   
                // There is a transformation and the visual is not clipped. We
                // could try to calculate where the visual's content may end up,
                // but in a 3D projection this may be too complicated to yield
                // any benefits. We will just assume that the entire display
                // needs redrawing.
                dirty = TRect(TPoint(0, 0), Display()->VisibleArea().Size());
                }

            }
        else
            {
            THuiRealRect displayRect = DisplayRect();
            dirty = TRect(HUI_ROUND_FLOAT_TO_INT(displayRect.iTl.iX), HUI_ROUND_FLOAT_TO_INT(displayRect.iTl.iY),
                          HUI_ROUND_FLOAT_TO_INT(displayRect.iBr.iX), HUI_ROUND_FLOAT_TO_INT(displayRect.iBr.iY));
            ExpandRectWithContent(dirty);
            }

		TRect& previousDrawn = iPreviousDrawn;
        
        // CombineAndAddDirtyRegion modifies "dirty" param by transforming it.
        // "previousDrawn" is supposed to be already transformed. 
        display->CombineAndAddDirtyRegion(previousDrawn, dirty);                
        
        // Note that "previousDrawn" is a reference so it updates correct member data.
        previousDrawn = dirty;       

        if (transformedReport)
            {
            EnterLocalProjection(*display->Gc(), EFalse);
            Transform(*display->Gc(), EFalse);                                        
            }
            
        }
    }


EXPORT_C void CHuiVisual::EnterLocalProjection(
        CHuiGc& aGc, TBool aApply, TReal32 /*aExtraOffset*/,
        const THuiRealRect* aVisibleRect) const __SOFTFP
    {
    TReal32 depthOffset = iDepthOffset.Now();
        
    if(aApply && depthOffset != 0)
        {
        aGc.Enable(CHuiGc::EFeatureDepthTest);        
        aGc.Push(EHuiGcMatrixModel);
        aGc.Translate(EHuiGcMatrixModel, 0.f, 0.f, depthOffset);
        }
        
    if(aApply && (iFlags & EHuiVisualFlag3DProjection))
        {
        // Change to a 3D projection.
        aGc.SetProjection(CHuiGc::EProjectionPerspective);

        if(aVisibleRect && (Flags() & EHuiVisualFlagLocalHorizon))
            {
            aGc.SetFrustumOffset(aVisibleRect->Center());
            }

        if(iFlags & EHuiVisualFlagNoDepthOcclusion)
            {
            aGc.Disable(CHuiGc::EFeatureDepthTest);
            }
        else
            {
            aGc.Enable(CHuiGc::EFeatureDepthTest);
            }
        
        if(iFlags & EHuiVisualFlagNoDepthUpdate)
            {
            aGc.Disable(CHuiGc::EFeatureDepthWrite);
            }
        else
            {
            aGc.Enable(CHuiGc::EFeatureDepthWrite);
            }
        }

    if(!aApply)
        {
        // Back to normal.
        aGc.Disable(CHuiGc::EFeatureDepthTest);
        aGc.Enable(CHuiGc::EFeatureDepthWrite);
        }

    if(!aApply && (iFlags & EHuiVisualFlag3DProjection))
        {
        // Back to 2D projection.
        aGc.SetProjection();
        }
        
    if(!aApply && depthOffset != 0)
        {
        aGc.Pop(EHuiGcMatrixModel);
        }
    }


EXPORT_C void CHuiVisual::Transform(CHuiGc& aGc, TBool aApply,
                                    const THuiRealRect* aVisibleRect) const
    {
    if(iTransform)
        {
        if(aApply)
            {
            THuiRealPoint center;
            aGc.Push(EHuiGcMatrixModel);
            if(aVisibleRect)
                {
                center.iX = aVisibleRect->iTl.iX + aVisibleRect->Width() * iTransform->OriginPoint().iX.Now();
                center.iY = aVisibleRect->iTl.iY + aVisibleRect->Height() * iTransform->OriginPoint().iY.Now();
                aGc.Translate(EHuiGcMatrixModel, center.iX, center.iY, 0.f);
                }
            iTransform->Execute(EHuiGcMatrixModel, aGc);
            if(aVisibleRect)
                {
                aGc.Translate(EHuiGcMatrixModel, -center.iX, -center.iY, 0.f);
                }
            }
        else
            {
            aGc.Pop(EHuiGcMatrixModel);
            }
        }
    }


EXPORT_C void CHuiVisual::SetPadding(TInt aPadding)
    {
    iPadding = THuiBoxMetric(THuiXYMetric(aPadding, aPadding));
    SetChanged();
    }


EXPORT_C void CHuiVisual::SetPadding(const TPoint& aPadding)
    {
    iPadding = THuiBoxMetric(THuiXYMetric(aPadding.iX, aPadding.iY));
    SetChanged();
    }


EXPORT_C void CHuiVisual::SetPadding(const THuiBoxMetric& aPadding)
    {
    iPadding = aPadding;
    SetChanged();
    }
    
    
EXPORT_C const THuiBoxMetric& CHuiVisual::Padding() const
    {
    return iPadding;
    }
    
EXPORT_C THuiRealRect CHuiVisual::PaddingInPixels() const __SOFTFP
    {
    return PaddingInPixels(EHuiReferenceStateNow);
    }
    
EXPORT_C THuiRealRect CHuiVisual::PaddingInPixels(THuiReferenceState aReferenceState) const __SOFTFP
    {
    THuiRealRect paddingInPixels;
    BoxMetricToPixelRect(Padding(), paddingInPixels, aReferenceState);
    return paddingInPixels;    
    }
        

EXPORT_C void CHuiVisual::MoveToFront(TInt aTransitionTime)
    {
    if(iLayout)
        {
        // Make this the last child of the parent layout.
        iLayout->MoveVisualToFront(*this, aTransitionTime);
        }
    else if(Display())
        {
        Display()->Roster().MoveVisualToFront(this);
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C void CHuiVisual::GetDualAlpha(TReal32 aOpacity, TReal32 aSecondary,
                              TInt& aPrimaryAlpha, TInt& aSecondaryAlpha)
    {
    // Default values.
    aPrimaryAlpha = TInt(255 * aOpacity);
    aSecondaryAlpha = 0;

    if(aSecondary < 1 && aOpacity < 1)
        {
        /** This reduces the artifact where the sharp image is clearly
                   visible during a transition even though secondary alpha
                   has a value near to one. Not completely perfect, though. */
        aPrimaryAlpha = TInt(255 * (aOpacity + aSecondary * .5 * (aOpacity - 1)));

        if(aPrimaryAlpha < 0)
            {
            aPrimaryAlpha = 0;
            }
        }
    else if(aSecondary >= 1)
        {
        aPrimaryAlpha = 0;
        }
    else
        {
        // for PC lint
        }

    if(aSecondary > 0)
        {
        aSecondaryAlpha = TInt(255 * aOpacity * aSecondary);
        }
    }


EXPORT_C CHuiTransformation& CHuiVisual::Transformation()
    {
    if(!iTransform)
        {
        THuiPanic::Panic(THuiPanic::EVisualTransformNotEnabled);
        }

    return *iTransform;
    }

CHuiTransformation* CHuiVisual::Transformation() const
    {
    return iTransform;        
    }


EXPORT_C void CHuiVisual::EnableTransformationL(TBool aIsTransformed)
    {
    if(iTransform && !aIsTransformed)
        {
        // Disable scrolling.
        delete iTransform;
        iTransform = NULL;
        }
    else if(!iTransform && aIsTransformed)
        {
        // Enable scrolling.
        iTransform = CHuiTransformation::NewL();
        }
    else
        {
        // for PC lint
        }
     
    }


EXPORT_C void CHuiVisual::DumpTree() const
    {
    TBuf<100> buf;
    TBuf<100> className;
    GetClassName(className);
    GetInstanceName(buf);
    buf.AppendFormat(_L(" [label=%S];"), &className);
    CHuiStatic::Printf(buf);
    }


EXPORT_C void CHuiVisual::GetInstanceName(TDes& aName) const
    {
    GetClassName(aName);
	TUint addr = TUint(this);
	aName.AppendFormat(_L("%u"), addr);
    }


EXPORT_C void CHuiVisual::GetClassName(TDes& aName) const
    {
    aName = _L("CHuiVisual");
    }


EXPORT_C void CHuiVisual::EnableBrushesL(TBool aEnabled)
    {
    if(aEnabled && !iBrushes)
        {
        iBrushes = CHuiBrushArray::NewL();
        }
    else if(!aEnabled && iBrushes)
        {
        if ( iBrushes->Count() )
            {
            iFlags |= EHuiVisualChanged;
            }
        
        delete iBrushes;
        iBrushes = 0;
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C CHuiBrushArray* CHuiVisual::Brushes()
    {
    return iBrushes;
    }


EXPORT_C void CHuiVisual::NotifySkinChangedL()
    {
    // Do nothing by default.
    }


EXPORT_C void CHuiVisual::ExpandRectWithContent(TRect& aRect) const
    {
    if(iBrushes)
        {
        iBrushes->ActivateBrushGuide(this);
        iBrushes->ExpandVisualRect(aRect);
        iBrushes->ActivateBrushGuide(NULL);
        }
/*
    if (iVisualData->iEffect)
        {
        TRect visualArea;
        iVisualData->iEffect->VisualArea(visualArea);
        aRect.BoundingRect(visualArea);
        }
*/        
    }


EXPORT_C void CHuiVisual::SetTagL(const TDesC8& aTag)
    {
//    ASSERT(!iTag);
    if ( iTag )
        {
        delete iTag;
        iTag = NULL;
        }
    if ( aTag.Length() > 0 )
        {
        iTag = aTag.AllocL();
        }
    }


EXPORT_C const TDesC8& CHuiVisual::Tag() const
    {
    if(iTag)
        {
        return *iTag;
        }
    return KHuiVisualUndefinedTag;
    }


EXPORT_C TBool CHuiVisual::IsTagged() const
    {
    return iTag != 0;
    }


EXPORT_C CHuiVisual* CHuiVisual::FindTag(const TDesC8& aTag)
    {
    // Look for the tag in this visual's tag descriptor.
    if(iTag && HuiUtil::TagMatches(*iTag, aTag))
        {
        return this;
        }
    else
        {
        // Could not find it.
        return NULL;
        }
    }

EXPORT_C TReal32 CHuiVisual::BrushOpacity() const __SOFTFP
    {
    return EffectiveOpacity();
    }


EXPORT_C THuiRealRect CHuiVisual::BrushRect() const __SOFTFP
    {
    // Subclasses can override this to make content-dependent brush guides.
    return DisplayRect();
    }


EXPORT_C CHuiSkin& CHuiVisual::BrushSkin() const
    {
    return Skin();
    }

HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiVisual, ETypeVisual)

EXPORT_C void CHuiVisual::VisualExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    }

EXPORT_C void CHuiVisual::UpdateSiblingLayout(TInt aTransitionTime)
    {
  	if(Layout() && !(Layout()->Flags() & EHuiVisualFlagFreezeLayout))
  	    {
  	    Layout()->UpdateChildrenLayout(aTransitionTime);
  	    }
    }

EXPORT_C void CHuiVisual::BoxMetricToPixelRect(const THuiBoxMetric& aBox,
                                               THuiRealRect& aRectInPixels, 
                                               THuiReferenceState aReferenceState) const
    {
    aRectInPixels.iTl = MetricToPixels(THuiXYMetric(aBox.iLeft, aBox.iTop), aReferenceState);
    aRectInPixels.iBr = MetricToPixels(THuiXYMetric(aBox.iRight, aBox.iBottom), aReferenceState);
    }
        
EXPORT_C THuiRealPoint CHuiVisual::LocalPointInPixels(const THuiRealPoint& aLocalPoint, 
                                                      THuiReferenceState aReferenceState) const __SOFTFP
    {
    return MetricToPixels(LocalBaseUnit().Abs() * aLocalPoint, aReferenceState);
    }

EXPORT_C THuiRealPoint CHuiVisual::PixelPointInLocal(const THuiRealPoint& aPixelPoint,
                                                     THuiReferenceState aReferenceState) const __SOFTFP
    {
  	return PixelPointInUnits(aPixelPoint, LocalBaseUnit().Abs(), aReferenceState);
  	}


TInt CHuiVisual::FindTacticon( TInt aEventType ) const
    {
    for ( TInt i = 0 ; i < iVisualData->iTacticons.Count() ; i++ )
        {
        if ( iVisualData->iTacticons[i].iEventType == aEventType )
            {
            return i;
            }       
        }
    return KErrNotFound;
    }
    
TInt CHuiVisual::TactileFeedback( TInt aEventType ) const
    {
    TInt returnType = KErrNotFound;
    TInt index = FindTacticon( aEventType );
    
    if ( index == KErrNotFound )
        {
        // Try the layout
        CHuiVisual* layoutToCheck = Layout();
        while ( layoutToCheck )
            {
            index = layoutToCheck->FindTacticon( aEventType );
            if ( index != KErrNotFound )
                {
                returnType = layoutToCheck->iVisualData->iTacticons[index].iFeedbackType;
                break;
                }
            
            layoutToCheck = layoutToCheck->Layout();
            }
        }
    else
        {
        returnType = iVisualData->iTacticons[index].iFeedbackType;
        }
        
    return returnType;
    }

EXPORT_C void CHuiVisual::SetTactileFeedbackL( TInt aEventType, TInt aFeedbackType )
    {
    if ( aFeedbackType < 0 || aEventType < 0 )
        {
        User::Leave( KErrArgument );
        }
    
    // find if it exists already
    const TInt index = FindTacticon( aEventType );
    
    if ( index != KErrNotFound )
        {
        iVisualData->iTacticons[index].iFeedbackType = aFeedbackType;
        }
    else
        {            
        // Create new item
        THuiVisualPrivateData::TTacticon newTacticon;
        newTacticon.iEventType = aEventType;
        newTacticon.iFeedbackType = aFeedbackType;
        
        iVisualData->iTacticons.AppendL( newTacticon );
        }
    }
    
EXPORT_C TBool CHuiVisual::HasTactileFeedback( TInt aEventType ) const
    {
    if ( aEventType == KHuiTactileFeedbackEventTypeNone )
        {
        return iVisualData->iTacticons.Count();
        }
    
    const TInt index = FindTacticon( aEventType );
    return index != KErrNotFound;
    }
    
EXPORT_C void CHuiVisual::RemoveTactileFeedback( TInt aEventType )
    {
    if ( aEventType == KHuiTactileFeedbackEventTypeNone )
        {
        iVisualData->iTacticons.Reset();
        return;
        }
        
    const TInt index = FindTacticon( aEventType );
    if ( index != KErrNotFound )
        {
        iVisualData->iTacticons.Remove(index);
        }
    }
    

TBool CHuiVisual::IsTransformed() const
    {
    return iTransform != 0;    
    }

TBool CHuiVisual::ParentChanged() const
    {
    // Check if parent has been changed in a way that requires me to report myself as changed
    // This may be a performance issue because this function is called often.
    const CHuiLayout* iter = Layout(); // start from parent
    const CHuiLayout* parent = Layout(); // Immediate parent
    
    while(iter)
        {
        // Parent scrolling, we check this before clipping because otherwise
        // iPreviousDrawn would not get updated for children correctly as it
        // is nowdays only updated in ReportChanged() method (and no more in the draw).
        // NOTE: Only immediate parent is checked for better performance (no need to
        // check others ?)
        // TBD: Should transformations be also checked before clipping ?
        if (parent->Scrolling() && parent->ScrollOffset().Changed())
            {
            return ETrue;    
            }   

        // If the parent is clipped, there is no need to check any more.
        // If the parent in this case has been changed, the parent rect is added
        // into the dirty regions and will be drawn anyways.
        if ( iter->Clipping() )
            {
            return EFalse;
            }
        
        
        // Parent transformation affects dirty regions if visual is not clipped and outside parent
        if(iter->IsTransformed() && iter->Transformation()->Changed())
            {
            return ETrue;
            }
        // Parent opacity affects this visuals opacity if visual is not clipped and outside parent
        if (iter->iOpacity.Changed())
            {
            return ETrue;    
            }    
            
        // Also position (and size) affects
        if (iter->iPos.Changed() || iter->iSize.Changed())
            {
            return ETrue;    
            }   
            
                    
        iter = iter->Layout();
        }
    return EFalse;        
    }

void CHuiVisual::HandleFlagsChanged( TInt aOldFlags )
    {
    // If clipping setting is changed, refresh automatically.
    const TBool oldClipping = aOldFlags & EHuiVisualFlagClipping;
    const TBool newClipping = iFlags & EHuiVisualFlagClipping;
    if ( ( oldClipping && !newClipping ) ||
         ( !oldClipping && newClipping ) )
        {
        SetChanged();
        }
    }

EXPORT_C void CHuiVisual::EnableDropShadowL( TBool aEnable )
    {
    if ( aEnable && !iVisualData->iDropShadow )
        {
        CHuiDropShadow* dropShadowHandler = new (ELeave) CHuiDropShadow;
        CleanupStack::PushL( dropShadowHandler );
        dropShadowHandler->ConstructL();
        CleanupStack::Pop( dropShadowHandler );
        iVisualData->iDropShadow = dropShadowHandler;
        SetChanged();
        }
    else if ( !aEnable && iVisualData->iDropShadow )
        {
        delete iVisualData->iDropShadow;
        iVisualData->iDropShadow = NULL;
        SetChanged();
        }
    }
    
EXPORT_C CHuiDropShadow* CHuiVisual::DropShadowHandler() const
    {
    return iVisualData->iDropShadow;
    }
CHuiBrushArray* CHuiVisual::Brushes() const
    {
    return iBrushes;
    }


EXPORT_C CHuiFxEffect* CHuiVisual::Effect() const
    {
    if (!iVisualData)
        return NULL;
    else
        return iVisualData->iEffect;
    }

EXPORT_C MHuiEffectable *CHuiVisual::Effectable() const
    {
    if (!iVisualData)
        return NULL;
    else
        return iVisualData->iEffectable;
    }

TBool CHuiVisual::IsDelayedEffectSource() const
    {
    if (!Effectable())
        return EFalse;
    else
        {
        TBool b = Effectable()->EffectGetSource();
        if (!b)
            { // if not found from this object, try the parent
            CHuiLayout *l = Layout();
            if (l)
                return Layout()->IsDelayedEffectSource();
            }
        return b;
        }
   }

EXPORT_C void CHuiVisual::SetEffect(CHuiFxEffect* aEffect)
    {
    __ALFFXLOGSTRING2("CHuiVisual::SetEffect - : 0x%d on visual 0x%x", aEffect, this );
    // If effect is replaced by a new effect, we don't want the effect end callback
    // as it would end the new effect
    if ( iVisualData->iEffect )
        {
#ifdef _ALF_FXLOGGING
        if ( aEffect )
            {
            __ALFFXLOGSTRING1("CHuiVisual::SetEffect - : Deleting previous 0x%x effect on this visual ", iVisualData->iEffect );
            }
#endif
        iVisualData->iEffect->SetEffectEndObserver( NULL, 0 );
        }
    delete iVisualData->iEffect;
    iVisualData->iEffect = aEffect;
    // The parser must now go
    delete iVisualData->iEffectParser;
    iVisualData->iEffectParser = NULL; // the parser just died
    SetChanged();
    }
    
void CHuiVisual::SetEffectParser( CHuiFxEffectParser* aEffectParser )
    {
    delete iVisualData->iEffectParser;
    iVisualData->iEffectParser = aEffectParser;
    }

TInt CHuiVisual::QueryCanvasFlags()
    {
    THuiVisualQueryParams p;
    p.iQueryType = THuiVisualQueryParams::EQueryCanvasFlags;
    p.iValue = 0;
    p.iResult = KErrNotSupported;
    TAny* ptr = &p;
    
    VisualExtension(KHuiVisualQueryUid, &ptr);
    
    // If visual does not support this extension, assume 0
    
    return ( p.iResult == KErrNone ) ? p.iValue : 0;
    }
    
TBool CHuiVisual::QueryExternalContentDrawingEnabled()
    {
    THuiVisualQueryParams p;
    p.iQueryType = THuiVisualQueryParams::EQueryExternalContentDrawingEnabled;
    p.iValue = 0;
    p.iResult = KErrNotSupported;
    TAny* ptr = &p;
    
    VisualExtension(KHuiVisualQueryUid, &ptr);
    
    // If visual does not support this extension, assume
    // that visual does not have external content drawing enabled.
    
    return ( p.iResult == KErrNone ) && p.iValue;
    }
    
TBool CHuiVisual::QueryHasDrawableContent()
    {
    THuiVisualQueryParams p;
    p.iQueryType = THuiVisualQueryParams::EQueryHasDrawableContent;
    p.iValue = 0;
    p.iResult = KErrNotSupported;
    TAny* ptr = &p;
    
    VisualExtension(KHuiVisualQueryUid, &ptr);

    // If visual does not support this extension, assume
    // that it has something to draw.
    
    return p.iValue || ( p.iResult != KErrNone );
    }
