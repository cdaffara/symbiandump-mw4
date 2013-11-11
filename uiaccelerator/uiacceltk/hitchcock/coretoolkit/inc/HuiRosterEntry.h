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



#ifndef __HUIROSTERENTRY_H__
#define __HUIROSTERENTRY_H__


#include <e32base.h>


/* Forward declarations. */
class CHuiControlGroup;
class CHuiVisual;


/**
 * Control group reference in the roster. Each entry has a list of root 
 * visuals.
 */
NONSHARABLE_CLASS(CHuiRosterEntry) : public CBase
	{
public:
    
    /* Constructors and destructors. */
    
    /**
     * Constructor.
     */
    CHuiRosterEntry(CHuiControlGroup& aGroup);
    
    /**
     * Destructor. 
     */
    ~CHuiRosterEntry();
    
    /* Methods. */
    
    /**
     * Determines the control group of the entry.
     */
    CHuiControlGroup& ControlGroup() const;
    

private:

    /** Control group of the entry. */
    CHuiControlGroup& iGroup;
    
    
public:

    /* Public data. */    
    
    /** Array of root visuals. */
    RPointerArray<CHuiVisual> iRootVisuals;

	};
	
#endif // __HUIROSTERENTRY_H__
