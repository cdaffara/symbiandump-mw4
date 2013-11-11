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


#ifndef __INC_CSVGFITTOVIEWBOXIMPL__
#define __INC_CSVGFITTOVIEWBOXIMPL__


#include "GfxRectangle2D.h"

#include "SVGPreserveAspectRatioImpl.h"
#include "SVGFitToViewBox.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgFitToViewBoxImpl : public CBase, public MSvgFitToViewBox
    {
    public:

        // Constructor/deconstructor


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgFitToViewBoxImpl*    NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgFitToViewBoxImpl*    NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgFitToViewBoxImpl();

        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            GetPreserveAspectRatio( MSvgPreserveAspectRatio*& aAspectRatio );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           GetViewBox( TGfxRectangle2D& aViewBox );

        // Class-specified


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetPreserveAspectRatioL( TSvgPreserveAspectAlignType aAlign,
                                                                 TSvgMeetOrSliceType aMeetslice );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetViewBoxL( TGfxRectangle2D aViewBox );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           ViewBoxDefined();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           SetViewBoxL( const TDesC& aAttributeName,
                                                    const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetWindowViewportTrans( TGfxRectangle2D aXYWH,
                                                                TGfxAffineTransform& aTr,
                                                                TSize aSize );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgFitToViewBoxImpl* CloneL();

    private:

        TGfxRectangle2D                 iViewBox;

        CSvgPreserveAspectRatioImpl*    iAspectRatio;

        TBool                           iViewBoxDefined;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgFitToViewBoxImpl();
    };

#endif /* #ifndef __INC_CSVGFITTOVIEWBOXIMPL__ */
