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
* Description:  Is a serialized version of CFolderItem suitable for transfer between
*                client and server.
*
*/

#include "PackedAttributes.h"
#include "FeedAttributes.h"
#include "FolderAttributes.h"
#include "PackedFolder.h"
#include "Logger.h"


const TInt KTokenArrayIncrementSize = 100;
const TInt KStringTableIncrementSize = 1024;


// -----------------------------------------------------------------------------
// CPackedFolder::NewL
//
// Two-phased constructor.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFolder* CPackedFolder::NewL()
    {
    CPackedFolder* self = new (ELeave) CPackedFolder(KTokenArrayIncrementSize,
            KStringTableIncrementSize);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CPackedFolder::CPackedFolder
//
// C++ default constructor can NOT contain any code, that
// might leave.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFolder::CPackedFolder(TInt aTokenArrayInc, TInt aStringTableInc):
        CPacked(aTokenArrayInc, aStringTableInc), iLeakTracker(CLeakTracker::EPackedFolder)
    {
    }
        

// -----------------------------------------------------------------------------
// CPackedFolder::ConstructL
//
// Symbian 2nd phase constructor can leave.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::ConstructL()
    {
    BaseConstructL();
    }
        

// -----------------------------------------------------------------------------
// CPackedFolder::~CPackedFolder
//
// Deconstructor.  Used on both the client and server.
// -----------------------------------------------------------------------------
//
CPackedFolder::~CPackedFolder()
    {
    }

        
// -----------------------------------------------------------------------------
// CPackedFolder::FolderBeginsL
//
// Insert the begining of a folder.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::FolderBeginsL(const TDesC& aTitle, TInt aEntryId, TInt aStatus)
    {
    TBuf16<25>  buf;

    InsertTokenL(EFolderTokenFolderBegin);
    AddAttributeL(EFolderAttributeTitle, aTitle);

    buf.AppendNum(aEntryId);
    AddAttributeL(EFolderAttributeEntryId, buf);
    
    buf.Zero();
    buf.AppendNum(aStatus);
    AddAttributeL(EFolderAttributeStatus, buf);
    
    InsertTokenL(EFolderAttributeEnd);    
    }


// -----------------------------------------------------------------------------
// CPackedFolder::FolderEndsL
//
// Insert the ending of a folder.    Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::FolderEndsL()
    {
    InsertTokenL(EFolderTokenFolderEnd);
    }


// -----------------------------------------------------------------------------
// CPackedFolder::AddFeedL
//
// Insert a feed.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::AddFeedL( const TDesC& aTitle, const TDesC& aFeedUrl, 
        const TTime& aLastUpdate, TInt aFreq, TInt aStatus,TInt aUnreadCount, TInt aEntryId, TInt aFeedId)
    {
    TBuf16<25>  buf;
    
    InsertTokenL(EFolderTokenFeed);
    AddAttributeL(EFolderAttributeTitle, aTitle);
    AddAttributeL(EFolderAttributeUrl, aFeedUrl);
    
    buf.Zero();
    buf.Format(_L("%Ld"), aLastUpdate.Int64());
    AddAttributeL(EFolderAttributeTimestamp, buf);

    buf.Zero();
    buf.AppendNum(aUnreadCount);
    AddAttributeL(EFolderAttributeUnreadCount, buf);

    buf.Zero();
    buf.AppendNum(aEntryId);
    AddAttributeL(EFolderAttributeEntryId, buf);

    buf.Zero();
    buf.AppendNum(aFeedId);
    AddAttributeL(EFolderAttributeFeedId, buf);

    buf.Zero();
    buf.AppendNum(aStatus);
    AddAttributeL(EFolderAttributeStatus, buf);

    buf.Zero();
    buf.AppendNum(aFreq);
    AddAttributeL(EFolderAttributeFreq, buf);
    
    InsertTokenL(EFolderAttributeEnd);    
    }


// -----------------------------------------------------------------------------
// CPackedFolder::ItemBeginsL
//
// Insert the begining of a item.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::ItemBeginsL()
    {
    InsertTokenL(EFolderTokenItemBegin);
    }


// -----------------------------------------------------------------------------
// CPackedFolder::ItemEndsL
//
// Insert the ending of a item.   Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::ItemEndsL()
    {
    InsertTokenL(EFolderTokenItemEnd);
    }

    
// -----------------------------------------------------------------------------
// CPackedFolder::DoneL
//
// Called when when packing is done.  Used only on the server.
// -----------------------------------------------------------------------------
//
void CPackedFolder::DoneL()
    {
    iTokenArrayAsDes.Set((const TUint8*) iTokenArray, iTokenArrayLength * sizeof(TUint));
    }


// -----------------------------------------------------------------------------
// CPackedFolder::ExtractAttributes
//
// Extract the folder item's attributes
// -----------------------------------------------------------------------------
//
void CPackedFolder::ExtractAttributes( TPtrC& aTitle, TPtrC& aUrl, 
        TInt& aEntryId, TInt& aFeedId, TTime& aTimestamp, TInt& aUnreadCount, TInt &aStatus, TInt& aFreq) const
    {   
    TBool  done = EFalse;
    TUint  attributeToken;
    TPtrC  attributeValue;
     
    // Set the default values.
    aTitle.Set(KNullDesC);
    aUrl.Set(KNullDesC);
    aEntryId = -1;
    aFeedId = 0;
    aTimestamp.UniversalTime();
    
    // Extract the attribute tokens.
    while (!done)
        {
        TUint  token;
            
        token = NextToken();
            
        // Extract the next attribute.
        if (token != EFolderAttributeEnd)
            {            
            // Get the attribute.
            ExtractAttributeValue(attributeToken, attributeValue);

            switch (attributeToken)
                {
                case EFolderAttributeTitle:
                    aTitle.Set(attributeValue);
                    break;

                case EFolderAttributeUrl:
                    aUrl.Set(attributeValue);
                    break;

                case EFolderAttributeEntryId:
                    {                        
                    TLex16  lex(attributeValue);
                      
                    lex.Val(aEntryId);
                    }
                    break;

                case EFolderAttributeFeedId:
                    {                        
                    TLex16  lex(attributeValue);
                      
                    lex.Val(aFeedId);
                    }
                    break;

                case EFolderAttributeTimestamp:
                    {                    
                    TLex16  lex(attributeValue);
                    TInt64  ts;
                      
                    lex.Val(ts);
                    aTimestamp = ts;
                    }
                    break;

                case EFolderAttributeUnreadCount:
                    {                    
                    TLex16  lex(attributeValue);
                    TInt  unreadCount;
                      
                    lex.Val(unreadCount);
                    aUnreadCount = unreadCount;
                    }
                    break;
                case EFolderAttributeStatus:
                    {                        
                    TLex16  lex(attributeValue);
                      
                    lex.Val(aStatus);
                    }
                    break;                    
                case EFolderAttributeFreq:
                    {
                    TLex16  lex(attributeValue);
                    TInt  freq;
                      
                    lex.Val(freq);
                    aFreq = freq;
                    }
                    break;                    
                }
            }
            
        // Otherwise there are no more attributes.
        else
            {
            done = ETrue;
            }
        }
    }


    
