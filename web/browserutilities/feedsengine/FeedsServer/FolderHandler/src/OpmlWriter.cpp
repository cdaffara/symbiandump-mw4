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
* Description:  Methods used to translated CFolderItems back into opml files.
*
*/


#include "OpmlWriter.h"

#include <escapeutils.h>

_LIT(KTempDirName, "c:\\system\\temp\\");

// Private conts.
_LIT8(KUtf8, "utf8");
_LIT8(KOpml, "opml");
_LIT8(KVersion, "version");
_LIT8(K1, "1.0");

_LIT8(KHead, "head");
_LIT8(KTitle, "title");
_LIT8(KTitleValue, "S60 Feeds Client - Feed Subscriptions");

_LIT8(KBody, "body");
_LIT8(KOutline, "outline");
_LIT8(KText, "text");
_LIT8(KXmlUrl, "xmlUrl");
_LIT8(KType, "type");
_LIT16(KTypeValue, "rss");

const TInt KMaxFolderItemTitleLength = 256;
const TInt KMaxFolderItemURLLength = 256;


// -----------------------------------------------------------------------------
// OpmlWriter::WriteFolderItemL
//
// Writes the FolderItem and all of its child to the given file.
// -----------------------------------------------------------------------------
//
void OpmlWriter::WriteFolderItemL(xmlTextWriterPtr aWriter, TInt aFolderItemId, CFeedsDatabase &aFeedsDatabase)
    {
    TInt	rc;
	TBool	isFolder;
	HBufC*	FolderItemTitle =  HBufC::NewLC(KMaxFolderItemTitleLength);
	HBufC*	FolderItemURL =  HBufC::NewLC(KMaxFolderItemURLLength);

	aFeedsDatabase.FolderItemInfoL(aFolderItemId, isFolder, FolderItemTitle, FolderItemURL);

      // Write out a single entry.
	if(!isFolder)
        {
        // Open the opml element.
		rc = xmlTextWriterStartElement(aWriter, BAD_CAST(KOutline().Ptr()));
        if (rc < 0) 
            {
            User::Leave(KErrCorrupt);
            }

        // Write out the text attribute.
        if (FolderItemTitle->Length() != 0)
            {
            WriteAttributeL(aWriter, KText(), *FolderItemTitle);
            }

        // Write out the XmlUrl attribute.
        if (FolderItemURL->Length() != 0)
            {
            WriteAttributeL(aWriter, KXmlUrl(), *FolderItemURL);
            }

		WriteAttributeL(aWriter, KType(), KTypeValue());

        // Close the outline element.
        rc = xmlTextWriterEndElement(aWriter);
        if (rc < 0) 
            {
            User::Leave(KErrCorrupt);
            }
        }
    // Otherwise write it and its children.
    else
		{

		rc = xmlTextWriterStartElement(aWriter, BAD_CAST(KOutline().Ptr()));
        if (rc < 0) 
            {
            User::Leave(KErrCorrupt);
            }
    
        // Write out the text attribute.
        if (FolderItemTitle->Length() != 0)
            {
            WriteAttributeL(aWriter, KText(), *FolderItemTitle);
            }

		RArray<TInt> Children;
		CleanupClosePushL(Children);
		
		aFeedsDatabase.FolderItemGetChildrenL(aFolderItemId, Children);

        // Write out each child.
		for (TInt i = 0; i < Children.Count(); i++)
            {
            WriteFolderItemL(aWriter, Children[i], aFeedsDatabase);
            }

		CleanupStack::PopAndDestroy(/*Children*/);

		rc = xmlTextWriterEndElement(aWriter);
		if (rc < 0) 
			{
			User::Leave(KErrCorrupt);
			}
        }
        
        CleanupStack::PopAndDestroy(/*FolderItemURL*/);
        CleanupStack::PopAndDestroy(/*FolderItemTitle*/);
    }


