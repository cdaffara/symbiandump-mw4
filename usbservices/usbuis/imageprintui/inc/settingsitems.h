/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for settingsitems
*
*/




#ifndef CSETTINGSITEMS_H
#define CSETTINGSITEMS_H

//  EXTERNAL INCLUDES
#include <bamdesca.h>
#include <eiklbo.h>
#include "settingstables.h"
#include "pictbridge.h"

// FORWARD DECLARATIONS
class CCapabilityManager;
class CNotes; 

// CONSTANTS
_LIT( KTab, "\t" );
const TInt KSettingsMaxSize = 256; 
const TInt KDefaultArrayValue = 3;

/**
 *
 *  List box model that handles the contents of the list box and launches
 *  the pop-up dialogs that are used .
 *
 */
class CSettingsItems :
    public CBase,
    public MEikListBoxObserver,
    public MDesCArray
    {

public:     

    /**
     *  Two-phase constructor
     *
     *  @param aManager the pointer to the  CapabilityManager
     *  @param aNotes the pointer to the  CNotes
     *  @return Initialized instance of CSettingsItems
     */
    static CSettingsItems* NewL( CCapabilityManager* aManager, CNotes* aNotes); 

   /**
     *  Two-phase constructor
     *
     *  @param aManager the pointer to the  CapabilityManager
     *  @param aNotes the pointer to the  CNotes
     *  @return Initialized instance of CSettingsItems
     */
    static CSettingsItems* NewLC( CCapabilityManager* aManager, CNotes* aNotes); 


    /**
     *  Destructor
     */
    virtual ~CSettingsItems();

private:        //  Constructors and destructors

   /**
    *  Constructor
    *  @param aManager the pointer to the  CapabilityManager
    *  @param aNotes the pointer to the  CNotes  
    */
    CSettingsItems( CCapabilityManager* aManager, CNotes* aNotes); 

   /**
    *  2nd phase constructor
    */
    void ConstructL();

public:         //  Methods derived from MEikListBoxObserver


   /**
	*  Framework method to handle listbox event
	*/
    void HandleListBoxEventL( CEikListBox* aListBox,
                              TListBoxEvent aEventType );

public:        //  Methods derived from MDesCArray


    
   /**
	*  Return number of items in listbox
	*/
    TInt MdcaCount() const;
    
   /**
	*  Creates the text string for the list box item and returns it,
	*  set sting to iBubber,  can leave
	*/
    TPtrC MdcaPointL( TInt aIndex ) const;
    
   /**
	*  Creates the text string for the list box item and returns it,
	*  set string to iBuffer
	*/
    TPtrC MdcaPoint( TInt aIndex ) const;


private:    

    /**
     *  Opens the radio button page to change print settings
     */
     void HandleEvent( TInt aCapability, 
                            const TConversionTable* aTable, 
                            const TInt aTableSize, const HBufC* aTitle ); 
    /**
     *  Opens the radio button page to change print settings, can leave
     */
     void HandleEventL(TInt aCapability, 
    					    const TConversionTable* aTable,
    					    const TInt aTableSize, const HBufC* aTitle );
    
    /**
     *  Retrieve current print setting string 
     */
    void GetCurrentStringL( TInt aCapability, 
                            const TConversionTable* aTable, 
                            const TInt aTableSize ) const;


private:  // Data

    CCapabilityManager* iCapabilityManager;
    
    CNotes* iNotes;

    TInt iCount;
    
    HBufC* iBuffer;
    
    HBufC* iLayout; 
    
    HBufC* iPaperSize; 
    
    HBufC* iQuality; 
    };

#endif