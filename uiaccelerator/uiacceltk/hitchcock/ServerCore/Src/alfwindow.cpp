/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/

#include "alfwindow.h"

#include "alfwindowmanager.h"
#include "alfwindowstructs.h"
#include "alfwindowbuffer.h"
#include "alfstreamerbridge.h"
#include "alf/alfconstants.h"
#include <uiacceltk/HuiUtil.h>
#include "alfwindowdata.h"

#include <string.h>
#include <s32mem.h>

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfWindow* CAlfWindow::NewL(TUint aId, CAlfWindowManager& aOwner, const TSize& aSizeInPixels)
    {
    CAlfWindow* self = new (ELeave) CAlfWindow;
    CleanupStack::PushL(self);
    self->ConstructL(aId, aOwner, aSizeInPixels);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CAlfWindow::~CAlfWindow()
    {
    if (iData)
        {
        delete iData->iBufferGc;
        delete iData->iBufferDev;
        delete iData->iInBuffer;
        delete iData->iOutBuffer;
        
        iData->iWindowArea.Close();
        
        delete iData;
        }
    }
        
// ---------------------------------------------------------------------------
// WsInfo
// ---------------------------------------------------------------------------
//
EXPORT_C const TAlfWServInfo& CAlfWindow::WsInfo() const
    {
    return iData->iOriginalInfo;
    }
    
// ---------------------------------------------------------------------------
// Position
// ---------------------------------------------------------------------------
//
EXPORT_C TPoint CAlfWindow::Position() const
    {
    return iData->iPosition;
    }

// ---------------------------------------------------------------------------
// Size
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CAlfWindow::Size() const
    {
    return iData->iSize;
    }
    
// ---------------------------------------------------------------------------
// Opacity
// ---------------------------------------------------------------------------
//
EXPORT_C TReal32 CAlfWindow::Opacity() const __SOFTFP
    {
    return iData->iOpacity;
    }
    
// ---------------------------------------------------------------------------
// Rotation
// ---------------------------------------------------------------------------
//
EXPORT_C TReal32 CAlfWindow::Rotation() const __SOFTFP
    {
    return iData->iRotation;
    }
    
// ---------------------------------------------------------------------------
// Identifier
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CAlfWindow::Identifier() const
    {
    return iData->iIdentifier;
    }

// ---------------------------------------------------------------------------
// IsActive
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfWindow::IsActive() const
    {
    return iData->iIsActive;
    }
    
// ---------------------------------------------------------------------------
// SurfaceExtent
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CAlfWindow::SurfaceExtent() const
    {
    return iData->iSurfaceExtent;
    }
    
// ---------------------------------------------------------------------------
// Index
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CAlfWindow::Index() const
    {
    return iData->iIndex;
    }
    
// ---------------------------------------------------------------------------
// InBuffer
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfWindowBuffer* CAlfWindow::InBuffer()
    { // we currently support only fixed type of buffers
    if (!iData->iInBuffer)
        {
        TRAP_IGNORE(CreateInBufferL());    
        }

    return iData->iInBuffer;
    }

// ---------------------------------------------------------------------------
// OutBuffer
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfWindowBuffer* CAlfWindow::OutBufferL(TBool aCopyContents)
    { // we currently support only fixed type of buffers
    if (!iData->iInBuffer)
        {
        CreateInBufferL();    
        }

    if (!iData->iOutBuffer)
        {
        iData->iOutBuffer = iData->iInBuffer->CreateCopyL(aCopyContents);
        }
    return iData->iOutBuffer;
    }


// ---------------------------------------------------------------------------
// SetActive
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetActive(TBool aVisible)
    {
    if (!IsInsertedToScene())
     	{
     	iData->iIsActive = aVisible;
     	}

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iActive = aVisible; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowActive, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iIsActive = aVisible;
            }
        }

    //SetOpacity(aVisible?1.0f:0.0f, 500);
    }
    
