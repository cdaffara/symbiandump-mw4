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


#ifndef __INC_MSVGZOOMANDPAN__
#define __INC_MSVGZOOMANDPAN__


enum TSvgZoomAndPanType
    {
    ESvgZoomAndPanDisable,
    ESvgZoomAndPanMagnify,
    ESvgZoomAndPanUnknown
    };


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgZoomAndPan
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TSvgZoomAndPanType  GetZoomAndPan() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void                SetZoomAndPan( TSvgZoomAndPanType aZoomAndPan ) = 0;
    };

#endif /* __INC_MSVGZOOMANDPAN__ */
