/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   factory for alf built-in hitchcock compenents
*
*/


#include <e32cmn.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiLayout.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiEvent.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiUtil.h>
#include "alf/alfvisualhandlers.h"
#include "alf/alflayouthandlers.h"
#include "alf/alfbrushhandlers.h"
#include "alftextstylehandlers.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfstaticfactory.h"
#include "alf/alfconstants.h"
#include "alfmappingfunctionhandlers.h"
#include "alf/alfmetric.h"


CHuiVisual* FindLastHittingChild( const CHuiVisual& aParentVisual, 
                                  const TPoint& aDisplayPoint )
    {
    // If we want that the pointer event are passed only to the visuals which 
    // (effective) opacity is greater than 50%, this would be the place to 
    // implement it.
    for ( TInt i = aParentVisual.Count() - 1 ; i >= 0 ; i-- )
        {
        // DisplayRect() returns the current value of the rectange. If the pointer
        // hit needs to be compared to the target rect, change this function call
        // from DisplayRect() into DisplayRectTarget()
        const TRect rect =  aParentVisual.Visual( i ).DisplayRect();
        if ( rect.Contains( aDisplayPoint ) )
            {
            CHuiVisual* leaf = 
                FindLastHittingChild( aParentVisual.Visual( i ), aDisplayPoint );
            return leaf ? leaf : &aParentVisual.Visual( i );
            }
        }
    return NULL;
    }

//----------------------------
// Control, private so declaration directly here
//----------------------------

NONSHARABLE_CLASS(CAlfControl2): public CHuiControl
    {
public:
    CAlfControl2(CHuiEnv& aSharedEnv, CAlfControl2** ptr, CAlfAppSrvSessionBase* aSession):
        CHuiControl(aSharedEnv),
        iPtr(ptr),
        iSession(aSession)
        {}
        
    TBool OfferEventL(const THuiEvent& aEvent)
        {
        if ( aEvent.IsPointerEvent() )
            {
            // Update event coordinates to match the "client rect" of the application
            THuiEvent eventWithLocalCoordinates(aEvent); 
            
            // Set the iParentPosition coordinates to be ralative to the 
            // visual aread of the display i.e  the root visuals. 
            const TPoint clientDrawingAreaTl = iSession->ClientDrawingArea().iTl;
            eventWithLocalCoordinates.iPointerEvent.iParentPosition -= clientDrawingAreaTl;
            
            if ( aEvent.Visual() )
                {
                // If visual is found, move the iPosition coordinates to be relative to the
                // visual area.
                const THuiRealRect realVisualArea = aEvent.Visual()->DisplayRect();
                const TPoint intVisualTl( 
                    HUI_ROUND_FLOAT_TO_INT( realVisualArea.iTl.iX ),
                    HUI_ROUND_FLOAT_TO_INT( realVisualArea.iTl.iY ) );
                 
                eventWithLocalCoordinates.iPointerEvent.iPosition -= intVisualTl;
                }
            else
                {
                // If there is no accociated visual, set iPosotion coordinates to be relative to
                // the display visual area.
                eventWithLocalCoordinates.iPointerEvent.iPosition -= clientDrawingAreaTl;
                }
            
            TBool isLongTap = aEvent.iPointerEvent.iType == TPointerEvent::EButtonRepeat;
            if ( isLongTap )
                {
                iSession->StartPointerHandling();
                }
            iSession->HandleEventL(this, aEvent.Visual() , eventWithLocalCoordinates);
            if ( isLongTap )
                {
                iSession->FlushPointerHandling();
                }
            }
        return EFalse; // let env offer event to all ctrls it hits
        }
    
    ~CAlfControl2()
        {
        *iPtr = 0;
        
        // Remove possbile pointer observers
        if ( Env().DisplayCount() )
            {
            CHuiDisplay& disp = Env().PrimaryDisplay(); // assume one display
            disp.Roster().Observers(EHuiInputPointerDrag).RemoveIfFound(*this);
            disp.Roster().Observers(EHuiInputPointerLongTap).RemoveIfFound(*this);
            disp.Roster().Observers(EHuiInputPointerUnhandled).RemoveIfFound(*this);
            }
        }
    
private:
    CAlfControl2** iPtr;
    CAlfAppSrvSessionBase* iSession;
    };


