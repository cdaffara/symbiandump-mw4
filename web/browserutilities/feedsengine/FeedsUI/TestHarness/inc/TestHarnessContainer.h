/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A container to change settings.
*
*/


#ifndef SETTINGS_CONTAINER_H
#define SETTINGS_CONTAINER_H


// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTestHarnessView;

// CLASS DECLARATION


/**
*  A container to change settings.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CTestHarnessContainer: public CCoeControl
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CTestHarnessContainer* NewL(CTestHarnessView& aView, const TRect& aRect);
        
        /**
        * Destructor.
        */        
        ~CTestHarnessContainer();


    public: // From CoeControl
	    /**
	    * Handles key event.
	    *
        * @param aKeyEvent The key event.
        * @param aType The type of the event.
        * @return Indicates whether the key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);


    protected: // From CoeControl
        /**
        * Called by framework when the view size is changed.
        *
        * @since ?
        * @return void.
        */
        void SizeChanged();

        /**
        * Returns number of components.
	    *
        * @since ?
        * @return Number of component controls.
        */
        TInt CountComponentControls() const;

        /**
        * Returns pointer to particular component.
	    *
        * @since ?
        * @param aIndex Index whose control's pointer has to returned.
        * @return Pointer to component control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * Draws the container.
        *
        * @since ?
        * @param aRect The rect to draw into.
        * @return Void.
        */
        void Draw(const TRect& aRect) const;


    private:
        /**
        * C++ default constructor.
        */
        CTestHarnessContainer(CTestHarnessView& aView);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TRect& aRect);


    private:
        CTestHarnessView&  iView;
    };

#endif      // SETTINGS_CONTAINER_H
            
// End of File
