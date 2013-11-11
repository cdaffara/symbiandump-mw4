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
* Description:  Model definition
*
*/


#include "alfperfappmodel.h"
#include <coemain.h>
#include <barsread.h>
#include <badesca.h>
#include <bautils.h>
#include <eikenv.h>

#include "alfperfapp.hrh"
#include "alfperfapptestcaseselectionview.h"

/**
 * Granularity of execution array.
 */
const TInt KAlfPerfAppModelExecuteArrayGranularity = 4;

/**
 * Maximum amount of test suites.
 */
const TInt KAlfPerfAppMaxTestSuites = 255;

/**
 * Maximum amount of test cases.
 */
const TInt KAlfPerfAppMaxTestCases = 255;

/**
 * Space reserved for this amount of items in execute array.
 */
const TInt KAlfPerfAppReserveExecution = 100;

/**
 * Class to hold test case information.
 */
class CAlfPerfAppModelTestCase : public CBase
    {
public:
    CAlfPerfAppModelTestCase( HBufC* aName, TInt aId, TInt aFlags, TInt aSequenceLength);
    ~CAlfPerfAppModelTestCase();
    void Get( TPtrC& aName, TInt& aId, TInt& aTestCaseFlags, TInt& aTestCaseSequenceLength ) const;
    TInt Flags();
    TInt SequenceIndex();
    
private:
    HBufC* iName;
    // Case ID is negative if this is a sequence case. This need to be taken
    // into account when doing comparisons.
    TInt iId;    
    TInt iFlags;
    TInt iSequenceLength;
    };

/**
 * Class to hold test suite information.
 */
class CAlfPerfAppModelTestSuite : public CBase
    {
public:
    CAlfPerfAppModelTestSuite( HBufC* aSuite, TInt aFlags );
    ~CAlfPerfAppModelTestSuite();
    void AppendL( CAlfPerfAppModelTestCase* aCase );

    TPtrC Name() const;
    TInt Flags() const;

    TInt TestCaseCount() const;
    void GetTestCase( TInt aIndex, TPtrC& aName, TInt& aId,
         TInt& aTestCaseFlags, TInt& aTestCaseSequenceLength) const;
    TInt FindNameById( TPtrC& aName, TInt aId ) const;

private:
    HBufC* iName;
    TInt iFlags;

    RPointerArray< CAlfPerfAppModelTestCase > iCases;    
    };

// ============================ MEMBER FUNCTIONS ===============================

CAlfPerfAppModel* CAlfPerfAppModel::NewL()
    {
    CAlfPerfAppModel* self = new (ELeave) CAlfPerfAppModel;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); // self;
    return self;
    }

CAlfPerfAppModel::~CAlfPerfAppModel()
    {
    iExecuteTestCases.Close();
    iSuites.ResetAndDestroy();
    delete iErrorMessages;

    // Close the target file
    iTargetFile.Close();
    iSummaryFile.Close();
    }

