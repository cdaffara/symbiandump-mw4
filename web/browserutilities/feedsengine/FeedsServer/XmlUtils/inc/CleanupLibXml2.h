/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A util class to cleanup libxml2 related instances.
*
*/


#ifndef CLEANUP_LIBXML2_H
#define CLEANUP_LIBXML2_H


// INCLUDES
#include <e32std.h>
#include <libxml2_parser.h>
#include "libxml2_xmlwriter.h"
#include <xmlengxestrings.h>
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  A util class to cleanup libxml2 related instances.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CleanupLibXml2
    {
    public:
        /**
        * Pushes a Libxml2 doc-ptr on the cleanup stack.
        *
        * @since 3.0
        * @param aDocPtr A libxml2 document ptr.
        * @return void.
        */
        static void PushL(xmlDocPtr aDocPtr);

        /**
        * Pushes a Libxml2 string on the cleanup stack.
        *
        * @since 3.0
        * @param aXmlString A libxml2 string.
        * @return void.
        */
        static void PushL(xmlChar* aXmlString);

        /**
        * Pushes a Libxml2 buffer-ptr on the cleanup stack.
        *
        * @since 3.0
        * @param aXmlString A libxml2 buffer-ptr.
        * @return void.
        */
        static void PushL(xmlBufferPtr aBufferPtr);

        /**
        * Pushes a Libxml2 text-writer-ptr on the cleanup stack.
        *
        * @since 3.0
        * @param aTextWriterPtr A libxml2 text-writer-ptr.
        * @return void.
        */
        static void PushL(xmlTextWriterPtr aTextWriterPtr);


    private:
        /**
        * Cleanup stack callback method to delete a Libxml2 doc-ptr.
        *
        * @since 3.0
        * @param aPtr A libxml2 document ptr.
        * @return void.
        */
        static void CleanupXmlDocPtr(TAny *aPtr);

        /**
        * Cleanup stack callback method to delete a Libxml2 string.
        *
        * @since 3.0
        * @param aPtr A libxml2 string.
        * @return void.
        */
        static void CleanupXmlString(TAny *aPtr);

        /**
        * Cleanup stack callback method to delete a Libxml2 buffer-ptr.
        *
        * @since 3.0
        * @param aPtr A libxml2 buffer-ptr.
        * @return void.
        */
        static void CleanupXmlBufferPtr(TAny *aPtr);

        /**
        * Cleanup stack callback method to delete a Libxml2 text-writer-ptr.
        *
        * @since 3.0
        * @param aPtr A libxml2 text-writer-ptr.
        * @return void.
        */
        static void CleanupXmlTextWriterPtr(TAny *aPtr);
    };

#endif      // CLEANUP_LIBXML2_H
            
// End of File
