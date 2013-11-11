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
* Description:  CAlfPerfAppTestCaseSelectionView implementation.
*
*/


#include <aknutils.h>
#include <aknviewappui.h>
#include <alfperfapp.rsg>
#include <stringloader.h>
#include <AknBidiTextUtils.h>
#include <aknutils.h>
#include <aknnotewrappers.h>
#include <eiksbfrm.h>
#include <akndef.h>
#include <bautils.h> 
#include "alfperfapptestcaseselectionview.h"
#include "alfperfapp.hrh"
#include "alfperfappconstants.h"
#include "alfperfappmodel.h"

// Execution ongoing
_LIT( KAlfPerfAppExecuting, "Executing..\n" );

// Print output path
_LIT( KAlfPerfAppPrintOutputFormat, "Result files are in %S\n" );

// Error message display
_LIT( KAlfPerfAppNoErrors, "No errors.\n\n" );
_LIT( KAlfPerfAppErrorHeader, "Errors:\n" );
_LIT( KAlfPerfappErrorItem, "  %S\n" );
_LIT( KAlfPerfAppErrorFooter, "\n" );

// Results display
_LIT( KAlfPerfAppInfoNote, "Program records results directly to two files, results are appended to the end of each file.\n\n");
_LIT( KAlfPerfAppClearFilesQueryLine1, "Earlier results present.");
_LIT( KAlfPerfAppClearFilesQueryLine2, "Clear the result files?");
_LIT( KAlfPerfAppResultsHeader, 
        "*** Field Guide ***\n- Time spend\n- Mem.usage of app,server and system\n- Frame count and fps\n");
//_LIT( KAlfPerfAppResultsCaseStart, "%.2f s: Case: %x\n" );
_LIT( KAlfPerfAppResultsCaseStart, "Started at: %.2f s\n");
_LIT( KAlfPerfAppResultsCaseTitle, "\n=== %S / %S (0x%x) ===\n");
_LIT( KAlfPerfAppResultsNone, "No results.\n\n" );
_LIT( KAlfPerfAppSpecificResultFooter,"\n");

_LIT( KAlfPerfAppResultsPhaseTitle0,"\n*** Startup ***\n" );
_LIT( KAlfPerfAppResultsPhaseTitle1,"\n*** Run ***\n" );
_LIT( KAlfPerfAppResultsPhaseTitle2,"\n*** Teardown ***\n" );
_LIT( KAlfPerfAppResultsCaseTotalTitle,"\n*** Case total ***\n" );
_LIT( KAlfPerfAppResultsCaseItem,
        "Time: %.2f s\nMem.usage: %d/%d/%d kb\nFrames: %d (%3.1f fps)\n" );

// Buffer full
const TInt KAlfPerfAppDisplayResultMaxChars = 35000; //increase is needed for Sequence test cases
_LIT( KAlfPerfAppDisplayBufferFull,"\n*** Display buffer full ***\n");




/**
 * CAlfPerfAppTestCaseSelectionView::CContainer container class.
 */
