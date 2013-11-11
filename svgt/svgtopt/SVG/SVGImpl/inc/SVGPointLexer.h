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
* Description:  SVG Implementation header file
 *
*/


// TSvgPointLexer:
// Parse a text string and returns a set of coordinates and the last character
// before the coordinates. e.g. " M100, 30.2" -> 'M', 100.0 and 30.2
//
#ifndef __TSVGPOINTLEXER_H__
#define __TSVGPOINTLEXER_H__

#include <e32std.h>
#include "GfxFloatFixPt.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class TSvgPointLexer
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgPointLexer( const TDesC& aDes );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt32  GetNextPoint( TChar& aCommand, TFloatFixPt& aX, TFloatFixPt& aY );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt32  GetNext( TChar& aCommand, TFloatFixPt& aVal );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TBool GetNextWithNumOfPoints( TFloatFixPt *aVal, TInt NoOfPoints);
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TBool GetCommand(TUint8 &a);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool   IsDone()
			{
			// skip any space to see that this is the last character. ADDED BY RAVINDER
			iLex.SkipSpace();
			// END OF ADDITION.
            return iLex.Eos();
            };

        void Cleanup();
    private:
        TLex    iLex;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TChar   SkipUntilNum();

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TBool SkipToNumEndWithoutLex(TReal32 &aNum);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool   SkipToNumEnd();
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool   iNegativeNumber;
		TUint16	*iData;
        TUint16 *iDataEnd;

        HBufC* iDataCopy;
    };

#endif // __TSvgPointLexer_H__
