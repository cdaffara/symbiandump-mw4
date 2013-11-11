/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Inlined command methods
*
*/



#ifndef __DMADCOMMAND_INL__
#define __DMADCOMMAND_INL__

inline TInt CDmAdCommand::StatusRef(void) const
    {
    return iStatusRef;
    }

inline TInt CDmAdCommand::Status(void) const
    {
    return iStatus;
    }

inline TBool CDmAdCommand::IsLeaf(void) const
    {
    return iLeaf;
    }

#endif
