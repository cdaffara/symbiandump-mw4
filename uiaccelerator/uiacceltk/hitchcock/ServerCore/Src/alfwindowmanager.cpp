/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include <fbs.h>

#include "alfwindowmanager.h"
#include "alfhierarchymodel.h"
#include "alfstreamerserver.h"
#include "alfstreamerbridge.h"
#include "alfwindow.h"
#include "alfwindowfxplugin.h"
#include "alfwindowbuffer.h"
#include "alfstreamerconsts.h"
#include "alflogger.h"
#include "alfwindowdata.h"

#include <ecom.h>


struct TPluginInfo
    {
    TPluginInfo():iPlugin(0),iDtor(TUid::Uid(0)), iPluginUid(TUid::Uid(0)), iNotifyEnabled(EFalse), iWindowDestroyNotifyEnabled(EFalse)
        {}
        
    CAlfWindowFxPlugin* iPlugin;
    TUid iDtor;
    TUid iPluginUid;
    TBool iNotifyEnabled; // flags would be more sophisticated way..
    TBool iWindowDestroyNotifyEnabled; 
    
    void EnableNotifications(TBool aEnable)
        {
        iNotifyEnabled = aEnable;
        }
    
    void NotifyNewWindow(const TWindowIdentifier& aId)
        {
        if (iNotifyEnabled)
            iPlugin->NewWindow(aId);
        }
    void EnableWindowDestroyNotifications(TBool aEnable)
        {
        iWindowDestroyNotifyEnabled = aEnable;
        }
            
    void NotifyDestroyWindow(const TWindowIdentifier& aId)
        {
        if (iWindowDestroyNotifyEnabled)
            iPlugin->DestroyWindow(aId);
        }    
    };

class CAlfWindowManager::TAlfWindowMgrData
    {
    public:
    TInt iScreenId;
    
    RPointerArray<CAlfWindow> iWindows;
    RArray<TPluginInfo> iPlugins;
    CAlfHierarchyModel* iTree;
    };

