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
* Description:  S60 Document class
*
*/


#include "imageprintuidocument.h"
#include "imageprintuiappui.h"
#include "imageprintuidebug.h"

#include <e32cmn.h>
#include <badesca.h>
#include <bautils.h>



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CImagePrintUiDocument::CImagePrintUiDocument(CEikApplication& aApp)
    : CAknDocument(aApp)
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CImagePrintUiDocument::ConstructL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiDocument::ConstructL START"));
   
    TRAPD(err, iEngine = CDpsEngine::GetEngineL());
    if(err == KErrInUse)
    	{
        FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiDocument::ConstructL, already in use"));	
    	}
	
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiDocument::ConstructL END"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CImagePrintUiDocument* CImagePrintUiDocument::NewL( CEikApplication& aApp)
    {
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiDocument::NewL()"));
    CImagePrintUiDocument* self = new (ELeave) CImagePrintUiDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiDocument::NewL() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
// Constructs CImagePrintUiAppUi
// ---------------------------------------------------------------------------
//
CEikAppUi* CImagePrintUiDocument::CreateAppUiL()
    {
    return new (ELeave) CImagePrintUiAppUi;
    }


// ---------------------------------------------------------------------------
// Returns an instance of Image Print UI engine
// ---------------------------------------------------------------------------
//
CDpsEngine* CImagePrintUiDocument::Engine()
    {
    return iEngine; 
    }


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CImagePrintUiDocument::~CImagePrintUiDocument()
    {
    if(iEngine)
    	{
        iEngine->Delete();	
    	}	
    }

    

//End of File
