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


#ifndef __INC_CSVGTRANSFORMLISTIMPL__
#define __INC_CSVGTRANSFORMLISTIMPL__


#include "SVGTransformList.h"

class TGfxAffineTransform;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTransformListImpl : public CBase, public MSvgTransformList
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
        static CSvgTransformListImpl*           NewL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgTransformListImpl*           NewLC();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                                 ~CSvgTransformListImpl();

        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                                   AppendItemL( TGfxAffineTransform& aNewItem );

         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                                   AppendItemL( TGfxAffineTransform& aNewItem,
                                                             TInt32 aAdditive,
                                                             TInt32 aSourceAnimMotion );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                    Clear();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TGfxAffineTransform                     Consolidate(TBool aOverwrite);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TGfxAffineTransform                     GetItem( TInt index );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                                    NumberOfItems();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TGfxAffineTransform                     InitializeL( TGfxAffineTransform& newItem );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TGfxAffineTransform                     ReplaceItem( TGfxAffineTransform newItem,
                                                             int index );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                    SetAdditive( TInt32 additive,
                                                             TInt32 index );

    private:

        RArray<TGfxAffineTransform>             iTransformations;

        // -1 do nothing 1 ETrue: Additive. 0 EFalse: Replace
        RArray<TInt32>                          iTransformAdditive;


        // 1 animmotion, 2 animtransf, 0 else ETrue: Set by animMotion, EFalse: other (e.g. animTransform)
        RArray<TInt32>                          iTransformTypes;

        TBool                                   iAvoidConsolidation;
        TGfxAffineTransform                     iConsolidatedMatrix;

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
                                                CSvgTransformListImpl();

    public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        CSvgTransformListImpl*                  CloneL();

		void									SetAvoidConsolidation(TBool aAvoidConsolidation);
    };

#endif /* __INC_CSVGTRANSFORMLISTIMPL__ */
