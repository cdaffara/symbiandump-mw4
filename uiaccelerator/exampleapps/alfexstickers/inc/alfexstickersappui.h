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
* Description: 
*
*/




#ifndef __ALFEXSTICKERS_APPUI_H__
#define __ALFEXSTICKERS_APPUI_H__

#include <aknappui.h>

/* Forward declarations. */
class CAlfEnv;

/** 
 * @class CAlfExStickersAppUi
 * 
 * @discussion An instance of class CAlfExStickersAppUi is the UserInterface part of the AVKON
 * application framework for the AlfExStickers example application
 */
class CAlfExStickersAppUi : public CAknAppUi
    {
public:
	/**
	 * @function ConstructL
	 * 
	 * @discussion Perform the second phase construction of a CAlfExStickersAppUi object
	 * this needs to be public due to the way the framework constructs the AppUi 
	 */
    void ConstructL();


    /**
     * @function CAlfExStickersAppUi
     * 
     * @discussion Perform the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the AppUi 
     */
    CAlfExStickersAppUi();

    /**
     * @function ~CAlfExStickersAppUi
     * 
     * @discussion Destroy the object and release all memory objects
     */
    ~CAlfExStickersAppUi();


public: // from CAknAppUi
	/**
	 * @function HandleCommandL
	 * 
	 * @discussion Handle user menu selections.
	 * @param aCommand the enumerated code for the option selected
	 */
    void HandleCommandL(TInt aCommand);
    
    /*
     * @function HandleResourceChangeL
     * 
     * @discussion Handles layout changes.
     */
    virtual void HandleResourceChangeL( TInt aType );
    
    /*
     * @function HandleWsEventL
     * 
     * @discussion Handle releasing/restoring of bitmaps, textures etc.
     */
    void HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination);

private:
    CAlfEnv* iEnv;
    // file server session handle
    };


#endif // __ALFEXSTICKERS_APPUI_H__

