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


#ifndef __INC_MSVGTRANSFORMLIST__
#define __INC_MSVGTRANSFORMLIST__

class TGfxAffineTransform;

// This interface was implemented instead of using RPointerArray directly,
// because it also includes Consolidate and CreateTransformFromMatrix
// methods, which is not in RPointerArray.
// Note: not all methods of interface implemented.


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class MSvgTransformList
    {
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TBool               AppendItemL( TGfxAffineTransform& aNewItem ) = 0;

        // Not in SVG DOM, temporary only. Should be fixed when SVGImpl uses CTransformList
        // instead of MTransformList internally.

        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TBool               AppendItemL( TGfxAffineTransform& aTransform,
                                                 TInt32 aAdditive,
                                                 TInt32 animMotion ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void                SetAdditive( TInt32 additive, TInt32 index ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual void                Clear() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TGfxAffineTransform Consolidate(TBool aOverwriteTrans) = 0;

         /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TGfxAffineTransform GetItem( TInt index ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TInt                NumberOfItems() = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TGfxAffineTransform InitializeL( TGfxAffineTransform& newItem ) = 0;


        /**
         * Need method description
         *
         * @since 1.0
         * @param 
         * @return
         */
        virtual TGfxAffineTransform ReplaceItem( TGfxAffineTransform newItem,
                                                 int index ) = 0;
    };

#endif /* __INC_MSVGTRANSFORMLIST__ */
