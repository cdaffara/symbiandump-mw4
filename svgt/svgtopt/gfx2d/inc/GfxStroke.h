/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library header file
*
*/



#ifndef GFXSTROKE_H
#define GFXSTROKE_H

#include <e32std.h>
#include <e32base.h>
#include <fbs.h>
#include "GfxFloatFixPt.h"



class CGfxGeneralPath;
class MGfxShape;
class CGfx2dGc;


/**
 *  Implementation of a Stroke.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxStroke
    {
    public:

        /**
         * Constructor.  Default values for width, join, cap and miter-limit.
         *
         * @since 1.0
         * @return
         */
            TGfxStroke();

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aStrokeWidth : width of stroke.
         * @return
         */
            TGfxStroke( TFloatFixPt aStrokeWidth );

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aStrokeWidth : width of stroke.
         * @param aJoin : join type
         * @param aCap : cap type
         * @param aMiterLimit : miter-limit value.
         * @return
         */
            TGfxStroke( TFloatFixPt aStrokeWidth,
                                TGfxJoinType aJoin,
                                TGfxCapType aCap,
                                TFloatFixPt& aMiterLimit );

			void SetMiterLimit( TFloatFixPt aMiterLimit);
			void SetJoinType( TGfxJoinType aJoin );
			void SetCapType( TGfxCapType aCap );

        /**
         * Set the stroke width
         *
         * @since 1.0
         * @param aWidth : width of stroke.
         * @return
         */
            void SetStrokeWidth( TFloatFixPt aWidth );

        /**
         * Get the stroke width
         *
         * @since 1.0
         * @return stroke width
         */
            TFloatFixPt StrokeWidth();
			TGfxJoinType StrokeJoin();
			TGfxCapType StrokeCap();
			TFloatFixPt StrokeMiterLimit();

    protected:

        /**
         * Get the stroke width, join type, cap type and miter-limit.
         *
         * @since 1.0
         * @param aStrokeWidth : width of stroke.
         * @param aJoin : join type
         * @param aCap : cap type
         * @param aMiterLimit : miter-limit value.
         * @return
         */
        void            SetVars( TFloatFixPt aStrokeWidth,
                                 TGfxJoinType aJoin,
                                 TGfxCapType aCap,
                                 const TFloatFixPt& aMiterLimit );

    protected:
        TFloatFixPt          iStrokeWidth;
        TGfxJoinType    iJoin;
        TGfxCapType     iCap;
        TFloatFixPt          iMiterLimit;


    private:
        friend          class CGfx2dGc;

    };


#endif      // GFXSTROKE_H
