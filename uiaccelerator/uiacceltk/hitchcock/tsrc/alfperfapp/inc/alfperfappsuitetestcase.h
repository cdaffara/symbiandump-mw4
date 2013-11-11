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
* Description:  
*
*/

#ifndef __ALFPERFAPPSUITETESTCASE_H__
#define __ALFPERFAPPSUITETESTCASE_H__

// INCLUDES
#include <e32base.h>
#include "alfperfapptestcase.h"

// FORWARD DECLARTIONS
class CAlfPerfAppSuiteTestCaseControl;

// CLASS DECLARATION
class CAlfPerfAppSuiteTestCase : public CAlfPerfAppTestCase
    {
	public: // Construction & destruction
		
		/**
		 * Construction
		 * @param aCaseId Testcase id.
		 */
	    static CAlfPerfAppSuiteTestCase* NewL( TInt aCaseId, TInt aSequenceIndex);
	    
		/**
		 * Destructor.
		 */	    
	    virtual ~CAlfPerfAppSuiteTestCase();
	    
	private: // Construction
	    
		/**
		 * C++ constructor.
		 */		
		CAlfPerfAppSuiteTestCase( TInt aCaseId, TInt aSequenceIndex );
		
		/**
		 * Second phase constructor.
		 */
	    void ConstructL();	    
		
	public: // from CAlfPerfAppTestCase
	    /**
	     * Starts set up phase.
	     * @param aEnv reference to alf environment instance.
	     * @param aVisibleArea visible area.
	     * @param aStatus request status that needs to be completed when
	     *                test case is ready to be executed.
	     */
	    void SetupL( 
	        CAlfEnv& aEnv, 
	        const TRect& aVisibleArea, 
	        TRequestStatus& aStatus );

	    /**
	     * Starts test case execution.
	     * @param aStatus request status that needs to be completed when
	     *                execution is finished.
	     */
	    void ExecuteL( TRequestStatus& aStatus );

	    /** 
	     * Tears down. 
	     */
	    void TearDown();

	    /**
	     * Handles visible area changes.
	     * @param aRect new visible area rect.
	     */
	    void HandleVisibleAreaChange( const TRect& aRect );
	    virtual TInt CaseID();
	    
	public: // New functions
		
		/**
		 * Tests if a testcase is supported.
		 * @param aCaseId Testcase id.
		 * @return ETrue if supported.
		 */
		static TBool IsSupported( TInt aCaseId );

		/**
		 * sends request completion after successful loading of all textures
		 * @param aErrorCode err code of texture loading.
		 */
		void ImagesLoaded( TInt aErrorCode );

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
	     * Testcase control
	     */
	    CAlfPerfAppSuiteTestCaseControl* iControl;

	    /**
	     * Testcase state change status. To indicate end of Setup state.
	     */
	    TRequestStatus* iStatus;
    };

#endif // __ALFPERFAPPSUITETESTCASE_H__

// end of file

