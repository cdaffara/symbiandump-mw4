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


#ifndef C_KMDDEBUGLOGGER_H
#define C_KMDDEBUGLOGGER_H

#include <e32base.h>
#include <flogger.h>

#include "ikedebug.h"

/**
 *  KMD debug logger.
 *  Logger for writing logs in debug builds.
 *  @lib internal (kmdserver.exe)
 */
class CKmdDebugLogger : public CBase,
                        public MIkeDebug
    {
public:

    /**
     * Two-phased constructor.
     */    
    static CKmdDebugLogger* NewL();
    
    /**
    * Destructor.
    */    
    ~CKmdDebugLogger();

#ifdef _DEBUG

    void LogWrite( const TDesC& aText );
    void LogWrite( const TDesC8& aText );
    
    void LogWriteF( TRefByValue<const TDesC> aFmt, ... );
    void LogWriteF( TRefByValue<const TDesC8> aFmt, ... );
    
    void LogWriteArray( const TUint8* aArray, TInt aLength );
    
    void LogWriteNum( TUint aNum );
    
    void TraceMessage( const TDesC8& aMessage, 
                       const TInetAddr& aSourceAddress, 
                       const TInetAddr& aDestinationAddress,
                       CIkePcapTrace::TEncryptionType aEncryptionType );
   
private:
    void ConstructL();
        
    /**
     * File logger.
     * Own.
     */
    RFileLogger     iFileLogger;

    /**
     * PCap tracer.
     * Own.
     */
    CIkePcapTrace*  iIkePcapTrace;
    
#endif // _DEBUG    
    };

#endif // C_KMDDEBUGLOGGER_H
