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


#ifndef GOOMPLUGINWAITER_H_
#define GOOMPLUGINWAITER_H_

#include <e32base.h>

class CGOomRunPlugin;


/*
 *  A class used for waiting for a pre-determined period before completing the plugin operation
 *  It is intended to be used to force a delay between the call to the plugin and the memory check,
 *  thus allowing the plugin to free some memory first.
 * 
 *  @lib goommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomPluginWaiter) : public CTimer
    {
public:
    static CGOomPluginWaiter* NewL(TInt aMillisecondsToWait, CGOomRunPlugin& aCallbackClass);
    
    // Start the timer, it will call the plugin back when it expires
    void Start();
    
protected:
    void RunL();
    
    CGOomPluginWaiter(TInt aMillisecondsToWait, CGOomRunPlugin& aCallbackClass);
    
private:
    TInt iMillisecondsToWait;
    CGOomRunPlugin& iCallbackClass;
    };

#endif /*GOOMPLUGINWAITER_H_*/
