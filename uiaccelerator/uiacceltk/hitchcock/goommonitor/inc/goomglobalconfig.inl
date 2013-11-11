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
* Description:  Configuration representation classes for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMGLOBALCONFIGINL_H_
#define GOOMGLOBALCONFIGINL_H_

void CGOomGlobalConfig::AddForceCheckPriorityL(TUint aPriority)
    {
    iForceCheckPriorities.InsertL(aPriority);
    }

// Return ETrue if a force check has been added for this priority, return EFalse otherwise
TBool CGOomGlobalConfig::ForceCheckAtPriority(TUint aPriority) const
    {
    return (iForceCheckPriorities.Find(aPriority) != NULL);
    }

#endif /*GOOMGLOBALCONFIGINL_H_*/
