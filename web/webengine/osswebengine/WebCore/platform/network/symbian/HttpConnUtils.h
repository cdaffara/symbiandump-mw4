/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef THttpConnUtils_H
#define THttpConnUtils_H

//  INCLUDES
#include <e32base.h>
#include <BrCtlDefs.h>
#include "HttpDefs.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RStringPool;
class RStringF;
class THTTPHdrVal;
class RString;
class RFs;

/**
*  This is an internal utility class
*/
class THttpConnUtils
    {
    public:  // New functions

        /**
        * Panic the loader
        * @since 3.1
        * @param aError OS wide error code
        * @return
        */
        static void PanicLoader( TInt aError );

        /**
        * Write log
        * @since 3.1
        * @param
        * @return
        */
        static void WriteLog( TPtrC aBuf, TInt aStatus );

        /**
        * Checks if the content is wml
        * @since 3.1
        * @param
        * @return
        */
        static TBool WmlContent( const TDesC& aContentType );

        /**
        * Checks if the content is wml
        * @since 3.1
        * @param
        * @return
        */
        static TInt MapHttpErrors( TInt aError );
        /**
        * return true if intrapage navigation is needed
        * @since 3.1
        * @param aUrl1
        * @param aUrl2
        * @return
        */
        static TBool IsIntraPageNavigation( const TDesC& aUrl1, const TDesC& aUrl2 );

        /**
        * Convert the settings characterset from TUint to RString
        * @since 3.1
        * @param aStringPool The stringpool used for creating the RString.
        * @param aCharset The characterset representation to be converted.
        * @return The corresponding charset.
        */
        static THTTPHdrVal ConvertCharset( RStringPool aStringPool, TUint aCharset );

        /**
        * Create an RString object from a UCS2 buffer.
        * @since 3.1
        * @param aStringPool The stringpool to use for creating the string.
        * @param aStr The UCS2 string to be converted.
        * @param aString The new RString object.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt OpenStringFromUnicode( RStringPool aStringPool, TUint16* aStr,
                         RString& aString );

        /**
        * Convert a UCS2 buffer to ascii.
        * @since 3.1
        * @param aUsc2Ptr A descriptor to the buffer to convert.
        * @param aAsciiPtr A descriptor to the converted buffer.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt ConvertPtrUsc2ToAscii( const TPtrC& aUsc2Ptr, TPtrC8& aAsciiPtr, TUint8** retPtr );

    };

#endif      // THttpConnUtils_H

// End of File
