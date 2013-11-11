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
* Description:   ?Description
*
*/



#ifndef C_ALFSRVSETTTINGSHANDLER_H
#define C_ALFSRVSETTTINGSHANDLER_H

#include <e32base.h>
#include <uiacceltk/HuiRenderer.h>

class CRepository;
class MAlfSrvSettingsObserver;

enum TAlfSrvRndFlags
    {
    EAlfSrvRndFlagFirst = 0x0100,
    
    // For drawing the dirty regions
    EAlfSrvRndFlagDrawDirtyRegions = EAlfSrvRndFlagFirst,
    
    // For drawing visual outline for all visuals
    EAlfSrvRndFlagDrawAllVisualOutline = EAlfSrvRndFlagFirst << 1, // 0x0200 if first 0x0100
    // For drawing visual outline for selected visuals and their children
    EAlfSrvRndFlagDrawSelectedVisualOutline = EAlfSrvRndFlagFirst << 2, // 0x0400 if first 0x0100

    // To enable texture memory usage calculation
    EAlfSrvRndFlagEnableTextureMemoryCalculation = EAlfSrvRndFlagFirst << 3 // 0x0800 if first 0x0100    
    };

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CAlfSrvSettingsHandler ) : public CActive
    {

public:

    static CAlfSrvSettingsHandler* NewL( MAlfSrvSettingsObserver& aObserver );

    virtual ~CAlfSrvSettingsHandler();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    THuiRenderer Renderer();
    
    TUint DefaultFramerate();
    
    TUint MaxCpuUsage();
    
    TUint MaxResourceCacheSize();
    
    TUint EffectsComplexityFactor();

    TUint HWDriverConfiguration();
    
    TUint RndFlags();

protected:

// from base class CActive

    /**
     * From CActive
     * Run loop of active object.
     */
    void RunL();
    
    /**
     * From CActive
     * Cancel execution.
     */    
    void DoCancel();
    
    /**
     * From CActive
     * Called on an error within the RunL loop. Performs any
     * cleanup operations.
     * @param aError The error that occured.
     * @return the error.
     */        
    TInt RunError(TInt aError);

private:

    CAlfSrvSettingsHandler( MAlfSrvSettingsObserver& aObserver );

    void ConstructL();
    
    void StartObserving();

private: // data

    /**
     * ?description_of_member
     */
    MAlfSrvSettingsObserver& iObserver;
    CRepository* iCentralRepository;
    TUint iMaxCpuUsage; // Local cached value
    };


#endif // C_ALFSRVSETTTINGSHANDLER_H
