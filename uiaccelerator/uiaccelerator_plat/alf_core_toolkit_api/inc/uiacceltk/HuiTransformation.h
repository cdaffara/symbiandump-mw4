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
* Description:   Definition of CHuiTransformaton. CHuiTransformation describes 
*                a series of matrix transformation operations.
*
*/



#ifndef __HUITRANSFORMATION_H__
#define __HUITRANSFORMATION_H__


#include <e32base.h>

#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiTimedValue.h>
#include <uiacceltk/HuiTimedPoint.h>


/**
 * Indices to be used in the iParams array of TTransform.
 */ 
enum THuiTransformParam
    {
    // Translate.
    EHuiTransformParamTranslateX = 0,
    EHuiTransformParamTranslateY = 1,
    EHuiTransformParamTranslateZ = 2,
    
    // Rotate.
    EHuiTransformParamRotateAngle = 0,
    EHuiTransformParamRotateAxisX = 1,
    EHuiTransformParamRotateAxisY = 2,
    EHuiTransformParamRotateAxisZ = 3,
    
    // Scale.
    EHuiTransformParamScaleX = 0,
    EHuiTransformParamScaleY = 1,
    EHuiTransformParamScaleZ = 2
    };


/** Number of parameters for a transformation step. */
const TInt KHuiTransformParamCount = 4;


/**
 * Transformation matrix with a manipulation interface.
 */
NONSHARABLE_CLASS(CHuiTransformation) : public CBase
    {
public:

    /** @beginAPI */
        
    /** Transformation step types. */
    enum TType
        {
        ETypeTranslate,
        ETypeRotate,
        ETypeScale
        };

    /** Transformation step. */
    struct TTransform
        {
        TType iType;
        THuiTimedValue iParams[KHuiTransformParamCount];
        };
        

public:

    /* Constructors and destructor. */

    /**
     * Constructs a new transformation.
     */
    IMPORT_C static CHuiTransformation* NewL();

    /**
     * Constructs a new transformation and leaves it on the cleanup stack.
     */
    IMPORT_C static CHuiTransformation* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiTransformation();
    
    
    /* Methods. */
    
    /**
     * Returns the number of transform steps in the transformation.
     */
    IMPORT_C TInt Count() const;
    
    /**
     * Returns the number of non-identity transform steps in the transformation.
     * Non-identity steps will result in the transformation having an effect
     * on geometry.
     */
    IMPORT_C TInt NonIdentityCount() const;
    
    /**
     * Returns a reference to one of the transformation steps so that it 
     * can be modified.
     *
     * @param aIndex  Index of the transformation step.
     *
     * @return  Reference to the transformation step. 
     */
    IMPORT_C TTransform& Step(TInt aIndex);
    
    /**
     * Equivalent to Step().
     */
    IMPORT_C TTransform& operator [] (TInt aIndex);
    
    IMPORT_C void LoadIdentity();    
    
    //IMPORT_C void Begin();

    /**
     * 2D translation.
     */
    IMPORT_C void Translate(TReal32 aX, TReal32 aY) __SOFTFP;

    /**
     * 2D translation with timed values.
     */
    IMPORT_C void Translate(const THuiTimedValue& aX, const THuiTimedValue& aY);
    
    /**
     * 2D scaling.
     */
    IMPORT_C void Scale(TReal32 aX, TReal32 aY) __SOFTFP;

    /**
     * 2D scaling with timed values.
     */
    IMPORT_C void Scale(const THuiTimedValue& aX, const THuiTimedValue& aY);

    /**
     * 3D scaling.
     */
    IMPORT_C void Scale(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    /**
     * 3D scaling with timed values.
     */
    IMPORT_C void Scale(const THuiTimedValue& aX, const THuiTimedValue& aY,
                        const THuiTimedValue& aZ);
    
    /**
     * 2D rotation.
     */
    IMPORT_C void Rotate(TReal32 aAngle) __SOFTFP;
    
    /**
     * 2D rotation with timed value.
     */
    IMPORT_C void Rotate(const THuiTimedValue& aAngle);

    /**
     * 3D rotation.
     */
    IMPORT_C void Rotate(TReal32 aAngle, TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP;
    
    /**
     * 3D rotation with timed values.
     */
    IMPORT_C void Rotate(const THuiTimedValue& aAngle, 
                         TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP;

    /**
     * Execute the transformation on a matrix.
     *
     * @param aMatrix  Matrix stack to execute the transformation on.
     */
    IMPORT_C void Execute(THuiGcMatrix aMatrix, CHuiGc& aGc) const;
    
    /** @endAPI */
    
    
    /**
     * Determines if the transformation has changed since the latest call
     * to ClearChanged().
     */
    TBool Changed() const;

    /**
     * Clears the changed flags of the transformation.
     */   
    void ClearChanged();

    const THuiTimedPoint& OriginPoint() const;


protected:

    /* Constructors. */

    /**
     * Constructor. Initializes member variables as an identity 
     * transformation.
     */   
    CHuiTransformation();

private:

    /* Private methods. */
    
    /**
     * Appends a new transformation step.
     */
    TInt Append(const TTransform& aTransform);


private:

    /** An array of transformation steps. These are executed in order. */
    RPointerArray<TTransform> iTransforms;  
    
    /** Flag that tells if the transformation steps have changed. */
    TBool iChanged;
    
    THuiTimedPoint iOriginPoint;
     
    };


#endif  // __HUITRANSFORMATION_H__
