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
* Description:  Listbox for select element.
*
*
*/


#ifndef CBROWSERSELECTELEMENTLISTBOX_H
#define CBROWSERSELECTELEMENTLISTBOX_H

//  INCLUDES
#include <aknlists.h>
#include <BrCtlDialogsProvider.h>

// CLASSES

class TBrCtlSelectElementFilterTextArray: public MDesCArray
    {
    /**
    *   Construction
    */
    public: 
        TBrCtlSelectElementFilterTextArray(
                        const CArrayFix<TBrCtlSelectOptionData>& aOptions );

    public:     // from MDesCArray

        /**
        * Get number of items in the array.
        * @return Number of items.
        */
        TInt MdcaCount() const;

        /**
        * Text to be displayed for this item.
        * @param aIndex Index of item.
        * @return TPtrC for this item text.
        */
        TPtrC MdcaPoint( TInt aIndex ) const;

        /**
        * The model uses this class.
        */
        friend class CBrowserSelectElementModel;

    private:    // data

        /**
        * Pointer to real data. Not owned.
        */
        const CArrayFix<TBrCtlSelectOptionData>& iOptions;
    };

class TBrCtlSelectElementItemTextArray: public MDesCArray
    {
    public:
        /**
        * Contructor.
        */
        TBrCtlSelectElementItemTextArray( 
                        const CArrayFix<TBrCtlSelectOptionData>& aOptions );
        
        /**
        * Destructor.
        */
        ~TBrCtlSelectElementItemTextArray();

    public:     // from MDesCArray

        /**
        * Get number of items in the array.
        * @return Number of items.
        */
        TInt MdcaCount() const;

        /**
        * Text to be displayed for this item.
        * @param aIndex Index of item.
        * @return TPtrC for this item text.
        */
        TPtrC MdcaPoint( TInt aIndex ) const;

        void FormatTextL(  const TBrCtlSelectOptionData& optionData );

        HBufC* AddPrefixLC( const TDesC& aText, TBool aIsOptionGroup ) const;

        /**
        * The model uses this class.
        */
        friend class CBrowserSelectElementModel;

    private:    // data

        const CArrayFix<TBrCtlSelectOptionData>& iOptions; //Pointer to real data. Not owned.
        HBufC* iText;
    };


/**
*  Implements the WML selection list.
*/
class CBrowserSelectElementListBox : 
                                public CAknSingleGraphicPopupMenuStyleListBox
    {
    public: // Constructors and destructor

        CBrowserSelectElementListBox( 
                        const TBrCtlSelectOptionType aBrCtlSelectOptionType,
                        const CArrayFix<TBrCtlSelectOptionData>& aOptions );
        
        /**
        * C++ default constructor.
        * @param aParent The parent control.
        * @param aElement The select element.
        */
        void ConstructL( const CCoeControl& aParent );
  
    public: // Functions from CAknSingleGraphicStyleListBox

        /**
        * Creates its own itemdrawer.
        */
        void CreateItemDrawerL();

        /**
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type.
        * @return Response.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * Calls the base class's method if its possible.
        * @param aDrawNow Enable redraw.
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Calls AdjustHighlight.
        */
        void SizeChanged();

        void HighlightSelectableItem();

        /**
        * Sets the selection, and moves the highlight to the right position.
        */
        void SetSelectionIndexesL();

        void HandleResourceChangeL( TInt aType );
        /**
        * HandlePointerEventL
        * From CCoeControl
        *
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);


    private:    // New functions

        /**
        * Moves the highlight to the first visible selected item
        * or the first item.
        */
        void AdjustHighlight();

    private:

        const CArrayFix<TBrCtlSelectOptionData>& iOptions;

        const TBrCtlSelectOptionType iBrCtlSelectOptionType;
    
        TBool iHasNotMoved;//show whether the user has pressed any key
    };

#endif
            
// End of File
