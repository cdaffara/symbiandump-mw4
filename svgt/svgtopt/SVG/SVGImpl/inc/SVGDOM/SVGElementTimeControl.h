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


#ifndef _INC_MELEMENTTIMECONTROL_
#define _INC_MELEMENTTIMECONTROL_


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MElementTimeControl
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool   BeginElementL() = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool   BeginElementAtL( TFloatFixPt aOffset ) = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool   EndElement() = 0;

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool   EndElementAt( TFloatFixPt aOffset ) = 0;
    };

#endif /* _INC_MElementTimeControl_ */
