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
* Description:   Canvas visual
*
*/



#include "alf/alfcanvasvisual.h"
#include "alf/alfcontrol.h"
#include "alflogger.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alf/alfenv.h"
#include "alf/alfgc.h"
#include "alflogger.h"



// ======== MEMBER FUNCTIONS ========

struct CAlfCanvasVisual::TCanvasVisualPrivateData
    {
    TCanvasVisualPrivateData() : iObserver(NULL) {}
    MAlfCanvasObserver* iObserver;
    CAlfGc* iGc;
    };

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//

EXPORT_C CAlfCanvasVisual* CAlfCanvasVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfCanvasVisual* canvas = STATIC_CAST(CAlfCanvasVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeCanvas, aParentLayout));
    return canvas;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfCanvasVisual::CAlfCanvasVisual()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfVisual::ConstructL(aOwner);
    
    iCanvasVisualData = new (ELeave) TCanvasVisualPrivateData;
    iCanvasVisualData->iGc = CAlfGc::NewL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfCanvasVisual::~CAlfCanvasVisual()
    {
    if ( iCanvasVisualData )
        {
        delete iCanvasVisualData->iGc;
        }
        
    delete iCanvasVisualData;
    iCanvasVisualData = NULL;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetCanvasObserver( MAlfCanvasObserver* aObserver )
    {
    iCanvasVisualData->iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
void CAlfCanvasVisual::SendBuffer( const TPtrC8& aBuffer )
    {
    TBuf8<1> dum;
    TInt err = Comms()->DoSynchronousCmd(EAlfCanvasVisualSendbuffer, aBuffer, dum);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::BeginDraw()
    {
    TRAP_IGNORE(iCanvasVisualData->iGc->ActivateL());    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::EndDraw()
    {
    HBufC8* commandBuffer = iCanvasVisualData->iGc->CommandBuffer();        
    __ALFLOGSTRING1( "CAlfCanvasVisual::Refresh buffer length: %d", commandBuffer->Length() )        
    if (commandBuffer)
        {
        SendBuffer( commandBuffer->Des() );            
        }
    
    iCanvasVisualData->iGc->Deactivate();        
    }
    
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::RemoveAndDestroyAllD()
    {
    CAlfVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCanvasVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfCanvasVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfVisual::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCanvasVisual::DoRemoveAndDestroyAllD()
    {
    CAlfVisual::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfCanvasVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawImage(const CAlfTexture& aTexture, const TAlfRealRect& aDestinationRect)
    {
    iCanvasVisualData->iGc->DrawImage(aTexture, aDestinationRect);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawText(const TDesC& aText, const TAlfRealRect& aDestinationRect)
    {
    iCanvasVisualData->iGc->DrawText(aText, aDestinationRect);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawLines(const TAlfRealLine* aLines, TInt aLineCount)
	{
	RArray<TReal32> linesPointCords;
	for(TInt i = 0; i< aLineCount; i++)
		{
		linesPointCords.Append(aLines[i].iStart.iX);
		linesPointCords.Append(aLines[i].iStart.iY);
		linesPointCords.Append(aLines[i].iEnd.iX);
		linesPointCords.Append(aLines[i].iEnd.iY);
		}
	iCanvasVisualData->iGc->DrawLines(linesPointCords);    
	linesPointCords.Close();  
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawEllipse(const TAlfRealRect& aDestinationRect)
	{
	iCanvasVisualData->iGc->DrawEllipse(aDestinationRect);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawPoints(const TAlfRealPoint* aPoints, TInt aPointCount)
	{
	RArray<TReal32> pointCords;
	for(TInt i = 0; i< aPointCount; i++)
		{
		pointCords.Append(aPoints[i].iX);
		pointCords.Append(aPoints[i].iY);
		}
	iCanvasVisualData->iGc->DrawPoints(pointCords);
	pointCords.Close();  
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawPolygon(const TAlfRealPoint* aPoints, TInt aPointCount)
	{
	RArray<TReal32> pointCords;
	for(TInt i = 0; i< aPointCount; i++)
		{
		pointCords.Append(aPoints[i].iX);
		pointCords.Append(aPoints[i].iY);
		}
	iCanvasVisualData->iGc->DrawPolygon(pointCords);
    pointCords.Close();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::DrawRects(const TAlfRealRect* aRects, TInt aRectCount)
	{
	RArray<TReal32> rectPointCords;
	for(TInt i = 0; i< aRectCount; i++)
		{
		rectPointCords.Append(aRects[i].iTl.iX);
		rectPointCords.Append(aRects[i].iTl.iY);
		rectPointCords.Append(aRects[i].iBr.iX);
		rectPointCords.Append(aRects[i].iBr.iY);
		}
	iCanvasVisualData->iGc->DrawRects(rectPointCords);
	rectPointCords.Close();
	}

    
// Setters
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetTextStyle(const CAlfTextStyle& aTextStyle)
    {
    iCanvasVisualData->iGc->SetTextStyle(aTextStyle);            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetPenColor(const TRgb& aColor)
	{
    iCanvasVisualData->iGc->SetPenColor(aColor);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetPenWidth(const TReal32& aWidth)
	{
    iCanvasVisualData->iGc->SetPenWidth(aWidth);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetPenOpacity(const TReal32& aOpacity)
	{
    iCanvasVisualData->iGc->SetOpacity(aOpacity);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetPolygonDrawMode(TAlfPolygonDrawMode aPolygonDrawMode)
	{
    iCanvasVisualData->iGc->SetPolygonDrawMode(aPolygonDrawMode);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::SetTextAlign(TAlfAlignHorizontal aAlignHorizontal, TAlfAlignVertical aAlignVertical)
	{
    iCanvasVisualData->iGc->SetTextAlign(aAlignHorizontal, aAlignVertical);
    }


// Transformations
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::LoadIdentity()
	{
    iCanvasVisualData->iGc->LoadIdentity();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
    iCanvasVisualData->iGc->Translate(aX, aY, aZ);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
    iCanvasVisualData->iGc->Scale(aX, aY, aZ);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfCanvasVisual::Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
    iCanvasVisualData->iGc->Rotate(aAngle, aX, aY, aZ);
    }

