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
// Define Debug API for phone as modem at extension plugin
// 
//

#ifndef ATCMDPAM_DEBUG_H
#define ATCMDPAM_DEBUG_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>

// FORWARD DECLARATIONS
class CATCommand;

#ifdef ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES
#define DebugMsg( msg ) CATCmdPAMDebug::NoteL( msg );
#else // !ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES
#define DebugMsg( msg )
#endif // ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES

#ifdef ATCMDPAM_DEBUG_TRACES

NONSHARABLE_CLASS( TOverflowTruncate ) : public TDes16Overflow
    {
    public:
	    virtual void Overflow( TDes16& aDes );
	};

NONSHARABLE_CLASS( TOverflowTruncate8 ) : public TDes8Overflow
    {
    public:
	    virtual void Overflow( TDes8& aDes );
	};



class CATCmdPAMDebug 
    : public CBase
    {
    public:
#ifdef ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES
		/**
        * Debug message
        * @param aNote Message
        */
        static void NoteL( TPtrC aNote );
#endif // ATCMDPAM_DEBUG_INFORMATION_NOTE_MESSAGES

		/**
        * Log AT command / response with flogger
        * @param aCmd
        */
        static void ATLog( CATCommand& aCmd );

#ifndef ATCMDPAM_DEBUG_NOFLOGGER
		/**
        * Debug print
        * @param aFmt.,, Print content
        */
        static void Print( TRefByValue<const TDesC> aFmt,... );

		/**
        * Debug print8
        * @param aFmt.,, Print content
        */
        static void Print8( TRefByValue<const TDesC8> aFmt,... );
#endif // !ATCMDPAM_DEBUG_NOFLOGGER
    };
#endif // ATCMDPAM_DEBUG_TRACES

#ifdef ATCMDPAM_DEBUG_TRACES
#define ATDEBUG( f ) { f; }
#ifndef ATCMDPAM_DEBUG_NOFLOGGER
#define DebugPrint CATCmdPAMDebug::Print
#define DebugPrint8 CATCmdPAMDebug::Print8
#define ATLog CATCmdPAMDebug::ATLog
#else // ATCMDPAM_DEBUG_NOFLOGGER
#define DebugPrint (void)
#define DebugPrint8 (void)
#define ATLog RDebug::Print
#endif // !ATCMDPAM_DEBUG_NOFLOGGER
TText* DEBUG_STR( TDes* aStr );
TText16* DEBUG_STR( TDes16& aStr );
TText* DEBUG_STR( TPtr aStr );
TText* DEBUG_STR( HBufC* aStr );
TText* DEBUG_STR( TDesC* aStr );
TText* DEBUG_STR( const TDes& aStr );
TText* DEBUG_STR( const TPtrC& aStr );
TText* DEBUG_STR( const TDesC& aStr );
TText8* DEBUG_STR( const TDes8& aStr );
TText8* DEBUG_STR( const TDesC8& aStr );
TText8* DEBUG_STR( TDes8& aStr );
void DEBUG_BUF( TDes aBuf, TRefByValue<const TDesC> aFmt,... );
#else // !ATCMDPAM_DEBUG_TRACES
// Compiler preprosessor should ignore this line with it's content, therefore
// parameter format and function inline prefix are meaningless. Function parameters
// may not have operations build-in.
inline void DebugPrint( TRefByValue<const TDesC> aFmt,... ) { (void) aFmt; };
inline void DebugPrint8( TRefByValue<const TDesC8> aFmt,... ) { (void) aFmt; };

#define DEBUG_STR( str ) str
#define ATDEBUG( f )
#endif // ATCMDPAM_DEBUG_TRACES

#endif // ATCMDPAM_DEBUG_H

// End of File
