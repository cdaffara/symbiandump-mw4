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
* Description:  Implements storage functionality in the DownloadManager
*
*/



#ifndef BUFFSTORAGE_H
#define BUFFSTORAGE_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CHttpDownload;
class CHttpStorage;
class RFotaEngineSession;
class RWriteStream;
class CDocumentHandler;
class CBuffStorage;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/


NONSHARABLE_CLASS( CBuffStorage ) : public CActive
    {
    public:  // Constructors and destructor
    
            
    /**
     * Two-phased constructor.
     */
    static CBuffStorage* NewL( CHttpStorage* aStorage );
    
   /**
    * Destructor.
    */
    ~CBuffStorage();
    
    public: // Interface
    
    /**
    * Delets the storage buffers.
    * @param 
    * @return
    */
    void ResetBuffers();
    
	/**
    * Persist received chunk
    * @since Series 60 v2.8
    * @param aBuf new chunk to be added
    * @return EFalse if content-length had to be updated.
    */
    TBool WriteOutNextBodyDataL( const TDesC8& aBuf );
    
    /**
    * If buffering enabled, flush data to disk
    */
    void FlushBuffersL();
    
    /**
    * Returns buffer size
    * @since Series 60 v2.8
    * @param 
    * @return  Buffer size.
    */
    const TInt& CurrentBufferSize() { return iBufferSize; }
    
    /**
    * Clear the error encountered during writing to file
    * @since Series 60 v2.8
    * @param 
    * @return  Buffer size.
    */
    void ClearErrors() { iLastWriteErrorCode = KErrNone; }
        

    protected:  // Functions from base classes

    
    private: // From CActive

    /**
    * Cancels the timer.
    * @param 
    * @return
    */
    void DoCancel();
    
    /**
    * Handles an active object’s request completion event.
    * @param 
    * @return
    */
    void RunL();
    
    /**
    * Persist received chunk
    * @since Series 60 v2.8
    * @param aBuf new chunk to be added
    * @return EFalse if content-length had to be updated.
    */
    void DoBufferingWriteL(const TDesC8& aBuf);
    void DoNonbufferingWriteL(const TDesC8& aBuf);
    
    private:              
    /**
     * C++ default constructor.
     */
    CBuffStorage(CHttpStorage*);
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
	
	public:     // Data
	
	friend class CBuffStorage;
    
    
    protected:  // Data


    private:
    
    // Double-buffering
    HBufC8* iBuff1, *iBuff2;
    HBufC8* iClientBuffer;
    TPtr8 *iWritePtr;
    TInt iBufferSize;
    
    // For stalling condition handling
    TInt iLastWriteErrorCode;
    CActiveSchedulerWait iWait;
    
    // References to CHttpStorage data
    RFile*& iFile;
    TInt32& iDownloadedSize; // How much data actually written to finally
    TInt32& iBufferedSize;   // How much data received over the air (but not necessarily all in file yet)
    
    TInt& iHttpStorageBufferSize;
    TInt32& iHttpStorageLength;
    TBool& iBufferingEnabled;
    TBool& iProgressiveDownload;
    
#ifdef __SYNCML_DM_FOTA
    RWriteStream*& iFotaStream;
#endif // __SYNCML_DM_FOTA

    };
    


    
#endif      // BUFFSTORAGE_H   
            
// End of File
