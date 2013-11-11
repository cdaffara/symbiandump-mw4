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



#ifndef __INC_MSVGLOCATABLE__
#define __INC_MSVGLOCATABLE__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif


#include "GfxAffineTransform.h"
#include "GfxRectangle2D.h"


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgLocatable
    {
    public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual const TGfxAffineTransform& GetCTM() = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void                SetCTM( TGfxAffineTransform& aTr ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void                GetBBox( TGfxRectangle2D& aBbox ) = 0;
    };

#endif /* __INC_MSVGLOCATABLE__ */