NONSHARABLE_CLASS(CAlfControlHandler): public CBase, public MAlfExtension
    {
public:
    static MAlfExtension* NewL(const TDesC8& aInputBuffer, CHuiEnv& aSharedEnv, CAlfAppSrvSessionBase* aSession );
private:    
    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);    
    CAlfControl2* iControl;
    ~CAlfControlHandler() 
        { 
        if (iControl && !iControl->ControlGroup())
            {
            delete iControl; 
            }
        }
    };

MAlfExtension* CAlfControlHandler::NewL(const TDesC8& /*aInputBuffer*/, CHuiEnv& aSharedEnv, CAlfAppSrvSessionBase* aSession)
    {
    CAlfControlHandler* me = new (ELeave) CAlfControlHandler();
    CleanupStack::PushL(me);
    me->iControl = new (ELeave) CAlfControl2(aSharedEnv, &me->iControl, aSession);
    CleanupStack::Pop();
    return me;
    }

void CAlfControlHandler::Release()
    {
    delete this;
    }
        
TAny* CAlfControlHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    switch (aInterface)
        {
        case EHuiObjectTypeControl:
            return static_cast<CHuiControl*>(iControl);
        case EHuiInterfaceVisualOwner:
            return static_cast<MHuiVisualOwner*>(iControl);
        default:
            return 0;
                    
        }
          
    }
    
void CAlfControlHandler::HandleCmdL(TInt /*aCommandId*/, const TDesC8& /*aInputBuffer*/, TDes8& /*aResponse*/)
    {
    }


