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
* Description:   Application UI definition
*
*/



#ifndef C_ALFAPPUI_H
#define C_ALFAPPUI_H

#include <aknappui.h>
#include <alf/alfconstants.h>

class CHuiEnv;
class CHuiRoster;
class CHuiControlGroup;
class CHuiVisual;
class CAlfAppServer;
class CAlfSharedDisplayCoeControl;
class CHuiDisplay;
class CAlfAppSrvSessionBase;
class CAlfSrvResourceManager;
class CAlfSrvSettingsHandler;
class RAlfBridgerClient;

enum TAlfSrvSetting
    {
    EAlfSrvSettingDefaultFramerate,
    EAlfSrvSettingMaxCpuUsage,
    EAlfSrvSettingMaxResourceCacheSize,
    EAlfSrvSettingEffectsComplexityFactor // includes the RnD flag changes.
    };

class MAlfSrvSettingsObserver
    {
public:  
    virtual void MAlfSrvUintSettingChangedL( TAlfSrvSetting aSetting, TUint aNewValue ) = 0;
    };


/**
 *  Application UI class.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfAppUi) : public CBase, public MAlfSrvSettingsObserver
    {

public: // new methods 
    
    /**
     * Constructor
     */
    IMPORT_C CAlfAppUi();

    /**
     * Destructor
     */
    IMPORT_C ~CAlfAppUi();
    
    /**
     * Retuns a reference to CHuiEnv object.
     * @return HuiEnv. Ownership not transferred.
     */
    CHuiEnv& HuiEnv();
    
    /**
     * Called when all clients have existed.
     */
    void AllClientsClosed();
    
    /**
     * Called when client is about to exit.
     * @param aClientId Client to be removed.
     */
    void FreeResourcesForClient( TInt aClientId );
    
    /**
     * Returns a common window-owning control
     * @param Control. NULL if not used. Ownership not transferred.
     */
    IMPORT_C CCoeControl* Container();
    
    /**
     * Adds display into the common container.
     * @param aDisplay to be added.
     */
    IMPORT_C void AppendDisplayOnSharedWindowL( CHuiDisplay& aDisplay );

    void AppendTvDisplayOnSharedWindowL( CHuiDisplay& aDisplay );

    void RemoveTvDisplayOnSharedWindow( CHuiDisplay& aDisplay );
    
    CHuiDisplay* CAlfAppUi::FindTvDisplayOnSharedWindow();
    
    CAlfSrvSettingsHandler& SettingsHandler();
    
    /**
     * Sets window server fading to common window-owning control.
     * @param aFading ETrue if it needs to be faded, EFalse otherwise.
     */
    void SetContainerNonFading( TBool aFading );
    
public: // from base classes    

// from base class CAknAppUi

    /**
     * From CAknAppUi.
     * 2nd phase constructor. Called by the framework after 
     * the 1st phase construction.
     */
    IMPORT_C virtual void ConstructL();
    
    /**
     * From CAknAppUi.
     * Called by the frawework when a system event is received.
     *
     * @param aEvent Occurred event.
     */
    IMPORT_C virtual void HandleSystemEventL(const TWsEvent& aEvent);

    /**
     * From CAknAppUi.
     * Called by the frawework when a system event is received.
     *
     * @param aEvent Occurred event.
     */    
    IMPORT_C virtual void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    
    /**
     * From CAknAppUi.
     * Called when framework calls rendezvous.
     *
     * @return ETrue. Meaning the framework will synchronize the application to the starter
     */
    IMPORT_C virtual TBool FrameworkCallsRendezvous() const;
    
    /**
     * From CAknAppUi.
     * Called when system resources changes.
     *
     * @param aType Changed resource. 
     */
    IMPORT_C virtual void HandleResourceChangeL( TInt aType );
   
   /**
     * From CAknAppUi.
     * Called by the frawework when a command is received.
     *
     * @param aCommand The command
     */
    IMPORT_C virtual void HandleCommandL(TInt aCommand);
    
    void MAlfSrvUintSettingChangedL( TAlfSrvSetting aSetting, TUint aNewValue );
    
    /**
     * Posts QTcommand  buffer from clientwindow  to CanvasVisual
     *
     * @param  aParams Commandbuffer parameters containg window information
     *                 and QT commandbuffer
     */        
    void PostQTCommandBufferL( TAlfQtCommandBufferParams aParams );
public: // internal utilities
    void StartPointerEventHandling();
    void EndPointerEventHandling();
    void UpdateActiveSession(CAlfAppSrvSessionBase* aSession);
    
    CAlfAppSrvSessionBase* ActiveSession();
    
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    TInt FirstAlfControlGroupIndex( TInt aScreenNumber = 0);    
    TInt LastAlfControlGroupIndex( TInt aScreenNumber = 0);    
    void ShowControlGroupL(CHuiRoster& aRoster, CHuiControlGroup& aGroup, TInt aWhere, TInt aScreenNumber );
	void SetClientWindowForDrawingL(TInt aWindowGroupId, TInt aClientWindowHandle, CHuiVisual* aVisual);
    /**
     * Moves the server window group relative to the clinet (parent) wg.
     *
     * @param aWindowGroup Server window group
     * @param aParentIdentifier Client side window group
     * @param aPosition Server's wg position compared to the client's wg.
     */
    void AdjustWindowGroupPositionL(
        TInt aParentIdentifier, 
        TInt aPosition );
    void SetAlfWindowGroupId(TInt aWgId);
    void NotifyLowMemory(TInt aAmountOfFreeMemRequested);
    TInt ForceSwRendering(TBool aEnabled);
    TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation);
    TInt ReadPixels(CFbsBitmap* aBitmap);
    void SetAlfAppWindowGroup( TInt aId );
    void RemoveAlfAppWindowGroup( TInt aId );
    
    CAlfAppSrvSessionBase* SrvSessionForControlGroup(CHuiControlGroup& aGroup);
    void DoBlankScreen(const RMessage2& aMessage);
    
    TInt GetLastActiveClient();
    RAlfBridgerClient* BridgerClient();

private:

    void CreateHuiEnvL();
    
private:

    class CAlfAppUiData;
    CAlfAppUiData* iData;
    };

#endif // C_ALFAPPUI_H
