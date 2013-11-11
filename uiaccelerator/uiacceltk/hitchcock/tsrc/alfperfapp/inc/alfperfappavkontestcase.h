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
* Description:  alfperfapp Avkon test case class definition.
*
*/


#ifndef C_ALFPERFAPPAVKONTESTCASE_H
#define C_ALFPERFAPPAVKONTESTCASE_H

#include <e32base.h>
#include "alfperfapptestcase.h"

class CAlfPerfAppBaseTestCaseControl;

/**
 *  CAlfPerfAppAvkonTestCase class.
 */
class CAlfPerfAppAvkonTestCase : public CAlfPerfAppTestCase
    {
public:
    /**
     * Returns ETrue if test case is supported.
     * @param aCaseId test case id.
     * @return ETrue if supported.
     */
    static TBool IsSupported( TInt aCaseId );

    static CAlfPerfAppAvkonTestCase* NewL( TInt aCaseId, TInt aSequenceIndex );
    ~CAlfPerfAppAvkonTestCase();

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
    
    CAlfPerfAppAvkonTestCase( TInt aCaseId, TInt aSequenceIndex);
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

#endif // C_ALFPERFAPPAVKONTESTCASE_H

