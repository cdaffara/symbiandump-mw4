/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of HttpPostDataSupplier 
*
*/

#ifndef HTTPPOSTDATASUPPLIER_H
#define HTTPPOSTDATASUPPLIER_H

//  INCLUDES
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <http/rhttptransaction.h>
#include <wtf/Vector.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
namespace WebCore {
    class FormData;
};

class PostDataItem;
class CBrCtl;

// CLASS DECLARATION

/**
*  Data supplier for POST request body.
*
*  @lib resloader.lib
*  @since 3.1
*/
class HttpPostDataSupplier :public MHTTPDataSupplier
    {
    public:  // Constructors 

        /**
        * Construct.
        * @since 3.1
        * @param aData Post body.
        * @param aDataLen Post body length.
        * @return
        */
        HttpPostDataSupplier(RHTTPTransaction* transaction, CBrCtl* brctl);

        void initL(WebCore::FormData* formData);
        
        //destructor
        ~HttpPostDataSupplier();
        
    public: // Functions from base classes
        
        /**
        * Release data.
        * @since 3.1
        * @param 
        * @return void
        */
        void ReleaseData();
        
        /**
        * Get body length.
        * @since 3.1
        * @param 
        * @return The length of the body.
        */
        TInt OverallDataSize();
        
        /**
        * Get the body in one chunk.
        * @since 3.1
        * @param aDataPart A pointer to be set to the data body and length.
        * @return ETrue
        */
        TBool GetNextDataPart( TPtrC8& aDataPart );        
        
        /**
        * Since we don't stream requests, there is nothing to reset.
        * @since 3.1
        * @param 
        * @return KErrNone
        */
        TInt Reset();
    private:
        /**
        * Get the body in one chunk.
        * @since 3.1
        * @param aDataPart A pointer to be set to the data body and length.
        * @return ETrue
        */
        void getNextDataPartL( TPtrC8& aDataPart );        

        
    private:    // Data
        
		RHTTPTransaction* m_transaction; // not owned
        Vector<PostDataItem*> m_postData; // owned
        HBufC8* m_dataPart;// owned , part data
        int m_totalSize;// total size of the post data
        int m_postItemIndex;// post data items track
        int m_refCount;
        bool m_lastPart;
        CBrCtl* m_brctl;
        bool m_firstPart;
    };

#endif      // HTTPPOSTDATASUPPLIER_H

// End of File
