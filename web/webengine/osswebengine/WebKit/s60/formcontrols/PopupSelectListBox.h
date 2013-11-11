/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PopupSelectLIstBox.h
*
*/


#ifndef POPUPSELECTLISTBOX_H
#define POPUPSELECTLISTBOX_H

//  INCLUDES
#include <e32def.h>
#include <eiktxlbx.h>
#include <eikfrlb.h> //CFormattedCellListBox


// CONSTANTS


// FORWARD DECLARATIONS

class TBrCtlSelectOptionData;
class WebView;

// CLASS DECLARATION

/**
*  PopupSelectListBox extends CEikTextListBox
*/
//class PopupSelectListBox : public CEikFormattedCellListBox
class PopupSelectListBox : public CEikTextListBox
    {
 

    public: // Constructors and destructor
    
        /**
        * Overrides CEikListBox ConstructL
        * @return 
        */
        void ConstructL(WebView* aParent, TBool aMultipleSelection, const CFont* aFont);
        /**
        * UpdateContentL
        * Updates the list
        * @param aOptions - list of TBrCtlSelectOptionData items
        * @return 
        */
        void UpdateContentL(const RPointerArray<TBrCtlSelectOptionData>& aOptions);
                
        /**
        * Overrides CEikListBox SetFocus
        * @return 
        */
        void SetFocus(TBool aFocus);

        /**
        * Overrides CEikListBox SetFocus
        * @return 
        */
        TBool IsFocused() {return m_focused;}
        
        void SetBackground(TRgb aColor);

        /*
         * Destructor.
         */
        virtual ~PopupSelectListBox();
        
        TCoeInputCapabilities InputCapabilities() const;

    private:        // New functions


    private:        // Data

        //CArrayPtr<TBrCtlSelectOptionData>* m_options; // not owned
        CDesC16ArrayFlat* m_itemArray; // Array is owned but descriptors are NOT OWNED
        bool m_multipleSelection;
        bool m_focused;
        WebView* m_parent;
    };

#endif // POPUPSELECTLISTBOX_H

// End of File
