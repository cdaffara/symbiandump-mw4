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
* Description:   This direct client to alf server, that does not require Alf 
*                environment.
*                
*/


#ifndef _ALFDIRECTCLIENT_H_
#define _ALFDIRECTCLIENT_H_

#include <alf/alfclientbase.h>
#include <alf/alfconstants.h>

NONSHARABLE_CLASS( RAlfDirectClient ): public RAlfClientBase
    {
    public:
    /**
     * Constructor
     */
    IMPORT_C RAlfDirectClient();
    
    /**
     * Disconnect from the server. Must be called before destruction
     */ 
    IMPORT_C void Disconnect();
    
    
    /** 
     * MeasureFPS
     * 
     * Several FPS counters can be ongoing simultaniously. If a measurement is 
     * started for already ongoing measurement identifier, then the first 
     * measurement is disgarded.
     * 
     * @param   aId Unique identifier for the measurement.
     * @param   aStart  ETrue, if starting measurement. EFalse, if stopping 
     *          and logging measurement result.
     */
    IMPORT_C void MeasureFPS( TInt32 aId, TBool aStart );
    
    /**
     * EnableLowMemoryState
     *
     * Enables low memory state in ALF
     * 
     * @param aMode ETrue to enable low memory, EFalse to disable  
     */
    IMPORT_C void EnableLowMemoryState( TBool aMode, TBool aUseSwRendering);

    /**
     * Forces SW rendering to be used. 
     * This method is intended to be only used for testing
     * of SW rendering.
     * @param aEnabled ETrue - SW rendering enabled, 
     *                 EFalse - SW rendering disabled.
     * @return error code.
     */
    IMPORT_C TInt ForceSwRendering( TBool aEnabled );
    
    /**
     * Gets display size and virtual rotation.
     * @param aSize this will contain size.
     * @param aRotation this will contain rotation.
     * @return error code, KErrNone upon success.
     * @internal
     */
    TInt GetSizeAndRotation(TSize& aSize, TInt& aRotation);
    
    /**
     * Reads pixels from surface to bitmap.
     * @param aBitmapHandle bitmap handle.
     * @return error code, KErrNone upon success.
     * @internal
     */
    TInt ReadPixels(TInt aBitmapHandle);
    
    /**
     * NGA Substitute for Avkon screen blanker.
     * Only AknCapserver can use this method (protected using SID)
     * Does not reference count, i.e. all calls are absolute 
     * @return error code.
     */
    IMPORT_C TInt BlankScreen( TBool aEnabled );    
    };

#endif
