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
* Description:  A base class to serialize objects between clients and the server.
*
*/


#include "Logger.h"
#include "Packed.h"
#include "PackedAttributes.h"


// -----------------------------------------------------------------------------
// CPacked::CPacked
//
// C++ default constructor can NOT contain any code, that
// might leave.  
// -----------------------------------------------------------------------------
//
CPacked::CPacked(TInt aTokenArrayInc, TInt aStringTableInc):
        KTokenArrayInc(aTokenArrayInc), KStringTableInc(aStringTableInc)
    {
    }
        

// -----------------------------------------------------------------------------
// CPacked::ConstructL
//
// Symbian 2nd phase constructor can leave.  
// -----------------------------------------------------------------------------
//
void CPacked::BaseConstructL()
    {
    iStringTable = HBufC::NewL(KStringTableInc);
    }
        

// -----------------------------------------------------------------------------
// CPacked::~CPacked
//
// Deconstructor.  
// -----------------------------------------------------------------------------
//
CPacked::~CPacked()
    {
    delete iTokenArray;
    delete iStringTable;
    }

        
// -----------------------------------------------------------------------------
// CPacked::AddAttributeL
//
// Insert an attribute.  This attribute is applied to the enclosing entity 
// (Feed, Item, Enclosure, folder, etc.).   
// -----------------------------------------------------------------------------
//
void CPacked::AddAttributeL(TInt aAttribute, const TDesC& aValue)
    {
    TUint  offset;
    
    // Get the offset for the string.
    offset = iStringTable->Length();
        
    // If need be re-alloc the string table.
    if ((iStringTable->Length() + aValue.Length()) >= iStringTable->Des().MaxLength())
        {
        TUint delta = KStringTableInc;
        
        if (aValue.Length() > KStringTableInc)
            {
            delta = aValue.Length() + KStringTableInc;
            }
            
        iStringTable = iStringTable->ReAllocL(iStringTable->Des().MaxLength() + delta);
        }

    // Add the value to the string table.
    iStringTable->Des().Append(aValue);
    
    // Add the attribute token block.
    InsertTokenL(EPackedTokenAttribute);
    InsertTokenL(aAttribute);
    InsertTokenL(offset);
    InsertTokenL(aValue.Length());
    }


// -----------------------------------------------------------------------------
// CPacked::AddAttributeL
//
// Insert an attribute.  This attribute is applied to the enclosing entity 
// (Feed, Item, Enclosure, folder, etc.).   
// -----------------------------------------------------------------------------
//
void CPacked::AddAttributeL(TInt aAttribute, TInt aValue)
    {
    TBuf<25> buff;
    
    buff.AppendNum(aValue);
    AddAttributeL(aAttribute, buff);
    }
    
    
// -----------------------------------------------------------------------------
// CPacked::InsertTokenL
//
// Insert a token.   
// -----------------------------------------------------------------------------
//
void CPacked::InsertTokenL(TUint aToken)
    {
    // If need be re-alloc the token array.
    if (iTokenArrayLength == iTokenArrayMaxLength)
        {
        iTokenArray = static_cast<TUint*>(User::ReAllocL(iTokenArray, sizeof(TUint) * 
                (iTokenArrayMaxLength + KTokenArrayInc)));
                
        iTokenArrayMaxLength += KTokenArrayInc;
        }
        
    // Append the token.
    iTokenArray[iTokenArrayLength++] = aToken;
    }


