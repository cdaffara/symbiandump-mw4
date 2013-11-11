/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Header declaration
*
*/






#ifndef SENATOMENTRY_H_
#define SENATOMENTRY_H_


/**
*     Provides object and methods to manipulate <entry> tag from atom-pub API.
* 
*   @lib SenMessages.DLL
*   @since 5.0
*/


//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>

#include "senatommessage.h"



// CLASS DECLARATION
class CSenFragmentBase;

class CSenAtomEntry : public CSenAtomMessage
    {
    public:
        // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenAtomEntry* NewL();

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenAtomEntry* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CSenAtomEntry();

        
        /*
        *        Callback funtions. Not a part of API.
        */
        IMPORT_C virtual void OnStartElementL(const RTagInfo& aElement,
                                              const RAttributeArray& aAttributes,
                                              TInt aErrorCode);                                            
        IMPORT_C virtual void OnEndElementL(const RTagInfo& aElement,
                                            TInt aErrorCode);
        IMPORT_C virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
        
    protected:
    
    // Internal functions
        IMPORT_C CSenAtomEntry();
        IMPORT_C void BaseConstructL();

    protected: // Data
        
        TInt localState;
        RBuf8 iBuffer;

        
    private:
    RAttributeArray* CreateLinkAttribs( const TDesC8& href, const TDesC8& rel, const TDesC8& type, const TDesC8& hrefLang, const TDesC8& title, const TDesC8& lenght);
    };

#endif /*SENATOMENTRY_H*/

// End of File
