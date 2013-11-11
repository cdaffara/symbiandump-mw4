/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dialog containing the Downloads List
*
*/



#ifndef CDOWNLOADSLISTDLG_H
#define CDOWNLOADSLISTDLG_H

//  INCLUDES
#include <e32base.h>
#include <eikmobs.h>
#include <aknPopup.h>

// DATA TYPES
enum TDownloadsListDlgEvent
    {
    EDownloadAdded,
    EDownloadChanged,
    EDownloadRemoved
    };

// FORWARD DECLARATIONS
class CEikonEnv;
class CEikFormattedCellListBox;
class CEikMenuBar;
class CDownloadsListArray;
class MDownloadsListDlgObserver;
class RHttpDownload;
class CDownloadUtils;

// CONSTANTS
/// Refresh timer setting. If this is zero, no timer is used! (Micro is 10^(-6))
const TInt KRefreshIntervalMicroSeconds = 2000000;
/** Refresh timer setting. Active object priority. Ensure that this is higher 
*   than standard so that user will be notified. */
const TInt KRefreshTimerPriority = CActive::EPriorityStandard + 1;

// CLASS DECLARATION

/**
*  This is the 'view' containing the Downloads List with Options menu.
*  Use it like a dialog!
*
*  @lib Download Manager UI Lib
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS( CDownloadsListDlg ) : public CAknPopupList, public MEikMenuObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDownloadsListDlg* NewL( MDownloadsListDlgObserver& aDlgObserver );
        
        /**
        * Two-phased constructor.
        */
        static CDownloadsListDlg* NewL( MDownloadsListDlgObserver& aDlgObserver, TBool aProgressiveDownload );

        /**
        * Destructor.
        */
        virtual ~CDownloadsListDlg();

    public: // New functions

        /**
        * Set the model.
        */
        void SetModelL( CDownloadsListArray& aModel );

        /**
        * Notify the dialog that model has changed.
        */
        void HandleModelChangeL( TDownloadsListDlgEvent aEvent, TInt aIndex );

        /**
        * The index of the highlighted download.
        */
        TInt CurrentItemIndex() const;

        /**
        * Display the Options menu.
        */
        void DisplayMenuL();

        /**
        * Hide the Options menu.
        */
        void HideMenu();

        /**
        * Is the Options menu opened?
        */
        TBool MenuShowing() const;

        /**
        * Move the highlight to the given download.
        */
        void HighlightDownload( RHttpDownload& aHighlightDl );

        /**
        * Handle Middle Softkey Change
        */
        void HandleMiddleSoftKeyChangeL();

    public: // From CAknPopupList

        /**
        * Execute the list. It calls CAknPopupList::ExecuteLD(), which calls CActiveScheduler::Start()!
        * @return ETrue if the popup was accepted. EFalse if the popup was cancelled.
        */
        TBool ExecuteLD();

    public: // From MEikMenuObserver

        void ProcessCommandL( TInt aCommandId );
        void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public: // from MEikListBoxObserver
    	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);


    public: // From CCoeControl

        void FocusChanged( TDrawNow aDrawNow );
        
        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        void HandlePointerEventL(const TPointerEvent& /*aPointerEvent*/);

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CDownloadsListDlg( MDownloadsListDlgObserver& aDlgObserver );

        /**
        * C++ default constructor.
        */
        CDownloadsListDlg( MDownloadsListDlgObserver& aDlgObserver, TBool aProgressiveFlag );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:  // From CAknPopupList

        /**
        * Key event handling.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    private: // New functions
    
        /**
        * Number of visible, inprogress downlods.
        */
        TInt VisibleInProgressDownloads() const;
        
        /**
        * Refreshes progress info for the given download at index.
        */
        void RefreshProgressL( TInt aIndex );
        
        /**
        * Refreshing timer static callback.
        */
        static TInt RefreshTimerCallback( TAny* aPtr );
        
    private:  // Data

        CEikonEnv& iMyEikonEnv;
        CEikFormattedCellListBox* iListBox; ///< Owned.
        CEikMenuBar* iMenuBar; ///< Owned menu bar.
        MDownloadsListDlgObserver& iDlgObserver;
        CDownloadsListArray* iDownloadsListArray; ///< Not owned;
        RHttpDownload* iHighlightDownload; ///< Not owned.
        CPeriodic* iRefreshTimer; ///< Refreshes the list. Owned.
        CDownloadUtils* iDownloadUtils; ///< Owned. 
        TBool iIsMSKChangeHandled;
		TBool iProgressiveDownload;
    };

#endif /* CDOWNLOADSLISTDLG_H */
