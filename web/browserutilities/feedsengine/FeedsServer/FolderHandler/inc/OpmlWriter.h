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
* Description:  Translates CFolderItems back to opml.
*
*/


#ifndef OPML_WRITER_H
#define OPML_WRITER_H


// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <libxml2_parser.h>
#include <libxml2_encoding.h>
#include "libxml2_xmlwriter.h"

#include "FeedsDatabase.h"
#include "CleanupLibXml2.h"
#include "XmlUtils.h"



// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Translates CFolderItems back to opml.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class OpmlWriter
    {
    public:  // New Methods
        /**
        * Writes the Folder Items (as opml) to the given file -- using utf-8 
        * for the char-encoding.
        *
        * @since 3.2
        * @param aFolderItemIDs The array of aFolderItemIDs
        * @param aExportFileName The file the folder-item will be written to.  aFile 
        *              must be opened prior to making this call.
        * @return void
        */
		static void ExportL(RArray<TInt> &aFolderItemIDs, const TDesC& aExportFileName);
		
    private:
        /**
        * Writes the FolderItem and all of its child to the given file.
        *
        * @since 3.0
        * @param aWriter The xml writer into which the folder is written into.
        * @param aFolderItemId The folder item id which has to be written.
        * @param aFeedsDatabase The Feeds Database where all the feelds/folders are stored.
        * @return void
        */
		static void WriteFolderItemL(xmlTextWriterPtr aWriter, TInt aFolderItemId, CFeedsDatabase &aFeedsDatabase);
		
        /**
        * Writes out a attribute.
        *
        * @since 3.0
        * @param aWriter The writer the folder is written into.
        * @param aName The name of the attribute.
        * @param aValue The value of the attribute.
        * @return void
        */
        static void WriteAttributeL(xmlTextWriterPtr aWriter, const TDesC8& aName, 
                const TDesC& aValue);
    };
    
#endif      // OPML_WRITER_H
            
// End of File