class CAlfPerfAppTestCaseSelectionView::CContainer : public CCoeControl, MCoeControlObserver, MEikScrollBarObserver
    {
    public:
        CContainer(CAlfPerfAppModel* aModel);
        void ConstructL();
        ~CContainer();

    public: // New functions
        TBool PrintText(const TDesC& aDes);
        void ClearOutputWindowL();
        void UpdateVisualContentL(TBool aScrollToBottom);

    public: // From base classes
        virtual TKeyResponse OfferKeyEventL( 
                const TKeyEvent& aKeyEvent, TEventCode aType );
        virtual void HandleResourceChange( TInt aType );
        virtual void Draw(const TRect& aRect) const;
        virtual void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        virtual void HandleScrollEventL(CEikScrollBar* aScrollBar,TEikScrollEvent aEventType); // From MEikScrollBarObserver

    private:
        CEikScrollBarFrame*     iScrollBarFrame;
        HBufC*                  iText;
        CFont*                    iFont;
        HBufC*                  iWrapperString;
        CArrayFix<TPtrC>*       iWrappedArray;
        TInt                    iCurrentLine;
        TInt                    iLineCount;
        TInt                    iNumberOfLinesFitsScreen;   
        TInt                    iLeftDrawingPosition;
        TReal                   iX_factor;
        TReal                   iY_factor;
        TBool                   iNotPrintedResultDisplayBufferExceeded;
        CAlfPerfAppModel*       iModel;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CAlfPerfAppTestCaseSelectionView* CAlfPerfAppTestCaseSelectionView::NewLC(
        CAlfPerfAppModel& aModel )
    {
    CAlfPerfAppTestCaseSelectionView* self = 
    new (ELeave) CAlfPerfAppTestCaseSelectionView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CAlfPerfAppTestCaseSelectionView::~CAlfPerfAppTestCaseSelectionView()
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CAlfPerfAppTestCaseSelectionView::CAlfPerfAppTestCaseSelectionView(
        CAlfPerfAppModel& aModel )
: iModel( aModel )
       {
       }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::ConstructL()
            {
            BaseConstructL( R_ALFPERFAPP_TEST_CASE_SELECTION_VIEW );
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CmdRunAllL()
            {
            // Run all test cases (that are not excluded from 'run all').
            iModel.ResetErrorArray();
            if (iContainer)
                {
                iContainer->ClearOutputWindowL();
                iContainer->PrintText( KAlfPerfAppExecuting );
                }
            iModel.ResetExecuteArray();

            const TInt suiteCount = iModel.SuiteCount();
            for ( TInt ii = 0; ii < suiteCount; ii++ )
                {
                // If suite is not excluded from run all, 
                // add all test cases to execute array.
                if ( !( iModel.SuiteFlags( ii ) & 
                        EAlfPerfAppTestSuiteFlagExcludeFromRunAll ) )
                    {
                    // Add all test cases to execute array.
                    const TInt testCount = iModel.TestCaseCount( ii );
                    for ( TInt jj = 0; jj < testCount; jj++ )
                        {
                        TPtrC testName;
                        TInt testCaseId;
                        TInt testCaseFlags;
                        TInt testCaseSequenceLength;

                        iModel.GetTestCase(
                                ii,
                                jj,
                                testName,
                                testCaseId,
                                testCaseFlags,
                                testCaseSequenceLength);
                        
                        // Add the test case multiple times to fill the sequence (at least once though)
                        iModel.AddToExecuteArrayL( testCaseId );
                        for(TInt index = 1; index < testCaseSequenceLength;index++)
                            {
                            iModel.AddToExecuteArrayL( testCaseId );
                            }
                        }
                    }

                }

            AppUi()->ActivateLocalViewL( KAlfPerfAppTestCaseExecutionViewId );
            }



        void CAlfPerfAppTestCaseSelectionView::CmdRunAllContinuouslyL()
            {
            // Run all test cases (that are not excluded from 'run all').
            iModel.ResetErrorArray();
            if (iContainer)
                {
                iContainer->ClearOutputWindowL();
                iContainer->PrintText( KAlfPerfAppExecuting );
                }
            iModel.ResetExecuteArray();

            const TInt suiteCount = iModel.SuiteCount();
            for ( TInt ii = 0; ii < suiteCount; ii++ )
                {
                // If suite is not excluded from run all, 
                // add all test cases to execute array.
                if ( !( iModel.SuiteFlags( ii ) & 
                        EAlfPerfAppTestSuiteFlagExcludeFromRunAll ) )
                    {
                    // Add all test cases to execute array.
                    const TInt testCount = iModel.TestCaseCount( ii );
                    for ( TInt jj = 0; jj < testCount; jj++ )
                        {
                        TPtrC testName;
                        TInt testCaseId;
                        TInt testCaseFlags;
                        TInt testCaseSequenceLength;

                        iModel.GetTestCase(
                                ii,
                                jj,
                                testName,
                                testCaseId,
                                testCaseFlags,
                                testCaseSequenceLength);
                        
                        // Add the test case multiple times to fill the sequence (at least once though)
                        iModel.AddToExecuteArrayL( testCaseId );
                        for(TInt index = 1; index < testCaseSequenceLength;index++)
                            {
                            iModel.AddToExecuteArrayL( testCaseId );
                            }
                        }
                    }

                }

            // Finally tell ExecutionView to use Continuous Run
            const TUid KCustomMessageUid = {2};
            TBuf8<KAlfPerfAppMaxCharsInLine> customMessage;
            customMessage.Copy(_L8("UseContinuousRun"));
            AppUi()->ActivateLocalViewL( KAlfPerfAppTestCaseExecutionViewId,KCustomMessageUid,customMessage);
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CmdRunAllFromSuiteL( 
                TInt aSuiteIndex )
            {
            // Run all test cases related to specific suite.
            const TInt testCaseCount = iModel.TestCaseCount( aSuiteIndex );
            iModel.ResetErrorArray();

            if (iContainer)
                {
                iContainer->ClearOutputWindowL();
                iContainer->PrintText( KAlfPerfAppExecuting );
                }

            iModel.ResetExecuteArray();

            for ( TInt ii = 0; ii < testCaseCount; ii++ )
                {
                TPtrC testCaseName;
                TInt testCaseId;
                TInt testCaseFlags;
                TInt testCaseSequenceLength;

                iModel.GetTestCase(
                        aSuiteIndex,
                        ii,
                        testCaseName,
                        testCaseId,
                        testCaseFlags,
                        testCaseSequenceLength);

                // Add the test case multiple times to fill the sequence (at least once though)
                iModel.AddToExecuteArrayL( testCaseId );
                for(TInt index = 1; index < testCaseSequenceLength;index++)
                    {
                    iModel.AddToExecuteArrayL( testCaseId );
                    }

                AppUi()->ActivateLocalViewL( KAlfPerfAppTestCaseExecutionViewId );
                }
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CmdRunTestCaseL( 
                TInt aSuiteIndex, TInt aTestCaseIndex )
            {
            TPtrC testCaseName;
            TInt testCaseId;
            TInt testCaseFlags;
            TInt testCaseSequenceLength;

            iModel.GetTestCase(
                    aSuiteIndex,
                    aTestCaseIndex,
                    testCaseName,
                    testCaseId,
                    testCaseFlags,
                    testCaseSequenceLength);

            iModel.ResetErrorArray();
            if (iContainer)
                {
                iContainer->ClearOutputWindowL();
                iContainer->PrintText( KAlfPerfAppExecuting );
                }
            iModel.ResetExecuteArray();
            
            // Add the test case multiple times to fill the sequence (at least once though)
            iModel.AddToExecuteArrayL( testCaseId );
            for(TInt index = 1; index < testCaseSequenceLength;index++)
                {
                iModel.AddToExecuteArrayL( testCaseId );
                }
            
            AppUi()->ActivateLocalViewL( KAlfPerfAppTestCaseExecutionViewId );
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        TUid CAlfPerfAppTestCaseSelectionView::Id() const
        {
        return KAlfPerfAppTestCaseSelectionViewId;
        }

        // -----------------------------------------------------------------------------
        // Handles view activation
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::DoActivateL(
                const TVwsViewId& /*aPrevViewId*/,
                TUid /*aCustomMessageId*/,
                const TDesC8& /*aCustomMessage*/ )
            {
            ResetContainerL();

            DisplayResultsL();

            // Pop up the "Clear result files?"-dialog if either of the files existed.
            static TBool firstTime = ETrue;
            if(firstTime)
                {
                if(iModel.TargetFilesExisted())
                    {
                    if(CEikonEnv::Static()->QueryWinL(KAlfPerfAppClearFilesQueryLine1,KAlfPerfAppClearFilesQueryLine2))
                        {
                        // Clear the files
                        iModel.OpenFilesL(true);
                        }
                    firstTime = EFalse;
                    }
                }
            }

        // -----------------------------------------------------------------------------
        // Handles view deactivation
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::DoDeactivate()
            {
            if ( iContainer )
                {
                AppUi()->RemoveFromStack( iContainer );
                delete iContainer;
                iContainer = NULL;
                }
            }


        // -----------------------------------------------------------------------------
        // Display test results on screen. The text is wrapped.
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::DisplayResultsL()
             {
            if (iContainer)
                {
                TBuf<KAlfPerfAppMaxCharsInLine> line;
                iContainer->ClearOutputWindowL();

                const TInt errorCount = iModel.ErrorArrayCount();
                if ( !errorCount )
                    {
                    iContainer->PrintText( KAlfPerfAppNoErrors );
                    }
                else
                    {
                    iContainer->PrintText( KAlfPerfAppErrorHeader );
                    for ( TInt ii = 0; ii < errorCount; ii++ )
                        {
                        TPtrC message;
                        iModel.GetFromErrorArray( ii, message );
                        if ( message.Length() )
                            {
                            line.Zero();
                            TPtrC item = message.Left( 64 );
                            line.Format( KAlfPerfappErrorItem, &item );
                            iContainer->PrintText( line );
                            }
                        }
                    iContainer->PrintText( KAlfPerfAppErrorFooter );
                    }

                if (iModel.ResultsNotAddedToFile())
                    {           
                    iContainer->PrintText( KAlfPerfAppResultsNone );
                    iContainer->PrintText( KAlfPerfAppInfoNote );
                    }    
                else
                    {  
                    line.Zero();
                    line.Format( KAlfPerfAppResultsHeader ); 
                    iContainer->PrintText(line);

                    // Prepare to read lines
                    TFileText LineFromFile;
                    LineFromFile.Set(*iModel.ResultFile());
                    LineFromFile.Seek(ESeekStart);
                    
                    // Read the first line and forget it (header info only)
                    line.Copy(getLineFromTargetFile());
                    
                    TInt currentCase = 0;
                    TBool startTimeNotRecorded = ETrue;
                    TUint32 timeStampAtStart = 0;
                    TInt framesSinceCase = 0;
                    CAlfPerfAppModel::TTestCaseResultItem itemAtCase;
                    CAlfPerfAppModel::TTestCaseResultItem itemAtPhase;
                    TBuf8<KAlfPerfAppMaxCharsInSpecificResultText> specificResultStorage;
                    for(TInt i=0; ; i++) // i = line number in result file
                        {
                        line.Zero();

                        // Read a line from file and convert it to an TTestCaseResultItem
                        line.Copy(getLineFromTargetFile());
                        if(line.Length() == 0)
                            {
                            // If the line is empty, end this loop
                            break;
                            }
                        CAlfPerfAppModel::TTestCaseResultItem item;
                        item = ResultItemFromLine(line);
                        line.Zero();
                        
                        if(startTimeNotRecorded)
                            {
                            timeStampAtStart = item.iTimeStamp;
                            startTimeNotRecorded = EFalse;
                            }

                        TUint32 timeFromStart = 
                        DeltaFromCurrentToPrevious( 
                                item.iTimeStamp, timeStampAtStart );

                        TBool changedCase = EFalse;
                        if ( !i || currentCase != item.iCaseId )
                            {
                            currentCase = item.iCaseId;
                            itemAtCase = item;
                            framesSinceCase = 0;
                            changedCase = ETrue;
                            //specificResultStorage.Zero(); 
                            }

                        if(changedCase || itemAtPhase.iPhase != item.iPhase)
                            {
                            itemAtPhase = item;
                            }

                        if ( changedCase )
                            {
                            // Name
                            TBuf<100> suiteName;
                            suiteName.Copy(item.iSuiteName);
                            TBuf<100> caseName;
                            caseName.Copy(item.iCaseName);
                            line.Format( 
                                    KAlfPerfAppResultsCaseTitle,                           
                                    &suiteName,
                                    &caseName,
                                    item.iCaseId);
                            if(!iContainer->PrintText(line))break;
                            line.Zero();

                            // Start time
                            line.AppendFormat( KAlfPerfAppResultsCaseStart, 
                                    ((TReal32)timeFromStart/1000));
                            if(!iContainer->PrintText(line))break;
                            line.Zero();
                            }
                            
                        if(item.specificResult8.Length() != 0)
                            {
                            specificResultStorage.Zero();
                            specificResultStorage.Append(item.specificResult8);
                            }
                        
                        if(i&0x1) // Skip even lines
                            {
                            // Print phase data
                            if (item.iPhase == 0)
                                {
                                line.AppendFormat(KAlfPerfAppResultsPhaseTitle0);
                                }
                            else if (item.iPhase == 1)
                                {
                                line.AppendFormat(KAlfPerfAppResultsPhaseTitle1);
                                }
                            else
                                {
                                line.AppendFormat(KAlfPerfAppResultsPhaseTitle2);
                                }
                            
                            line.AppendFormat( KAlfPerfAppResultsCaseItem, 
                                    // duration
                                    ((TReal32)item.imsSincePhase)/1000,
                                    // memory consumption delta
                                    ( item.iAppMemory - itemAtPhase.iAppMemory )/ 1024, 
                                    ( item.iServerMemory - itemAtPhase.iServerMemory )/ 1024, 
                                    ( item.iSystemMemory - itemAtPhase.iSystemMemory )/ 1024,
                                    // frame info
                                    item.iFrameCount,
                                    item.iFPS );
                            if(!iContainer->PrintText(line))break;
                            line.Zero();
                            
                            // Testcase specific results
                            if(specificResultStorage.Length() != 0)
                                {
                                TBuf<KAlfPerfAppMaxCharsInSpecificResultText> specificResult16;
                                specificResult16.Copy(specificResultStorage);
                                line.Append(specificResult16);
                                line.Append(KAlfPerfAppSpecificResultFooter);
                                if(!iContainer->PrintText(line))break;
                                specificResultStorage.Zero();
                                line.Zero();
                                }
                            

                            framesSinceCase += item.iFrameCount;
                            if (item.iPhase == 2)
                                {
                                // Print case data
                                // need to be done like this because of fpsSinceCase is not
                                // recorded to file
                                TUint32 timeFromCase = item.imsSinceCase;

                                TReal32 fpsSinceCase = 0.f;
                                if ( timeFromCase != 0 )
                                    {
                                    fpsSinceCase = ((TReal32)framesSinceCase*1000)/timeFromCase;
                                    }

                                line.AppendFormat(KAlfPerfAppResultsCaseTotalTitle);
                                if(!iContainer->PrintText(line))break;
                                
                                line.Format( KAlfPerfAppResultsCaseItem, 
                                        // duration
                                        ((TReal32)timeFromCase/1000),
                                        // memory consumption delta
                                        ( item.iAppMemory - itemAtCase.iAppMemory )/ 1024, 
                                        ( item.iServerMemory - itemAtCase.iServerMemory )/ 1024, 
                                        ( item.iSystemMemory - itemAtCase.iSystemMemory )/ 1024,
                                        // frame info
                                        framesSinceCase,
                                        fpsSinceCase );

                                if(!iContainer->PrintText(line))break;
                                line.Zero();
                                }	                

                            }
                        }
                    }
             
                TBuf<KAlfPerfAppMaxCharsInLine> temp = iModel.TargetPath();
            	line.Format( KAlfPerfAppPrintOutputFormat, &temp);
            	iContainer->PrintText(line);

            	// Finally update results to screen            
            	iContainer->UpdateVisualContentL(ETrue);
            	}
            }

        
        // -----------------------------------------------------------------------------
        // Handles test case selection view specific commands.
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::HandleCommandL( TInt aCommand )
            {
            switch ( aCommand )
                {
                case EAlfPerfAppCmdRunAll:
                    CmdRunAllL();
                    break;

                case EAlfPerfAppCmdRunAllContinuously:
                    CmdRunAllContinuouslyL();
                    break;
                   
                case EAlfPerfAppCmdClearFiles:
                    
                    TBuf<1> empty;
                    if(CEikonEnv::Static()->QueryWinL(KAlfPerfAppClearFilesQueryLine2,empty))
                        {
                        // Clear the files
                        iModel.OpenFilesL(true);
                        ResetContainerL();
                        DisplayResultsL();
                        }
                   
                    break;
                   

                default:
                    {
                    if ( ( aCommand & ~0xFFFF ) == EAlfPerfAppCmdTestCase )
                        {
                        const TInt testSuiteIndex = ( aCommand & 0xFF00 ) >> 8;
                        TInt testCaseIndex = aCommand & 0xFF;

                        if ( !testCaseIndex )
                            {
                            CmdRunAllFromSuiteL( testSuiteIndex );
                            }
                        else
                            {
                            // Run one specific test case
                            testCaseIndex--;
                            CmdRunTestCaseL( testSuiteIndex, testCaseIndex );                   
                            }
                        }
                    else
                        {
                        AppUi()->HandleCommandL( aCommand );
                        }
                    }
                    break;
                }
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::RestoreMenuL( 
                CCoeControl* aMenuControl, TInt aResourceId, TMenuType aType )
            {
            // Override resource in case of test suite menu
            iResourceId = 0;

            if ( ( aResourceId & ~0xFFFF ) == KAlfPerfAppMenuPaneResourceId )
                {
                iResourceId = aResourceId;
                aResourceId = R_ALFPERFAPP_TEST_CASE_SELECTION_SUITE_MENUPANE;
                }

            CAknView::RestoreMenuL( 
                    aMenuControl, 
                    aResourceId,
                    aType );
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane )
            {
            if ( aResourceId == R_ALFPERFAPP_TEST_CASE_SELECTION_MENUPANE )
                {
                // Add all suites to top level options menu.
                const TInt suiteCount = iModel.SuiteCount();
                for ( TInt ii = suiteCount - 1; ii >= 0; ii-- )
                    {
                    CEikMenuPaneItem::SData data;
                    data.iCommandId = 0;
                    data.iCascadeId = KAlfPerfAppMenuPaneResourceId + (ii << 8);
                    data.iFlags = 0;        
                    data.iText.Copy( iModel.SuiteName( ii ) );
                    data.iExtraText.Zero();

                    aMenuPane->AddMenuItemL( data, EAlfPerfAppCmdRunAllContinuously );
                    }
                }

            if ( aResourceId == R_ALFPERFAPP_TEST_CASE_SELECTION_SUITE_MENUPANE )
                {
                const TInt suiteIndex = ( iResourceId & 0xFF00 ) >> 8;
                const TInt testCaseCount = iModel.TestCaseCount( suiteIndex );

                // Add 'Run All' option
                HBufC* runAllText = StringLoader::LoadLC( R_ALFPERFAPP_TEXT_RUNALL );
                CEikMenuPaneItem::SData runAll;
                runAll.iCommandId = EAlfPerfAppCmdTestCase + (suiteIndex << 8);
                runAll.iCascadeId = 0;
                runAll.iFlags = 0;           
                runAll.iText.Copy( *runAllText );
                runAll.iExtraText.Zero();
                aMenuPane->AddMenuItemL( runAll );
                CleanupStack::PopAndDestroy( runAllText );

                // Add options menu items for all test cases of this suite        
                for ( TInt ii = 0; ii < testCaseCount; ii++ )
                    {
                    CEikMenuPaneItem::SData data;
                    data.iCommandId = EAlfPerfAppCmdTestCase + (suiteIndex << 8) + ii + 1;
                    data.iCascadeId = 0;
                    data.iFlags = 0;

                    TPtrC testCaseName;
                    TInt testCaseId;
                    TInt flags;
                    TInt sequenceLength;

                    iModel.GetTestCase( 
                            suiteIndex, 
                            ii, 
                            testCaseName,
                            testCaseId,
                            flags,
                            sequenceLength);

                    data.iText.Copy( testCaseName );
                    data.iExtraText.Zero();

                    aMenuPane->AddMenuItemL( data );
                    }
                }
            }

        TBuf8<KAlfPerfAppMaxCharsInLine> CAlfPerfAppTestCaseSelectionView::getLineFromTargetFile()
            {
            TBuf8<KAlfPerfAppMaxCharsInLine> line;
            TBuf8<1> character;
            iModel.ResultFile()->Read(character,1);
            if(character.Length() == 0)
                {
                // File was empty --> return empty line
                return line;
                }
            while(character[0]  != '\n')
                {
                line.Append(character);
                iModel.ResultFile()->Read(character,1);
                }
            return line;
            }
        
        void CAlfPerfAppTestCaseSelectionView::ResetContainerL()
            {
            // Reset the container
            if ( iContainer )
                {
                AppUi()->RemoveFromStack( iContainer );
                delete iContainer;
                iContainer = NULL;
                }
            CContainer* container = new (ELeave) CContainer(&iModel);
            CleanupStack::PushL( container );
            container->ConstructL();
            AppUi()->AddToStackL( *this, container );
            CleanupStack::Pop( container );
            iContainer = container;
            }
        
        
        // Let's create a function template for extracting items from lexer
        TBuf<KAlfPerfAppMaxCharsInLine>& ProcessLexer(TLex& aLexer, TChar aSeparator,TBuf<KAlfPerfAppMaxCharsInLine>& aToken)
            {
            aToken.Zero();
            TChar ch;
            aLexer.SkipAndMark(0);
            while (!aLexer.Eos())
                {
                ch = aLexer.Get();                    
                if(ch == aSeparator)
                    {
                    aLexer.UnGet();
                    aToken.Append(aLexer.MarkedToken());
                    aLexer.SkipAndMark(1);
                    break;
                    }
                }
            return aToken;
            }

        CAlfPerfAppModel::TTestCaseResultItem CAlfPerfAppTestCaseSelectionView::ResultItemFromLine(TBuf<KAlfPerfAppMaxCharsInLine>& aLine)
            {
            CAlfPerfAppModel::TTestCaseResultItem item;

            TBuf<KAlfPerfAppMaxCharsInLine> temp;
            TLex lexer(aLine);
            TLex tempLex;
            
            item.iSuiteName.Copy(ProcessLexer(lexer,'/',temp));
            item.iSuiteName.Trim();
            item.iCaseName.Copy(ProcessLexer(lexer,',',temp));
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            item.iCaseName.Trim();
            tempLex.Val(item.iCaseId);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iPhase);           
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iTimeStamp,EDecimal);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iAppCells);   
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iAppMemory);   
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iAppFree);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iServerCells);   
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iServerMemory);   
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iServerFree);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iSystemMemory);   
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iSystemFree);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.iFrameCount,EDecimal);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.imsSinceCase,EDecimal);
            tempLex.Assign(ProcessLexer(lexer,',',temp));
            tempLex.Val(item.imsSincePhase,EDecimal);
            TBuf<15> fpsBuffer (ProcessLexer(lexer,',',temp));
            tempLex.Assign(fpsBuffer);
            tempLex.Val(item.iFPS);
            item.specificResult8.Copy(lexer.Remainder());
            item.specificResult8.Delete(item.specificResult8.Length()-1,1);

            return item;
            }



        // Implementation of DeltaFromCurrentToPrevious
        TUint32 CAlfPerfAppTestCaseSelectionView::DeltaFromCurrentToPrevious( 
                TUint32 aCurrent, TUint32 aPrevious )
            {
            // It's expected that execution of all test suites take less
            // than KMaxTInt ms (so that it fits to TInt).

            TUint64 delta = 0;
            if ( aPrevious <= aCurrent )
                {
                // aPrevious .. aCurrent
                delta = aCurrent - aPrevious;
                }
            else
                {
                // Overflow has occurred.
                // aPrevious .. 2**32 + aCurrent

                delta = 0xFFFFFFFF;
                delta -= aPrevious;
                delta++;
                delta += aCurrent;        
                }

            if ( delta >= KMaxTInt )
                {
                delta = KMaxTInt;
                }

            return I64LOW( delta );
            }


        // Implementation of CAlfPerfAppTestCaseSelectionView::CContainer:
        // -----------------------------------------------------------------------------
        //
        CAlfPerfAppTestCaseSelectionView::CContainer::CContainer(CAlfPerfAppModel* aModel)
            {
                iModel = aModel;
            }

        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::ConstructL()
            {
            CreateWindowL();

            TRect rect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
            SetRect( rect );

            iText = HBufC::NewL(KAlfPerfScreenTextBufferSize);
            iCurrentLine = 0;
            iLineCount = 0;
            iNumberOfLinesFitsScreen = 0;
            iX_factor = 1;
            iY_factor = 1;
            iLeftDrawingPosition = KAlfPerfAppLeftMargin;

            TFontSpec myFontSpec(KAlfPerfAppFontName,KAlfPerfAppFontSizeInTwips); 
            CGraphicsDevice* screenDevice=iCoeEnv->ScreenDevice();
            screenDevice->GetNearestFontInTwips(iFont,myFontSpec);

            iWrappedArray = new(ELeave) CArrayFixFlat<TPtrC>(KAlfPerfScreenWrappedArraySize);

            iNotPrintedResultDisplayBufferExceeded = ETrue;

            ActivateL();
            }



        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        CAlfPerfAppTestCaseSelectionView::CContainer::~CContainer()
            {
            // Destroy the font
            iCoeEnv->ScreenDevice()->ReleaseFont(iFont);

            if (iWrappedArray)
                {
                iWrappedArray->Reset();
                delete iWrappedArray;
                }

            if (iText)
                delete iText;

            if (iScrollBarFrame)
                delete iScrollBarFrame;

            if (iWrapperString)
                delete iWrapperString;
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        TKeyResponse CAlfPerfAppTestCaseSelectionView::CContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
            {
            if(aType != EEventKey)
                return EKeyWasConsumed;

            if(aKeyEvent.iCode == EKeyUpArrow)
                {
                if (iCurrentLine > 0)
                    {
                    iCurrentLine--;
                    UpdateVisualContentL(EFalse);
                    }

                return EKeyWasConsumed;
                }

            else if(aKeyEvent.iCode == EKeyDownArrow)
                {
                if (iCurrentLine < iLineCount - iNumberOfLinesFitsScreen)
                    {
                    iCurrentLine++;
                    UpdateVisualContentL(EFalse);
                    }

                return EKeyWasConsumed;
                }

            return EKeyWasConsumed;    
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::HandleResourceChange(TInt aType)
            {
            if ( aType == KEikDynamicLayoutVariantSwitch )
                {
                TRect mainPaneRect;
                AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
                SetRect(mainPaneRect);

                iCurrentLine = 0; // scroll to top

                // update font
                // Discard and destroy the font
                iCoeEnv->ScreenDevice()->ReleaseFont(iFont);

                // Create the font 
                TFontSpec myFontSpec(KAlfPerfAppFontName,KAlfPerfAppFontSizeInTwips); 
                CGraphicsDevice* screenDevice=iCoeEnv->ScreenDevice();
                screenDevice->GetNearestFontInTwips(iFont,myFontSpec);

                // delete the scroll and update screen
                if (iScrollBarFrame)
                    {
                    delete iScrollBarFrame;
                    iScrollBarFrame = NULL;
                    }
                TRAP_IGNORE(UpdateVisualContentL(EFalse));
                }
            else
                {
                CCoeControl::HandleResourceChange(aType);
                }
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
            {
            // only on page up/down,scroll up/down and drag events
            if ((aEventType == EEikScrollPageDown) || (aEventType == EEikScrollPageUp) || 
                    (aEventType == EEikScrollThumbDragVert) || (aEventType == EEikScrollUp) ||
                    (aEventType == EEikScrollDown))
                {
                // get the current position from the scroll bar
                iCurrentLine = aScrollBar->ThumbPosition();

                // refresh now
                UpdateVisualContentL(EFalse);
                }
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::HandleControlEventL(
                CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
            {
            // TODO: Add your control event handler code here
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::UpdateVisualContentL(TBool aScrollToBottom)
            {
            TSize rectSize;
            AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, rectSize);
            TRect rect(rectSize);

            // init scroll bar if not yet done
            if (!iScrollBarFrame)
                {
                iScrollBarFrame = new(ELeave) CEikScrollBarFrame(this, this, ETrue);

                CAknAppUi* appUi = iAvkonAppUi;

                if (AknLayoutUtils::DefaultScrollBarType(appUi) == CEikScrollBarFrame::EDoubleSpan)
                    {
                    iScrollBarFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse);            
                    iScrollBarFrame->SetTypeOfVScrollBar(CEikScrollBarFrame::EDoubleSpan);
                    }
                else
                    {
                    iScrollBarFrame->SetTypeOfVScrollBar(CEikScrollBarFrame::EArrowHead);
                    }  
                iScrollBarFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
                }

            // drawing positions, needed for scalable ui
            iX_factor = TReal(rect.Width()) / 176;
            iY_factor = TReal(rect.Height()) / 144;

            iWrappedArray->Reset();

            if (iWrapperString)
                {
                delete iWrapperString;
                iWrapperString = NULL;
                }

            // define drawing width, some extra space needed if using double span scrolling bar
            TUint drawingWidth(0);
            if (iScrollBarFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
                {
                if (AknLayoutUtils::LayoutMirrored())  // scroll bar on 'left'
                    {
                    iLeftDrawingPosition = KAlfPerfAppLeftMargin + 12;
                    drawingWidth = TInt( rect.Width() - iLeftDrawingPosition*iX_factor - (KAlfPerfAppRightMargin*iX_factor)); 
                    }
                else // scroll bar on 'right'
                    {
                    iLeftDrawingPosition = KAlfPerfAppLeftMargin;  
                    drawingWidth = TInt( rect.Width() - iLeftDrawingPosition*iX_factor - (KAlfPerfAppRightMargin*iX_factor + 7*iX_factor)); 
                    }        
                }
            else
                drawingWidth = TInt( rect.Width() - KAlfPerfAppLeftMargin*iX_factor - KAlfPerfAppRightMargin*iX_factor);

            // wrap the text
            iWrapperString = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
                    iText->Des(),
                    drawingWidth,
                    *iFont,
                    *iWrappedArray
            );

            iLineCount = iWrappedArray->Count();

            // count amount of lines fits to screen
            iNumberOfLinesFitsScreen = TInt(rect.Height() / (iFont->HeightInPixels()));

            // check if needed to scroll to the bottom
            if (aScrollToBottom && iCurrentLine < iLineCount - iNumberOfLinesFitsScreen)
                {
                iCurrentLine = iLineCount-iNumberOfLinesFitsScreen;
                }

            // update the scroll bars
            TEikScrollBarModel horizontalBar;
            TEikScrollBarModel verticalBar;
            verticalBar.iThumbPosition = iCurrentLine;
            verticalBar.iScrollSpan = iLineCount - iNumberOfLinesFitsScreen + 1;
            verticalBar.iThumbSpan = 1;

            TEikScrollBarFrameLayout layout;
            layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;

            if (iScrollBarFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan)
                {    
                // do not let scrollbar values overflow
                if (verticalBar.iThumbPosition + verticalBar.iThumbSpan > verticalBar.iScrollSpan)
                    verticalBar.iThumbPosition = verticalBar.iScrollSpan - verticalBar.iThumbSpan;

                TAknDoubleSpanScrollBarModel horizontalDSBar(horizontalBar);
                TAknDoubleSpanScrollBarModel verticalDSBar(verticalBar);

                iScrollBarFrame->TileL(&horizontalDSBar, &verticalDSBar, rect, rect, layout);        
                iScrollBarFrame->SetVFocusPosToThumbPos(verticalDSBar.FocusPosition());
                }  
            else
                {
                iScrollBarFrame->TileL( &horizontalBar, &verticalBar, rect, rect, layout );
                iScrollBarFrame->SetVFocusPosToThumbPos( verticalBar.iThumbPosition );
                }


            // update the screen
            DrawNow();
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        TBool CAlfPerfAppTestCaseSelectionView::CContainer::PrintText(const TDesC& aDes)
            {
            // This doesn't guarantee that iText length actually remains below KAlfPerfAppDisplayResultMaxChars
            // but it won't exceed it that much. Approximation can be used for the limiting value.
            if(iText->Length() < KAlfPerfAppDisplayResultMaxChars)
                {
                iText->Des().Append( aDes );
                return ETrue;
                }
            else
                {
                if(iNotPrintedResultDisplayBufferExceeded)
                    {
                    iText->Des().Append( KAlfPerfAppDisplayBufferFull);
                    TBuf<KAlfPerfAppMaxCharsInLine> line;
                    TBuf<KAlfPerfAppMaxCharsInLine> temp = iModel->TargetPath();
                    line.Format(KAlfPerfAppPrintOutputFormat, &temp);
                    iText->Des().Append(line); 
                    iNotPrintedResultDisplayBufferExceeded = EFalse;
                    }
                return EFalse;
                }
            }

        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::ClearOutputWindowL()
            {
            iText->Des().Zero();
            iNotPrintedResultDisplayBufferExceeded = ETrue;
            UpdateVisualContentL(ETrue);
            }


        // -----------------------------------------------------------------------------
        // -----------------------------------------------------------------------------
        //
        void CAlfPerfAppTestCaseSelectionView::CContainer::Draw( 
                const TRect& aRect ) const
                {
                CWindowGc& gc = SystemGc();
                gc.Clear(aRect);
                gc.SetPenColor(KRgbBlack);
                gc.UseFont(iFont);

                // draw the text
                for (TInt i=0; i<iNumberOfLinesFitsScreen; i++)
                    {
                    // index out of bounds check and then draw text
                    if (i+iCurrentLine < iWrappedArray->Count())
                        gc.DrawText(iWrappedArray->At(i+iCurrentLine), 
                                TPoint(TInt(iLeftDrawingPosition*iX_factor), TInt(iFont->HeightInPixels()*(i+1))));
                    }
                gc.DiscardFont();
                }
