/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef T_ALFDEBUGSERVER_H
#define T_ALFDEBUGSERVER_H

#include <alf/alfextensionfactory.h>

class CHuiControlGroup;
class CMonitorControl;

class TAlfDebugHandler : public MAlfExtensionFactory
    {
public:
    MAlfExtension* CreateExtensionL(
        const TInt aObjectId, 
        const TDesC8& aInitialParams, 
        MAlfInterfaceProvider& aResolver );
    void Release();
    };
    
class CAlfDebugServer : public CBase, public MAlfExtension
    {
public:
    CAlfDebugServer( CHuiEnv& aEnv );
    ~CAlfDebugServer();
    virtual void Release();
    virtual TAny* GetInterface( const THuiInterfaceSupport& aInterface );
    virtual void HandleCmdL( TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse );
    
    void ShowFrameRateL( TInt aInterval );
    void ShowServerHeapL( TBool aShow );
    
private:
    void CreateControlIfNeededL();
    void DeteteControlIfNeeded();

private:
    CHuiEnv& iEnv;
    
    CHuiControlGroup* iGroup;
    CMonitorControl* iControl; 
    
    // For showing the framerate
    TInt iUsedFrameRateMonitorControlGroupId;
    
public:
    // 
    TInt iFpsUpdatePeriod;
    TInt iServerHeapUpdatePeriod;
    
    };


#endif // T_ALFDEBUGSERVER_H
