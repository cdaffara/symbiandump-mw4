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

#ifndef WMLDISPATCHER_H
#define WMLDISPATCHER_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include "BrCtlDefs.h"

#include "BrCtl.h"
#include "SelfDownloadContentHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CBrCtl;
class CTempFile;
class CWmlContentInterface;
class WmlResourceLoaderClient;

namespace WebCore {
    class Frame;
}

 
// CLASS DECLARATION



/**
*
*  @lib resLoader.lib
*  @since 3.0
*/
NONSHARABLE_CLASS(CWmlDispatcher) : public CBase//, public MContentListener
    {
    public:  // Constructors and destructor        
        
        /**
        * Two-phased constructor.
        */
        static CWmlDispatcher* NewL(CBrCtl* brCtl, WebCore::Frame* frame);  
        
        /**
        * Destructor.
        */
        virtual ~CWmlDispatcher( );

	public: 

        /**
        *
        * @since 3.1
        * @param
        * @return KErrNotSupported if it is not a native browser content
        */
     TInt HeadersL( TInt aTransactionId, WebCore::ResourceResponse& response/*, CUrlResponseInfo& aResponse*/ );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ResponseL(  const char* data, int length /*TInt aTransactionId, CUrlResponseInfo& aResponse*/ );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void CompleteL( TInt aTransactionId/*, CUrlResponseInfo& aResponse*/, TInt aError );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void HandleError( TInt aTransactionId, TInt aError );
        void MakeWmlVisible(bool visisble);    
        void WmlParameters( const char* data, int length, TDesC& contentType, TDesC& charset, TInt httpStatus, TInt chunkIndex, TDesC& url  );    
        void loadResource(TDesC8& url,  CWmlContentInterface* content );        
    
    private:

        /**
        * C++ default constructor.
        */
        CWmlDispatcher(CBrCtl* aBrCtl, WebCore::Frame* frame);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        void LoadFileL(const TDesC& aFileName, const TDesC& aUrl, const TDesC& aContType);
		      void LoadUrlL( const TDesC& aFileName);
		      void AddUrlToFileL(const TDesC& aFileName, const TDesC& aUrl, const TDesC& aContType);
        


    private:       
        
        CBrCtl*    m_brCtl;
        CTempFile* m_contentFile;	          //owned
        HBufC*     m_contentType;           // owned
        HBufC*     m_charset;               // owned
        HBufC*     m_url;                   // owned
        
        TInt	   m_error;
        TInt       m_restLength;
        TBool      m_loading; 
        void*      m_loadContext;
        void*      m_partialCallback;
    
    private: //from WmlStreamLoaderClient
        
        WebCore::Frame* m_frame;
        WebCore::ResourceRequest* m_request;    
    };

#endif      // WMLDISPATCHER_H            
// End of File
