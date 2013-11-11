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


#ifndef __INC_CSVGPRESERVEASPECTRATIOIMPL__
#define __INC_CSVGPRESERVEASPECTRATIOIMPL__

#include "SVGPreserveAspectRatio.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgPreserveAspectRatioImpl : public CBase, public MSvgPreserveAspectRatio
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
        static CSvgPreserveAspectRatioImpl*     NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgPreserveAspectRatioImpl*     NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual                                 ~CSvgPreserveAspectRatioImpl();

        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TSvgPreserveAspectAlignType            GetAlign();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        TSvgMeetOrSliceType                     GetMeetOrSlice();

        // Class-specified


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                    SetAlign( TSvgPreserveAspectAlignType aType );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                    SetMeetOrSlice( TSvgMeetOrSliceType aType );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgPreserveAspectRatioImpl* CloneL();

    private:

        TSvgMeetOrSliceType                     iMeetOrSlice;

        TSvgPreserveAspectAlignType             iAlign;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                                    ConstructL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                                CSvgPreserveAspectRatioImpl();
    };

#endif /* #ifndef __INC_CSVGPRESERVEASPECTRATIOIMPL__ */
