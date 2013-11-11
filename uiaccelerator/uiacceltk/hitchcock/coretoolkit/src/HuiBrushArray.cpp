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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiBrushArray.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"


EXPORT_C CHuiBrushArray* CHuiBrushArray::NewL()
    {
    CHuiBrushArray* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
    
EXPORT_C CHuiBrushArray* CHuiBrushArray::NewLC()
    {
    CHuiBrushArray* self = new (ELeave) CHuiBrushArray();
    CleanupStack::PushL(self);
    return self;
    }


CHuiBrushArray::CHuiBrushArray()
    {
    }
    
  
EXPORT_C CHuiBrushArray::~CHuiBrushArray()
    {
    Reset();
    }
  
    
EXPORT_C void CHuiBrushArray::Reset()
    {
    iElements.ResetAndDestroy();
    }


EXPORT_C void CHuiBrushArray::AppendL(CHuiBrush* aBrush, THuiOwnership aOwnership)
    {
    if (!aBrush) // RArrayElement can't handle null ptr correctly in all cases
        {
        User::Leave(KErrArgument);
        }
        
    RArrayElement* element = new (ELeave) RArrayElement();
    CleanupStack::PushL(element);
    User::LeaveIfError(iElements.Append(element));
    CleanupStack::Pop(element);
    
    element->Set(aBrush, aOwnership);
    }


EXPORT_C void CHuiBrushArray::InsertL(TInt aPos, CHuiBrush* aBrush, 
                                      THuiOwnership aOwnership)
    {
    if (!aBrush) // RArrayElement can't handle null ptr correctly in all cases
        {
        User::Leave(KErrArgument);
        }

    RArrayElement* element = new (ELeave) RArrayElement();
    CleanupStack::PushL(element);
    User::LeaveIfError(iElements.Insert(element, aPos));
    CleanupStack::Pop(element);
    
    element->Set(aBrush, aOwnership);
    }


EXPORT_C void CHuiBrushArray::Remove(TInt aPos)
    {
    if ( aPos >= Count() )
        {
        // if does not exist, just return
        return;
        }
    delete iElements[aPos];
    iElements.Remove(aPos);
    }


EXPORT_C TInt CHuiBrushArray::Count() const
    {
    return iElements.Count();
    }
    
    
EXPORT_C CHuiBrush& CHuiBrushArray::operator [] (TInt aPos)
    {
    return iElements[aPos]->Ref();
    }
    
    
EXPORT_C CHuiBrush& CHuiBrushArray::At(TInt aPos)
    {
    return iElements[aPos]->Ref();
    }


EXPORT_C void CHuiBrushArray::Draw(THuiBrushLayer aLayer, CHuiGc& aGc, 
                                   const MHuiBrushGuide& aGuide) const
    {
    TInt layerBrushCount = LayerBrushCount(aLayer);
    TInt depth = 0;

    if(!layerBrushCount)
        {
        // Nothing on this layer.
        return;
        }

    if(aLayer == EHuiBrushLayerBackground)
        {
        depth = -layerBrushCount - 1;
        }
    
    for(TInt i = 0; i < Count(); ++i)
        {
        CHuiBrush& brush = iElements[i]->Ref();
        
        if(brush.Layer() == aLayer)
            {
            // Polygon depth offset to avoid Z-conflicts.
            if(aLayer == EHuiBrushLayerBackground || 
               aLayer == EHuiBrushLayerForeground)
                {
                ++depth;                
                }
            aGc.SetDepthOffset(-depth);
            
            brush.BeginDraw(aGc, aGuide);
            brush.Draw(aGc, aGuide);
            brush.EndDraw(aGc, aGuide);
            }
        }
        
    // Reset depth offset back to zero.
    aGc.SetDepthOffset(0);    
    }


EXPORT_C TInt CHuiBrushArray::LayerBrushCount(THuiBrushLayer aLayer) const
    {
    TInt count = 0;
    
    for(TInt i = 0; i < Count(); ++i)
        {
        if(iElements[i]->Ref().Layer() == aLayer)
            {
            ++count;
            }
        }    
    
    return count;
    }
EXPORT_C TInt CHuiBrushArray::BrushWithTypeCount(TInt aBrushType) const
    {
    TInt count = 0;
    TInt num = Count();
    for(TInt i=0;i<num;i++)
        {
        if (iElements[i]->Ref().Type2()==aBrushType)
            {
            count++;
            }
        }
    return count;
    }
EXPORT_C CHuiBrush *CHuiBrushArray::BrushWithTypeAt(TInt aBrushType, TInt aIndex)
    {
    TInt count = 0;
    TInt num = Count();
    for(TInt i=0;i<num;i++)
        {
        if (iElements[i]->Ref().Type2()==aBrushType)
            {
            if (count == aIndex)
                {
                return &iElements[i]->Ref();
                }
            count++;
            }
        }
    return 0;    
    }
    
    
TBool CHuiBrushArray::Changed() const
    {
    for(TInt i = 0; i < iElements.Count(); ++i)
        {
        if(iElements[i]->Ref().Changed())
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
    
void CHuiBrushArray::ClearChanged()
    {
    for(TInt i = 0; i < Count(); ++i)
        {
        iElements[i]->Ref().ClearChanged();
        }
    }
    
    
void CHuiBrushArray::ExpandVisualRect(TRect& aRect) const
    {
    TRect original = aRect;
    
    for(TInt i = 0; i < Count(); ++i)
        {
        CHuiBrush& brush = iElements[i]->Ref();

        // If the drawing of the brush is clipped to visual contents,
        // there is no need to expand the rectangle.
        if(!brush.ClipToVisual())
            {
            TRect expanded = original;
            brush.ExpandVisualRect(expanded);
            aRect.BoundingRect(expanded);
            }
        }
    }


void CHuiBrushArray::ActivateBrushGuide(const CHuiVisual* aNewGuide) const
    {
    for(TInt i = 0; i < Count(); ++i)
        {
        CHuiBrush& brush = iElements[i]->Ref();
        brush.ActivateBrushGuide(aNewGuide);
        }        
    }
