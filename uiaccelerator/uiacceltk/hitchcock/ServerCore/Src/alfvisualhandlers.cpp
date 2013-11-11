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
* Description:   definitions of alf built-in visual support
*
*/



#include "alf/alfvisualhandlers.h"
#include "alftextstylehandlers.h"
#include "alf/alfconstants.h"
#include <uiacceltk/HuiTextVisual.h>
#include <uiacceltk/HuiGridLayout.h>
#include <uiacceltk/HuiImageVisual.h>
#include "alf/alfbrushhandlers.h"
#include <alf/alfvisual.h>
#include <alf/alftextvisual.h>
#include <uiacceltk/HuiLineVisual.h>
#include <uiacceltk/HuiMeshVisual.h>
#include <uiacceltk/huiproceduralmesh.h>
#include <uiacceltk/huim3gmesh.h>
#include <uiacceltk/HuiUtil.h>
#include <alf/alfmaterial.h>
#include "alf/alfserverutils.h"
#include "alfsrvvisualowner.h"
#include <AknsConstants.h>
#include "alf/alfappui.h"
#include "alfsrvdropshadowhandler.h"
#include <AknFontSpecification.h> // @todo mka
#include <gdi.h> // @todo mka
#include <uiacceltk/HuiCanvasVisual.h>

// Remove when EAlfVisualEnableTransformation handing is fixed
#include "alfappsrvsession.h"
#include "alfsrvsubsessionbase.h"
#include "alfsrvtransformationsubsession.h"

#define AMT_CONTROL() static_cast<CAlfModuleTestDataControl*>(Dll::Tls())
#include "alfmoduletest.h" 

#ifdef RD_ALF_IN_PLATFORM
#include <aknlayout2hierarchy.h>
#endif

#include <avkon.hrh> //     ELayoutAlignNone, etc

#include "HuiFxEngine.h"
#include "HuiFxEffect.h"
struct CAlfVisualHandler::TPrivateData
    {
    CAlfSrvVisualOwner* iVisualOwner;
    };

//----------------------------
// Visual base
//----------------------------
EXPORT_C MAlfExtension* CAlfVisualHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfVisualHandler* me = new (ELeave) CAlfVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }

EXPORT_C CAlfVisualHandler::CAlfVisualHandler(MAlfInterfaceProvider& aResolver):iResolver(aResolver)
    {
    }


EXPORT_C void CAlfVisualHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        iVisual = CHuiVisual::AddNewL(aOwner, aParentLayout);          
        }
    else 
        {
        iVisual = aVisual;    
        }
        
    iPrivateData = new (ELeave) TPrivateData;
    iPrivateData->iVisualOwner = NULL;
    
    }


EXPORT_C CAlfVisualHandler::~CAlfVisualHandler()
    {
    if ( iPrivateData )
        {
        if ( iPrivateData->iVisualOwner )
            {
            if ( iVisual ) // If the visual has not been deleted already
                {
                iVisual->SetOwner( *iPrivateData->iVisualOwner->RealOwner() );
                }
            delete iPrivateData->iVisualOwner;
            iPrivateData->iVisualOwner = NULL;
            }
        }
    delete iPrivateData;
    iPrivateData = NULL;
    
    iVisual = NULL;
    }

EXPORT_C void CAlfVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfVisualHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    switch (aInterface)
        {
        case EHuiObjectTypeVisual:
            return iVisual;
        case EAlfVisualHandler:
            return this;
        default:
            return 0;
                    
        }

    }

void NullChildVisualHandlerPointers(
    CHuiVisual& iVisual, 
    const RPointerArray<CAlfSrvSubSessionBase>& aArray)
    {
    for(TInt i = iVisual.Count() - 1; i >= 0; --i)
        {
        NullChildVisualHandlerPointers( iVisual.Visual(i), aArray );
        }
        
    for ( TInt h = 0 ; h < aArray.Count(); h++ )
        {
        if ( aArray[h]->AsHuiVisual() == &iVisual )
            {
            CAlfVisualHandler* visualHandler = static_cast<CAlfVisualHandler*>( aArray[h]->AsCommandHandler() );
            visualHandler->HuiVisualAboutToTerminate();
            }
        }
   
    }

void CAlfVisualHandler::HuiVisualAboutToTerminate()
    {
    iVisual = NULL;
    }


