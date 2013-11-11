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


#ifndef _INC_CSVGELEMENTIMPL_
#define _INC_CSVGELEMENTIMPL_



#include "GfxAffineTransform.h"
#include "SVGColor.h"

#include "SVGTransformableImpl.h"
#include "SVGTestsImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGUriReferenceImpl.h"
#include "SVGEventReceiver.h"
#include "SVGEvent.h"

#define SVG_FONTS_INCLUDE // Macro to include SVG FONTS Functionality

class   CGfx2dGc;
class   CSvgTransformListImpl;
class   MSvgTransformList;

#include "SVGXmlElementImpl.h"
#include "SVGTransformable.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGXmlElementOpt.h"
#include "SVGUriReference.h"

#include "GfxFloatFixPt.h"
#include "GfxAffineTransform.h"
#include "SVGSchemaData.h"

class CCssValue;


const float KGradiansPerDegree = 3.1415926f / 180.0f;

/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */



class CSvgElementImpl : public CXmlElementImpl,
                        public MSvgTransformable,
                        public MSvgTests,
                        public MXmlElementOpt,
                        public MSvgLangSpace,
                        public MSvgUriReference,
                        public MSvgEventReceiver

    {
    public:


        virtual TBool  DoAnimProcL(MSvgEvent* aEvent);
        void PointToParent(const TInt aAttrId);
        TBool                       IsInherited(const TInt aAttrId);

        TBool                       HasAnimation();

       TInt32 AnimateTransformMatrixIndexL();

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               DrawL( CGfx2dGc* agc,
                                           CSvgElementImpl* aElement );
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
		virtual	TBool StyleInParent(CSvgElementImpl* aElement, TInt aAttrIndex);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                DrawShapeL( CGfx2dGc* aGc,
                                                MGfxShape& aShape,
                                                CSvgElementImpl* aElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                DrawShapeL( CGfx2dGc* aGc,
                                                CSvgElementImpl* aElement );
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        PropagateToChildren(CCssValue* aOldvalue,CCssValue* aNewValue, const TInt aNameId);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC*        Id();


        virtual CGfxGeneralPath* GetPathAttribute(TInt aAttributeId);
        virtual void SetPathAttribute(TInt aSvgAttrId, CGfxGeneralPath* aPathHandle);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetIdandXmlbaseL( const TDesC& aPropertyName,
                                                     const TDesC& aValue );


        // need to move to CXmlElementImpl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual MXmlElement*        AppendChildL( MXmlElement* aElement, TBool aIsJSR226Element = EFalse );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
            virtual TInt                SetAttributeL(  const TDesC&  aName ,
                                                        const TDesC&  aValue  );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        virtual TInt                RemoveAttribute( const TInt aAttrId );



        // From MXmlElementOpt API

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                GetAttributeIntL( const TInt aNameId,
                                                     TInt32& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetAttributeIntL( const TInt aNameId,
                                                      const TInt32 aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                GetAttributeFloat( const TInt aNameId,
                                                       TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                GetAttributePath( const TInt aNameId,
                                                      CGfxGeneralPath*& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetAttributePathL( const TInt aNameId,
                                                       CGfxGeneralPath* aValue );

        //used in decoder to avoid local copying.
         virtual TInt                SetAttributePathRef( const TInt aNameId,
                                                       CGfxGeneralPath*& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetAttributeDesL( const TInt aNameId,
                                                      const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        void                        GetSvgStylePropertiesArray( RPointerArray<CCssValue>*& aSvgStyleProperties);

        // From MSvgLocatable/MSvgTransformable

         /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         TBool                IsTransformable();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetTransform( MSvgTransformList*& aTransformList );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                SetTransform( MSvgTransformList*& aTransformList );

        virtual void                SetTransformList(TGfxAffineTransform& aTr);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     const TGfxAffineTransform& GetCTM();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                SetCTM( TGfxAffineTransform& aTr );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetBBox( TGfxRectangle2D& /*aBbox*/ );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MSvgStylable


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */

        void                                SetPresentationAttribute( const TInt8 aPropertyId,
                                                CCssValue*& aCssValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
         TBool                      SetPropertyL( const TInt& aAttrId,
                                    const TDesC& aValue);
        /**
         * This method sets a property value that takes a float
         *
         * @since 1.0
         * @param const TInt& (attribute ID), TReal32 (float value)
         * @return TBool
         */
         TBool                        SetPropertyFloatL( const TInt& aAttrId,
                                                TReal32 aValue);
        // From MSvgTests


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetRequiredFeatures( const CDesCArrayFlat*& aRequiredFeatures );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetRequiredExtensions( const CDesCArrayFlat*& aRequiredExtensions );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool                GetSystemLanguage( const CDesCArrayFlat*& aSystemLanguage );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               HasExtension( const TDesC& aExtension );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               HasFeature( const TDesC& aFeature );

        virtual TBool               HasAnyTests();

        // From MSvgLangSpace.h

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&        XMLLang();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetXMLLangL( const TDesC& aXMLLang );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&        XMLSpace();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt                SetXMLSpaceL( const TDesC& aXMLSpace );

        // TKC: Shouldn't this be a wrapper to CSVGTransformableImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               SetTransform( const TDesC& aAttributeName,
                                                  const TDesC& aValue );

        // TKC Wrapper to CSvgTestsImpl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               SetTestAttributeL( const TDesC& aPropertyName,
                                                       const TDesC& aValue );

        // SSB Wrapper to CSvgLangSpace

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               SetLangSpaceAttributeL( const TDesC& aPropertyName,
                                                            const TDesC& aValue );

        // SSB Wrapper to CSvgUriReferenceImpl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               SetXlinkAttributeL( const TDesC& aPropertyName,
                                                        const TDesC& aValue );
        // From MSvgLangSpace.h

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&        Href();
        
        virtual TDesC*				HrefPointer();
        
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual const TDesC&        XlinkShow();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                     ~CSvgElementImpl();

        // From MSvgEventReceiver

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
               
        virtual TBool               ReceiveEventL( MSvgEvent* aEvent);
     
        /**
         * Receives events and generates suitable internal events
         * for engine based on aSubEventMask
         * 
         * @since v3.2
         * @param aEvent - The event of type TSvgEngineEventType
         * @param aSubEventMask - contains the list of interactive events 
         *                        that the element can respond to.
         * @return ETrue if the aEvent is of type TSvgEngineEventType and
         *               event processing needs redraw else EFalse.    
         */                        
          
        virtual TBool               ReceiveEventL( MSvgEvent* aEvent, const TUint16 aSubEventMask );
     
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                ResetReferenceElementL();

        // Internal methods

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TSvgColor*           CurrentColor();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                SetGCL( CGfx2dGc* aGc);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TInt32              AnimateMotionMatrixIndexL();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */


        void                        FindProperty( const TInt8 aPropertyId,
                                        CCssValue*& aCssValue,
                                        CSvgElementImpl* aElement );

        void                        FindProperty( const TInt8 aPropertyId,
                                        CCssValue*& aCssValue);
        void                        CreateSvgTestL(const TUint16 aAttrName);

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        CloneSvgStylePropertiesL( CSvgElementImpl* aNewChild,
                                        CSvgElementImpl* aParentElement);

        TInt                       SetUriRefDesL( const TUint16 aNameId,
                                        const TDesC& aValue );

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue is element is viewable.
         */
        virtual TBool               IsViewable() { return EFalse; }

        TBool IsVisible();

		TFloatFixPt GetCurrentScale( );

       virtual void UpdateCTM();
        
        
    protected:
        // Internal methods


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                InitializeL(  TUint8 aElemID );



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                    CSvgElementImpl();



        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual TBool               IsDrawable();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */


        TInt                        InitSvgStyleProperties();


    public:

		void 					InitAllSvgStyleProperties(CSvgElementImpl* aElement);
		void 					InitSvgJSRStyleProperties(CSvgElementImpl* aParentElement);
        void					PrintStyleProperties();
        virtual void			Print( TBool aIsEncodeOn ) = 0;

        TBool						GetDefaultAttributeL( const TInt aNameId, CCssValue*& aValue );

            // functions for encoding

        virtual void                    SetRequiredFeatures(  CDesCArrayFlat*& aRequiredFeatures );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetRequiredExtensions(  CDesCArrayFlat*& aRequiredExtensions );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    SetSystemLanguage(  CDesCArrayFlat*& aSystemLanguage );

        /**
         * Initialize all style property values with NULL.
         *
         * @since 1.0
         * @return none
         */
        void InitSvgStylePropertiesWithNullL();

        virtual void                DeactivateAnimation();

        MXmlElement*                CloneL(MXmlElement* aParentElement);

        void                        ResetReferenceAttrSet();

        void                        Reset(MSvgEvent* aEvent);

        virtual void                ReInitializeAnimation();

        void                        SetIsAnimating( TBool aBool );
        TBool                       IsAnimating();

        void                        SetTurnOff( TBool aBool );
        TBool                       WasTurnedOff();

        void                        SetOverwriteTransforms( TBool aBool );
        TBool                       IsOverwriteTransforms();

        void                        SetAllAttributesAdded( TBool aBool );
        TBool                       AreAllAttributesAdded();

        void                        SetMouseDown( TBool aBool );
        TBool                       IsMouseDown();

        void                        SetMouseIn( TBool aBool );
        TBool                       IsMouseIn();

		TBool						IsAnimatedElement( );
		
	    /**
		 * Check whether the internal event can be generated
		 *
		 * @since v3.2
		 * @param aSubEventMask - The bit field that contains the list of events
		 *                        registered for the element 
		 * @param aEvent - The internal event to be generated
		 * @return ETrue if the element is registered for the aEvent otherwise EFalse.
		 */                                                
		TBool                     CanGenerateInternalEvent(TUint16 aSubEventmask, TSvgEvent aEvent);
		
		TBool                     IsSVGEnginePaused();
    protected:
        void                        CopyL( CSvgElementImpl* aDestElement );

    public:

        // Additive
        RArray<TUint32>*            iAnimateAttrSet;

        TInt                        iAnimateTransformMatrixIndex;

        TInt                        iHasAnimationBase;

    protected:
        // Internal variables

        HBufC*                      iId;

        RPointerArray<CCssValue>*   iSvgStyleProperties;
        CSvgTransformableImpl*      iSvgTransformable;

        CSvgTestsImpl*              iSvgTests;

        CSvgLangSpaceImpl*          iSvgLangSpace;

        CSvgUriReferenceImpl*       iSvgUriReferenceImpl;

    private:
        // Enumeration for boolean bits:  boolean variables use bitfield to reduce memory usage
        enum TElementBooleanBits
        {
            KAnimatingBit           = 0x00000001,
            KWasTurnedOffBit        = 0x00000002,
            KOverwriteTransformsBit = 0x00000004,
            KAllAttributesAddedBit  = 0x00000008,
            KMouseDownBit           = 0x00000010,
            KMouseInBit             = 0x00000020

        };

        // variable to track TBool members with bitwise operations
        TUint32                     iBooleanFlags;

    public:
        TInt16                      iAnimateMotionMatrixIndex;

        TUint16                     iReqAttrFlag;






    };

#endif /* _INC_CSVGELEMENTIMPL_ */