// -----------------------------------------------------------------------------
// OpmlWriter::WriteAttributeL
//
// Writes out a attribute.
// -----------------------------------------------------------------------------
//
void OpmlWriter::WriteAttributeL(xmlTextWriterPtr aWriter, const TDesC8& aName, 
        const TDesC& aValue)
    {
    TInt      rc;
    HBufC8*  utf8 = NULL;
    HBufC8*  utf8Zero = NULL;

    // Convert it to utf8.
    utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aValue);
    CleanupStack::PushL(utf8);
    utf8Zero = HBufC8::NewL(utf8->Length() + 1);
    utf8Zero->Des().Copy(*utf8);
    utf8Zero->Des().ZeroTerminate();
    CleanupStack::PushL(utf8Zero);

    // Add the attribute.
    rc = xmlTextWriterWriteAttribute(aWriter, BAD_CAST(aName.Ptr()), utf8Zero->Ptr());
    
    CleanupStack::PopAndDestroy(utf8Zero);
    CleanupStack::PopAndDestroy(utf8);
    if (rc < 0) 
        {
        User::Leave(KErrCorrupt);
        }
    }


// -----------------------------------------------------------------------------
// OpmlWriter::ExportL
//
// Export all specified folders (by FolderItemIDs) to an OPML file
// -----------------------------------------------------------------------------
//
void OpmlWriter::ExportL(RArray<TInt> &aFolderItemIDs, const TDesC& aExportFileName)
	{

    RFs                 rfs;
    RFile               OPMLFile;
	TInt rc;
	xmlTextWriterPtr writer = NULL;
	xmlBufferPtr buf = NULL;

	if(aFolderItemIDs.Count()<=0 || aExportFileName.Length()<=0)
		{
		User::Leave(KErrCorrupt);
		}

	// Create a new XML buffer, to which the XML document will be
	// written.
	buf = xmlBufferCreate();
    User::LeaveIfNull(buf);
    CleanupLibXml2::PushL(buf);

    // Create a new XmlWriter for memory.
    writer = xmlNewTextWriterMemory(buf, 0);
    User::LeaveIfNull(writer);
    CleanupLibXml2::PushL(writer);

    xmlTextWriterSetIndent(writer, 4);

    // Start the document.
	rc = xmlTextWriterStartDocument(writer, NULL, (const char*) KUtf8().Ptr(), NULL);
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}

	// Open the opml element with its version.
	rc = xmlTextWriterStartElement(writer, BAD_CAST(KOpml().Ptr()));
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}

	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST(KVersion().Ptr()), BAD_CAST(K1().Ptr()));
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}

	rc = xmlTextWriterStartElement(writer, BAD_CAST(KHead().Ptr()));
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST(KTitle().Ptr()));
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}
	
	rc = xmlTextWriterWriteFormatRaw(writer, "%s", KTitleValue().Ptr());
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}
		
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) 
        {
        User::Leave(KErrCorrupt);
        }


    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) 
        {
        User::Leave(KErrCorrupt);
        }

	// Open the body element.
	rc = xmlTextWriterStartElement(writer, BAD_CAST(KBody().Ptr()));
	if (rc < 0) 
		{
		User::Leave(KErrCorrupt);
		}

	TBool created;
	CFeedsDatabase *FeedsDatabase = CFeedsDatabase::NewL(NULL,created);
	CleanupStack::PushL(FeedsDatabase);
	
    // Write out the folder.
    for(TInt i=0; i<aFolderItemIDs.Count(); i++)
		{
 		WriteFolderItemL(writer, aFolderItemIDs[i], *FeedsDatabase);	
		}
    
    CleanupStack::PopAndDestroy(/*FeedsDatabase*/);
    // Close the body element.
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) 
        {
        User::Leave(KErrCorrupt);
        }

    // Close the opml element.
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) 
        {
        User::Leave(KErrCorrupt);
        }

    (void) xmlTextWriterFlush(writer);
    
    // Write the buffer to the file.
    TPtrC8 ptr(buf->content, buf->use);
    
	TBuf<KMaxFileName>  path(KTempDirName);
	path.Append(aExportFileName.Mid(0, Min(aExportFileName.Length(), (KMaxFileName - path.Length()))));
	
	User::LeaveIfError(rfs.Connect());
	CleanupClosePushL(rfs);

	User::LeaveIfError(OPMLFile.Replace(rfs, path, EFileShareExclusive|EFileWrite));

	CleanupClosePushL(OPMLFile);

    OPMLFile.Write(ptr);

	// Clean up.    
	CleanupStack::PopAndDestroy(/*OPMLFile*/);
	CleanupStack::PopAndDestroy(/*rfs*/);

	CleanupStack::PopAndDestroy(/*writer*/);
	CleanupStack::PopAndDestroy(/*buf*/);

	}
