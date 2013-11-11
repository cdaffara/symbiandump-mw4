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


#ifndef _INC_TSVGCOLOR_
#define _INC_TSVGCOLOR_

#include <e32std.h>
#include <fbs.h>

#include "GfxColor.h"



const TUint32 KSvgCurrentColor = 0x2ffffff;

class MXmlElement;

//Subclass of TGfxColor

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class  TSvgColor : public TGfxColor
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgColor( TUint32 aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgColor( TInt aRed, TInt aGreen, TInt aBlue );


		public:
		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */

		TBool GetStringL( const TDesC& aColorValue, TUint32 &aColor);


		private:

		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		TUint32           MapColorToInt( const TDesC& aColorName );


	private:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TUint32         ProcessRGBL( const TDesC& aColorValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool           ProcessRGBPercentageL( const TDesC& aColorValue,
                                              TInt& color );

    };

#endif