// -----------------------------------------------------------------------------
// Loads array of test sets. Each test set contains several test cases.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppModel::LoadTestSetL( TInt aResourceId )
    {
    // Delete previous suites.
    iSuites.ResetAndDestroy();
    iSuites.Close();

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    const TInt suiteCount = reader.ReadInt16();
    if ( suiteCount > KAlfPerfAppMaxTestSuites )
        {
        User::Leave( KErrArgument );
        }

    for ( TInt ii = 0; ii < suiteCount; ii++ )
        {
        HBufC* suiteName = reader.ReadHBufCL();
        CleanupStack::PushL( suiteName );

        const TInt flags = reader.ReadInt16();

        CAlfPerfAppModelTestSuite* suite = new (ELeave)
        CAlfPerfAppModelTestSuite( suiteName, flags );
        CleanupStack::Pop( suiteName );
        CleanupStack::PushL( suite );            

        const TInt testCaseCount = reader.ReadInt16();
        if ( testCaseCount > KAlfPerfAppMaxTestCases )
            {
            User::Leave( KErrArgument );
            }

        for ( TInt jj = 0; jj < testCaseCount; jj++ )
            {
            TInt caseId = reader.ReadInt16();
            HBufC* name = reader.ReadHBufCL();
            CleanupStack::PushL( name );
            const TInt flags = reader.ReadInt16();
            const TInt sequenceLength = reader.ReadInt16();
            
            // Mark sequence cases
            if(sequenceLength > 0)
                {
                caseId *= -1;
                }
            
            CAlfPerfAppModelTestCase* testCase = 
            new (ELeave) CAlfPerfAppModelTestCase( name, caseId, flags, sequenceLength );
            CleanupStack::Pop( name );
            #ifdef ALFPERFAPP_ENABLE_INACTIVE_FLAG_CASES
            // If ALFPERFAPP_ENABLE_INACTIVE_FLAG_CASES is defined, just ignore flags
            CleanupStack::PushL( testCase );
            suite->AppendL( testCase );
            CleanupStack::Pop( testCase );
            #else
            // Otherwise check that one of the flags is not EAlfPerfAppTestCaseFlagRequiresInActiveFlag
            if(!(testCase->Flags() & EAlfPerfAppTestCaseFlagRequiresInActiveFlag))
                {
                CleanupStack::PushL( testCase );
                suite->AppendL( testCase );
                CleanupStack::Pop( testCase );
                }
            else
                {
                delete testCase; 
                }
            #endif // ALFPERFAPP_ENABLE_INACTIVE_FLAG_CASES

            }

        iSuites.AppendL( suite );
        CleanupStack::Pop( suite );
        }

    CleanupStack::PopAndDestroy();    
    }

TInt CAlfPerfAppModel::SuiteCount() const
    {
    return iSuites.Count();
    }
        
TPtrC CAlfPerfAppModel::SuiteName( TInt aSuiteIndex ) const
    {
    return iSuites[ aSuiteIndex ]->Name();
    }

TInt CAlfPerfAppModel::SuiteFlags( TInt aSuiteIndex ) const
    {
    return iSuites[ aSuiteIndex ]->Flags();
    }
        
TInt CAlfPerfAppModel::TestCaseCount( TInt aSuiteIndex ) const
    {
    return iSuites[ aSuiteIndex ]->TestCaseCount();
    }
        
void CAlfPerfAppModel::GetTestCase( 
        TInt aSuiteIndex, 
        TInt aTestCaseIndex, 
        TPtrC& aTestCaseName,
        TInt& aTestCaseId,
        TInt& aTestCaseFlags,
        TInt& aTestCaseSequenceLength) const
    {
    iSuites[ aSuiteIndex ]->GetTestCase( 
        aTestCaseIndex, aTestCaseName, aTestCaseId, aTestCaseFlags, aTestCaseSequenceLength );
    }
    
TInt CAlfPerfAppModel::FindById( 
        TInt aCaseId,
        TPtrC& aSuiteName,
        TPtrC& aTestCaseName ) const
    {
    const TInt count = iSuites.Count();
    TInt result = KErrNotFound;
    
    for ( TInt ii = 0; ii < count; ii++ )
        {
        if ( iSuites[ ii ]->FindNameById( aTestCaseName, aCaseId ) != 
             KErrNotFound )
            {
            aSuiteName.Set( iSuites[ ii ]->Name() );
            result = KErrNone;
            break;
            }
        }
    
    return result;
    }
    
void CAlfPerfAppModel::ResetExecuteArray()
    {
    // Clear array without releasing memory
    while ( iExecuteTestCases.Count() )
        {
        iExecuteTestCases.Remove( iExecuteTestCases.Count() - 1 );
        }
    }

void CAlfPerfAppModel::AddToExecuteArrayL( TInt aCaseId )
    {
    iExecuteTestCases.AppendL( aCaseId );
    }

TBool CAlfPerfAppModel::GetFromExecuteArray( TInt& aCaseId )
    {
    TBool result = EFalse;
    if ( iExecuteTestCases.Count() )
        {
        result = ETrue;
        aCaseId = iExecuteTestCases[ 0 ];
        iExecuteTestCases.Remove( 0 );
        }

    return result;
    }

