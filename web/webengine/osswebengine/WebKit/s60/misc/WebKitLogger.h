/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#ifndef WEBKITLOGGER_H
#define WEBKITLOGGER_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <flogger.h>
#include "StaticObjectsContainer.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


class LogStream;

LogStream & flush(LogStream& stream);
LogStream & mem(LogStream& stream);

/**
*
*  @lib resLoader.lib
*  @since 3.0
*/
class LogStream 
    {
    public:
        LogStream();
        LogStream& operator<<(TInt aInt );
        LogStream& operator<<(TUint aUint );
        LogStream& operator<<(TReal);
        LogStream& operator<<(const char* aString );
        LogStream& operator<<(const TDesC& aString );
        LogStream& operator<<(const TDesC8& );
        LogStream& operator<<(void *);

        // LogStream& operator<<(const LogStreamManipulator& );
        void flush();
        void mem();

    private:

        TBuf8<1024>  iString;
        TBool iLoggerReady;
    };

/**
*
*  @lib resLoader.lib
*  @since 3.0
*/
class TWebKitDebugStream;

typedef TWebKitDebugStream &  (*DEBUGFUNC)( TWebKitDebugStream& );

class TWebKitDebugStream
    {
    public:

#if ( defined(__WINSCW__) || defined(PERF_REGRESSION_LOG) )
        TWebKitDebugStream& operator<<( TInt aInt ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aInt; return *this; }
        TWebKitDebugStream& operator<<( TUint aUint ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aUint; return *this; }
        TWebKitDebugStream& operator<<( TReal aReal ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aReal; return *this; }
        TWebKitDebugStream& operator<<( const char* aString ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aString; return *this; }
        TWebKitDebugStream& operator<<( const TAny* aAny ) { WebCore::StaticObjectsContainer::instance()->logStream()<<(void*)aAny; return *this; }
        TWebKitDebugStream& operator<<( const TDesC& aString ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aString; return *this; }
        TWebKitDebugStream& operator<<( const TDesC8& aString ) { WebCore::StaticObjectsContainer::instance()->logStream()<<aString; return *this; }
        TWebKitDebugStream& operator<<( DEBUGFUNC aFN ) { aFN(*this); return *this; }
#else
        TWebKitDebugStream& operator<<( TInt aInt ) { return *this; }
        TWebKitDebugStream& operator<<( TUint aUint ) { return *this; }
        TWebKitDebugStream& operator<<( TReal aReal ) { return *this; }
        TWebKitDebugStream& operator<<( const char* aString ) { return *this; }
        TWebKitDebugStream& operator<<( const TAny* aAny ) { return *this; }
        TWebKitDebugStream& operator<<( const TDesC& aString ) { return *this; }
        TWebKitDebugStream& operator<<( const TDesC8& aString ) { return *this; }
        TWebKitDebugStream& operator<<( DEBUGFUNC aFN )     { return *this; }
#endif
};

#if ( defined(__WINSCW__) || defined(PERF_REGRESSION_LOG) )
inline TWebKitDebugStream& flush(TWebKitDebugStream& s) { flush(WebCore::StaticObjectsContainer::instance()->logStream()); return s; }
inline TWebKitDebugStream& mem(TWebKitDebugStream& s) { mem(WebCore::StaticObjectsContainer::instance()->logStream()); return s; }
#else
inline TWebKitDebugStream& flush(TWebKitDebugStream& s) { return s; }
inline TWebKitDebugStream& mem(TWebKitDebugStream& s) { return s; }
#endif

inline TWebKitDebugStream wkDebug() { return TWebKitDebugStream(); }

#endif // WEBKITLOGGER_H
