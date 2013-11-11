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



#ifndef __HUIBRUSHARRAY_H__
#define __HUIBRUSHARRAY_H__


#include <e32base.h>
#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiBrush.h>


/* Forward declarations. */
class CHuiVisual;


/**
 * Array of brushes.
 */
NONSHARABLE_CLASS(CHuiBrushArray) : public CBase
    {
public:

    IMPORT_C static CHuiBrushArray* NewL();
    
    IMPORT_C static CHuiBrushArray* NewLC();
    
    IMPORT_C virtual ~CHuiBrushArray();
    
    
    /* Methods. */

    /**
     *
     */
    IMPORT_C void Reset();
    
    /**
     * Appends a new brush to the array.
     *
     * @param aBrush      Brush instance.
     * @param aOwnership  Ownership of the brush.
     */
    IMPORT_C void AppendL(CHuiBrush* aBrush, THuiOwnership aOwnership);

    /**
     * Inserts a brush to the array.
     *
     * @param aPos        Index for the new brush instance.
     * @param aBrush      Brush instance.
     * @param aOwnership  Ownership of the brush.
     */
    IMPORT_C void InsertL(TInt aPos, CHuiBrush* aBrush, THuiOwnership aOwnership);
    
    /**
     * Removes a brush from the array. If the array has ownership of the 
     * brush, it will be deleted.
     *
     * @param aPos  Index of the brush instance to remove.
     */
    IMPORT_C void Remove(TInt aPos);

    /** 
     * Count the number of brushes in the array.
     *
     * @return  Number of brushes.
     */
    IMPORT_C TInt Count() const;    
    
    /**
     * Returns a reference to a brush in the array.
     *
     * @param aPos  Index.
     */
    IMPORT_C CHuiBrush& operator [] (TInt aPos);

    /**
     * Returns a reference to a brush in the array.
     *
     * @param aPos  Index.
     */
    IMPORT_C CHuiBrush& At(TInt aPos);

    /**
     * Count the number of brushes on a certain layer.
     *
     * @param aLayer  Layer to count.
     */
    IMPORT_C TInt LayerBrushCount(THuiBrushLayer aLayer) const;

    /**
     * Draws all the brushes in the array.
     *
     * @param aLayer        Which layer to draw.
     * @param aGc           Graphics context.
     * @param aDisplayRect  Area affected by the brush.
     */    
    IMPORT_C void Draw(THuiBrushLayer aLayer, CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;

    /**
     * Determines if a change flag is raised in the array.
     *
     * @return  <code>ETrue</code>, if there is something to refresh.
     *          <code>EFalse</code>, if no changes.
     */
    TBool Changed() const;
    
    /**
     * Clears the change flags of the brushes.
     */
    void ClearChanged();
    
    /**
     * Expands the dirty region of a visual. Called when a visual's dirty 
     * region is being determined. 
     *
     * @param aRect  Rectangle to expand. Visual's dirty region.
     */
    void ExpandVisualRect(TRect& aRect) const;
    
    /**
     * Notifies change in the brush guide
     */    
    void ActivateBrushGuide(const CHuiVisual* aNewGuide) const;
    /**
     * aBrushType is CHuiBrush::TType2 enum
     */
    IMPORT_C TInt BrushWithTypeCount(TInt aBrushType) const;  
    IMPORT_C CHuiBrush *BrushWithTypeAt(TInt aBrushType, TInt aIndex);
protected:

    CHuiBrushArray();
    
private:    
    


private:

    typedef RHuiOwnedPointer<CHuiBrush> RArrayElement;

    RPointerArray<RArrayElement> iElements;    
    
    };


#endif // __HUIBRUSHARRAY_H__

