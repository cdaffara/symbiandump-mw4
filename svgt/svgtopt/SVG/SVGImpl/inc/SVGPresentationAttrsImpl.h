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


#ifndef _INC_CSVGPRESENTATIONATTRSIMPL_
#define _INC_CSVGPRESENTATIONATTRSIMPL_

#include "GfxColor.h"


/**
 * Class description
 *
 ***
 *** THIS CLASS IS NO LONGER IN USE
 ***
 */
class CSvgPresentationAttrsImpl : public CBase
    {
    private:
        // 'fill'
        TGfxColor                           aFill;

        // 'stroke'
        TGfxColor                           aStroke;

        // 'stroke-width
        TFixPt                              aStrokeWidth;

        // 'visibility'
        TInt8                               aVisibility;

        // 'color'
        TGfxColor                           aColor;

        // 'font-family'
        // TText aFontFamily;

        // 'font-size'
        TInt8                               aFontSize;

        // 'font-style'
        //TText aFontStyle;

        // 'font-weight'
        TInt8                               aFontWeight;

        // 'stroke-dasharray'
        TInt8*                              aStrokeDasharray;

        // 'display'
        TInt16                              aDisplay;

        // 'fill-rule'
        TInt8                               aFillRule;

        // 'stroke-linecap'
        TInt8                               aStrokeLinecap;

        // 'stroke-linejoin'
        TInt8                               aStrokeLinejoin;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                ConstructL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                            CSvgPresentationAttrsImpl();

    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgPresentationAttrsImpl*   NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgPresentationAttrsImpl*   NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TGfxColor                           Fill();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetFill( TGfxColor );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TGfxColor                           Stroke();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetStroke( TGfxColor );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TFixPt                              StrokeWidth();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetStrokeWidth( TFixPt );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               Visibility();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetVisibility( TInt8 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TGfxColor                           Color();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetColor();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        //TPtr FontFamily();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        //void SetFontFamily(TPtr);


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               FontSize();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetFontSize();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               FontWeight();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetFontWeight();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        // TInt8* StrokeDasharray();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        // void SetStrokeDasharray(TInt8*);


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt16                              Display();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetDisplay( TInt16 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               FillRule();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetFillRule( TInt8 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               StrokeLinecap();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetStrokeLinecap( TInt8 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TInt8                               StrokeLinejoin();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                SetStrokeLinejoin( TInt8 );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                            ~CSvgPresentationAttrsImpl();
    };

#endif /* _INC_CSVGPRESENTATIONATTRSIMPL_ */
