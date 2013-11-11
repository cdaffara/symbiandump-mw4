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
* Description:  
*
*/



// INCLUDE FILES

#include "minimapscaler.h"

#include <fbs.h>
#include <bitmaptransforms.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMinimapScaler::CMinimapScaler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMinimapScaler::CMinimapScaler(MMinimapScalerCallback& aCallback)
    : CActive(CActive::EPriorityIdle-1), iCallback(&aCallback)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMinimapScaler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMinimapScaler::ConstructL()
    {
    iScaler = CBitmapScaler::NewL();
    }

// -----------------------------------------------------------------------------
// CMinimapScaler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMinimapScaler* CMinimapScaler::NewL(MMinimapScalerCallback& aCallback)
    {
    CMinimapScaler* self = new( ELeave ) CMinimapScaler(aCallback);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CMinimapScaler::~CMinimapScaler()
    {       
    Cancel();
    delete iScaler;
    delete iResultBitmap;
    }


// -----------------------------------------------------------------------------
// CMinimapScaler::StartScalingL
// 
// 
// -----------------------------------------------------------------------------
//
void CMinimapScaler::StartScalingL(CFbsBitmap& aSource, const TRect& aTargetRect)
    {
    // cancel outstanding request
    Cancel();
    
    if (!iResultBitmap || iResultBitmap->SizeInPixels() != aTargetRect.Size())
        {        
        DeleteResultBitmap();
        // create target bitmap
        iResultBitmap = new (ELeave) CFbsBitmap();
        User::LeaveIfError(iResultBitmap->Create(aTargetRect.Size(),aSource.DisplayMode()));              
        }   
    // start scaling, async        
    iTargetRect = aTargetRect;
    iScaler->Scale(&iStatus, aSource, *iResultBitmap, EFalse);
    
    SetActive();     
    }    


// -----------------------------------------------------------------------------
// CMinimap::DeleteResultBitmap
// 
// 
// -----------------------------------------------------------------------------
//
void CMinimapScaler::DeleteResultBitmap()
    {
    delete iResultBitmap;    
    iResultBitmap = 0;
    }
        
        
// -----------------------------------------------------------------------------
// CMinimap::DoCancel
// 
// 
// -----------------------------------------------------------------------------
//
void CMinimapScaler::DoCancel()
    {
    iScaler->Cancel();
    DeleteResultBitmap();
    }
    
    
// -----------------------------------------------------------------------------
// CMinimap::RunL
// 
// 
// -----------------------------------------------------------------------------
//
void CMinimapScaler::RunL()
    {
    iCallback->ScalingCompletedL(*iResultBitmap, iTargetRect);
    // if the callback called StartScalingL(), we must not delete the bitmap
    if (!IsActive())
        {
        DeleteResultBitmap();
        }
    }    


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
