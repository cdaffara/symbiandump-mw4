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
*      Logging macros.   
*      
*
*/


#ifndef ROAP_APP_LOGGER_H
#define ROAP_APP_LOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

/**
* Determines log detail.
* - 0 -> Basic level. Only key events / data. Public API method calls.
*        Failures (panics).
* - 1 -> Basic level. More detailed about key events / data.
* - 2 -> Medium level. Function calls are logged.
* - 3 -> Medium level. More detailed. Observer callbacks.
* - 4 -> Detailed level.
* - 5 -> Detailed level. Most detailed, log grows very big.
*/
#define ROAP_APP_LOG_LEVEL 4

#ifdef _DEBUG

    /// Defining this enables logging. Needs flogger.lib.
    #define __TEST_ROAP_APP_LOG

#endif /* def _DEBUG */

#ifdef __TEST_ROAP_APP_LOG

    // CLASS DECLARATION

    /**
    * Logger class.
    */
    class RoapAppLogger
        {
        public:     // new methods

        /**
        * Write formatted log.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt aLevel, TRefByValue<const TDesC16> aFmt, ... );

        /**
        * Write formatted log.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt aLevel,
            TRefByValue<const TDesC16> aFmt,
            VA_LIST& aList
            );

        /**
        * Write formatted log.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt aLevel, TRefByValue<const TDesC8> aFmt, ... );

        /**
        * Write formatted log.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt aLevel,
            TRefByValue<const TDesC8> aFmt,
            VA_LIST& aList
            );

        /**
        * Write hex dump.
        * @param aLevel Log level.
        * @param aHeader Header string.
        * @param aMargin Margin.
        * @param aPtr Data.
        * @param aLen Data length.
        */
        static void HexDump
            (
            TInt aLevel, 
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            );
        };

#endif /* def __TEST_ROAP_APP_LOG */

#ifdef __TEST_ROAP_APP_LOG

    /// Write formatted to log.
    #define CLOG( body ) RoapAppLogger::Write body
    /// Write hex dump.
    #define CDUMP( body ) RoapAppLogger::HexDump body
    /// Guard "log-only" argument name with this (expands to argument).
    #define LOG_ONLY( argName ) argName

#else /* not defined __TEST_ROAP_APP_LOG */

    /// Do nothing (log disabled).
    #define CLOG( body )
    /// Do nothing (log disabled).
    #define CDUMP( body )
    /// Guard "log-only" argument name with this (expands to nothing).
    #define LOG_ONLY( argName )

#endif /* def __TEST_ROAP_APP_LOG */

#ifdef _DEBUG

    /// Guard "debug-only" argument name with this (expands to argument).
    #define DEBUG_ONLY( argName ) argName

#else /* not defined _DEBUG */

    /// Guard "ldebug-only" argument name with this (expands to nothing).
    #define DEBUG_ONLY( argName )

#endif /* def _DEBUG */

#endif /* def ROAP_APP_LOGGER_H */
