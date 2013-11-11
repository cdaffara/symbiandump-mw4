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
* Description:   Control group
*
*/



#include "alf/alfcontrolgroup.h"
#include "alfcontrolgroupsubsession.h"
#include "alf/alfcontrol.h" 
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alftransformation.h"

#include <uiacceltk/HuiUtil.h>

// used flags
enum TAlfControlGroupFlags
    {
    EAlfControlGroupAllowInput =        0x01
    };

// private data
struct CAlfControlGroup::TPrivateData
    {
    RAlfControlGroupSubSession iControlGroupSubSession; // Owned
    TUint iFlags;                           // Owned.
    TInt iResourceId;                       // Owned.
    CAlfEnv* iEnv;                          // Not owned.
    RPointerArray<CAlfControl> iControls;   // Owned.
    CAlfTransformation* iTransform;         // Owned.
    CAlfDisplay* iBoundDisplay;				// Not owned.
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfControlGroup::CAlfControlGroup()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfControlGroup::ConstructL(TInt aResourceId, CAlfEnv& aEnv)
    {
    iData = new (ELeave) TPrivateData;
    
    User::LeaveIfError(
        iData->iControlGroupSubSession.Open(aEnv.Client(), aResourceId) );
    
    iData->iFlags = EAlfControlGroupAllowInput;
    iData->iEnv = &aEnv;
    //iData->iOpacity.Set(1.0);
    iData->iResourceId = aResourceId;
    iData->iTransform = NULL;
    iData->iBoundDisplay = NULL;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfControlGroup::~CAlfControlGroup()
    {
    if ( iData )
        {
        iData->iControls.ResetAndDestroy();
        
        delete iData->iTransform;
        iData->iTransform = NULL;
        
        iData->iControlGroupSubSession.Close();
        }
    
    delete iData;
    iData = NULL;
    }
  
// ---------------------------------------------------------------------------
// Returns server side handle
// ---------------------------------------------------------------------------
//  
TInt CAlfControlGroup::Identifier() const
    {
    return iData->iControlGroupSubSession.SubSessionHandle();
    }

// ---------------------------------------------------------------------------
// Binds this control group to the given display.
// ---------------------------------------------------------------------------
//
void CAlfControlGroup::BindDisplay(CAlfDisplay* aDisplay)
	{
	iData->iBoundDisplay = aDisplay;
	}

// ---------------------------------------------------------------------------
// Returns ID.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControlGroup::ResourceId() const
    {
    return iData->iResourceId;
    }

// ---------------------------------------------------------------------------
// Appends a control
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControlGroup::AppendL(CAlfControl* aControl)
    {
    TInt err = KErrNone;
    
    User::LeaveIfError( iData->iControls.Append(aControl) );
        
    // Update server side:
    err = iData->iControlGroupSubSession.Append( aControl->Identifier() );
    if ( err != KErrNone )
        {
        iData->iControls.Remove( iData->iControls.Count()-1 );
        User::LeaveIfError( err );
        }
    
    // Show the appended control if the control group was shown.
    if ( iData->iBoundDisplay )
    	{
        TRAP( err, aControl->ShowL( *iData->iBoundDisplay ));    	
    	}

    // If a leave occurred remove the control from client and server side control group.
    if ( err != KErrNone )
        {        
        iData->iControlGroupSubSession.Remove( aControl->Identifier() );
        iData->iControls.Remove( iData->iControls.Count()-1 );
        User::LeaveIfError( err );
        }
    
    // Everything OK.
    aControl->SetControlGroup(*this);    
    }
 
// ---------------------------------------------------------------------------
// Removes a control
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CAlfControlGroup::Remove(CAlfControl* aControl)
    {
    // Update server side:
    TInt err = iData->iControlGroupSubSession.Remove( aControl->Identifier() );
    
    // Update own array if server side is OK.
    if ( err == KErrNone )
        {
        // If found on the server side, we must find it also from the client side.
        TInt index = iData->iControls.Find(aControl);
        __ASSERT_DEBUG( index != KErrNotFound, USER_INVARIANT() );
        if(index != KErrNotFound)
            {
            iData->iControls.Remove(index);
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else
        {
        // If we cannot find it from the server side, we must not find it on the client side.
        __ASSERT_DEBUG( iData->iControls.Find(aControl) == KErrNotFound , USER_INVARIANT() );
        }
    
    if ( err == KErrNone )
        {
        CAlfControlGroup* dummy = NULL;
        aControl->SetControlGroup(*dummy);
        
        // Hide the control, if this control group has been visible
        if( iData->iBoundDisplay )
        	{
        	aControl->Hide( *iData->iBoundDisplay );
        	}
        }
    
    return err;
    }
   
// ---------------------------------------------------------------------------
// Accepts input?
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CAlfControlGroup::AcceptInput() const
    {
    return iData->iFlags&EAlfControlGroupAllowInput;
    }
    
// ---------------------------------------------------------------------------
// Set accepts input
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfControlGroup::SetAcceptInput(TBool aAcceptInput) 
    {
    if ( aAcceptInput )
        {
        iData->iFlags|=EAlfControlGroupAllowInput;
        }
    else
        {
        iData->iFlags&=~EAlfControlGroupAllowInput;
        }
    }
   
// ---------------------------------------------------------------------------
// Finds control by ID
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfControl* CAlfControlGroup::FindControl(TInt aId, TBool aUserId) const
    {
    for(TInt i = 0; i < iData->iControls.Count(); ++i)
        {
        if( (aUserId?
            iData->iControls[i]->Id():
            iData->iControls[i]->Identifier()) 
                == aId)
            {
            return iData->iControls[i];
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// Returns control count.
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfControlGroup::Count() const
    {
    return iData->iControls.Count();
    }
    
// ---------------------------------------------------------------------------
// Returns indexed control
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfControl& CAlfControlGroup::Control(TInt aIndex) const
    {
    return *iData->iControls[aIndex];
    }

// ---------------------------------------------------------------------------
// Enables transformation.
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfControlGroup::EnableTransformationL(TBool aIsTransformed)
    {
    if ( aIsTransformed && iData->iTransform )
        {
        // Already enabled
        return;
        }
    
    if ( !aIsTransformed && !iData->iTransform )
        {
        // Already disabled
        return;
        }
    
    if ( aIsTransformed )
        {
        iData->iTransform = CAlfTransformation::NewL(*iData->iEnv);
        }
   
    // use the server handle from the transformation object if present
    TInt transformationHandle = 
        iData->iTransform ? 
            iData->iTransform->ServerHandle() : 
            0 ;
            
    // send the message to the server
    TInt err = iData->iControlGroupSubSession.EnableTransformation(
        transformationHandle,
        aIsTransformed );
     
    if ( err == KErrNone && !aIsTransformed )
        {
        delete iData->iTransform;
        iData->iTransform = NULL;
        }
    
    if ( err != KErrNone )
        {
        delete iData->iTransform;
        iData->iTransform = NULL;
        User::Leave( err );
        }
    }
    
    
// ---------------------------------------------------------------------------
// Returns is transformation enabled
// ---------------------------------------------------------------------------
// 
EXPORT_C TBool CAlfControlGroup::IsTransformed() const
    {
    return iData->iTransform != 0;
    }
    
// ---------------------------------------------------------------------------
// Returns transformation
// ---------------------------------------------------------------------------
// 
EXPORT_C CAlfTransformation& CAlfControlGroup::Transformation()
    {
    __ASSERT_ALWAYS( iData->iTransform, USER_INVARIANT() );
    return *iData->iTransform;        
    }

