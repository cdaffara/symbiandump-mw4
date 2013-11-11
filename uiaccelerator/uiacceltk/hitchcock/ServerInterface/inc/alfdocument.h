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
* Description:   Document definition
*
*/



#ifndef C_ALFDOCUMENT_H
#define C_ALFDOCUMENT_H

#include <AknDoc.h>

class CAlfApplication;

/**
 *  Alfred Server document class.
 *
 *  @since S60 v3.2
 */
class CAlfDocument : public CAknDocument
    {
    
public:
    
    /**
     * Constructor
     */
    CAlfDocument( CAlfApplication& aApplication );

// from base class CAknDocument

    /**
     * From CAknDocument.
     * Called by the framework when an appui is created.
     *
     * @return New appui instannce. Ownership transferred.
     */
    CEikAppUi* CreateAppUiL();

    // from CEikDocument
    void UpdateTaskNameL(CApaWindowGroupName* aWgName);

    };

#endif // C_ALFDOCUMENT_H