EXPORT_C void CAlfVisualHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    switch (aCommandId)
        {
        case EAlfVisualRemoveAndDestroyAll:
            {
            // we need to NULL all the child visual handlers
            CHuiVisual* tmp = iVisual;
            
            CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
            RPointerArray<CAlfSrvSubSessionBase> array;
            TRAP_IGNORE( session.GetSubsessionsByTypeL( array, EAlfVisualHandler ) );
            NullChildVisualHandlerPointers(*iVisual, array);
            array.Close();
            
            tmp->RemoveAndDestroyAllD();
            tmp = NULL;
            break;                
            }
        
        case EAlfVisualUpdateChildrenLayout:
            {
            TInt* transitionTime = (TInt*) aInputBuffer.Ptr();
            iVisual->UpdateChildrenLayout( *transitionTime >= 0 ? *transitionTime : CHuiStatic::LayoutTransitionTime() );
            break;
            }
                
        case  EAlfVisualSetOpacity:
            {
            TAlfTimedValue* opacity = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*opacity, iVisual->iOpacity, iResolver);
            break;    
            }
        
        case EAlfVisualGetOpacity:
            {
            TAlfTimedValue opacity;
            AlfTimedValueUtility::CopyTimedValue(iVisual->iOpacity, opacity);
            TPckg<TAlfTimedValue> resultPckg(opacity);
            
            aResponse = resultPckg; // awkward     
            break;
            }
        
        case  EAlfVisualLocalToDisplay:
            {
            TAlfRealPoint* alfPoint = (TAlfRealPoint*) aInputBuffer.Ptr();
            const THuiRealPoint realPoint( alfPoint->iX, alfPoint->iY );
            
            THuiRealPoint result = iVisual->LocalToDisplay( realPoint );

            // if the visual is showing, the container layout rect is included
            // in the LocalToDisplay calculations
            if ( iVisual->Display() )
                {
                // if not showing, we need to add the display rect offset.
                CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
                result -= session.ClientDrawingArea().iTl;
                }
            
            TAlfRealPoint alf_point(result.iX, result.iY);
            TPckg<TAlfRealPoint> resultPckg(alf_point);            
            
            aResponse = resultPckg; // awkward 
            break;     
            }
        
        case  EAlfVisualDisplayToLocal:
            {
            TAlfRealPoint* alfPoint = (TAlfRealPoint*) aInputBuffer.Ptr();
            THuiRealPoint realPoint( alfPoint->iX, alfPoint->iY );
            
            // if the visual is showing, the container layout rect is included
            // in the DisplayToLocal calculations
            if ( iVisual->Display() )
                {
                // if not showing, we need to reduce the display rect offset
                // from the comparision value.
                CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
                realPoint += session.ClientDrawingArea().iTl;
                }
            
            THuiRealPoint result = iVisual->DisplayToLocal( realPoint );
                        
            TAlfRealPoint alf_point(result.iX, result.iY);
            TPckg<TAlfRealPoint> resultPckg(alf_point);            

            aResponse = resultPckg; // awkward 
            break;     
            }
        
        case  EAlfVisualDisplayRectTarget:
            {
            THuiRealRect huiResult = iVisual->DisplayRectTarget();
            
            if ( iVisual->Display() )
                {
                CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
                huiResult.Move( -session.ClientDrawingArea().iTl.iX, -session.ClientDrawingArea().iTl.iY ); 
                }
            
            TAlfRealRect alfResult;
            alfResult.iTl.iX = huiResult.iTl.iX;
            alfResult.iTl.iY = huiResult.iTl.iY;
            alfResult.iBr.iX = huiResult.iBr.iX;
            alfResult.iBr.iY = huiResult.iBr.iY;
            TPckg<TAlfRealRect> resultPckg(alfResult);
            
            aResponse = resultPckg; // awkward 
            break;            
            }
        
        case  EAlfVisualPos:
            {
            TAlfTimedPoint result;
            AlfTimedPointUtility::CopyTimedPoint(iVisual->Pos(), result);             
            
            TPckg<TAlfTimedPoint> resultPckg(result);

            aResponse = resultPckg; // awkward 
            break;            
            }
        
        case  EAlfVisualSetPos:
            {
            TAlfVisualPos* pos = (TAlfVisualPos*) aInputBuffer.Ptr();
            
            THuiRealPoint huiPoint( pos->iPoint.iX, pos->iPoint.iY );
            
            iVisual->SetPos(huiPoint, pos->iTime);
            break;            
            }
            
        
        case EAlfVisualMoveToFront:
            { 
            iVisual->MoveToFront();
            break;            
            }
        
        case EAlfVisualSize:
            {
            TAlfTimedPoint a_size;
            AlfTimedPointUtility::CopyTimedPoint(iVisual->Size(), a_size);             

            TPckg<TAlfTimedPoint> sizePckg(a_size);
            
            aResponse = sizePckg; // awkward 
            break;            
            }
        
        case EAlfVisualSetSize:
            {
            TAlfVisualSize* size = (TAlfVisualSize*) aInputBuffer.Ptr();
            iVisual->SetSize(THuiRealSize( size->iSize.iWidth,size->iSize.iHeight ) , size->iTime);

            break;            
            }
            
        case EAlfVisualBrushArrayReset:
            {
            if ( iVisual ) // during destruction the iVisual is already deleted
                {
                iVisual->EnableBrushesL( EFalse );
                }
            break;
            }
            

        case EAlfVisualBrushArrayAppend:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            
            const THuiOwnership ownership = (THuiOwnership)params->iInt2;
            
            CHuiBrush* brush = (CHuiBrush*)iResolver.GetInterfaceL(EHuiObjectTypeBrush, params->iInt1);
            iVisual->EnableBrushesL();
            iVisual->Brushes()->AppendL(brush,ownership);
            CAlfBrushHandler* holder = (CAlfBrushHandler*) iResolver.GetInterfaceL(EAlfBrushHandler, params->iInt1);
            
            // If the brush array is taking the ownership...
            if ( ownership == EHuiHasOwnership )
                {
                // ... the brush holder cannot have it anymore
                holder->SetOwnership(EFalse);
                }
            break;
            }
        case EAlfVisualBrushArrayInsert:
            {
            TInt3* params = (TInt3*) aInputBuffer.Ptr();
            
            CHuiBrush* brush = (CHuiBrush*)iResolver.GetInterfaceL(EHuiObjectTypeBrush, params->iInt1);
            iVisual->EnableBrushesL();
            iVisual->Brushes()->InsertL(params->iInt3, brush,(THuiOwnership)params->iInt2);
            CAlfBrushHandler* holder = (CAlfBrushHandler*) iResolver.GetInterfaceL(EAlfBrushHandler, params->iInt1);
            holder->SetOwnership(!params->iInt2); // THuiOwnership matches boolean 
            break;
            }
        case EAlfVisualBrushArrayRemove:
            {
            if ( iVisual ) // iVisual is NULL:led i.e. deleted
                {
                TInt* params = (TInt*) aInputBuffer.Ptr();
                iVisual->EnableBrushesL();
                iVisual->Brushes()->Remove(*params);
                }
            break;
            }
            
        case EAlfVisualSetFlag:
            {
            THuiVisualFlags* params = (THuiVisualFlags*) aInputBuffer.Ptr();
            iVisual->SetFlag( *params );
            break;
            }
            
        case EAlfVisualSetFlags:
            {
            TUint* params = (TUint*) aInputBuffer.Ptr();
            iVisual->SetFlags( *params );
            break;
            }
            
        case EAlfVisualClearFlag:
            {
            THuiVisualFlags* params = (THuiVisualFlags*) aInputBuffer.Ptr();
            iVisual->ClearFlag( *params );
            break;
            }
            
        case EAlfVisualClearFlags:
            {
            TUint* params = (TUint*) aInputBuffer.Ptr();
            iVisual->ClearFlags( *params );
            break;
            }
            
        case EAlfVisualFlags:
            {
            const TUint flags = iVisual->Flags();
            TPckg<TUint> flagsPckg(flags);
            aResponse = flagsPckg; // awkward 
            break;
            }
            
        case EAlfVisualSetPaddingInt:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();
            iVisual->SetPadding( *params );
            break;
            }
            
        case EAlfVisualSetPaddingPoint:
            {
            TPoint* params = (TPoint*) aInputBuffer.Ptr();
            iVisual->SetPadding( *params );
            break;
            }
            
        case EAlfVisualSetPaddingBox:
            {
            TAlfBoxMetric* metric = (TAlfBoxMetric*) aInputBuffer.Ptr();
                    
            THuiMetric newMetricLeft(metric->iLeft.iMagnitude, metric->iLeft.iUnit, metric->iLeft.iReferenceTextStyleId);
            THuiMetric newMetricRight(metric->iRight.iMagnitude, metric->iRight.iUnit, metric->iRight.iReferenceTextStyleId);
            THuiMetric newMetricTop(metric->iTop.iMagnitude, metric->iTop.iUnit, metric->iTop.iReferenceTextStyleId);
            THuiMetric newMetricBottom(metric->iBottom.iMagnitude, metric->iBottom.iUnit, metric->iBottom.iReferenceTextStyleId);
            
            THuiBoxMetric newBoxMetric(newMetricLeft, newMetricRight, newMetricTop, newMetricBottom);
            iVisual->SetPadding(newBoxMetric);
            break;
            }
            
        case EAlfVisualPaddingInPixels:
            {
            const THuiRealRect huiResult = iVisual->PaddingInPixels();  
            
            TAlfRealRect alfResult;
            alfResult.iTl.iX = huiResult.iTl.iX;
            alfResult.iTl.iY = huiResult.iTl.iY;
            alfResult.iBr.iX = huiResult.iBr.iX;
            alfResult.iBr.iY = huiResult.iBr.iY;
            
            TPckg<TAlfRealRect> resultPckg(alfResult);           
            aResponse = resultPckg; // awkward 
            break;    
        }
        
        case EAlfVisualEnableTransformation:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            
            const TBool isTransformed = params->iInt2;
            const TInt transHandle = params->iInt1;
            
            
            // @todo: nasty hack
            CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
            CAlfSrvSubSessionBase& transSubSession = session.SubSession( transHandle );
            CAlfSrvTransformationSubSession& transformationSubSession = 
                static_cast<CAlfSrvTransformationSubSession&>(transSubSession);
            
            // Enable transformation
            iVisual->EnableTransformationL( isTransformed );
            CHuiTransformation* transformation = NULL;
            if ( isTransformed )
                {
                transformation = &iVisual->Transformation();
                }
                
            transformationSubSession.SetTransformation( transformation );
            break;
            }

        case EAlfVisualDepthOffset:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(iVisual->iDepthOffset, value);             

            TPckg<TAlfTimedValue> resultPckg(value);            
            aResponse = resultPckg; // awkward                 
            break;    
            }

        case EAlfVisualSetDepthOffset:
            {
            TAlfTimedValue* params = (TAlfTimedValue*) aInputBuffer.Ptr();            
            AlfTimedValueUtility::CopyTimedValue(*params, iVisual->iDepthOffset, iResolver);
            break;    
            }

        case EAlfVisualSetMinSize:
            {
            TSize* params = (TSize*) aInputBuffer.Ptr();
            iVisual->SetMinSize( *params );            
            break;    
            }

        case EAlfVisualGetMinSize:
            {
            TSize result = iVisual->MinSize();            
            TPckg<TSize> resultPckg(result);
            aResponse = resultPckg; // awkward 
            break;    
            }

        case EAlfVisualSetMinSizeInBaseUnits:
            {
            TAlfRealSize* params = (TAlfRealSize*) aInputBuffer.Ptr();
            const THuiRealSize minSize(params->iWidth, params->iHeight);
            iVisual->SetMinSize( minSize );            
            break;    
            }

        case EAlfVisualGetMinSizeInBaseUnits:
            {
            THuiRealSize result = iVisual->MinSizeInBaseUnits(); 
            TAlfRealSize alfResult(result.iWidth, result.iHeight);  
            TPckg<TAlfRealSize> resultPckg(alfResult);
            aResponse = resultPckg; // awkward 
            break;    
            }

        case EAlfVisualSetMaxSize:
            {            
            TSize* params = (TSize*) aInputBuffer.Ptr();
            iVisual->SetMaxSize( *params );            
            break;    
            }

        case EAlfVisualGetMaxSize:
            {
            TSize result = iVisual->MaxSize();            
            TPckg<TSize> resultPckg(result);
            aResponse = resultPckg; // awkward 
            break;    
            }

        case EAlfVisualSetMaxSizeInBaseUnits:
            {            
            TAlfRealSize* params = (TAlfRealSize*) aInputBuffer.Ptr();
            const THuiRealSize maxSize(params->iWidth, params->iHeight);
            iVisual->SetMaxSize( maxSize );            
            break;    
            }

        case EAlfVisualGetMaxSizeInBaseUnits:
            {
            THuiRealSize result = iVisual->MaxSizeInBaseUnits(); 
            TAlfRealSize alfResult(result.iWidth, result.iHeight);  
            TPckg<TAlfRealSize> resultPckg(alfResult);
            aResponse = resultPckg; // awkward 
            break;    
            }

        case EAlfVisualMove:
            {            
            TAlfVisualPos* pos = (TAlfVisualPos*) aInputBuffer.Ptr();           
            const THuiRealPoint huiPoint(pos->iPoint.iX, pos->iPoint.iY);
            iVisual->Move(huiPoint, pos->iTime);
            break;            
            }

       case EAlfVisualSetCenteredPosAndSize:
            {            
            TAlfVisualPosSize* posAndSize = (TAlfVisualPosSize*) aInputBuffer.Ptr();            
            const THuiRealPoint huiPoint(posAndSize->iPoint.iX, posAndSize->iPoint.iY);
            const THuiRealSize huiSize(posAndSize->iSize.iWidth, posAndSize->iSize.iHeight);
            iVisual->SetCenteredPosAndSize(huiPoint, huiSize, posAndSize->iTime);
            break;            
            }

        case EAlfVisualDisplayRect:
            {          
            THuiRealRect huiResult = iVisual->DisplayRect();
            
            if ( iVisual->Display() )
                {
                CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
                huiResult.Move( -session.ClientDrawingArea().iTl.iX, -session.ClientDrawingArea().iTl.iY );
                }
            
            TAlfRealRect alfResult;
            alfResult.iTl.iX = huiResult.iTl.iX;
            alfResult.iTl.iY = huiResult.iTl.iY;
            alfResult.iBr.iX = huiResult.iBr.iX;
            alfResult.iBr.iY = huiResult.iBr.iY;
            
            TPckg<TAlfRealRect> resultPckg(alfResult);           
            aResponse = resultPckg; // awkward 
            break;    
            }

        case EAlfVisualEffectiveOpacity:
            {
            TReal32 value = iVisual->EffectiveOpacity();
            TPckg<TReal32> resultPckg(value);            
            aResponse = resultPckg; // awkward                 
            break;    
            }
        case EAlfVisualSetPosTimed:
            {
            TAlfTimedPoint* params = (TAlfTimedPoint*) aInputBuffer.Ptr();                        
            AlfTimedValueUtility::CopyTimedValue(params->iX, iVisual->Pos().iX, iResolver);
            AlfTimedValueUtility::CopyTimedValue(params->iY, iVisual->Pos().iY, iResolver);
            break;        
            }            
        case EAlfVisualSetSizeTimed:            
            {
            TAlfTimedPoint* params = (TAlfTimedPoint*) aInputBuffer.Ptr();                        
            AlfTimedValueUtility::CopyTimedValue(params->iX, iVisual->Size().iX, iResolver);
            AlfTimedValueUtility::CopyTimedValue(params->iY, iVisual->Size().iY, iResolver);
            break;        
            }
            
        case EAlfVisualLayoutUpdatedNotification:
            {
            if ( !iPrivateData->iVisualOwner )
                {
                // @todo: nasty hack
                CAlfAppSrvSessionBase& session = static_cast<CAlfAppSrvSessionBase&>( iResolver );
                iPrivateData->iVisualOwner = CAlfSrvVisualOwner::NewL( session, &iVisual->Owner() );
                iVisual->SetOwner( *iPrivateData->iVisualOwner );
                }
            
            TInt commandId = iResolver.HandleCurrentCommanndAsynch();
            iPrivateData->iVisualOwner->SetVisualLayoutUpdatedNotificationId( commandId );
            break;
            }
        case EAlfSubSCancelAsynchRequest:
            {
            TInt2* inptr =  (TInt2*) aInputBuffer.Ptr();
            if (inptr->iInt2 != EAlfVisualLayoutUpdatedNotification )
                { 
                // strange one, someone tries to cancel something we don't support
                // we shouldn't leave on cancel though
                break;
                }
            // else let fall through    
            }
        case EAlfVisualLayoutUpdatedNotificationCancel:
            {
            if ( iPrivateData->iVisualOwner )
                {
                if ( iVisual ) // If the visual has not been deleted already
                    {
                    iVisual->SetOwner( *iPrivateData->iVisualOwner->RealOwner() );
                    }
                delete iPrivateData->iVisualOwner;
                iPrivateData->iVisualOwner = NULL;
                }    
            break;
            }

        case EAlfVisualCopyValuesFromOtherVisual:
            {
            TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
            CHuiVisual* sourceVisual = (CHuiVisual*) iResolver.GetInterfaceL( EHuiObjectTypeVisual, inptr->iInt1 );
            if (inptr->iInt2&AlfVisualProperties::ESize)
                {
                THuiRealPoint gotThePoint = sourceVisual->Size().RealTarget(); 
                THuiRealSize rs(gotThePoint.iX, gotThePoint.iY);
                iVisual->SetSize(rs, inptr->iInt3 );
                }
            if (inptr->iInt2&AlfVisualProperties::EPosition)
                { 
                THuiRealPoint gotThePoint = sourceVisual->Pos().RealTarget(); 
                THuiRealPoint rPoint(gotThePoint.iX, gotThePoint.iY);
                iVisual->SetPos(rPoint, inptr->iInt3);
                }
            if (inptr->iInt2&AlfVisualProperties::EOpacity)
                {
                iVisual->iOpacity.Set(sourceVisual->iOpacity.Target(), inptr->iInt3); 
                }
            if (inptr->iInt2&AlfVisualProperties::EDepthOffset)
                {
                iVisual->iDepthOffset.Set( sourceVisual->iDepthOffset.Target(), inptr->iInt3);
                }
            break;
            }

        case EAlfVisualPauseProperties:
            {
            TInt* inptr =  (TInt*) aInputBuffer.Ptr();
            if (*inptr&AlfVisualProperties::ESize)
                {
                iVisual->Size().iX.Suspend(); 
                iVisual->Size().iY.Suspend(); 
                }
            if (*inptr&AlfVisualProperties::EPosition)
                { 
                iVisual->Pos().iX.Suspend(); 
                iVisual->Pos().iY.Suspend(); 
                }
            if (*inptr&AlfVisualProperties::EOpacity)
                {
                iVisual->iOpacity.Suspend(); 
                }
            if (*inptr&AlfVisualProperties::EDepthOffset)
                {
                iVisual->iDepthOffset.Suspend();
                }
            break;
            }

        case EAlfVisualResumeProperties:
            {
            TInt* inptr =  (TInt*) aInputBuffer.Ptr();
            if (*inptr&AlfVisualProperties::ESize)
                {
                iVisual->Size().iX.Suspend(EFalse); 
                iVisual->Size().iY.Suspend(EFalse); 
                }
            if (*inptr&AlfVisualProperties::EPosition)
                { 
                iVisual->Pos().iX.Suspend(EFalse); 
                iVisual->Pos().iY.Suspend(EFalse); 
                }
            if (*inptr&AlfVisualProperties::EOpacity)
                {
                iVisual->iOpacity.Suspend(EFalse); 
                }
            if (*inptr&AlfVisualProperties::EDepthOffset)
                {
                iVisual->iDepthOffset.Suspend(EFalse);
                }
            break;
            }
            
        case EAlfVisualSetTactileFeedback:
            {
            TInt2* value = (TInt2*) aInputBuffer.Ptr();
            iVisual->SetTactileFeedbackL( value->iInt1, value->iInt2 );
            }
            break;
            
        case EAlfVisualHasTactileFeedback:
            {
            TInt* value = (TInt*) aInputBuffer.Ptr();
            TBool answer = iVisual->HasTactileFeedback( *value );
            TPckg<TBool> resultPckg(answer);           
            aResponse = resultPckg;
            }
            break;
            
        case EAlfVisualRemoveTactileFeedback:
            {
            TInt* value = (TInt*) aInputBuffer.Ptr();
            iVisual->RemoveTactileFeedback( *value );
            }
            break;
            
        case EAlfVisualEnableDropShadow:
            {
            const TBool* const enable = (TBool*) aInputBuffer.Ptr();
            iVisual->EnableDropShadowL( *enable );
            }
            break;
            
        case EAlfVisualSetEffect:
            {
            // TODO: handle errors more gracefully
            // TODO: effect merging
            TAlfVisualEffectParams* params = (TAlfVisualEffectParams*)aInputBuffer.Ptr();
            CHuiFxEngine* engine = iVisual->Env().EffectsEngine();
            if (engine)
                {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
                TTime time;
                time.UniversalTime();
                AMT_ADD_TIME(params->iHandle, time.Int64(), ETrue);
#endif  
                CHuiFxEffect* effect = NULL;
                // Note: alf effects do not support taking screenshot 
				// Note: alf effects do not support freezing.
				engine->LoadEffectL( params->iFileName, effect, iVisual->Effectable(), NULL , NULL, params->iHandle, KHuiFxDelayRunUntilFirstFrameHasBeenDrawn );
	            iVisual->SetFreezeState(EFalse); 
                // The effect will be automatically set to the visual if parsing succeeds
                }
            break;
            }
        case EAlfVisualSetGroupEffect:
            {
            TAlfVisualEffectParams* params = (TAlfVisualEffectParams*)aInputBuffer.Ptr();
            TInt groupHandle = params->iGroupHandle;
            CHuiFxEngine* engine = iVisual->Env().EffectsEngine();
            if (engine)
                {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
                TTime time;
                time.UniversalTime();
                AMT_ADD_TIME(params->iHandle, time.Int64(), ETrue);
#endif  
                CHuiFxEffect* effect = NULL;
                // this will add the group, if it does not exist already
                // Begin and End group events are supposed to come through GfxTransEffect API.
                engine->BeginGroupEffect(groupHandle);
                // Note: alf effects do not support taking screenshot 
				// Note: alf effects do not support freezing.
                engine->LoadGroupEffectL( params->iFileName, effect, iVisual->Effectable(),groupHandle, NULL, NULL, params->iHandle, KHuiFxDelayRunUntilFirstFrameHasBeenDrawn );
	            iVisual->SetFreezeState(EFalse);
                // The effect will be automatically set to the visual if parsing succeeds
                }
            break;
            }            
        case EAlfVisualRemoveEffect:
            {
            iVisual->SetEffect(NULL);
            iVisual->SetFreezeState(EFalse); // Note: alf effects do not support freezing.
            break;
            }

        default:
            
            if ( AlfSrvDropShadowHandler::CanHandleCommand( aCommandId ) )
                {
                CHuiDropShadow* shadow = iVisual->DropShadowHandler();
                
                // Drop shadow should be enabled first
                __ASSERT_DEBUG( shadow , User::Leave( KErrGeneral ) );
                
                if ( !shadow )
                    {
                    iVisual->EnableDropShadowL( ETrue );
                    shadow = iVisual->DropShadowHandler();
                    }
                
                AlfSrvDropShadowHandler::HandleCommandL( *shadow, aCommandId, aInputBuffer, aResponse );
                break;
                }
        
            User::Leave(KErrNotSupported);
        }
    }

