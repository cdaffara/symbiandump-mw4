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
* Description:  alfperfapp application UI class definition.
*
*/


#ifndef C_ALFPERFAPPAPPUI_H
#define C_ALFPERFAPPAPPUI_H

#include <aknviewappui.h>

class CAlfPerfAppModel;

/**
 *  CAlfPerfAppAppUi appui class.
 */
class CAlfPerfAppAppUi : public CAknViewAppUi
    {
public:

    CAlfPerfAppAppUi( CAlfPerfAppModel& aModel );
    ~CAlfPerfAppAppUi();

    // From base class CAknViewAppUi

    virtual void ConstructL();

private:

    // From base class CAknViewAppUi

    virtual void HandleCommandL( TInt aCommand );
    virtual void HandleResourceChangeL( TInt aType );

private:

    /**
     * Model of this application.
     * Owned.
     */
    CAlfPerfAppModel& iModel;
     
    };

#endif // C_ALFPERFAPPAPPUI_H

