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
* Description: 
*
*/


#ifndef WIDGETUIWINDOWCONTAINER_H_
#define WIDGETUIWINDOWCONTAINER_H_

// INCLUDES

#include <coecntrl.h>
#include <AknServerApp.h>

#include "WidgetUiObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CBrCtlInterface;
class CWidgetUiWindowManager;
class CWidgetUiWindowView;
class CAknsBasicBackgroundControlContext;


// CLASS DECLARATION

/**
*
*  @lib WidgetUI.exe
*  @since 3.1
*/
class CWidgetUiWindowContainer : public CCoeControl
    {
    public: // Constructors and destructor
        /** 
        * NewL
        * two-phase constructor
        * @since 3.1
        */
        static CWidgetUiWindowContainer* NewL( 
            const TRect& aRect, 
            CWidgetUiWindowManager& aWindowManager );
  
        /**
        * ~CWidgetUiWindowContainer
        * Destructor
        * @since 3.1
        * @param none
        * @param none
        */
        virtual ~CWidgetUiWindowContainer();
     
    public: // New functions

        /**
        * HandleCommandL
        * @since 3.1
        * @param aCommand id
        * @return void
        */
        void HandleCommandL( TInt aCommand );

        /**
        * HandleKeyEventL
        * @since 3.1
        * @param aKeyEvent
        * @param aType
        * @return TKeyResponse
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * SizeChanged
        * @since 3.1
        * @param none
        * @return void
        */
        void SizeChanged();
 
    private: // Functions from base classes

        /**
        * CountComponentControls
        * @since 3.1
        * @param none
        * @return TInt the count of sub controls
        */
        TInt CountComponentControls() const;

        /**
        * ComponentControl
        * @since 3.1
        * @param aIndex
        * @return CCoeControl*
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Draw
        * @since 3.1
        * @param aRect
        * @return void
        */
        void Draw( const TRect& aRect ) const;

        /**
        * OfferKeyEventL
        * @since 3.1
        * @param aKeyEvent
        * @param aType
        * @return TKeyResponse
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * FocusChanged
        * @since 3.1
        * @param TDrawNow
        * @param aDrawNow
        * @return void
        */
        void FocusChanged(TDrawNow aDrawNow);
        
    private:

        /**
        * C++ default constructor.
        */
        CWidgetUiWindowContainer( CWidgetUiWindowManager& aWindowManager );

        /**
        * ConstructL
        * EPOC default constructor
        * @since 3.1
        * @param aRect Frame rectangle for container.
        * @param aView
        * @param aWindowManager
        * @return void
        */
        void ConstructL( const TRect& aRect );

    private: // new functions
    
        /**
        * CancelLoad
        * @since 3.1
        * @param none
        * @return void
        */
        void CancelLoad();

        /**
        * Engine
        * @since 3.1
        * @param none
        * @return CBrCtlInterface*
        */
        CBrCtlInterface* Engine() const;

        /**
        * ErrorNoteL
        * Error note display
        * @since 3.1
        * @param aPrompt
        * @return void
        */
        void ErrorNoteL( const TDesC& aPrompt );

        /**
        * InfoNoteL
        * Confirmation note display
        * @since 3.1
        * @param aPrompt
        * @return void
        */
        void InfoNoteL( const TDesC& aPrompt );

    private: // data

        CWidgetUiWindowManager&     iWindowManager; // not owned, not responsible for deleting
        TInt                        iCommandBase;
       
#ifdef _DEBUG
        RFileLogger                 iFileLogger;
        TBool                       iCanLog;
#endif
       
        CAknsBasicBackgroundControlContext* iSkinContext; // owned, responsible for deleting
        
    };

#endif // WIDGETUIWINDOWCONTAINER_H

// End of File
