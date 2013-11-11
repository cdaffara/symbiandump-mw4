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
*      Implementation of class FavouritesLogger
*      
*
*/


// INCLUDE FILES

#include "FavouritesLogger.h"
#include <flogger.h>

#ifdef __TEST_FAVOURITES_LOG

// ================= CONSTANTS =======================

/// Logging directory.
_LIT( KFavouritesLogDir, "Faveng" );
/// Log file name.
_LIT( KFavouritesLogFile, "FavEng.txt" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// FavouritesLogger::Write()
// ---------------------------------------------------------
//
void FavouritesLogger::Write( TRefByValue<const TDesC16> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat
        (
        KFavouritesLogDir,
        KFavouritesLogFile,
        EFileLoggingModeAppend,
        aFmt,
        list
        );
    VA_END( list );
    }

// ---------------------------------------------------------
// FavouritesLogger::Write()
// ---------------------------------------------------------
//
void FavouritesLogger::Write
( TRefByValue<const TDesC16> aFmt, VA_LIST& aList )
    {
    RFileLogger::WriteFormat
        (
        KFavouritesLogDir,
        KFavouritesLogFile,
        EFileLoggingModeAppend,
        aFmt,
        aList
        );
    }

// ---------------------------------------------------------
// FavouritesLogger::Write()
// ---------------------------------------------------------
//
void FavouritesLogger::Write( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat
        (
        KFavouritesLogDir,
        KFavouritesLogFile,
        EFileLoggingModeAppend,
        aFmt,
        list
        );
    VA_END( list );
    }

// ---------------------------------------------------------
// FavouritesLogger::Write()
// ---------------------------------------------------------
//
void FavouritesLogger::Write( TRefByValue<const TDesC8> aFmt, VA_LIST& aList )
    {
    RFileLogger::WriteFormat
        (
        KFavouritesLogDir,
        KFavouritesLogFile,
        EFileLoggingModeAppend,
        aFmt,
        aList
        );
    }

// ---------------------------------------------------------
// FavouritesLogger::HexDump()
// ---------------------------------------------------------
//
void FavouritesLogger::HexDump
        (
        const TText* aHeader,
        const TText* aMargin,
        const TUint8* aPtr,
        TInt aLen
        )
    {
    RFileLogger::HexDump
        (
        KFavouritesLogDir,
        KFavouritesLogFile,
        EFileLoggingModeAppend,
        aHeader,
        aMargin,
        aPtr,
        aLen
        );
    }

#endif /* def __TEST_FAVOURITES_LOG */

//  End of File  
