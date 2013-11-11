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



#include "HuiRosterEntry.h"
#include "uiacceltk/HuiDisplay.h"
#include "HuiRosterImpl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiPanic.h"


CHuiRosterEntry::CHuiRosterEntry(CHuiControlGroup& aGroup)
        : iGroup(aGroup)
    {    
    }
    
    
CHuiRosterEntry::~CHuiRosterEntry()
    {
    // Make sure the root visuals are hidden.
    for(TInt i = iRootVisuals.Count() - 1; i >= 0; --i)
        {
        CHuiDisplay* display = iRootVisuals[i]->Display();
        if(display)
            {
            CHuiRosterImpl& roster = STATIC_CAST(CHuiRosterImpl&, display->Roster());
            roster.HideVisual(iRootVisuals[i]);
            }
        }
    iRootVisuals.Reset();
    }
   

CHuiControlGroup& CHuiRosterEntry::ControlGroup() const
    {
    return iGroup;    
    }
