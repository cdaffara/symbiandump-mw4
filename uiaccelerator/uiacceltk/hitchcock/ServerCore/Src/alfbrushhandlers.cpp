/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   All the built in brush handlers
*
*/



#include "alf/alfbrushhandlers.h"
#include "alf/alfconstants.h"
#include "alf/alfserverutils.h"
#include "alf/alfmetric.h"
#include <uiacceltk/HuiBrush.h>
#include <uiacceltk/HuiBorderBrush.h>
#include <uiacceltk/HuiImageBrush.h>
#include <uiacceltk/HuiDropShadowBrush.h>
#include <uiacceltk/HuiShadowBorderBrush.h>
#include <uiacceltk/HuiGradientBrush.h>
#include <uiacceltk/huiframebrush.h>
#include <uiacceltk/HuiUtil.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrushHandler::CAlfBrushHandler(MAlfInterfaceProvider& aResolver)
 : iResolver( aResolver ), iOwnsBrush(ETrue)
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBrushHandler::ConstructL( CHuiBrush* aBrush )
    {
    if (!aBrush)
        {
        // the child class needs to provide the brush instance.
        USER_INVARIANT();        
        }
    else 
        {
        iBrush = aBrush;    
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBrushHandler::~CAlfBrushHandler()
    {
    if ( iOwnsBrush )
        {
        delete iBrush;
        }
    iBrush = NULL;
    }

// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBrushHandler::Release()
    {
    delete this;
    }
 
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//   
EXPORT_C TAny* CAlfBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface ) 
        {
        case EHuiObjectTypeBrush:
            return iBrush;
        case EAlfBrushHandler:
            return this;
        
        default:
            return NULL;          
        }
    }
   
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfBrushHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    switch (aCommandId)
        {
        case EAlfBrushGetOpacity:
            {
            TAlfTimedValue opacity;
            AlfTimedValueUtility::CopyTimedValue(iBrush->iOpacity, opacity);
            TPckg<TAlfTimedValue> resultPckg(opacity);
            
            aResponse = resultPckg; // awkward   
            break;
            }
        case EAlfBrushSetOpacity:
            {
            TAlfTimedValue* opacity = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*opacity, iBrush->iOpacity, iResolver);            
            break;
            }
            
        case EAlfBrushSetLayer:
            {
            THuiBrushLayer* layer = (THuiBrushLayer*) aInputBuffer.Ptr() ;
            iBrush->SetLayer( *layer );
            break;
            }
        case EAlfBrushClipToVisual:
            {
            TBool clip = iBrush->ClipToVisual();
            TPckg<TBool> resultPckg(clip);            
            aResponse = resultPckg; // awkward   
            break;
            }
        case EAlfBrushSetClipToVisual:
            {
            TBool* clip = (TBool*) aInputBuffer.Ptr() ;
            iBrush->SetClipToVisual(*clip);
            break;
            }
            
        default:
            User::Leave(KErrNotSupported);
        }
    }

// ---------------------------------------------------------------------------
// New method
// ?implementation_description
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfBrushHandler::SetOwnership(TBool aHolderOwnsBrush)
    {
    iOwnsBrush = aHolderOwnsBrush;
    }
 
EXPORT_C void CAlfBrushHandler::BrushHandlerExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    
    }
 
 
 
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrushHandler::CAlfBorderBrushHandler(
    MAlfInterfaceProvider& aResolver)