// ---------------------------------------------------------------------------
// FindPlugin
// ---------------------------------------------------------------------------
//
CAlfWindowFxPlugin* CAlfWindowManager::FindPlugin(const TUid& aPluginUid)
    {
    for (TInt i = 0; i <= iData->iPlugins.Count()-1; i++)
        {
        if (iData->iPlugins[i].iPluginUid == aPluginUid )
            {
            return iData->iPlugins[i].iPlugin;
            }
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfWindowManager* CAlfWindowManager::NewL(CAlfHierarchyModel* aAlfHierarchy)
    {
    CAlfWindowManager* self = new (ELeave) CAlfWindowManager;
    CleanupStack::PushL(self);
    self->ConstructL(aAlfHierarchy);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//   
CAlfWindowManager::~CAlfWindowManager()
    {
    if (iData)
        {
        iData->iWindows.ResetAndDestroy();
        delete iData;
        }
    REComSession::FinalClose();
    RFbsSession::Disconnect();
    }


// ---------------------------------------------------------------------------
// CreateNewWindowL
// For Fx plugins   
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfWindow* CAlfWindowManager::CreateNewWindowL(TUint aIndex, const TSize& aSizeInPixels )
    {
    if(!RFbsSession::GetSession())  
        {
        User::LeaveIfError(RFbsSession::Connect());
        }

    CAlfWindow* window = CAlfWindow::NewL(aIndex,*this, aSizeInPixels);
    CleanupStack::PushL(window);
    iData->iWindows.AppendL(window);
    CleanupStack::Pop();
      
    TAlfWServInfo wservInfo = window->WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes( offset );
    if (windowAttributes)
        {
        windowAttributes->iSize = aSizeInPixels; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
        windowAttributes->iParentNodeId = wservInfo.iParentRefId.iWindowIdentifer;
        
        TAlfBridgerData data;
        data.Set(EAlfDSNewWindow, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 
        
        User::LeaveIfError(PostIt(data));            
        }
    
    return window;
    }

// ---------------------------------------------------------------------------
// BindWindowToHitch
// 
// ---------------------------------------------------------------------------
//
void CAlfWindowManager::BindWindowToHitchL(CAlfWindow* aWindow, CAlfWindowManager* aMgr)
	{
    TAlfWServInfo wservInfo = aWindow->WsInfo();

    TInt offset;
    TAlfWindowAttributes* windowAttributes = CreateWindowAttributes( offset );
    if ( !windowAttributes )
        {
        return;
        }
    windowAttributes->iSize = aWindow->Size(); 
    windowAttributes->iPosition = aWindow->Position();
    windowAttributes->iWindowNodeType = wservInfo.iNodeType; 
    windowAttributes->iScreenNumber = wservInfo.iScreenNumber;
    windowAttributes->iParentNodeId = wservInfo.iParentRefId.iWindowIdentifer;
    windowAttributes->iClientHandle = wservInfo.iClientSideId.iWindowIdentifer;
    windowAttributes->iClientGroupHandle = wservInfo.iClientSideId.iWindowGroupId;
    
#ifdef ALF_DEBUG_TRACK_DRAWING
    windowAttributes->iTrackWindow = wservInfo.iTrackWindow;
#endif
	TAlfBridgerData data;
    data.Set(EAlfDSNewWindow, 
        wservInfo.iRefId.iWindowGroupId, 
        wservInfo.iRefId.iWindowIdentifer, 
        (TAny*)offset); 

	User::LeaveIfError(aMgr->PostIt(data));

	aWindow->InsertedToScene(ETrue);
	aWindow->SetPos(aWindow->Position());
	if (aWindow->Rotation())
	    {
	    aWindow->SetRotation(aWindow->Rotation());
	    }

    if ( aWindow->Opacity() != 1.0f) // different from default
        {
        aWindow->SetOpacity(aWindow->Opacity());
        }
	aWindow->SetSurfaceExtent(aWindow->SurfaceExtent());
    aWindow->SetWindowArea();
    aWindow->SetTransparencyAlphaChannel(aWindow->TransparencyAlphaChannel());
    aWindow->IncludeToVisibilityCalculation(aWindow->IsIncludedToVisibilityCalculation());
	aWindow->SetActive(aWindow->IsActive());
	}

// ---------------------------------------------------------------------------
// CreateNewWindowL
// for hierarchy model
// ---------------------------------------------------------------------------
//   
CAlfWindow* CAlfWindowManager::CreateNewWindowL(TUint aIndex, const TAlfWServInfo& aInfo)
    {
    if(!RFbsSession::GetSession())
         {
         User::LeaveIfError(RFbsSession::Connect());
         }

     CAlfWindow* window = CAlfWindow::NewL(aIndex,*this, aInfo.iSize );
     CleanupStack::PushL(window);
     iData->iWindows.AppendL(window);
     CleanupStack::Pop();

     window->SetWsInfo(aInfo);

    // provide notification for plugins that have enabled it
    for (TInt i = 0; i <= iData->iPlugins.Count()-1; i++)
        {
        iData->iPlugins[i].NotifyNewWindow(aInfo.iClientSideId);
        }

    return window;
    }
    
// ---------------------------------------------------------------------------
// DestroyWindow
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::DestroyWindow(CAlfWindow& aWindow)
    {
    TInt i = iData->iWindows.Find(&aWindow);
    if (i >= 0)
        {
        TAlfWServInfo wservInfo = aWindow.WsInfo();

        TInt offset;
        TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);
        windowAttributes->iWindowNodeType = wservInfo.iNodeType; 
        windowAttributes->iScreenNumber = wservInfo.iScreenNumber; 

    	TAlfBridgerData data;
        data.Set(EAlfDSDestroyWindow, 
            wservInfo.iRefId.iWindowGroupId, 
            wservInfo.iRefId.iWindowIdentifer, 
            (TAny*)offset); 

        PostIt(data);

        iData->iWindows.Remove(i);
        delete &aWindow;
        
         // provide notification for plugins that have enabled it
    	for (TInt i = 0; i <= iData->iPlugins.Count()-1; i++)
        {
        iData->iPlugins[i].NotifyDestroyWindow(wservInfo.iClientSideId);
        }
        
       }
    }
    
// ---------------------------------------------------------------------------
// EnableNewWindowNotificationsL
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::EnableNewWindowNotificationsL(CAlfWindowFxPlugin& aPlugin, 
                                                TBool aEnable)
    {
    // Assuming the amount of plugins to remain very limited
    for (TInt i = 0; i <= iData->iPlugins.Count()-1; i++)
        {
        if (iData->iPlugins[i].iPluginUid == *aPlugin.Identifier() )
            {
            iData->iPlugins[i].EnableNotifications(aEnable);
            }
        }
    }
    

// ---------------------------------------------------------------------------
// EnableDestroyWindowNotificationsL
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::EnableDestroyWindowNotificationsL(CAlfWindowFxPlugin& aPlugin, 
                                                TBool aEnable )
	{
	 // Assuming the amount of plugins to remain very limited
    for (TInt i = 0; i <= iData->iPlugins.Count()-1; i++)
        {
        if (iData->iPlugins[i].iPluginUid == *aPlugin.Identifier() )
            {
            iData->iPlugins[i].EnableWindowDestroyNotifications(aEnable);
            }
        }
	}

// ---------------------------------------------------------------------------
// ReserveL
// Decaprecated
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfWindow* CAlfWindowManager::ReserveL( const TWindowIdentifier& /*aId*/, TBool aReserveBuffer /*= ETrue*/ )
    {
    return 0; // we never get here 
    }
    

// ---------------------------------------------------------------------------
// Release
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::Release( CAlfWindow& /*aWindow*/, TBool /*aReserveBuffer = ETrue*/ )
    {
    /*TInt i = iData->iWindows.Find(&aWindow);
    if (i >= 0)
        {
        CAlfWindow* window = iData->iWindows[i]; 
        window->SetReserved( EFalse, aReserveBuffer );
        if (!window->WsInfo().ExistsInWServ())
            {
            __ALFFXLOGSTRING1("CAlfWindowManager::Released, 0x%x", window->Identifier());
            DestroyWindow( *window, EFalse );
            }
        }*/
    }

// ---------------------------------------------------------------------------
// ScreenId
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CAlfWindowManager::ScreenId() const
    {
    if (iData)
        {
        return iData->iScreenId;          
        }
    return 0;
    }


#ifdef RND_FXPLUGIN_FIX
// ---------------------------------------------------------------------------
// GetWinIdListL
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::GetWinIdListL(RArray<TWindowIdentifier>& aWinIdList)
    {
    // Create list 
    aWinIdList.Reset();
    for (TInt i=0; i < iData->iWindows.Count(); i++)
    	{
   		if (iData->iWindows[i]->IsActive() && iData->iWindows[i]->IsReserved() == EFalse )
	    	{
	    	aWinIdList.AppendL( iData->iWindows[i]->WsInfo().iClientSideId ); //\todo shoud this be ordered list?	
	    	}
    	}
    }
#endif    

// ---------------------------------------------------------------------------
// GetWinIdListL
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::GetWinIdListL( TInt aWsGroupId, RArray<TWindowIdentifier>& aWinIdList)
    {
    // Get list of windows which belong to the window server group specified
    aWinIdList.Reset();
    for ( TInt i=0; i < iData->iWindows.Count(); i++ )
    	{
   	    if ( iData->iWindows[i]->WsInfo().iClientSideId.iWindowGroupId == aWsGroupId )
   	        {
        	aWinIdList.AppendL( iData->iWindows[i]->WsInfo().iClientSideId );
        	}
    	}
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfWindowManager::GetWinInfoListL( TInt aWsGroupId, TInt aClientWindowHandle, RArray<TAlfWServInfo>& aWinInfoList )
    {
    aWinInfoList.Reset();
    for ( TInt i = 0; i < iData->iWindows.Count(); i++ )
        {
   	    if ( iData->iWindows[i]->WsInfo().iClientSideId.iWindowGroupId == aWsGroupId )
   	        {
            if ( aClientWindowHandle == KErrNotFound ||
                iData->iWindows[i]->WsInfo().iClientSideId.iWindowIdentifer == aClientWindowHandle )
                {
                aWinInfoList.AppendL( iData->iWindows[i]->WsInfo() );
                }
   	        }
        }
    }
    

// ---------------------------------------------------------------------------
// MoveWindow
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::MoveWindow(CAlfWindow& aWindow, TInt aIndex)
    {
    aWindow.Move(aIndex);    
    }
    

// ---------------------------------------------------------------------------
// SetWindowVisibility
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::SetWindowVisibility(CAlfWindow& /*aWindow*/, TBool /*aVisible*/)
    {
        
    }
    

// ---------------------------------------------------------------------------
// ContentReady
// ---------------------------------------------------------------------------
//
void CAlfWindowManager::ContentReady(CAlfWindow& aWindow, CAlfWindowBuffer* aOutBuffer)
    {
    if(!aWindow.IsInsertedToScene())
    	{
    	TRAPD( err, BindWindowToHitchL(&aWindow, this) );
    	if (err)
    	    {
    	    __ALFLOGSTRING1( "CAlfWindowManager::ContentReady() ignore error: %d", err )
    	    }
    	}
    else
    	{
        TAlfWServInfo wservInfo = aWindow.WsInfo();

        TInt offset;
        TAlfWindowAttributes* windowAttributes = CreateWindowAttributes(offset);

        if (windowAttributes)
            {
            windowAttributes->iBitmapHandle = aOutBuffer->Handle();
            TAlfBridgerData d;
            d.Set(EAlfDSSetCoveringBitmap, 
                wservInfo.iRefId.iWindowGroupId, 
                wservInfo.iRefId.iWindowIdentifer, 
                (TAny*)offset); 

            PostIt(d);
            }
    	}
    }
    

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//   
CAlfWindowManager::CAlfWindowManager()
    {
        
    }
    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::ConstructL(CAlfHierarchyModel* aAlfHierarchy)
    {
    iData = new (ELeave) TAlfWindowMgrData;
    iData->iTree = aAlfHierarchy;
    }
        

// ---------------------------------------------------------------------------
// LoadPluginL
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::LoadPluginL(const TUid& aPluginUid)
    {
    ASSERT(iData);
    if ( FindPlugin(aPluginUid) )
        {
        User::Leave(KErrAlreadyExists);
        }
        
    TUid dtoridkey;
    CAlfWindowFxPlugin* fxplugin = (CAlfWindowFxPlugin*)REComSession::CreateImplementationL(aPluginUid, dtoridkey);    
    CleanupStack::PushL(fxplugin);
    fxplugin->ConstructL(*this, EColor16MAP); // TODO: decide buffer format
    TUid* pluginUid = fxplugin->Identifier();
    
    ASSERT(pluginUid); // construction was not complete i.e. basecall missing in plugin implementation
    
    *pluginUid = aPluginUid;
    
    TPluginInfo info;
    info.iPlugin = fxplugin;
    info.iDtor = dtoridkey;
    info.iPluginUid = aPluginUid;
    iData->iPlugins.AppendL(info);
    CleanupStack::Pop();
    }


// ---------------------------------------------------------------------------
// DestroyPlugin
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::DestroyPlugin(const TUid& aPluginUid)
    {
    ASSERT(iData);
    TPluginInfo info;
    TInt pluginCount = iData->iPlugins.Count();
    for (int index = 0; index < pluginCount; index++)
        {
        info = iData->iPlugins[index];
        if (info.iPluginUid == aPluginUid)
            {
            delete info.iPlugin;
            REComSession::DestroyedImplementation(info.iDtor);
            iData->iPlugins.Remove(index);
            break;    
            }
        }
    }


// ---------------------------------------------------------------------------
// HandlePluginMsgL
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::HandlePluginMsgL(const RMessage2& aMessage)
    {
    ASSERT(iData);
    CAlfWindowFxPlugin* fxplugin = FindPlugin(TUid::Uid(aMessage.Int0()));
    if (fxplugin)
        {
        TInt lenght1 = aMessage.GetDesLength(1);
        TInt lenght2 = aMessage.GetDesMaxLength(2);
     
        HBufC8* inHBuf = HBufC8::NewLC(lenght1);
        TPtr8 inBuf = inHBuf->Des();
        aMessage.ReadL(1,inBuf); 
        
        if ( aMessage.Function() == EAlfDecodSSendSynch )
            {
            HBufC8* outBuf = HBufC8::NewLC(lenght2);
            TPtr8 outptr = outBuf->Des();
            fxplugin->HandleMessageL(inBuf, outptr);
            
            if(!aMessage.IsNull())
                {
                aMessage.WriteL(2,*outBuf);
                aMessage.Complete(KErrNone);
                }   
            
            CleanupStack::PopAndDestroy();
            }
        else
            {
            fxplugin->HandleMessageL(inBuf, aMessage);
            }
    
        CleanupStack::PopAndDestroy();
        }
    else
        {
        aMessage.Complete(KErrNotFound);
        }
    }


// ---------------------------------------------------------------------------
// CancelPluginMsg
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::CancelPluginMsg(const TUid& aPluginUid, TInt aMessageId)
    {
    if (!iData)
        {
        return;
        }
        
    if (aPluginUid == KNullUid)
        {
        for (TInt i = iData->iPlugins.Count()-1; i >= 0 ; i--)
            {
            iData->iPlugins[i].iPlugin->CancelMessage(aMessageId);
            }
        }
    else
        {
        CAlfWindowFxPlugin* fxplugin = FindPlugin(aPluginUid);
        if (fxplugin)
            {
            fxplugin->CancelMessage(aMessageId);
            }
        }    
    }


// ---------------------------------------------------------------------------
// PostIt
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CAlfWindowManager::PostIt(const TAlfBridgerData& aData)
    {
//    RDebug::Print(_L("CAlfWindowManager::PostIt") );
    return iData->iTree->Server().Bridge()->AddData(aData.iOp, aData.iInt1, aData.iInt2, aData.iPtr);
    }

// ---------------------------------------------------------------------------
// Bridge
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfStreamerBridge* CAlfWindowManager::Bridge()
    {
    return iData->iTree->Server().Bridge();
    }

// ---------------------------------------------------------------------------
// PrepareNewFrame
// ---------------------------------------------------------------------------
//   
void CAlfWindowManager::PrepareNewFrame(TUint aEstimatedInterval)
    {
    for (TInt i = iData->iPlugins.Count()-1; i >= 0 ; i--)
        {
        TRAP_IGNORE(iData->iPlugins[i].iPlugin->PrepareFrameL(aEstimatedInterval));
        // for now, just ignore error. When collecting complete frames, we could use this info though
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAlfWindowAttributes* CAlfWindowManager::CreateWindowAttributes(TInt& aIndex)
    {
    TAlfWindowAttributes empty;
    TAlfWindowAttributes* attributes = NULL;
    TRAP_IGNORE(attributes = (TAlfWindowAttributes*)Bridge()->AppendVarDataL( sizeof(TAlfWindowAttributes), aIndex ))
    *attributes = empty;
    return attributes;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAlfWindowCommandBufferAttributes* CAlfWindowManager::CreateWindowCommandBufferAttributes(TInt& aIndex)
    {
    TAlfWindowCommandBufferAttributes empty;    
    TAlfWindowCommandBufferAttributes* windowCommandBuffer = NULL;    
    TRAP_IGNORE(windowCommandBuffer = (TAlfWindowCommandBufferAttributes*)Bridge()->AppendVarDataL( sizeof(TAlfWindowCommandBufferAttributes), aIndex ))    
    *windowCommandBuffer = empty;
    return windowCommandBuffer;
    }


void CAlfWindowManager::HandleClientExit(TThreadId aThreadId)
    {
    for (TInt i = iData->iPlugins.Count()-1; i >= 0 ; i--)
        { // assuming that plugins won't actually change the order on this message 
          //(i.e. they may destroy them selves but not the plugine inserted before them)
        iData->iPlugins[i].iPlugin->ClientAboutToExit(aThreadId);
        } 
    }


