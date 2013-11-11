/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definitions for logging in debug builds
*
*/

#ifndef M_IKEDEBUG_H
#define M_IKEDEBUG_H

#include <e32base.h>

#ifndef _DEBUG

class MIkeDebug
    {
public:
    };


#define DEBUG_LOG(a) 
#define DEBUG_LOG1(a, b) 
#define DEBUG_LOG2(a, b, c) 
#define DEBUG_LOG3(a, b, c, d)
#define DEBUG_LOG_ARRAY(a, b)
#define DEBUG_LOG_NUM(a)
#define TRACE_MSG(aMsg, aSrcAddr, aDstAddr, aEncryptType)

#else

#include "ikepcaptrace.h"

// FORWARD DECLARATIONS
class TInetAddr;

/**
 *  KMD debug interface for logging in debug builds.
 *  @lib internal (kmdserver.exe)
 */
class MIkeDebug
    {
public:

    virtual void LogWrite( const TDesC& aText ) = 0;
    virtual void LogWrite( const TDesC8& aText ) = 0;
    
    virtual void LogWriteF( TRefByValue<const TDesC> aFmt, ... ) = 0;
    virtual void LogWriteF( TRefByValue<const TDesC8> aFmt, ... ) = 0;
    
    virtual void LogWriteArray( const TUint8* aArray, TInt aLength ) = 0;
    
    virtual void LogWriteNum( TUint aNum ) = 0;
    
    virtual void TraceMessage( const TDesC8& aMessage, 
                               const TInetAddr& aSourceAddress, 
                               const TInetAddr& aDestinationAddress,
                               CIkePcapTrace::TEncryptionType aEncryptionType ) = 0;
            
    };

#define DEBUG_LOG(a) iDebug.LogWrite((a))
#define DEBUG_LOG1(a, b) iDebug.LogWriteF((a), (b))
#define DEBUG_LOG2(a, b, c) iDebug.LogWriteF((a), (b), (c))
#define DEBUG_LOG3(a, b, c, d) iDebug.LogWriteF((a), (b), (c), (d))
#define DEBUG_LOG_ARRAY(a, b) iDebug.LogWriteArray((a), (b))
#define DEBUG_LOG_NUM(a) iDebug.LogWriteNum((a))

#define TRACE_MSG(aMsg, aSrcAddr, aDstAddr, aEncryptType) iDebug.TraceMessage((aMsg), (aSrcAddr), (aDstAddr), (aEncryptType))
#define TRACE_MSG_IKEV1(aMsg, aSrcAddr, aDstAddr ) iDebug.TraceMessage((aMsg), (aSrcAddr), (aDstAddr), (CIkePcapTrace::EEncrDes))

#endif //_DEBUG
#endif //M_IKEDEBUG_H
