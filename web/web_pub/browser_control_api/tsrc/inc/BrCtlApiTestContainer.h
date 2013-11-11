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
* Description:  CBrCtlApiTestContainer from BrCtlBCTestContainer.h
*
*/

#ifndef BRCTLBCTESTCONTAINER_H
#define BRCTLBCTESTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <BrCtlInterface.h> // interface under test

#include "BrCtlApiTestObserver.h"
#include "BrCtlApiTestStateChangeObserver.h"
#include "BrCtlApiTestDialogsProvider.h"
   
// CONSTATNS
const TInt KThisAppUid = 0xA0000181;

_LIT( KUrlLoadTestHtml, "c:\\BrCtlTest\\loadtest.html" );
_LIT( KUrlLoadTestHtml2, "file:///c:\\BrCtlTest\\loadtest2.html" );
_LIT( KUrlHistoryHandlerHtml1, "file:///c:\\BrCtlTest\\historytest1.html" );
_LIT( KUrlHistoryHandlerHtml2, "file:///c:\\BrCtlTest\\historytest2.html" );
_LIT( KUrlHistoryHandlerHtml3, "file:///c:\\BrCtlTest\\historytest3.html" );
_LIT( KUrlLoadTestWml1, "file:///c:\\BrCtlTest\\loadtest1.wml" );
_LIT( KUrlLoadTestWml2, "file:///c:\\BrCtlTest\\loadtest2.wml" );
_LIT( KUrlLoadTestWml3, "file:///c:\\BrCtlTest\\loadtest3.wml" );

enum TBrCtlFindResponseExpected
  {
  EFindNoMatchesExpected = 0, ///< Reserved for future use
  EFindMatchExpected,    ///< Reserved for future use
  EFindMoreMatchesExpected    ///< Reserved for future use
  };



// FORWARD DECLARATIONS
//class CBrCtlInterface;

// CLASS DECLARATION

/**
*  CBrCtlApiTestContainer  container control class.
*  
*/
class CBrCtlApiTestContainer : public CCoeControl,
                                     MCoeControlObserver,
                                     MBrCtlDataLoadSupplier,
                                     MBrCtlCommandObserver,
                                     MWidgetCallback // Added for WidgetExtension Test
    {
    public: // Constructors and destructor
        void ConstructL (const TRect& aRect );
        ~CBrCtlApiTestContainer();

    private: // Functions from base classes 
        void HandleCommandL( TBrCtlDefs::TBrCtlClientCommands aCommand, const CArrayFix<TPtrC>& aAttributesNames,
                                     const CArrayFix<TPtrC>& aAttributeValues ); 
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        void Draw( const TRect& aRect ) const;
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * From MBrCtlDataLoadSupplier, Cancel an on-going load
        */
        void CancelLoad();
        
    // Functions from MWidgetCallback class
        TBool DialogMimeFileSelectLC(HBufC*& aSelectedFileName,
                                 const TDesC& aMimeType);

       /**
        * Called to show or hide softkeys
        * @since 3.1
        * @param aVisible ETrue to show softkeys, EFalse when full screen is needed
        */
        void SetSoftkeysVisible(TBool aVisible);

        /**
        * Called to change the display orientation to landscape
        * @since 3.1
        */
        void SetDisplayMode(TBrCtlDefs::TBrCtlOrientation aOrientation);

        //Reserved for future use
        TInt Reserved_1(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_2(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_3(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_4(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_5(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_6(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_7(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_8(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_9(TAny*& a0, TAny* a1, TAny* a2);
        TInt Reserved_10(TAny*& a0, TAny* a1, TAny* a2);



    public: // New functions
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType );

    public: // Functions utility
        HBufC8* ReadFileLC( const TDesC& aFileName );
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
        void RunAllTestsL();

    public: // Functions to test CBrCtlInterface
        void CreateBrowserControlInterfaceL(TInt aBrCtlConfig); //Change done to take a parameter
        TInt LoadPageL();
        TInt HandleCommandL(TInt aCommand );
        TInt LoadDataL();
        TInt ClearCacheTest();
        TBool IsUrlInCacheTest( TDesC16& aUrl );
        TInt ClearItemInCacheTest();
        TInt PageInfoLC( TBrCtlDefs::TBrCtlPageInfo aType );
        TInt CertInfo();
        TInt NavigationAvailableTest();
        TInt GetBitmapData();
        TInt SaveToFileL();
        TInt OkToExit();
        TInt FindKeyword( TPtrC aHtml , TPtrC aKeyword , TInt aMatch );
        TInt FindKeywordAgain( TInt aMatch );
        TInt SetFocus();
        TInt OfferKeyEvent();
        TInt BrCtlMinimumSize();
        TInt BrCtlSubscribeToMenuItemsL();
        TInt BrCtlContentSize();
        TInt BrowserSettingL( TUint& aSetting );
        TInt SetBrowserSettingL();
        TInt FocusedImageL();
        TInt AddCommandObserverL();
        TInt RemoveCommandObserver();
        TInt ImageCountL();
        TInt BrCtlFocusedElementTypeL();
        TInt BrCtlSetParam();
        TInt LoadSavedPageL();
        TInt SetSelfDownloadContentTypes();
        TInt ZoomLevels();
        TInt AddOptionMenuItems();
        TInt BrCtlInitLoadDataL();
        TInt HandleDownloadCommand();
        TInt VersionInfoL();
        TInt WmlOptionMenuItemsL();
        TInt BrCtlPostL();
        TInt CreateBrCtlInterfaceAndWidgetExtensionL(); // Added for WidgetExtension Test
        TInt HistoryLoad1L();
        TInt HistoryLoad2L();
        TInt HistoryLoad3L();
        TInt WMLLoad1L();
        TInt WMLLoad2L();
        TInt WMLLoad3L();
        
    private: //data
        CBrCtlInterface* iBrCtlInterface;
        TInt iCommandBase;
        CBrCtlApiTestObserver* iBrCtlApiTestObserver;
        CBrCtlApiTestStateChangeObserver* iBrCtlApiTestStateChangeObserver;
        CBrowserDialogsProviderProxy* iBrCtlDialogs;
        CEikMenuPane* iMenuPane;
        TBool iCancelInitDataLoad;
        
        // Added for Widget Extension Test
        MWidgetExtension* iWidgetExtension;  
    };

#endif // BRCTLBCTESTCONTAINER_H

// End of File