void CAlfPerfAppModel::AddToResultFilesL( const TTestCaseResultItem& aItem)
    {
    iResultItemsNotAddedToFile = EFalse;

    static TUint32 timeStampAtCaseStart = 0;
    static TUint32 timeStampAtPhaseStart = 0;
    static TUint frameCountAtPhaseStart = 0;
    static TInt currentCase  = 0;
    static TInt currentPhase = 0;
    static TBool firstCase = ETrue;
    TBool caseChanged = EFalse;

    if ( firstCase || currentCase != aItem.iCaseId )
        {
        // Test case changed.
        currentCase = aItem.iCaseId;
        timeStampAtCaseStart = aItem.iTimeStamp;
        caseChanged = ETrue;
        firstCase = EFalse;
        }

    if ( caseChanged || currentPhase != aItem.iPhase )
        {
        // Test case phase changed.
        currentPhase = aItem.iPhase;
        timeStampAtPhaseStart = aItem.iTimeStamp;
        frameCountAtPhaseStart = aItem.iFrameCount;
        }


    // Get Suite and Case name
    TPtrC testSuite;
    TPtrC testCase;               
    FindById( aItem.iCaseId, testSuite, testCase );
    TBuf<KAlfPerfAppMaxCharsInLine> testSuiteName(testSuite);
    TBuf8<KAlfPerfAppMaxCharsInLine> testSuiteName8;
    testSuiteName8.Copy(testSuiteName);
    TBuf<KAlfPerfAppMaxCharsInLine> testCaseName(testCase);
    TBuf8<KAlfPerfAppMaxCharsInLine> testCaseName8;
    testCaseName8.Copy(testCaseName);


    TUint32 msSinceCase = 
    CAlfPerfAppTestCaseSelectionView::DeltaFromCurrentToPrevious( aItem.iTimeStamp, timeStampAtCaseStart );
    TUint32 msSincePhase = 
    CAlfPerfAppTestCaseSelectionView::DeltaFromCurrentToPrevious( aItem.iTimeStamp, timeStampAtPhaseStart );
    TUint framesSincePhase = 
    aItem.iFrameCount - frameCountAtPhaseStart;

    TReal32 fps = 0.0;
    if ( msSincePhase != 0 )
        {
        fps = framesSincePhase;
        fps *= 1000;
        fps /= msSincePhase;
        }

    // Write to main file 
    TBuf8<KAlfPerfAppMaxCharsInLine * 2> line; // * 2 because casespecific results can be 256 chars
    line.Format( KAlfPerfAppPrintFormatData,
            &testSuiteName8, &testCaseName8,aItem.iCaseId, aItem.iPhase,
            aItem.iTimeStamp,
            aItem.iAppCells, aItem.iAppMemory, aItem.iAppFree, 
            aItem.iServerCells, aItem.iServerMemory, aItem.iServerFree,
            aItem.iSystemMemory, aItem.iSystemFree,
            framesSincePhase, msSinceCase, msSincePhase,
            fps, &aItem.specificResult8);
    iTargetFile.Write(line);

    // Write to summary file
    static TInt lastPhase = -1; // -1 to mark first time
    static TInt appMemoryPhaseStart = 0; //   Value doensn't matter because next
    static TInt serverMemoryPhaseStart = 0;// if fails at the first time
    static TInt systemMemoryPhaseStart = 0;


    // Only print this if this was the second time this phase is here (end of phase results)
    if(lastPhase == aItem.iPhase)
        {
        line.Format(KAlfPerfAppSummaryPrintFormatData,
                &testSuiteName8, &testCaseName8,aItem.iCaseId, aItem.iPhase,
                aItem.iAppMemory - appMemoryPhaseStart,
                aItem.iServerMemory - serverMemoryPhaseStart,
                aItem.iSystemMemory - systemMemoryPhaseStart,
                framesSincePhase,msSincePhase,fps,&aItem.specificResult8);
        iSummaryFile.Write(line);
        }
    else
        {
        // If this was the beginning of phase, record memory values
        appMemoryPhaseStart = aItem.iAppMemory;
        serverMemoryPhaseStart = aItem.iServerMemory;
        systemMemoryPhaseStart = aItem.iSystemMemory;
        }
    // Set the lastPhase
    lastPhase = aItem.iPhase;
    }

