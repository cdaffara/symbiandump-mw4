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
* Description:  Declaration of class CCodConfirm.   
*
*/


#ifndef COD_CONFIRM_H
#define COD_CONFIRM_H

// INCLUDES

#include <coecntrl.h>

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* Control displaying COD details.
*/
NONSHARABLE_CLASS( CCodConfirm ): public CCoeControl
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        * @param aMopParent MOP Parent.
        */
        CCodConfirm();

        
        /**
        * Destructor.
        */
        virtual ~CCodConfirm();

    public:     // new methods
                                        
        /**
        * Show confiration 
        * @param aDownload The actual download
        * @param aCbaResource Softkeys resource ID (eg. R_AVKON_SOFTKEYS_OK_CANCEL)
        * @return Confirmation accepted? True/False.
        */
        
         TBool ShowDownloadConfirmationL(TInt aCbaResource, 
                                         const CCodData& aData, 
                                         TBool aCode); 
         								
        /**
        * Format list box string for Download confirmation.
        * @param aFirst First line
        * @param aSecond Second line of list item
        * @return The allocated heap buffer.
        */
         HBufC* FormatListBoxItemLC( const TDesC& aFirst, 
                                     const TDesC& aSecond );



    public:     // from CCoeControl


    private:    // new methods

        /**
        * Allocate and return human-readable name for a MIME type.
        * @param MIME type.
        * @return Allocated human-readable name in a buffer. Ownership passed
        * to the caller.
        */
        HBufC* AllocTypeNameL( const TDesC8& aMimeType );



        /**
        * Get MIME type to be displayed. (First type which is 'meaningless'
        * to the user, e.g. leave out wrappers like DRM).
        * @param aData COD data.
        * @return MIME type.
        */
        TPtrC8 DisplayType( const CCodData& aData );

    private:    // data


    };

#endif /* def COD_CONFIRM_H */
