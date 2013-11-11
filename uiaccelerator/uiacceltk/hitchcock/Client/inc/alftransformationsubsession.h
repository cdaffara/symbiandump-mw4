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
* Description:   Sub-session for transformation
*
*/



#ifndef R_ALFTRANSFORMATIONSUBSESSION_H
#define R_ALFTRANSFORMATIONSUBSESSION_H

#include <e32std.h>
#include "alfclient.h"
#include "alf/alftransformation.h"

/**
 *  Sub-session for transformation
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RAlfTransformationSubSession ): public RSubSessionBase 
    {

public:

    /**
     * Open subsession for given session.
     *
     * @param aSession Used session.
     * @return Error code.
     */
    TInt Open( RAlfClient& aSession );
    
    /**
     *  Closes the subsession.
     */
    void Close();

    /**
     * Loads transformation identity matrix i.e. reset.
     */
    void LoadIdentity();
    
    /**
     * 2D translation.
     *
     * @param aX X coordinate.
     * @param aY Y coordinate.
     */
    void Translate( TReal32 aX, TReal32 aY );
    
    /**
     * 2D rotation.
     *
     * @param aAngle Rotation angle.
     */
    void Rotate( const TAlfTimedValue& aAngle );

    /**
     * Returns the number of transform steps in the transformation.
     */
    TInt Count() const;
    
    /**
     * Returns the number of non-identity transform steps in the transformation.
     * Non-identity steps will result in the transformation having an effect
     * on geometry.
     */
    TInt NonIdentityCount() const;

    /**
     * Returns a one of the transformation steps so that it 
     * can be modified and set with SetStep method.
     *
     * @param aIndex  Index of the transformation step.
     *
     * @return  Transformation step. 
     */
    CAlfTransformation::TTransform Step(TInt aIndex);

    /**
     * Modifies a one of the transformation steps by replacing
     * the existing step with a given step.
     *
     * @param aIndex  Index of the transformation step to be replaced
     *
     * @return  Transformation step. 
     */
    void ReplaceStep(TInt aIndex, CAlfTransformation::TTransform aTransformationStep);

    /**
     * 3D rotation with timed values.
     */
    void Rotate(const TAlfTimedValue& aAngle, 
                         TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ);
    
    /**
     * 2D translation with timed values.
     */
    void Translate(const TAlfTimedValue& aX, const TAlfTimedValue& aY);

    /**
     * 3D scaling with timed values.
     */
    void Scale(const TAlfTimedValue& aX, const TAlfTimedValue& aY,
                        const TAlfTimedValue& aZ);
    };


#endif // R_ALFTRANSFORMATIONSUBSESSION_H
