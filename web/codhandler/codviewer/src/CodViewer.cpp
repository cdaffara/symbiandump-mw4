/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CodViewer application entry point.   
*
*/


// INCLUDE FILES

#include <eikstart.h>
#include <CodViewerApp.h>

// ================= OTHER FUNCTIONS ==============

/**
* Create application object.
* @return The created application object.
*/
LOCAL_C CApaApplication* NewApplication()
    {
    return new CCodViewerApp;
    }

/**
* Executable entry point.
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
