/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  A class to help track memory leaks.
*
*/



#include "LeakTracker.h"
#include "Logger.h"

// Globals
#ifdef TRACK_LEAKS
CLeakTracker*  gLeakTracker = NULL;

// -----------------------------------------------------------------------------
// CLeakTracker::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLeakTracker* CLeakTracker::NewL(const TDesC& aSource)
    {
    CLeakTracker* self = new (ELeave) CLeakTracker();

    CleanupStack::PushL(self);
    self->ConstructL(aSource);
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CLeakTracker::CLeakTracker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLeakTracker::CLeakTracker():
        iValues(40)
    {
    }
        

// -----------------------------------------------------------------------------
// CLeakTracker::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLeakTracker::ConstructL(const TDesC& aSource)
    {
    //++pK : Added if condition checking for NULL
    	if ( iSource != NULL )
    	{
    		iSource = aSource.AllocL();	
    	}
    }
        

// -----------------------------------------------------------------------------
// CLeakTracker::~CLeakTracker
// Deconstructor.
// -----------------------------------------------------------------------------
//
CLeakTracker::~CLeakTracker()
    {
    TBool foundLeak = EFalse;

	if( iSource != NULL )
	{
		// Report leaks.
	    TPtrC  source(*iSource);
	    
	    if( iValues != NULL)
	    {
	    	for (TInt i = 0; i < iValues.Count(); i++)
	        {
	        if (iValues[i].value != 0)
	            {
	            FEED_LOG3(_L("Feeds"), _L("Feeds_Leaks.log"), 
	                    EFileLoggingModeAppend, _L("%S: Leak[%d]: %d"), &source, 
	                    iValues[i].valueId, iValues[i].value);

	            foundLeak = ETrue;
	            }
	        }

	    // Free the array.
	    iValues.Close();

	    }
	    
	    
	    gLeakTracker = NULL;

	    if (!foundLeak)
	        {
	        FEED_LOG1(_L("Feeds"), _L("Feeds_Leaks.log"), 
	                EFileLoggingModeAppend, _L("%S; No leaks found!!!"), &source);
	        }
	        
	    delete iSource;
			
	}
    
        
    
    }

        
// -----------------------------------------------------------------------------
// CLeakTracker::Increment
//
// Increments the ref-count on the given class type
// -----------------------------------------------------------------------------
//
void CLeakTracker::Increment(TInstType aType)
    {
    LeakTracker_Value  newValue;

    if (gLeakTracker == NULL)
        {
        return;
        }

    // Find the value-id
    for (TInt i = 0; i < iValues.Count(); i++)
        {
        if (iValues[i].valueId == aType)
            {
            iValues[i].value++;
            return;
            }
        }
    
    // The type wasn't found so append a new entry.
    newValue.valueId = aType;
    newValue.value = 1;
    
    iValues.Append(newValue);
    // Ignore errors.
    }


// -----------------------------------------------------------------------------
// CLeakTracker::Decrement
//
// Decrements the ref-count on the given class type
// -----------------------------------------------------------------------------
//
void CLeakTracker::Decrement(TInstType aType)
    {
    if (gLeakTracker == NULL)
        {
        return;
        }

    // Find the value-id
    for (TInt i = 0; i < iValues.Count(); i++)
        {
        if (iValues[i].valueId == aType)
            {
            iValues[i].value--;
            return;
            }
        }
    }

#endif
