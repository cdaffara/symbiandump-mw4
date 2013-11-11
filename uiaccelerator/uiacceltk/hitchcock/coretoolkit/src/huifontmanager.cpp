/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of THuiFontManager. Handles the management
*                of Hitchcock UI Toolkit specific fonts. The implementation
*                consists only of Symbian / S60 specific implementation but
*                the interface is designed so that alternative platforms can be
*                utilized.
*
*/



#include <uiacceltk/huifontmanager.h>
#include <uiacceltk/HuiEnv.h>
#include <coemain.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creates the THuiFontManager. Sets parameters to default values.
// ---------------------------------------------------------------------------
//
EXPORT_C THuiFontManager::THuiFontManager()
    {
    }

// ---------------------------------------------------------------------------
// Destroys the THuiFontManager. Makes sure all allocated resources are freed.
// ---------------------------------------------------------------------------
//
EXPORT_C THuiFontManager::~THuiFontManager()
    {
    // Close the array of fonts
    iFonts.Close();
    }

// ---------------------------------------------------------------------------
// Returns font specifications of all platform-supported fonts.
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiFontManager::EnumerateFontsL(RArray<THuiFontSpecification>& aArray) const
    {   
    // Add all font specification objects into the output array.
    CWsScreenDevice* screenDevice = CHuiStatic::ScreenDevice();
    TInt numSystemFonts = screenDevice->NumTypefaces();    
    for(TInt i = 0; i < numSystemFonts; ++i)
        {
    	THuiFontSpecification fontSpecification;
    	TTypefaceSupport typefaceSupport;
    	
    	// Retrieve the typeface support information from the system
    	screenDevice->TypefaceSupport(typefaceSupport, i);
    	
    	// Retrieve the typeface family name from typeface support
    	fontSpecification.SetTypefaceFamily(typefaceSupport.iTypeface.iName.Des());
    	
    	// Append font specification to the array.
        aArray.AppendL(fontSpecification);
        }
    }

// ---------------------------------------------------------------------------
// Creates a new font instance.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiFontManager::CreateFontL(const THuiFontSpecification& aSpecification)
    {
    // Include the specified font into the array of fonts.
    iFonts.AppendL(aSpecification);
    
    // Return the unique ID of the font.
    return iFonts.Count() - 1;
    }