// ---------------------------------------------------------------------------
// SetSurfaceExtent
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetSurfaceExtent(const TRect& aRect)
    {
    if (!IsInsertedToScene())
     	{
        iData->iSurfaceExtent = aRect;
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
        
    if (windowAttributes)
        {
        // Setting the extent rectangle in one command fails in armv5 if the alignment is not
        // divisible by 8 (maybe 4 would be enough).
        // Therefore the corners must be set one by one. That succeeds even if the address
        // is not aligned.
//        windowAttributes->iSurfaceExtent = aRect;
        windowAttributes->iSurfaceExtent.iTl.iX = aRect.iTl.iX;
        windowAttributes->iSurfaceExtent.iTl.iY = aRect.iTl.iY;
        windowAttributes->iSurfaceExtent.iBr.iX = aRect.iBr.iX;
        windowAttributes->iSurfaceExtent.iBr.iY = aRect.iBr.iY;
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;

        TAlfBridgerData d;
        d.Set(EAlfDSSetSurfaceExtent, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
        	iData->iSurfaceExtent = aRect;
            }
        }
    }


// ---------------------------------------------------------------------------
// SetLayerUsesAplhaFlag
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetLayerUsesAplhaFlag( TBool aEnabled )
    {
    if (!IsInsertedToScene())
        {
        iData->iLayerUsesAlphaFlagEnabled = aEnabled;
        return;
        }

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
        
    if (windowAttributes)
        {
        windowAttributes->iLayerUsesAlphaFlagEnabled = aEnabled;
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDsLayerUsesAlphaFlagChanged, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iLayerUsesAlphaFlagEnabled = aEnabled;
            }
        }    
    }

// ---------------------------------------------------------------------------
// ContentReady
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::ContentReady(const TRegion& /*aUpdatedArea*/)
    {
    // Todo: Should check whether window is active
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    
    CAlfWindowBuffer* buffer = iData->iOutBuffer ? iData->iOutBuffer : iData->iInBuffer;
        
    iData->iWindowManager->ContentReady(*this, buffer); // Todo: add region;            
    }

// ---------------------------------------------------------------------------
// Move
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::Move(TUint aIndex)
    {
    if (!IsInsertedToScene())
     	{
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iOrdinalPosition = aIndex; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSReorder, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset ); 
        
        if (iData->iWindowManager->PostIt(d) == KErrNone)
            {
            iData->iIndex = aIndex; // ToDo: if app gives out of bounds val, it is modified in 
                                    // alf scene and this member is set to wrong value
            }
        }
    }

// ---------------------------------------------------------------------------
// EnableIndexChangedNotifications
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::EnableIndexChangedNotifications(TBool /*aEnable*/)
    {
    // Todo
    //iData->iWindowManager->
    }

// ---------------------------------------------------------------------------
// SetPos
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetPos(TPoint aPos, TUint aDuration)
    {
    if (!IsInsertedToScene())
     	{
        iData->iPosition = aPos;
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();
    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if (windowAttributes)
        {
        windowAttributes->iPosition = aPos;
        windowAttributes->iTransitionTime = aDuration; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;

        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowPos, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset ); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iPosition = aPos;
            }
        }
    iData->iPosition = aPos; // TODO: This breaks the animation loop
    }

// ---------------------------------------------------------------------------
// SetSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetSize(TSize aSize, TUint aDuration)
    {
    if (!IsInsertedToScene())
    	{
        iData->iSize = aSize;
    	return;
    	}

    if ( aSize != iData->iSize )
    	{
    	// TODO: Only texture size should be changed. not the window size at this point.
    	iData->iSize = aSize;
    	if (iData->iBufferDev)
    		{
    		iData->iBufferDev->Resize( aSize );
    		}
    	}

    TAlfWServInfo wservInfo = WsInfo();
    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    
    if (windowAttributes)
        {
        windowAttributes->iSize = aSize;
        windowAttributes->iTransitionTime = aDuration; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;

        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowSize, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iSize = aSize;
            }
        }
    }

