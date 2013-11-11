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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMWINDOWGROUPLISTINL_H_
#define GOOMWINDOWGROUPLISTINL_H_

// Return the number of application instances in this list
inline TUint CGOomWindowGroupList::Count() const 
    {
    return iWgIds.Count();    
    }

inline const RWsSession::TWindowGroupChainInfo& CGOomWindowGroupList::WgId(TInt aIndex) const
    {
    return iWgIds[aIndex];
    }
    
#endif /*GOOMWINDOWGROUPLISTINL_H_*/
