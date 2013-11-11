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


#ifndef _INC_CSVGLANGSPACEIMPL_
#define _INC_CSVGLANGSPACEIMPL_

#include "SVGLangSpace.h"



/**
 * Class description
 *
 *  SVG elements' superclass.
 *
 */
class CSvgLangSpaceImpl : public CBase
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&                XMLLang();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetXMLLangL( const TDesC& aXMLLang );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&                XMLSpace();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetXMLSpaceL( const TDesC& aXMLSpace );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       SetLangSpaceAttributeL( const TDesC& aPropertyName,
                                                            const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     ~CSvgLangSpaceImpl();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgLangSpaceImpl*   NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgLangSpaceImpl*   NewLC();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgLangSpaceImpl* CloneL();

    protected:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        ConstructL();

        HBufC*                      iXMLLang;

        HBufC*                      iXMLSpace;
    };

#endif /* _INC_CSVGELEMENTIMPL_ */