MAlfExtension* TAlfFixedFactory::CreateExtensionL(const TInt aObjectId, const TDesC8& aInitialParams, MAlfInterfaceProvider& aResolver )
    {
    
    MAlfExtension* result = 0;
    switch(aObjectId)
        {
        case EAlfCntrlCreate:
            {
            result = CAlfControlHandler::NewL(aInitialParams, *aResolver.SharedHuiEnv(), static_cast<CAlfAppSrvSessionBase*>(&aResolver));
            break;
            }
        
        case EAlfVisualCreate:
        case EAlfTextVisualCreate:
        case EAlfLCTTextVisualCreate:
        case EAlfLayoutCreate:
        case EAlfAnchorLayoutCreate:
        case EAlfLCTAnchorLayoutCreate:
        case EAlfDeckLayoutCreate:
        case EAlfFlowLayoutCreate:
        case EAlfCurvePathLayoutCreate:
        case EAlfGridLayoutCreate:
        case EAlfLCTGridLayoutCreate:
        case EAlfViewportLayoutCreate:
        case EAlfImageVisualCreate:
        case EAlfLineVisualCreate:
        case EAlfCanvasVisualCreate:
            {
            TInt2* params = (TInt2*)aInitialParams.Ptr();
            
            CHuiControl* owner = 0;
            CHuiLayout* parentLayout = 0;
            
            if (!params->iInt1)
                {
                User::Leave(KErrArgument);
                }
            else
                { // currently we must use concrete owner as interface does not provide means to 
                  // transfer ownership
                owner = (CHuiControl*) aResolver.GetInterfaceL(EHuiObjectTypeControl, params->iInt1 );    
                } 
                   
            if (params->iInt2) // not compulsory
                {
                parentLayout = (CHuiLayout*) aResolver.GetInterfaceL(EHuiObjectTypeLayout, params->iInt2 ); 
                }
            
            if (aObjectId == EAlfVisualCreate)
                result = CAlfVisualHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId == EAlfTextVisualCreate)
                result = CAlfTextVisualHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId == EAlfLCTTextVisualCreate)
                result = CAlfLCTTextVisualHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId == EAlfImageVisualCreate)
                result = CAlfImageVisualHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfLayoutCreate)
                result = CAlfLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfAnchorLayoutCreate)
                result = CAlfAnchorLayoutHandler::NewL(aResolver, owner, parentLayout);            
            else if (aObjectId ==EAlfLCTAnchorLayoutCreate)
                result = CAlfLCTAnchorLayoutHandler::NewL(aResolver, owner, parentLayout);            
            else if (aObjectId ==EAlfGridLayoutCreate)
                result = CAlfGridLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfLCTGridLayoutCreate)
                result = CAlfLCTGridLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfDeckLayoutCreate)
                result = CAlfDeckLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfFlowLayoutCreate)
                result = CAlfFlowLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfViewportLayoutCreate)
                result = CAlfViewportLayoutHandler::NewL(aResolver, owner, parentLayout);                        
            else if (aObjectId ==EAlfCurvePathLayoutCreate)
                result = CAlfCurvePathLayoutHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfLineVisualCreate)
                result = CAlfLineVisualHandler::NewL(aResolver, owner, parentLayout);
            else if (aObjectId ==EAlfCanvasVisualCreate)
                result = CAlfCanvasVisualHandler::NewL(aResolver, owner, parentLayout);
            
            ASSERT(result); // missing obejct would be clear implementation error
            break;
            }
            
        case EAlfMeshVisualCreate:
            {
            TInt3* params = (TInt3*)aInitialParams.Ptr();
            
            CHuiControl* owner = 0;
            THuiMeshType type = EHuiMeshTypeProcedural;
            CHuiLayout* parentLayout = 0;
            
            if (!params->iInt1)
                {
                User::Leave(KErrArgument);
                }
            else
                { // currently we must use concrete owner as interface does not provide means to 
                  // transfer ownership
                owner = (CHuiControl*) aResolver.GetInterfaceL(EHuiObjectTypeControl, params->iInt1 );    
                }
            
                   
            if (params->iInt2) // not compulsory
                {
                parentLayout = (CHuiLayout*) aResolver.GetInterfaceL(EHuiObjectTypeLayout, params->iInt2 ); 
                }
            
            result = CAlfMeshVisualHandler::NewL(aResolver, owner, type, parentLayout);  
            
            ASSERT(result); // missing obejct would be clear implementation error            
            break;
            }

        case EAlfBorderBrushCreate:
            {
            TAlfCreateBorderBrushParams* params = (TAlfCreateBorderBrushParams*)aInitialParams.Ptr();
            
            result = CAlfBorderBrushHandler::NewL(
                aResolver,
                params->iThicknessWidth,
                params->iThicknessHeight,
                params->iEdgeOffsetX,
                params->iEdgeOffsetY );
            }
            break;   
        case EAlfBorderBrushCreateMetric:
            {
            TAlfCreateBorderBrushMetricParams* params = (TAlfCreateBorderBrushMetricParams*)aInitialParams.Ptr();
            
            result = CAlfBorderBrushHandler::NewL(
                aResolver,
                params->iThickness,
                params->iEdgeOffset );
            }
            break;   

        case EAlfImageBrushCreate:
            {
            TAlfImageBrushParams* params = (TAlfImageBrushParams*)aInitialParams.Ptr();
            MHuiTexture* texture = reinterpret_cast<MHuiTexture*> (params->iTextureHandle);            
            result = CAlfImageBrushHandler::NewL(aResolver, THuiImage(*texture));
            }
            break;  
            
        case EAlfShadowBorderBrushCreate:
            {
            TInt* params = (TInt*)aInitialParams.Ptr();
            result = CAlfShadowBorderBrushHandler::NewL(aResolver, *params);
            break;
            }
        case EAlfShadowBorderBrushCreateMetric:
            {
            TAlfMetric* params = (TAlfMetric*)aInitialParams.Ptr();
            result = CAlfShadowBorderBrushHandler::NewL(aResolver, *params);
            break;
            }
            
        case EAlfDropShadowBrushCreate:
            {
            TInt* params = (TInt*)aInitialParams.Ptr();
            result = CAlfDropShadowBrushHandler::NewL(aResolver, *params);
            break;
            }
        case EAlfDropShadowBrushCreateMetric:
            {
            TAlfMetric* params = (TAlfMetric*)aInitialParams.Ptr();
            result = CAlfDropShadowBrushHandler::NewL(aResolver, *params);
            break;
            }
            
        case EAlfGradientBrushCreate:
            {
            result = CAlfGradientBrushHandler::NewL( aResolver );
            break;
            }
        case EAlfFrameBrushCreate:
            {
            TAlfFrameBrushParams* params = (TAlfFrameBrushParams*)aInitialParams.Ptr();
            result = CAlfFrameBrushHandler::NewL(aResolver, params->iFrameIID, params->iEdgeOffsetX, params->iEdgeOffsetY);
            }
            break;  
        case EAlfFrameBrushCreateMetric:
            {
            TAlfFrameBrushMetricParams* params = (TAlfFrameBrushMetricParams*)aInitialParams.Ptr();
            result = CAlfFrameBrushHandler::NewL(aResolver, params->iFrameIID, params->iEdgeOffset);
            }
            break;  
        case EAlfFrameBrushCreateImage:
            {
            TAlfFrameBrushImageParams* params = (TAlfFrameBrushImageParams*)aInitialParams.Ptr();

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

            result = CAlfFrameBrushHandler::NewL(aResolver, huiImage, params->iEdgeOffset);
            }
            break;  

        case EAlfMappingFunctionConstantCreate:
            {
            TReal32* params = (TReal32*)aInitialParams.Ptr();
            result = new (ELeave) TAlfConstantMappingFunctionHandler(
                aResolver,
                *params );
            break;
            }
            
        case EAlfMappingFunctionLinearCreate:
            {
            TReal2* params = (TReal2*)aInitialParams.Ptr();
            result = new (ELeave) TAlfLinearMappingFunctionHandler(
                aResolver,
                params->iReal1,
                params->iReal2 );
            break;
            }
            
        case EAlfMappingFunctionSineCreate:
            {
            TReal2* params = (TReal2*)aInitialParams.Ptr();
            result = new (ELeave) TAlfSineMappingFunctionHandler(
                aResolver,
                params->iReal1,
                params->iReal2 );
            break;
            }
            
        case EAlfMappingFunctionCosineCreate:
            {
            TReal2* params = (TReal2*)aInitialParams.Ptr();
            result = new (ELeave) TAlfCosineMappingFunctionHandler(
                aResolver,
                params->iReal1,
                params->iReal2 );
            break;
            }
            
        case EAlfMappingFunctionAverageCreate:
            {
            TInt2* params = (TInt2*)aInitialParams.Ptr();
            
            MHuiMappingFunction* func1 = NULL;
            MHuiMappingFunction* func2 = NULL;
            
            if ( params->iInt1 )
                {
                func1 = (MHuiMappingFunction*) aResolver.GetInterfaceL(EHuiInterfaceMappingFunction, params->iInt1 );
                }
                
            if ( params->iInt2 )
                {
                func2 = (MHuiMappingFunction*) aResolver.GetInterfaceL(EHuiInterfaceMappingFunction, params->iInt2 );
                }
            
            
            result = new (ELeave) TAlfAverageMappingFunctionHandler(
                aResolver,
                func1,
                func2
                );
            }
            break;
            
        case EAlfCurvePathCreate:
            {
            CHuiCurvePath* curvePath = NULL;
            TInt* params = (TInt*)aInitialParams.Ptr();
            if ( *params )
                {
                curvePath = (CHuiCurvePath*) aResolver.GetInterfaceL(EHuiObjectTypeCurvePathProvider, *params );
                }
            result = CAlfCurvePathHandler::NewL( aResolver,curvePath );      
            break;
            }
        case EAlfMappingFunctionTableCreate:
            {
            result = new (ELeave) TAlfTableMappingFunctionHandler(aResolver);            
            break;
            }
            
        case EAlfPlatformTextStyleCreate:
	        {
	        TInt2* params = (TInt2*)aInitialParams.Ptr();
	        result = new (ELeave) TAlfTextStyleHandler(aResolver, params->iInt1, params->iInt2, EAlfPlatformTextStyleCreate);
	        break;	
	        }
	        
        case EAlfPreconfiguredTextStyleCreate:
	        {
	        TInt* params = (TInt*)aInitialParams.Ptr();
	        result = new (ELeave) TAlfTextStyleHandler(aResolver, *params, 0, EAlfPreconfiguredTextStyleCreate);
	        break;
	        }	         
	        
        case EAlfPlatformTextStyleCopy:
	        {
	        TInt* params = (TInt*)aInitialParams.Ptr();
	        result = new (ELeave) TAlfTextStyleHandler(aResolver, *params, 0, EAlfPlatformTextStyleCopy);
	        break;	
	        }
	        
        default:
            User::Leave(KErrNotSupported);
            break;            
        }

    return result;
    }

void TAlfFixedFactory::Release()
    {
    delete this;
    }

// end of file