// ---------------------------------------------------------------------------
// SetRotation
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetRotation(TReal32 aAngle, TUint aDuration) __SOFTFP
    {
    if (!IsInsertedToScene())
     	{
        iData->iRotation = aAngle;
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);

    if (windowAttributes)
        {
        windowAttributes->iRotation = aAngle;
        windowAttributes->iTransitionTime = aDuration; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowRotation, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset ); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iRotation = aAngle;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// SetOpacity
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfWindow::SetOpacity(TReal32 aOpacity, TUint aDuration) __SOFTFP
    {
    if (!IsInsertedToScene())
     	{
        iData->iOpacity = aOpacity;
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();
    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if (windowAttributes)
        {
        windowAttributes->iOpacity = aOpacity;
        windowAttributes->iTransitionTime = aDuration;
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowOpacity, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iOpacity = aOpacity;
            }
        }
    }


// ---------------------------------------------------------------------------
// CreateSetWindowAreaL
// ---------------------------------------------------------------------------
//
TAny* CAlfWindow::CreateSetWindowAreaL(
        TInt& aOffset, TInt aScreenNumber, const TPoint& aOrigin, const TRegion& aRegion)
    {
    const TInt size = ( 4*aRegion.Count() + 4 ) * sizeof(TInt32);
    TAny* data = (TAny*)iData->iWindowManager->Bridge()->AppendVarDataL( size, aOffset );

    RMemWriteStream str( data, size );
    str.WriteInt32L( aScreenNumber );
    str.WriteInt32L( aOrigin.iX );
    str.WriteInt32L( aOrigin.iY );
    str.WriteInt32L( aRegion.Count() );
        
    for (TInt i = 0; i < aRegion.Count(); ++i )
        {
        const TRect rect(aRegion[i]);
        str.WriteInt32L(rect.iTl.iX);
        str.WriteInt32L(rect.iTl.iY);
        str.WriteInt32L(rect.iBr.iX);
        str.WriteInt32L(rect.iBr.iY);
        }
    
    return data;
    }

// ---------------------------------------------------------------------------
// SetWindowArea
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetWindowArea()
    {
    if ( iData->iWindowArea.Count() )
        {
        // Now inserted to scene - forward
        SetWindowArea(iData->iWindowAreaOrigin, iData->iWindowArea);
        iData->iWindowArea.Clear();
        }
    }

// ---------------------------------------------------------------------------
// SetWindowArea
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetWindowArea(const TPoint& aOrigin, const TRegion& aRegion)
    {
    if ( WsInfo().iNodeType != MWsWindowTreeNode::EWinTreeNodeClient )
        {
        return;
        }
        
    if (!IsInsertedToScene())
     	{
     	// Cache data to be able to send once this is inserted to scene
        iData->iWindowAreaOrigin = aOrigin;
        iData->iWindowArea.Copy(aRegion);
        if ( iData->iWindowArea.CheckError() )
            {
            iData->iWindowArea.Clear();
            }
     	return;
     	}

    TAlfWServInfo wservInfo = WsInfo();
    TInt offset = 0;
    TAny* data = NULL;
    
    TRAP_IGNORE(data = CreateSetWindowAreaL( offset, wservInfo.iScreenNumber, aOrigin, aRegion ));  
    
    if (data)
        {
        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowArea, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        iData->iWindowManager->PostIt(d);
        }
    }

// ---------------------------------------------------------------------------
// SetTransparencyAlphaChannel
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetTransparencyAlphaChannel(TBool aActive)
    {
    if ( WsInfo().iNodeType != MWsWindowTreeNode::EWinTreeNodeClient )
        {
        return;
        }

    if (!IsInsertedToScene())
        {
        iData->iTransparencyAlphaChannel = aActive;
        return;        
        }
        
    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iActive = aActive; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSSetTransparencyAlphaChannel, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iTransparencyAlphaChannel = aActive;
            }
        }
    }

// ---------------------------------------------------------------------------
// TransparencyAlphaChannel
// ---------------------------------------------------------------------------
//
TBool CAlfWindow::TransparencyAlphaChannel() const
    {
    return iData->iTransparencyAlphaChannel;
    }

// ---------------------------------------------------------------------------
// IsIncludedToVisibilityCalculation
// ---------------------------------------------------------------------------
//
TBool CAlfWindow::IsIncludedToVisibilityCalculation() const
    {
    return iData->iIncludeToVisibilityCalculation;
    }

// ---------------------------------------------------------------------------
// IncludeToVisibilityCalculation
// ---------------------------------------------------------------------------
//
void CAlfWindow::IncludeToVisibilityCalculation(TBool aActive)
    {
    if (!IsInsertedToScene())
        {
        iData->iIncludeToVisibilityCalculation = aActive;
        return;
        }

    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iActive = aActive; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        windowAttributes->iWindowNodeType = wservInfo.iNodeType; 
        
        TAlfBridgerData d;
        d.Set(EAlfDSIncludeToVisibilityCalculation, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            iData->iIncludeToVisibilityCalculation = aActive;
            }
        }
    }


