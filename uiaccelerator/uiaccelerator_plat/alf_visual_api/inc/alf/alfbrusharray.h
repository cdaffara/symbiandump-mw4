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
* Description:   Place holder for brushes.
*
*/



#ifndef C_ALFBRUSHARRY_H
#define C_ALFBRUSHARRY_H

#include <e32base.h>
#include <alf/alfownership.h>

class CAlfBrush;
class CAlfVisual;

/**
 *  Array for brushes.
 *
 *  This client object can store owned items, but all the real objects
 *  are on the server side. Also, the order of brushes is maintained on
 *  the server side.
 *  
 * Usage:
 * @code
 * 
 * //Create Visual
 * CAlfTextVisual* textVisual = control.AppendVisualL( EAlfVisualTypeText, NULL );
 * 
 * //Create border brush
 * CAlfBorderBrush* borderBrush = CAlfBorderBrush::NewL( *iEnv, TAlfXYMetric(TAlfMetric(1)),
 *             TAlfXYMetric(TAlfMetric(KAlfDefaultBorderBrushEdgeOffsetX, EAlfUnitPixel),
 *             TAlfMetric(KAlfDefaultBorderBrushEdgeOffsetY, EAlfUnitPixel)));
 * 
 * //Enable brush
 * textVisual->EnableBrushesL();
 * 
 * //Get brush array
 * CAlfBrushArray* brushArray = textVisual->Brushes();
 * 
 * //Append border brush in brush array, with ownership transfered
 * brushArray->AppendL( borderBrush, EAlfHasOwnership );
 * 
 * //removes and deletes border brush in brush array
 * brushArray->Remove(0);
 * 
 * //Cleanup
 * delete iEnv; //Will delete visual , brush array as well.
 * 
 * @endcode
 * 
 * @lib alfclient.lib 
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfBrushArray) : public CBase
    {

public:

    /**
     * Constructor.
     */
    static CAlfBrushArray* NewL( CAlfVisual& aOwner );

    /**
     * Destructor.
     */
    virtual ~CAlfBrushArray();
    
    /**
     * Resets the array
     */
    IMPORT_C void Reset();

    /**
     * Appends a new brush to the array.
     *
     * @param aBrush      Brush instance.
     * @param aOwnership  Ownership of the brush.
     */
    IMPORT_C void AppendL( CAlfBrush* aBrush, TAlfOwnership aOwnership );
    
    /**
     * Inserts a brush to the array.
     *
     * @param aPos        Index for the new brush instance.
     * @param aBrush      Brush instance.
     * @param aOwnership  Ownership of the brush.
     */
    IMPORT_C void InsertL(TInt aPos, CAlfBrush* aBrush, TAlfOwnership aOwnership);
    
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
    IMPORT_C CAlfBrush& operator [] (TInt aPos);

    /**
     * Returns a reference to a brush in the array.
     *
     * @param aPos  Index.
     */
    IMPORT_C CAlfBrush& At(TInt aPos);

    /**
     * @internal
     *
     * Called when a brush is deleted
     * @param aBrush Deleted brush
     */
    void HandleBrushDestroyed( CAlfBrush& aBrush );

private:

    /**
     * Constructor.
     */
    CAlfBrushArray();

    /**
     * Constructor.
     */
    void ConstructL( CAlfVisual& aOwner );

    /**
     * Remove a brush, plus owned brushes.
     * @param aBrush Brushes owned by this brush array will be removed.
     * @param aPosIndex This brush index will be removed if it is >= 0.
     */
    void RemoveBrush(CAlfBrush* aBrush, TInt aPosIndex = -1);
    
private: // data

    // Private data structure. Owned.
    struct TPrivateData;
    TPrivateData* iData;

    };
    
#endif // C_ALFBRUSHARRY_H
