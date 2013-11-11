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
* Description:   Manages window and plugin life cycles 
*
*/


#ifndef __ALFWINDOWMANAGER_H__
#define __ALFWINDOWMANAGER_H__

#include <e32base.h>
#include <alfwindowstructs.h>

class CAlfHierarchyModel;
class CAlfWindow;
class CAlfWindowFxPlugin;
class CAlfWindowBuffer;
class CAlfStreamerBridge;
class TAlfBridgerData;
class TAlfWindowAttributes;
class TAlfWindowCommandBufferAttributes;

/**
 *  CAlfWindowManager
 *
 *  @lib alfappservercore.dll
 *  @since S60 5.0
 */
NONSHARABLE_CLASS(CAlfWindowManager): public CBase
    {
public:
    
    static CAlfWindowManager* NewL(CAlfHierarchyModel* aAlfHierarchy);
    
    virtual ~CAlfWindowManager();
    
    // Utils
    /**
     * CreateNewWindowL
     *
     * @since S60 5.0
     * @param aIndex
     * @param aSizeInPixels
     * @return New window
     */
    IMPORT_C CAlfWindow* CreateNewWindowL(TUint aIndex, const TSize& aSizeInPixels);
    
    /**
     * DestroyWindow
     *
     * @since S60 5.0
     * @param aWindow
     */
    IMPORT_C void DestroyWindow(CAlfWindow& aWindow);
    
    /**
     * EnableNewWindowNotificationsL
     *
     * @since S60 5.0
     * @param aPlugin
     * @param aEnable   New notification state
     */
    IMPORT_C void EnableNewWindowNotificationsL(CAlfWindowFxPlugin& aPlugin, 
                                                TBool aEnable = ETrue);

    /**
     * EnableDestroyWindowNotificationsL
     *
     * @since S60 5.0
     * @param aPlugin
     * @param aEnable   New notification state for window destruction.
     */
                                                
	IMPORT_C void EnableDestroyWindowNotificationsL(CAlfWindowFxPlugin& aPlugin, 
                                                TBool aEnable = ETrue);                                                
    /**
     * ReserveL
     *
     * Reserves exclusive right to use the window. 
     * Leaves with value KErrNotFound, if window is not found. 
     * Leaves with value KErrInUse, If window is already reserved.
     *  
     * @since S60 5.0
     * @param aId
     * @param aReserveBuffer reserve data buffer for effects
     * @return the reserved window
     */
    IMPORT_C CAlfWindow* ReserveL( const TWindowIdentifier& aId, TBool aReserveBuffer = ETrue );

    /**
     * Release
     *
     * Releases window
     * 
     * @since S60 5.0
     * @param aWindow
     * @param aReserveBuffer release the data buffer reserved for effects
     */
    IMPORT_C void Release( CAlfWindow& aWindow, TBool aReserveBuffer = ETrue );
    
    /**
     * aWindow
     *
     * @since S60 5.0

     * @return screen id
     */
    IMPORT_C TInt ScreenId() const;

	#ifdef RND_FXPLUGIN_FIX
    
    /**
     * aWindow
     *
     * @since S60 5.0

     * @return screen id
     */
    IMPORT_C void GetWinIdListL(RArray<TWindowIdentifier>& aWinIdList);
    
	#endif

    /**
     * Find all windows whose Window Server Group Id matches the one given as input
     * 
     * @param aWsGroupId Window server group id
     * @param aWinIdList list of window info structures whose group id matches the group given
     * 
     */
    IMPORT_C void GetWinIdListL( TInt aWsGroupId, RArray<TWindowIdentifier>& aWinIdList );
    
    /**
     * Find all windows whose Window Server Group Id matches the one given as input
     * The purpose is to be able to convert from window server identifiers to alf indentifiers
     * 
     * @param aWsGroupId Window server group id
     * @param aClientWindowHandle handle of a single window
     * @param aWinInfoList list of window info structures whose group id matches the group given
     *        if aClientWindowHandle != KErrNotFound, it must also match
     * 
     */
    IMPORT_C void GetWinInfoListL( TInt aWsGroupId, TInt aClientWindowHandle, RArray<TAlfWServInfo>& aWinInfoList );
    
    // Internal Utils
    
    /**
     * MoveWindow
     *
     * @since S60 5.0
     * @param aWindow
     * @param aIndex    New index for the window. The topmost is having index 0. 
     */
    void MoveWindow(CAlfWindow& aWindow, TInt aIndex);

    /**
     * SetWindowVisibility
     *
     * @since S60 5.0
     * @param aWindow
     * @param aVisible  New visibility state
     */
    void SetWindowVisibility(CAlfWindow& aWindow, TBool aVisible);

    /**
     * ContentReady
     *
     * Call to ContentReady will trigger update for all textures.
     * 
     * @since S60 5.0
     * @param aWindow   
     * @param aOutBuffer
     */
    void ContentReady(CAlfWindow& aWindow, CAlfWindowBuffer* aOutBuffer);

    /**
     * LoadPluginL
     *
     * @since S60 5.0
     * @param aPluginUid    Id for the plugin to be created
     */
    void LoadPluginL(const TUid& aPluginUid);

    /**
     * DestroyPlugin
     *
     * @since S60 5.0
     * @param aPluginUid    Id for the plugin to be destroyed
     */
    void DestroyPlugin(const TUid& aPluginUid);

    /**
     * PrepareNewFrame
     *
     * @since S60 5.0
     * @param aEstimatedInterval
     */
    void PrepareNewFrame(TUint aEstimatedInterval);

    /**
     * HandlePluginMsgL
     *
     * @since S60 5.0
     * @param aMessage  Message for message
     */
    void HandlePluginMsgL(const RMessage2& aMessage);

    /**
     * CancelPluginMsg
     *
     * @since S60 5.0
     * @param aPluginUid    Id of the plugin
     * @param aMessageId    Id for the message to be canceled
     * @return ?description
     */
    void CancelPluginMsg(const TUid& aPluginUid, TInt aMessageId);

    /**
     * CreateNewWindowL
     *
     * @since S60 5.0
     * @param aIndex    Order of window. Index 0 means the topmost.
     * @param aInfo     Window info 
     * @return ?description
     */
    CAlfWindow* CreateNewWindowL(TUint aIndex, const TAlfWServInfo& aInfo);

    /**
     * PostIt
     *
     * Posts command for asyncronous processing
     * 
     * @since S60 5.0
     * @param aData Command data
     * @return KErrNone, if data was succesfully posted. Otherwise an error code.
     */
    IMPORT_C TInt PostIt(const TAlfBridgerData& aData);
    
    IMPORT_C CAlfStreamerBridge* Bridge();
    
    void HandleClientExit(TThreadId aThreadId);
    
private: // utils

    CAlfWindowManager();
    
    void ConstructL(CAlfHierarchyModel* aAlfHierarchy);
    
    /**
     * FindPlugin
     *
     * @since S60 5.0
     * @param ?arg1 ?description
     * @return Pointer to requested plugin. 0 if the requested plugin was not found.
     */
    CAlfWindowFxPlugin* FindPlugin(const TUid& aPluginUid);

    void BindWindowToHitchL(CAlfWindow* aWindow, CAlfWindowManager* aMgr);

    TAlfWindowAttributes* CreateWindowAttributes(TInt& aIndex);

    TAlfWindowCommandBufferAttributes* CreateWindowCommandBufferAttributes(TInt& aIndex);
    
private: // data
        
        class TAlfWindowMgrData;
        
        TAlfWindowMgrData* iData;
    };


#endif