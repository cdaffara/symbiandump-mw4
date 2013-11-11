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
* Description:  Status-pane related methods
*
*/



#include <akntitle.h>
#include <aknview.h>
#include <AknViewAppUi.h>
#include <EikSpane.h>
#include <StringLoader.h>

#include "StatusPane.h"


// -----------------------------------------------------------------------------
// StatusPane::SetTitleL
//
// Sets the title.
// -----------------------------------------------------------------------------
//
void StatusPane::SetTitleL(TInt aStringId)
    {
    HBufC*  label = NULL;

    // Load the label's resource.
	label = StringLoader::LoadLC(aStringId);

    SetTitleL(*label);
    CleanupStack::PopAndDestroy(label);
    }


// -----------------------------------------------------------------------------
// StatusPane::SetTitleL
//
// Sets the title.
// -----------------------------------------------------------------------------
//
void StatusPane::SetTitleL(const TDesC& aString)
    {
    CAknViewAppUi*  appUi;

    // Get the title sub-pane.
    appUi = static_cast<CAknViewAppUi*>(CCoeEnv::Static()->AppUi());

    CEikStatusPaneBase::TPaneCapabilities subPane = appUi->StatusPane()->
            PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle));

    // Set the title if the pane belongs to the app.
	if (subPane.IsPresent() && subPane.IsAppOwned())
		{
        CAknTitlePane*  titlePane;
        TUid            uid;

        uid.iUid = EEikStatusPaneUidTitle;

		titlePane = (CAknTitlePane*) appUi->StatusPane()->ControlL(uid);
        titlePane->SetTextL(aString);
        }
    }
