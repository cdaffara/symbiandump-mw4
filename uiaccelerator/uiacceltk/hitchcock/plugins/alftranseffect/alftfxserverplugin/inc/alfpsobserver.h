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
* Description: 
*
*/

#ifndef ALFPSOBSERVER_H
#define ALFPSOBSERVER_H

#include <e32base.h>
#include <e32property.h>

const TUid KTestCategoryUid = { 0x10281f87 };
const TUint KTfxClientKey =  0x12341234;
const TUint KTfxServerKey = 0x12341235;
const TUint KTfxServerCompFsKey = 0x12341236;



const TUint KRandomMask = 0x80000000;
const TUint KGetHeapSize = 0x90000000;

// Set by the testee when it has finished setting up it's stuff, signalling
// that the test app can start running its test.
const TUint KTestInitComplete = 0xffff0000;

// Set by the test app when it has finished running a test.
const TUint KTestFinished = 0xffffffff;

// Set by the testee when it determines that the test round is finished.
// Used for deterministic memory allocation failure tests.
const TUint KTestRoundComplete = 0xffffff00;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
NONSHARABLE_CLASS( MPsObserver )
    {
public:
    virtual void PsValueUpdated( const TUid aCategory, const TUint aKey,
                                 const TInt aVal ) = 0;
    };

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
NONSHARABLE_CLASS( CAlfPsObserver ) : public CActive
    {
public:
    static CAlfPsObserver* NewL( MPsObserver* aClient, const TUid aCategory, 
                              const TUint aKey );
    static CAlfPsObserver* New( MPsObserver* aClient, const TUid aCategory, 
                              const TUint aKey );
    ~CAlfPsObserver();
    
    // From CActive
    virtual void DoCancel();
    virtual void RunL();
    
private:
    CAlfPsObserver( MPsObserver* aClient, const TUid aCategory, 
                 const TUint aKey);
    void Construct( const TUid aCategory, const TUint aKey );

private: // data
    RProperty iProperty;
    MPsObserver* iClient;
    TUid iCategory;
    TUint iKey;
    };

#endif // ALFPSOBSERVER_H
