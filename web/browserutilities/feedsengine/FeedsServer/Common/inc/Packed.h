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


#ifndef PACKED_H
#define PACKED_H


// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CFeedsServerSession;


/**
*  A base class to serialize objects between clients and the server.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class CPacked: public CBase
    {
    public: // Destructor
        /**
        * Destructor.
        */        
        virtual ~CPacked();
        
        
    public: // New Methods
        /**
        * Insert an attribute.  This attribute should be applied to
        * the enclosing entity (Feed, Item, or Enclosure).
        *
        * @since 3.1
        * @param aAttribute The attribute's id.
        * @param aValue The attribute's value.
        * @return void.
        */
        void AddAttributeL(TInt aAttribute, const TDesC& aValue);

        /**
        * Insert an attribute.  This attribute should be applied to
        * the enclosing entity (Feed, Item, or Enclosure).
        *
        * @since 3.1
        * @param aAttribute The attribute's id.
        * @param aValue The attribute's value.
        * @return void.
        */
        void AddAttributeL(TInt aAttribute, TInt aValue);

        /**
        * Append a chunk of "token" values to the feed.
        *
        * @since 3.1
        * @param aChunk A token chunk.
        * @return void.
        */
        void AppendTokenChunkL(const TDesC8& aChunk);
        
        /**
        * Append a chunk of "string-table" values to the feed.
        *
        * @since 3.1
        * @param aChunk A string chunk.
        * @return void.
        */
        void AppendStringTableChunkL(const TDesC8& aChunk);
    
        /**
        * Removes any temp memory used while appending chunks.
        *
        * @since 3.1
        * @return void.
        */
        void Trim();
    
        /**
        * Determine if there is a next token.
        *
        * @since 3.1
        * @return ETrue if there is another token.
        */
        TBool HasNextToken() const;
        
        /**
        * Returns the next token.
        *
        * @since 3.1
        * @return The next token.
        */
        TUint NextToken() const;
        
        /**
        * Resets back to the first token.
        *
        * @since 3.1
        * @return The next token.
        */
        void Reset() const;
        
        /**
        * Upon completion aValue wraps the current attribute value.
        * This method must only be called when the call to NextToken
        * returns EPackedTokenAttribute.
        *
        * @since 3.1
        * @param aAttribute The attribute.
        * @param aValue The attribute value.
        * @return void.
        */
        void ExtractAttributeValue(TUint& aAttribute, TPtrC& aValue) const;
        
        
    protected:  // New Methods
        /**
        * C++ default constructor.
        */
        CPacked(TInt aTokenArrayInc, TInt aStringTableInc);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
    
        /**
        * Insert a token.
        *
        * @since 3.1
        * @param aToken The token.
        * @return void.
        */
        void InsertTokenL(TUint aToken);

        
    protected:  // Data
        const TInt KTokenArrayInc;
        const TInt KStringTableInc;
        
        TUint*     iTokenArray;
        TInt       iTokenArrayLength;
        TInt       iTokenArrayMaxLength;
        TPtrC8     iTokenArrayAsDes;

        HBufC*     iStringTable;
        
        TInt       iNextToken;
        
        
    public:  // Friends
        friend class CFeedsServerSession;
    };


#endif      // PACKED_H
            
// End of File