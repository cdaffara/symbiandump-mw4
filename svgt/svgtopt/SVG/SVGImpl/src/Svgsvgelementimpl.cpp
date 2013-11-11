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
* Description:  SVG Implementation source file
 *
*/


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGSvgElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"
#include "SVGSchemaData.h"

#include "SVGClrCssValueImpl.h"
#include "SVGPaintCssValueImpl.h"
#include "SVGIntCssValueImpl.h"
#include "SVGFloatCssValueImpl.h"
#include "SVGStrCssValueImpl.h"
#include "SVGVectorCssValueImpl.h"
#include "SVGPathElementImpl.h"
#include "SVGAnimTimingParser.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSvgElementImpl* CSvgSvgElementImpl::NewL(  const TUint8 aElemID,
                                              CSvgDocumentImpl* aDoc )
    {
    CSvgSvgElementImpl* self    = new ( ELeave ) CSvgSvgElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSvgElementImpl* CSvgSvgElementImpl::NewLC(  const TUint8 aElemID,
                                               CSvgDocumentImpl* aDoc )
    {
    CSvgSvgElementImpl* self    = new ( ELeave ) CSvgSvgElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::ConstructL( const TUint8 aElemID )
    {

    CSvgElementImpl::InitializeL( aElemID );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);

	CSvgDocumentImpl* lDocument = (CSvgDocumentImpl*)OwnerDocument();

	if (!lDocument)
	return;

    // This is required to allocate buffer up to granularity so that the
    // following Insert calls cannot leave.

    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );

    iSvgTransformable = CSvgTransformableImpl::NewL();

    // Initilaise Svg Element with default values

    CPaintCssValueImpl* lPaintObject = lDocument->iMemoryManager->GetCssPaintObjectL( _L("black"),(CSvgElementImpl *)this );
    iSvgStyleProperties->InsertL(lPaintObject,KCSS_ATTR_FILL);

	CClrCssValueImpl* lClrObject = lDocument->iMemoryManager->GetCssClrObjectL(_L("none"));
    iSvgStyleProperties->InsertL(lClrObject,KCSS_ATTR_STROKE);

	CFloatCssValueImpl* lFloatObject = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));
    iSvgStyleProperties->InsertL(lFloatObject,KCSS_ATTR_STROKEWIDTH);

    //set default to visible
    CIntCssValueImpl* lIntObject = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lIntObject,KCSS_ATTR_VISIBILITY);

    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_FONTFAMILY);

	CFloatCssValueImpl* lFloatObject2 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("10"));
    iSvgStyleProperties->InsertL(lFloatObject2,KCSS_ATTR_FONTSIZE);

	CIntCssValueImpl* lIntObject2 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lIntObject2,KCSS_ATTR_FONTSTYLE);

	CIntCssValueImpl* lIntObject3 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lIntObject3,KCSS_ATTR_FONTWEIGHT);

    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_STROKE_DASHARRAY);

    //set default to inline
    CIntCssValueImpl* lIntObject4 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lIntObject4,KCSS_ATTR_DISPLAY);

	CStrCssValueImpl* lStrObject1 = lDocument->iMemoryManager->GetCssStrObjectL(_L("nonzero"));
    iSvgStyleProperties->InsertL(lStrObject1,KCSS_ATTR_FILLRULE);

	CStrCssValueImpl* lStrObject2 = lDocument->iMemoryManager->GetCssStrObjectL(_L("butt"));
    iSvgStyleProperties->InsertL(lStrObject2,KCSS_ATTR_STROKE_LINECAP);

	CStrCssValueImpl* lStrObject3 = lDocument->iMemoryManager->GetCssStrObjectL(_L("miter"));
    iSvgStyleProperties->InsertL(lStrObject3,KCSS_ATTR_STROKE_LINEJOIN);

	CFloatCssValueImpl* lFloatObject3 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lFloatObject3,KCSS_ATTR_STROKE_DASHOFFSET);

	CFloatCssValueImpl* lFloatObject4 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("4"));
    iSvgStyleProperties->InsertL(lFloatObject4,KCSS_ATTR_STROKE_MITERLIMIT);

	CClrCssValueImpl* lClrObject2 = lDocument->iMemoryManager->GetCssClrObjectL(_L("black"));
    iSvgStyleProperties->InsertL(lClrObject2,KCSS_ATTR_COLOR);

	CIntCssValueImpl* lIntObject5 = lDocument->iMemoryManager->GetCssIntObjectL(_L("0"));
    iSvgStyleProperties->InsertL(lIntObject5,KCSS_ATTR_TEXTANCHOR);

	CIntCssValueImpl* lIntObject6 = lDocument->iMemoryManager->GetCssIntObjectL(_L("-1"));
    iSvgStyleProperties->InsertL(lIntObject6,KCSS_ATTR_TEXTDECORATION);

    //default value is indeterminate at this point
    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_COLORINTERPOLATION);

    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_COLORRENDERING);

    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_LETTERSPACING);

    iSvgStyleProperties->InsertL(NULL,KCSS_ATTR_WORDSPACING);

	CFloatCssValueImpl* lFloatObject5 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));
    iSvgStyleProperties->InsertL(lFloatObject5,KCSS_ATTR_FILL_OPACITY);

	CFloatCssValueImpl* lFloatObject6 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));
    iSvgStyleProperties->InsertL(lFloatObject6,KCSS_ATTR_STROKE_OPACITY);

	CFloatCssValueImpl* lFloatObject7 = lDocument->iMemoryManager->GetCssFloatObjectL(_L("1"));
    iSvgStyleProperties->InsertL(lFloatObject7,KCSS_ATTR_GROUP_OPACITY);

    iViewBoxImpl = CSvgFitToViewBoxImpl::NewL();

    iBaseProfile = HBufC::NewL( 0 );
    iVersion = HBufC::NewL( 0 );
    SetBaseProfileL(_L("tiny"));
    SetVersion(_L("1.1"));

	iWidthInUserCoordinate = 100;
	iHeightInUserCoordinate= 100;

    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgSvgElementImpl::~CSvgSvgElementImpl()
    {

    if (iContent)
    {
	delete iContent;
	iContent = NULL;
    }

    if ( iViewBoxImpl )
        {
        delete iViewBoxImpl;
        iViewBoxImpl = NULL;
        }

    if ( iBaseProfile )
        {
        delete iBaseProfile;
        iBaseProfile = NULL;
        }
    if ( iVersion )
    	{
    	delete iVersion;
    	iVersion = NULL;
        }

    if ( iSvgStyleProperties )
        {
        iSvgStyleProperties->Close();
        delete iSvgStyleProperties;
        iSvgStyleProperties = NULL;
        }

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgSvgElementImpl::CSvgSvgElementImpl( CSvgDocumentImpl* aDoc ) : iZoomAndPan( ESvgZoomAndPanMagnify ),
                                                                   iWidth(100),
                                                                   iHeight(100),
                                                                   iWidthInPercentage(ETrue),
                                                                   iHeightInPercentage(ETrue),
                                                                   iIsWidthSet(EFalse),
                                                                   iIsHeightSet(EFalse)
    {
    SetOwnerDocument(aDoc);

    // Obtain the default sync behaviour/tolerance from the document
    if ( aDoc )
        {
        iSyncBehaviorDefault = aDoc->SyncBehaviorDefault();
        iSyncToleranceDefault = aDoc->SyncToleranceDefault();
        }    
    
    }



// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgSvgElementImpl::X()
    {
    return iX;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgSvgElementImpl::Y()
    {
    return iY;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgSvgElementImpl::Width()
    {
    return iWidth;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgSvgElementImpl::Height()
    {
    return iHeight;
    }

// ---------------------------------------------------------------------------
// Accessor funciton for SyncBehaviorDefault value
// ---------------------------------------------------------------------------
TSvgSyncBehaviour CSvgSvgElementImpl::SyncBehaviorDefault()
    {
    return iSyncBehaviorDefault;
    }

// ---------------------------------------------------------------------------
// Accessor funciton for SyncBehaviorDefault value
// ---------------------------------------------------------------------------
TUint32 CSvgSvgElementImpl::SyncToleranceDefault()
    {
    return iSyncToleranceDefault;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgSvgElementImpl::GetElementById( const TDesC& /* aElementId */ )
    {
    return ( CSvgElementImpl * ) NULL;
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetWidth( TFloatFixPt aWidth )
    {
    iWidth = aWidth;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetHeight( TFloatFixPt aHeight )
    {
    iHeight = aHeight;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
EXPORT_C TInt CSvgSvgElementImpl::GetAttribute( const TDesC& aName,
                                                TDes& aValue )
    {
    _LIT( KTmpWidth, "width" );
    _LIT( KTmpHeight, "height" );

    if ( aName == KTmpWidth )
        {
        TFloatFixPt  width;
        GetAttributeFloat( KAtrWidth, width );
	#ifdef SVG_FLOAT_BUILD
        TFloatFixPt::GetString( width, aValue );
	#else
        width.GetString( aValue );
	#endif
        }
    else if ( aName == KTmpHeight )
        {
        TFloatFixPt  height;
        GetAttributeFloat( KAtrHeight, height );
	#ifdef SVG_FLOAT_BUILD
        TFloatFixPt::GetString( height, aValue );
	#else
        height.GetString( aValue );
	#endif
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSvgElementImpl::SetAttributeL( const TDesC& aName,
                                        const TDesC& aValue )
    {


    CSvgElementImpl::SetAttributeL(aName,aValue);

    // Encapsulates both viewBox and preserveAspectRatio attributes.
    if ( iViewBoxImpl->SetViewBoxL( aName, aValue ) )
        {
        return KErrNone;
        }


    _LIT( KTmpBaseProfile, "baseProfile" );
    _LIT( KTmpVersion, "version" );
    _LIT( KTmpZoomAndPan, "zoomAndPan" );
    _LIT( KTmpDisable, "disable" );
    _LIT( KTmpMagnify, "magnify" );
    _LIT( KNameSpace, "xmlns" );
    _LIT( KSvgNameSpace, "http://www.w3.org/2000/svg" );
    _LIT( KTmpSyncBehaviorDefault, "syncBehaviorDefault" );
    _LIT( KTmpSyncToleranceDefault,"syncToleranceDefault" );


    TLex    input   ( aValue );

    if ( aName == KTmpBaseProfile )
        {
        SetBaseProfileL( aValue );
        }
    else if ( aName == KTmpVersion )
        {
        //if (input.Val( tmpFloat, '.' ) == KErrNone)
        SetVersion( aValue );
        }
    else if ( aName == KTmpZoomAndPan )
        {
        if ( aValue == KTmpDisable )
            iZoomAndPan = ESvgZoomAndPanDisable;
        else if ( aValue == KTmpMagnify )
            iZoomAndPan = ESvgZoomAndPanMagnify;
        }
    else if ( aName == KNameSpace && aValue != KSvgNameSpace )
        {
        if ( iOwnerDocument )
            {
            _LIT( KMsg, "Invalid <svg> namespace: " );
            ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
            }
        }
    else if ( aName == KTmpSyncBehaviorDefault )
        {
        SetSyncBehaviorDefault( aValue );
        }
    else if ( aName == KTmpSyncToleranceDefault )
        {
        SetSyncToleranceDefault( aValue );
        }

    return KErrNone;
    }

// *******************************************************
// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSvgElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
        aValue = X();
        break;
        case KAtrY:
        aValue = Y();
        break;
        case KAtrWidth:
        aValue = Width();
        break;
        case KAtrHeight:
        aValue = Height();
        break;
        default:
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgSvgElementImpl::SetAttributeFloatL( const TInt aNameId,
                                             const TFloatFixPt aValue )
    {
    TGfxRectangle2D lViewBox;
    switch ( aNameId )
        {
        case KAtrX:
        iX = aValue;
        break;
        case KAtrY:
        iY = aValue;
        break;
        case KAtrWidth:

        iWidth = aValue;
        if(!iIsWidthSet) // make sure that this is set only once
	        {
	        iWidthInUserCoordinate = (TReal32)aValue;
	        iIsWidthSet = ETrue;
	        }
        break;
        case KAtrHeight:

        iHeight = aValue;
        if(!iIsHeightSet ) // make sure that this is set only once
	        {
	        iHeightInUserCoordinate = (TReal32)aValue;
	        iIsHeightSet= ETrue;
	        }
        break;

        default:
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgSvgElementImpl::CloneL(MXmlElement* aParentElement)
    {

    CSvgSvgElementImpl* retValue = CSvgSvgElementImpl::NewL(this->ElemID(),  ((CSvgDocumentImpl*)iOwnerDocument));

    CleanupStack::PushL(retValue);
    retValue->iParentNode = aParentElement;

    // *** have base class copy what it needs to
    this->CopyL(retValue);

    // *** copy iViewBoxImpl
    if (this->iViewBoxImpl != NULL)
        {
        if (retValue->iViewBoxImpl != NULL)
        {
			delete retValue->iViewBoxImpl;
			retValue->iViewBoxImpl= NULL;
		}
        retValue->iViewBoxImpl = this->iViewBoxImpl->CloneL();
        }

    // *** copy iBaseProfile
    if (retValue->iBaseProfile != NULL)
        {
        delete retValue->iBaseProfile;
        retValue->iBaseProfile = NULL;
        }
    retValue->iBaseProfile = this->iBaseProfile->AllocL();

    // *** copy others
    retValue->iZoomAndPan = this->iZoomAndPan;
    retValue->iX = this->iX;
    retValue->iY = this->iY;
    retValue->iWidth = this->iWidth;
    retValue->iHeight = this->iHeight;
    retValue->iVersion = this->iVersion->AllocL();
    CleanupStack::Pop();

    return retValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgSvgElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                 CSvgElementImpl* /* aElement */ )
    {
    return ETrue;
    }

// *******************************************************
// From MSvgZoomAndPan

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TSvgZoomAndPanType CSvgSvgElementImpl::GetZoomAndPan()
    {
    return iZoomAndPan;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetZoomAndPan( TSvgZoomAndPanType aZoomAndPan )
    {
    iZoomAndPan = aZoomAndPan;
    }

// *******************************************************
// From MSvgFitToWindow

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::GetPreserveAspectRatio( MSvgPreserveAspectRatio*& aAspectRatio )
    {
    iViewBoxImpl->GetPreserveAspectRatio( aAspectRatio );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetPreserveAspectRatioL( TSvgPreserveAspectAlignType aAlign, TSvgMeetOrSliceType aMeetslice )
    {
    iViewBoxImpl->SetPreserveAspectRatioL( aAlign, aMeetslice);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgSvgElementImpl::GetViewBox( TGfxRectangle2D& aViewBox )
    {
    return iViewBoxImpl->GetViewBox( aViewBox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetViewBoxL( TGfxRectangle2D aViewBox )
    {
    iViewBoxImpl->SetViewBoxL(aViewBox);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgSvgElementImpl::ViewBoxDefined()
    {
    return iViewBoxImpl->ViewBoxDefined();
    }

// *******************************************************
// Baseprofile

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgSvgElementImpl::BaseProfile()
    {
    return *iBaseProfile;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetBaseProfileL( const TDesC& aValue )
    {
    if ( iBaseProfile )
        {
        delete iBaseProfile;
        iBaseProfile = NULL;
        }
    iBaseProfile = aValue.AllocL();
    }

// Version

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgSvgElementImpl::Version()
    {
    return *iVersion;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetVersion( const TDesC& aValue )
    {
    if ( iVersion )
        {
        delete iVersion;
        iVersion = NULL;
        }
        TInt error = KErrNone;
        	TRAP(error,iVersion = aValue.AllocL());
    }

// ---------------------------------------------------------------------------
// set synchronised behaviour default
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetSyncBehaviorDefault( const TDesC& aValue )
    {
    _LIT( KTmpCanSlip, "canSlip" );
    _LIT( KTmpLocked, "locked" );
    _LIT( KTmpIndependent, "independent" );
    _LIT( KTmpInherit, "inherit" );
	
	if ( !aValue.CompareF( KTmpCanSlip ) )
	    {
        iSyncBehaviorDefault = ESvgSyncCanSlip;        
	    }
	else if ( !aValue.CompareF( KTmpLocked ) )
	    {
        iSyncBehaviorDefault = ESvgSyncLocked;
	    }
	else if ( !aValue.CompareF( KTmpIndependent ) )
	    {
        iSyncBehaviorDefault = ESvgSyncIndependent;
	    }
	else if ( !aValue.CompareF( KTmpInherit ) )
	    {
        // Obtain the sync behaviour from the document
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        iSyncBehaviorDefault = lSvgDoc->SyncBehaviorDefault();
	    }
    }

// ---------------------------------------------------------------------------
// set synchronised tolerance default
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetSyncToleranceDefault( const TDesC& aValue )
    {
    _LIT( KTmpInherit, "inherit" );
	if ( !aValue.CompareF( KTmpInherit ) )
	    {
	    CSvgAnimTimingParser* lATParserPtr = NULL;
	    // Treat as clock value
	    TRAPD( lCreateATPErr, 
	        {
	        lATParserPtr = CSvgAnimTimingParser::NewL( aValue, this );
	        } );
	    
	    if ( lCreateATPErr != KErrNone )
	        {
	        // Error Processing
	        return;
	        }
        TInt32 lClockValue;
        TLex lLex( aValue );
        lATParserPtr->ParseClockValue( lLex, lClockValue );
        delete lATParserPtr;
        iSyncToleranceDefault = lClockValue; // TODO: Convert Units
	    }
	else
	    {
	    // Get clock value from parent
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        iSyncToleranceDefault = lSvgDoc->SyncToleranceDefault();
	    }
    }

// -----------------------------------------------------------------------------
// CSvgSvgElementImpl::SetSyncBehaviorDefault
// Set the Synchronised behaviour(default) 
// -----------------------------------------------------------------------------
//        
void CSvgSvgElementImpl::SetSyncBehaviorDefault( 
    const TSvgSyncBehaviour aValue )
    {
    if ( aValue == ESvgSyncDefault )
        {
        // Obtain the sync behaviour from the document
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        iSyncBehaviorDefault = lSvgDoc->SyncBehaviorDefault();
        }
     else
        {
        iSyncBehaviorDefault = aValue;
        }
    }   

// -----------------------------------------------------------------------------
// CSvgSvgElementImpl::SetSyncToleranceDefault
// Set the Synchronised Tolerance (Default) 
// -----------------------------------------------------------------------------
//        
void CSvgSvgElementImpl::SetSyncToleranceDefault( const TInt32 aValue , 
    const TBool aUseDefault )
    {
    if ( aUseDefault )
        {
	    // Get clock value from parent
        CSvgDocumentImpl* lSvgDoc = (( CSvgDocumentImpl* ) 
            iOwnerDocument);
        iSyncToleranceDefault = lSvgDoc->SyncToleranceDefault();
        }
        else
            {
            iSyncToleranceDefault = aValue;            
            }
    }
     
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    	if (!HasChildNodes())
    	{
    		return;
    	}

    CSvgGElementImpl::GetGroupBounding( aBbox, this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    	if (!HasChildNodes())
    	{
    		return;
    	}

    CSvgGElementImpl::GetGroupBoundingUnscaled( aBbox, this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgSvgElementImpl::SetWindowViewportTrans( TGfxRectangle2D aXYWH,
                                                 TGfxAffineTransform& aTr )
    {
		//this is a problem for the JSR.  There isn't an engine associated here yet.
    if ( (CSvgDocumentImpl*)iOwnerDocument)
    {
    	TSize lSize;

        TFloatFixPt width, height;
        TGfxRectangle2D viewBox;
        
        // Is engine defined
        TBool engine = EFalse;
        
        // Does the client define the viewPort
        TBool clientDefinedViewport = EFalse;

        TBool viewBoxDefined = iViewBoxImpl->GetViewBox(viewBox);
        if(((CSvgDocumentImpl*)iOwnerDocument)->Engine())
            engine = ETrue;
        
        // if Client like browser or viewer has set the viewport 
        // take the width/height got by using GetViewPort()
        if ( engine )
            {
            clientDefinedViewport = ((CSvgDocumentImpl*)iOwnerDocument )->Engine()->iClientDefinedViewPort;
            }
        
        if( clientDefinedViewport )
            {
            width = Width();
            height = Height();
            }
        else
            {
            if(iWidthInPercentage && engine)
                {
                lSize = ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->GetSize();
                TReal32 per = 100;
                width = ( TFloatFixPt ) (( lSize.iWidth/per ) * iWidthInUserCoordinate);
                }
            else if(iWidthInPercentage && !engine && viewBoxDefined)
                {
                aXYWH.iWidth = viewBox.iWidth;
                aXYWH.iX = viewBox.iX;
                width = viewBox.iWidth;
                }
            else
                {
                width  = Width();
                }
            
            if(iHeightInPercentage && engine)
                {
                lSize = ((CSvgDocumentImpl*)iOwnerDocument)->Engine()->GetSize();
                TReal32 per = 100;
                height = ( TFloatFixPt ) (( lSize.iHeight/per ) * iHeightInUserCoordinate);
                }
            else if( iHeightInPercentage && !engine && viewBoxDefined)
                {
            	aXYWH.iHeight = viewBox.iHeight;
            	aXYWH.iY = viewBox.iY;
            	height = viewBox.iHeight;
                }
            else
                {
                height = Height();
                }
        
            }
           

         TFloatFixPt KZero;
         if ( ( width < KZero ) && ( height < KZero ) )
            {
            width = lSize.iWidth;
            height = lSize.iHeight;
            }
        else if ( width <= KZero )
            {
            width = height;
            }
        else if ( height <= KZero )
            {
            height = width;
            }

            TSize lTmpViewBox(width,height);

			//this should update the CTM based for the new viewbox...hopefully
    	iViewBoxImpl->SetWindowViewportTrans( aXYWH, aTr, lTmpViewBox );
    }
    else
    {
    	#ifdef _DEBUG
    	RDebug::Printf("SvgSvgElement:SetWindowViewportTrans Owner Document Not Found! Maybe JSR226?");
    	#endif
		}
}

//Added as a part of update of Set/Get API's
TInt CSvgSvgElementImpl::GetAttributeDes( const TInt aNameId,
                                                     TPtrC16& aValue )
{
 if(aNameId == KAtrBaseProfile)
	{
	 aValue.Set( BaseProfile() );
	 return KErrNone;
	}
 else if (aNameId == KAtrVersion)
 	{
 	aValue.Set( Version() );
 	return KErrNone;
 	}

 return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
}
TInt CSvgSvgElementImpl::SetAttributeDesL( const TInt aNameId,
                                                     TDesC& aValue )
{
 if(aNameId == KAtrBaseProfile)
	{
	 SetBaseProfileL(aValue);
	 return KErrNone;
	}
 else if (aNameId == KAtrVersion)
 	{
 	SetVersion(aValue);
 	return KErrNone;
 	}
 return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );

}

void CSvgSvgElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<svg x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">", (int)iX, (int)iY, (int)iWidth, (int)iHeight);
		#endif
	}
}

