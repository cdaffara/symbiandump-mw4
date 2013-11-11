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


#ifndef _INC_CSVGANIMATETRANSFORMELEMENTIMPL_
#define _INC_CSVGANIMATETRANSFORMELEMENTIMPL_

#include "SVGAnimationBase.h"




/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgAnimateTransformElementImpl : public CSvgAnimationBase
    {
    public:

        // Constructor/deconstructor

		void  SetMatrixDataSize(TUint8 aValue);
		 		TBool DoAnimProcL(MSvgEvent* aEvent);
		void SetAccumMatrix();
		CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
		void SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle);
		void Reset(MSvgEvent* aEvent);
       void SetAccumulateValuesForSetMediaTime();
		void DeactivateAnimation();
		/**

		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateTransformElementImpl* NewL(  const TUint8 aElemID,
													  CSvgDocumentImpl* aDoc );


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		static CSvgAnimateTransformElementImpl* NewLC(  const TUint8 aElemID,
													   CSvgDocumentImpl* aDoc );

		// Dom Reuse
		 void		 					ReInitializeAnimation();

		 void                          SetOriginalValues_DOMReuse();


	protected:


		/**
		 * Need method description
		 *
		 * @since 1.0
		 * @param
		 * @return
		 */
		virtual void                            ConstructL(  const TUint8 aElemID);


	public:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                                 ~CSvgAnimateTransformElementImpl();

    protected:



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                                CSvgAnimateTransformElementImpl( CSvgDocumentImpl* aDoc );


    public:

        
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt SetAttributeIntL( const TInt aNameId, const TInt32 aValue);
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

		TInt GetAttributeIntL( const TInt aNameId, TInt32& aValue );
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt SetAttributeFloatL(const TInt aNameId, TFloatFixPt aValue );
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		TInt GetAttributeFloat(const TInt aNameId, TFloatFixPt& aValue );
        // From MXmlElement API
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                            SetAttributeL( const TDesC& aName,
                                                               const TDesC& aValue );

        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                                   ReceiveEventL( MSvgEvent* aEvent );
        // From MEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                                    ResetReferenceElementL();

		 /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                    CloneL(MXmlElement* aParentElement);

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgAnimateTransformElementImpl* aDestElement );

    public:
        class TMatrixData
            {
            public:
                TFloatFixPt              iData[3];
            };

		TBool      SetMatrixData( TInt aIndex, TMatrixData& aMatrix);
		void       SetTransformValues(RArray<TMatrixData>*&	aValues);
		// for DOM reusage
		
		void       SetTransValues_DOMReuse();
		
		void Print( TBool aIsEncodeOn );
		
    protected:
        TGfxAffineTransform iFillMatrix;
        RArray<TMatrixData>*iTransformValues;
		RArray<TMatrixData>* iOrgTransformValues;
        TUint8              iMatrixDataSize;
        TGfxAffineTransform iEndMatrix;
		TMatrixData			iAccumMatrixData;

		// DOM_Reuse
		TBool               iMultipleRendering; 


    protected:

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                DesToMatrixData( const TDesC& aDes,
                                             TMatrixData& aMatrixData );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        InitAnimationL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        SetAccumulateValues();

    protected:
        // From CSvgAnimationBase

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        virtual TBool                   AnimProcL( MSvgTimerEvent* aEvent );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        SetToOriginalL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        SetFillValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        ResetAnimationL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        SetToInitialValueL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void        SetToEndValueL();
    };


#endif /* _INC_CSvgAnimateTransformElementImpl_ */
