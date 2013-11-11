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
* Description:  alfperfapp model class definition.
*
*/


#ifndef C_ALFPERFAPPMODEL_H
#define C_ALFPERFAPPMODEL_H

#include <e32base.h>
#include <f32file.h>

#include "alfperfappconstants.h"
class CDesC16ArrayFlat;
class CAlfPerfAppModelTestSuite;


/**
 *  CAlfPerfAppModel model class.
 *  This class records test cases to be executed as well as 
 *  results of those cases.
 */
class CAlfPerfAppModel : public CBase
    {
public:

    struct TTestCaseResultItem
        {
        // Header
        TBuf8<KAlfPerfAppMaxCharsInLine> iSuiteName;
        TBuf8<KAlfPerfAppMaxCharsInLine> iCaseName;
        TInt iCaseId;				// Case id
        TInt iPhase; 				// Phase: 0=startup, 1=run, 2=teardown
        
        // Data
        TUint32 iTimeStamp;         // Timestamp
        TUint32 imsSinceCase;       // ms since case start
        TUint32 imsSincePhase;      // ms since phase start
        
        // Application memory
        TInt iAppCells;
        TInt iAppMemory;
        TInt iAppFree;
        
        // Server memory
        TInt iServerCells;
        TInt iServerMemory;
        TInt iServerFree;

        // Frame count from server
        TUint iFrameCount; // Frames from beginning of phase
        TReal32 iFPS;        // During this phase
        
        // System total memory consumption
        TInt iSystemMemory;
        TInt iSystemFree;
        
        // Testcase specifig result
        TBuf8<KAlfPerfAppMaxCharsInSpecificResultText> specificResult8;
		};

   
public:
    static CAlfPerfAppModel* NewL();
    ~CAlfPerfAppModel();
    
    //
    // Test set management functions
    //
    
    /**
     * Loads test set.
     * @param aResourceId resource id for test set.
     */
    void LoadTestSetL( TInt aResourceId );
    
    /**
     * Returns amount of test suites.
     * @return amount of test suites.
     */
    TInt SuiteCount() const;
    
    /**
     * Returns name of specified suite.
     * @param aSuiteIndex index to suite.
     * @return name of suite.     
     */
    TPtrC SuiteName( TInt aSuiteIndex ) const;

    /**
     * Returns flags of specified suite.
     * @param aSuiteIndex index to suite.
     * @return flags of suite.     
     */
    TInt SuiteFlags( TInt aSuiteIndex ) const;

    /**
     * Returns amount of test cases in specified suite.
     * @param aSuiteIndex index to suite.
     * @return amount of test cases in the suite.
     */
    TInt TestCaseCount( TInt aSuiteIndex ) const;

    /**
     * Gets information about specified suite.
     * @param aSuiteIndex index to suite.
     * @param aTestCaseIndex index inside suite.
     * @param aTestCaseName this is updated to point to name.
     * @param aTestCaseId this is updated to hold case id.
     */
    void GetTestCase( 
        TInt aSuiteIndex, 
        TInt aTestCaseIndex, 
        TPtrC& aTestCaseName,
        TInt& aTestCaseId,
        TInt& aTestCaseFlags,
        TInt& aTestCaseSequenceLength ) const;
    
    /**
     * Finds test suite name & test case name by case id.
     * @param aCaseId test case id.
     * @param aSuiteName this is updated to point to suite name.
     * @param aTestCaseName this is updated to point to test case name.
     * @return KErrNone in success, KErrNotFound if not found.
     */
    TInt FindById( 
        TInt aCaseId,
        TPtrC& aSuiteName,
        TPtrC& aTestCaseName ) const;
        
   	//
    // Execute array management functions
    //    
    /**
     * Resets execute test cases array.
     */
    void ResetExecuteArray();
    
    /**
     * Appends a case to execute array.
     * @param aCaseId case to be added.
     */
    void AddToExecuteArrayL( TInt aCaseId );
        
    /**
     * Gets first test case to be executed.
     * This also removes that item from array.
     */
    TBool GetFromExecuteArray( TInt& aCaseId );

    //
    // Test results management functions
    //
    /**
     * Appends a result item to result files.
     * @param aItem result item to be added.
     */
	void AddToResultFilesL( const TTestCaseResultItem& aItem);

	   
	/**
	 * Adds error.
	 * @param aMessage message to be added.
	 */
	void AddToErrorArrayL( const TDesC& aMessage );
	
	/**
	 * Returns total amount of error messages.
	 * @return total amounf of error messages.
	 */
	TInt ErrorArrayCount();
	
	/**
	 * Gets error message.
	 * @param aIndex index from which to take.
	 * @param aMessage message.
	 */
	void GetFromErrorArray( TInt aIndex, TPtrC& aMessage );
    
    /**
     * Resets error array.
     */
    void ResetErrorArray();
    
    /*
     *  Tells if there are any results available in file.
     */
    TBool ResultsNotAddedToFile();
    
    /*
     *  Returns pointer to the file which results are written to
     */
    RFile* ResultFile();
    

    /*
     * Returns the target path location.
     */
    TFileName TargetPath();    
    
    /*
     * Returns if the files existed when the program was launched.
     */
    TBool TargetFilesExisted();
    
    /*
     * Opens the result files for appending. Creates new ones if necessary.
     * If reset is set to ETrue, forces recreation.
     */
    void OpenFilesL(TBool reset = EFalse);
    
private:

    CAlfPerfAppModel();
    void ConstructL();

private:

    /**
     * Test suites.
     * Owned.
     */
    RPointerArray< CAlfPerfAppModelTestSuite > iSuites;
    
    /**
     * Array of cases to be executed.
     * Owned.
     */
    RArray< TInt > iExecuteTestCases;

    /**
     * Array of error messages.
     * Owned.
     */
    CDesC16ArrayFlat* iErrorMessages;

    TBool   iFilesOpened;
    TBool   iFilesExisted;
    TFileName iTargetPath;
    
    RFs iTargetFileServerSession;
    TFileName iTargetFileName;
    RFile iTargetFile;
    
    TBool iResultItemsNotAddedToFile;
    
    TFileName iSummaryFileName;
    RFile iSummaryFile;
    TFileText iSummaryWriteFileText;
    
    };

#endif // C_ALFPERFAPPMODEL_H
            
// End of File
