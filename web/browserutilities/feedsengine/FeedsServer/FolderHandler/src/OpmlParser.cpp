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
* Description:  Opml parser.
*
*/


#include "OpmlParser.h"
#include "CleanupLibXml2.h"
#include "LeakTracker.h"
#include "PackedFolder.h"
#include "XmlUtils.h"


// Element and attribute names used by this parser.
_LIT8(KBody, "body");
_LIT8(KHead, "head");
_LIT8(KTitle, "title");
_LIT8(KIsComment, "isComment");
_LIT8(KOml, "oml");
_LIT8(KOpml, "opml");
_LIT8(KOutlineDocument, "outlineDocument");
_LIT8(KOutline, "outline");
_LIT8(KText, "text");
_LIT8(KUrl, "url");
_LIT8(KXmlUrl, "xmlUrl");

_LIT(KRootFolder, "Root Folder");

_LIT(KComma, " , ");
_LIT(KBackQuote, " ` ");



const TInt KMaxURLLength = 256; 

// -----------------------------------------------------------------------------
// RssFeedParser::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COpmlParser* COpmlParser::NewL(CXmlUtils& aXmlUtils)
    {
    COpmlParser* self = new (ELeave) COpmlParser(aXmlUtils);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// COpmlParser::COpmlParser
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COpmlParser::COpmlParser(CXmlUtils& aXmlUtils):
        iLeakTracker(CLeakTracker::EOpmlParser), iXmlUtils(aXmlUtils)
    {
    }
        

// -----------------------------------------------------------------------------
// COpmlParser::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COpmlParser::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// COpmlParser::~COpmlParser
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
COpmlParser::~COpmlParser()
    {
    }

        
// -----------------------------------------------------------------------------
// COpmlParser::IsSupported
//
// Returns true if this parser can process the given document. 
// -----------------------------------------------------------------------------
//
TBool COpmlParser::IsSupported(RXmlEngDocument aDocument, const TDesC& /*aContentType*/) const
    {
    TXmlEngElement        node = NULL;
    
    // Get the root element.
    node = iXmlUtils.GetDocumentFirstElement(aDocument);

    // If the root node is missing the feed isn't supported.    
    if (node.IsNull())
        {
        return EFalse;
        }

    // If the root node is not HEAD_STR the feed isn't supported.
    else if (!iXmlUtils.IsNamed(node, KOpml) &&
            !iXmlUtils.IsNamed(node, KOml) &&
            !iXmlUtils.IsNamed(node, KOutlineDocument))
        {
        return EFalse;
        }

    return ETrue;
    }


// -----------------------------------------------------------------------------
// COpmlParser::ParseL
//
// Creates a FolderItem instance from the given document.
// -----------------------------------------------------------------------------
//
CPackedFolder* COpmlParser::ParseL(TDesC8& aBuffer, const TDesC& aContentType, 
        const TDesC& aCharSet, const TDesC& aImportRootFolder) const
    {
    RXmlEngDocument       document;
    TXmlEngElement        rootNode;
    TXmlEngElement        bodyNode;
    CPackedFolder*  packedFolder = NULL;

    // Parse the buffer.
    document = iXmlUtils.ParseBufferL(aBuffer, aCharSet);
    CleanupClosePushL(document);

    if (!IsSupported(document, aContentType))
        {
        User::Leave(KErrNotSupported);
        }

    // Get the root node.
    rootNode = iXmlUtils.GetDocumentFirstElement(document);
    
    // Find the body element.
    bodyNode = iXmlUtils.GetFirstNamedChild(rootNode, KBody());
    
    if (bodyNode.IsNull())
        {
        User::Leave(KErrCorrupt);
        }
        
    // Process the body.
    packedFolder = CPackedFolder::NewL();
    CleanupStack::PushL(packedFolder);
    
	TBuf<KMaxFileName> OPMLRootFolderName;
    
    if(aImportRootFolder.Length()<=0)
		{
    	// This will happen only while importing default OPML file - Do nothing
		}
	else
		{
		// 	Query for the opml/head/title

	    // Find the head/title element.
	    TXmlEngElement headNode;
	    HBufC* Title = NULL;
		headNode = iXmlUtils.GetFirstNamedChild(rootNode, KHead());

	    if(!headNode.IsNull())
    		{
    		TXmlEngElement titleNode;
			titleNode = iXmlUtils.GetFirstNamedChild(headNode, KTitle());

		    if(!titleNode.IsNull())
	   			Title = iXmlUtils.ExtractTextL(titleNode);
    		}

    	CleanupStack::PushL(Title);

    	if(Title == NULL)
    		OPMLRootFolderName.Append(aImportRootFolder);
    	else
    		OPMLRootFolderName.Append(Title->Des());

		CleanupStack::PopAndDestroy(/*Title*/);
		}

	packedFolder->FolderBeginsL(KRootFolder);

	if(OPMLRootFolderName.Length() > 0)
		packedFolder->FolderBeginsL(OPMLRootFolderName);
    
	ProcessFolderL(bodyNode, *packedFolder);

	if(OPMLRootFolderName.Length() > 0)
		packedFolder->FolderEndsL();
    
	packedFolder->FolderEndsL();
	packedFolder->Trim();
	packedFolder->DoneL();    

	CleanupStack::Pop(packedFolder);
	CleanupStack::PopAndDestroy(/*document*/);    

	return packedFolder;
	}


// -----------------------------------------------------------------------------
// COpmlParser::ProcessFolderL
//
// Populates the FolderItem instance with the values from the given node. 
//
// TODO: Factor out the recursion.
// -----------------------------------------------------------------------------
//
void COpmlParser::ProcessFolderL(TXmlEngElement aNode, CPackedFolder& aPackedFolder) const
    {
    _LIT(KTrue,  "true");
    const TTime  KTimeZero(0);

    TXmlEngElement  node;
    TBool     isFolder;
    HBufC*    itemName = NULL;
    HBufC*    itemUrl = NULL;
    HBufC*    isComment = NULL;
    RPointerArray<HBufC> titleArray;
    const TInt KAutoUpdatingOff = 0;
    
    HBufC*	  FeedUrlsInCurrentFolder = NULL;
    
    // Process the body's children.
    node = iXmlUtils.GetFirstElementChild(aNode);
    
    while (node.NotNull())
        {
        isFolder = EFalse;

        // Process an outline element.
        if (iXmlUtils.IsNamed(node, KOutline))
            {
            // Skip the element and any children if its a comment.
            if ((isComment = iXmlUtils.AttributeL(node, KIsComment)) != NULL)
                {
                if (isComment->CompareF(KTrue()) == 0)
                    {
                    delete isComment;
                    
                    // Skip the element.
                    node = iXmlUtils.GetNextSiblingElement(node);
                    continue;
                    }
                
                delete isComment;
                }
                
            // If the node has a outline child its a folder.
            if (iXmlUtils.GetFirstNamedChild(node, KOutline).NotNull())
                {
                isFolder = ETrue;
                }
            
            // Extract the name.
            itemName = iXmlUtils.AttributeL(node, KText);
            if (itemName == NULL)
                {
                itemName = KNullDesC().AllocL();
                }
			itemName->Des().Trim();
            CleanupStack::PushL(itemName);
            
            // If the node isn't a folder then add a feed item
            if (!isFolder)
                {
                TBool found = EFalse;
                titleArray.AppendL(itemName);
                CleanupStack::Pop(itemName);
                // Extract the url.
                itemUrl = iXmlUtils.AttributeL(node, KUrl);
                if (itemUrl == NULL)
                    {
                    itemUrl = iXmlUtils.AttributeL(node, KXmlUrl);
                    if (itemUrl == NULL)
                    {
                        itemUrl = KNullDesC().AllocL();
                    }
                    }
            
                CleanupStack::PushL(itemUrl);

                // Clean up the url.
                TPtr ptr(itemUrl->Des());
                iXmlUtils.CleanupUrlL(ptr);
                
                itemUrl->Des().Trim();

                for(TInt i = 0;i<titleArray.Count()-1;i++)
                	{
                	if(itemName->Des().Compare(titleArray[i]->Des())==0)
                		found = ETrue;
                	}
                
                HBufC* titleAndUrl = HBufC::NewL(itemUrl->Length()+itemName->Length()+KBackQuote().Length());
                CleanupStack::PushL(titleAndUrl);
                titleAndUrl->Des().Copy(itemName->Des());
                titleAndUrl->Des().Append(KBackQuote());
                titleAndUrl->Des().Append(itemUrl->Des());
                if(found && itemUrl->Length() <= KMaxURLLength)
                {
                	found = EFalse;
                	SearchURLInCurrentFolderL(FeedUrlsInCurrentFolder,titleAndUrl,found);
                	if(!found)
                		{
                		aPackedFolder.AddFeedL(*itemName, *itemUrl, KTimeZero, KAutoUpdatingOff, KErrNotFound);	
                		}
                }
                else if(itemUrl->Length() <= KMaxURLLength)
                {
                	aPackedFolder.AddFeedL(*itemName, *itemUrl, KTimeZero, KAutoUpdatingOff, KErrNotFound);	
                }
                if(!FeedUrlsInCurrentFolder)
                	{
                	FeedUrlsInCurrentFolder = HBufC::NewL(itemUrl->Length()+itemName->Length()+KBackQuote().Length());
                	/*
                		Cleanupstack manipulation, since we are popping itemUrl & itemName before FeedUrlsInCurrentFolder
                	*/
                	CleanupStack::Pop(2);
					CleanupStack::PushL(FeedUrlsInCurrentFolder);
	                CleanupStack::PushL(itemUrl);
    	            CleanupStack::PushL(titleAndUrl);
		        	}
                else
					{
                	/*
                		Cleanupstack manipulation, since we are popping itemUrl & itemName before FeedUrlsInCurrentFolder
                	*/

	               	HBufC* temp = FeedUrlsInCurrentFolder->ReAllocL(FeedUrlsInCurrentFolder->Length() + itemUrl->Length()+itemName->Length()+KBackQuote().Length() + KComma().Length());
	               	CleanupStack::Pop(3);
	               	FeedUrlsInCurrentFolder = temp;
					CleanupStack::PushL(FeedUrlsInCurrentFolder);
	                CleanupStack::PushL(itemUrl);
    	            CleanupStack::PushL(titleAndUrl);
                	FeedUrlsInCurrentFolder->Des().Append(KComma());
	               	}
                FeedUrlsInCurrentFolder->Des().Append(titleAndUrl->Des());
                CleanupStack::PopAndDestroy(titleAndUrl);
                CleanupStack::PopAndDestroy(itemUrl);

                }
            
            // Otherwise add a folder item.
            else
                {
                aPackedFolder.FolderBeginsL(*itemName);
                CleanupStack::PopAndDestroy(itemName);
                ProcessFolderL(node, aPackedFolder);
                aPackedFolder.FolderEndsL();                
                }                        
            }
            
        // Get the next element.
        node = iXmlUtils.GetNextSiblingElement(node);
        }
        
    if(FeedUrlsInCurrentFolder)
        {
    	CleanupStack::PopAndDestroy(FeedUrlsInCurrentFolder);	
        }
    titleArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// COpmlParser::SearchURLInCurrentFolderL
//
// Search for the feed URL from a set of feed URLs present in the current folder seperated by commas
//
// -----------------------------------------------------------------------------
//
void COpmlParser::SearchURLInCurrentFolderL(HBufC* aURLsInCurFolder, HBufC* aFeedURL, TBool& aFound) const
	{
	
	aFound = EFalse;

	if(!aURLsInCurFolder)
		{
		aFound = EFalse;
		return;
		}

	HBufC *temp = HBufC::NewLC(aURLsInCurFolder->Length());
	temp->Des().Copy(aURLsInCurFolder->Des());
	
	TInt curPos = 0;
	TInt commaPos = 0;
	while(ETrue)
		{
		commaPos = temp->Find(KComma());
		if(commaPos == KErrNotFound)
			{
			aFound = !temp->Compare(aFeedURL->Des());
			break;
			}
		TPtrC curURL = temp->Mid(curPos, commaPos);
		if(curURL == aFeedURL->Des())
			{
			aFound = ETrue;
			break;
			}
			
		temp->Des().Copy(temp->Mid(commaPos+KComma().Length()));
		}
	CleanupStack::PopAndDestroy(/*temp*/);	
	}
