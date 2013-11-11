/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View of WidgetUi application
*
*/


#ifndef WIDGETUIWINDOWVIEW_H_
#define WIDGETUIWINDOWVIEW_H_

//  INCLUDES
#include <aknview.h>
#include <brctlinterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWidgetUiWindowContainer;
class CBrCtlInterface;
class CWidgetUiObserver;
class CWidgetUiWindowManager;
class CAknNavigationDecorator;
class CAknIndicatorContainer;
class CAknNavigationControlContainer;

// CLASS DECLARATION

/**
*  CWidgetUiWindowView
*  @lib resLoader.lib
*  @since 3.1
*/
class CWidgetUiWindowView : public CAknView,
                            public MBrCtlStateChangeObserver
    {
    public:  // Constructors and destructor

        /**
        * NewLC
        * Two-phased constructor.
        * @since 3.1
        * @param aWindowManger
        * @return CWidgetUiWindowView*
        */
        static CWidgetUiWindowView* NewLC( CWidgetUiWindowManager& aWindowManager );

        /**
        * ~CWidgetUiWindowView
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiWindowView();

    public: // new functions

         /**
        * Container
        * @since 3.1
        * @param none
        * @return CWidgetUiWindowContainer*
        */
        CWidgetUiWindowContainer* Container();

        
        /**
        * UpdateStatusPane
        * show/hide the status pane 
        * @since 3.1
        * @param aVisible - ETrue to show; EFalse to hide
        * @return void
        */
        void UpdateStatusPane( TBool aVisible );

        /**
        * CbaGroup
        * @since 3.1
        * @param none
        * @return CEikButtonGroupContainer*
        */
        CEikButtonGroupContainer* CbaGroup() { return Cba(); }

        /**
        * IsOptionsMenuActivated
        * @return ETrue if Options menu loaded, EFalse otherwise
        */
        TBool IsOptionsMenuActivated();
        /**
        * DeActivateOptionsMenu
        * Set Optionsmenu loaded flag to EFalse
        * @return void
        */
        void DeActivateOptionsMenu();
    public: // Functions from base classes

        /**
        * HandleCommandL
        * @since 3.1
        * @param aCommand
        * @return void
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Id
        * @since 3.1
        * @param none
        * @return TUid
        */
        TUid Id() const;
        
        /**
        * Find if the platform supports touch
        * @return ETrue if the platform supports touch, EFalse otherwise
        */
        TBool PenEnabled() { return iPenEnabled; }

    public: // Functions from MEikStatusPaneObserver

        /**
        * HandleStatusPaneSizeChange
        * @since 3.1
        * @param none
        * @return void
        */
        void HandleStatusPaneSizeChange();

    public:     // from MBrCtlStateChangeObserver

        /**
        * State change event handling for disable javascript notification changes.
        * @param aState which state we are in
        * @param aValue state specific value
        * @return void
        */
        void StateChanged( TBrCtlDefs::TBrCtlState aState, TInt aValue );
        
        /**
        * UpdateToolbar in case Orientation is changed
        * @param aShow to define visibility of toolbar
        * @return void
        */        
        void UpdateToolbar(TBool aShow);
        
    private:

        /**
        * CWidgetUiWindowView
        * Constructor
        * @since 3.1
        * @param aWindowManager
        * @return none
        */
        CWidgetUiWindowView( CWidgetUiWindowManager& aWindowManager );

        /**
        * ConstructL
        * EPOC default constructor.
        * By default Symbian 2nd phase constructor is private.
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();

    private: // from CAknView

        /**
        * DynInitMenuPaneL
        * @since 3.1
        * @param aResourceId
        * @param aMenuPane
        * @return void
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * DoActivateL
        * @since 3.1
        * @param aPrevViewId
        * @param aCustomMessageId
        * @param aCustomMessage
        * @return void
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage );

        /**
        * DoDeactivate
        * @since 3.1
        * @param none
        * @return void
        */
        void DoDeactivate();

        /**
        * Redraw
        * @since 3.1
        * @param none
        * @return void
        */
        void Redraw() const;
        
      private:
          
          /**
          * IsEditMode check if over an editable element
          * @return ETrue is over an editable element ; otherwise return EFalse
          */        
          TBool IsEditMode();

          
      private :  // Data

        CWidgetUiWindowContainer*           iContainer;     // Component container owned
        CWidgetUiWindowManager&             iWindowManager; // not owned, not responsible for deleting
        CAknNavigationDecorator*            iNaviDecorator; // owned, responsible for deleting
        CAknIndicatorContainer*             iIndiContainer; // not owned, not responsible for deleting
        CAknNavigationControlContainer*     iNaviCont;
        TBool                               iCbaVisible;
        TBool                               iStatusPaneVisible;
        TBool                               iPenEnabled;
        TBool                               iActivatedObjectVisible; 
		TBool 								iIsOptionsMenuActivated;
  };

#endif // CWidgetUIWINDOWVIEW_H_

// End of File
