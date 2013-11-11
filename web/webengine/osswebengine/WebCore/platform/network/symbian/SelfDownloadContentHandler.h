/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SELFDOWNLOADCONTENTHANDLER_H
#define SELFDOWNLOADCONTENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <apparc.h>
#include <http/rhttptransaction.h>
#include <http/mhttptransactioncallback.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CDocumentHandler;
class TDataType;
class MBrCtlSpecialLoadObserver;
class WebFrame;
namespace WebCore {
    class ResourceHandle;
    struct ResourceRequest;
    class ResourceResponse;
}

// CLASS DECLARATION

/**
*
*  @lib 
*  @since 3.0
*/
class CTempFile : public CBase
    {
    public:   // Constructors and destructor

        /**
          * Two-phased constructor.
          */
        static CTempFile* NewL(HBufC* aFileName = NULL);

        /**
        * Destructor.
        */
        ~CTempFile( void );

    public:  // New functions

        /**
        *
        * @since 3.0
        * @param
        * @return number of bytes written
        */
        TInt WriteL( const TDesC8& aFileData );

        /**
        *
        * @since 3.0
        * @param
        * @return number of bytes written
        */
        void CloseL( void );

        /**
        *
        * @since 3.0
        * @param
        * @return void
        */
        const TDesC& Name( void ) const;

    private:

        /**
        * C++ default constructor.
        */
        CTempFile( void );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(HBufC* aFileName);

    public:      // Data

    protected:   // Data

    private:     // Data

        HBufC* iFileName;

        RFs iFsSession;
        
        RFile iFile;
       
    };

/**
*
*  @since 5.0
*/
class SelfDownloadContentHandler : public MHTTPTransactionCallback
    {
    public:  // Constructors and destructor        
        
        /**
        * Two-phased constructor.
        */
        static SelfDownloadContentHandler* NewL(WebFrame* webFrame, TDesC& selfDownloadTypes);
        
        /**
        * Destructor.
        */
        virtual ~SelfDownloadContentHandler();

    public: // from MHTTPTransactionCallback

        void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent &aEvent);

        TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent &aEvent);

    public: // New functions
    
        TInt IsSupported(const WebCore::ResourceRequest& request,
            const WebCore::ResourceResponse& response, RHTTPTransaction& httpTransaction);

        TInt HandleResponseHeadersL(const WebCore::ResourceRequest& request,
            const WebCore::ResourceResponse& response, RHTTPTransaction& httpTransaction);

        TInt HandleResponseBodyL(RHTTPTransaction& httpTransaction);

        TInt ResponseCompleteL(RHTTPTransaction httpTransaction, TInt error);
        //This method reinitialize the special loader reference this static class.
        void ReinitializeSpecialLoadObserver(WebFrame* webFrame);

    private:

        /**
        * C++ default constructor.
        */
        SelfDownloadContentHandler(WebFrame* webFrame);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(WebFrame* webFrame, TDesC& selfDownloadTypes);

        void HandleError(/* TInt aTransactionId,*/ TInt aError);

        TBool IsSelfDownloadContent(TDesC& contentType);

//        TBool RecognizeContentTypeL(TPtr aContentType);

    private:    // Data

        WebFrame* m_WebFrame;
        //
        // file containing the response content - this should be deleted
        // and a new one created for each response.
        CTempFile*      m_ContentFile;                      // owned
        //
        HBufC*          m_ContentType;                      // owned
        //
        HBufC*          m_Charset;                          // owned
        //
        HBufC*          m_ReqUrl;                           // owned
        //
        MBrCtlSpecialLoadObserver* m_SpecialLoadObserver;   // not owned
        //
        HBufC*          m_SelfDlMimeTypes;                  // owned
    };

#endif      // SELFDOWNLOADCONTENTHANDLER_H
            
// End of File
