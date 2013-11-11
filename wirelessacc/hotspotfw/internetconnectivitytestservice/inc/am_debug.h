/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debug utilities
*
*/



#ifndef AM_DEBUG_H
#define AM_DEBUG_H

#include <e32svr.h>
#ifdef _DEBUG

/**
* Class for producing debug traces
* @lib
* @since Series 60 3.0
*/
class Debug
    {
    public:
        static inline void PrintBuffer( TUint aLength, const TUint8* aData );
        static inline void PrintString( const TDesC& aString, TUint aLength, const TUint8* aData );
        static inline void PrintTimestamp( const TDesC& aString );
    };

#define DEBUG(a)		            RDebug::Print(_L(a))
#define DEBUG1(a,b)		            RDebug::Print(_L(a),b)
#define DEBUG2(a,b,c)	            RDebug::Print(_L(a),b,c)


#include "am_debug.inl"

#else // _DEBUG

#define DEBUG(a)                    /* _DEBUG is not defined. */
#define DEBUG1(a,b)                 /* _DEBUG is not defined. */
#define DEBUG2(a,b,c)               /* _DEBUG is not defined. */


#endif // _DEBUG

#endif // AM_DEBUG_H