: CAlfBrushHandler( aResolver ) 
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrushHandler::ConstructL( 
    CHuiBrush* aBrush,
    TAlfXYMetric aThickness, 
    TAlfXYMetric aEdgeOffset)
    {
    if (!aBrush)
        {
        THuiXYMetric huiThickness;
        THuiXYMetric huiEdgeOffset;        
        AlfXYMetricUtility::CopyMetric(aThickness, huiThickness);
        AlfXYMetricUtility::CopyMetric(aEdgeOffset, huiEdgeOffset);
        
        CHuiBorderBrush* borderBrush = CHuiBorderBrush::NewLC( 
            huiThickness, huiEdgeOffset);
        CAlfBrushHandler::ConstructL( borderBrush );
        CleanupStack::Pop( borderBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfBorderBrushHandler::NewL( 
    MAlfInterfaceProvider& aResolver,
    TAlfXYMetric aThickness, 
    TAlfXYMetric aEdgeOffset)
    {
    CAlfBorderBrushHandler* self = new( ELeave ) CAlfBorderBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aThickness,
        aEdgeOffset );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrushHandler::ConstructL( 
    CHuiBrush* aBrush,
    TInt aThicknessWidth, 
    TInt aThicknessHeight,
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY )
    {
    if (!aBrush)
        {
        CHuiBorderBrush* borderBrush = CHuiBorderBrush::NewLC( 
            aThicknessWidth,
            aThicknessHeight,
            aEdgeOffsetX,
            aEdgeOffsetY);
        CAlfBrushHandler::ConstructL( borderBrush );
        CleanupStack::Pop( borderBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfBorderBrushHandler::NewL( 
    MAlfInterfaceProvider& aResolver,
    TInt aThicknessWidth, 
    TInt aThicknessHeight,
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY )
    {
    CAlfBorderBrushHandler* self = new( ELeave ) CAlfBorderBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aThicknessWidth,
        aThicknessHeight,
        aEdgeOffsetX,
        aEdgeOffsetY );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfBorderBrushHandler::~CAlfBorderBrushHandler()
    {
    }



// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfBorderBrushHandler::Release()
    {
    delete this;
    }
 
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//   
EXPORT_C TAny* CAlfBorderBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface(aInterface);   
    }
   
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfBorderBrushHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiBorderBrush* borderBrush = static_cast<CHuiBorderBrush*>(iBrush);
    
    switch (aCommandId)
        {
        case EAlfBorderBrushSetImage:
            {
            TAlfImageParams* params = (TAlfImageParams*) aInputBuffer.Ptr();
        	
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iTextureHandle;
        	MHuiTexture* texture = reinterpret_cast<MHuiTexture*>(handle);
   	        THuiImage huiImage;
   	        if (texture)
   	            {
   	            huiImage = THuiImage(*texture, 
	                params->iTl.iX.ValueNow(),
	                params->iTl.iY.ValueNow(),
	                params->iBr.iX.ValueNow(),
	                params->iBr.iY.ValueNow());    
   	            
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iBr.iY.TimeToTargetinMilliSeconds();
    	        
    	        huiImage.SetTexCoords(params->iTl.iX.Target(),
    	            params->iTl.iY.Target(),
    	            params->iBr.iX.Target(),
    	            params->iBr.iY.Target(),
    	            transitionTime );
   	            }
	            
	        borderBrush->SetImage( huiImage );
            break;
            }
            
        case EAlfBorderBrushSetColor:
            {
            TRgb* color = (TRgb*) aInputBuffer.Ptr();
            borderBrush->SetColor( *color );
            break;
            }
            
        case EAlfBorderBrushSetThickness:
            {
            TAlfTimedPoint* value = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*value, borderBrush->iThickness, iResolver);            
            break;
            }
        case EAlfBorderBrushSetThicknessMetric:
            {
            TAlfXYMetric* value = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiXYMetric huivalue;
            AlfXYMetricUtility::CopyMetric(*value, huivalue);            
            borderBrush->SetThickness(huivalue);
            break;
            }
            
        case EAlfBorderBrushGetThickness:
            {
            TAlfTimedPoint value;
            AlfTimedPointUtility::CopyTimedPoint(borderBrush->iThickness, value);
            TPckg<TAlfTimedPoint> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
            
        case EAlfBorderBrushSetEdgeOffset:
            {
            TAlfTimedPoint* value = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*value, borderBrush->iEdgeOffset, iResolver);            
            break;
            }
        case EAlfBorderBrushSetEdgeOffsetMetric:
            {
            TAlfXYMetric* value = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiXYMetric huivalue;
            AlfXYMetricUtility::CopyMetric(*value, huivalue);            
            borderBrush->SetEdgeOffset(huivalue);
            break;
            }
            
        case EAlfBorderBrushGetEdgeOffset:
            {
            TAlfTimedPoint value;
            AlfTimedPointUtility::CopyTimedPoint(borderBrush->iEdgeOffset, value);
            TPckg<TAlfTimedPoint> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
            
        case EAlfBorderBrushSetImageOffset:
            {
            TAlfTimedPoint* value = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*value, borderBrush->iImageOffset, iResolver);            
            break;
            }
        case EAlfBorderBrushSetImageOffsetMetric:
            {
            TAlfXYMetric* value = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiXYMetric huivalue;
            AlfXYMetricUtility::CopyMetric(*value, huivalue);            
            borderBrush->SetImageOffset(huivalue);
            break;
            }
            
        case EAlfBorderBrushGetImageOffset:
            {
            TAlfTimedPoint value;
            AlfTimedPointUtility::CopyTimedPoint(borderBrush->iImageOffset, value);
            TPckg<TAlfTimedPoint> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
            
        default:
            CAlfBrushHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfBorderBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageBrushHandler::CAlfImageBrushHandler(
    MAlfInterfaceProvider& aResolver)
: CAlfBrushHandler( aResolver ) 
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageBrushHandler::ConstructL( 
    CHuiBrush* aBrush, THuiImage aImage)
    {
    if (!aBrush)
        {
        CHuiImageBrush* imageBrush = CHuiImageBrush::NewLC(aImage);
        CAlfBrushHandler::ConstructL( imageBrush );
        CleanupStack::Pop( imageBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfImageBrushHandler::NewL( 
    MAlfInterfaceProvider& aResolver, THuiImage aImage )
    {
    CAlfImageBrushHandler* self = new( ELeave ) CAlfImageBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL , aImage );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfImageBrushHandler::~CAlfImageBrushHandler()
    {
    }


// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfImageBrushHandler::Release()
    {
    delete this;
    }
 
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//   
EXPORT_C TAny* CAlfImageBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface(aInterface);   
    }
   
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfImageBrushHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiImageBrush* imageBrush = static_cast<CHuiImageBrush*>(iBrush);
    
    switch (aCommandId)
        {
        case EAlfImageBrushSetBorders:
            {
            TAlfImageBrushParams* params = (TAlfImageBrushParams*) aInputBuffer.Ptr();
            imageBrush->SetBorders(params->iLeftBorderWidth, 
                params->iRightBorderWidth, 
                params->iTopBorderWidth, 
                params->iBottomBorderWidth);
            break;
            }
        case EAlfImageBrushSetImage:
            {
            TAlfImageBrushParams* params = (TAlfImageBrushParams*) aInputBuffer.Ptr();
            MHuiTexture* texture = reinterpret_cast<MHuiTexture*> (params->iTextureHandle);
   	        THuiImage huiImage;
   	        if (texture)
   	            {
   	            huiImage.SetTexture(*texture);
   	            
   	            huiImage.SetTexCoords(params->iTl.iX.Target(),
                                   params->iTl.iY.Target(),
                                   params->iBr.iX.Target(),
                                   params->iBr.iY.Target());
   	            }

            imageBrush->SetImage(huiImage);
            imageBrush->SetBorders(params->iLeftBorderWidth, 
                    params->iRightBorderWidth, 
                    params->iTopBorderWidth,
                    params->iBottomBorderWidth);
            break;
            }
            
        case EAlfImageBrushSetScaleMode:
            {
            CHuiImageVisual::TScaleMode* scale = (CHuiImageVisual::TScaleMode*) aInputBuffer.Ptr() ;
            imageBrush->SetScaleMode( *scale );
            break;
            }
            
        case EAlfImageBrushSetScale:
            {
            TAlfTimedValue* scale = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*scale, imageBrush->iScale, iResolver);  
            break;
            }
            
        case EAlfImageBrushScale:
            {
            TAlfTimedValue scale;
            AlfTimedValueUtility::CopyTimedValue(imageBrush->iScale, scale);
            TPckg<TAlfTimedValue> resultPckg(scale);
            aResponse = resultPckg;
            break;
            }
            
        case EAlfImageBrushSetOffset:
            {
            TAlfTimedPoint* offset = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*offset, imageBrush->iImageOffset, iResolver); 
            break;
            }
            
        case EAlfImageBrushOffset:
            {
            TAlfTimedPoint point;
            AlfTimedPointUtility::CopyTimedPoint(imageBrush->iImageOffset, point);
            TPckg<TAlfTimedPoint> buf(point);
            aResponse = buf;
            break;
            }
            
        default:
            CAlfBrushHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfImageBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }



// SHADOW BORDER BRUSH HANDLER

EXPORT_C MAlfExtension* CAlfShadowBorderBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver,
        TInt aWidth )
    {
    CAlfShadowBorderBrushHandler* self = new( ELeave ) CAlfShadowBorderBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aWidth );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CAlfShadowBorderBrushHandler::CAlfShadowBorderBrushHandler(
    MAlfInterfaceProvider& aResolver )
    : CAlfBrushHandler( aResolver )
    {
    }
    
EXPORT_C void CAlfShadowBorderBrushHandler::ConstructL( 
        CHuiBrush* aBrush,
        TInt aWidth)
    {
    if (!aBrush)
        {
        CHuiShadowBorderBrush* shadowBorderBrush = CHuiShadowBorderBrush::NewLC( 
           aWidth );
        CAlfBrushHandler::ConstructL( shadowBorderBrush );
        CleanupStack::Pop( shadowBorderBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }

EXPORT_C MAlfExtension* CAlfShadowBorderBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver,
        TAlfMetric aWidth )
    {    
    CAlfShadowBorderBrushHandler* self = new( ELeave ) CAlfShadowBorderBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aWidth );
    CleanupStack::Pop( self );  
    return self;
    }
        
EXPORT_C void CAlfShadowBorderBrushHandler::ConstructL( 
        CHuiBrush* aBrush,
        TAlfMetric aWidth)
    {
    if (!aBrush)
        {
        THuiMetric huiWidth;
        AlfMetricUtility::CopyMetric(aWidth, huiWidth);
        CHuiShadowBorderBrush* shadowBorderBrush = CHuiShadowBorderBrush::NewLC( 
           huiWidth );
        CAlfBrushHandler::ConstructL( shadowBorderBrush );
        CleanupStack::Pop( shadowBorderBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }
    
EXPORT_C CAlfShadowBorderBrushHandler::~CAlfShadowBorderBrushHandler()
    {
    }
   
    
EXPORT_C void CAlfShadowBorderBrushHandler::Release()
    {
    delete this;
    }
    
EXPORT_C TAny* CAlfShadowBorderBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface( aInterface );
    }
    
EXPORT_C void CAlfShadowBorderBrushHandler::HandleCmdL(
    TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiShadowBorderBrush* shadowBorderBrush = static_cast<CHuiShadowBorderBrush*>(iBrush);
    
    switch ( aCommandId )
        {
        case EAlfShadowBorderBrushGetWidth:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(shadowBorderBrush->iWidth, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward  
            break;
            }
            
        case EAlfShadowBorderBrushSetWidth:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, shadowBorderBrush->iWidth, iResolver);            
            break;
            }

        case EAlfShadowBorderBrushSetWidthMetric:
            {
            TAlfMetric* value = (TAlfMetric*) aInputBuffer.Ptr() ;
            THuiMetric huivalue;
            AlfMetricUtility::CopyMetric(*value, huivalue);            
            shadowBorderBrush->SetWidth(huivalue);
            break;
            }            
        default:
            CAlfBrushHandler::HandleCmdL( aCommandId, aInputBuffer, aResponse );
        }
    }

EXPORT_C void CAlfShadowBorderBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }
    
// DROP SHADOW BRUSH HANDLER  
    
EXPORT_C MAlfExtension* CAlfDropShadowBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver,
        TInt aDepth )
    {
    CAlfDropShadowBrushHandler* self = new( ELeave ) CAlfDropShadowBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aDepth );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C MAlfExtension* CAlfDropShadowBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver,
        TAlfMetric aDepth )
    {
    CAlfDropShadowBrushHandler* self = new( ELeave ) CAlfDropShadowBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL(
        NULL,
        aDepth );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CAlfDropShadowBrushHandler::CAlfDropShadowBrushHandler(
    MAlfInterfaceProvider& aResolver)
    : CAlfBrushHandler( aResolver )
    {
    }
    
EXPORT_C void CAlfDropShadowBrushHandler::ConstructL( 
        CHuiBrush* aBrush,
        TAlfMetric aDepth )
    {
    if (!aBrush)
        {
        THuiMetric huiDepth;
        AlfMetricUtility::CopyMetric(aDepth, huiDepth);
        CHuiDropShadowBrush* dropShadowBrush = CHuiDropShadowBrush::NewLC( 
           huiDepth );
        CAlfBrushHandler::ConstructL( dropShadowBrush );
        CleanupStack::Pop( dropShadowBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }

EXPORT_C void CAlfDropShadowBrushHandler::ConstructL( 
        CHuiBrush* aBrush,
        TInt aDepth )
    {
    if (!aBrush)
        {
        CHuiDropShadowBrush* dropShadowBrush = CHuiDropShadowBrush::NewLC( 
           aDepth );
        CAlfBrushHandler::ConstructL( dropShadowBrush );
        CleanupStack::Pop( dropShadowBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }
    
EXPORT_C CAlfDropShadowBrushHandler::~CAlfDropShadowBrushHandler()
    {
    }

    
EXPORT_C void CAlfDropShadowBrushHandler::Release()
    {
    delete this;
    }
    
EXPORT_C TAny* CAlfDropShadowBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface( aInterface );
    }
    
EXPORT_C void CAlfDropShadowBrushHandler::HandleCmdL(
    TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiDropShadowBrush* dropShadowBrush = static_cast<CHuiDropShadowBrush*>(iBrush);
    
    switch ( aCommandId )
        {
        case EAlfDropShadowBrushGetSoftness:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(dropShadowBrush->iSoftness, value);
            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward  
            break;
            }
            
        case EAlfDropShadowBrushSetSoftness:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, dropShadowBrush->iSoftness, iResolver);            
            break;
            }
        case EAlfDropShadowBrushSetSoftnessMetric:
            {
            TAlfMetric* value = (TAlfMetric*) aInputBuffer.Ptr() ;
            THuiMetric huivalue;
            AlfMetricUtility::CopyMetric(*value, huivalue);            
            dropShadowBrush->SetSoftness(huivalue);
            break;
            }
            
        case EAlfDropShadowBrushGetDepth:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(dropShadowBrush->iDepth, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward  
            break;
            }
            
        case EAlfDropShadowBrushSetDepth:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, dropShadowBrush->iDepth, iResolver);            
            break;
            }
        case EAlfDropShadowBrushSetDepthMetric:
            {
            TAlfMetric* value = (TAlfMetric*) aInputBuffer.Ptr() ;
            THuiMetric huivalue;
            AlfMetricUtility::CopyMetric(*value, huivalue);            
            dropShadowBrush->SetDepth(huivalue);
            break;
            }
        case EAlfDropShadowBrushSetColor:
            {
            TRgb* value = (TRgb*) aInputBuffer.Ptr() ;
            dropShadowBrush->SetColor(*value);
            break;
            }
        default:
            CAlfBrushHandler::HandleCmdL( aCommandId, aInputBuffer, aResponse );
        }
    }

