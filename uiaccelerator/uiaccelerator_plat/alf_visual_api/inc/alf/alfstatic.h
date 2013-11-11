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
* Description:   Static class.
*
*/



#ifndef __ALFSTATIC_H__
#define __ALFSTATIC_H__

#include <e32base.h>

/* Forward declarations. */
class CAlfEnv;

/**
 * Miscellaneous globally used functions, and thread local storage space.
 * 
 * CAlfStatic contains all the functions that tend to be used across classes.
 * These functions are written in a non-OO fashion because
 * they are used everywhere, so passing data such as delta step down through the
 * entire class heirarchy is inefficient.
 */
NONSHARABLE_CLASS(CAlfStatic) : public CBase
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructor. The user application should not explicitly
     * construct a CAlfStatic - it is constructed by CHuiEnv.
     *
     * @param aPrimaryEnv The toolkit environment. 
     */
    IMPORT_C static CAlfStatic* NewLC(CAlfEnv* aPrimaryEnv);

    /**
     * Constructor. The user application should not explicitly
     * construct a CAlfStatic - it is constructed by CHuiEnv.
     *
     * @param aPrimaryEnv The toolkit environment. 
     */
    IMPORT_C static CAlfStatic* NewL(CAlfEnv* aPrimaryEnv);


    /**
     * Destructor.
     */
    virtual ~CAlfStatic();


    /* Methods. */

    /**
     * Determines the primary CHuiEnv instance.
     */
    IMPORT_C static CAlfEnv& Env();

protected:

    /* Constructors. */

    /**
     * Constructor. Should not be called through the API.
     */
    CAlfStatic();

    /**
     * Second-phase constructor.
     * @param aPrimaryEnv Pass in the environment.
     */
    void ConstructL(CAlfEnv* aPrimaryEnv);


private:

    /* Private methods */

    /**
     * Returns a pointer to the thread-local data struct.
     */
    IMPORT_C static struct TTlsData* Data();

    /**
     * Updates the toolkit's time counters. This includes the toolkit's
     * realtime clock, the internal absolute clock (which is affected by the
     * time factor), the amount of elapsed time since last UpdateTime()
     * invocation, and the amount of elapsed time since the first UpdateTime()
     * invocation (which was done in the beginning of the first refresh).
     * 
     * @param aData TLS Data which encapsulates the internal time variables.
     */
    static void UpdateTime(struct TTlsData* aData);

private:

    /**
     * Thread local storage for this DLL, as we cannot declare static
     * global variables in Symbian.
     */
    struct TTlsData* iData;

    friend class CAlfEnv;
    };

#endif //__ALFSTATIC_H__