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


#ifndef GOOMACTION_H_
#define GOOMACTION_H_

#include <e32base.h>

class MGOomActionObserver;

/*
 * The base class for all GOOM actions (i.e. close application or run GOOM plug-in).
 * 
 * A base class is used because both types of action have common aspects, specifically:
 *  - They are prioritised according to application idle time
 *  - They need to be prioritised against each other
 *  - For each action it is possible to either continue immediately or to wait for completion
 *  
 * @lib goommonitor.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomAction) : public CBase
    {
public:

    virtual ~CGOomAction();
    
    /**
     * Activate the OOM action
     *
     * @since S60 5.0
     * @param aBytesRequested ?description
     */ 
    virtual void FreeMemory(TInt aBytesRequested, TBool aUseSwRendering) = 0;
    
    /**
     * @since S60 5.0
     * @return ETrue if the action is currently freeing memory, else EFalse
     */ 
    virtual TBool IsRunning() = 0;
    
protected:
    // 
    /**
     * To be called by the derived class after the memory has been freed (or if it fails)
     * 
     * @since S60 5.0
     * @param aError KErrNone if memory has successfully been freed, otherwise any system wide error code
     */ 
    void MemoryFreed(TInt aError);
    
    CGOomAction(MGOomActionObserver& aStateChangeObserver);
    
private: //data
    
    enum TGOomActionState
        {
        EGOomIdle,
        EGOomFreeingMemory
        };  
    TGOomActionState iState;

protected:    
    MGOomActionObserver& iStateChangeObserver;
    };

#endif /*GOOMACTION_H_*/
