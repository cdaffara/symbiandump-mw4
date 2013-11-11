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
* Description:  alfperfapp image test case class definition.
*
*/


#ifndef C_ALFPERFAPPIMAGETESTCASE_H
#define C_ALFPERFAPPIMAGETESTCASE_H

#include <e32base.h>
#include "alfperfapptestcase.h"

class CAlfPerfAppBaseTestCaseControl;

/**
 *  CAlfPerfAppImageTestCase class.
 */
class CAlfPerfAppImageTestCase : public CAlfPerfAppTestCase
    {
public:
    /**
     * Returns ETrue if test case is supported.
     * @param aCaseId test case id.
     * @return ETrue if supported.
     */
    static TBool IsSupported( TInt aCaseId );

    static CAlfPerfAppImageTestCase* NewL( TInt aCaseId, TInt aSequenceIndex );
    ~CAlfPerfAppImageTestCase();

    // From base class CAlfPerfAppTestCase:
    virtual void SetupL( 
        CAlfEnv& aEnv, 
        const TRect& aVisibleArea, 
        TRequestStatus& aStatus );
    virtual void ExecuteL( TRequestStatus& aStatus );
    virtual void TearDown();   
    virtual void HandleVisibleAreaChange( const TRect& aRect );
    virtual TInt CaseID();
    
   TTestCaseSpecificResultText getCaseSpecificResultL();
    
private:
    
    CAlfPerfAppImageTestCase( TInt aCaseId, TInt aSequenceIndex);
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
    
    /*
     *  Total Visual Count, used only with sequence cases
     */
    TInt iTotalVisualCount;
    TInt iAnimationDivider;
    };

#endif // C_ALFPERFAPPIMAGETESTCASE_H