// ---------------------------------------------------------------------------
// SetWsInfo
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetWsInfo(const TAlfWServInfo& aInfo)
    {
    iData->iOriginalInfo = aInfo;    
    SetPos(aInfo.iPosition);
    SetSize(aInfo.iSize);
    }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CAlfWindow::CAlfWindow()
    {
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfWindow::ConstructL(TUint aId, CAlfWindowManager& aOwner, const TSize& aSizeInPixels)
    {
    iData = new (ELeave) TAlfWindowData;
    memset(iData, 0, sizeof(TAlfWindowData)); // initialize all values to zero
    iData->iWindowManager = &aOwner;
    iData->iIdentifier = aId;
    iData->iIsActive = EFalse;
    iData->iOpacity = 1.0f;    
    iData->iWindowArea = RRegion();
    iData->iTransparencyAlphaChannel = EFalse;
    iData->iIncludeToVisibilityCalculation = EFalse;
    
    SetSize(aSizeInPixels);
    }
    
// ---------------------------------------------------------------------------
// CreateInBufferL
// ---------------------------------------------------------------------------
//
void CAlfWindow::CreateInBufferL()
    {
    if (!iData->iInBuffer)
        {
        iData->iInBuffer = CAlfWindowBuffer::NewL(TSize(0,0),0,0,0);            
        }    
    }
    
// ---------------------------------------------------------------------------
// SetFlag
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetFlag( TInt aFlag )
    {
    iData->iFlags |= aFlag;
    
    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iActive = aFlag;  // transporting this in iActive field
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSSetWindowFlag, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        iData->iWindowManager->PostIt(d);
        }
    }

// ---------------------------------------------------------------------------
// ClearFlag
// ---------------------------------------------------------------------------
//
void CAlfWindow::ClearFlag( TInt aFlag )
    {
    iData->iFlags &= ~aFlag;
    
    TAlfWServInfo wservInfo = WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
    if ( windowAttributes )
        {
        windowAttributes->iActive = aFlag;  // transporting this in iActive field
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        
        TAlfBridgerData d;
        d.Set(EAlfDSClearWindowFlag, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        iData->iWindowManager->PostIt(d);
        }
    }


// ---------------------------------------------------------------------------
// GetBitGcL
// ---------------------------------------------------------------------------
//
CFbsBitGc* CAlfWindow::GetBitGcL()
    {
    if (!iData->iBufferDev)
        {
        if (!iData->iInBuffer)
            {
            CreateInBufferL();    
            }
        
        iData->iBufferDev = CFbsBitmapDevice::NewL( iData->iInBuffer->Bmp() );
        TInt err = iData->iBufferDev->CreateContext( iData->iBufferGc );
        if ( err != KErrNone)
            {
            delete iData->iBufferDev;
            iData->iBufferDev = 0;
            User::Leave(err);
            }
        }
         
    return iData->iBufferGc;
    }    

// ---------------------------------------------------------------------------
// CommitGc
// ---------------------------------------------------------------------------
//
void CAlfWindow::CommitGc()
    {
    delete iData->iBufferGc;
    iData->iBufferGc = 0;
    delete iData->iBufferDev;
    iData->iBufferDev = 0;
    if(!iData->iOutBuffer) // Todo: Should we update textures even if window not active?
        {
        iData->iWindowManager->ContentReady(*this, iData->iInBuffer);
        }
    }

// ---------------------------------------------------------------------------
// IsInsertedToScene
// ---------------------------------------------------------------------------
//
TBool CAlfWindow::IsInsertedToScene()
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    return iData->iIsInScene;
    }


// ---------------------------------------------------------------------------
// InsertedToScene
// ---------------------------------------------------------------------------
//
void CAlfWindow::InsertedToScene(TBool aInserted)
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    iData->iIsInScene = aInserted;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CAlfWindow::ShouldBeDeactivated()
    {
    __ASSERT_ALWAYS( iData, USER_INVARIANT() );
    return iData->iFlags & TAlfWindowData::EShouldInactivate;
    }