void CAlfPerfAppModel::AddToErrorArrayL( const TDesC& aMessage )
    {
    if ( !iErrorMessages )
        {
        iErrorMessages = new (ELeave) CDesC16ArrayFlat( 8 );
        }

    iErrorMessages->AppendL( aMessage );
    }

TInt CAlfPerfAppModel::ErrorArrayCount()
    {
    TInt count = 0;
    if ( iErrorMessages )
        {
        count = iErrorMessages->MdcaCount();
        }
    return count;
    }

void CAlfPerfAppModel::GetFromErrorArray( TInt aIndex, TPtrC& aMessage )
    {
    aMessage.Set( KNullDesC );
    if ( iErrorMessages )
        {
        if ( aIndex >= 0 && aIndex < iErrorMessages->MdcaCount() )
            {
            aMessage.Set( iErrorMessages->MdcaPoint( aIndex ) );
            }
        }
    }

void CAlfPerfAppModel::ResetErrorArray()
    {
    delete iErrorMessages;
    iErrorMessages = NULL;
    }

TBool CAlfPerfAppModel::ResultsNotAddedToFile()
    {
    return iResultItemsNotAddedToFile;
    }

RFile* CAlfPerfAppModel::ResultFile()
    {
    return &iTargetFile;
    }

TFileName CAlfPerfAppModel::TargetPath()
    {
    return iTargetPath;
    }

TBool CAlfPerfAppModel::TargetFilesExisted()
    {
    return iFilesExisted;
    }

void CAlfPerfAppModel::OpenFilesL(TBool reset)
    {
    TBuf8<KAlfPerfAppMaxCharsInLine> line;
    
    // If the files are open, close them
    if(iFilesOpened)
        {
        iTargetFile.Close();
        iSummaryFile.Close();
        iResultItemsNotAddedToFile = ETrue;
        }
    
    iTargetFileServerSession = CEikonEnv::Static()->FsSession();

    // Ready the target and summary file names
    iTargetPath = KAlfPerfAppOutputFilePath2;
    iTargetFileName = KAlfPerfAppOutputFilePath2;
    iSummaryFileName = KAlfPerfAppOutputFilePath2;
    if (!BaflUtils::FolderExists(iTargetFileServerSession, iTargetFileName))
        {
        iTargetPath = KAlfPerfAppOutputFilePath1;
        iTargetFileName = KAlfPerfAppOutputFilePath1;
        iSummaryFileName = KAlfPerfAppOutputFilePath1;
        }

    iTargetFileName.Append( KAlfPerfAppPrintPathSeparator );
    iTargetFileName.Append( KAlfPerfAppOutputFileName );
    iSummaryFileName.Append( KAlfPerfAppPrintPathSeparator );
    iSummaryFileName.Append(KAlfPerfAppSummaryFileName);

    // Check if the files are already in place
    TBool targetFileWasThere = BaflUtils::FileExists(iTargetFileServerSession,iTargetFileName);
    TBool summaryFileWasThere = BaflUtils::FileExists(iTargetFileServerSession,iSummaryFileName);
        
    iFilesExisted = targetFileWasThere || summaryFileWasThere;
    
    // Create the directory and files
    iTargetFileServerSession.MkDirAll(iTargetFileName);

    // If caller forced clearing of files
    if(reset)
        {
        // Files were closed in the beginning of this function
        BaflUtils::DeleteFile(iTargetFileServerSession,iTargetFileName);
        BaflUtils::DeleteFile(iTargetFileServerSession,iSummaryFileName);
        targetFileWasThere = EFalse;
        summaryFileWasThere = EFalse;
        }
    
    
    
    // Create or reopen the target file
    TInt targetLoc = 0;
    if(targetFileWasThere)
        {
        User::LeaveIfError(iTargetFile.Open(iTargetFileServerSession,
                iTargetFileName, EFileWrite|EFileShareExclusive));
        iTargetFile.Seek(ESeekEnd,targetLoc);
        }
    else
        {
        User::LeaveIfError(iTargetFile.Create(iTargetFileServerSession,
                iTargetFileName, EFileWrite|EFileShareExclusive));
        line.Format( KAlfPerfAppPrintFormatHeader );
        iTargetFile.Write(line);
        }
    
    // Create or reopen the summary file
    if(summaryFileWasThere)
        {
        User::LeaveIfError(iSummaryFile.Open(iTargetFileServerSession,
                iSummaryFileName, EFileWrite|EFileShareExclusive));
        iSummaryFile.Seek(ESeekEnd,targetLoc);
        }
    else
        {
        User::LeaveIfError(iSummaryFile.Create(iTargetFileServerSession,
                iSummaryFileName, EFileWrite|EFileShareExclusive));
        line.Format(KAlfPerfAppSummaryPrintFormatHeader);
        iSummaryFile.Write(line);
        }

    iResultItemsNotAddedToFile = ETrue;
    }


