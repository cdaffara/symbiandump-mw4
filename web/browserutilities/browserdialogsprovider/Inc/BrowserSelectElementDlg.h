/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Popuplist for select element.
*
*
*/


#ifndef BROWSERSELECTELEMENTDLG_H
#define BROWSERSELECTELEMENTDLG_H

//  INCLUDES

// User includes

// System includes
#include <BrCtlDialogsProvider.h>

#include <aknPopup.h>
#include <aknlists.h>

// FORWARD DECLARATIONS
class CBrowserSelectElementListBox;
class CBrowserDialogsProviderAsyncExit;

// CLASS DECLARATION

/**
*   The dialog, that pops up the selection list
*   for the select element.
*/
class CBrowserSelectElementDlg : public CAknPopupList
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aTitle title of the listbox
        * @param aBrCtlSelectOptionType
        * @param aOptions listbox elements array
        */
        static CBrowserSelectElementDlg* NewL( 
                        const TDesC& aTitle,
                        const TBrCtlSelectOptionType aBrCtlSelectOptionType,
                        CArrayFix<TBrCtlSelectOptionData>& aOptions );

        /**
        * Destructor.
        */
        virtual ~CBrowserSelectElementDlg();

    protected:  // Constructors and destructor
    
        /**
        * C++ default constructor.
        * @param aElement The select element
        */
        CBrowserSelectElementDlg( 
            const TBrCtlSelectOptionType aBrCtlSelectOptionType,
            CArrayFix<TBrCtlSelectOptionData>& aOptionsOrg );

        /**
        * EPOC constructor that can leave.
        * @param aTitle
        */
        void ConstructL( const TDesC& aTitle );
        
    public: // From MEikListBoxObserver

        /**
        * Handles listbox events.
        * Closes popup when item is selected in a singleselect list.
        * @since Series60 1.2
        * @param aListBox Source of event
        * @param aEventType Event type
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

        /**
        * Handles listbox events.
        * Closes popup when item is selected in a singleselect list.
        * @since Series60 1.2
        * @param aListBox Source of event
        * @param aEventType Event type
        */
        void HandleControlEventL
            ( CCoeControl* aControl, TCoeEvent aEventType );

    protected:  // From CAknPopupList

        void AttemptExitL( TBool aAccept );

    public:

        /**
        * Force the dialog to destroy itself
        */
        void DestroyPopupL();
 
    protected:

        /**
        * Store selection in the original options array
        */
        void StoreSelections();

    private:    // Data

        TBool                               iContentValid;

        TBrCtlSelectOptionType              iBrCtlSelectOptionType;

        CBrowserSelectElementListBox*       iListBox;       // owned
        CAknSinglePopupMenuStyleListBox*    iHistoryList;   // owned

        CArrayFix<TBrCtlSelectOptionData>&  iOptionsOrg;    // not owned

        CBrowserDialogsProviderAsyncExit*   iAsyncExit;      // owned
        
        TInt iResource;
    };

#endif  //BROWSERSELECTELEMENTDLG_H
            
// End of File
