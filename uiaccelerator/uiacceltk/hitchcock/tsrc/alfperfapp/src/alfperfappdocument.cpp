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
* Description:  CAlfPerfAppDocument implementation.
*
*/


#include "alfperfappdocument.h"
#include "alfperfappappui.h"
#include "alfperfappmodel.h"

// ============================ MEMBER FUNCTIONS ===============================

CAlfPerfAppDocument::CAlfPerfAppDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

CAlfPerfAppDocument::~CAlfPerfAppDocument()
    {
    delete iModel;
    }

void CAlfPerfAppDocument::ConstructL()
    {
    iModel = CAlfPerfAppModel::NewL();
    }

CEikAppUi* CAlfPerfAppDocument::CreateAppUiL()
    {
    return new (ELeave) CAlfPerfAppAppUi( *iModel );
    }
