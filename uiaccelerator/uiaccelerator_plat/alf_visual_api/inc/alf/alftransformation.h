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
* Description:   Transformation class.
*
*/



#ifndef C_ALFTRANSFORMATION_H
#define C_ALFTRANSFORMATION_H

#include <e32base.h>

#include <alf/alftimedvalue.h>

class CAlfEnv;

/**
 * Indices to be used in the iParams array of TTransform.
 */ 
enum TAlfTransformParam
    {
    // Translate.
    EAlfTransformParamTranslateX = 0,
    EAlfTransformParamTranslateY = 1,
    EAlfTransformParamTranslateZ = 2,
    
    // Rotate.
    EAlfTransformParamRotateAngle = 0,
    EAlfTransformParamRotateAxisX = 1,
    EAlfTransformParamRotateAxisY = 2,
    EAlfTransformParamRotateAxisZ = 3,
    
    // Scale.
    EAlfTransformParamScaleX = 0,
    EAlfTransformParamScaleY = 1,
    EAlfTransformParamScaleZ = 2
    };

/** Number of parameters for a transformation step. */
const TInt KAlfTransformParamCount = 4;

/**
 *  Transformation class for visual effects.
 *  Usage:
 *  @code
 * 
 *   // Spin around the control group
 *   group.EnableTransformationL(); 
 *   CAlfTransformation& xf = group.Transformation();
 *   
 *   // Set up a transformation that makes the control group rotate a full
 *   // 360 degrees around the center of the display.
 *   
 *   TAlfTimedValue angle;
 *   angle.SetValueNow(0);
 *   angle.SetTarget(360, 5000); // 360 degrees in 5 seconds
 *   
 *   // Clear previously created transformation steps.
 *   xf.LoadIdentity();
 *   
 *   xf.Translate(displaySize.mWidth/2, displaySize.mHeight/2);
 *   xf.Rotate(angle);
 *   xf.Translate(-displaySize.mWidth/2, -displaySize.mHeight/2);
 * 
 *  @endcode
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfTransformation ): public CBase
    {

public:

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
        TAlfTimedValue iParams[KAlfTransformParamCount];
        };

    /**
     * Constructor
     */
    IMPORT_C static CAlfTransformation* NewL(CAlfEnv& aEnv);

    /**
     * Constructor
     */
    IMPORT_C static CAlfTransformation* NewLC(CAlfEnv& aEnv);

    /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfTransformation();

    /**
     * Return the server side handle
     *
     * @return Handle to the server side CAlfSrvTransformationSubSession 
     * object. 0 if not set.
     */
    TInt ServerHandle() const;
    

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
     * Returns a one of the transformation steps so that it 
     * can be modified and set with SetStep method.
     *
     * @param aIndex  Index of the transformation step.
     *
     * @return  Transformation step. 
     */
    IMPORT_C TTransform Step(TInt aIndex);

    /**
     * Modifies a one of the transformation steps by replacing
     * the existing step with a given step.
     *
     * @param aIndex  Index of the transformation step to be replaced
     *
     * @return  Transformation step. 
     */
    IMPORT_C void ReplaceStep(TInt aIndex, TTransform aTransformationStep);
    
    /**
     * Clears previously created transformation steps.
     */
    IMPORT_C void LoadIdentity();

    /**
     * 2D rotation.
     */
    IMPORT_C void Rotate(TReal32 aAngle) __SOFTFP;
    
    /**
     * 2D translation.
     *
     * @param aX x-coordinate
     * @param aY y-coordinate
     */
    IMPORT_C void Translate(TReal32 aX, TReal32 aY) __SOFTFP;

    /**
     * 2D translation with timed values.
     */
    IMPORT_C void Translate(const TAlfTimedValue& aX, const TAlfTimedValue& aY);

    /**
     * 2D scaling.
     */
    IMPORT_C void Scale(TReal32 aX, TReal32 aY) __SOFTFP;

    /**
     * 2D scaling with timed values.
     */
    IMPORT_C void Scale(const TAlfTimedValue& aX, const TAlfTimedValue& aY);

    /**
     * 3D scaling.
     */
    IMPORT_C void Scale(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    /**
     * 3D scaling with timed values.
     */
    IMPORT_C void Scale(const TAlfTimedValue& aX, const TAlfTimedValue& aY,
                        const TAlfTimedValue& aZ);
    
    
    /**
     * 2D rotation with timed value.
     *
     * @param aAngle How much and the duration.
     */
    IMPORT_C void Rotate(const TAlfTimedValue& aAngle);

    /**
     * 3D rotation.
     */
    IMPORT_C void Rotate(TReal32 aAngle, TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP;
    
    /**
     * 3D rotation with timed values.
     */
    IMPORT_C void Rotate(const TAlfTimedValue& aAngle, 
                         TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP;



private:

    // private constructors
    CAlfTransformation();
    void ConstructL(CAlfEnv& aEnv);

private: // data

    // Private data. Owned.
    struct TPrivateData;
    TPrivateData* iData;

    };


#endif // C_ALFTRANSFORMATION_H
