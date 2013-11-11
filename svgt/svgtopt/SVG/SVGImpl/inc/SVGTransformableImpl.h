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


#ifndef __INC_CSVGTRANSFORMABLEIMPL__
#define __INC_CSVGTRANSFORMABLEIMPL__

#include "SVGTransformable.h"
#include "SVGTransformListImpl.h"

class TGfxAffineTransform;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTransformableImpl : public CBase, public MSvgTransformable
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
        static CSvgTransformableImpl*   NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        static CSvgTransformableImpl*   NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual                         ~CSvgTransformableImpl();

        // From MSvgTransformable


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                            GetTransform( MSvgTransformList*& aTransformList );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                            SetTransform( MSvgTransformList*& aTransformList );


		void                            SetTransformList(TGfxAffineTransform& aTr);

        // From MSvgLocatable


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        const TGfxAffineTransform&             GetCTM();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                            SetCTM( TGfxAffineTransform& aTr );


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void GetBBox( TGfxRectangle2D& /*aBbox*/ )
            {
            }

    private:

        TGfxAffineTransform     iCTM;

        CSvgTransformListImpl*  iTransform;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        void                    ConstructL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
                                CSvgTransformableImpl();
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        CSvgTransformableImpl* CloneL();

    };

#endif /* __INC_MSVGTRANSFORMABLE__ */