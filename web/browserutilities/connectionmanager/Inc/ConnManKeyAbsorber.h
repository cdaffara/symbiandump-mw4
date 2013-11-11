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
* Description: 
*
*  CConnManKeyAbsorber class. Absorbs all the key presses.
*
*
*
*/



#ifndef __Conn_Man_Key_Absorber_H__
#define __Conn_Man_Key_Absorber_H__

//  INCLUDES
#include <coecntrl.h>

// Forward references
class CEikAppUi;

// CLASS DECLARATION
/**
* CConnManKeyAbsorber
*   Absorbs all the key presses.
*/
NONSHARABLE_CLASS(  CConnManKeyAbsorber ) : public CCoeControl
    {
    public:
        /**
        * Two-phased constructor.
        */
       static CConnManKeyAbsorber* NewLC();

        /**
        * Destructor.
        */
        virtual ~CConnManKeyAbsorber();
	public:
		/**
		*
		*/
		void RemoveFromStack();

		
		/**
		*
		*/
		void AddToStackL();

		/**
		*This is the function responsible to wrap on in a TCleanupItem and push it 
		*/
		static void CleanupOnLeave( TAny* aSelf );

    private:
        /**
        * From CCoeControl
        */
        virtual TKeyResponse OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/);

    protected:
        /**
        * C++ default constructor.
        */
        CConnManKeyAbsorber();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    public:

		TBool iAddedToStack;

    private: // Data
        CEikAppUi*  iAppUi;
    };

#endif //__Conn_Man_Key_Absorber_H__

// End of File