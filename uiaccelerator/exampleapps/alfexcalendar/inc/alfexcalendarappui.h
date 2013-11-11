/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#ifndef __ALFEXCALENDAR_APPUI_H__
#define __ALFEXCALENDAR_APPUI_H__

#include <aknappui.h>


// Forward reference
class CAlfEnv;
class CAlfExCalendarEngine;

/*! 
  @class CAlfExCalendarAppUi
  
  @discussion An instance of class CAlfExCalendarAppUi is the UserInterface
  part of the AVKON application framework for the alfexcalendar application
  */
class CAlfExCalendarAppUi : public CAknAppUi
    {
public:
/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CAlfExCalendarAppUi object
  this needs to be public due to the way the framework constructs the AppUi 
  */
    void ConstructL();

/*!
  @function CAlfExCalendarAppUi
  
  @discussion Perform the first phase of two phase construction.
  This needs to be public due to the way the framework constructs the AppUi 
  */
    CAlfExCalendarAppUi();


/*!
  @function ~CAlfExCalendarAppUi
  
  @discussion Destroy the object and release all memory objects
  */
    ~CAlfExCalendarAppUi();


public: // from CAknAppUi
    /*!
      @function HandleCommandL
      
      @discussion Handle user menu selections
      @param aCommand the enumerated code for the option selected
      */
    void HandleCommandL(TInt aCommand);

    /*!
      @function DynInitMenuPaneL
      
      @discussion dynamically initializes the options menu
      @param aResourceId resource id of menu
      @param aMenuPane MenuPane which contains list of options
      */
    void DynInitMenuPaneL( 
            TInt aResourceId,
            CEikMenuPane* aMenuPane );

    /*!
      @function HandleResourceChangeL
      
      @discussion handles resolution and orientation changes properly
      @param aType typs of resource change
      */
    virtual void HandleResourceChangeL( TInt aType );

    /*
     * Handles window server events to the application. Performs resource 
     * loading/unloading when application goes changes to foreground/background.
     * All events are forwarded to base class implementation.
     * 
     * @param aEvent Event ID. Handles KAknFullOrPartialForegroundLost 
     * and KAknFullOrPartialForegroundGained events. 
     * 
     * @param aDestination Not used in this function
     */
    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    
private:

    // Hitchcock toolkit environment
    CAlfEnv* iEnv;

    // Dummy calendar engine
    CAlfExCalendarEngine* iCalendarEngine;
    
    };


#endif // __ALFEXCALENDAR_APPUI_H__
