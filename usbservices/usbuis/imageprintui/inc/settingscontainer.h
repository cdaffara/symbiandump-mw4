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
* Description:  Header file for settingscontainer
*
*/

#ifndef CSETTINGSCONTAINER_H
#define CSETTINGSCONTAINER_H


// INCLUDES
#include <coecntrl.h>
#include <aknview.h>    // AVKON components
#include <aknlists.h>

#include "settingsview.h"


// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;
class CSettingsItems;
class CImagePrintUiAppUi;
class CCapabilityManager;


/**
*   Container class for setting items   
*/
class CSettingsContainer : public CCoeControl
    {


public:

   /**
    *   Two phase constructor
    *   @param aRect  reference to container area
    *   @param aAppUi the pointer to  the CImagePrintUiAppUi
    *   @return a CSettingsContainer instance
    */
    static CSettingsContainer* NewL( const TRect& aRect, CImagePrintUiAppUi* aAppUi );

   /**
    *   Two phase constructor
    *   
    *   @param aRect  reference to container area
    *   @param aAppUi the pointer to  the CImagePrintUiAppUi
    *   @return a CSettingsContainer instance
    */
    static CSettingsContainer* NewLC( const TRect& aRect, CImagePrintUiAppUi* aAppUi );

   /**
    *   Destructor
    */	
    virtual ~CSettingsContainer();

    /**
     * Forward change event to container
     * @param None
     * @return None.
     */
    void HandleChange(); 
    

private: // Methods derived from CCoeControl

   /**
    * From CCoeControl Handles a chage of client area size.
    */
    void SizeChanged(); 	

   /**
    * From CCoeControl, Handles a change to the control's resources.
    */
    void HandleResourceChange( TInt aType ); 
    	
   /**
    * From CCoeControl Returns the number of components.
    */
    TInt CountComponentControls() const;
	
   /**
    * From CCoeControl Returns a pointer of component.
    */
	CCoeControl* ComponentControl(TInt aIndex) const;
	
   /**
    * From CCoeControl indicate whether or not a key event was processed 
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

    #ifdef __SERIES60_HELP
    /**
    * From CCoeControl read help 
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    #endif
   
private:

   /**
    *   Default constructor
    */
    CSettingsContainer();

   /**
    *   Second phase constructor. Operations which might leave should 
    *   be called here
    *   @param aRect  reference to container area
    *   @param aAppUi the pointer to  the CImagePrintUiAppUi
    */ 
    void ConstructL( const TRect& aRect, CImagePrintUiAppUi* aAppUi );



private: // Data

    CAknSettingStyleListBox*    iSettingsMainListBox;   // The setting items list (scrollable)
    CSettingsItems*             iSettingsItems;         // Format strings for list items
    CCapabilityManager*         iCapabilityManager;     // Contains capability needed by settings
    CImagePrintUiAppUi*         iAppUi;
    CAknsBasicBackgroundControlContext* iSkinContext;  // skin data
    };

#endif // CSETTINGSCONTAINER_H
