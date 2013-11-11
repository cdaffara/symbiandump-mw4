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



#ifndef C_ALFDEBUGEXTENSION_H
#define C_ALFDEBUGEXTENSION_H

#include <e32base.h>

_LIT( KAlfDebugExtensionLibraryName, "alfdebug.dll" );
const TInt KAlfDebugExtensionLibraryEntryOrdinal = 1;

class CAlfEnv;

/**
 * Measurement structure for server.
 */
struct TAlfDebugServerMeasurements
    {
    TUint32 iTimeStamp;
    TInt iServerCells;
    TInt iServerMemory;
    TInt iServerFree;
    TUint iFrameCount;
    };
        
/**
 *  ?one_line_short_description
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CAlfDebugExtension ) : public CBase
    {
    
public:
        
    /**
     * Sets the time factor that affects the internal absolute clock, which
     * is returned by Time(). Zero stops the progress of time completely,
     * 0.5 means that time  progresses in half the speed compared to real
     * time (1.0).
     *
     * @param aTimeFactor  Time factor.
     */
    virtual void SetTimeFactor(TReal32 aTimeFactor) = 0;

    /**
     * Determines the current time factor.
     *
     * @return  Time factor.
     */
    virtual TReal32 TimeFactor() = 0;

    /**
     * Determines the number of frames shown.
     *
     * @return  Frame count since the system first began rendering.
     */
    virtual TUint FrameCount() = 0;
    
    /**
     * Determines the current frame rate. The rate is calculated at most
     * once per second. Calling this less frequently causes the rate to be
     * calculated as an average since the last time this was called.
     *
     * @return  Frames per second.
     * 
     */
    virtual TReal32 FrameRate() = 0;
    
    
    /**
     * Shows the frame rate on screen
     *
     * @param aInterval Interval in milliseconds when the framerate is 
     *                  measured and shown
     *
     *                  Special cased (from alfdebugextensionconstants.h)
     *                  KAlfDebugHideFraweRate (0) Hides the framerate counter
     *                  KAlfDebugShowFraweRateAfterDrawLoop (-1) Updates frame rate
     *                      on screen when a drawing sequence ends (idle-idle)
     *                  KAlfDebugShowFraweRateWhenQueried (-2) updates the screen 
     *                      when user calls FrameCount().
     *
     */ 
    virtual void ShowFrameRate( TInt aInterval = 500 ) = 0;
       
    virtual void ShowServerHeapUsage( TBool aShow = ETrue ) = 0;

    /**
     * Measures performance from the server side.
     * @param aResult this will contain results of measurements.
     * @return error code.
     */
    virtual TInt GetMeasurements( TAlfDebugServerMeasurements& aResult ) = 0;
    
    };


NONSHARABLE_CLASS( CAlfDebugFactory ) : public CBase
    {
public:
    virtual CAlfDebugExtension* CreateDebugExtensionL( CAlfEnv* aEnv = NULL ) = 0; 
    };


IMPORT_C CAlfDebugFactory* CreateDebugExtensionFactoryL();


#endif // C_ALFDEBUGEXTENSION_H
