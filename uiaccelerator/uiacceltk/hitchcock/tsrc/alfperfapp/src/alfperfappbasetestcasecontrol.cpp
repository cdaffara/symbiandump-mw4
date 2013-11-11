/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAlfPerfAppBaseTestCaseControl implementation.
*
*/


#include "alfperfappbasetestcasecontrol.h"
#include "alfperfapp.hrh"

#include <aknutils.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alfcommand.h>
#include <alf/alfevent.h>
#include <alf/alftexturemanager.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfutil.h>
#include <alf/alfgridlayout.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfanchorlayout.h>

#include <alfperfapp_imagetest.mbg>

/**
 * Complete now command.
 */
const TInt KAlfPerfAppCmdCompleteNow = 0x5000;

        
// ============================ MEMBER FUNCTIONS ===============================


// Implementation of CAlfPerfAppBaseTestCaseControl:

static void GetImageTestCasePrivatePath( TFileName& aPath )
    {
    CEikonEnv::Static()->FsSession().PrivatePath( aPath );
    
    ::CompleteWithAppPath( aPath );
    }

CAlfPerfAppBaseTestCaseControl::CAlfPerfAppBaseTestCaseControl()  
    {
    }
        
CAlfPerfAppBaseTestCaseControl::~CAlfPerfAppBaseTestCaseControl()
    {
    CompleteNow( KErrCancel );
    }
        
void CAlfPerfAppBaseTestCaseControl::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& /*aVisibleArea*/ )
    {
    iCaseId = aCaseId;
    CAlfControl::ConstructL( aEnv );
    
    TFileName privatePath;
    GetImageTestCasePrivatePath( privatePath );
    aEnv.TextureManager().SetImagePathL( privatePath );
    }
        
void CAlfPerfAppBaseTestCaseControl::StartExecuteL( TRequestStatus& aStatus )
    {
    iStatus = &aStatus;
    *iStatus = KRequestPending;
    
    TRAPD( err, DoStartExecuteL() );
    if ( err != KErrNone )
        {
        iStatus = NULL;
        User::Leave( err );
        }
    }

void CAlfPerfAppBaseTestCaseControl::CancelExecution()
    {
    CompleteNow( KErrCancel );
    }

void CAlfPerfAppBaseTestCaseControl::DoStartExecuteL()
    {
    CompleteNow( KErrNone );
    }

void CAlfPerfAppBaseTestCaseControl::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    }

TBool CAlfPerfAppBaseTestCaseControl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppCmdCompleteNow )
        {
        CompleteNow( KErrNone );
        return ETrue;
        }
        
    return CAlfControl::OfferEventL( aEvent );
    }
        
void CAlfPerfAppBaseTestCaseControl::CompleteAfterL( TInt aDuration )
    {
    // In order to use this service, base class StartExecuteL must
    // have been called.
    __ASSERT_ALWAYS( iStatus, User::Invariant() );
    
    TAlfCustomEventCommand command( KAlfPerfAppCmdCompleteNow, this );
    User::LeaveIfError( Env().Send( command, aDuration ) );
    }
        
void CAlfPerfAppBaseTestCaseControl::CompleteNow( TInt aErrorCode )
    {
    if ( iStatus )
        {
        User::RequestComplete( iStatus, aErrorCode );
        iStatus = NULL;
        }
    }
/*
inline TInt CAlfPerfAppBaseTestCaseControl::CaseId() const
    {
    return iCaseId;
    }

inline TBool CAlfPerfAppBaseTestCaseControl::IsExecutionOngoing() const
    {
    return ( iStatus != NULL );
    }
*/