EXPORT_C void CAlfDropShadowBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }


    
// GRADIENT BRUSH HANDLER  
    
EXPORT_C MAlfExtension* CAlfGradientBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver )
    {
    CAlfGradientBrushHandler* self = new( ELeave ) CAlfGradientBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CAlfGradientBrushHandler::CAlfGradientBrushHandler(
    MAlfInterfaceProvider& aResolver)
    : CAlfBrushHandler( aResolver )
    {
    }
    
EXPORT_C void CAlfGradientBrushHandler::ConstructL( 
        CHuiBrush* aBrush )
    {
    if (!aBrush)
        {
        CHuiGradientBrush* gradientBrush = CHuiGradientBrush::NewLC();
        CAlfBrushHandler::ConstructL( gradientBrush );
        CleanupStack::Pop( gradientBrush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }
    
EXPORT_C CAlfGradientBrushHandler::~CAlfGradientBrushHandler()
    {
    }
    
    
EXPORT_C void CAlfGradientBrushHandler::Release()
    {
    delete this;
    }
    
EXPORT_C TAny* CAlfGradientBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface( aInterface );
    }
    
EXPORT_C void CAlfGradientBrushHandler::HandleCmdL(
    TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiGradientBrush* dropShadowBrush = static_cast<CHuiGradientBrush*>(iBrush);
    
    switch ( aCommandId )
        { 
        case EAlfGradientBrushSetDirection:
            {
            CHuiGradientBrush::TDirection* value = (CHuiGradientBrush::TDirection*) aInputBuffer.Ptr();
            dropShadowBrush->SetDirection( *value );
            break;
            }
            
        case EAlfGradientBrushSetColor:
            {
            TAlfGradientBrushSetColorParams* value = (TAlfGradientBrushSetColorParams*) aInputBuffer.Ptr();
            dropShadowBrush->SetColor( value->iColor, value->iOpacity );
            break;
            }
            
        case EAlfGradientBrushSetImage:
            {
            TAlfImageParams* params = (TAlfImageParams*) aInputBuffer.Ptr();
        	
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iTextureHandle;
        	MHuiTexture* texture = reinterpret_cast<MHuiTexture*>(handle);
   	        THuiImage huiImage;
   	        if (texture)
   	            {
   	            huiImage = THuiImage(*texture, 
	                params->iTl.iX.ValueNow(),
	                params->iTl.iY.ValueNow(),
	                params->iBr.iX.ValueNow(),
	                params->iBr.iY.ValueNow());    
   	            
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iBr.iY.TimeToTargetinMilliSeconds();
    	        
    	        huiImage.SetTexCoords(params->iTl.iX.Target(),
    	            params->iTl.iY.Target(),
    	            params->iBr.iX.Target(),
    	            params->iBr.iY.Target(),
    	            transitionTime );
   	            }
	            
	        dropShadowBrush->SetImage( huiImage );
            break;
            }
            
        case EAlfGradientBrushColor:
            {
            TInt* value = (TInt*) aInputBuffer.Ptr();
            const TRgb result = dropShadowBrush->Color( *value );
            TPckg<TRgb> resultPckg(result);
            aResponse = resultPckg; // awkward  
            break;
            }
            
        case EAlfGradientBrushOpacity:
            {
            TInt* value = (TInt*) aInputBuffer.Ptr();
            const TReal32 result = dropShadowBrush->Opacity( *value );
            TPckg<TReal32> resultPckg(result);
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfGradientBrushAppendColor:
            {
            TAlfGradientBrushAppendColorParams* value = (TAlfGradientBrushAppendColorParams*) aInputBuffer.Ptr();
            dropShadowBrush->AppendColorL( value->iPosition, value->iColor, value->iOpacity );
            break;
            }
            
        default:
            CAlfBrushHandler::HandleCmdL( aCommandId, aInputBuffer, aResponse );
        }
    }

EXPORT_C void CAlfGradientBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }


// FRAME BRUSH HANDLER

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrushHandler::CAlfFrameBrushHandler(
    MAlfInterfaceProvider& aResolver)
: CAlfBrushHandler( aResolver ) 
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrushHandler::ConstructL( 
    CHuiBrush* aBrush, const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    if (!aBrush)
        {
        CHuiFrameBrush* brush = CHuiFrameBrush::NewLC(aIID,aEdgeOffsetX,aEdgeOffsetY);
        CAlfBrushHandler::ConstructL( brush );
        CleanupStack::Pop( brush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrushHandler::ConstructL( 
    CHuiBrush* aBrush, const TAknsItemID& aIID, TAlfXYMetric aEdgeOffset)
    {
    if (!aBrush)
        {
        THuiXYMetric huiEdgeOffset;
        AlfXYMetricUtility::CopyMetric(aEdgeOffset, huiEdgeOffset);
        CHuiFrameBrush* brush = CHuiFrameBrush::NewLC(aIID, huiEdgeOffset);
        CAlfBrushHandler::ConstructL( brush );
        CleanupStack::Pop( brush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrushHandler::ConstructL( 
        CHuiBrush* aBrush, const THuiImage& aImage, TAlfXYMetric aEdgeOffset)
    {
    if (!aBrush)
        {
        THuiXYMetric huiEdgeOffset;
        AlfXYMetricUtility::CopyMetric(aEdgeOffset, huiEdgeOffset);

        CHuiFrameBrush* brush = CHuiFrameBrush::NewLC(aImage, huiEdgeOffset);
        CAlfBrushHandler::ConstructL( brush );
        CleanupStack::Pop( brush );
        }
    else 
        {
        CAlfBrushHandler::ConstructL( aBrush );
        }        
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfFrameBrushHandler::NewL( 
    MAlfInterfaceProvider& aResolver, const TAknsItemID& aIID, TInt aEdgeOffsetX, TInt aEdgeOffsetY )
    {
    CAlfFrameBrushHandler* self = new( ELeave ) CAlfFrameBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL , aIID, aEdgeOffsetX, aEdgeOffsetY );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfFrameBrushHandler::NewL( 
    MAlfInterfaceProvider& aResolver, const TAknsItemID& aIID, TAlfXYMetric aEdgeOffset )
    {
    CAlfFrameBrushHandler* self = new( ELeave ) CAlfFrameBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL , aIID, aEdgeOffset );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C MAlfExtension* CAlfFrameBrushHandler::NewL( 
        MAlfInterfaceProvider& aResolver, const THuiImage& aImage, TAlfXYMetric aEdgeOffset)
    {
    CAlfFrameBrushHandler* self = new( ELeave ) CAlfFrameBrushHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( NULL , aImage, aEdgeOffset );
    CleanupStack::Pop( self );
    return self;        
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfFrameBrushHandler::~CAlfFrameBrushHandler()
    {
    }



// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfFrameBrushHandler::Release()
    {
    delete this;
    }
 
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//   
EXPORT_C TAny* CAlfFrameBrushHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfBrushHandler::GetInterface(aInterface);   
    }
   
// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfFrameBrushHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiFrameBrush* frameBrush = static_cast<CHuiFrameBrush*>(iBrush);
    
    switch (aCommandId)
        {
        case EAlfFrameBrushSetFrameRects:
            {
            TAlfFrameBrushParams* params = (TAlfFrameBrushParams*) aInputBuffer.Ptr();
            frameBrush->SetFrameRectsL(params->iInnerRect, params->iOuterRect);
            break;
            }
        case EAlfFrameBrushSetEdgeOffsetMetric:
            {
            TAlfXYMetric* params = (TAlfXYMetric*) aInputBuffer.Ptr();
            THuiXYMetric huimetric;
            AlfXYMetricUtility::CopyMetric(*params, huimetric);
            frameBrush->SetEdgeOffset(huimetric);
            break;
            }
        case EAlfFrameBrushSetImage:
            {
            TAlfImageParams* params = (TAlfImageParams*)aInputBuffer.Ptr();
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iTextureHandle;
        	MHuiTexture* texture = reinterpret_cast<MHuiTexture*>(handle);
            THuiImage huiImage;
            if (texture)
                {
                huiImage = THuiImage(*texture, 
                    params->iTl.iX.ValueNow(),
                    params->iTl.iY.ValueNow(),
                    params->iBr.iX.ValueNow(),
                    params->iBr.iY.ValueNow());    
                
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iBr.iY.TimeToTargetinMilliSeconds();
    	        
    	        huiImage.SetTexCoords(params->iTl.iX.Target(),
    	            params->iTl.iY.Target(),
    	            params->iBr.iX.Target(),
    	            params->iBr.iY.Target(),
    	            transitionTime );
                }
            
            frameBrush->SetImage(huiImage);

            break;
            }
        case EAlfFrameBrushSetFramePartImage:
            {
            TAlfFrameBrushFramePartImageParams* params = (TAlfFrameBrushFramePartImageParams*)aInputBuffer.Ptr();
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iImage.iTextureHandle;
        	MHuiTexture* texture = reinterpret_cast<MHuiTexture*>(handle);
            THuiImage huiImage;
            if (texture)
                {
                huiImage = THuiImage(*texture, 
                    params->iImage.iTl.iX.ValueNow(),
                    params->iImage.iTl.iY.ValueNow(),
                    params->iImage.iBr.iX.ValueNow(),
                    params->iImage.iBr.iY.ValueNow());    
                
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iImage.iBr.iY.TimeToTargetinMilliSeconds();
    	        
    	        huiImage.SetTexCoords(params->iImage.iTl.iX.Target(),
    	            params->iImage.iTl.iY.Target(),
    	            params->iImage.iBr.iX.Target(),
    	            params->iImage.iBr.iY.Target(),
    	            transitionTime );
                }
            
            CHuiFrameBrush::TFrameParts part = CHuiFrameBrush::TFrameParts(params->iFramePart);
            frameBrush->SetImage(part, huiImage);

            break;
            }
        default:
            CAlfBrushHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfFrameBrushHandler::BrushHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfBrushHandler::BrushHandlerExtension(aExtensionUid, aExtensionParams);
    }

