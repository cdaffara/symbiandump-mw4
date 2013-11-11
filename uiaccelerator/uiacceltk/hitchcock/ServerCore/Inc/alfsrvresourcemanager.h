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
* Description:   Manager for cross-session resources.
*
*/



#ifndef C_ALFSRVRESOURCEMANAGER_H
#define C_ALFSRVRESOURCEMANAGER_H

#include <e32base.h>
#include <hwrmlight.h>
#include <uiacceltk/HuiEnv.h>

class CAlfAppSrvSessionBase;

/**
 *  Manager for cross-session resources.
 *
 *  Handles the scheduler pausing when the lights go off or the 
 *  application server goes into the background. 
 *
 *  This class is a candidate for adding logic when 
 *  releasing/restoring textures based on the cache side.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfSrvResourceManager ): 
    public CBase, 
    public MHWRMLightObserver
    {
    
public:

    /**
     * Two-phased constructor
     *
     * @param aEnv Environment.
     * @return New object. Ownership transfrerred.
     */
    static CAlfSrvResourceManager* NewL( CHuiEnv& aEnv );
    
    /**
     * Destructor.
     */
    virtual ~CAlfSrvResourceManager();

    /**
     * Called when the visibility of the appserver window changes.
     *
     * @param aVisible ETrue if the window is fully/partially visible
     *                 EFalse if the window is totally hidden
     */
    void SetServerWindowPartiallyVisible( TBool aVisible );
    
    /**
     * Called when the active session changes.
     *
     * @param aSession Can be NULL.
     */
    void SetActiveSession( CAlfAppSrvSessionBase* aSession );

private:

    /**
     * Constructor
     */
    CAlfSrvResourceManager( CHuiEnv& aEnv );
    
    /**
     * Constructor
     */
    void ConstructL();
    
    /**
     * From MHWRMLightObserver
     *
     * Called when light status changes.
     */
    void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);
    
    /**
     * Updates the scheduler state based on the current status.
     */
    void UpdateSchedulerState();
    
    /**
     * Restarts the scheduler.
     */
    void Run();
    
    /**
     * Pauses the scheduler.
     */
    void Pause();
    
    /**
     * Return ETrue if any display lights are on
     * @return ETrue if display lights are ON.
     */
    TBool LightsOn() const;

private: // data

    /**
     * Environment. 
     * Not own.
     */
    CHuiEnv& iEnv;
    
    /**
     * Currently active session. Can be NULL. 
     * Not own.
     */
    CAlfAppSrvSessionBase* iActiveSession;
        
    /**
     * Internal flags.
     */
    TUint iFlags;
    
    /**
     * Lights controller/observer.
     * Own.
     */
    CHWRMLight* iLight; 
    };

#endif // C_ALFSRVRESOURCEMANAGER_H
