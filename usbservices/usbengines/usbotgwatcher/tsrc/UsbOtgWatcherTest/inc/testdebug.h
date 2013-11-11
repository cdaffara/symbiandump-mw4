/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef TESTDEBUG_H_
#define TESTDEBUG_H_

//  INCLUDES
#include <e32debug.h>
#include <e32def.h>

// CONSTANTS
_LIT8(KStifTestPrefix8, "[STIF_LOG] ");
_LIT( KStifTestPrefix, "[STIF_LOG] ");
_LIT8( KStifEntryPrefix8, ">>> %S" );
_LIT8( KStifExitPrefix8, "<<< %S" );

const TInt KMaxLogLineLength = 512;
typedef TBuf8< KMaxLogLineLength > TLogBuf8;
typedef TBuf16< KMaxLogLineLength > TLogBuf16;

// CLASS DECLARATION
NONSHARABLE_CLASS( TTestLogger )
    {
public:
    static inline TLogBuf16 CreateLogBuf( TRefByValue<const TDesC8> aFmt, ... );
    static inline TLogBuf16 CreateLogBuf( TRefByValue<const TDesC16> aFmt, ... );
    static inline void Trace( CStifLogger& aLog, TLogBuf16 aLogBuf );
    };

// MACROS
#define TRACE( p ) {\
        TLogBuf16 buf = TTestLogger::CreateLogBuf( _L( p ) );\
        TTestLogger::Trace( *iLog, buf );\
        }
#define TRACE_INFO( p, args... ) {\
        TLogBuf16 buf = TTestLogger::CreateLogBuf( _L( p ), args );\
        TTestLogger::Trace( *iLog, buf );\
        }
#define TRACE_FUNC_ENTRY {\
        TPtrC8 ptr8( ( TUint8* ) __FUNCTION__ );\
        TLogBuf16 buf = TTestLogger::CreateLogBuf( KStifEntryPrefix8, &ptr8 );\
        TTestLogger::Trace( *iLog, buf );\
        }
#define TRACE_FUNC_EXIT {\
        TPtrC8 ptr8( ( TUint8* ) __FUNCTION__ );\
        TLogBuf16 buf = TTestLogger::CreateLogBuf( KStifExitPrefix8, &ptr8 );\
        TTestLogger::Trace( *iLog, buf );\
        }

#include "testdebug.inl"

#endif /* TESTDEBUG_H_ */
