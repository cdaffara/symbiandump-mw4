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
* Description:  alfperfapp test case selection view class definition.
*
*/


#ifndef C_ALFPERFAPPTESTCASESELECTIONVIEW_H
#define C_ALFPERFAPPTESTCASESELECTIONVIEW_H

#include <aknview.h>
#include <coecntrl.h>
#include <eiksbobs.h>

#include "alfperfappmodel.h"
#include "alfperfappconstants.h"

class CEikRichTextEditor;
class CEikScrollBarFrame;
class CAlfPerfAppModel;

/**
 *  CAlfPerfAppTestCaseSelectionView view class.
 */
class CAlfPerfAppTestCaseSelectionView : public CAknView
    {
public:

    static CAlfPerfAppTestCaseSelectionView* NewLC( CAlfPerfAppModel& aModel );
    ~CAlfPerfAppTestCaseSelectionView();
    
    /*
     * Calculates time difference between current and previous.
     * @param aCurrent current timestamp (ms).
     * @param aPrevious previous timestamp (ms).
     * @return time difference (0xFFFFFFFF in case of overflow).
     */
    static TUint32 DeltaFromCurrentToPrevious( 
        TUint32 aCurrent, TUint32 aPrevious );

private:

    CAlfPerfAppTestCaseSelectionView( CAlfPerfAppModel& aModel );
    void ConstructL();

    /**
     * Run all test cases.
     */
    void CmdRunAllL();

    /**
     * Run all test cases in continuous loop
     */
    void CmdRunAllContinuouslyL();
    
    /**
     * Run all tests from a suite.
     * @param aSuiteIndex index to suite.
     */
    void CmdRunAllFromSuiteL( TInt aSuiteIndex );

    /**
     * Runs one test case.
     * @param aSuiteIndex index to suite.
     * @param aTestCaseIndex test case index inside suite.
     */	
    void CmdRunTestCaseL( TInt aSuiteIndex, TInt aTestCaseIndex );
    
    void DisplayResultsL();

private:

    // From base class CAknView
    virtual TUid Id() const;
    virtual void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );
    virtual void DoDeactivate();
    virtual void HandleCommandL( TInt aCommand );
    virtual void RestoreMenuL( CCoeControl* aMenuControl, TInt aResourceId,
        TMenuType aType );
    virtual void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);  
    
    CAlfPerfAppModel::TTestCaseResultItem ResultItemFromLine(TBuf<KAlfPerfAppMaxCharsInLine>& aLine);
    TBuf8<KAlfPerfAppMaxCharsInLine> getLineFromTargetFile();                      

    void ResetContainerL();
    
private:

    /**
     * Reference to model.
     * Not owned.
     */
    CAlfPerfAppModel& iModel;
    
    /**
     * Container control class.
     */
    class CContainer;
    
    /**
     * Container object.
     */
    CContainer* iContainer;
    
    /**
     * Current resource id.
     */
    TInt iResourceId;
    };

#endif // C_ALFPERFAPPTESTCASESELECTIONVIEW_H

