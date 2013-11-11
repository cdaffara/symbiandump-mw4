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
* Description:   Control group sub session
*
*/



#include "alfsrvcontrolgroupsubsession.h"
#include "alf/alfconstants.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfappsrvsession.h"
#include "alfscreen.h" // EAlfSessionContainer
#include "alfsrvtransformationsubsession.h"
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiControlGroup.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiDeckLayout.h>

// host control

NONSHARABLE_CLASS( CAlfSrvHostControl ) : public CHuiControl
    {
public:
    CAlfSrvHostControl(CHuiEnv& aEnv) : CHuiControl(aEnv) {}
    CHuiLayout* ContainerLayout(const CHuiControl* aConnected) const;
    };
    
   
CHuiLayout* CAlfSrvHostControl::ContainerLayout(const CHuiControl* /*aConnected*/) const
    {
    if ( VisualCount() )
        {
        return static_cast<CHuiLayout*>(&Visual(0)); 
        }
    return NULL;
    }
    

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvControlGroupSubSession::CAlfSrvControlGroupSubSession( 
    CAlfAppSrvSessionBase& aSession,
    TInt aId )
    : CAlfSrvSubSessionBase( aSession ), iId( aId ) 
    {
    }


// ---------------------------------------------------------------------------
// 2nd phasse constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvControlGroupSubSession::ConstructL()
    {
    CAlfSrvSubSessionBase::ConstructL();
    
    iControlGroup = &Session().SharedHuiEnv()->NewControlGroupL( 
        reinterpret_cast<TInt>(this) );
        
    iHostControl = new (ELeave) CAlfSrvHostControl( *Session().SharedHuiEnv() );
    iHostControl->ConstructL();
    iHostControl->SetRole(EHuiSessionContainer);
        
    CHuiLayout* sessionContainer = CHuiDeckLayout::AddNewL( *iHostControl, NULL );
    sessionContainer->SetFlags( EHuiVisualFlagManualLayout );
    sessionContainer->SetTagL( _L8("Session container") );
    sessionContainer->SetSize( Session().ClientDrawingArea().Size() );
    sessionContainer->SetPos( Session().ClientDrawingArea().iTl );
        
    iControlGroup->AppendL( iHostControl );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfSrvControlGroupSubSession* CAlfSrvControlGroupSubSession::NewL( 
    CAlfAppSrvSessionBase& aSession,
    TInt aId )
    {
    CAlfSrvControlGroupSubSession* self = 
        CAlfSrvControlGroupSubSession::NewLC( aSession, aId );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfSrvControlGroupSubSession* CAlfSrvControlGroupSubSession::NewLC( 
    CAlfAppSrvSessionBase& aSession,
    TInt aId )
    {
    CAlfSrvControlGroupSubSession* self = 
        new( ELeave ) CAlfSrvControlGroupSubSession( aSession, aId );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvControlGroupSubSession::~CAlfSrvControlGroupSubSession()
    {
    // Hide the control group from the display
    CHuiDisplay* display = Session().SharedHuiEnv()->DisplayCount() ? 
                                &Session().SharedHuiEnv()->PrimaryDisplay() : 
                                NULL ;
    if ( display )
        {
        display->Roster().Hide( ControlGroup() );
        }

    Session().SharedHuiEnv()->DeleteControlGroup( reinterpret_cast<TInt>(this) );
    
    iControlGroup = NULL;
    iHostControl = NULL;
    }

// ---------------------------------------------------------------------------
// Gets control group
// ---------------------------------------------------------------------------
//
CHuiControlGroup& CAlfSrvControlGroupSubSession::ControlGroup()
    {
    return *iControlGroup;
    }

// ---------------------------------------------------------------------------
// Is control group shown
// ---------------------------------------------------------------------------
//
TBool CAlfSrvControlGroupSubSession::IsShown() const
    {
    return iIsShown;
    }

// ---------------------------------------------------------------------------
// Set if the control group is shown
// ---------------------------------------------------------------------------
//    
void CAlfSrvControlGroupSubSession::SetIsShown( TBool aShown )
    {
    iIsShown = aShown;
    }
    
// ---------------------------------------------------------------------------
// Append control
// ---------------------------------------------------------------------------
//
void CAlfSrvControlGroupSubSession::AppendL( 
    const RMessage2& aMessage )
    {    
    // Parse parameters

    CHuiControl* control = static_cast<CHuiControl*>
        ( Session().GetInterfaceL(EHuiObjectTypeControl, aMessage.Int1()) );
    
    // Set host        
    control->SetHostId( iHostControl->Id() );
        
    // Append control
    ControlGroup().AppendL(control);

    // Complete
    aMessage.Complete( KErrNone );
    }
 
 // ---------------------------------------------------------------------------
// Remove control
// ---------------------------------------------------------------------------
//
void CAlfSrvControlGroupSubSession::RemoveL( 
    const RMessage2& aMessage )
    {    
    // Parse parameters

    CHuiControl* control = static_cast<CHuiControl*>
        ( Session().GetInterfaceL(EHuiObjectTypeControl, aMessage.Int1()));
            
    // Append control
    TInt err = ControlGroup().Remove(control);

    // Complete
    aMessage.Complete( err );
    }
 
// ---------------------------------------------------------------------------
// Enables transformation
// ---------------------------------------------------------------------------
//
void CAlfSrvControlGroupSubSession::EnableTransformationL( 
    const RMessage2& aMessage )
    {
    // Parser parameters
    
    // 1: is transformed (in)
    const TBool isTransformed = aMessage.Int1();
    
    // 2: transformation handle (in)
    const TInt transHandle = aMessage.Int2();
    CAlfSrvSubSessionBase& subSession = Session().SubSession( transHandle );
    CAlfSrvTransformationSubSession& transformationSubSession = 
        static_cast<CAlfSrvTransformationSubSession&>(subSession);
    
    // Enable transformation
    ControlGroup().EnableTransformationL( isTransformed );
    CHuiTransformation* transformation = NULL;
    if ( isTransformed )
        {
        transformation = &ControlGroup().Transformation();
        }
    transformationSubSession.SetTransformation( transformation );
    
    // Complete
    aMessage.Complete( KErrNone );
    }
   
// ---------------------------------------------------------------------------
// From CAlfSrvSubSessionBase
// Process the message.
// ---------------------------------------------------------------------------
// 
void CAlfSrvControlGroupSubSession::DoProcessMessageL(const RMessage2& aMessage)
    {
    switch( aMessage.Int0() )
        {
        case EAlfControlGroupAppend:
            {
            AppendL( aMessage );
            break;
            }
        
        case EAlfControlGroupRemove:
            {
            RemoveL( aMessage );
            break;
            }
            
        case EAlfControlGroupEnableTransformation:
            {
            EnableTransformationL( aMessage );
            break;
            }
        case EAlfControlGroupSubSessionClose: // obsolete
            {
            Session().CloseSubSession( aMessage );
            break;
            }
        default:
            CAlfSrvSubSessionBase::DoProcessMessageL( aMessage );
            break;
        }
    }


CHuiControlGroup* CAlfSrvControlGroupSubSession::AsHuiControlCroup()
    {
    return iControlGroup;
    }

TInt& CAlfSrvControlGroupSubSession::PreferredPos()
    {
    return iPrefrerredPos;
    }
    
 

