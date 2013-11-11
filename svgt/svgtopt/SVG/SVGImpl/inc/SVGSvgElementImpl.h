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


#ifndef __INC_CSVGSVGELEMENT__
#define __INC_CSVGSVGELEMENT__

#include "SVGElementImpl.h"
#include "GfxRectangle2D.h"
#include "GfxAffineTransform.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGGElementImpl.h"
#include "SVGZoomAndPan.h"
#include "SVGTimedEntityInterface.h"


class   CSvgViewSpec;
class   CSvgTestAttrs;
class   CSvgExternalResourcesRequiredAttrs;
class   CSvgPresentationAttrs;
class   CSvgAngle;
class   CSvgMatrix;
class   CSvgDocumentImpl;
class   TGfxAffineTransform;
class   CSvgElementImpl;
class   MXmlDocument;


/**
 * Class description
 */
class CSvgSvgElementImpl : public CSvgElementImpl, public MSvgZoomAndPan
    {
    public:

		CSvgSvgElementImpl();
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSvgElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgSvgElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

	private:
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL(  const TUint8 aElemID  );


	public:


        /**
         * Destructor
         */
        virtual                     ~CSvgSvgElementImpl();


        // From SVG DOM

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      X();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Y();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Width();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                      Height();

        /**
         * Accessor funciton for SyncBehaviorDefault value
         *
         * @since S60 3.2
         * @param none
         * @return TSvgSyncBehavior indicating default value.
         */
        TSvgSyncBehaviour                SyncBehaviorDefault();

        /**
         * Accessor funciton for SyncToleranceDefault value
         *
         * @since S60 3.2
         * @param none
         * @return TUint32 indicating default value in msecs.
         */
        TUint32        SyncToleranceDefault();

        
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                GetElementById( const TDesC& aElementId );
        
		TInt SetAttributeDesL( const TInt aNameId, TDesC& aValue );
		
		TInt GetAttributeDes( const TInt aNameId, TPtrC16& aValue );


        // SVG Implementation

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetWidth( TFloatFixPt aWidth );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetHeight( TFloatFixPt aHeight );




        // From MXmlElement API


        /**
         * get attribute of CSvgSvgElementImpl
         */
        IMPORT_C TInt               GetAttribute( const TDesC& aName,
                                                  TDes& aValue );


        /**
         * set attribute
         */
        TInt                        SetAttributeL( const TDesC& aName,
                                                   const TDesC& aValue );

        // From MXmlElementOpt API

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        GetAttributeFloat( const TInt aNameId,
                                                       TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                        SetAttributeFloatL( const TInt aNameId,
                                                        const TFloatFixPt aValue );


        // From CSvgElementImpl

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*        CloneL(MXmlElement* aParentElement);


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       DrawL( CGfx2dGc* aGc,
                                           CSvgElementImpl* aElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MSvgZoomAndPan


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TSvgZoomAndPanType          GetZoomAndPan();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetZoomAndPan( TSvgZoomAndPanType aZoomAndPan );

        // From MSvgFitToViewBox


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        GetPreserveAspectRatio( MSvgPreserveAspectRatio*& aAspectRatio );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetPreserveAspectRatioL ( TSvgPreserveAspectAlignType aAlign,
                                                                 TSvgMeetOrSliceType aMeetslice );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       GetViewBox( TGfxRectangle2D& aViewBox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                       SetViewBoxL( TGfxRectangle2D aViewBox );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                       ViewBoxDefined();

        // From CSvgFitToViewBoxImpl


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                        SetWindowViewportTrans( TGfxRectangle2D aXYWH,
                                                                                TGfxAffineTransform& aTr );


	
        // For Baseprofile

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&                BaseProfile();
        // For Version

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        const TDesC&                Version();



         /**
         * Constructor
         */
                                    CSvgSvgElementImpl( CSvgDocumentImpl* aDoc );

		  /**
         * set base profile
         */
        void                        SetBaseProfileL( const TDesC& aValue );


        /**
         * set version
         */
        void                        SetVersion( const TDesC& aValue );

        /**
         * set synchronised behaviour default
         * @since S60 3.2
         * @param aValue canSlip | independent | locked | inherit
         * @return
         */
        void                        SetSyncBehaviorDefault( const 
            TDesC& aValue );
        
        /**
        * set synchronised tolerance default
        * @since S60 3.2
        * @param aValue Clock-value | "inherit"
        * @return none.
        */
        void                        SetSyncToleranceDefault( const 
            TDesC& aValue );

        /**
         * set synchronised behaviour default
         * @since S60 3.2
         * @param aValue canSlip | independent | locked | inherit
         * @return
         */
        void                        SetSyncBehaviorDefault( const 
            TSvgSyncBehaviour aValue );
        
        /**
        * set synchronised tolerance default
        * @since S60 3.2
        * @param aValue Clock-value 
        *        aInheritValue true/false, should tolerance default 
        * @return none.
        */
        void                        SetSyncToleranceDefault( const 
            TInt32 aValue, TBool aInheritValue = EFalse );

		void 						Print( TBool aIsEncodeOn );

   
      
    private:
        TSvgZoomAndPanType          iZoomAndPan;

        TFloatFixPt                      iX;
        TFloatFixPt                      iY;
        TFloatFixPt                      iWidth;
        TFloatFixPt                      iHeight;

        CSvgFitToViewBoxImpl*       iViewBoxImpl;

        HBufC*                      iVersion;

        HBufC*                      iBaseProfile;
        
        // Default run time sync behaviour 
        TSvgSyncBehaviour           iSyncBehaviorDefault;
        
        // Default synchronised tolerance value in msecs
        TUint32                     iSyncToleranceDefault;
        
	public:
		// to support "%" for svg element in case of multiple
		// render buffers of different sizes
		TReal32						iWidthInUserCoordinate;
		TReal32						iHeightInUserCoordinate;
		TBool                       iWidthInPercentage;
		TBool                       iHeightInPercentage;
		
		TBool                        iIsWidthSet;
		TBool                        iIsHeightSet;
		HBufC*                     iContent;
    };


#endif /* __INC_CSVGSVGELEMENT__ */
