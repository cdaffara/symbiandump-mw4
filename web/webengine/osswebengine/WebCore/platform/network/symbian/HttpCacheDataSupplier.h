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
* Description:  
*
*/


#ifndef CHTTPCACHEDATASUPPLIER_H
#define CHTTPCACHEDATASUPPLIER_H

//  INCLUDES
#include <http/mhttpdatasupplier.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Data supplier to supply response body if the url is loaded
 *  from cache
 *
 *  @lib 
 *  @since 3.1
 */
class CHttpCacheDataSupplier : public CBase, public MHTTPDataSupplier
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpCacheDataSupplier* NewL( HBufC8* aBody );
        
        /**
        * Destructor.
        */
        virtual ~CHttpCacheDataSupplier();      
     
    public: // from MHTTPDataSupplier
        
        /**
        * From MHTTPDataSupplier. Free the data chunk that was requested.
        * @since 2.0
        */
        virtual void ReleaseData() { delete m_body; m_body = NULL;}
        
        /**
        * From MHTTPDataSupplier. It is not possible to know
        * the data size, so return KErrNotFound
        * @since 2.0
        */
        virtual TInt OverallDataSize () { return KErrNotFound; }
        
        /**
        * From MHTTPDataSupplier. Get the next data part of the response body
        * @since 2.0
        */
        virtual TBool GetNextDataPart( TPtrC8& aDataPart );
        
        /**
        * From MHTTPDataSupplier. Not Supported
        * @since 2.0
        */
        virtual TInt Reset() { return KErrNotFound; }
        
    private:
        
        /**
        * C++ default constructor.
        */
        CHttpCacheDataSupplier( HBufC8* aBody );
        
    private:    // Data
        
        //owned by datasupplier and is destroyed 
        //when the releasedata function is called
        HBufC8* m_body;        
    };

#endif      // CHTTPCACHEDATASUPPLIER_H

// End of File
