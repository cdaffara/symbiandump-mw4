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


#ifndef _INC_MSVGLANGSPACE_
#define _INC_MSVGLANGSPACE_

#include <e32base.h>
#include <badesca.h>



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgLangSpace
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&    XMLLang() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt            SetXMLLangL( const TDesC& aXMLLang ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&    XMLSpace() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt            SetXMLSpaceL( const TDesC& aXMLSpace ) = 0;
    };

#endif /* _INC_MSVGLANGSPACE_ */
