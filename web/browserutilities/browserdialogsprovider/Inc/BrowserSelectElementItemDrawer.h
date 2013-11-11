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

#ifndef CBROWSERSELECTELEMENTITEMDRAWER_H
#define CBROWSERSELECTELEMENTITEMDRAWER_H

// INCLUDES
#include "BrCtlDialogsProvider.h"

#include <aknlists.h>
#include <avkon.mbg>

/**
*  Itemdrawer for the WML selection list.
*/
class CBrowserSelectElementItemDrawer : public CFormattedCellListBoxItemDrawer
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param aTextListBoxModel The model.
        * @param aFont The font.
        * @param aFormattedCellData The data.
        * @param The select element.
        */
        CBrowserSelectElementItemDrawer(
            MTextListBoxModel& aTextListBoxModel,
            const CFont& aFont,
            CFormattedCellListBoxData& aFormattedCellData,
            TBrCtlSelectOptionType aBrCtlSelectOptionType,
            const CArrayFix<TBrCtlSelectOptionData>& aOptions );

    public: // New functions

        /**
        * Fills the iconarray depending on select element's data.
        */
        void LoadIconsL();

    public: // From CFormattedCellListBoxItemDrawer

        /**
        * @see CFormattedCellListBoxItemDrawer.
        */
        void DrawItem( TInt aItemIndex,
                       TPoint aItemRectPos,
                       TBool aItemIsSelected,
                       TBool aItemIsCurrent,
                       TBool aViewIsEmphasized,
                       TBool aViewIsDimmed ) const;

    private:    // Data

        const CArrayFix<TBrCtlSelectOptionData>& iOptions;

        TBrCtlSelectOptionType iBrCtlSelectOptionType;
    };

#endif
// End of file
