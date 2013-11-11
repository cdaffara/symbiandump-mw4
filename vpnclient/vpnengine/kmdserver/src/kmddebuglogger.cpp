/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Write logs in debug builds
*
*/


// CLASS HEADER
#include "kmddebuglogger.h"

#if defined(_DEBUG)
_LIT(KLogFolder,"vpn");
_LIT(KLogFile,"kmd.txt");
#endif

// ======== MEMBER FUNCTIONS ========

#ifndef _DEBUG
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CKmdDebugLogger* CKmdDebugLogger::NewL()
    {
    CKmdDebugLogger* self = new ( ELeave ) CKmdDebugLogger;
    return self;        
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CKmdDebugLogger::~CKmdDebugLogger()
    {    
    }

#else

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CKmdDebugLogger* CKmdDebugLogger::NewL()
    {
    CKmdDebugLogger* self = new ( ELeave ) CKmdDebugLogger;
    self->ConstructL();
    return self;        
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CKmdDebugLogger::~CKmdDebugLogger()
    {
    delete iIkePcapTrace;
    iFileLogger.Close();
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::ConstructL()
    {    
    User::LeaveIfError( iFileLogger.Connect() );
    iFileLogger.SetDateAndTime( ETrue, ETrue );
    iFileLogger.CreateLog( KLogFolder, KLogFile, EFileLoggingModeAppend );	
   
    iIkePcapTrace = CIkePcapTrace::NewL(KLogFolder);
    }

// ---------------------------------------------------------------------------
// Writes to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWrite( const TDesC& aText )
    {
    const TInt KMaxLineWidth( 100 );
    const TInt textLength( aText.Length() );
    TInt charsLeft( textLength );
    
    while ( charsLeft )
        {
        if ( charsLeft >= KMaxLineWidth )
            {
            // Write next KMaxLineWidth chars.
            iFileLogger.Write( aText.Mid( textLength-charsLeft, KMaxLineWidth ) );
            charsLeft -= KMaxLineWidth;            
            }
        else
            {
            // Write remaining chars (<KMaxLineWidth chars).
            iFileLogger.Write( aText.Mid( textLength-charsLeft, charsLeft ) );
            charsLeft = 0;            
            }
        }
    }

// ---------------------------------------------------------------------------
// Writes to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWrite( const TDesC8& aText )
    {
    const TInt KMaxLineWidth( 100 );
    const TInt textLength( aText.Length() );
    TInt charsLeft( textLength );
    
    while ( charsLeft )
        {
        if ( charsLeft >= KMaxLineWidth )
            {
            // Write next KMaxLineWidth chars.
            iFileLogger.Write( aText.Mid( textLength-charsLeft, KMaxLineWidth ) );
            charsLeft -= KMaxLineWidth;            
            }
        else
            {
            // Write remaining chars (<KMaxLineWidth chars).
            iFileLogger.Write( aText.Mid( textLength-charsLeft, charsLeft ) );
            charsLeft = 0;            
            }
        }
    }

// ---------------------------------------------------------------------------
// Writes to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWriteF( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );

    iFileLogger.WriteFormat( aFmt, list );    
    }

// ---------------------------------------------------------------------------
// Writes to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWriteF( TRefByValue<const TDesC8> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list,aFmt );

    iFileLogger.WriteFormat( aFmt, list );    
    }

// ---------------------------------------------------------------------------
// Writes array to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWriteArray( const TUint8* aArray, TInt aLength )
    {
    ASSERT(aArray);
    HBufC *buf = HBufC::New( aLength*4+1 ); // max 3 num and a blank (+1 for /n)
    if ( !buf )
        {
        return;
        }

    for ( TInt i=0; i<aLength; i++ )
        {
        if ( i%4 == 0 )
            {
            buf->Des().AppendFormat(_L(" "));
            }
        buf->Des().AppendFormat( _L("%02.2x"), aArray[i] ); // key Data byte2byte
        }

    LogWrite( buf->Des() );

    delete buf;
    buf = NULL;    
    }

// ---------------------------------------------------------------------------
// Writes number to log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::LogWriteNum( TUint aNum )
    {
    const TInt KMaxNumLength( 20 );
    TBuf<KMaxNumLength> buf;
    buf.AppendFormat( _L("%u\n"), aNum );  // key Data byte2byte
    LogWrite( buf );
    }

// ---------------------------------------------------------------------------
// Writes message to PCap log.
// ---------------------------------------------------------------------------
//
void CKmdDebugLogger::TraceMessage(const TDesC8& aMessage, 
                                   const TInetAddr& aSourceAddress, 
                                   const TInetAddr& aDestinationAddress,
                                   CIkePcapTrace::TEncryptionType aEncryptionType)
    {
    iIkePcapTrace->TraceMessage(aMessage, 
                                aSourceAddress, aDestinationAddress,
                                aEncryptionType);
    }


#endif //_DEBUG
