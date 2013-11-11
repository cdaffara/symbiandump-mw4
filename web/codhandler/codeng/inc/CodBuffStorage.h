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
* Description:  Implements double buffering storage functionality for OMA downloads
*
*/



#ifndef COD_BUFFSTORAGE_H
#define COD_BUFFSTORAGE_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CFileSaver;


// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/


NONSHARABLE_CLASS( CCodBuffStorage ) : public CActive
    {
    public:  // Constructors and destructor
    
            
    /**
     * Two-phased constructor.
     */
    static CCodBuffStorage* NewL( CFileSaver* aStorage );
    
   /**
    * Destructor.
    */
    ~CCodBuffStorage();
    
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
    //void DoNonbufferingWriteL(const TDesC8& aBuf);
    
    private:              
    /**
     * C++ default constructor.
     */
    CCodBuffStorage(CFileSaver*);
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    public:     // Data


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
    
    // References to CFileSaver data
    RFile* iFile;
    TInt& iDownloadedSize; // How much data actually written to finally
    TInt32& iBufferedSize;   // How much data received over the air (but not necessarily all in file yet)
    
    TInt32& iHttpStorageBufferSize; //Storage buffer size 128kb(Non-progressive)/16kb(Progressive)
    TInt32& iHttpStorageLength;     //Content Length
    TBool& iProgressiveDownload;    //Flag to indicate Progressive play
    };
    
#endif      // COD_BUFFSTORAGE_H   
            
// End of File
