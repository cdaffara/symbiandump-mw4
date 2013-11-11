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
* Description:   Transformation sub session.
*
*/



#ifndef C_ALFSRVTRANSFORMATIONSUBSESSION_H
#define C_ALFSRVTRANSFORMATIONSUBSESSION_H

#include "alfsrvsubsessionbase.h"

class CAlfAppSrvSessionBase;
class CHuiTransformation;

/**
 *  Transformation sub session.
 *
 *  @lib alfappservercore.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfSrvTransformationSubSession): public CAlfSrvSubSessionBase
    {

public:

    /**
     * Two-phased constructor
     *
     * @param aSession Session in which this sub session belongs into.
     * @return New object. Ownership transferred.
     */
    static CAlfSrvTransformationSubSession* NewL( CAlfAppSrvSessionBase& aSession );

    /**
     * Two-phased constructor. Leaves the objenct onto the cleanup stack.
     *
     * @param aSession Session in which this sub session belongs into.
     * @return New object on stack. Ownership transferred.
     */
    static CAlfSrvTransformationSubSession* NewLC( CAlfAppSrvSessionBase& aSession );

    /**
     * Destructor.
     */
    virtual ~CAlfSrvTransformationSubSession();

    /**
     * Returns the used transformation.
     *
     * @return Used transformation.
     */
    CHuiTransformation& Transformation();
    
    /**
     * Sets the used transformation.
     *
     * @param aTransformation Used transformation.
     */
    void SetTransformation( CHuiTransformation* aTransformation );
    
    /**
     * From CAlfSrvSubSessionBase
     * Processes the message.
     *
     * @param aMessage The message.
     */
    void DoProcessMessageL(const RMessage2& aMessage);

private:

    CAlfSrvTransformationSubSession( CAlfAppSrvSessionBase& aSession );

    void ConstructL();

    void LoadIdentityL(const RMessage2& aMessage);
    void TranslateL(const RMessage2& aMessage);
    void RotateL(const RMessage2& aMessage);
    void CountL(const RMessage2& aMessage);
    void NonIdentityCountL(const RMessage2& aMessage);
    void StepL(const RMessage2& aMessage);
    void ReplaceStepL(const RMessage2& aMessage);
    void Rotate3DL(const RMessage2& aMessage);
    void Scale3DL(const RMessage2& aMessage);
    void Translate2DL(const RMessage2& aMessage);

private: // data
    
    CHuiTransformation* iTransformation;
    };

#endif // C_ALFSRVTRANSFORMATIONSUBSESSION_H