EXPORT_C void CAlfVisualHandler::VisualHandlerExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    }

//----------------------------
// TextVisual
//----------------------------
EXPORT_C MAlfExtension* CAlfTextVisualHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfTextVisualHandler* me = new (ELeave) CAlfTextVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfTextVisualHandler::CAlfTextVisualHandler(MAlfInterfaceProvider& aResolver):CAlfVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfTextVisualHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiTextVisual* textVisual = CHuiTextVisual::AddNewL(aOwner, aParentLayout); 
        CAlfVisualHandler::ConstructL(textVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfTextVisualHandler::~CAlfTextVisualHandler()
    {
    }

EXPORT_C void CAlfTextVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfTextVisualHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfVisualHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfTextVisualHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiTextVisual* visual = static_cast<CHuiTextVisual*>(iVisual);
    
    switch (aCommandId)
        {
        case  EAlfTextVisualSetText:           
            {
            TPtrC text((const TUint16*)aInputBuffer.Ptr(), aInputBuffer.Length()/2);                        
            visual->SetTextL(text);
            break;                
            }
        
        case  EAlfTextVisualSetStyle:
            {      
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            visual->SetStyle( (THuiPreconfiguredTextStyle)params->iInt1,(THuiBackgroundType)params->iInt2 );
            break;    
            }
        
        case  EAlfTextVisualSetTextStyle:
            {
    		// Convert text style id to server domain
            TInt* value = (TInt*) aInputBuffer.Ptr();            	
    		TAlfTextStyleHandler* textStyleHandler = static_cast<TAlfTextStyleHandler*>(iResolver.GetInterfaceL(EAlfTextStyleHandler, *value));    		
            visual->SetTextStyle(textStyleHandler->TextStyleId());
            break;    
            }
        
        case  EAlfTextVisualSetEffects:
            {
            // This is for future expansion, unimplemented.
            break;    
            }
        
        case EAlfTextVisualSetAlign:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            visual->SetAlign( (THuiAlignHorizontal)params->iInt1,(THuiAlignVertical)params->iInt2 );
            break;
            }
            
        case EAlfTextVisualSetLineSpacing:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            visual->SetLineSpacing( params->iInt1, (CHuiTextVisual::TLineSpacingUnits)params->iInt2 );
            break;
            }
            
        case EAlfTextVisualTextExtents:
            {
            const TSize size = visual->TextExtents();
            TPckg<TSize> resultPckg(size);            
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfTextVisualSubstringExtents:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();            
            const TRect size = visual->SubstringExtents(params->iInt1, params->iInt2);
            TPckg<TRect> resultPckg(size);            
            aResponse = resultPckg; // awkward 
            break;
            }
                        
        case EAlfTextVisualStyle:
            {
            const TInt2 result( 
                visual->Style(),
                visual->BackgroundType() );
            
            
            TPckg<TInt2> resultPckg(result);            
            aResponse = resultPckg; // awkward 
            break;
            }
                                            
        case EAlfTextVisualSetMaxLineCount:
            {
            TInt* value = (TInt*) aInputBuffer.Ptr();
            visual->SetMaxLineCount(*value);
            break;
            }

        case EAlfTextVisualMaxLineCount:
            {
            const TInt value = visual->MaxLineCount();
            TPckg<TInt> resultPckg(value);            
            aResponse = resultPckg; // awkward 
            break;
            }

        case EAlfTextVisualWrapping:
            {
            // Client side uses CAlfTextVisual::TLineWrap, should be compatible.
            CHuiTextVisual::TLineWrap value = visual->Wrapping();
            TPckg<CHuiTextVisual::TLineWrap> resultPckg(value);            
            aResponse = resultPckg; // awkward 
            break;
            }

        case EAlfTextVisualSetWrapping:
            {
            CHuiTextVisual::TLineWrap* value = (CHuiTextVisual::TLineWrap*) aInputBuffer.Ptr();
            visual->SetWrapping(*value);
            break;
            }

        /* deprecated
        case EAlfTextVisualShadowOpacity:
        case EAlfTextVisualSetShadowOpacity:        
        */
            
        case EAlfTextVisualSetColor:        
            {
            TAlfTextVisualFontColorParams* value = (TAlfTextVisualFontColorParams*) aInputBuffer.Ptr();
            if (value->iId == KAknsIIDNone)
                {
                visual->SetColor(value->iColor);                        
                }
            else
                {
                visual->SetColor(value->iId, value->iIndex);                                            
                }    
            break;
            }
            
        case EAlfTextVisualOffset:        
            {
            TAlfTimedPoint value;
            AlfTimedPointUtility::CopyTimedPoint(visual->Offset(), value);
            TPckg<TAlfTimedPoint> resultPckg(value);            
            aResponse = resultPckg; // awkward 
            break;
            }            
        case EAlfTextVisualSetOffset:
            {
            TAlfTimedPoint* value = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*value, visual->Offset(), iResolver);
            break;
            }
        case EAlfTextVisualSetHighlightRange:
        	{
			TAlfTextVisualSetHighlightRangeParams* value = (TAlfTextVisualSetHighlightRangeParams*) aInputBuffer.Ptr();        	
			visual->SetHighlightRange(value->iStart, value->iEnd, value->iHighlightColor, value->iHighlightTextColor);
			break;
        	}    

        /* deprecated
        case EAlfTextVisualEnableShadow:        
        */       
        case EAlfTextVisualSetRasterizedMesh:
            {
        	visual->UpdateMeshL(aInputBuffer);
        	break;
            }
        	
        case EAlfVisualCopyValuesFromOtherVisual:
        case EAlfVisualPauseProperties:
        case EAlfVisualResumeProperties:        
            {
            if (aCommandId == EAlfVisualCopyValuesFromOtherVisual)
                {
                TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
                CHuiTextVisual* sourceVisual = (CHuiTextVisual*) iResolver.GetInterfaceL( EHuiObjectTypeVisual, inptr->iInt1 );
                if (inptr->iInt2&AlfVisualProperties::ETextVisShadowOpacity)
                    {
                    visual->iShadow.Set(sourceVisual->iShadow.Target(), inptr->iInt3 );
                    }
                }
            else if (aCommandId == EAlfVisualPauseProperties)
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ETextVisShadowOpacity)
                    {
                    visual->iShadow.Suspend();
                    }
                }
            else  // AlfVisualResumeProperties
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ETextVisShadowOpacity)
                    {
                    visual->iShadow.Suspend(EFalse);
                    }
                }
            } // FALL THROUGH !!
        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfTextVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }

 
