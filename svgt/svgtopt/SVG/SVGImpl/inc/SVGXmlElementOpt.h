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


#ifndef _INC_MXMLELEMENTOPT_
#define _INC_MXMLELEMENTOPT_

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

const TInt KErrInvalidAttribute = -11;
const TInt KErrNoAttribute = -10;

class   CGfxGeneralPath;
class   TFloatFixPt;
class   MSvgStylable;
class   CSvgElementImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MXmlElementOpt
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    GetAttributeIntL( const TInt aNameId, TInt32& aValue ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    SetAttributeIntL( const TInt aNameId,
                                          const TInt32 aValue ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    SetAttributeFloatL( const TInt aNameId,
                                            const TFloatFixPt aValue ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    GetAttributePath( const TInt aNameId,
                                          CGfxGeneralPath*& aValue ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    SetAttributePathL( const TInt aNameId,
                                           CGfxGeneralPath* aValue ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    GetAttributeDes( const TInt aNameId, TPtrC16& aValue ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt    SetAttributeDesL( const TInt aNameId,
                                          const TDesC& aValue ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

    };

#endif /* _INC_MXmlElementOpt_ */
