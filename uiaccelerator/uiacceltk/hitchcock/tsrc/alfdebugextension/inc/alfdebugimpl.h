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



#ifndef C_ALFDEBUGEXTENSIONOIMPL_H
#define C_ALFDEBUGEXTENSIONOIMPL_H

#include "alfdebug.h"

class CAlfEnv;
class CAlfGenComponent;

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
NONSHARABLE_CLASS( CAlfDebugExtensionImpl ): public CAlfDebugExtension
    {
    
public:
    
    static CAlfDebugExtensionImpl* NewL( CAlfEnv& aEnv );
    ~CAlfDebugExtensionImpl();
    
public:
    
    /**
     * Sets the time factor that affects the internal absolute clock, which
     * is returned by Time(). Zero stops the progress of time completely,
     * 0.5 means that time  progresses in half the speed compared to real
     * time (1.0).
     *
     * @param aTimeFactor  Time factor.
     */
    void SetTimeFactor(TReal32 aTimeFactor);

    /**
     * Determines the current time factor.
     *
     * @return  Time factor.
     */
    TReal32 TimeFactor();

    /**
     * Determines the number of frames shown.
     *
     * @return  Frame count since the system first began rendering.
     */
    TUint FrameCount();
    
    /**
     * Determines the current frame rate. The rate is calculated at most
     * once per second. Calling this less frequently causes the rate to be
     * calculated as an average since the last time this was called.
     *
     * @return  Frames per second.
     * 
     */
    TReal32 FrameRate();
    
    void ShowFrameRate( TBool aShow = ETrue );
    
    void ShowServerHeapUsage( TInt aInterval = 500 );

    /**
     * Measures performance from the server side.
     * @param aResult this will contain results of measurements.
     * @return error code.
     */
    virtual TInt GetMeasurements( TAlfDebugServerMeasurements& aResult );
    
private:

    CAlfDebugExtensionImpl();

    void ConstructL( CAlfEnv& aEnv );

private: // data
        
    CAlfEnv* iEnv;
    CAlfGenComponent* iComms;
    };
    
    
NONSHARABLE_CLASS( CAlfDebugFactoryImpl ) : public CAlfDebugFactory
    {
public:
    ~CAlfDebugFactoryImpl();
    CAlfDebugExtension* CreateDebugExtensionL( CAlfEnv* aEnv = NULL ); 
    
private:
    CAlfEnv* iOwnEnv;
    };


#endif // C_ALFDEBUGEXTENSIONOIMPL_H