// -----------------------------------------------------------------------------
// CPacked::AppendTokenChunkL
//
// Append a chunk of "token" values to the feed.   
// -----------------------------------------------------------------------------
//
void CPacked::AppendTokenChunkL(const TDesC8& aChunk)
    {
    if (aChunk.Length() == 0)
        {
        return;
        }
        
    TInt          tokenLength = aChunk.Size() / sizeof(TUint);
    const TUint*  tokens = (const TUint*) aChunk.Ptr();
    
    // If need be re-alloc the token array.
    if ((iTokenArrayLength + tokenLength) >= iTokenArrayMaxLength)
        {
        TInt inc = tokenLength; 
        
        if (KTokenArrayInc > tokenLength)
            {
            inc = KTokenArrayInc;
            }
            
        iTokenArray = static_cast<TUint*>(User::ReAllocL(iTokenArray, sizeof(TUint) * 
                (iTokenArrayMaxLength + inc)));
                
        iTokenArrayMaxLength += inc;
        }
        
    // Append the tokens.
    for (TInt i = 0; i < tokenLength; i++)
        {
        iTokenArray[iTokenArrayLength++] = tokens[i];
        }
    }
        

// -----------------------------------------------------------------------------
// CPacked::AppendStringTableChunkL
//
// Append a chunk of "string-table" values to the feed.   
// -----------------------------------------------------------------------------
//
void CPacked::AppendStringTableChunkL(const TDesC8& aChunk)
    {
    if (aChunk.Length() == 0)
        {
        return;
        }
        
    TPtrC buf((const TUint16 *) aChunk.Ptr(), aChunk.Size() / sizeof(TUint16));
    
    // If need be re-alloc the string table.
    if ((iStringTable->Length() + buf.Length()) >= iStringTable->Des().MaxLength())
        {
        TInt inc = buf.Length(); 
        
        if (KStringTableInc > buf.Length())
            {
            inc = KStringTableInc;
            }

        iStringTable = iStringTable->ReAllocL(iStringTable->Des().MaxLength() + 
                inc);
        }

    // Add the value to the string table.
    iStringTable->Des().Append(buf);    
    }
    

// -----------------------------------------------------------------------------
// CPacked::Trim
//
// Removes any temp memory used while appending chunks.   
// -----------------------------------------------------------------------------
//
void CPacked::Trim()
    {
    //  Trim the string table.
    if (iStringTable->Length() < iStringTable->Des().MaxLength())
        {
        TRAP_IGNORE(iStringTable = iStringTable->ReAllocL(iStringTable->Length()));
        }

    //  Trim the string table.
    if (iTokenArrayLength < iTokenArrayMaxLength)
        {
        TRAP_IGNORE(iTokenArray = static_cast<TUint*>(User::ReAllocL(iTokenArray, 
                sizeof(TUint) * iTokenArrayLength)));
        }
    }
    
    
// -----------------------------------------------------------------------------
// CPacked::HasNextToken
//
// Determine if there is a next token.   
// -----------------------------------------------------------------------------
//
TBool CPacked::HasNextToken() const
    {
    return (iNextToken < iTokenArrayLength);
    }
    
    
// -----------------------------------------------------------------------------
// CPacked::NextToken
//
// Returns the next token.   
// -----------------------------------------------------------------------------
//
TUint CPacked::NextToken() const
    {
    // TODO: Panic if iNextToken >= iTokenArrayLength.
    
    return iTokenArray[const_cast<CPacked*>(this)->iNextToken++];
    }
    

// -----------------------------------------------------------------------------
// CPacked::NextToken
//
// Resets back to the first token.   
// -----------------------------------------------------------------------------
//
void CPacked::Reset() const
    {
    const_cast<CPacked*>(this)->iNextToken = 0;
    }


// -----------------------------------------------------------------------------
// CPacked::ExtractAttributeValue
//
// Upon completion aValue wraps the current attribute value.
// This method must only be called when the call to NextToken
// returns EPackedTokenAttribute.   
// -----------------------------------------------------------------------------
//
void CPacked::ExtractAttributeValue(TUint& aAttribute, TPtrC& aValue) const
    {
    TUint  offset;
    TUint  length;
    
    // TODO: Panic if iTokenArray[iNextToken] != EPackedTokenAttribute.
    
    aAttribute = NextToken();
    offset = NextToken();
    length = NextToken();
    
    if(iStringTable->Length() >= (length + offset))
    	{
    	aValue.Set(iStringTable->Mid(offset, length));
    	}
    else
    	{
    	aValue.Set(KNullDesC);	
    	}
    
    }
