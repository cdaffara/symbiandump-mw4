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
* Description:  CAlfPerfAppBaseTestCaseControl class definition.
*
*/


#ifndef C_ALFPERFAPPBASETESTCASECONTROL_H
#define C_ALFPERFAPPBASETESTCASECONTROL_H

#include <e32base.h>
#include <alf/alfcontrol.h>

/**
 *  CAlfPerfAppBaseTestCase class.
 */
/**
 * Abstract image test case control.
 */
class CAlfPerfAppBaseTestCaseControl : public CAlfControl
    {
public:
        
    /**
     * Constructor.
     */
    CAlfPerfAppBaseTestCaseControl();
        
    /**
     * Destructor.
     */
    ~CAlfPerfAppBaseTestCaseControl();
        
    /**
     * Base class constructor. Derived classes may
     * override this, but base class needs to be called.
     */
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );
        
    /**
     * Starts execution of the test case. By default,
     * request status is stored to local variable.
     */
    void StartExecuteL( TRequestStatus& aStatus );    
    
    /**
     * Cancels execution. This control and environment will be
     * deleted soon after calling this method.
     */
    void CancelExecution();
    
    /**
     * Starts execution.
     * If this method leaves, then request must not be completed.
     * By default, this method completes immediately.
     */
    virtual void DoStartExecuteL();
    
    /**
     * Sets visible area.
     * @param aVisibleArea visible area.
     */
    virtual void SetVisibleArea( const TRect& aVisibleArea );    
    
    // From base class CAlfControl:
    
    /**
     * Handles events.
     * Derived classes should forward to base class.
     * @param aEvent event to be handled.
     * @return ETrue if consumed, EFalse otherwise.
     */
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    
protected:
        
    /**
     * Completes automatically after specified duration.
     * @param aDuration duration in ms
     */
    void CompleteAfterL( TInt aDuration );
        
    /**
     * Completes current request.
     */
    void CompleteNow( TInt aErrorCode );

    /**
     * Returns test case id.
     */
    TInt CaseId() const
    	{
    	return iCaseId;
    	}
        
    /**
     * Returns ETrue if test case execution is still ongoing.
     */
    TBool IsExecutionOngoing() const
    	{
    	return ( iStatus != NULL );
    	}
    
private:
    
    /**
     * Test case id.
     */
    TInt iCaseId;
    
    /**
     * Pointer to request status.
     * Not owned.
     */
    TRequestStatus* iStatus;
        
    };

        
#endif // C_ALFPERFAPPBASETESTCASECONTROL_H

