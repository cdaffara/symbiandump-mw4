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
* Description:   Environment class for transition effect plugins.
*
*/



#ifndef C_ALFSRVEFFECTENV_H
#define C_ALFSRVEFFECTENV_H

#include <e32base.h>

class CHuiEnv;
class CHuiControlGroup;
class CHuiDisplay;
class CAlfAppSrvSession;

/**
 * This class implements an environment that can be utilized inside transition
 * effect plugins to get access to the UI accelerator toolkit visuals, layouts
 * and other resources that are needed in transition effects.
 *
 */
NONSHARABLE_CLASS(CAlfSrvEffectEnv) : public CBase
    {
public:

    /**
     * Constructor
     */
    static CAlfSrvEffectEnv* NewL(CAlfAppSrvSession& aSession);

    /**
     * Constructor
     */
    static CAlfSrvEffectEnv* NewLC(CAlfAppSrvSession& aSession);

    /**
     * Destructor
     */
    ~CAlfSrvEffectEnv();

    /**
     * Returns number of available control groups for 
     * currently active application.
     *
     * @return Amount of available control groups.
     */
    IMPORT_C TInt ControlGroupCountL();

    /**
     * Returns number of visible and available control groups
     * for currently active application.      
     *
     * @return Amount of visible and available control groups.
     */
    IMPORT_C TInt VisibleControlGroupCountL();

    /**
     * Returns an available control group based on its index.
     *
     * @param aIndex Control group index.
     * @return Control group
     */
    IMPORT_C CHuiControlGroup* ControlGroupL( TInt aIndex );
    
    /**
     * Returns a visible control group based on its index.
     *
     * @param aIndex Control group index.
     * @return Control group
     */
    IMPORT_C CHuiControlGroup* VisibleControlGroupL( TInt aIndex );

    /**
     * Returns number of available displays for 
     * currently active applicaion.
     *
     * @return Amount of available displays.
     */
    IMPORT_C TInt DisplayCountL();

    /**
     * Returns an available display based on its index.
     *
     * @param aIndex Display index.
     * @return Display
     */
    IMPORT_C CHuiDisplay* DisplayL( TInt aIndex );

    /**
     * Returns env which is shared between all client apps.
     *
     * @return HUI Env
     */
    IMPORT_C CHuiEnv* SharedEnv();

private:

    // Constructor
    CAlfSrvEffectEnv(CAlfAppSrvSession& aSession);
    
    // ConstructL
    void ConstructL();

private:
   
   CAlfAppSrvSession* iSession; // Not owned

};


#endif

