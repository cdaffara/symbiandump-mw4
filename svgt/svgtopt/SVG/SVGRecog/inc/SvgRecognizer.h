/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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



#if !defined(__SVGRECOG_H__)
#define __SVGRECOG_H__

#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

const TInt KMimeSvgRecognizerValue = 0x101F8570;
const TUid KUidMimeSvgRecognizer =
    {
    KMimeSvgRecognizerValue
    };

const TInt KImplementationValue = 0x101F8571;
const TUid KUidImplementation =
    {
    KImplementationValue
    };


// minimum amount of file needed to determine a text file IF it's not called .TXT
const TInt KMinBufferLength = 42;
// maximum amount of buffer space we will ever use
const TInt KMaxBufferLength = 1024;

_LIT8( KDataTypeImageSvg, "image/svg+xml" );

/**
 * A standard Symbian Recognizer for SVG MIME Type.
 * Inherits from CApaDataRecognizerType
 * Provides recognition for MIME Type 'image\svg+xml'
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgRecognizer : public CApaDataRecognizerType
    {
    public: // from CApaDataRecognizerType

        /**
         * Constructor
         *
         * @since 1.0
         * @param
         * @return
         */
                    CSvgRecognizer();

        /**
         * Return the preferred buffer size
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint       PreferredBufSize();

        /**
         * Return the Supported Data Type
         *
         * @since 1.0
         * @param
         * @return
         */
        TDataType   SupportedDataTypeL( TInt /*aIndex*/ ) const;
    private: // from CApaDataRecognizerType

        /**
         * Perform the required recognition process
         *
         * @since 1.0
         * @param aName - A string buffer containing file name
         * @param aBuffer - A binary buffer that has the contents of the file
         * @return
         */
        void        DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );

    public:
         /**
         * This is passes to the ECom Framework as an handle to the implementation
         *
         * @since 1.0
         * @param
         * @return
         */
 		static CApaDataRecognizerType* CreateRecognizerL();
    };

#endif
