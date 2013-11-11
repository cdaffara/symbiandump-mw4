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


#ifndef __INC_MSVGPRESERVEASPECTRATIO__
#define __INC_MSVGPRESERVEASPECTRATIO__

#include "SVGEngineInterfaceImpl.h"

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgPreserveAspectRatio
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TSvgPreserveAspectAlignType GetAlign() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TSvgMeetOrSliceType GetMeetOrSlice() = 0;
    };

#endif /* __INC_MSVGPRESERVEASPECTRATIO__ */
