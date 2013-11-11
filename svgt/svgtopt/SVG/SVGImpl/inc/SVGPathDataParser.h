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


#ifndef __TSVGPATHDATAPARSER_H__
#define __TSVGPATHDATAPARSER_H__

#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxGeneralPath.h"



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class TSvgPathDataParser
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void   ParsePathData( const TDesC& aData,
                                       CGfxGeneralPath* aPath );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static void   ParsePointDataL( const TDesC& aData,
                                        CGfxGeneralPath* aPath );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static TFloatFixPt   PathLengthL( CGfxGeneralPath* aPath );

    private:
        /**
         * Leavable ParsePathData method.
         *
         * @since 1.0
         * @param
         * @return
         */
		
		static void   ParsePathDataL( const TDesC& aData,
                                        CGfxGeneralPath* aPath );
	

    };


#endif /* __TSvgPathDataParser_H__ */