// ---------------------------------------------------------------------------
// PostBuffer
// ---------------------------------------------------------------------------
//
void CAlfWindow::PostPartialBuffer( TAny* aPtr, TInt aSize, TBool aPartial, TBool aLastPart, TBool aEmptyBuffers, TInt aNodeFlags )
    {
    TAlfWServInfo wservInfo = WsInfo();
    TInt offset;
    TAlfWindowCommandBufferAttributes* windowCommandBuffer = CreateWindowCommandBufferAttributes(offset);
    if (windowCommandBuffer)
        {
        windowCommandBuffer->iBufferLength = aSize;
        windowCommandBuffer->iBuffer = aPtr;
        windowCommandBuffer->iEmptyThisBuffer = aEmptyBuffers;
        windowCommandBuffer->iWindowNodeFlags = aNodeFlags;
        windowCommandBuffer->iScreenNumber = wservInfo.iScreenNumber;
        if ( aLastPart && aPartial )
        	{
        	windowCommandBuffer->iPartStatus = TAlfWindowCommandBufferAttributes::ELastPart;
        	}
        else if ( aPartial )
        	{
        	windowCommandBuffer->iPartStatus = TAlfWindowCommandBufferAttributes::ENotComplete;
        	}
        else if (!aPartial)
        	{
        	windowCommandBuffer->iPartStatus = TAlfWindowCommandBufferAttributes::EPartComplete;
        	}
        
        TAlfBridgerData d;
        d.Set(EAlfDSPostCanvasBuffer, 
        		wservInfo.iRefId.iWindowGroupId, 
        		wservInfo.iRefId.iWindowIdentifer, 
        		(TAny*)offset ); 

        if (iData->iWindowManager->PostIt(d))
            {
            // ???
            }
        }
    
    }

// ---------------------------------------------------------------------------
// PostBuffer
// ---------------------------------------------------------------------------
//
void CAlfWindow::PostBuffer( TAny* aPtr, TInt aSize, TInt aNodeFlags )
    {
    TAlfWServInfo wservInfo = WsInfo();
    TInt offset;
    TAlfWindowCommandBufferAttributes* windowCommandBuffer = CreateWindowCommandBufferAttributes(offset);
           
    if (windowCommandBuffer)
        {
        windowCommandBuffer->iBufferLength = aSize;
        windowCommandBuffer->iBuffer = aPtr;
        windowCommandBuffer->iEmptyThisBuffer = ETrue;
        windowCommandBuffer->iPartStatus = TAlfWindowCommandBufferAttributes::EPartComplete;
        windowCommandBuffer->iWindowNodeFlags = aNodeFlags;
        

        TAlfBridgerData d;
        d.Set(EAlfDSPostCanvasBuffer, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        if (iData->iWindowManager->PostIt(d))
            {
            // ???
            }
        }
    }

// ---------------------------------------------------------------------------
// SetNodeTracking
// ---------------------------------------------------------------------------
//
void CAlfWindow::SetNodeTracking( TInt aValue )
    {
    TAlfBridgerData d;
    TAlfWServInfo wservInfo = WsInfo();
      
    d.Set(EAlfDSSetNodeTracking, 
        wservInfo.iRefId.iWindowIdentifer,
        aValue,
        NULL);
    iData->iWindowManager->PostIt(d);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAlfWindowAttributes* CAlfWindow::CreateWindowAttributes(TInt& aIndex)
    {
    TAlfWindowAttributes empty;
    TAlfWindowAttributes* attributes = NULL;
    TRAP_IGNORE(attributes = (TAlfWindowAttributes*)iData->iWindowManager->Bridge()->AppendVarDataL( sizeof(TAlfWindowAttributes), aIndex ))
    if ( attributes )
        {
        *attributes = empty;
        }
    return attributes;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAlfWindowCommandBufferAttributes* CAlfWindow::CreateWindowCommandBufferAttributes(TInt& aIndex)
    {
    TAlfWindowCommandBufferAttributes empty;    
    TAlfWindowCommandBufferAttributes* windowCommandBuffer = NULL;    
    TRAP_IGNORE(windowCommandBuffer = (TAlfWindowCommandBufferAttributes*)iData->iWindowManager->Bridge()->AppendVarDataL( sizeof(TAlfWindowCommandBufferAttributes), aIndex ))    
    if ( windowCommandBuffer )
        {
        *windowCommandBuffer = empty;
        }
    return windowCommandBuffer;
    }
