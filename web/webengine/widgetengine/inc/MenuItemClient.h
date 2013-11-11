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
* Description:  This class represents the Widget Extension object
*
*/

#ifndef __MENUITEMCLIENT_
#define __MENUITEMCLIENT_

//  INCLUDES
#include <e32base.h>
#include "brctlinterface.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION
namespace KJS {
    class WidgetEventHandler;
}

class CMenuItem : public CBase
{
public:
        /**
        * Constructor
        */
        static CMenuItem* NewL( const TDesC16& aText, TInt aCmdId, 
            TInt aInternalId, TBool aDimmed = EFalse, CMenuItem* aParent=NULL, 
            void* aObj = NULL, TBool aShow = EFalse );

        /**
        * Destructor.
        */
        virtual ~CMenuItem();

protected:
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * C++ constructor.
        */
        CMenuItem( const TDesC16& aText, TInt aCmdId, TInt aInternalId, 
            TBool aDimmed, CMenuItem* aParent = NULL, void* aObj = NULL, 
            TBool aShow = EFalse );
public:

        /*
        * Method text()
        * @since 3.2
        * @param none
        * @return text of menu item
        */
        const TDesC& text()
            {
            if ( iText ) 
                return *iText;
            else 
                return KNullDesC();
            }

        /*
        * Method cmdId()
        * @since 3.2
        * @param none
        * @return command id of menu item
        */
        TInt cmdId() { return iCmdId; }

        /*
        * Method internalId()
        * @since 3.2
        * @param none
        * @return internal id of menu item
        */
        TInt internalId() { return iInternalId; }

        /*
        * Method firstChild()
        * @since 3.2
        * @param none
        * @return first child of menu item
        */
        CMenuItem* firstChild() { return iFirstChild; }

        /*
        * Method nextSibling()
        * @since 3.2
        * @param none
        * @return next sibling of menu item
        */
        CMenuItem* nextSibling() { return iNextSibling; }

        /*
        * Method dimmed()
        * @since 3.2
        * @param none
        * @return dimmed status of menu item
        */
        TBool dimmed() { return iDimmed; }

        /*
        * Method show()
        * @since 3.2
        * @param none
        * @return Show status of menu item
        */
        TBool show() { return iShow; }

        /*
        * Method Callback()
        * @since 3.2
        * @param none
        * @return onselect callback function of menu item
        */
        KJS::WidgetEventHandler* callback() { return iCallback; }

        /*
        * Method obj()
        * @since 3.2
        * @param none
        * @return menu item javascript object
        */
        void* obj() { return iObj; }

        /*
        * Method parent()
        * @since 3.2
        * @param none
        * @return parent menu item
        */
        CMenuItem* parent() { return iParent; }

        /*
        * Method setText()
        * @since 3.2
        * @param text of menu item
        * @return none
        */
        void setText( const TDesC& aText )
            {
            HBufC* tmpText = HBufC::NewL( aText.Length() );
            tmpText->Des().Append( aText );
            delete iText;
            iText = tmpText;
            }

        /*
        * Method setCmdId()
        * @since 3.2
        * @param command id of menu item
        * @return none
        */
        void setCmdId( TInt aCmdId ) { iCmdId = aCmdId; }

        /*
        * Method setFirstChild()
        * @since 3.2
        * @param first child of menu item
        * @return none
        */
        void setFirstChild( CMenuItem* aFirstChild ) { iFirstChild = aFirstChild; }

        /*
        * Method setNextSibling()
        * @since 3.2
        * @param next sibling of menu item
        * @return none
        */
        void setNextSibling( CMenuItem* aNextSibling ) { iNextSibling = aNextSibling; }

        /*
        * Method setDimmed()
        * @since 3.2
        * @param dimmed status
        * @return none
        */
        void setDimmed( TBool aDimmed ) { iDimmed = aDimmed; }

        /*
        * Method setShow()
        * @since 3.2
        * @param show status
        * @return none
        */
        void setShow( TBool aShow ) { iShow = aShow; }

        /*
        * Method setCallback()
        * @since 3.2
        * @param callback function
        * @return none
        */
        void setCallback( KJS::WidgetEventHandler* aCallback ) { iCallback = aCallback; }

        /*
        * Method setObj()
        * @since 3.2
        * @param menu item javascript object
        * @return none
        */
        void setObj( void* aObj ) { iObj = aObj; }

private:
        HBufC*                  iText;//data
        TInt                    iCmdId;//data
        TInt                    iInternalId;//data
        CMenuItem*              iParent;//data
        CMenuItem*              iFirstChild;//left node
        CMenuItem*              iNextSibling;//right node
        TBool                   iDimmed;//ETrue to dim this menu item. EFalse to un-dim this menu item.
        KJS::WidgetEventHandler*     iCallback;
        void*                   iObj;
        TBool                   iShow;
};

#endif