CAlfPerfAppModel::CAlfPerfAppModel()
: iExecuteTestCases( KAlfPerfAppModelExecuteArrayGranularity )
        {
        }

void CAlfPerfAppModel::ConstructL()
    {

    iExecuteTestCases.ReserveL( KAlfPerfAppReserveExecution );
   
    // Let's open the target  files
    iFilesOpened = EFalse;
    OpenFilesL(); // Don't force clearing
    iFilesOpened = ETrue;
    }

//
// Implementation of CAlfPerfAppTestCase:
//

CAlfPerfAppModelTestCase::CAlfPerfAppModelTestCase( HBufC* aName, TInt aId, TInt aFlags,  TInt aSequenceLength  )
    : iName( aName ), iId( aId ), iFlags(aFlags), iSequenceLength(aSequenceLength)
    {
    }
    
CAlfPerfAppModelTestCase::~CAlfPerfAppModelTestCase()
    {
    delete iName;
    }

void CAlfPerfAppModelTestCase::Get( TPtrC& aName, TInt& aId,TInt& aTestCaseFlags, TInt& aTestCaseSequenceLength ) const
    {
    aName.Set( KNullDesC );
    if ( iName )
        {
        aName.Set( *iName );
        }
    aId = iId;
    aTestCaseFlags = iFlags;
    aTestCaseSequenceLength = iSequenceLength;
    }
    
TInt CAlfPerfAppModelTestCase::Flags()
    {
    return iFlags;
    }

TInt CAlfPerfAppModelTestCase::SequenceIndex()
    {
    return iSequenceLength;
    }

//
// Implementation of CAlfPerfAppTestSuite:
//

CAlfPerfAppModelTestSuite::CAlfPerfAppModelTestSuite( HBufC* aSuite, TInt aFlags )
    : iName( aSuite ), iFlags( aFlags ), iCases( 4 )
    {
    }

CAlfPerfAppModelTestSuite::~CAlfPerfAppModelTestSuite()
    {
    delete iName;
    iCases.ResetAndDestroy();
    }

void CAlfPerfAppModelTestSuite::AppendL( CAlfPerfAppModelTestCase* aCase )
    {
    iCases.AppendL( aCase );
    }

TPtrC CAlfPerfAppModelTestSuite::Name() const
    {
    TPtrC name( KNullDesC );
    if ( iName )
        {
        name.Set( *iName );
        }
    return name;
    }
        
TInt CAlfPerfAppModelTestSuite::Flags() const
    {
    return iFlags;
    }

TInt CAlfPerfAppModelTestSuite::TestCaseCount() const
    {
    return iCases.Count();
    }

void CAlfPerfAppModelTestSuite::GetTestCase( 
        TInt aIndex, TPtrC& aName, TInt& aId, 
        TInt& aTestCaseFlags, TInt& aTestCaseSequenceLength ) const
    {
    return iCases[ aIndex ]->Get( aName, aId, aTestCaseFlags, aTestCaseSequenceLength );    
    }


TInt CAlfPerfAppModelTestSuite::FindNameById( TPtrC& aName, TInt aId ) const
    {
    const TInt count = iCases.Count();
    TInt result = KErrNotFound;
    
    for ( TInt ii = 0; ii < count; ii++ )
        {
        TPtrC name;
        TInt id;
        TInt flags;
        TInt sequenceLength;
        
        iCases[ ii ]->Get( name, id, flags, sequenceLength );
        
        if ( aId == id )
            {
            result = KErrNone;
            aName.Set( name );
            break;
            }
        }
    
    return result;
    }
