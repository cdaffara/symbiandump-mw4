/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class CodLogger.   
*      
*
*/


// INCLUDE FILES

#include "CodLogger.h"
#include <flogger.h>

#ifdef __TEST_COD_LOG

// ================= CONSTANTS =======================

/// COD logging directory.
_LIT( KCodLogDir, "Cod" );
/// COD log file name.
_LIT( KCodLogFile, "CodEng.txt" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CodLogger::Write()
// ---------------------------------------------------------
//
void CodLogger::Write
( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, ... )
    {
    if( (aMask & COD_LOG_MASK) && (aLevel <= COD_LOG_LEVEL) )
        {
        VA_LIST list;
        VA_START( list, aFmt );
        RFileLogger::WriteFormat
            (
            KCodLogDir,
            KCodLogFile,
            EFileLoggingModeAppend,
            aFmt,
            list
            );
        VA_END( list );
        }
    }

// ---------------------------------------------------------
// CodLogger::Write()
// ---------------------------------------------------------
//
void CodLogger::Write
( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList )
    {
    if( (aMask & COD_LOG_MASK) && (aLevel <= COD_LOG_LEVEL) )
        {
        RFileLogger::WriteFormat
            (
            KCodLogDir,
            KCodLogFile,
            EFileLoggingModeAppend,
            aFmt,
            aList
            );
        }
    }

// ---------------------------------------------------------
// CodLogger::Write()
// ---------------------------------------------------------
//
void CodLogger::Write
( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, ... )
    {
    if( (aMask & COD_LOG_MASK) && (aLevel <= COD_LOG_LEVEL) )
        {
        VA_LIST list;
        VA_START( list, aFmt );
        RFileLogger::WriteFormat
            (
            KCodLogDir,
            KCodLogFile,
            EFileLoggingModeAppend,
            aFmt,
            list
            );
        VA_END( list );
        }
    }

// ---------------------------------------------------------
// CodLogger::Write()
// ---------------------------------------------------------
//
void CodLogger::Write
( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList )
    {
    if( (aMask & COD_LOG_MASK) && (aLevel <= COD_LOG_LEVEL) )
        {
        RFileLogger::WriteFormat
            (
            KCodLogDir,
            KCodLogFile,
            EFileLoggingModeAppend,
            aFmt,
            aList
            );
        }
    }

// ---------------------------------------------------------
// CodLogger::HexDump()
// ---------------------------------------------------------
//
void CodLogger::HexDump
        (
        TInt32 aMask,
        TInt aLevel, 
        const TText* aHeader,
        const TText* aMargin,
        const TUint8* aPtr,
        TInt aLen
        )
    {
    if( (aMask & COD_LOG_MASK) && (aLevel <= COD_LOG_LEVEL) )
        {
        RFileLogger::HexDump
            (
            KCodLogDir,
            KCodLogFile,
            EFileLoggingModeAppend,
            aHeader,
            aMargin,
            aPtr,
            aLen
            );
        }
    }

#endif /* def __TEST_COD_LOG */
