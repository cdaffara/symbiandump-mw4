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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMRUNPLUGIN_H_
#define GOOMRUNPLUGIN_H_

#include "goomaction.h"

class CGOomRunPlugin;
class CGOomRunPluginConfig;
class MGOomActionObserver;
class CGOomPluginWaiter;
class CGOomMonitorPlugin;
class CGOomMonitorPluginV2;
class CGOomActionConfig;

/*
 * The GOOM action of running an GOOM plug-in to free up memory.
 * 
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomRunPlugin) : public CGOomAction
    {
public:
    static CGOomRunPlugin* NewL(TUint aPluginId, CGOomRunPluginConfig& aConfig, MGOomActionObserver& aStateChangeObserver, CGOomMonitorPlugin& aPlugin);

    ~CGOomRunPlugin();   
    
    // Run the GOOM plugin in order to free memory
    // Call the CGOomAction::MemoryFreed when it is done
    virtual void FreeMemory(TInt aBytesRequested, TBool aUseSwRendering);
    
    // Call the memory good function on the plugin but...
    // only if there is an outstanding FreeMemory request
    void MemoryGood();
        
    inline TBool IsRunning();
    
    // To be called by the CGOomPluginWaiter
    inline void WaitCompleted();
    
    TUint Id();
    
protected:
    
    void ConstructL(CGOomRunPluginConfig& aPluginConfig);    
    
    inline CGOomActionConfig& GetConfig();   
    
private:
    
    CGOomRunPlugin(TUint aPluginId, CGOomRunPluginConfig& aConfig, MGOomActionObserver& aStateChangeObserver, CGOomMonitorPlugin& aPlugin);
    
    TUint iPluginId;
    
    CGOomMonitorPlugin& iPlugin;
    
    CGOomRunPluginConfig& iConfig;
    
    CGOomPluginWaiter* iPluginWaiter;
    
    TBool iFreeMemoryCalled; // True if FreeMemory has been called since the last call to MemoryGood
    
    TBool iFreeMemoryWithSwRenderingCalled;
    };

#include "goomrunplugin.inl"

#endif /*GOOMRUNPLUGIN_H_*/
