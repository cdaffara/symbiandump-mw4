/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiControlGroup.h"  // Class definition
#include "uiacceltk/HuiLayouts.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEnv.h"

CHuiControlGroup::CHuiControlGroup(TInt aResourceId, CHuiEnv& aEnv)
        : iResourceId(aResourceId), iEnv(aEnv), iAcceptInput(ETrue), iOpacity( 1.f ),iAlfApp(EFalse)
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    }


void CHuiControlGroup::ConstructL()
    {    
    }
    
    
void CHuiControlGroup::ConstructFromResourceL(TResourceReader& /*aReader*/)
    {
    /// @todo Method not yet implemented. Implement if UI creation from resource file is required.
    }

	
CHuiControlGroup::~CHuiControlGroup()
    {   
    
       
        iEnv.RemoveTheControlGroup(iResourceId);
    
        iUnhandledEventObservers.Close();
            
            // The group owns the controls, so it must destroy them.
            // Destroy in reverse order so that references will be removed in 
            // natural order (children of a pane will be removed before the pane).
            for(TInt i = iControls.Count() - 1; i >= 0; --i)
                {
                CHuiControl* c = iControls[i];
                iControls.Remove(i);
                delete c;
                }
            iControls.Reset();
            
            delete iTransform;
            iTransform = NULL;
            HUI_PROBE_REPORT_DESTRUCTED
      
    
    }
    
    
EXPORT_C TInt CHuiControlGroup::ResourceId() const
    {
    return iResourceId;
    }


EXPORT_C void CHuiControlGroup::AppendL(CHuiControl* aControl)
    {
    User::LeaveIfError( iControls.Append(aControl) );
    aControl->SetControlGroup(*this);
    
    // if the control group is shown in a roster, the new control 
    // has to be shown as well
    
    TRAPD( err, 
        {
        // if there are other controls in the group, check from those
        if ( iControls.Count() > 1 ) // so more than this added one
            {
            if ( iControls[0]->Display() )
                {
                aControl->ShowL( *iControls[0]->Display() );
                }
            }
        else
            {
            // if not, go though the displays
            for ( TInt d = 0; d < iEnv.DisplayCount(); d++ )
                {
                CHuiDisplay* display = (iEnv.Displays())[d];
                // if this control group is in any roster, show the control in that display.
                if ( display->Roster().Find( this ) != KErrNotFound )
                    {
                    aControl->ShowL( *display );
                    }
                }
            }
            
        }); // TRAPD
    if ( err != KErrNone )
        {
        iControls.Remove( iControls.Count()-1);
        CHuiControlGroup* nullGroup = NULL;
        aControl->SetControlGroup(*nullGroup);
        User::Leave( err );
        }
    }


EXPORT_C void CHuiControlGroup::AppendAndPopL(CHuiControl* aControl)
    {
    AppendL(aControl);
    CleanupStack::Pop(aControl);
    }


EXPORT_C TInt CHuiControlGroup::Remove(CHuiControl* aControl)
    {
    TInt index = iControls.Find(aControl);
    if(index != KErrNotFound)
        {
        // If the control group is shown is a roster, the 
        // control has to be hidden as well.
        CHuiDisplay* display = aControl->Display();
        if ( display ) // check that the control is bound to a display
            {
            // Check that this group is shown on the display.
            if ( display->Roster().Find( this ) != KErrNotFound )
                {
                // hide control 
                aControl->Hide( *display );
                }
            }
        
        iControls.Remove(index);
        CHuiControlGroup* nullGroup = NULL;
        aControl->SetControlGroup(*nullGroup);
        return KErrNone;
        }
    // The control was not in the group.    
    return KErrNotFound;
    }


EXPORT_C TInt CHuiControlGroup::Count() const
    {
    return iControls.Count();
    }


EXPORT_C CHuiControl& CHuiControlGroup::Control(TInt aIndex) const
    {
    return *iControls[aIndex];
    }


EXPORT_C CHuiControl* CHuiControlGroup::FindControl(TInt aId) const
    {
    for(TInt i = 0; i < iControls.Count(); ++i)
        {
        if(iControls[i]->Id() == aId)
            {
            return iControls[i];
            }
        }
    return NULL;
    }


EXPORT_C void CHuiControlGroup::EnableTransformationL(TBool aIsTransformed)
    {
    if(iTransform && !aIsTransformed)
        {
        // Disable transformation.
        delete iTransform;
        iTransform = NULL;
        }
    else if(!iTransform && aIsTransformed)
        {
        // Enable transformation.
        iTransform = CHuiTransformation::NewL();
        }
    else
        {
        // For PC-lint
        }
    }

    
EXPORT_C CHuiTransformation& CHuiControlGroup::Transformation()
    {
    if(!iTransform)
        {
        THuiPanic::Panic(THuiPanic::EControlGroupTransformNotEnabled);
        }
    return *iTransform;        
    }


EXPORT_C TBool CHuiControlGroup::IsTransformed() const
    {
    return iTransform != 0;
    }


EXPORT_C TBool CHuiControlGroup::AcceptInput() const
    {
    return iAcceptInput;
    }


EXPORT_C void CHuiControlGroup::SetAcceptInput(TBool aAcceptInput) 
    {
    iAcceptInput = aAcceptInput;
    }

EXPORT_C void CHuiControlGroup::SetSecureId(TInt aSecureId)
    {
    iSecureId = aSecureId;
    }
EXPORT_C TInt CHuiControlGroup::SecureId()
    {
    return iSecureId;
    }


TBool CHuiControlGroup::Changed() const
    {
    if(iTransform && iTransform->Changed())
        {
        return ETrue;
        }

    return iOpacity.Changed();
    }


void CHuiControlGroup::ClearChanged()
    {
    iOpacity.ClearChanged();

    if(iTransform)
        {
        iTransform->ClearChanged();
        }
    
    for(TInt i = 0; i < iControls.Count(); ++i)
        {
        iControls[i]->ClearChanged();
        }
    }


HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiControlGroup, ETypeControlGroup)



