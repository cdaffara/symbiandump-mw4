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
*      Logging macros for COD Handler.   
*      
*
*/


#ifndef COD_LOGGER_H
#define COD_LOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

// TYPES

enum TCodLogMask                ///< Log mask bits.
    {
    ELogOff     = 0x00000000,   ///< Don't log.
    EWapConn    = 0x00000001,   ///< Log WAP connection.
    ETcpConn    = 0x00000002,   ///< Log TCP connection.
    EConn       = 0x00000004,   ///< Log Connection.
    EHttpLoad   = 0x00000008,   ///< Log HTTP Loader.
    EWspHeader  = 0x00000010,   ///< Log WSP header handling.
    ECodEng     = 0x00000020,   ///< Log COD Engine.
    ECharset    = 0x00000040,   ///< Log charset conversions.
    EParse      = 0x00000080,   ///< Log parsing.
    ECodStorage = 0x00000100,   ///< Log Storage.
    ELogAll     = 0xFFFFFFFF    ///< Log all.
    };

// MACROS

/// Determines what to log. Construct this from TCodLogMask values.
#define COD_LOG_MASK ELogAll
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
#define COD_LOG_LEVEL 4

#ifdef _DEBUG

    /// Defining this enables COD logging. Needs flogger.lib.
    #define __TEST_COD_LOG

#endif /* def _DEBUG */

#ifdef __TEST_COD_LOG

    // CLASS DECLARATION

    /**
    * Logger class.
    */
    NONSHARABLE_CLASS( CodLogger )
        {
        public:     // new methods

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC16> aFmt, ... );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt32 aMask,
            TInt aLevel,
            TRefByValue<const TDesC16> aFmt,
            VA_LIST& aList
            );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        */
        static void Write
            ( TInt32 aMask, TInt aLevel, TRefByValue<const TDesC8> aFmt, ... );

        /**
        * Write formatted log.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aFmt Format string.
        * @param aList Variable argument list.
        */
        static void Write
            (
            TInt32 aMask,
            TInt aLevel,
            TRefByValue<const TDesC8> aFmt,
            VA_LIST& aList
            );

        /**
        * Write hex dump.
        * @param aMask Log mask.
        * @param aLevel Log level.
        * @param aHeader Header string.
        * @param aMargin Margin.
        * @param aPtr Data.
        * @param aLen Data length.
        */
        static void HexDump
            (
            TInt32 aMask,
            TInt aLevel, 
            const TText* aHeader,
            const TText* aMargin,
            const TUint8* aPtr,
            TInt aLen
            );
        };

#endif /* def __TEST_COD_LOG */

#ifdef __TEST_COD_LOG

    /// Write formatted to log.
    #define CLOG( body ) CodLogger::Write body
    /// Write hex dump.
    #define CDUMP( body ) CodLogger::HexDump body
    /// Guard "log-only" argument name with this (expands to argument).
    #define LOG_ONLY( argName ) argName

#else /* not defined __TEST_COD_LOG */

    /// Do nothing (log disabled).
    #define CLOG( body )
    /// Do nothing (log disabled).
    #define CDUMP( body )
    /// Guard "log-only" argument name with this (expands to nothing).
    #define LOG_ONLY( argName )

#endif /* def __TEST_COD_LOG */

#ifdef _DEBUG

    /// Guard "debug-only" argument name with this (expands to argument).
    #define DEBUG_ONLY( argName ) argName

#else /* not defined _DEBUG */

    /// Guard "debug-only" argument name with this (expands to nothing).
    #define DEBUG_ONLY( argName )

#endif /* def _DEBUG */

#endif /* def COD_LOGGER_H */
