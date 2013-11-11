/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of RUnicodeFile.
*      
*
*/


#ifndef UNICODE_FILE_H
#define UNICODE_FILE_H

// INCLUDE FILES

#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION

/**
* Read characters from a Unicode text file.
*/
class RUnicodeFile
    {
    public:     // New methods

        /**
        * Open Unicode file for input (read). Write access not supported.
        * @param aFs File Server Session handle.
        * @param aName File name.
        * @param aFileMode File opening mode.
        * @return Error code.
        */
        TInt Open( RFs& aFs, const TDesC& aName, TUint aFileMode );

        /**
        * Create/replace Unicode file for output (write). Existing content of
        * file is lost.
        * Read access not supported.
        * @param aFs File Server Session handle.
        * @param aName File name.
        * @param aFileMode File opening mode.
        * @return Error code.
        */
        TInt ReplaceL( RFs& aFs, const TDesC& aName, TUint aFileMode );

        /**
        * Close file.
        */
        void Close();

        /**
        * Get a character.
        * @return Character got.
        */
        TUint GetCharL();

        /**
        * Write Unicode descriptor to file.
        * @param aDes Descriptor to write.
        * @return Error code.
        */
        TInt Write( const TDesC16& aDes );

    private:    // data

        RFile iFile;    ///< File handle.
    };

#endif
            
// End of File
