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
*      Declaration of class FavouritesLogger
*      
*
*/


#ifndef FAVOURITES_LOGGER_H
#define FAVOURITES_LOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

#ifdef _DEBUG

    /// Defining this enables logging. Needs flogger.lib.
    #define __TEST_FAVOURITES_LOG

#endif /* def _DEBUG */

#ifdef __TEST_FAVOURITES_LOG

    // CLASS DECLARATION

    /**
    * Logger class.
    */
    NONSHARABLE_CLASS(FavouritesLogger)
        {
        public:     // new methods

        /**
        * Write formatted log.
        * @param aFmt Format string.
        */
        static void Write( TRefByValue<const TDesC16> aFmt, ... );

        /**
        * Write formatted log.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write( TRefByValue<const TDesC16> aFmt, VA_LIST& aList );

        /**
        * Write formatted log.
        * @param aFmt Format string.
        */
        static void Write( TRefByValue<const TDesC8> aFmt, ... );

        /**
        * Write formatted log.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write( TRefByValue<const TDesC8> aFmt, VA_LIST& aList );

        /**
        * Write hex dump.
        * @param aHeader Header string.
        * @param aMargin Margin.
        * @param aPtr Data.
        * @param aLen Data length.
        */
        static void HexDump
            (
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            );

        };

#endif /* def __TEST_FAVOURITES_LOG */

#ifdef __TEST_FAVOURITES_LOG

    /// Write formatted to log.
    #define FLOG( body ) FavouritesLogger::Write body
    /// Write hex dump.
    #define FDUMP( body ) FavouritesLogger::HexDump body
    /// Guard "log-only" argument name with this (expands to argument).
    #define LOG_ONLY( argName ) argName

#else /* not defined __TEST_FAVOURITES_LOG */

    /// Do nothing (log disabled).
    #define FLOG( body )
    /// Do nothing (log disabled).
    #define FDUMP( body )
    /// Guard "log-only" argument name with this (expands to nothing).
    #define LOG_ONLY( argName )

#endif /* def __TEST_FAVOURITES_LOG */

#ifdef _DEBUG

    /// Guard "debug-only" argument name with this (expands to argument).
    #define DEBUG_ONLY( argName ) argName

#else /* not defined _DEBUG */

    /// Guard "debug-only" argument name with this (expands to nothing).
    #define DEBUG_ONLY( argName )

#endif /* def _DEBUG */

#endif /* def FAVOURITES_LOGGER_H */

// End of File