//----------------------------
// LCTTextVisual
//----------------------------
EXPORT_C MAlfExtension* CAlfLCTTextVisualHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfLCTTextVisualHandler* me = new (ELeave) CAlfLCTTextVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfLCTTextVisualHandler::CAlfLCTTextVisualHandler(MAlfInterfaceProvider& aResolver):CAlfTextVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfLCTTextVisualHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiTextVisual* textVisual = CHuiTextVisual::AddNewL(aOwner, aParentLayout); 
        CAlfVisualHandler::ConstructL(textVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfLCTTextVisualHandler::~CAlfLCTTextVisualHandler()
    {
    }


EXPORT_C void CAlfLCTTextVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfLCTTextVisualHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfTextVisualHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfLCTTextVisualHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    switch (aCommandId)
        {
        case  EAlfLCTTextVisualSetTextPane:
            {
#ifdef RD_ALF_IN_PLATFORM
            CHuiTextVisual* visual = static_cast<CHuiTextVisual*>(iVisual);
            TAlfLCTTextVisualSetTextPaneParams* params = (TAlfLCTTextVisualSetTextPaneParams*)aInputBuffer.Ptr();

            TAknLayoutHierarchyComponentHandle handle;
            handle.SetApiId(params->iApiId);
            handle.SetComponentId(params->iComponentId);
            handle.SetVarietyIndex(params->iVarietyIndex);
            handle.SetColumn(params->iColumn);
            handle.SetRow(params->iRow);
    
            // we can access the layout data as a text line, as we are expecting it to be a text pane
            TAknTextLineLayout componentLayout = AknLayout2Hierarchy::GetTextComponentLayout(handle).LayoutLineNoEmptys();
            SetTextPaneL(visual, componentLayout);
#endif // RD_ALF_IN_PLATFORM

            break;                
            }
            
        default:
            CAlfTextVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

#ifdef RD_ALF_IN_PLATFORM
void CAlfLCTTextVisualHandler::SetTextPaneL(CHuiTextVisual* aTextVisual, const TAknTextLineLayout& aLine)
    {
    THuiAlignHorizontal alignHorizontal(EHuiAlignHLocale);
    switch(aLine.iJ)
        {
        case ELayoutAlignNone:
            alignHorizontal = EHuiAlignHLocale;
            break;
        case ELayoutAlignCenter:
            alignHorizontal = EHuiAlignHCenter;
            break;
        case ELayoutAlignLeft:
            alignHorizontal = EHuiAlignHLeft;
            break;
        case ELayoutAlignRight:
            alignHorizontal = EHuiAlignHRight;
            break;
        case ELayoutAlignBidi:
            alignHorizontal = EHuiAlignHBidirectionalText;
            break;
        default:
            alignHorizontal = EHuiAlignHLocale;
            break;
        }
  
    aTextVisual->SetAlign(alignHorizontal, EHuiAlignVCenter);// always vertical center, as anchor will have been already set to text pane height

    // Create a platform text style
    CHuiTextStyleManager& manager = iResolver.SharedHuiEnv()->TextStyleManager();
    TInt textStyleId = manager.CreatePlatformTextStyleL(aLine.iFont);
    aTextVisual->SetTextStyle(textStyleId);
    }
#else // RD_ALF_IN_PLATFORM
void CAlfLCTTextVisualHandler::SetTextPaneL(CHuiTextVisual* /*aTextVisual*/, const TAknTextLineLayout& /*aLine*/)
    {
    
    }
#endif // RD_ALF_IN_PLATFORM
    

EXPORT_C void CAlfLCTTextVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }


//----------------------------
// ImageVisual
//----------------------------

EXPORT_C MAlfExtension* CAlfImageVisualHandler::NewL(MAlfInterfaceProvider& aResolver, CHuiControl* aOwner, CHuiLayout* aParentLayout)
    {
    CAlfImageVisualHandler* me = new (ELeave) CAlfImageVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfImageVisualHandler::CAlfImageVisualHandler(MAlfInterfaceProvider& aResolver):CAlfVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfImageVisualHandler::ConstructL(CHuiVisual* aVisual, CHuiControl& aOwner, CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiImageVisual* imageVisual = CHuiImageVisual::AddNewL(aOwner, aParentLayout); 
        CAlfVisualHandler::ConstructL(imageVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfImageVisualHandler::~CAlfImageVisualHandler()
    {
    }

EXPORT_C void CAlfImageVisualHandler::Release()
    {
    delete this;    
    }


EXPORT_C TAny* CAlfImageVisualHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    return CAlfVisualHandler::GetInterface(aInterface);    
    }


EXPORT_C void CAlfImageVisualHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    CHuiImageVisual* visual = static_cast<CHuiImageVisual*>(iVisual);
    
    switch (aCommandId)
        {        
        case  EAlfImageVisualSetImage:           
            {
        	TAlfImageParams* params = (TAlfImageParams*) aInputBuffer.Ptr();
        	
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iTextureHandle;
        	MHuiTexture* texture = (MHuiTexture*)handle;
   	        
   	        THuiImage huiImage;
   	        if (texture)
   	            {
   	            huiImage = THuiImage(*texture,//*(dynamic_cast<MHuiTexture*>(animgif)), 
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
   	        
	        visual->SetImage( huiImage );
            break;                
            }
        
        case  EAlfImageVisualSetScaleMode:           
            {
            TInt* mode = (TInt*) aInputBuffer.Ptr();
        	visual->SetScaleMode(CHuiImageVisual::TScaleMode(*mode));
            break;                
            }
        
        case  EAlfImageVisualSetSecondaryImage:           
            {
        	TAlfImageParams* params = (TAlfImageParams*) aInputBuffer.Ptr();
        	
            // Todo: is is safe enough to handle textures as direct casts..
        	TInt handle = params->iTextureHandle;
        	MHuiTexture* texture = (MHuiTexture*)(handle);
   	        
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

	        visual->SetSecondaryImage( huiImage );
            break;                
            }
        
        case  EAlfImageVisualSetSecondaryAlpha:           
            {
            TAlfTimedValue* alpha = (TAlfTimedValue*) aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue(*alpha, visual->iSecondaryAlpha, iResolver);
            break;                
            }

        case  EAlfImageVisualSecondaryAlpha:           
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(visual->iSecondaryAlpha, value);             

            TPckg<TAlfTimedValue> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
        
        case  EAlfImageVisualSetColor:           
            {
            TRgb* color = (TRgb*) aInputBuffer.Ptr();
        	visual->SetColor(*color);
            break;                
            }
        
        case  EAlfImageVisualSetColorMode:           
            {
            TIntTReal* mode = (TIntTReal*) aInputBuffer.Ptr();
        	visual->SetColorMode(CHuiImageVisual::TColorMode(mode->iInt),mode->iReal);
            break;                
            }
        
        case  EAlfImageVisualSetStretch:           
            {
            TInt2* mode = (TInt2*) aInputBuffer.Ptr();
        	visual->SetStretch(mode->iInt1,mode->iInt2);
            break;                
            }
        
        case  EAlfImageVisualSetStretchMode:           
            {
            CHuiGc::TStretchMode* mode = (CHuiGc::TStretchMode*) aInputBuffer.Ptr();
        	visual->SetStretchMode(*mode);
            break;                
            }
        
        /* deprecated
        case  EAlfImageVisualSetDropShadow:           
        case  EAlfImageVisualSetDropShadowMetric:
        */
            
        case EAlfImageVisualSetTurnAngle:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue(*value, visual->iTurnAngle, iResolver);
            break;    
            }
        case  EAlfImageVisualTurnAngle:           
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(visual->iTurnAngle, value);             
            TPckg<TAlfTimedValue> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
        case EAlfImageVisualSetScale:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue(*value, visual->iScale, iResolver);
            break;    
            }
        case  EAlfImageVisualScale:           
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(visual->iScale, value);             
            TPckg<TAlfTimedValue> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
        case EAlfImageVisualSetOffset:
            {
            TAlfTimedPoint* value = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedValueUtility::CopyTimedValue(value->iX, visual->iOffset.iX, iResolver);
            AlfTimedValueUtility::CopyTimedValue(value->iY, visual->iOffset.iY, iResolver);
            break;    
            }
        case  EAlfImageVisualOffset:           
            {
            TAlfTimedPoint value;
            AlfTimedPointUtility::CopyTimedPoint(visual->iOffset, value);             
            TPckg<TAlfTimedPoint> resultPckg(value);            
            aResponse = resultPckg; 
            break;
            }
        case EAlfVisualCopyValuesFromOtherVisual:
        case EAlfVisualPauseProperties:
        case EAlfVisualResumeProperties:        
            {
            if (aCommandId == EAlfVisualCopyValuesFromOtherVisual)
                {
                TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
                CHuiImageVisual* sourceVisual = (CHuiImageVisual*) iResolver.GetInterfaceL( EHuiObjectTypeVisual, inptr->iInt1 );
                if (inptr->iInt2&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    visual->iSecondaryAlpha.Set(sourceVisual->iSecondaryAlpha.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::EPosOffset)
                    {
                    visual->iOffset.iY.Set(sourceVisual->iOffset.iY.Target(), inptr->iInt3); 
                    visual->iOffset.iX.Set(sourceVisual->iOffset.iX.Target(), inptr->iInt3); 
                    }
                if (inptr->iInt2&AlfVisualProperties::EImageVisTurnAngle)
                    {
                    visual->iTurnAngle.Set(sourceVisual->iTurnAngle.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::EScale)
                    {
                    visual->iScale.Set(sourceVisual->iScale.Target(), inptr->iInt3 );
                    }
                }
            else if (aCommandId == EAlfVisualPauseProperties)
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    visual->iSecondaryAlpha.Suspend();
                    }
                if (*inptr&AlfVisualProperties::EPosOffset)
                    {
                    visual->iOffset.iY.Suspend(); 
                    visual->iOffset.iX.Suspend(); 
                    }
                if (*inptr&AlfVisualProperties::EImageVisTurnAngle)
                    {
                    visual->iTurnAngle.Suspend();
                    }
                if (*inptr&AlfVisualProperties::EScale)
                    {
                    visual->iScale.Suspend();
                    }
                }
            else //  EAlfVisualResumeProperties
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    visual->iSecondaryAlpha.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::EPosOffset)
                    {
                    visual->iOffset.iY.Suspend(EFalse); 
                    visual->iOffset.iX.Suspend(EFalse); 
                    }
                if (*inptr&AlfVisualProperties::EImageVisTurnAngle)
                    {
                    visual->iTurnAngle.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::EScale)
                    {
                    visual->iScale.Suspend(EFalse);
                    }
                }
            } // FALL THROUGH !!
        
        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfImageVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }

    
//----------------------------
// Line Visual
//----------------------------
EXPORT_C MAlfExtension* CAlfLineVisualHandler::NewL(
    MAlfInterfaceProvider& aResolver, 
    CHuiControl* aOwner, 
    CHuiLayout* aParentLayout)
    {
    CAlfLineVisualHandler* me = new (ELeave) CAlfLineVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfLineVisualHandler::CAlfLineVisualHandler(
    MAlfInterfaceProvider& aResolver)
    : CAlfVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfLineVisualHandler::ConstructL(
    CHuiVisual* aVisual, 
    CHuiControl& aOwner, 
    CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiLineVisual* lineVisual = 
            CHuiLineVisual::AddNewL(aOwner, aParentLayout); 
        CAlfVisualHandler::ConstructL(lineVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfLineVisualHandler::~CAlfLineVisualHandler()
    {
    }

EXPORT_C void CAlfLineVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfLineVisualHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    CHuiLineVisual* lineVisual = static_cast<CHuiLineVisual*>(iVisual);
    switch (aInterface)
        {
        case EHuiObjectTypeCurvePathProvider:
            return &lineVisual->Path();
        default:
            return CAlfVisualHandler::GetInterface(aInterface);
        }
    }


EXPORT_C void CAlfLineVisualHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiLineVisual* lineVisual = static_cast<CHuiLineVisual*>(iVisual);
    
    switch (aCommandId)
        {
        case EAlfLineVisualSetPath:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            CHuiCurvePath* curvePath = NULL;
            if ( params->iInt1 )
                {
                MHuiMappingFunction* mappingFunction = NULL;
                mappingFunction = static_cast<MHuiMappingFunction*>
                    (iResolver.GetInterfaceL( 
                        EHuiInterfaceMappingFunction, 
                        params->iInt1 ) );
                curvePath = static_cast<CHuiCurvePath*>( mappingFunction );
                }
            
            // The CAlfCurvePathHandler owns the CHuiCurveParh instance always
            lineVisual->SetPath( curvePath , EHuiDoesNotHaveOwnership );
            
            break;
            }
            
        case EAlfLineVisualSetImage:
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
	            
	        lineVisual->SetImage( huiImage );
            break;
            }
            
        case EAlfLineVisualSetAlphaFunction:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();
            MHuiMappingFunction* mappingFunction = NULL;
            if ( *params )
                {
                mappingFunction = static_cast<MHuiMappingFunction*>
                    (iResolver.GetInterfaceL( 
                        EHuiInterfaceMappingFunction, 
                        *params ) );
                }
                
            lineVisual->SetAlphaFunction( mappingFunction );
            break;
            }
            
        case EAlfLineVisualSetWidthFunction:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();
            MHuiMappingFunction* mappingFunction = NULL;
            if ( *params )
                {
                mappingFunction = static_cast<MHuiMappingFunction*>
                    (iResolver.GetInterfaceL( 
                        EHuiInterfaceMappingFunction, 
                        *params ) );
                }
                
            lineVisual->SetWidthFunction( mappingFunction );
            break;
            }
            
        case EAlfLineVisualGetThickness:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(lineVisual->iThickness, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward   
            break;
            }
            
        case EAlfLineVisualSetThickness:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, lineVisual->iThickness, iResolver);
            break;
            }
            
        case EAlfLineVisualGetShadowThickness:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(lineVisual->iShadowThickness, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfLineVisualSetShadowThickness:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, lineVisual->iShadowThickness, iResolver);
            break;
            }
            
        case EAlfLineVisualGetStartPos:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(lineVisual->iStartPos, value);
            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfLineVisualSetStartPos:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, lineVisual->iStartPos, iResolver);
            break;
            }
            
        case EAlfLineVisualGetEndPos:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(lineVisual->iEndPos, value);
            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfLineVisualSetEndPos:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, lineVisual->iEndPos, iResolver);
            break;
            }

        case EAlfLineVisualSetColor:           
            {
            TRgb* color = (TRgb*) aInputBuffer.Ptr();
        	lineVisual->SetColor(*color);
            break;                
            }
        
        case EAlfVisualCopyValuesFromOtherVisual:
        case EAlfVisualPauseProperties:
        case EAlfVisualResumeProperties:        
            {
            if (aCommandId == EAlfVisualCopyValuesFromOtherVisual)
               {
                TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
                CHuiLineVisual* sourceVisual = (CHuiLineVisual*) iResolver.GetInterfaceL( EHuiObjectTypeVisual, inptr->iInt1 );
                if (inptr->iInt2&AlfVisualProperties::ELineVisThickness)
                    {
                    lineVisual->iThickness.Set(sourceVisual->iThickness.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::ELineVisShadowThickness)
                    {
                    lineVisual->iShadowThickness.Set(sourceVisual->iShadowThickness.Target(), inptr->iInt3); 
                    }
                if (inptr->iInt2&AlfVisualProperties::ELineVisStartPos)
                    {
                    lineVisual->iStartPos.Set(sourceVisual->iStartPos.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::ELineVisEndPos)
                    {
                    lineVisual->iEndPos.Set(sourceVisual->iEndPos.Target(), inptr->iInt3 );
                    }
               }
            else if (aCommandId == EAlfVisualPauseProperties)
               {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ELineVisThickness)
                    {
                    lineVisual->iThickness.Suspend();
                    }
                if (*inptr&AlfVisualProperties::ELineVisShadowThickness)
                    {
                    lineVisual->iShadowThickness.Suspend(); 
                    }
                if (*inptr&AlfVisualProperties::ELineVisStartPos)
                    {
                    lineVisual->iStartPos.Suspend();
                    }
                if (*inptr&AlfVisualProperties::ELineVisEndPos)
                    {
                    lineVisual->iEndPos.Suspend();
                    }
               }
            else // EAlfVisualPauseProperties
               {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ELineVisThickness)
                    {
                    lineVisual->iThickness.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::ELineVisShadowThickness)
                    {
                    lineVisual->iShadowThickness.Suspend(EFalse); 
                    }
                if (*inptr&AlfVisualProperties::ELineVisStartPos)
                    {
                    lineVisual->iStartPos.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::ELineVisEndPos)
                    {
                    lineVisual->iEndPos.Suspend(EFalse);
                    }
               }
            
            
            } // FALL THROUGH !!
            
        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfLineVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }

