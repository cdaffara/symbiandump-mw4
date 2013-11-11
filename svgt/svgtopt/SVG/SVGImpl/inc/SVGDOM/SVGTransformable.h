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


#ifndef __INC_MSVGTRANSFORMABLE__
#define __INC_MSVGTRANSFORMABLE__

#include "SVGLocatable.h"
#include "SVGTransformList.h"

class TGfxAffineTransform;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgTransformable : public MSvgLocatable
    {
    public:

        /**
         * Gets transformation list. The first entry should be the original
         * transformation, and the other entries should be the matrices
         *  appended for animations on the element (if any).
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
        virtual void    GetTransform( MSvgTransformList*& aTransformList ) = 0;

        // Temporarily, non-DOM method. This could go to the implementation.

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void    SetTransform( MSvgTransformList*& aTransformList ) = 0;
    };

#endif /* __INC_MSVGTRANSFORMABLE__ */
