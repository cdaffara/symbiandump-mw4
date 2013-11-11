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
* Description:  alfperfapp document class definition.
*
*/


#ifndef C_ALFPERFAPPDOCUMENT_H
#define C_ALFPERFAPPDOCUMENT_H

#include <akndoc.h>

class CEikAppUi;
class CAlfPerfAppModel;

/**
 *  CAlfPerfAppDocument document class.
 */
class CAlfPerfAppDocument : public CAknDocument
    {
public:

    CAlfPerfAppDocument( CEikApplication& aApp );
    ~CAlfPerfAppDocument();

    // New functions
    void ConstructL();

    // From base class CAknDocument
    CEikAppUi* CreateAppUiL();

private:
    /**
     * Model.
     * Owned.
     */
    CAlfPerfAppModel* iModel;
    
    };

#endif // C_ALFPERFAPPDOCUMENT_H
            
// End of File
