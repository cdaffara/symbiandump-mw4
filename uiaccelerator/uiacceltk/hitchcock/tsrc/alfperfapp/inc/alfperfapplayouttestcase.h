/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAlfPerfAppLayoutTestCase class definition.
*
*/


#ifndef C_ALFPERFAPPLAYOUTTESTCASE_H
#define C_ALFPERFAPPLAYOUTTESTCASE_H

#include <e32base.h>
#include "alfperfapptestcase.h"

class CAlfPerfAppBaseTestCaseControl;

/**
 *  CAlfPerfAppLayoutTestCase class.
 */
class CAlfPerfAppLayoutTestCase : public CAlfPerfAppTestCase
    {
public:
    /**
     * Returns ETrue if test case is supported.
     * @param aCaseId test case id.
     * @return ETrue if supported.
     */
    static TBool IsSupported( TInt aCaseId );

    static CAlfPerfAppLayoutTestCase* NewL( TInt aCaseId, TInt aSequenceIndex );
    ~CAlfPerfAppLayoutTestCase();

    // From base class CAlfPerfAppTestCase:
    virtual void SetupL( 
        CAlfEnv& aEnv, 
        const TRect& aVisibleArea, 
        TRequestStatus& aStatus );
    virtual void ExecuteL( TRequestStatus& aStatus );
    virtual void TearDown();   
    virtual void HandleVisibleAreaChange( const TRect& aRect );
    virtual TInt CaseID();
    
private:
    
    CAlfPerfAppLayoutTestCase( TInt aCaseId, TInt aSequenceIndex );
    void ConstructL();

private:
    /**
     * Test case of which execution this instance was created.
     */ 
    TInt iCaseId;
    
    /**
     * Alf environment.
     * Not owned.
     */
    CAlfEnv* iEnv;

    /**
     * Test case control.
     * Not owned.
     */
    CAlfPerfAppBaseTestCaseControl* iControl;

    };

#endif // C_ALFPERFAPPLAYOUTTESTCASE_H

