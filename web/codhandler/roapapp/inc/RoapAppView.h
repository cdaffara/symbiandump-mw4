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
* Description: 
*      Declaration of class CRoapAppView.   
*      
*
*/


#ifndef ROAP_APP_VIEW_H
#define ROAP_APP_VIEW_H

// INCLUDES

#include <coecntrl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h> 

// FORWARD DECLARATION

class CCodData;
class CEikLabel;
class MObjectProvider;

// CLASS DECLARATION

/**
* Application's main view.
*/
class CRoapAppView: public CCoeControl
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param aRect Rectangle to draw.
        * @param aMopParent MOP Parent.
        * @return The constructed view.
        */      
        static CRoapAppView* NewL
            ( const TRect& aRect, MObjectProvider* aMopParent );

        /**
        * Destructor.
        */
        virtual ~CRoapAppView();

    public:     // from CCoeControl

        /**
        * Size changed. Lay out and draw components.
        */
        void SizeChanged();

        /**
        * Count component controls.
        * @return Number of component controls.
        */
        TInt CountComponentControls() const;

        /**
        * Return component control.
        * @param aIndex Index of component control.
        * @param Component control.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Handle key event.
        * @param aKeyEvent Key event.
        * @param aType Event type.
        * @return Key response.
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Draw this control.
        * @param aRect Rectangle to draw.
        */
        void Draw( const TRect& aRect ) const;
        
      	/** 
      	* Gets an object whose type is encapsulated by the specified TTypeUid object.
        * @param aId Encapsulates the Uid that identifies the type of object required.
	    * @return Encapsulates the pointer to the object provided. 
	    * Note that the encapsulated pointer may be NULL.
        */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    protected:  // Constructors

        /**
        * Constructor.
        * @param aMopParent MOP Parent.
        */
        CRoapAppView( MObjectProvider* aMopParent );

        /**
        * Second phase constructor. Leaves on failure.
        * @param aRect Rectangle to draw.
        */      
        void ConstructL( const TRect& aRect );

    private:    // data

        CEikLabel* iLabel;  ///< Label. Owned.
        
        CAknsBasicBackgroundControlContext* iBgContext;  ///<BgContext. Owned.

    };

#endif /* def ROAP_APP_VIEW_H */
