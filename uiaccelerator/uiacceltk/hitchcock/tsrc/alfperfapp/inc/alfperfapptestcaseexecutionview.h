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
* Description:  alfperfapp test case runner view class definition.
*
*/


#ifndef C_ALFPERFAPPTESTCASEEXECUTIONVIEW_H
#define C_ALFPERFAPPTESTCASEEXECUTIONVIEW_H

#include <aknview.h>
#include <apaserverapp.h>

class CAlfPerfAppModel;
class CAlfPerfAppTestCaseFactory;
struct TAlfDebugServerMeasurements;
class RAlfPerfAppDaemon;
   
/**
 *  CAlfPerfAppTestCaseExecutionView view class.
 */
class CAlfPerfAppTestCaseExecutionView : public CAknView
    {
public:

    static CAlfPerfAppTestCaseExecutionView* NewLC( CAlfPerfAppModel& aModel );
    ~CAlfPerfAppTestCaseExecutionView();

    /**
     * This method is called by view container. This will notify
     * current test case about layout change.
     * @param aRect visible area rect.
     */
    void NotifyLayoutChange( const TRect& aRect );
    
    /**
     * This method is called when all test cases has been executed.
     */
    void NotifyExecutionCompletedL();
    
    /**
     * This method is called when test case should be shown to end-user.
     * @param aVisible ETrue if test case is visible, EFalse otherwise.
     */
    void NotifyTestCaseVisible( TBool aVisible );
    
    
private:

    CAlfPerfAppTestCaseExecutionView( CAlfPerfAppModel& aModel );
    void ConstructL();

private:

    // From base class CAknView
    virtual TUid Id() const;

    virtual void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );
    virtual void DoDeactivate();

    virtual void HandleCommandL( TInt aCommand );
        
private:

    /**
     * Reference to model.
     * Not owned.
     */
    CAlfPerfAppModel& iModel;
    
    /**
     * Test case factory.
     * Owned.
     */
    CAlfPerfAppTestCaseFactory* iFactory;

    /**
     * Container control class.
     */
    class CContainer;
    
    /**
     * Container object.
     * Owned.
     */
    CContainer* iContainer;
   
    /**
     * Active object class that is used to execute test cases.
     */
    class CActiveExecuter;
     
    /**
     * Active object for execution.
     * Owned.
     */
    CActiveExecuter* iExecuter;

    /**
     * Daemon for performance measurements.
     * Owned.
     */
    RAlfPerfAppDaemon* iDaemon;
    };

#endif // C_ALFPERFAPPTESTCASEEXECUTIONVIEW_H