//----------------------------
// Mesh Visual
//----------------------------
EXPORT_C MAlfExtension* CAlfMeshVisualHandler::NewL(
    MAlfInterfaceProvider& aResolver, 
    CHuiControl* aOwner, 
    THuiMeshType aMeshType,
    CHuiLayout* aParentLayout)
    {
    CAlfMeshVisualHandler* me = new (ELeave) CAlfMeshVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aMeshType, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfMeshVisualHandler::CAlfMeshVisualHandler(
    MAlfInterfaceProvider& aResolver)
    : CAlfVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfMeshVisualHandler::ConstructL(
    CHuiVisual* aVisual, 
    CHuiControl& aOwner,
    THuiMeshType aMeshType, 
    CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiMeshVisual* meshVisual = 
            CHuiMeshVisual::AddNewL(aOwner, aMeshType, aParentLayout); 
        CAlfVisualHandler::ConstructL(meshVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfMeshVisualHandler::~CAlfMeshVisualHandler()
    {
    }

EXPORT_C void CAlfMeshVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfMeshVisualHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    return CAlfVisualHandler::GetInterface(aInterface);
    }


EXPORT_C void CAlfMeshVisualHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiMeshVisual* meshVisual = static_cast<CHuiMeshVisual*>(iVisual);
    
    switch (aCommandId)
        {
        case EAlfMeshVisualSetImage:            // flow through
        case EAlfMeshVisualSetSecondaryImage:   // flow through
        case EAlfMeshVisualSetSpecularImage:
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
   	        
   	        if ( aCommandId == EAlfMeshVisualSetImage )
   	            {
   	            meshVisual->SetImage( huiImage );
   	            }
   	        else if ( aCommandId == EAlfMeshVisualSetSecondaryImage )
   	            {
   	            meshVisual->SetSecondaryImage( huiImage );
   	            }
   	        else if ( aCommandId == EAlfMeshVisualSetSpecularImage )
   	            {
   	            meshVisual->SetSpecularImage( huiImage );
   	            }
   	        else
   	            {
   	            // for PC lint
   	            }
            break;
            }
            
        case EAlfMeshVisualCreateMesh:
            {
            THuiMeshType* meshType = (THuiMeshType*)aInputBuffer.Ptr();
            
            // Create mesh for the mesh visual.
            meshVisual->CreateMeshL(*meshType);
            break;
            }
            
        case EAlfMeshVisualSecondaryAlpha:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(meshVisual->iSecondaryAlpha, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward   
            break;
            }
            
        case EAlfMeshVisualSetSecondaryAlpha:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, meshVisual->iSecondaryAlpha, iResolver);
            break;
            }
            
        case EAlfMeshVisualYawAngle:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(meshVisual->iYawAngle, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward   
            break;
            }
            
        case EAlfMeshVisualSetYawAngle:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, meshVisual->iYawAngle, iResolver);
            break;
            }
            
        case EAlfMeshVisualPitchAngle:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(meshVisual->iPitchAngle, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward   
            break;
            }
            
        case EAlfMeshVisualSetPitchAngle:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, meshVisual->iPitchAngle, iResolver);
            break;
            }
            
        case EAlfMeshVisualScale:
            {
            TAlfTimedValue value;
            AlfTimedValueUtility::CopyTimedValue(meshVisual->iScale, value);

            TPckg<TAlfTimedValue> resultPckg(value);
            aResponse = resultPckg; // awkward 
            break;
            }
            
        case EAlfMeshVisualSetScale:
            {
            TAlfTimedValue* value = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*value, meshVisual->iScale, iResolver);
            break;
            }
            
        case EAlfMeshReset:
            {
            meshVisual->Mesh().Reset();
            break;
            }
            
        case EAlfMeshMakeCube:
            {
            RArray<TReal32> params;
            CleanupClosePushL( params );
            InternalizeL(aInputBuffer, params);
              
            CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());
            mesh.MakeCubeL(
                params[0],
                params[1],
                params[2],
                params[3] );
                
            CleanupStack::PopAndDestroy( &params );  
            break;
            }
            
        case EAlfMeshMakeSphere:
            {
            RArray<TReal32> params;
            CleanupClosePushL( params );
            InternalizeL(aInputBuffer, params);
              
            CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());
            mesh.MakeSphereL(
                params[0],
                HUI_ROUND_FLOAT_TO_INT(params[1]),
                HUI_ROUND_FLOAT_TO_INT(params[2]) );
                
            CleanupStack::PopAndDestroy( &params );
            break;
            }
            
        case EAlfMeshMakeTorus:
            {
            RArray<TReal32> params;
            CleanupClosePushL( params );
            InternalizeL(aInputBuffer, params);
            
            CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());
            mesh.MakeTorusL(
                params[0],
                params[1],
                HUI_ROUND_FLOAT_TO_INT(params[2]),
                HUI_ROUND_FLOAT_TO_INT(params[3]),
                params[4] );
             
            CleanupStack::PopAndDestroy( &params );   
            break;
            }
            
        case EAlfMeshStretchUniformly:
            {
            RArray<TReal32> params;
            CleanupClosePushL( params );
            InternalizeL(aInputBuffer, params);
            
            meshVisual->Mesh().StretchUniformly(
                params[0],
                params[1],
                params[2],
                params[3],
                params[4],
                params[5] );
            
            CleanupStack::PopAndDestroy( &params );  
            break;
            }
            
        case EAlfMeshScaleUniformly:
            {
            RArray<TReal32> params;
            CleanupClosePushL( params );
            InternalizeL(aInputBuffer, params);
            
            meshVisual->Mesh().ScaleUniformly(
                params[0],
                params[1],
                params[2] );
                
            CleanupStack::PopAndDestroy( &params );  
            break;
            }
            
        case EAlfMeshMaterial:
            {
            TAlfMaterialParams alfMaterial;
            TPckg<TAlfMaterialParams> outputPckg(alfMaterial); 
            
            CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());            
            THuiMaterial huiMaterial = mesh.iMaterial;
            
            // Update only timed values
            AlfTimedValueUtility::CopyTimedValue(huiMaterial.iSpecular, alfMaterial.iSpecular);  
            
            // The client should know the other values. Or does other components 
            // modify the material?
            
            aResponse = outputPckg;
            break;
            }
            
        case EAlfMeshSetMaterial:
            {
            TAlfMaterialParams* params = (TAlfMaterialParams*) aInputBuffer.Ptr();
            
            CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());
            THuiMaterial& huiMaterial = mesh.iMaterial;
            // Preset
            huiMaterial.iPreset = (THuiMaterialPreset)params->iPreset;
            // Color
            huiMaterial.iColor = params->iColor;
            // Texture image
            TInt handleImage = params->iTextureImage.iTextureHandle;
        	MHuiTexture* textureImage = reinterpret_cast<MHuiTexture*>(handleImage);
   	        if (textureImage)
   	            {
   	            huiMaterial.iTextureImage = THuiImage(*textureImage, 
	                params->iTextureImage.iTl.iX.ValueNow(),
	                params->iTextureImage.iTl.iY.ValueNow(),
	                params->iTextureImage.iBr.iX.ValueNow(),
	                params->iTextureImage.iBr.iY.ValueNow());    
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iTextureImage.iBr.iY.TimeToTargetinMilliSeconds();
    	        huiMaterial.iTextureImage.SetTexCoords(
    	            params->iTextureImage.iTl.iX.Target(),
    	            params->iTextureImage.iTl.iY.Target(),
    	            params->iTextureImage.iBr.iX.Target(),
    	            params->iTextureImage.iBr.iY.Target(),
    	            transitionTime );
   	            }
   	            
   	        // Specular image
            TInt handleSpecular = params->iSpecularImage.iTextureHandle;
        	MHuiTexture* textureSpecular = reinterpret_cast<MHuiTexture*>(handleSpecular);
   	        if (textureSpecular)
   	            {
   	            huiMaterial.iSpecularImage = THuiImage(*textureSpecular, 
	                params->iSpecularImage.iTl.iX.ValueNow(),
	                params->iSpecularImage.iTl.iY.ValueNow(),
	                params->iSpecularImage.iBr.iX.ValueNow(),
	                params->iSpecularImage.iBr.iY.ValueNow());    
    	        // We take transition time from one, assuming it is same for everyone.
    	        TInt transitionTime = params->iSpecularImage.iBr.iY.TimeToTargetinMilliSeconds();
    	        huiMaterial.iSpecularImage.SetTexCoords(
    	            params->iSpecularImage.iTl.iX.Target(),
    	            params->iSpecularImage.iTl.iY.Target(),
    	            params->iSpecularImage.iBr.iX.Target(),
    	            params->iSpecularImage.iBr.iY.Target(),
    	            transitionTime );
   	            }
   	            
   	        // Specular
   	        AlfTimedValueUtility::CopyTimedValue(params->iSpecular, huiMaterial.iSpecular, iResolver );    

            break;
            }
            
        case EAlfMeshLoadM3GScene:
            {
            TAlfMeshLoadM3GSceneParams* params = (TAlfMeshLoadM3GSceneParams*) aInputBuffer.Ptr();            

            // Load the M3G scene.                        
            CHuiM3GMesh& mesh = static_cast<CHuiM3GMesh&>(meshVisual->Mesh());
            mesh.LoadSceneL(params->iFilename);                
            break;
            }            

        case EAlfMeshAnimationControllerCount:
            {
            TInt controllerCount = meshVisual->Mesh().AnimationControllerCount();
            TPckg<TInt> resultPckg(controllerCount);
            aResponse = resultPckg; // awkward             
            break;
            }            

        case EAlfMeshSetAnimationPosition:
            {
            TAlfMeshAnimationPositionParams* params = (TAlfMeshAnimationPositionParams*) aInputBuffer.Ptr();            
            meshVisual->Mesh().SetAnimationPosition(params->iControllerId, params->iTarget, params->iTime);
            break;
            }            

        case EAlfMeshStartAnimationController:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();            
            meshVisual->Mesh().StartAnimationController(*params);
            break;
            }            

        case EAlfMeshStopAnimationController:
            {
            TInt* params = (TInt*) aInputBuffer.Ptr();            
            meshVisual->Mesh().StopAnimationController(*params);
            break;
            }            

        case EAlfVisualCopyValuesFromOtherVisual:
        case EAlfVisualPauseProperties:
        case EAlfVisualResumeProperties:        
            {
            if (aCommandId == EAlfVisualCopyValuesFromOtherVisual)
                {
                TInt3* inptr =  (TInt3*) aInputBuffer.Ptr();
                CHuiMeshVisual* sourceVisual = (CHuiMeshVisual*) iResolver.GetInterfaceL( EHuiObjectTypeVisual, inptr->iInt1 );
                if (inptr->iInt2&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    meshVisual->iSecondaryAlpha.Set(sourceVisual->iSecondaryAlpha.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::EMeshVisYawAngle)
                    {
                    meshVisual->iYawAngle.Set(sourceVisual->iYawAngle.Target(), inptr->iInt3); 
                    }
                if (inptr->iInt2&AlfVisualProperties::EMeshVisPitchAngle)
                    {
                    meshVisual->iPitchAngle.Set(sourceVisual->iPitchAngle.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::EMeshVisMaterial)
                    {
                    CHuiProceduralMesh& mesh = static_cast<CHuiProceduralMesh&>(sourceVisual->Mesh());            
                    CHuiProceduralMesh& myMesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());            
                
                    myMesh.iMaterial.iSpecular.Set(mesh.iMaterial.iSpecular.Target(), inptr->iInt3 );
                    }
                if (inptr->iInt2&AlfVisualProperties::EScale)
                    {
                    meshVisual->iScale.Set(sourceVisual->iScale.Target(), inptr->iInt3 );
                    }
                }
            else if (aCommandId == EAlfVisualPauseProperties)
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    meshVisual->iSecondaryAlpha.Suspend();
                    }
                if (*inptr&AlfVisualProperties::EMeshVisYawAngle)
                    {
                    meshVisual->iYawAngle.Suspend(); 
                    }
                if (*inptr&AlfVisualProperties::EMeshVisPitchAngle)
                    {
                    meshVisual->iPitchAngle.Suspend();
                    }
                if (*inptr&AlfVisualProperties::EMeshVisMaterial)
                    {
                    CHuiProceduralMesh& myMesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());            
                
                    myMesh.iMaterial.iSpecular.Suspend();
                    }
                if (*inptr&AlfVisualProperties::EScale)
                    {
                    meshVisual->iScale.Suspend();
                    }
                }
            else  // EAlfVisualResumeProperties)
                {
                TInt* inptr =  (TInt*) aInputBuffer.Ptr();
                if (*inptr&AlfVisualProperties::ESecondaryImageAlpha)
                    {
                    meshVisual->iSecondaryAlpha.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::EMeshVisYawAngle)
                    {
                    meshVisual->iYawAngle.Suspend(EFalse); 
                    }
                if (*inptr&AlfVisualProperties::EMeshVisPitchAngle)
                    {
                    meshVisual->iPitchAngle.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::EMeshVisMaterial)
                    {
                    CHuiProceduralMesh& myMesh = static_cast<CHuiProceduralMesh&>(meshVisual->Mesh());            
                
                    myMesh.iMaterial.iSpecular.Suspend(EFalse);
                    }
                if (*inptr&AlfVisualProperties::EScale)
                    {
                    meshVisual->iScale.Suspend(EFalse);
                    }
                }
            } // FALL THROUGH !!

        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfMeshVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }




