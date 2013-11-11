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
* Description:  alfperfapp abstract test case class definition.
*
*/


#ifndef C_ALFPERFAPPTESTCASE_H
#define C_ALFPERFAPPTESTCASE_H

#include <e32base.h>
#include <eikenv.h>

#include "alfperfappmodel.h"
class CAlfEnv;

/**
 * MAlfPerfAppTestCaseInterface class.
 * It is interface from test case to execution environment. It allows
 * test case to ask measurements to be performed.
 */
class MAlfPerfAppTestCaseInterface
    {
public:
    
    /**
     * Asks execution enrivonment to perform measurements.
     */
    virtual void MeasureNow() = 0;
    
    };

/**
 *  CAlfPerfAppTestCase class.
 *  It's abstract class from which all concrete test cases needs to be derived
 *  from.
 *  Destruction of an instance of this type must complete ongoing requests.
 */
class CAlfPerfAppTestCase : public CBase
    {
public:

    /**
     * Sets interface towards execution environment.
     */
    inline void SetInterface( MAlfPerfAppTestCaseInterface* aInterface );
    
    /**
     * Starts set up phase.
     * @param aEnv reference to alf environment instance.
     * @param aVisibleArea visible area.
     * @param aStatus request status that needs to be completed when
     *                test case is ready to be executed.
     */
    virtual void SetupL( 
        CAlfEnv& aEnv, 
        const TRect& aVisibleArea, 
        TRequestStatus& aStatus ) = 0;

    /**
     * Starts test case execution.
     * @param aStatus request status that needs to be completed when
     *                execution is finished.
     */
    virtual void ExecuteL( TRequestStatus& aStatus ) = 0;

    /** 
     * Tears down. 
     */
    virtual void TearDown() = 0;

    /**
     * Handles visible area changes.
     * @param aRect new visible area rect.
     */
    virtual void HandleVisibleAreaChange( const TRect& aRect ) = 0;
    
    /**
     * Returns CaseID of this test case
     */
    virtual TInt CaseID() = 0;
   
    /*
     * Used to get test case specific results.
     * Default definition just returns empty TTestCaseSpecificResultText
     * This should be reimplemented in actual test case classes if case specific results are needed.
     * Notes:
     *  - Latest non-zero returning value of this is shown in DisplayResults. All are still
     *    recorded to file. 
     */
    virtual TTestCaseSpecificResultText getCaseSpecificResultL();
    
    
    TInt SequenceIndex();
    
protected:

    /**
     * Perform measurements.
     */
    inline void MeasureNow();

    /**
     * Completes request status now.
     * @param aStatus request status to be completed.
     * @param aErrorCode completion error code to be used.
     */
    inline static void CompleteNow( TRequestStatus& aStatus, TInt aErrorCode );

    /* 
     * Gives access to stored CEikonEnv pointer
     */
    CEikonEnv* EikonEnv();
    
    /*
     * Protected constructor
     */
    CAlfPerfAppTestCase(TInt aSequenceIndex);
    
private:
    /**
     * Interface towards execution environment.
     * Not owned.
     */
    MAlfPerfAppTestCaseInterface* iInterface;
    
    /*
     * Pointer to CEikonEnv.
     */
    CEikonEnv* iEnv;
    
    /*
     * Tells which iteration of this same case this is.
     * 0 Means that this is not a sequence case.
     */
    TInt iSequenceIndex;
    
    };

inline void CAlfPerfAppTestCase::SetInterface( 
        MAlfPerfAppTestCaseInterface* aInterface )
    {
    iInterface = aInterface;
    }

inline void CAlfPerfAppTestCase::MeasureNow()
    {
    if ( iInterface )
        {
        iInterface->MeasureNow();
        }
    }

inline void CAlfPerfAppTestCase::CompleteNow( TRequestStatus& aStatus, TInt aErrorCode )
    {
    __ASSERT_ALWAYS( aStatus.Int() == KRequestPending, User::Invariant() );
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, aErrorCode );
    }

#endif // C_ALFPERFAPPTESTCASE_H

