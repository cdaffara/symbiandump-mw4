// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements Debug API for phone as modem at extension plugin
// 
//

// INCLUDES
#include <e32std.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <atcodec.h>
#include <AknNoteWrappers.h>
#include <flogger.h>
#include "atcmdpam_debug.h"
#ifdef ATCMDPAM_DEBUG_TRACES
#pragma message( "Traces enabled" )
#endif // ATCMDPAM_DEBUG_TRACES
#ifdef ATCMDPAM_DEBUG_BTTRACES
#include <btlog.h>
#endif // ATCMDPAM_DEBUG_BTTRACES
// CONSTANTS
#ifdef ATCMDPAM_DEBUG_TRACES
_LIT( KDebugFile, "atcmdpam.txt" );
_LIT( KDebugDir, "atcmdpam" );
_LIT( KATDebugFile, "log.txt" );
_LIT( KATDebugDir, "atcmdpam" );
#endif // ATCMDPAM_DEBUG_TRACES

// ============================= LOCAL FUNCTIONS  =============================

/**
 Debug str
 */
#ifdef ATCMDPAM_DEBUG_TRACES
TText* DEBUG_STR( TDes* aStr )
    {
    if ( aStr )
        {
        return DEBUG_STR( *aStr );
        }
    else
        {
        return const_cast<TText*>( _L( "" ).Ptr() );
        }
    }

/**
 Debug str
 */
TText16* DEBUG_STR( TDes16& aStr )
    {
    if ( aStr.Length() < aStr.MaxLength() )
        {
        aStr.ZeroTerminate();
        }
    TText16* txt = const_cast<TText16*>( aStr.Ptr() );

    return txt;
    }

/**
 Debug str
 */
TText8* DEBUG_STR( TDes8& aStr )
    {
    if ( aStr.Length() < aStr.MaxLength() )
        {
        aStr.ZeroTerminate();
        }
    TText8* txt = const_cast<TText8*>( aStr.Ptr() );

    return txt;
    }

/**
 Debug str
 */
TText* DEBUG_STR( TPtr aStr )
    {
    if ( aStr.Length() < aStr.MaxLength() )
        {
        aStr.ZeroTerminate();
        }
    TText* txt = const_cast<TText*>( aStr.Ptr() );

    return txt;
    }

/**
 Debug str
 */
TText* DEBUG_STR( HBufC* aStr )
    {
    if ( aStr )
        {
        return DEBUG_STR( aStr->Des() );
        }
    else
        {
        return const_cast<TText*>( _L( "" ).Ptr() );
        }
    }

/**
 Debug str
 */
TText* DEBUG_STR( TDesC* aStr )
    {
    if ( aStr )
        {
        TText* txt = const_cast<TText*>( aStr->Ptr() );
        return txt;
        }
    else
        {
        return const_cast<TText*>( _L( "" ).Ptr() );
        }
    }

/**
 Debug str
 */
TText* DEBUG_STR( const TDes& aStr )
    {
    return DEBUG_STR( const_cast<TDes&>( aStr ) );
    }

/**
 Debug str
 */
TText8* DEBUG_STR( const TDes8& aStr )
    {
    return DEBUG_STR( const_cast<TDes8&>( aStr ) );
    }

/**
 Debug str
 */
TText* DEBUG_STR( const TPtrC& aStr )
    {
    return const_cast<TText*>( aStr.Ptr() );
    }

/**
 Debug str
 */
TText* DEBUG_STR( const TDesC& aStr )
    {
    return const_cast<TText*>( aStr.Ptr() );
    }

/**
 Debug str
 */
TText8* DEBUG_STR( const TDesC8& aStr )
    {
    return const_cast<TText8*>( aStr.Ptr() );
    }

// ============================= MEMBER FUNCTIONS =============================

/**
 Ignore overflow
 */
void TOverflowTruncate::Overflow( TDes16& /* aDes */)
    {
    return;
    }

/**
 Ignore overflow
 */
void TOverflowTruncate8::Overflow( TDes8& /* aDes */)
    {
    return;
    }

/**
 Debug print
 (static)
 */
#ifndef ATCMDPAM_DEBUG_NOFLOGGER
void CATCmdPAMDebug::Print( TRefByValue<const TDesC> aFmt,... )
    {
    TOverflowTruncate overflow;
    VA_LIST list;
    VA_START( list,aFmt );
    TBuf<256> buf;
    buf.AppendFormatList( aFmt, list, &overflow );

    // Output to IDE
    RDebug::Print( buf );

    // Output to file
    RFileLogger::WriteFormat( KDebugDir,
            KDebugFile,
            EFileLoggingModeAppend,
            buf
    );

#ifdef ATCMDPAM_DEBUG_BTTRACES
    BTLog::Print( buf );
#endif // ATCMDPAM_DEBUG_BTTRACES
    return;
    }
#endif // !ATCMDPAM_DEBUG_NOFLOGGER

/**
 Debug print
 (static)
 */
#ifndef ATCMDPAM_DEBUG_NOFLOGGER
void CATCmdPAMDebug::Print8( TRefByValue<const TDesC8> aFmt,... )
    {
    TOverflowTruncate8 overflow8;
    VA_LIST list;
    VA_START( list,aFmt );
    TBuf8<256> buf8;
    buf8.AppendFormatList( aFmt, list, &overflow8 );

    HBufC* buf = HBufC::New( buf8.Length() + 1 );
    if ( buf )
        {
        buf->Des().Copy( buf8 );
        buf->Des().ZeroTerminate();
        CATCmdPAMDebug::Print( buf->Des() );
        delete buf;
        }

    return;
    }
#endif // !ATCMDPAM_DEBUG_NOFLOGGER

/**
 Debug message
 (static, may leave)
 */
#ifdef ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES
void CATCmdPAMDebug::NoteL( TPtrC aNote )
    {
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    (void) note->ExecuteLD( aNote );

    return;
    }
#endif // ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES

/**
 Debug print
 (static)
 */
void CATCmdPAMDebug::ATLog( CATCommand& aCmd )
    {
    TBuf8<CATCommand::EMaxCmdSize> buf;
    const TInt err = aCmd.Des( buf );
    if ( !err )
        {
        // Output to file
        RFileLogger::WriteFormat( KATDebugDir, 
                                  KATDebugFile, 
                                  EFileLoggingModeAppend, 
                                  buf 
                                );
        }

    return;
    }
#endif // ATCMDPAM_DEBUG_TRACES

// ========================= OTHER EXPORTED FUNCTIONS =========================

//  End of File  