//----------------------------
// Canvas Visual
//----------------------------
EXPORT_C MAlfExtension* CAlfCanvasVisualHandler::NewL(
    MAlfInterfaceProvider& aResolver, 
    CHuiControl* aOwner, 
    CHuiLayout* aParentLayout)
    {
    CAlfCanvasVisualHandler* me = new (ELeave) CAlfCanvasVisualHandler(aResolver);
    CleanupStack::PushL(me);
    me->ConstructL(NULL, *aOwner, aParentLayout);
    CleanupStack::Pop();
    return me;
    }


EXPORT_C CAlfCanvasVisualHandler::CAlfCanvasVisualHandler(
    MAlfInterfaceProvider& aResolver)
    : CAlfVisualHandler(aResolver)
    {
    }


EXPORT_C void CAlfCanvasVisualHandler::ConstructL(
    CHuiVisual* aVisual, 
    CHuiControl& aOwner, 
    CHuiLayout* aParentLayout)
    {
    if (!aVisual)
        {
        CHuiCanvasVisual* canvasVisual = 
            CHuiCanvasVisual::AddNewL(aOwner, aParentLayout); 
        CAlfVisualHandler::ConstructL(canvasVisual, aOwner, aParentLayout);
        }
    else 
        {
        CAlfVisualHandler::ConstructL(aVisual, aOwner, aParentLayout);
        }
    }


EXPORT_C CAlfCanvasVisualHandler::~CAlfCanvasVisualHandler()
    {
    }

EXPORT_C void CAlfCanvasVisualHandler::Release()
    {
    delete this;
    }


EXPORT_C TAny* CAlfCanvasVisualHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch (aInterface)
        {
        default:
            return CAlfVisualHandler::GetInterface(aInterface);
        }
    }


EXPORT_C void CAlfCanvasVisualHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse)
    {
    CHuiCanvasVisual* canvasVisual = static_cast<CHuiCanvasVisual*>(iVisual);
    
    switch (aCommandId)
        {
        case EAlfCanvasVisualSendbuffer:
            {
            canvasVisual->SetCommandType(CHuiCanvasVisual::ECommandBufferAlf);
            canvasVisual->SetCommandSetL(aInputBuffer);         
            break;
            }
        default:
            CAlfVisualHandler::HandleCmdL(aCommandId, aInputBuffer, aResponse);
        }
    }

EXPORT_C void CAlfCanvasVisualHandler::VisualHandlerExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    CAlfVisualHandler::VisualHandlerExtension(aExtensionUid,aExtensionParameters);
    }

// end of file
