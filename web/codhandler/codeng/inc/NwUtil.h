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
*      Wrapper functions and classes for cXmlLibrary (NW_...) functionality.
*      
*
*/


#ifndef NW_UTIL_H
#define NW_UTIL_H

// INCLUDES

#include <e32base.h>
#include <xml/cxml/nwx_status.h>
#include <xml/cxml/nw_wbxml_dictionary.h>
#include <xml/cxml/nw_encoder_stringtable.h>
#include <xml/cxml/nw_string_string.h>

// FUNCTION DECLARATION

/**
* Convert cXML status code to Symbian OS error code.
* @param aNwError cXML status.
* @return Symbian OS error code.
*/
TInt NwStatus2Error( NW_Status_t aNwStatus );

/**
* Leave if cXML status is error status.
* @param aNwError cXML status.
* @return aNwStatus.
*/
inline TInt LeaveIfNwErrorL( NW_Status_t aNwStatus );

/**
* Push a cleanup operation for an NW_Ucs2* buffer on the cleanup stack.
* @param aUcs2 The buffer to be cleaned up.
*/
void CleanupNwUcs2PushL( NW_Ucs2* aUcs2 );

/**
* Push a cleanup operation for a NW_String_t storage. Cleanup operation
* calls NW_String_deleteStorage(), but does not delete aString.
*/
void CleanupNwStringStoragePushL( NW_String_t* aString );

// CLASS DECLARATION

/**
* Encapsulate WBXML dictionary initialization.
*/
NONSHARABLE_CLASS( RNwWbxmlDictionary )
    {
    public:     // New methods

        /**
        * Initialize dictionary.
        * @param aCount Dictionary array count.
        * @param aDictArray Dictionary array.
        * @return Error code.
        */
        TInt Initialize
            ( NW_Int32 aCount, NW_WBXML_Dictionary_t* aDictArray[] );

        /**
        * Destroy dictionary.
        */
        void Close();
    };

/**
* Encapsulate a WBXML document node.
*/
NONSHARABLE_CLASS( CNwDomDocumentNode ): public CBase
    {
    public:     // Constructors and destructor

        /**
        * Constructor.
        */      
        inline CNwDomDocumentNode();

        /**
        * Destructor.
        */      
        virtual ~CNwDomDocumentNode();

    public:     // Data

        NW_DOM_DocumentNode_t* iDocNode;    ///< The document node. Owned.
    };

#include "NwUtil.inl"

#endif /* def NW_UTIL_H */
