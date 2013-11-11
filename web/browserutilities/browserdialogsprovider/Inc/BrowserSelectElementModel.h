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

#ifndef CBROWSERSELECTELEMENTMODEL_H
#define CBROWSERSELECTELEMENTMODEL_H

// INCLUDES
#include <AknUtils.h>

// FORWARD DECLARATIONS
class TBrCtlSelectElementFilterTextArray;
class TBrCtlSelectElementItemTextArray;
class TBrCtlSelectOptionData;

// CLASS DECLARATION

/**
* Model for the WML selection list.
*/
class CBrowserSelectElementModel : public CAknFilteredTextListBoxModel
    {
    protected:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param aElement The select element.
        */
        CBrowserSelectElementModel( const CArrayFix<TBrCtlSelectOptionData>& aOptions );
    
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * It leaves a pointer on the cleanup stack.
        * @param aElement The select element.
        */
        static CBrowserSelectElementModel* NewLC( const CArrayFix<TBrCtlSelectOptionData>& aOptions );

        /**
        * Two-phased constructor.
        * @param aElement The select element.
        */
        static CBrowserSelectElementModel* NewL( const CArrayFix<TBrCtlSelectOptionData>& aOptions );

        /**
        * Destructor.
        */
        ~CBrowserSelectElementModel();


        /**
        * Get matchable text array (for filtering).
        * @return The matchable text array.
        */
        const MDesCArray* MatchableTextArray() const;

        
    public: // new function

        void SetItemArray();

    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

    private:    // New functions

        /**
        * Adds a prefix to the given string needed by the listbox.
        * @param aText Zeroterminated string without prefix.
        * @param aIsOptionGroup Optiongroups need different prefix.
        * @return Zeroterminated string with prefix.
        */
        TText* AddPrefixLC( TText* aText, TBool aIsOptionGroup ) const;

        /**
        * Stores text depending on the node's attributes.
        * @param aNode The node.
        */
        void FormatTextL( const TBrCtlSelectOptionData& optionData );

    private:    // Data
        
        TBrCtlSelectElementFilterTextArray* iFilterTexts;
        TBrCtlSelectElementItemTextArray*   iItemTexts;

        const CArrayFix<TBrCtlSelectOptionData>& iOptions;  // not owned
    };

#endif            
// End of File
