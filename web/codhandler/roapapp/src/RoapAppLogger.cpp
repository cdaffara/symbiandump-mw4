/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class RoapAppLogger.   
*      
*
*/


// INCLUDE FILES

#include "RoapAppLogger.h"
#include <flogger.h>

#ifdef __TEST_ROAP_APP_LOG

// ================= CONSTANTS =======================

/// COD logging directory.
_LIT( KRoapAppLogDir, "ROAP" );
/// COD log file name.
_LIT( KRoapAppLogFile, "RoapApp.txt" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RoapAppLogger::Write()
// ---------------------------------------------------------
//
void RoapAppLogger::Write
( TInt aLevel, TRefByValue<const TDesC16> aFmt, ... )
    {
    if( aLevel <= ROAP_APP_LOG_LEVEL )
        {
        VA_LIST list;
        VA_START( list, aFmt );
        RFileLogger::WriteFormat
            (
            KRoapAppLogDir,
            KRoapAppLogFile,
            EFileLoggingModeAppend,
            aFmt,
            list
            );
        VA_END( list );
        }
    }

// ---------------------------------------------------------
// RoapAppLogger::Write()
// ---------------------------------------------------------
//
void RoapAppLogger::Write
( TInt aLevel, TRefByValue<const TDesC16> aFmt, VA_LIST& aList )
    {
    if( aLevel <= ROAP_APP_LOG_LEVEL )
        {
        RFileLogger::WriteFormat
            (
            KRoapAppLogDir,
            KRoapAppLogFile,
            EFileLoggingModeAppend,
            aFmt,
            aList
            );
        }
    }

// ---------------------------------------------------------
// RoapAppLogger::Write()
// ---------------------------------------------------------
//
void RoapAppLogger::Write
( TInt aLevel, TRefByValue<const TDesC8> aFmt, ... )
    {
    if( aLevel <= ROAP_APP_LOG_LEVEL )
        {
        VA_LIST list;
        VA_START( list, aFmt );
        RFileLogger::WriteFormat
            (
            KRoapAppLogDir,
            KRoapAppLogFile,
            EFileLoggingModeAppend,
            aFmt,
            list
            );
        VA_END( list );
        }
    }

// ---------------------------------------------------------
// RoapAppLogger::Write()
// ---------------------------------------------------------
//
void RoapAppLogger::Write
( TInt aLevel, TRefByValue<const TDesC8> aFmt, VA_LIST& aList )
    {
    if( aLevel <= ROAP_APP_LOG_LEVEL )
        {
        RFileLogger::WriteFormat
            (
            KRoapAppLogDir,
            KRoapAppLogFile,
            EFileLoggingModeAppend,
            aFmt,
            aList
            );
        }
    }

// ---------------------------------------------------------
// RoapAppLogger::HexDump()
// ---------------------------------------------------------
//
void RoapAppLogger::HexDump
        (
        TInt aLevel, 
        const TText* aHeader,
        const TText* aMargin,
        const TUint8* aPtr,
        TInt aLen
        )
    {
    if( aLevel <= ROAP_APP_LOG_LEVEL )
        {
        RFileLogger::HexDump
            (
            KRoapAppLogDir,
            KRoapAppLogFile,
            EFileLoggingModeAppend,
            aHeader,
            aMargin,
            aPtr,
            aLen
            );
        }
    }

#endif /* def __TEST_ROAP_APP_LOG */
