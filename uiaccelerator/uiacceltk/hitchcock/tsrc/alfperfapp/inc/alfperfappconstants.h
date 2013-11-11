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
* Description:  alfperfapp application constants.
*
*/


#ifndef ALFPERFAPPCOSNTANTS_H
#define ALFPERFAPPCOSNTANTS_H

#include <e32std.h>

/**
 * View ID of test case selection view.
 */
const TUid KAlfPerfAppTestCaseSelectionViewId = { 0x1 };

/**
 * View ID of test case execution view.
 */
const TUid KAlfPerfAppTestCaseExecutionViewId = { 0x2 };

/**
 * View ID of test case results view.
 */
const TUid KAlfPerfAppTestCaseResultsViewId = { 0x1 };






// Some constants for handling printing info
_LIT(KAlfPerfAppFontName,"Courier");
const TUint KAlfPerfAppLeftMargin = 2;
const TUint KAlfPerfAppRightMargin = 2;
const TInt KAlfPerfAppFontSizeInTwips = 125;
const TInt KAlfPerfAppMaxCharsInLine = 256;
const TInt KAlfPerfScreenTextBufferSize = 200000;
const TInt KAlfPerfScreenWrappedArraySize = 500;


// Default paths for output file
_LIT(KAlfPerfAppOutputFilePath1, "c:\\data\\others");
_LIT(KAlfPerfAppOutputFilePath2, "e:\\others");
//_LIT(KAlfPerfAppOutputFilePath2, "c:\\data\\others");  // DEBUG
_LIT( KAlfPerfAppPrintPathSeparator, "\\" );
_LIT(KAlfPerfAppOutputFileName, "AlfPerfAppResults.csv");
_LIT(KAlfPerfAppSummaryFileName, "AlfPerfAppSummary.csv");

_LIT8( KAlfPerfAppPrintFormatHeader,
    "suite/case,caseid,phase,time,app cells,app ram usage,app ram free,server cells,server ram usage,server ram free,system ram usage,system ram free,frames since phase,ms since case,ms since phase,fps,casespecificresult\n");
_LIT8( KAlfPerfAppPrintFormatData,
      "%S / %S,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%S\x0D\x0A");

_LIT8( KAlfPerfAppSummaryPrintFormatHeader,
    "suite/case,caseid,phase,app ram delta,server ram delta,system ram delta,frames since phase,ms since phase,fps,casespecificresult\n");
_LIT8( KAlfPerfAppSummaryPrintFormatData,
      "%S / %S,%d,%d,%d,%d,%d,%d,%d,%f,%S\x0D\x0A");

/*
 * Descriptor to store test-case specific result data.
 */
const TInt KAlfPerfAppMaxCharsInSpecificResultText = 256;
typedef HBufC* TTestCaseSpecificResultText;





#endif // ALFPERFAPPCOSNTANTS_H
            
// End of File
